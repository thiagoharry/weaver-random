/*1:*/
#line 150 "weaver-random.tex"

#ifndef WEAVER_RANDOM
#define WEAVER_RANDOM
#ifdef __cplusplus
extern"C"{
#endif
#if defined(W_DEBUG_RNG)
#include <stdio.h> 
#endif
#include <stdint.h> 
#include <stdbool.h> 
#if defined(__unix__) || defined(__APPLE__)
#include <pthread.h> 
#endif
/*58:*/
#line 2056 "weaver-random.tex"

#if !defined(W_RNG_MERSENNE_TWISTER) && !defined(W_RNG_XOSHIRO) && \
!defined(W_RNG_PCG) && !defined(W_RNG_LCG) && !defined(W_RNG_CHACHA20) && \
!defined(W_RNG_SPLITMIX)
#if defined(__SIZEOF_INT128__) && defined(__linux__)
#define W_RNG_PCG
#else
#define W_RNG_SPLITMIX
#endif
#endif
/*:58*/
#line 164 "weaver-random.tex"

/*13:*/
#line 403 "weaver-random.tex"

#ifdef W_RNG_LCG
struct _Wrng{
uint64_t last_value,c;
/*7:*/
#line 280 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 407 "weaver-random.tex"

};
#endif
/*:13*//*18:*/
#line 536 "weaver-random.tex"

#ifdef W_RNG_MERSENNE_TWISTER
struct _Wrng{
char w[128*156/8];
int offset;
/*7:*/
#line 280 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 541 "weaver-random.tex"

};
#endif
/*:18*//*29:*/
#line 847 "weaver-random.tex"

#ifdef W_RNG_SPLITMIX
struct _Wrng{
uint64_t state,gamma;
/*7:*/
#line 280 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 851 "weaver-random.tex"

};
#endif
/*:29*//*34:*/
#line 947 "weaver-random.tex"

#ifdef W_RNG_XOSHIRO
struct _Wrng{
uint64_t w[4];
/*7:*/
#line 280 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 951 "weaver-random.tex"

};
#endif
/*:34*//*41:*/
#line 1105 "weaver-random.tex"

#ifdef W_RNG_PCG
#ifdef __SIZEOF_INT128__
struct _Wrng{
unsigned __int128 state;
unsigned __int128 increment;
/*7:*/
#line 280 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 1111 "weaver-random.tex"

};
#else
#error "PCG unsupported without 128 bit integer support."
#endif
#endif
/*:41*//*51:*/
#line 1371 "weaver-random.tex"

#ifdef W_RNG_CHACHA20
struct _Wrng{
uint64_t array[6];
uint32_t generated_values[16];
int index;
/*7:*/
#line 280 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 1377 "weaver-random.tex"

};
#endif
/*:51*/
#line 165 "weaver-random.tex"

/*2:*/
#line 218 "weaver-random.tex"

struct _Wrng*_Wcreate_rng(void*(*alloc)(size_t),size_t size,uint64_t*seed);
/*:2*//*3:*/
#line 231 "weaver-random.tex"

/*12:*/
#line 391 "weaver-random.tex"

#ifdef W_RNG_LCG
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  1
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  2
#endif
/*:12*//*25:*/
#line 752 "weaver-random.tex"

#ifdef W_RNG_MERSENNE_TWISTER
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  1
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  312
#endif
/*:25*//*32:*/
#line 907 "weaver-random.tex"

#ifdef W_RNG_SPLITMIX
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  1
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  2
#endif
/*:32*//*39:*/
#line 1062 "weaver-random.tex"

#ifdef W_RNG_XOSHIRO
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  1
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  4
#endif
/*:39*//*46:*/
#line 1247 "weaver-random.tex"

#ifdef W_RNG_PCG
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  2
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  4
#endif
/*:46*//*52:*/
#line 1390 "weaver-random.tex"

#ifdef W_RNG_CHACHA20
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  4
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  5
#endif
/*:52*/
#line 232 "weaver-random.tex"

/*:3*//*4:*/
#line 240 "weaver-random.tex"

uint64_t _Wrand(struct _Wrng*);
/*:4*//*5:*/
#line 253 "weaver-random.tex"

bool _Wdestroy_rng(void(*free)(void*),struct _Wrng*);
/*:5*/
#line 166 "weaver-random.tex"

#ifdef __cplusplus
}
#endif
#endif
/*:1*/
