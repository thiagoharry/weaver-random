/*1:*/
#line 150 "weaver-random.tex"

#ifndef WEAVER_RANDOM
#define WEAVER_RANDOM
#ifdef __cplusplus
extern"C"{
#endif
#include <stdint.h> 
#include <stdbool.h> 
#if defined(__unix__) || defined(__APPLE__)
#include <pthread.h> 
#endif
/*51:*/
#line 1889 "weaver-random.tex"

#if !defined(W_RNG_MERSENNE_TWISTER) && !defined(W_RNG_XOSHIRO) && \
!defined(W_RNG_PCG) && !defined(W_RNG_LCG) && !defined(W_RNG_CHACHA20) && \
 !defined(W_RNG_SPLITMIX)
#if !defined(__SIZEOF_INT128__) || defined(__EMSCRIPTEN__)
#define W_RNG_XOSHIRO
#else
#define W_RNG_PCG
#endif
#endif
/*:51*/
#line 161 "weaver-random.tex"

/*13:*/
#line 401 "weaver-random.tex"

#ifdef W_RNG_LCG
struct _Wrng{
uint64_t last_value,c;
/*7:*/
#line 278 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 405 "weaver-random.tex"

};
#endif
/*:13*//*16:*/
#line 504 "weaver-random.tex"

#ifdef W_RNG_MERSENNE_TWISTER
struct _Wrng{
char w[128*156/8];
int offset;
/*7:*/
#line 278 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 509 "weaver-random.tex"

};
#endif
/*:16*//*26:*/
#line 797 "weaver-random.tex"

#ifdef W_RNG_SPLITMIX
struct _Wrng{
uint64_t state,gamma;
/*7:*/
#line 278 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 801 "weaver-random.tex"

};
#endif
/*:26*//*30:*/
#line 880 "weaver-random.tex"

#ifdef W_RNG_XOSHIRO
struct _Wrng{
uint64_t w[4];
/*7:*/
#line 278 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 884 "weaver-random.tex"

};
#endif
/*:30*//*36:*/
#line 1010 "weaver-random.tex"

#ifdef W_RNG_PCG
#ifdef __SIZEOF_INT128__
struct _Wrng{
unsigned __int128 state;
unsigned __int128 increment;
/*7:*/
#line 278 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 1016 "weaver-random.tex"

};
#else
#error "PCG unsupported without 128 bit integer support."
#endif
#endif
/*:36*//*45:*/
#line 1269 "weaver-random.tex"

#ifdef W_RNG_CHACHA20
struct _Wrng{
uint64_t array[6];
uint32_t generated_values[16];
int index;
/*7:*/
#line 278 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 1275 "weaver-random.tex"

};
#endif
/*:45*/
#line 162 "weaver-random.tex"

/*2:*/
#line 215 "weaver-random.tex"

struct _Wrng*_Wcreate_rng(void*(*alloc)(size_t),size_t size,uint64_t*seed);
/*:2*//*3:*/
#line 228 "weaver-random.tex"

/*12:*/
#line 389 "weaver-random.tex"

#ifdef W_RNG_LCG
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  1
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  2
#endif
/*:12*//*22:*/
#line 703 "weaver-random.tex"

#ifdef W_RNG_MERSENNE_TWISTER
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  1
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  312
#endif
/*:22*//*29:*/
#line 856 "weaver-random.tex"

#ifdef W_RNG_SPLITMIX
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  1
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  2
#endif
/*:29*//*35:*/
#line 994 "weaver-random.tex"

#ifdef W_RNG_XOSHIRO
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  1
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  4
#endif
/*:35*//*40:*/
#line 1136 "weaver-random.tex"

#ifdef W_RNG_PCG
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  2
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  4
#endif
/*:40*//*46:*/
#line 1288 "weaver-random.tex"

#ifdef W_RNG_CHACHA20
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  4
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  5
#endif
/*:46*/
#line 229 "weaver-random.tex"

/*:3*//*4:*/
#line 238 "weaver-random.tex"

uint64_t _Wrand(struct _Wrng*);
/*:4*//*5:*/
#line 251 "weaver-random.tex"

bool _Wdestroy_rng(void(*free)(void*),struct _Wrng*);
/*:5*/
#line 163 "weaver-random.tex"

#ifdef __cplusplus
}
#endif
#endif
/*:1*/
