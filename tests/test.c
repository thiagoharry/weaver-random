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
#endif

size_t count_utf8_code_points(const char *s) {
    size_t count = 0;
    while (*s) {
        count += (*s++ & 0xC0) != 0x80;
    }
    return count;
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

#if defined(W_RNG_MERSENNE_TWISTER)
uint32_t seed;
#endif

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
  int penalty = 0;
  int j;
  for(j = 0; j < 3; j ++){
    int counter[6];
    int inv_prob[6] = {6, 6, 6, 6, 6, 6};
    int i, sum = 0;
    double V;
    for(i = 0; i < 6; i ++)
      counter[i] = 0;
    for(i = 0; i < 600; i ++)
      counter[roll_dice(my_rng) - 1] ++;
    for(i = 0; i < 6; i ++)
      sum += (counter[i] * counter[i] * inv_prob[i]);
    V = sum / 600.0;
    V -= 600.0;
    if(V < 0.8721 || V > 16.81){
      penalty += 2;
      break;
    }
    else if(V < 1.635 || V > 12.59){
      penalty ++;
      if(penalty >= 2)
	break;
    }
  }
  assert("Chi-square test simulating 1 dice", (penalty < 2));
  penalty = 0;
  for(j = 0; j < 3; j ++){
    int counter[11];
    int inv_prob[11] = {36, 18, 12, 9, 36, 6, 36, 9, 12, 18, 36};
    int inv_deno[11] = {1, 1, 1, 1, 5, 1, 5, 1, 1, 1, 1};
    int i, sum = 0;
    double V;
    for(i = 0; i < 11; i ++)
      counter[i] = 0;
    for(i = 0; i < 360; i ++)
      counter[(roll_dice(my_rng) + roll_dice(my_rng)) - 2] ++;
    for(i = 0; i < 11; i ++)
      sum += ((counter[i] * counter[i] * inv_prob[i]) / inv_deno[i]);
    V = sum / 360.0;
    V -= 360.0;
    if(V < 3.053 || V > 24.72){
      penalty += 2;
      break;
    }
    else if(V < 4.575 || V > 19.68){
      penalty ++;
      if(penalty >= 2)
	break;
    }
  }
  assert("Chi-square test simulating 2 dices", (penalty < 2));
  penalty = 0;
  for(j = 0; j < 3; j ++){
    int counter[16];
    int inv_deno[16] = {1, 3, 6, 10, 15, 21, 25, 27, 27, 25, 21, 15, 10, 6,
			3, 1};
    int i, sum = 0;
    double V;
    for(i = 0; i < 16; i ++)
      counter[i] = 0;
    for(i = 0; i < 2160; i ++)
      counter[(roll_dice(my_rng) + roll_dice(my_rng) + roll_dice(my_rng))
	      - 3] ++;
    for(i = 0; i < 16; i ++)
      sum += ((counter[i] * counter[i] * 216) / inv_deno[i]);
    V = sum / 2160.0;
    V -= 2160.0;
    //printf("V=%f\n", V);
    //for(i = 0; i < 16; i ++)
    //  printf(" %d ", counter[i]);
    //printf("\n");
    if(V < 5.812 || V > 32.00){
      penalty += 2;
      break;
    }
    else if(V < 7.962 || V > 26.30){
      penalty ++;
      if(penalty >= 2)
	break;
    }
  }
  assert("Chi-square test simulating 3 dices", (penalty < 2));
}

int main(int argc, char **argv){
  initialize_seed();
  printf("Starting tests. Seed: %lu\n\n", (long unsigned int) seed);
#if defined(W_RNG_MERSENNE_TWISTER)
  test_mersenne_twister();
#endif
  printf("The following tests should fail rarely: \n");
  test_chi_square1();
  imprime_resultado();
  return 0;
}
