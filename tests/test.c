#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <bsd/stdlib.h> 
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
char seed[W_RNG_SEED_SIZE];
#endif

void initialize_seed(void){
  arc4random_buf(seed, W_RNG_SEED_SIZE);
}

/* Início dos Testes */
#if defined(W_RNG_MERSENNE_TWISTER)
void test_mersenne_twister(void){
  bool equal = true;
  int i;
  struct SFMT_T ref_rng;
  struct _Wrng *my_rng = _Wcreate_rng(malloc, seed);
  sfmt_init_by_array(&ref_rng, (uint32_t *) seed, W_RNG_SEED_SIZE / 4);
  for(i = 0; i < 1000; i ++){
    uint64_t a, b;
    a = _Wrand(my_rng);
    b = sfmt_genrand_uint64(&ref_rng);
    printf("%lu %lu\n", a, b);
    if(a != b){
      equal = false;
      break;
    }
  }
  assert("Mersenne Twister generate same numbers as in reference", equal);
  free(my_rng);
}
#endif


 
int main(int argc, char **argv){
  initialize_seed();
  printf("Starting tests.\n\n");
#if defined(W_RNG_MERSENNE_TWISTER)
  test_mersenne_twister();
#endif
  imprime_resultado();
  return 0;
}