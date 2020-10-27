#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>

#include "../src/random.h"

/********************* TIMER **************************************/
#ifndef __timer_h_
#define __timer_h_
#include <sys/time.h>
#include <math.h>
#define N 100000
unsigned long t_sum = 0;
unsigned long measures[N];
int _i = 0;
#define TIMER_BEGIN() { struct timeval _begin, _end;	\
  gettimeofday(&_begin, NULL);
#define TIMER_END() gettimeofday(&_end, NULL);		  \
  measures[_i] = 1000000 * (_end.tv_sec - _begin.tv_sec) +	\
    _end.tv_usec - _begin.tv_usec;				\
  t_sum += measures[_i];					\
  _i ++;}
#define TIMER_RESULT() {					\
    double mean = ((double) t_sum) / ((double) N);			\
    unsigned long _dif_squared = 0;					\
    for(_i = 0; _i < N; _i ++)						\
      _dif_squared += (measures[_i] - mean) * (measures[_i] - mean);	\
    printf("Mean: %.8fs ± %.8fs\n", 0.000001 * mean,			\
	   0.000001 * (sqrt(((double) _dif_squared) / (double) (N-1)))); \
    _i = t_sum = 0;							\
  }
#endif
/********************* TIMER **************************************/

static uint32_t seed, v;

void initialize_seed(void){
  arc4random_buf(&seed, 4);
}


void measure_rand(void){
  struct _Wrng *rng = _Wcreate_rng(malloc, seed);
  int i, j;
  for(i = 0; i < N; i ++){
    TIMER_BEGIN();
    v = _Wrand(rng);
    TIMER_END();
  }
  TIMER_RESULT();
}


int main(int argc, char **argv){
  initialize_seed();
  #if defined(W_RNG_MERSENNE_TWISTER)
  printf("SFMT:       ");
#elif defined(W_RNG_XORSHIRO)
  printf("XORSHIRO**: ");
#elif defined(W_RNG_PCG)
  printf("PCG:        ");
#elif defined(W_RNG_ISO_C)
  printf("ISO C:      ");
#elif defined(W_RNG_CRYPTO)
  printf("Crypto:     ");
#endif

  measure_rand();
  return 0;
}
