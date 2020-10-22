#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
//#include <bsd/stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#if defined(_WIN32)
#include <windows.h>
#endif

#include "../src/random.h"

int numero_de_testes = 0, acertos = 0, falhas = 0;

#ifdef W_RNG_MERSENNE_TWISTER
#include "sfmt.c"
#elif defined(W_RNG_XORSHIRO)
#include "xorshiro.c"
#elif defined(W_RNG_PCG)
#include "pcg.c"
#endif

size_t count_utf8_code_points(const char *s) {
    size_t count = 0;
    while (*s) {
        count += (*s++ & 0xC0) != 0x80;
    }
    return count;
}

void quality(char *descricao, float valor){
  char pontos[72], *s = descricao;
  size_t tamanho_string = 0;
  int i;
  while(*s)
    tamanho_string += (*s++ & 0xC0) != 0x80;
  pontos[0] = ' ';
  for(i = 1; i < 71 - tamanho_string; i ++)
    pontos[i] = '.';
  pontos[i] = '\0';
  numero_de_testes ++;
  printf("%s%s", descricao, pontos);
#if defined(__unix__) && !defined(__EMSCRIPTEN__)
  if(valor > 0.6666666){
    printf("\e[32m[%03d%%]\033[0m\n", (int) (valor*100));
  }
  else if(valor > 0.333333333){
    printf("\e[1;33m[%03d%%]\033[0m\n", (int) (valor*100));
  }
  else{
    printf("\033[0;31m[%03d%%]\033[0m\n", (int) (valor*100));
  }
#else
  printf("[%03d\%]\n", (int) (valor*100));
#endif
}

void assert(char *descricao, bool valor){
  char pontos[72], *s = descricao;
  size_t tamanho_string = 0;
  int i;
  while(*s)
    tamanho_string += (*s++ & 0xC0) != 0x80;
  pontos[0] = ' ';
  for(i = 1; i < 71 - tamanho_string; i ++)
    pontos[i] = '.';
  pontos[i] = '\0';
  numero_de_testes ++;
  printf("%s%s", descricao, pontos);
  if(valor){
#if defined(__unix__) && !defined(__EMSCRIPTEN__)
    printf("\e[32m[OK]\033[0m\n");
#else
    printf("[OK]\n");
#endif
    acertos ++;
  }
  else{
#if defined(__unix__) && !defined(__EMSCRIPTEN__)
    printf("\033[0;31m[FAIL]\033[0m\n");
#else
    printf("[FAIL]\n");
#endif
    falhas ++;
  }
}

void imprime_resultado(void){
  printf("\n%d tests: %d sucess, %d fails\n\n",
	 numero_de_testes, acertos, falhas);
}

/* Globais */

uint32_t seed;

void initialize_seed(void){
  arc4random_buf(&seed, 4);
}

/* Início dos Testes */
#if defined(W_RNG_MERSENNE_TWISTER)
void test_mersenne_twister(void){
  bool equal = true;
  int i;
  struct SFMT_T ref_rng;
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed);
  sfmt_init_gen_rand(&ref_rng, seed);
  {
    char *p1 = (char *) &(ref_rng.state), *p2 = (char *) my_rng -> w;
    for(i = 0; i < _W * _N / 8; i ++){
      if(*p1 != *p2){
	equal = false;
	break;
      }
      p1 ++;
      p2 ++;
    }
    assert("SFMT initialization works as in reference code", equal);
    equal = true;
  }
  for(i = 0; i < 1000; i ++){
    uint64_t a, b;
    a = _Wrand(my_rng);
    b = sfmt_genrand_uint32(&ref_rng);
    if(a != b){
      equal = false;
      break;
    }
  }
  assert("SFMT generates same numbers as in reference", equal);
  _Wdestroy_rng(free, my_rng);
}
#endif

#if defined(W_RNG_XORSHIRO)
void test_xorshiro(void){
  bool equal = true;
  int i;
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed);
  for(i = 0; i < 4; i ++)
    s[i] = my_rng -> w[i];
  for(i = 0; i < 1000; i ++){
    uint64_t a, b;
    _Wrand(my_rng);
    a = my_rng -> result;
    _Wrand(my_rng);
    b = next();
    if(a != b){
      equal = false;
      break;
    }
  }
  assert("Xorshiro** generates same numbers as in reference", equal);
  _Wdestroy_rng(free, my_rng);
}
#endif

#if defined(W_RNG_PCG)
void test_pcg(void){
  bool equal = true;
  int i;
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed);
  pcg32_random_t pcg_rng;
  pcg_rng.state = my_rng -> state;
  pcg_rng.inc = my_rng -> increment;
  for(i = 0; i < 1000; i ++){
    uint64_t a, b;    
    a = _Wrand(my_rng);
    b = pcg32_random_r(&pcg_rng);
    if(a != b){
      equal = false;
      break;
    }
  }
  assert("PCG generates same numbers as in reference", equal);
  _Wdestroy_rng(free, my_rng);
}
#endif

// Thread test
#define THREAD_NUMBER 1000
#if defined(_WIN32)
DWORD _WINAPI thread_function(void *my_rng){
#else
void *thread_function(void *my_rng){
#endif
  struct _Wrng *rng = (struct _Wrng *) my_rng;
  int i;
  for(i = 0; i < 5; i ++)
    _Wrand(rng);
#if defined(_WIN32)
  return 0;
#else
  return NULL;
#endif
}
 
void test_multithread(void){
  uint32_t a, b;
  int i;
#if defined(_WIN32)
  HANDLE thread[THREAD_NUMBER];
#else
  pthread_t thread[THREAD_NUMBER];
#endif
  struct _Wrng *my_rng1 = _Wcreate_rng(malloc, seed);
  struct _Wrng *my_rng2 = _Wcreate_rng(malloc, seed);
  for(i = 0; i < THREAD_NUMBER; i ++){
#if defined(_WIN32)
    thread[i] = CreateThread(NULL, 0, thread_function, my_rng1, 0, NULL);
#else
    pthread_create(&(thread[i]), NULL, thread_function, my_rng1);
#endif
  }
  for(i = 0; i < THREAD_NUMBER; i ++){
#if defined(_WIN32)
    _WaitForSingleObject(thread[i], INFINITE);
#else
    pthread_join(thread[i], NULL);  
#endif
  }
  for(i = 0; i < 5 * THREAD_NUMBER; i ++)
    _Wrand(my_rng2);
  a = _Wrand(my_rng1);
  b = _Wrand(my_rng2);
  assert("RNG works with multiple threads", a == b);
  _Wdestroy_rng(free, my_rng1);
  _Wdestroy_rng(free, my_rng2);
}

uint32_t reverse(uint32_t x){
  x = ((x & 0x55555555) << 1) | ((x & 0xAAAAAAAA) >> 1); // Swap _<>_
  x = ((x & 0x33333333) << 2) | ((x & 0xCCCCCCCC) >> 2); // Swap __<>__
  x = ((x & 0x0F0F0F0F) << 4) | ((x & 0xF0F0F0F0) >> 4); // Swap ____<>____
  x = ((x & 0x00FF00FF) << 8) | ((x & 0xFF00FF00) >> 8); // Swap ...
  x = ((x & 0x0000FFFF) << 16) | ((x & 0xFFFF0000) >> 16); // Swap ...
  return x;
}


void test_equidistribution(bool reversal){
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed);
  uint32_t measures[1000];
  double Kp[30], Km[30], Kpp, Kmm;
  int i, j, k, three_tests, all_tests;
  int penalty1, penalty2, fails1 = 0, fails2 = 0;
  // F(x) = Pr(X <= x)
  // F_n(x) = (Número de X_1, ... X_n menores que x) / n
  // K_n+ = \sqrt(n) max_{x \in R} (F_n(x)-F(x)) ; Maior desvio pra cima
  // K_n- = \sqrt(n) max_{x \in R} (F(x)-F_n(x)) ; Maior desvio pra baixo
  for(all_tests = 0; all_tests < 1000; all_tests ++){
    // Repeating the tests 3 times:
    penalty1 = 0;
    penalty2 = 0;
    for(three_tests = 0; three_tests < 3; three_tests ++){
      // Repeating 30 times:
      for(k = 0; k < 30; k ++){
	// Measuring 1000 generations:
	for(i = 0; i < 1000; i ++)
	  if(reversal)
	    measures[i] = reverse(_Wrand(my_rng));
	  else
	    measures[i] = _Wrand(my_rng);
	// Sorting the measures:
	for(i = 0; i < 1000; i ++){
	  uint32_t tmp, min_value = measures[i];
	  int index = i;
	  for(j = i + 1; j < 1000; j ++){
	    if(measures[j] < min_value){
	      min_value = measures[j];
	      index = j;
	    }
	  }
	  tmp = measures[i];
	  measures[i] = measures[index];
	  measures[index] = tmp;
	}
	// Computing K_{1000}+:
	{
	  double max = 0.0;
	  for(i = 0; i < 1000; i ++){
	    double result = (((double) i + 1) / 1000.0) -
	      ((double) measures[i] / (double) 4294967295ull);
	    if(result > max)
	      max = result;
	  }
	  Kp[k] = 31.622776601683793 * max; // sqrt(1000) * max(j/1000 - F(X_i))
	}
	// Computing K_{1000}-:
	{
	  double max = 0.0;
	  for(i = 0; i < 1000; i ++){
	    double result =  ((double) measures[i] / (double) 4294967295ull) -
	      (((double) i) / 1000.0);
	    if(result > max)
	      max = result;
	  }
	  Km[k] = 31.622776601683793 *max; // sqrt(1000) * max(j/1000 - F(X_i))
	}
      }
      // Computed the Smirnof-Kolmogorov test over 1000 samples 30
      // times. Now applying a new Smirnof-Kolmogorov test over these 30 results:
      // Sorting:
      for(i = 0; i < 30; i ++){
	double tmp, min_value = Kp[i];
	int index = i;
	for(j = i + 1; j < 30; j ++){
	  if(Kp[j] < min_value){
	    min_value = Kp[j];
	    index = j;
	  }
	}
	tmp = Kp[i];
	Kp[i] = Kp[index];
	Kp[index] = tmp;
      }
      for(i = 0; i < 30; i ++){
	double tmp, min_value = Km[i];
	int index = i;
	for(j = i + 1; j < 30; j ++){
	  if(Km[j] < min_value){
	    min_value = Km[j];
	    index = j;
	  }
	}
	tmp = Km[i];
	Km[i] = Km[index];
	Km[index] = tmp;
      }
      // Computing K_{30}+:
      {
	double max = 0.0;
	for(i = 0; i < 30; i ++){
	  double result = (((double) i + 1) / 30.0) -
	    (1.0 - pow(M_E, -2.0 * Kp[i] * Kp[i]));
	  if(result > max)
	    max = result;
	}
	Kpp = 5.477225575051661 * max; // sqrt(30) * max(j/1000 - F(X_i))
      }
      // Computing K_{30}-:
      {
	double max = 0.0;
	for(i = 0; i < 30; i ++){
	  double result = (1.0 - pow(M_E, -2.0 * Km[i] * Km[i])) -
	    (((double) i) / 30.0);
	  if(result > max)
	    max = result;
	}
	Kmm = 5.477225575051661 * max; // sqrt(30) * max(j/1000 - F(X_i))
      }
      if(Kpp < 0.04354  || Kpp > 1.4801)
	penalty1 += 2;
      else if(Kpp < 0.1351 || Kpp > 1.1916)
	penalty1 ++;
      if(Kmm < 0.04354 || Kmm > 1.4801)
	penalty2 += 2;
      else if(Kmm < 0.1351 || Kmm > 1.1916)
	penalty2 ++;
      //printf("DEBUG: K+: %f K-: %f\n", Kpp, Kmm);
    } // End of three_tests
    if(penalty1 > 1)
      fails1 ++;
    if(penalty2 > 1)
      fails2 ++;
  } // End of all_tests
  quality("Quality of equidistribution test (K+)", (double) (1000 - fails1) /
	  (double) 1000);
  quality("Quality of equidistribution test (K-)", (double) (1000 - fails2) /
	  (double) 1000);
  _Wdestroy_rng(free, my_rng);
}

void test_serial(bool reversal){
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed);
  const int d = 100;
  int i, j, round_of_measures, three_tests, total_tests;
  int penalty, fails = 0;
  unsigned long measures[d][d];
  const int n =  d * d * 5;
  double  v = ((double) (d * d - 1));
  double too_bad = v - (2.33 * sqrt(2 * v)) + ((2.33 * 2.33 * 2.0) / 3.0) -
    0.66666666;
  double little_bad = v - (1.64 * sqrt(2 * v)) + ((1.64 * 1.64 * 2.0) / 3.0) -
    0.66666666;
  double little_ideal = v + (1.64 * sqrt(2 * v)) + ((1.64 * 1.64 * 2.0) / 3.0) -
    0.66666666;
  double too_ideal = v + (2.33 * sqrt(2 * v)) + ((2.33 * 2.33 * 2.0) / 3.0) -
    0.66666666;
  for(total_tests = 0; total_tests < 1000; total_tests ++){
    penalty = 0;
    for(three_tests = 0; three_tests < 3; three_tests ++){
      //printf("Teste %d/3\n", three_tests + 1);
      for(i = 0; i < d; i ++)
	for(j = 0; j < d; j ++)
	  measures[i][j] = 0;
      for(round_of_measures = 0; round_of_measures < n; round_of_measures ++){
	{ // Getting random values
	  int v1, v2;
	  if(reversal){
	    v1 = (int) ((((double) reverse(_Wrand(my_rng))) /
			 (double) 4294967295ull) * d);
	    v2 = (int) ((((double) reverse(_Wrand(my_rng))) /
			 (double) 4294967295ull) * d);
	  }
	  else{
	    v1 = (int) ((((double) _Wrand(my_rng)) /
			 (double) 4294967295ull) * d);
	    v2  = (int) ((((double) _Wrand(my_rng)) /
			  (double) 4294967295ull) * d);
	  }
	  if(v1 == d) v1 = d - 1; // Very rare event
	  if(v2 == d) v2 = d - 1; // Very rare event
	  measures[v1][v2] ++;
	}
      }
      {// Computing V:
	double V = 0.0;
	unsigned long long sum = 0;
	for(i = 0; i < d; i ++)
	  for(j = 0; j < d; j ++)
	    sum += ((double) (measures[i][j] * measures[i][j] * d * d));
	V = ((double) sum) / ((double) n);
	V -= n;
	//printf("penalty: %d\n", penalty);
	//printf("%f %f | %f %f\n", too_bad, little_bad, little_ideal, too_ideal);
	//printf("V: %f\n", V);
	if(V < too_bad || V > too_ideal)
	  penalty += 2;
	else if(V < little_bad || V > little_ideal)
	  penalty ++;
	if(penalty >= 2){
	  fails ++;
	  break;
	}
      }
    } // End of three tests
  } // End of total_tests
  quality("Quality of serial tests", (double) (1000 - fails) /
	  (double) 1000);
  _Wdestroy_rng(free, my_rng);
}
 
int main(int argc, char **argv){
  if(argc <= 1)
    initialize_seed();
  else
    seed = strtoull(argv[1], NULL, 10);
#if defined(W_RNG_MERSENNE_TWISTER)
  printf("Starting MERSENNE TWISTER tests. Seed: %lu\n\n", (long unsigned int) seed);
  test_mersenne_twister();
#elif defined(W_RNG_XORSHIRO)
  printf("Starting XORSHIRO** tests. Seed: %lu\n\n", (long unsigned int) seed);
  test_xorshiro();
#elif defined(W_RNG_PCG)
  printf("Starting PCG tests. Seed: %lu\n\n", (long unsigned int) seed);
  test_pcg();
#elif defined(W_RNG_ISO_C)
  printf("Starting ISO C RNG tests. Seed: %lu\n\n", (long unsigned int) seed);
#elif defined(W_RNG_CRYPTO)
  printf("Starting Crypto RNG tests. Seed: %lu\n\n", (long unsigned int) seed);
#endif
#if !defined(W_RNG_ISO_C) && !defined(W_RNG_CRYPTO)
  test_multithread();
#endif
  // Argument means: should reverse the obtained bits?
  test_equidistribution(false);
  test_serial(false);
  imprime_resultado();
  return 0;
}
