/*1:*/
#line 136 "weaver-random_en.cweb"

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
#line 2002 "weaver-random_en.cweb"

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
#line 150 "weaver-random_en.cweb"

/*13:*/
#line 408 "weaver-random_en.cweb"

#ifdef W_RNG_LCG
struct _Wrng{
uint64_t last_value,c;
/*7:*/
#line 271 "weaver-random_en.cweb"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 412 "weaver-random_en.cweb"

};
#endif
/*:13*//*18:*/
#line 536 "weaver-random_en.cweb"

#ifdef W_RNG_MERSENNE_TWISTER
struct _Wrng{
char w[128*156/8];
int offset;
/*7:*/
#line 271 "weaver-random_en.cweb"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 541 "weaver-random_en.cweb"

};
#endif
/*:18*//*29:*/
#line 834 "weaver-random_en.cweb"

#ifdef W_RNG_SPLITMIX
struct _Wrng{
uint64_t state,gamma;
/*7:*/
#line 271 "weaver-random_en.cweb"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 838 "weaver-random_en.cweb"

};
#endif
/*:29*//*34:*/
#line 931 "weaver-random_en.cweb"

#ifdef W_RNG_XOSHIRO
struct _Wrng{
uint64_t w[4];
/*7:*/
#line 271 "weaver-random_en.cweb"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 935 "weaver-random_en.cweb"

};
#endif
/*:34*//*41:*/
#line 1086 "weaver-random_en.cweb"

#ifdef W_RNG_PCG
#ifdef __SIZEOF_INT128__
struct _Wrng{
unsigned __int128 state;
unsigned __int128 increment;
/*7:*/
#line 271 "weaver-random_en.cweb"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 1092 "weaver-random_en.cweb"

};
#else
#error "PCG unsupported without 128 bit integer support."
#endif
#endif
/*:41*//*51:*/
#line 1346 "weaver-random_en.cweb"

#ifdef W_RNG_CHACHA20
struct _Wrng{
uint64_t array[6];
uint32_t generated_values[16];
int index;
/*7:*/
#line 271 "weaver-random_en.cweb"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 1352 "weaver-random_en.cweb"

};
#endif
struct _Wsecure_rng{
uint64_t array[6];
uint32_t generated_values[16];
int index;
/*7:*/
#line 271 "weaver-random_en.cweb"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_t mutex;
#endif
#if defined(_WIN32)
CRITICAL_SECTION mutex;
#endif
/*:7*/
#line 1359 "weaver-random_en.cweb"

};
/*:51*/
#line 151 "weaver-random_en.cweb"

/*2:*/
#line 203 "weaver-random_en.cweb"

struct _Wrng*_Wcreate_rng(void*(*alloc)(size_t),size_t size,uint64_t*seed);
struct _Wsecure_rng*_Wcreate_secure_rng(void*(*alloc)(size_t),size_t size,
uint64_t*seed);
/*:2*//*3:*/
#line 219 "weaver-random_en.cweb"

/*12:*/
#line 396 "weaver-random_en.cweb"

#ifdef W_RNG_LCG
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  1
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  2
#endif
/*:12*//*25:*/
#line 747 "weaver-random_en.cweb"

#ifdef W_RNG_MERSENNE_TWISTER
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  1
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  312
#endif
/*:25*//*32:*/
#line 893 "weaver-random_en.cweb"

#ifdef W_RNG_SPLITMIX
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  1
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  2
#endif
/*:32*//*39:*/
#line 1043 "weaver-random_en.cweb"

#ifdef W_RNG_XOSHIRO
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  1
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  4
#endif
/*:39*//*46:*/
#line 1224 "weaver-random_en.cweb"

#ifdef W_RNG_PCG
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  2
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  4
#endif
/*:46*//*52:*/
#line 1372 "weaver-random_en.cweb"

#ifdef W_RNG_CHACHA20
#define _W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE  4
#define _W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE  5
#endif
/*:52*/
#line 220 "weaver-random_en.cweb"

/*:3*//*4:*/
#line 228 "weaver-random_en.cweb"

uint64_t _Wrand(struct _Wrng*);
uint64_t _Wsecure_rand(struct _Wsecure_rng*);
/*:4*//*5:*/
#line 243 "weaver-random_en.cweb"

bool _Wdestroy_rng(void(*free)(void*),struct _Wrng*);
bool _Wdestroy_secure_rng(void(*free)(void*),struct _Wsecure_rng*);
/*:5*/
#line 152 "weaver-random_en.cweb"

#ifdef __cplusplus
}
#endif
#endif
/*:1*/
