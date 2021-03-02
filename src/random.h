/*1:*/
#line 154 "weaver-random.tex"

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
/*44:*/
#line 1662 "weaver-random.tex"

#if !defined(W_RNG_MERSENNE_TWISTER) && !defined(W_RNG_XORSHIRO) && \
!defined(W_RNG_PCG) && !defined(W_RNG_LCG) && !defined(W_RNG_CHACHA20)
#ifdef __SIZEOF_INT128__
#define W_RNG_MERSENNE_TWISTER
#else
#define W_RNG_PCG
#endif
#endif
/*:44*/
#line 165 "weaver-random.tex"

/*16:*/
#line 494 "weaver-random.tex"

#ifdef W_RNG_MERSENNE_TWISTER
#define _W 128 
#define _N 156 
#endif
/*:16*/
#line 166 "weaver-random.tex"

/*13:*/
#line 407 "weaver-random.tex"

#ifdef W_RNG_LCG
struct _Wrng{
uint64_t last_value,c;
/*7:*/
#line 287 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 411 "weaver-random.tex"

};
#endif
/*:13*//*17:*/
#line 513 "weaver-random.tex"

#ifdef W_RNG_MERSENNE_TWISTER
struct _Wrng{
char w[_W*_N/8];
int offset;

/*7:*/
#line 287 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 519 "weaver-random.tex"

};
#endif
/*:17*//*26:*/
#line 772 "weaver-random.tex"

#ifdef W_RNG_XORSHIRO
struct _Wrng{
uint64_t w[4];
/*7:*/
#line 287 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 776 "weaver-random.tex"

};
#endif
/*:26*//*29:*/
#line 839 "weaver-random.tex"

#ifdef W_RNG_PCG
#ifdef __SIZEOF_INT128__
struct _Wrng{
unsigned __int128 state;
unsigned __int128 increment;
/*7:*/
#line 287 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 845 "weaver-random.tex"

};
#else
#error "PCG unsupported without 128 bit integer support."
#endif
#endif
/*:29*//*38:*/
#line 1097 "weaver-random.tex"

#ifdef W_RNG_CHACHA20
struct _Wrng{
uint64_t array[6];
uint32_t generated_values[16];
int index;
/*7:*/
#line 287 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 1103 "weaver-random.tex"

};
#endif
/*:38*/
#line 167 "weaver-random.tex"

/*2:*/
#line 224 "weaver-random.tex"

struct _Wrng*_Wcreate_rng(void*(*alloc)(size_t),size_t size,uint64_t*seed);
/*:2*//*3:*/
#line 237 "weaver-random.tex"

/*12:*/
#line 395 "weaver-random.tex"

#ifdef W_RNG_LCG
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  1
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  2
#endif
/*:12*//*23:*/
#line 712 "weaver-random.tex"

#ifdef W_RNG_MERSENNE_TWISTER
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  1
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  312
#endif
/*:23*//*28:*/
#line 823 "weaver-random.tex"

#ifdef W_RNG_XORSHIRO
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  1
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  4
#endif
/*:28*//*33:*/
#line 965 "weaver-random.tex"

#ifdef W_RNG_PCG
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  2
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  4
#endif
/*:33*//*39:*/
#line 1116 "weaver-random.tex"

#ifdef W_RNG_CHACHA20
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  4
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  5
#endif
/*:39*/
#line 238 "weaver-random.tex"

/*:3*//*4:*/
#line 247 "weaver-random.tex"

uint64_t _Wrand(struct _Wrng*);
/*:4*//*5:*/
#line 260 "weaver-random.tex"

bool _Wdestroy_rng(void(*free)(void*),struct _Wrng*);
/*:5*/
#line 168 "weaver-random.tex"

#ifdef __cplusplus
}
#endif
#endif
/*:1*/
