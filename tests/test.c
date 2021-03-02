#include <sys/time.h>
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

/* SEED TESTED */
uint64_t seed[] = {
  0x32147198b5436569, 0x260287febfeb34e9, 0x0b6cc94a91a265e4, 0xc6a109c50dd52f1b,
  0x8298497f3992d73a};
size_t seed_size = sizeof(seed) / 8;

#ifdef W_RNG_MERSENNE_TWISTER
#include "sfmt.c"
#elif defined(W_RNG_XORSHIRO)
#include "xorshiro.c"
#elif defined(W_RNG_PCG)
#include "pcg.c"
#endif


uint64_t rng_value;
int rng_bit, rng_bits = 0;
int read_random_bits(struct _Wrng *my_rng, int size_in_bits){
  int result = 0;
  if(rng_bits == 0){// Need to generate next random bits
    rng_value = _Wrand(my_rng);
    result = rng_value % (1 << size_in_bits);
    rng_value = rng_value >> (size_in_bits);
    rng_bits = 64 - size_in_bits;    
  }
  else if(size_in_bits > rng_bits){// We have some, but need more random bits
    result = rng_value;
    result = result << (size_in_bits - rng_bits);
    rng_value = _Wrand(my_rng);
    result += rng_value % (1 << (size_in_bits - rng_bits));
    rng_value = rng_value >> (size_in_bits - rng_bits);
    rng_bits = 64 - (size_in_bits - rng_bits);
  }
  else{ // We still have remaining random bits, dont generate more
    result = rng_value % (1 << size_in_bits);
    rng_value = rng_value >> (size_in_bits);
    rng_bits -= size_in_bits;    
  }
  return result;
}

const double table_un[] = { // Chi-square table
  0.00016, 0.00393, 3.841, 6.635, //v=1
  0.02010, 0.1026, 5.991, 9.210, //v=2
  0.1148, 0.3518, 7.815, 11.34, //v=3
  0.2971, 0.7107, 9.488, 13,28, //v=4
  0.5543, 1.1455, 11.07, 15.09, // v=5
  0.8721, 1.635, 12.59, 16.81, // v=6
  1.239, 2.167, 14.07, 18.48, //v=7
  1.646, 2.733, 15.51, 20.09,  //v=8
  2.088, 3.325, 16.92, 21.67, //v=9
  2.558, 3.940, 18.31, 23.21, // v=10
  3.053, 4.575, 19.68, 24.72, // v=11
  3.571, 5.226, 21.03, 26.22 //v=12
};

int chi_square(int number_of_cases, unsigned long *observed, double *prob){
  unsigned long long n = 0;
  double V = 0.0;
  int i, v = number_of_cases - 1;
  for(i = 0; i < number_of_cases; i ++)
    n += observed[i];
  for(i = 0; i < number_of_cases; i ++)
    V += (double) (observed[i] * observed[i] *
		   ((prob==NULL)?(number_of_cases):(1.0 / prob[i])));
  V /= (double) n;
  V -= n;
  if(v <= 12){
    if(V < table_un[(v - 1) * 4] || V > table_un[(v - 1) * 4 + 3])
      return 2;
    else if(V < table_un[(v - 1) * 4 + 1] || V > table_un[(v - 1) * 4 + 2])
      return 1;
    else
      return 0;
  }
  if(v == 15){
    if(V < 5.229 || V > 30.58)
      return 2;
    else if(V < 7.261 || V > 25.00)
      return 1;
    else
      return 0;
  }
  if(v == 20){
    if(V < 8.260 || V > 37.57)
      return 2;
    else if(V < 10.85 || V > 31.41)
      return 1;
    else
      return 0;
  }
  if(v == 30){
    if(V < 14.95 || V > 50.89)
      return 2;
    else if(V < 18.49 || V > 43.77)
      return 1;
    else
      return 0;
  }
  if(v == 50){
    if(V < 14.95 || V > 50.89)
      return 2;
    else if(V < 18.49 || V > 43.77)
      return 1;
    else
      return 0;
  }
  else{
    if(v < 30)
      fprintf(stderr, "\nERROR: Chi-square test with unsupported value (%d).\n",
	      v);
    if(V < v - sqrt(2 * v) * 2.33 + 2.9526 ||
       V > v + sqrt(2 * v) * 2.33 + 2.9526)
      return 2;
    else if(V < v - sqrt(2 * v) * 1.64 + 1.1264 ||
	    V > v + sqrt(2 * v) * 1.64 + 1.1264)
      return 1;
    else
      return 0;
  }
}

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
  printf("[%03d%%]\n", (int) (valor*100));
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

/* Início dos Testes */
#if defined(W_RNG_MERSENNE_TWISTER)
void test_mersenne_twister(void){
  bool equal = true;
  int i;
  struct SFMT_T ref_rng;
  //uint64_t array[size];
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed_size, seed);
  sfmt_init_by_array(&ref_rng, (uint32_t *) seed, seed_size * 2);
  {
    unsigned char *p1 = (unsigned char *) &(ref_rng.state),
      *p2 = (unsigned char *) my_rng -> w;
    for(i = 0; i < _W * _N / 8; i ++){
      if(*p1 != *p2){
	equal = false;
	//break;
      }
      p1 ++;
      p2 ++;
    }
    assert("SFMT initialization works as in reference code", equal);
    equal = true;
  }
    //sfmt_fill_array64(&ref_rng, array, size);
  for(i = 0; i < 1000; i ++){
    uint64_t a, b;
    a = _Wrand(my_rng);
    b = sfmt_genrand_uint64(&ref_rng);
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
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed_size, seed);
  for(i = 0; i < 4; i ++)
    s[i] = my_rng -> w[i];
  for(i = 0; i < 1000; i ++){
    uint64_t a, b;
    a = _Wrand(my_rng);
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
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed_size, seed);
  struct pcg64_random_t ref_rng;
  pcg64_srandom_r(&ref_rng,
		  PCG_128BIT_CONSTANT(seed[0], seed[1]),
		  PCG_128BIT_CONSTANT(seed[2], seed[3]));
  if(my_rng -> state != ref_rng.state || my_rng -> increment != ref_rng.inc)
    equal = false;
  assert("PCG initialization works as in reference code", equal);
  equal = true;
  for(i = 0; i < 1000; i ++){
    uint64_t a, b;    
    a = _Wrand(my_rng);
    b = pcg64_random_r(&ref_rng);
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
DWORD _WINAPI thread_function(void *my_rng)
#else
void *thread_function(void *my_rng)
#endif
{
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
  struct _Wrng *my_rng1 = _Wcreate_rng(malloc, seed_size, seed);
  struct _Wrng *my_rng2 = _Wcreate_rng(malloc, seed_size, seed);
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
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed_size, seed);
  int i, three_tests, all_tests;
  int penalty, fails = 0;
  const int n = 10000;
  for(all_tests = 0; all_tests < 1000; all_tests ++){
    // Repeating the tests 3 times:
    penalty = 0;
    for(three_tests = 0; three_tests < 3; three_tests ++){
      unsigned long values[2] = {0, 0};
      // Measuring n generations:
      for(i = 0; i < n; i ++)
	values[read_random_bits(my_rng, 1)] ++;
      penalty += chi_square(2, values, NULL);
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
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed_size, seed);
  int i, round_of_measures, three_tests, total_tests;
  int penalty, fails = 0;
  const int size = 15;
  const int serie = (1 << size);
  unsigned long measures[serie]; // 2^20 values
  const unsigned long long n =  (5 * serie);
  for(total_tests = 0; total_tests < 1000; total_tests ++){
    penalty = 0;
    for(three_tests = 0; three_tests < 3; three_tests ++){
      for(i = 0; i < serie; i ++)
	measures[i] = 0;
      for(round_of_measures = 0; round_of_measures < n; round_of_measures ++)
	measures[read_random_bits(my_rng, 15)] ++;
      penalty += chi_square(serie, measures, NULL);
      if(penalty >= 2){
	fails ++;
	break;
      }
    } // End of three tests
  } // End of total_tests
  quality("Quality of serial tests", (double) (1000 - fails) /
	  (double) 1000);
  _Wdestroy_rng(free, my_rng);
}

void test_collector(void){
  const int d = 16;
  const int t = 116;
  // Recompute if d or t are changed:
  double expect[101] = {881657.9515664614, 117554.3935421949, 28862.4597955261, 9834.6159303274, 4151.8471814280, 2043.8330495174, 1130.2241210724, 684.7723865789, 446.5752400178, 309.4014816677, 225.4720134293, 171.4810062499, 135.2665663802, 110.1098645734, 92.1136805065, 78.9202502949, 69.0498996206, 61.5437070165, 55.7624048321, 51.2690472285, 47.7583139840, 45.0126672746, 42.8744459230, 41.2276812535, 39.9859923191, 39.0843700984, 38.4735017234, 38.1157857221, 37.9824931025, 38.0517176836, 38.3068784058, 38.7356132332, 39.3289546220, 40.0807100406, 40.9869936498, 42.0458707346, 43.2570872144, 44.6218640874, 46.1427420087, 47.8234650383, 49.6688953706, 51.6849528952, 53.8785749364, 56.2576926473, 58.8312213708, 61.6090629272, 64.6021182752, 67.8223093754, 71.2826093822, 74.9970805244, 78.9809192229, 83.2505081438, 87.8234750126, 92.7187581160, 97.9566785071, 103.5590190039, 109.5491101400, 115.9519232839, 122.7941711985, 130.1044163646, 137.9131874391, 146.2531042688, 155.1590119228, 164.6681242584, 174.8201775780, 185.6575949861, 197.2256621031, 209.5727148437, 222.7503400260, 236.8135896260, 251.8212095602, 267.8358839344, 284.9244957684, 303.1584052718, 322.6137468258, 343.3717458982, 365.5190572106, 389.1481255582, 414.3575707844, 441.2525985090, 469.9454383189, 500.5558112444, 533.2114284663, 568.0485233301, 605.2124188820, 644.8581332899, 687.1510256710, 732.2674850155, 780.3956650755, 831.7362682813, 886.5033819494, 944.9253702669, 1007.2458257671, 1073.7245842616, 1144.6388074560, 1220.2841377602, 1300.9759301053, 1387.0505658986, 1478.8668545906, 1576.8075286957, 104.7974528089};
  // Also recompute if d or t are changed:
  unsigned long n = 4408394ul;
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed_size, seed);
  int thousand_tests, fails = 0, three_tests, penalty, card;
  for(thousand_tests = 0; thousand_tests < 1000; thousand_tests ++){
    penalty = 0;
    for(three_tests = 0; three_tests < 3; three_tests ++){
      // C1: Initialize:
      unsigned long s;
      int r;
      unsigned long count[t+1];
      for(r = 0; r <= t; r ++)
	count[r] = 0;
      for(s = 0; s < n; s ++){
	// C2: Set q=r=0
	int k, q = 0;
	int r = 0;
	int occurs[d];
	for(k = 0; k < d; k ++)
	  occurs[k] = 0;
	while(q < d){
	  // C3: Next observation:
	  r ++;
	  // Observation
	  card = read_random_bits(my_rng, 4);
	  if(occurs[card] != 0)
	    continue; // Repeated card
	  // C4: Completed?
	  occurs[card] = 1;
	  q ++;
	}
	// C5: Register size:
	if(r >= t)
	  count[t] ++;
	else
	  count[r] ++;
      } // Loop gettng all values
      { // Computing V:
	int i, v = t - d;
	double V = 0;
	double too_bad = v - (2.33 * sqrt(2 * v)) + ((2.33 * 2.33 * 2.0) / 3.0) -
	  0.66666666;
	double little_bad = v - (1.64 * sqrt(2 * v)) +
	  ((1.64 * 1.64 * 2.0) / 3.0) - 0.66666666;
	double little_ideal = v + (1.64 * sqrt(2 * v)) +
	  ((1.64 * 1.64 * 2.0) / 3.0) - 0.66666666;
	double too_ideal = v + (2.33 * sqrt(2 * v)) +
	  ((2.33 * 2.33 * 2.0) / 3.0) - 0.66666666;
	for(i = d; i <= t; i ++){
	  V += ((double) (count[i] * count [i])) * expect[i-d];
	}
	V /= ((double) n);
	V -= ((double) n);
	if(V < too_bad || V > too_ideal)
	  penalty += 2;
	else if(V < little_bad || V > little_ideal)
	  penalty ++;
	if(penalty > 1){
	  fails ++;
	  break;
	} 
      }
    }
  }
  quality("Quality of collectors tests", (double) (1000 - fails) /
	  (double) 1000);
  _Wdestroy_rng(free, my_rng);
}

 
void test_poker(void){
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed_size, seed);
  int all_tests, fails = 0, all_hands;
   const unsigned int n = 327080;
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
	 for(i = 0; i < 5; i ++)
	   sorted[i] = read_random_bits(my_rng, 4);
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
  int i, fails = 0, all_tests, three_tests, penalty;
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed_size, seed);
  for(all_tests = 0; all_tests < 1000; all_tests ++){
    penalty = 0;
    for(three_tests = 0; three_tests < 3; three_tests ++){
      // G1: 
      const unsigned t = 20;
      unsigned long count[t + 1], inv[t + 1], s = 0, r, n = ((1 << t) * 5);
      double prob[t+1];
      for(i = 0; i <= t; i ++){
	count[i] = 0;
	inv[i] = (i<t)?(2 * (1 << i)):(1 << i);
	prob[i] = 1.0 / (double) inv[i];
      }
      // G2: Set r zero:
      r = 0;
      do{
	// G3: [a <= Uj < B?]:
	if(read_random_bits(my_rng, 1) == 0){
	  // G4: Increase r
	  r ++;
	  continue;
	}
	else // G5: Record the gap length:
	    count[(r>=t)?(t):(r)] ++;
	// G6: n gaps found?
	r = 0;
	s ++;
      }while(s < n);    
      penalty += chi_square(t + 1, count, prob);
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

unsigned long long fatorial(int n){
  unsigned long long r = 1;
  int i;
  for(i = 2; i <= n; i ++)
    r *= i;
  return r;
}

int permutation_identity(int quantity, int *U){
  int r, i, f = 0; // P1
  for(r = quantity - 1; r > 0; r --){
    // P2: Find maximum:
    int max = -1, index = -1;
    for(i = 0; i <= r; i ++){
      if(U[i] > max){
	max = U[i];
	index = i;
      }
    }
    f = f * (r + 1) + index;
    // P3: Exchange;
    U[index] = U[r];
    U[r] = max;
  }// P4: Loop test
  return f;
}
void test_permutation(void){
  const int BITS_TESTED = 3;// Counting permutations of {0, ..., 7}
  int fails = 0, all_tests, three_tests, penalty;
  unsigned long long i;
  const unsigned long long  n = 5 * fatorial(1 << BITS_TESTED); // 5*8!
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed_size, seed);
  for(all_tests = 0; all_tests < 1000; all_tests ++){
    penalty = 0;
    for(three_tests = 0; three_tests < 3; three_tests ++){
      unsigned long long n_tests;
      unsigned long count[n/5]; //8! possible values
       for(i = 0; i < n/5; i ++)
	 count[i] = 0;
       for(n_tests = 0; n_tests < n; n_tests ++){
	 // Generating the list
	 int U[1 << BITS_TESTED]; // 8 values to be generated
	 int gen = 0, control = 0; // Control: bit mask with already generated
	 while(control != 0xff){ // While not all generated
	   U[gen] = read_random_bits(my_rng, BITS_TESTED); // Generate 1
	   if(!(control & (1 << (U[gen])))){ // Check if new
	     control = control | (1 << U[gen]); // Update
	     gen ++;
	   }
	 }
	 // Generated all, get the permutation identity
	 count[permutation_identity(1 << BITS_TESTED, U)] ++;
       } // End of n_tests
       // Chi-square test

       penalty += chi_square(n/5, count, NULL);
       if(penalty >= 2){
	 fails ++;
	 break;
       }
    } // End of three tests
   } // End of all tests
   quality("Quality of permutation test", (double) (1000 - fails) /
	  (double) 1000);
   _Wdestroy_rng(free, my_rng);
}

void test_runs_up(void){
  const int BITS = 13;
  int i, n = 1;
  for(i = 0; i < BITS; i ++)
    n *= 2;
  unsigned long bn[6] = {n * 1, n * 5, n * 11, n * 19, n * 29, n * 1};
  unsigned long bd[6] = {6, 24, 120, 720, 5040, 840};
  double a[6][6] = {{4529.4, 9044.9, 13568, 18091, 22615, 27892},
		    {9044.9, 18097, 27139, 36187, 45234, 55789},
		    {13568, 27139, 40721, 54281, 67852, 83685},
		    {18091, 36187, 54281, 72414, 90470, 111580},
		    {22615, 45234, 67852, 90470, 113262, 139476},
		    {27892, 55789, 83685, 111580, 139476, 172860}};
  int fails = 0, all_tests, three_tests, penalty, j, c, prev;
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed_size, seed);
  for(all_tests = 0; all_tests < 1000; all_tests ++){
    penalty = 0;
    for(three_tests = 0; three_tests < 3; three_tests ++){
      int generated = 0, values[n], count[6];
      bool chosen[n]; // Generating permutation of 4096 elements
      for(i = 0; i < n; i ++) chosen[i] = 0;
      for(i = 0; i < 6; i ++) count[i] = 0;
      while(generated < n){
	values[generated] = read_random_bits(my_rng, BITS);
	if(chosen[values[generated]] == 0){
	  chosen[values[generated]] = 1;
	  generated ++;
	}
      }
      c = -1; // Count values
      prev = -1;
      for(i = 0; i < n; i ++){
	if(values[i] > prev) c++;
	else{
	  count[(c < 6)?(c):(5)] ++;
	  c = 0;
	}
	prev = values[i];
      }
      count[(c < 6)?(c):(5)] ++;
      {// COmpute V:
	double V = 0.0;
	for(i = 0; i < 6; i ++)
	  for(j = 0; j < 6; j ++){
	    long long num = count[i] * count[j] * bd[i] * bd[j] -
	      count[i] * bn[j] * bd[i] - count[j] * bn[i] * bd[j] + bn[i] * bn[j];
	    long long den = bd[i] * bd[j];
	    V += (a[i][j] * (((double) num) / ((double) den)));
	  }
	V /= (n-6);
	if(V < 0.8721 || V > 16.81) penalty += 2;
	else if(V < 1.635 || V > 12.59) penalty ++;
	if(penalty > 1){
	  fails ++;
	  break;
	}
      }
    }
  }
  quality("Quality of runs-up test", (double) (1000 - fails) /
	  (double) 1000);
  _Wdestroy_rng(free, my_rng);
}

void test_maximum_of_t(void){
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed_size, seed);
  int i, j, three_tests, total_tests;
  int penalty, fails = 0;
  const int t = 3;
  const int BITS = 6;
  const int n = 1310720; // Deve ser 5*2^(BITS * t)
  const int indices = 64; // Deve ser 2^BITS
  unsigned long count[indices];
  double prob[indices];
  for(i = 0; i < indices; i ++)
    prob[i] = ((double) (t * (i * i + i) + 1)) / (1 << (BITS * t));
  for(total_tests = 0; total_tests < 1000; total_tests ++){
    penalty = 0;
    for(three_tests = 0; three_tests < 3; three_tests ++){
      for(i = 0; i < indices; i ++)
	count[i] = 0;
      for(i = 0; i < n; i ++){ // Repeating 'n' times: 
	unsigned long max = 0;
	for(j = 0; j < t; j ++){ // Maximum of 't':
	  unsigned long temp = read_random_bits(my_rng, BITS); 
	  if(temp > max)
	    max = temp;
	}
	count[max] ++;
      }
      penalty += chi_square(indices, count, prob);
      if(penalty >= 2){
	fails ++;
	break;
      }
    } // End of three tests
  } // End of total_tests
  quality("Quality of maximum of t (t=3) test", (double) (1000 - fails) /
	  (double) 1000);
  _Wdestroy_rng(free, my_rng);
}

void test_collision(void){
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed_size, seed);
  int i, three_tests, total_tests;
  int penalty, fails = 0;
  const int BITS = 20;
  const int m = (1 << BITS);
  const int n = (1 << 14);
  for(total_tests = 0; total_tests < 1000; total_tests ++){
    penalty = 0;
    for(three_tests = 0; three_tests < 3; three_tests ++){
      int d[m];
      int collisions = 0;
      for(i = 0; i < m; i ++)
	d[i] = 0;
      for(i = 0; i < n; i ++){
	int v =  read_random_bits(my_rng, BITS);
	if(d[v] != 0)
	  collisions ++;
	d[v]++;
      }
      if(collisions < 101 || collisions > 153)
	penalty += 2;
      else if(collisions < 108 || collisions > 145)
	penalty ++;
      if(penalty >= 2){
	fails ++;
	break;
      }
    }// End of three tests
  }// End of total_tests
  quality("Quality of collision test", (double) (1000 - fails) /
	  (double) 1000);
  _Wdestroy_rng(free, my_rng);
}

void test_birthday_spacing(void){
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed_size, seed);
  int i, j, three_tests, total_tests, spacing_test;
  int penalty, fails = 0;
  const int BITS = 25;
  const int n = 512;
  double prob[4] = {0.368801577, 0.369035243, 0.183471182, 0.078691997};
  for(total_tests = 0; total_tests < 1000; total_tests ++){
    penalty = 0;
    for(three_tests = 0; three_tests < 3; three_tests ++){
      unsigned long birthday[n], spacing[n], equal_spacing[4];
      for(i = 0; i < 4; i ++)
	equal_spacing[i] = 0;
      for(spacing_test = 0; spacing_test < 1000; spacing_test ++){
	int spac = 0;
	for(i = 0; i < n; i ++)
	  birthday[i] = 0;
	for(i = 0; i < n; i ++)
	  spacing[i] = 0;
	for(i = 0; i < n; i ++)
	  birthday[i] = read_random_bits(my_rng, BITS);
	// Sorting the birthdays (ugly bubble sort):
	for(i = 0; i < n; i ++)
	  for(j = i+1; j < n; j ++)
	    if(birthday[i] > birthday[j]){
	      int aux = birthday[i];
	      birthday[i] = birthday[j];
	      birthday[j] = aux;
	    }
	// Reading their spacings:
	for(i = 0; i < n; i ++)
	  if(i < n - 1)
	    spacing[i] = birthday[i+1] - birthday[i];
	  else
	    spacing[i] = birthday[0] - birthday[i];
	// Sorting the spacing:
	for(i = 0; i < n; i ++)
	  for(j = i+1; j < n; j ++)
	    if(spacing[i] > spacing[j]){
	      int aux = spacing[i];
	      spacing[i] = spacing[j];
	      spacing[j] = aux;
	    }
	// Counting the number of equal spacing:
	for(i = 1; i < n; i ++)
	  if(spacing[i] == spacing[i-1])
	    spac ++;
	equal_spacing[(spac >= 3)?(3):(spac)] ++;
      } // End of spacing test
      penalty += chi_square(4, equal_spacing, prob);
      if(penalty >= 2){
	fails ++;
	break;
      }
    }// End of three tests
  }// End of total_tests
  quality("Quality of birthday spacing test", (double) (1000 - fails) /
	  (double) 1000);
  _Wdestroy_rng(free, my_rng);
}
 
void measure_time(void){
  int i;
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed_size, seed);
  struct timeval _begin, _end;
  gettimeofday(&_begin, NULL);
  for(i = 0; i < 100000000; i ++)
    _Wrand(my_rng);
  gettimeofday(&_end, NULL);
  printf("Generated 100 million of random values in %f seconds.\n",
	 (1000000 * (_end.tv_sec - _begin.tv_sec) + _end.tv_usec -
	  _begin.tv_usec) / 1000000.0);
}

void test_serial_correlation(void){
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed_size, seed);
  int i, shift, total_tests;
  int max_fails = 0;
  const int N = 1000;
  const double min = ((-1.0) / (N - 1.0)) -
    2.0 * sqrt(((double) (N * N))/((N-1)*(N-1)*(N-2)));
  const double max = ((-1.0) / (N - 1.0)) +
    2.0 * sqrt(((double) (N * N))/((N-1)*(N-1)*(N-2)));
  for(shift = 1; shift < (N/2); shift ++){
    int fails = 0;
    for(total_tests = 0; total_tests < 1000; total_tests ++){
      double values[N];
      //double a = 0.0, b = 0.0, c = 0.0, result;
      double uv = 0.0, u = 0.0, v = 0.0, u2 = 0.0, v2 = 0.0, result;
      for(i = 0; i < N; i ++)
	values[i] = ((double) _Wrand(my_rng)) / (double) 0xffffffffffffffff;
      for(i = 0; i < N; i ++){
	uv += values[i] * values[(i + shift) % N];
	u += values[i];
	v += values[(i + shift) % N];
	u2 += values[i] * values[i];
	v2 += values[(i + shift) % N] * values[(i + shift) % N];
      }
      result = (N * uv - u * v) / sqrt((N * u2 - u * u) * (N * v2 - v * v));
      if(result < min || result > max)
	fails ++;
    }
    if(fails  > max_fails)
      max_fails = fails;
  }
  quality("Quality of serial correlation test", (double) (1000 - max_fails) /
	  (double) 1000);
  _Wdestroy_rng(free, my_rng);
}

int main(int argc, char **argv){
#if defined(W_RNG_LCG)
  printf("Starting LCG tests.\n\n");
#elif defined(W_RNG_MERSENNE_TWISTER)
  printf("Starting MERSENNE TWISTER tests.\n\n");
#elif defined(W_RNG_XORSHIRO)
  printf("Starting XORSHIRO** tests.\n\n");
#elif defined(W_RNG_PCG)
  printf("Starting PCG tests.\n\n");
#elif defined(W_RNG_CHACHA20)
  printf("Starting ChaCha20 RNG tests.\n\n");
#endif
  printf("Recommended size for seed vector: (%d-%d)\n",
	 _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE,
	 _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE);
  measure_time();
#if defined(W_RNG_MERSENNE_TWISTER)
  test_mersenne_twister();
#elif defined(W_RNG_XORSHIRO)
  test_xorshiro();
#elif defined(W_RNG_PCG)
  test_pcg();
#endif
  test_multithread();
  test_equidistribution();
  test_serial();
  test_gap();
  test_poker();
  test_collector();
  test_permutation();
  test_runs_up();
  test_maximum_of_t();
  test_collision();
  test_birthday_spacing();
  test_serial_correlation();
  imprime_resultado();
  return 0;
}
