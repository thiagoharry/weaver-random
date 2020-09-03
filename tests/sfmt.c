// Reference implementation of Mersenne Twister

#define SFMT_POS1 122
#define SFMT_SL1 18
#define SFMT_SL2 1
#define SFMT_SR1 11
#define SFMT_SR2 1
#define SFMT_MSK1 0xdfffffefU
#define SFMT_MSK2 0xddfecb7fU
#define SFMT_MSK3 0xbffaffffU
#define SFMT_MSK4 0xbffffff6U
#define SFMT_PARITY1    0x00000001U
#define SFMT_PARITY2    0x00000000U
#define SFMT_PARITY3    0x00000000U
#define SFMT_PARITY4    0xc98e126aU


#if !defined(SFMT_MEXP)
#if defined(__GNUC__) && !defined(__ICC)
//#warning "SFMT_MEXP is not defined. I assume MEXP is 19937."
#endif
  #define SFMT_MEXP 19937
#endif

/*-----------------
  BASIC DEFINITIONS
  -----------------*/
/** Mersenne Exponent. The period of the sequence
 *  is a multiple of 2^MEXP-1.
 * #define SFMT_MEXP 19937 */
/** SFMT generator has an internal state array of 128-bit integers,
 * and N is its size. */
#define SFMT_N (SFMT_MEXP / 128 + 1)
/** N32 is the size of internal state array when regarded as an array
 * of 32-bit integers.*/
#define SFMT_N32 (SFMT_N * 4)
/** N64 is the size of internal state array when regarded as an array
 * of 64-bit integers.*/
#define SFMT_N64 (SFMT_N * 2)


/*------------------------------------------
  128-bit SIMD like data type for standard C
  ------------------------------------------*/
#if defined(HAVE_ALTIVEC)
  #if !defined(__APPLE__)
    #include <altivec.h>
  #endif
/** 128-bit data structure */
union W128_T {
    vector unsigned int s;
    uint32_t u[4];
    uint64_t u64[2];
};
#elif defined(HAVE_NEON)
  #include <arm_neon.h>

/** 128-bit data structure */
union W128_T {
    uint32_t u[4];
    uint64_t u64[2];
    uint32x4_t si;
};
#elif defined(HAVE_SSE2)
  #include <emmintrin.h>

/** 128-bit data structure */
union W128_T {
    uint32_t u[4];
    uint64_t u64[2];
    __m128i si;
};
#else
/** 128-bit data structure */
union W128_T {
    uint32_t u[4];
    uint64_t u64[2];
};
#endif

/** 128-bit data type */
typedef union W128_T w128_t;


/**
 * SFMT internal state
 */
struct SFMT_T {
    /** the 128-bit internal state array */
    w128_t state[SFMT_N];
    /** index counter to the 32-bit internal state array */
    int idx;
};


typedef struct SFMT_T sfmt_t;

/**
 * This function simulates SIMD 128-bit right shift by the standard C.
 * The 128-bit integer given in in is shifted by (shift * 8) bits.
 * This function simulates the LITTLE ENDIAN SIMD.
 * @param out the output of this function
 * @param in the 128-bit data to be shifted
 * @param shift the shift value
 */
#ifdef ONLY64
inline static void rshift128(w128_t *out, w128_t const *in, int shift) {
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in->u[2] << 32) | ((uint64_t)in->u[3]);
    tl = ((uint64_t)in->u[0] << 32) | ((uint64_t)in->u[1]);

    oh = th >> (shift * 8);
    ol = tl >> (shift * 8);
    ol |= th << (64 - shift * 8);
    out->u[0] = (uint32_t)(ol >> 32);
    out->u[1] = (uint32_t)ol;
    out->u[2] = (uint32_t)(oh >> 32);
    out->u[3] = (uint32_t)oh;
}
#else
inline static void rshift128(w128_t *out, w128_t const *in, int shift)
{
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in->u[3] << 32) | ((uint64_t)in->u[2]);
    tl = ((uint64_t)in->u[1] << 32) | ((uint64_t)in->u[0]);

    oh = th >> (shift * 8);
    ol = tl >> (shift * 8);
    ol |= th << (64 - shift * 8);
    out->u[1] = (uint32_t)(ol >> 32);
    out->u[0] = (uint32_t)ol;
    out->u[3] = (uint32_t)(oh >> 32);
    out->u[2] = (uint32_t)oh;
}
#endif

#ifdef ONLY64
inline static void lshift128(w128_t *out, w128_t const *in, int shift) {
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in->u[2] << 32) | ((uint64_t)in->u[3]);
    tl = ((uint64_t)in->u[0] << 32) | ((uint64_t)in->u[1]);

    oh = th << (shift * 8);
    ol = tl << (shift * 8);
    oh |= tl >> (64 - shift * 8);
    out->u[0] = (uint32_t)(ol >> 32);
    out->u[1] = (uint32_t)ol;
    out->u[2] = (uint32_t)(oh >> 32);
    out->u[3] = (uint32_t)oh;
}
#else
inline static void lshift128(w128_t *out, w128_t const *in, int shift)
{
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in->u[3] << 32) | ((uint64_t)in->u[2]);
    tl = ((uint64_t)in->u[1] << 32) | ((uint64_t)in->u[0]);

    oh = th << (shift * 8);
    ol = tl << (shift * 8);
    oh |= tl >> (64 - shift * 8);
    out->u[1] = (uint32_t)(ol >> 32);
    out->u[0] = (uint32_t)ol;
    out->u[3] = (uint32_t)(oh >> 32);
    out->u[2] = (uint32_t)oh;
}
#endif

/**
 * This function represents the recursion formula.
 * @param r output
 * @param a a 128-bit part of the internal state array
 * @param b a 128-bit part of the internal state array
 * @param c a 128-bit part of the internal state array
 * @param d a 128-bit part of the internal state array
 */
#ifdef ONLY64
inline static void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *c,
                                w128_t *d) {
    w128_t x;
    w128_t y;

    lshift128(&x, a, SFMT_SL2);
    rshift128(&y, c, SFMT_SR2);
    r->u[0] = a->u[0] ^ x.u[0] ^ ((b->u[0] >> SFMT_SR1) & SFMT_MSK2) ^ y.u[0]
        ^ (d->u[0] << SFMT_SL1);
    r->u[1] = a->u[1] ^ x.u[1] ^ ((b->u[1] >> SFMT_SR1) & SFMT_MSK1) ^ y.u[1]
        ^ (d->u[1] << SFMT_SL1);
    r->u[2] = a->u[2] ^ x.u[2] ^ ((b->u[2] >> SFMT_SR1) & SFMT_MSK4) ^ y.u[2]
        ^ (d->u[2] << SFMT_SL1);
    r->u[3] = a->u[3] ^ x.u[3] ^ ((b->u[3] >> SFMT_SR1) & SFMT_MSK3) ^ y.u[3]
        ^ (d->u[3] << SFMT_SL1);
}
#else
inline static void do_recursion(w128_t *r, w128_t *a, w128_t *b,
                                w128_t *c, w128_t *d)
{
    w128_t x;
    w128_t y;
    lshift128(&x, a, SFMT_SL2);
    rshift128(&y, c, SFMT_SR2);
    r->u[0] = a->u[0] ^ x.u[0] ^ ((b->u[0] >> SFMT_SR1) & SFMT_MSK1)
        ^ y.u[0] ^ (d->u[0] << SFMT_SL1);
    r->u[1] = a->u[1] ^ x.u[1] ^ ((b->u[1] >> SFMT_SR1) & SFMT_MSK2)
        ^ y.u[1] ^ (d->u[1] << SFMT_SL1);
    r->u[2] = a->u[2] ^ x.u[2] ^ ((b->u[2] >> SFMT_SR1) & SFMT_MSK3)
        ^ y.u[2] ^ (d->u[2] << SFMT_SL1);
    r->u[3] = a->u[3] ^ x.u[3] ^ ((b->u[3] >> SFMT_SR1) & SFMT_MSK4)
        ^ y.u[3] ^ (d->u[3] << SFMT_SL1);
}
#endif



#if !defined(HAVE_SSE2) && !defined(HAVE_ALTIVEC) && !defined(HAVE_NEON)
/**
 * This function fills the internal state array with pseudorandom
 * integers.
 * @param sfmt SFMT internal state
 */
void sfmt_gen_rand_all(sfmt_t * sfmt) {
    int i;
    w128_t *r1, *r2;

    r1 = &sfmt->state[SFMT_N - 2];
    r2 = &sfmt->state[SFMT_N - 1];
    for (i = 0; i < SFMT_N - SFMT_POS1; i++) {
      //printf("(%d %d %d %d)\n", i, i + SFMT_POS1, SFMT_N - 2, SFMT_N - 1);
        do_recursion(&sfmt->state[i], &sfmt->state[i],
                     &sfmt->state[i + SFMT_POS1], r1, r2);
        r1 = r2;
        r2 = &sfmt->state[i];
    }
    for (; i < SFMT_N; i++) {
      //printf("(%d %d %d %d)\n", i, i + SFMT_POS1 - SFMT_N, SFMT_N - 2, SFMT_N - 1);
        do_recursion(&sfmt->state[i], &sfmt->state[i],
                     &sfmt->state[i + SFMT_POS1 - SFMT_N], r1, r2);
        r1 = r2;
        r2 = &sfmt->state[i];
    }
}
#endif


/**
 * This function generates and returns 64-bit pseudorandom number.
 * init_gen_rand or init_by_array must be called before this function.
 * The function gen_rand64 should not be called after gen_rand32,
 * unless an initialization is again executed.
 * @param sfmt SFMT internal state
 * @return 64-bit pseudorandom number
 */
inline static uint64_t sfmt_genrand_uint64(sfmt_t * sfmt) {
#if defined(BIG_ENDIAN64) && !defined(ONLY64)
    uint32_t * psfmt32 = &sfmt->state[0].u[0];
    uint32_t r1, r2;
#else
    uint64_t r;
#endif
    uint64_t * psfmt64 = &sfmt->state[0].u64[0];
//    assert(sfmt->idx % 2 == 0);

    if (sfmt->idx >= SFMT_N32) {
        sfmt_gen_rand_all(sfmt);
        sfmt->idx = 0;
    }
#if defined(BIG_ENDIAN64) && !defined(ONLY64)
    r1 = psfmt32[sfmt->idx];
    r2 = psfmt32[sfmt->idx + 1];
    sfmt->idx += 2;
    return ((uint64_t)r2 << 32) | r1;
#else
    r = psfmt64[sfmt->idx / 2];
    sfmt->idx += 2;
    return r;
#endif
}

/**
 * This function represents a function used in the initialization
 * by init_by_array
 * @param x 32-bit integer
 * @return 32-bit integer
 */
static uint32_t func1(uint32_t x) {
    return (x ^ (x >> 27)) * (uint32_t)1664525UL;
}

/**
 * This function represents a function used in the initialization
 * by init_by_array
 * @param x 32-bit integer
 * @return 32-bit integer
 */
static uint32_t func2(uint32_t x) {
    return (x ^ (x >> 27)) * (uint32_t)1566083941UL;
}

/**
 * This function simulate a 64-bit index of LITTLE ENDIAN
 * in BIG ENDIAN machine.
 */
#ifdef ONLY64
inline static int idxof(int i) {
    return i ^ 1;
}
#else
inline static int idxof(int i) {
    return i;
}
#endif

/**
 * This function certificate the period of 2^{MEXP}
 * @param sfmt SFMT internal state
 */
static void period_certification(sfmt_t * sfmt) {
    uint32_t inner = 0;
    int i, j;
    uint32_t work;
    uint32_t *psfmt32 = &sfmt->state[0].u[0];
    const uint32_t parity[4] = {SFMT_PARITY1, SFMT_PARITY2,
                                SFMT_PARITY3, SFMT_PARITY4};

    for (i = 0; i < 4; i++) {
        inner ^= psfmt32[idxof(i)] & parity[i];
    }
    for (i = 16; i > 0; i >>= 1) {
        inner ^= inner >> i;
    }
    inner &= 1;
    /* check OK */
    if (inner == 1) {
        return;
    }
    /* check NG, and modification */
    for (i = 0; i < 4; i++) {
        work = 1;
        for (j = 0; j < 32; j++) {
            if ((work & parity[i]) != 0) {
                psfmt32[idxof(i)] ^= work;
                return;
            }
            work = work << 1;
        }
    }
}


/**
 * This function initializes the internal state array,
 * with an array of 32-bit integers used as the seeds
 * @param sfmt SFMT internal state
 * @param init_key the array of 32-bit integers, used as a seed.
 * @param key_length the length of init_key.
 */
void sfmt_init_by_array(sfmt_t * sfmt, uint32_t *init_key, int key_length) {
    int i, j, count;
    uint32_t r;
    int lag;
    int mid;
    int size = SFMT_N * 4;
    uint32_t *psfmt32 = &sfmt->state[0].u[0];

    if (size >= 623) {
        lag = 11;
    } else if (size >= 68) {
        lag = 7;
    } else if (size >= 39) {
        lag = 5;
    } else {
        lag = 3;
    }
    mid = (size - lag) / 2;

    memset(sfmt, 0x8b, sizeof(sfmt_t));
    if (key_length + 1 > SFMT_N32) {
        count = key_length + 1;
    } else {
        count = SFMT_N32;
    }
    r = func1(psfmt32[idxof(0)] ^ psfmt32[idxof(mid)]
              ^ psfmt32[idxof(SFMT_N32 - 1)]);
    psfmt32[idxof(mid)] += r;
    r += key_length;
    psfmt32[idxof(mid + lag)] += r;
    psfmt32[idxof(0)] = r;

    count--;
    for (i = 1, j = 0; (j < count) && (j < key_length); j++) {
        r = func1(psfmt32[idxof(i)] ^ psfmt32[idxof((i + mid) % SFMT_N32)]
                  ^ psfmt32[idxof((i + SFMT_N32 - 1) % SFMT_N32)]);
        psfmt32[idxof((i + mid) % SFMT_N32)] += r;
        r += init_key[j] + i;
        psfmt32[idxof((i + mid + lag) % SFMT_N32)] += r;
        psfmt32[idxof(i)] = r;
        i = (i + 1) % SFMT_N32;
    }
    for (; j < count; j++) {
        r = func1(psfmt32[idxof(i)] ^ psfmt32[idxof((i + mid) % SFMT_N32)]
                  ^ psfmt32[idxof((i + SFMT_N32 - 1) % SFMT_N32)]);
        psfmt32[idxof((i + mid) % SFMT_N32)] += r;
        r += i;
        psfmt32[idxof((i + mid + lag) % SFMT_N32)] += r;
        psfmt32[idxof(i)] = r;
        i = (i + 1) % SFMT_N32;
    }
    for (j = 0; j < SFMT_N32; j++) {
        r = func2(psfmt32[idxof(i)] + psfmt32[idxof((i + mid) % SFMT_N32)]
                  + psfmt32[idxof((i + SFMT_N32 - 1) % SFMT_N32)]);
        psfmt32[idxof((i + mid) % SFMT_N32)] ^= r;
        r -= i;
        psfmt32[idxof((i + mid + lag) % SFMT_N32)] ^= r;
        psfmt32[idxof(i)] = r;
        i = (i + 1) % SFMT_N32;
    }

    sfmt->idx = SFMT_N32;
    period_certification(sfmt);
}


/**
 * This function initializes the internal state array with a 32-bit
 * integer seed.
 *
 * @param sfmt SFMT internal state
 * @param seed a 32-bit integer used as the seed.
 */
void sfmt_init_gen_rand(sfmt_t * sfmt, uint32_t seed) {
    int i;

    uint32_t *psfmt32 = &sfmt->state[0].u[0];

    psfmt32[idxof(0)] = seed;
    for (i = 1; i < SFMT_N32; i++) {
        psfmt32[idxof(i)] = 1812433253UL * (psfmt32[idxof(i - 1)]
                                            ^ (psfmt32[idxof(i - 1)] >> 30))
            + i;
    }
    sfmt->idx = SFMT_N32;
    period_certification(sfmt);
}

#ifndef ONLY64
/**
 * This function generates and returns 32-bit pseudorandom number.
 * init_gen_rand or init_by_array must be called before this function.
 * @param sfmt SFMT internal state
 * @return 32-bit pseudorandom number
 */
inline static uint32_t sfmt_genrand_uint32(sfmt_t * sfmt) {
    uint32_t r;
    uint32_t * psfmt32 = &sfmt->state[0].u[0];
    //xprintf("\ngenrand\n");
    if (sfmt->idx >= SFMT_N32) {
        sfmt_gen_rand_all(sfmt);
        sfmt->idx = 0;
    }
    r = psfmt32[sfmt->idx++];
    return r;
}
#endif
