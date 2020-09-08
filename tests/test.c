#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <bsd/stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#if defined(_WIN32)
#include <windows.h>
#endif
#if defined(__unix__) || defined(__APPLE__)
#include <pthread.h>
#endif

#include "../src/random.h"

int numero_de_testes = 0, acertos = 0, falhas = 0;

#ifdef W_RNG_MERSENNE_TWISTER
#include "sfmt.c"
#elif defined(W_RNG_XORSHIRO)
#include "xorshiro.c"
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
  _Wdestroy_rng(my_rng);
  free(my_rng);
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
  _Wdestroy_rng(my_rng);
  free(my_rng);
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
  _Wdestroy_rng(my_rng1);
  free(my_rng1);
  _Wdestroy_rng(my_rng2);
  free(my_rng2);
}

///////////////////////////// Simulates a 6-side dice rolling
static uint32_t last_value_returned = 0;
static int bits_consummed = 32, accumulator = 0, bits_accumulated = 0;
int roll_dice(struct _Wrng *rng){
  int ret;
  for(;;){ // Try until you return 1, 2, 3, 4, 5 or 6
    if(bits_consummed >= 30){ // Less than 3 bits remaining
      if(bits_consummed == 32) // Use the remaining bits
	ret = 0;
      else
	ret = (last_value_returned >> bits_consummed);
      last_value_returned = _Wrand(rng); // Get new value
      //arc4random_buf(&last_value_returned, 4);
      ret = (ret << (32 - bits_consummed));
      ret += (last_value_returned % (3 - (32 - bits_consummed)));
      bits_consummed = (3 - (32 - bits_consummed)); // Read what you need
    }
    else{
      ret = (last_value_returned >> bits_consummed) % 8;
      bits_consummed += 3; // Just read 3 bits (number between 0 and 8)
    }
    if(ret < 6){ // If 0, 1, 2, 3, 4, 5, then it's a dice value
      return ret + 1;
    }
    else{ // Otherwise, its 6 or 7. This give us 1 bit to accumulate
      accumulator = (accumulator << 1) + (ret % 2);
      bits_accumulated ++;
      if(bits_accumulated == 3){ // With 3 accumulated bits: a new value
	ret = accumulator;
	accumulator = 0;
	bits_accumulated = 0;
	if(ret < 6){ // If 0, 1, 2, 3, 4, 5, then it's a dice value
	  return ret + 1;
	}
	else{ // Otherwise it's 6 or 7. The accumulator have only 1 bit now
	  accumulator = ret % 2;
	  bits_accumulated = 1;
	}
      }
    }
  }
}

void test_chi_square1(void){
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed);  
  int j, k;
  int success, total = 10000;
  int penalty = 0;
  success = 0;
  for(k = 0; k < total; k ++){
    penalty = 0;
    for(j = 0; j < 3; j ++){
      int counter[6];
      int inv_prob[6] = {6, 6, 6, 6, 6, 6};
      int i, sum = 0;
      double V;
      for(i = 0; i < 6; i ++)
	counter[i] = 0;
      for(i = 0; i < 30; i ++)
	counter[roll_dice(my_rng) - 1] ++;
      for(i = 0; i < 6; i ++)
	sum += (counter[i] * counter[i] * inv_prob[i]);
      V = sum / 30.0;
      V -= 30.0;
      //printf("V=%f\n", V);
      if(V < 0.5543 || V > 15.09){
	penalty += 2;
	break;
      }
      else if(V < 1.1415 || V > 11.07){
	penalty ++;
	if(penalty >= 2)
	  break;
      }
    }
    if(penalty < 2)
      success ++;
    //assert("Chi-square test simulating 1 dice", (penalty < 2));
  }
  quality("Quality of chi-square test simulating 1 dice", (double) success / (double) total);
  success = 0;
  for(k = 0; k < total; k ++){
    penalty = 0;
    for(j = 0; j < 3; j ++){
      int counter[11];
      double mult[11] = {36.0, 18.0, 12.0, 9.0, 7.2, 6.0, 7.2, 9.0, 12.0, 18.0, 36.0};
      int i;
      double V, sum = 0;
      for(i = 0; i < 11; i ++)
	counter[i] = 0;
      for(i = 0; i < 180; i ++)
	counter[(roll_dice(my_rng) + roll_dice(my_rng)) - 2] ++;
      for(i = 0; i < 11; i ++){
	sum += (mult[i] * (counter[i] * counter[i]));
      }
      V = sum / 180.0;
      V -= 180.0;
      //printf("V=%f\n", V);
      if(V < 2.558 || V > 23.21){
	penalty += 2;
	break;
      }
      else if(V < 3.940 || V > 18.31){
	penalty ++;
	if(penalty >= 2)
	  break;
      }
    }
    if(penalty < 2)
      success ++;
  }
  quality("Quality of chi-square test simulating 2 dices", (double) success / (double) total);
  success = 0;
  for(k = 0; k < total; k ++){
    penalty = 0;
    for(j = 0; j < 3; j ++){
      int counter[16];
      double mult[16] = {216.0, 72.0, 36.0, 21.6, 14.4, 10.285714285714286,
			 8.64, 8.0, 8.0, 8.64, 10.285714285714286, 14.4, 21.6,
			 36.0, 72.0, 216.0};
      int i, sum = 0;
      double V;
      for(i = 0; i < 16; i ++)
	counter[i] = 0;
      for(i = 0; i < 1080; i ++)
	counter[(roll_dice(my_rng) + roll_dice(my_rng) + roll_dice(my_rng))
		- 3] ++;
      for(i = 0; i < 16; i ++)
	sum += (mult[i] * (counter[i] * counter[i]));
      V = sum / 1080.0;
      V -= 1080.0;
      //printf("V=%f\n", V);
      //for(i = 0; i < 16; i ++)
      //  printf(" %d ", counter[i]);
      //printf("\n");
      if(V < 5.229 || V > 30.58){
	penalty += 2;
	break;
      }
      else if(V < 7.261 || V > 25.00){
	penalty ++;
	if(penalty >= 2)
	  break;
      }
    }
    if(penalty < 2)
      success ++;
  }
  quality("Quality of chi-square test simulating 3 dices", (double) success / (double) total);
  _Wdestroy_rng(my_rng);
  free(my_rng);
}

int main(int argc, char **argv){
  initialize_seed();
#if defined(W_RNG_MERSENNE_TWISTER)
  printf("Starting MERSENNE TWISTER tests. Seed: %lu\n\n", (long unsigned int) seed);
  test_mersenne_twister();
#elif defined(W_RNG_XORSHIRO)
  printf("Starting XORSHIRO** tests. Seed: %lu\n\n", (long unsigned int) seed);
  test_xorshiro();
#endif
  test_multithread();
  test_chi_square1();
  imprime_resultado();
  return 0;
}
