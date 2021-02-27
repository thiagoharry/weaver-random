#define PCG_128BIT_CONSTANT(high,low)		\
  ((((unsigned __int128)high) << 64) + low)

#define PCG_DEFAULT_MULTIPLIER_128 \
        PCG_128BIT_CONSTANT(2549297995355413924ULL,4865540595714422341ULL)

struct pcg64_random_t{
  unsigned __int128 state;
  unsigned __int128 inc;
};

uint64_t pcg_rotr_64(uint64_t value, unsigned int rot){
  return (value >> rot) | (value << ((- rot) & 63));
}

uint64_t pcg_output_xsl_rr_128_64(unsigned __int128 state){
  return pcg_rotr_64(((uint64_t)(state >> 64u)) ^ (uint64_t)state,
		     state >> 122u);
}

void pcg_setseq_128_step_r(struct pcg64_random_t *rng){
  rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_128 + rng->inc;
}

void pcg64_srandom_r(struct pcg64_random_t* rng,
		      unsigned __int128 initstate, unsigned __int128 initseq){
  rng->state = 0U;
  rng->inc = (initseq << 1u) | 1u;
  pcg_setseq_128_step_r(rng);
  rng->state += initstate;
  pcg_setseq_128_step_r(rng);
}

uint64_t pcg64_random_r(struct pcg64_random_t* rng){
  pcg_setseq_128_step_r(rng);
  return pcg_output_xsl_rr_128_64(rng->state);
}
