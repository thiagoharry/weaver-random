// Reference implementation of Mersenne Twister


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

void sfmt_fill_array64(sfmt_t * sfmt, uint64_t *array, int size) {
    assert(sfmt->idx == SFMT_N32);
    assert(size % 2 == 0);
    assert(size >= SFMT_N64);

    gen_rand_array(sfmt, (w128_t *)array, size / 2);
    sfmt->idx = SFMT_N32;

#if defined(BIG_ENDIAN64) && !defined(ONLY64)
    swap((w128_t *)array, size /2);
#endif
}
