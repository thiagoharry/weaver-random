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


void test_equidistribution(void){
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed);
  int i, three_tests, all_tests;
  int penalty, fails = 0;
  uint32_t value;
  int bit, bits = 0;
  const int n = 10000;
  // F(x) = Pr(X <= x)
  // F_n(x) = (Número de X_1, ... X_n menores que x) / n
  // K_n+ = \sqrt(n) max_{x \in R} (F_n(x)-F(x)) ; Maior desvio pra cima
  // K_n- = \sqrt(n) max_{x \in R} (F(x)-F_n(x)) ; Maior desvio pra baixo
  for(all_tests = 0; all_tests < 1000; all_tests ++){
    // Repeating the tests 3 times:
    penalty = 0;
    for(three_tests = 0; three_tests < 3; three_tests ++){
      unsigned long number_of_ones = 0, number_of_zeros = 0;
      double V;
      // Measuring n generations:
      for(i = 0; i < n; i ++){
	if(bits == 0){
	  value = _Wrand(my_rng);
	  bit = value % 2;
	  value = value >> 1;
	  bits = 31;
	}
	else{
	  bit = value % 2;
	  value = value >> 1;
	  bits --;
	}
	if(bit == 1) number_of_ones ++;
	else number_of_zeros ++;
      }
      V = ((double) ((number_of_zeros * number_of_zeros * 2) +
		     (number_of_ones * number_of_ones * 2)));
      V = V / ((double) n);
      V -= ((double) n);
      if(V < 0.00016 || V > 6.635)
	penalty += 2;
      else if(V < 0.00393 || V > 3.841)
	penalty ++;
      if(penalty > 1){
	fails ++;
	break;
      }
    } // End of three_tests
  } // End of all_tests
  quality("Quality of equidistribution test", (double) (1000 - fails) /
	  (double) 1000);
  _Wdestroy_rng(free, my_rng);
}

void test_serial(void){
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed);
  int i, round_of_measures, three_tests, total_tests;
  int penalty, fails = 0;
  const int size = 15;
  const int serie = (1 << size);
  unsigned int measures[serie]; // 2^20 values
  const unsigned long long n =  (5 * serie);
  double  v = (serie - 1);
  uint32_t value;
  int bit, bits = 0;
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
      for(i = 0; i < serie; i ++)
	measures[i] = 0;
      for(round_of_measures = 0; round_of_measures < n; round_of_measures ++){
	// Getting 'size' bits:
	int result = 0;
	for(i = 0; i < size; i ++){
	  if(bits == 0){
	    value = _Wrand(my_rng);
	    bit = value % 2;
	    value = value >> 1;
	    bits = 31;	    
	  }
	  else{
	    bit = value % 2;
	    value = value >> 1;
	    bits --;	    
	  }
	  result = result << 1;
	  result += bit;
	}
	measures[result] ++;
      }
      {// Computing V:
	double V = 0.0;
	unsigned long long sum = 0;
	for(i = 0; i < serie; i ++)
	  sum += measures[i] * measures[i] * serie;
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

 void test_poker(void){
   struct _Wrng *my_rng = _Wcreate_rng(malloc, seed);
   int all_tests, fails = 0, all_hands;
   const unsigned int n = 327080;
   uint32_t value;
   int bits = 0;
   for(all_tests = 0; all_tests < 1000; all_tests ++){
     int penalty = 0, three_tests;
     for(three_tests = 0; three_tests < 3; three_tests ++){
       unsigned int j, i;
       unsigned long all_different = 0;
       unsigned long five_of_a_kind = 0;
       unsigned long four_of_a_kind = 0;
       unsigned long full_house = 0;
       unsigned long three_of_a_kind = 0;
       unsigned long two_pairs = 0;
       unsigned long one_pair = 0;
       // Iterating over tuples:
       for(all_hands = 0; all_hands < n; all_hands ++){
	 int sorted[5];
	 for(i = 0; i < 5; i ++){
	   if(bits == 0){
	     value = _Wrand(my_rng);
	     sorted[i] = value % 16;
	     value = value >> 4;
	     bits = 28;	    
	   }
	   else{
	     sorted[i] = value % 16;
	     value = value >> 4;
	     bits -= 4;	    
	   }
	 }
	 // Now really sort the values:
	 for(i = 0; i < 5; i ++){
	   int index = i;
	   int tmp;
	   for(j = i + 1; j < 5; j ++){
	     if(sorted[j] < sorted[index])
	       index = j;
	   }
	   tmp = sorted[i];
	   sorted[i] = sorted[index];
	   sorted[index] = tmp;
	 }
	 // Got the hand sorted. Measuring the result
	 if(sorted[0] == sorted[1] && sorted[1] == sorted[2] &&
	    sorted[2] == sorted[3] && sorted[3] == sorted[4])
	   five_of_a_kind ++;
	 else if((sorted[0] == sorted[1] && sorted[1] == sorted[2] &&
		  sorted[2] == sorted[3]) ||
		 (sorted[1] == sorted[2] && sorted[2] == sorted[3] &&
		  sorted[3] == sorted[4]))
	   four_of_a_kind ++;
	 else if((sorted[0] == sorted[1] && sorted[1] == sorted[2] &&
		  sorted[3] == sorted[4]) ||
		 (sorted[0] == sorted[1] && sorted[2] == sorted[3] &&
		  sorted[3] == sorted[4]))
	   full_house ++;
	 else if((sorted[0] == sorted[1] && sorted[1] == sorted[2]) ||
		 (sorted[1] == sorted[2] && sorted[2] == sorted[3]) ||
		 (sorted[2] == sorted[3] && sorted[3] == sorted[4]))
	   three_of_a_kind ++;
	 else if(sorted[0] != sorted[1] && sorted[1] != sorted[2] &&
		 sorted[2] != sorted[3] && sorted[3] != sorted[4])
	   all_different ++;
	 else{
	   // 1 or 2 pairs
	   int pairs = 0;
	   for(i = 1; i < 5; i ++)
	     if(sorted[i] == sorted[i - 1])
	       pairs ++;
	   if(pairs == 1)
	     one_pair ++;
	   else if(pairs == 2)
	     two_pairs ++;
	 }
       } // End of current hand
       {// Measuring V after the hands:
	 double V = 0;
	 V += (((double) (all_different * all_different * 8192)) / 4095.0);
	 V += (((double) (one_pair * one_pair * 16384)) / 6825.0);
	 V += (((double) (two_pairs * two_pairs * 32768)) / 1575.0);
	 V += (((double) (three_of_a_kind * three_of_a_kind * 16384)) / 525.0);
	 V += (((double) (full_house * full_house * 32768)) / 75.0);
	 V += (((double) (four_of_a_kind * four_of_a_kind * 65536)) / 75.0);
	 V += ((double) (five_of_a_kind * five_of_a_kind * 65536));
	 V /= ((double) n);
	 V -= ((double) n);
	 if(V < 0.8721 || V > 16.81)
	   penalty += 2;
	 else if(V < 1.635 || V > 12.59)
	   penalty ++;
	 if(penalty > 1){
	   fails ++;
	   break;
	 }
       }
     } // End of three tests
   } // End of all tests
   quality("Quality of poker tests", (double) (1000 - fails) /
	  (double) 1000);
   _Wdestroy_rng(free, my_rng);
 }
 
void test_gap(void){
  int i;
  int fails = 0, all_tests;
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed);
  int three_tests, penalty;
  uint32_t Uj;
  int bits = 0, bit;
  for(all_tests = 0; all_tests < 1000; all_tests ++){
    penalty = 0;
    for(three_tests = 0; three_tests < 3; three_tests ++){
      // G1: Initialize:
      unsigned long s = 0, r;
      const unsigned t = 19;
      unsigned long count[t + 1];
      unsigned long n = ((1 << t) * 5);
      for(i = 0; i <= t; i ++)
	count[i] = 0;
      // G2: Set r zero:
      r = 0;
      do{
	// G3: [a <= Uj < B?]:
	if(bits == 0){
	  Uj = _Wrand(my_rng);
	  bit = Uj % 2;
	  Uj = Uj >> 1;
	  bits = 31;
	}
	else{
	  bit = Uj % 2;
	  Uj = Uj >> 1;
	  bits --;
	}
	if(bit == 0){
	  // G4: Increase r
	  r ++;
	  continue;
	}
	else{
	  // G5: Record the gap length:
	  if(r >= t)
	    count[t] ++;
	  else
	    count[r] ++;
	}
	// G6: n gaps found?
	r = 0;
	s ++;
      }while(s < n);    
      {// Computing chi-square test in count[]:
	unsigned long sum = 0;
	double V;
	for(i = 0; i < t + 1; i ++){
	  //printf("%d: {%lu} ", i, count[i]);
	  if(i < t){
	    sum += (count[i] * count[i] * (2 * (1 << i)));
	  }
	  else{
	    sum += (count[i] * count[i] * (1 << i));
	  }
	}
	V = ((double) sum) / ((double) n);
	V -= ((double) n);
	if(V < 8.260 || V > 37.57)
	  penalty += 2;
	else if(V < 10.85 || V > 31.41)
	  penalty ++;
      }
      if(penalty >= 2){
	fails ++;
	break;
      }
    } // End of three tests
  } // End of all tests
  quality("Quality of gap  test", (double) (1000 - fails) /
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
  //test_equidistribution();
  //test_serial();
  //test_gap();
  test_poker();
  imprime_resultado();
  return 0;
}
