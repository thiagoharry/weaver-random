#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <bsd/stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "../src/random.h"

#ifdef _WIN32
#include <windows.h>
#define TIMER_START() { LARGE_INTEGER t1, f, t2, e;		\
  QueryPerformanceFrequency(&f); QueryPerformanceCounter(&t1);
#define TIMER_END() QueryPerformanceCounter(&t2); \
  e.QuadPart = t2.QuadPart - t1.QuadPart; \
  e.QuadPart *= 1000000; \
  e.QuadPart /= f.QuadPart; \
  elapsed = ((double)e.QuadPart) *1e-6; }
#elif defined(__unix__) && !defined(__EMSCRIPTEN__)
#include <time.h>
#define TIMER_START() { struct timespec t1, t2;	\
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
#define TIMER_END() clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);	\
  elapsed = t2.tv_sec + t2.tv_nsec*1e-9 - t1.tv_sec - t1.tv_nsec*1e-9; }
#else
#include <sys/time.h>
#include <sys/resource.h>
#define TIMER_START() { struct timeval t1, t2; gettimeofday(&t1, NULL);
#define TIMER_END()  gettimeofday(&t2, NULL);				\
  elapsed = t2.tv_sec + t2.tv_usec*1e-6 - t1.tv_sec - t1.tv_usec*1e-6; }
#endif

#define ALLOC_SIZE 5*1024
#define N 100000
double measures[N];
double mean;
double standard_deviation;
void *malloc_data[N];
static uint32_t v;
uint32_t seed;

void initialize_seed(void){
  arc4random_buf(&seed, 4);
}


void measure_rand(void){
  struct _Wrng *rng = _Wcreate_rng(malloc, seed);
  int i;
  double elapsed, sum = 0, dif_squared = 0;
  for(i = 0; i < N; i ++){
    TIMER_START();
    v = _Wrand(rng);
    TIMER_END();
    measures[i] = elapsed;
  }
  for(i = 0; i < N; i ++)
    sum += measures[i];
  mean = sum / N;
  for(i = 0; i < N; i ++)
    dif_squared += (measures[i] - mean) * (measures[i] - mean);
  standard_deviation = sqrt(dif_squared / (N - 1));
  printf("%.15f seconds (%.15f seconds) (%d bytes)\n", mean, standard_deviation,
	 (int) sizeof(struct _Wrng));
}


int main(int argc, char **argv){
  initialize_seed();
  #if defined(W_RNG_MERSENNE_TWISTER)
  printf("SFMT:       ");
#elif defined(W_RNG_XORSHIRO)
  printf("XORSHIRO**: ");
#elif defined(W_RNG_PCG)
  printf("PCG:        ");
#endif

  measure_rand();
  return 0;
}
