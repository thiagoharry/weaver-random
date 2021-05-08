/*57:*/
#line 1514 "weaver-random.tex"

/*6:*/
#line 266 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
#include <pthread.h> 
#endif
#if defined(_WIN32)
#include <windows.h> 
#endif
/*:6*/
#line 1515 "weaver-random.tex"

#include "random.h"
#include <string.h>  
/*28:*/
#line 828 "weaver-random.tex"

#if defined(W_RNG_SPLITMIX) || defined(W_RNG_XOSHIRO) || defined(W_RNG_PCG) || \
  defined(W_RNG_CHACHA20)
static inline uint64_t splitmix_next(uint64_t*state,uint64_t gamma){
uint64_t ret;
/*26:*/
#line 802 "weaver-random.tex"

{
uint64_t tmp= *state;
tmp= (tmp^(tmp>>33))*0xff51afd7ed558ccdl;
tmp= (tmp^(tmp>>33))*0xc4ceb9fe1a85ec53l;
ret= tmp^(tmp>>33);
}
/*:26*/
#line 833 "weaver-random.tex"

/*27:*/
#line 817 "weaver-random.tex"

{
*state+= gamma;
}
/*:27*/
#line 834 "weaver-random.tex"

return ret;
}
#endif
/*:28*/
#line 1518 "weaver-random.tex"

/*17:*/
#line 469 "weaver-random.tex"

#ifdef W_RNG_LCG
uint64_t _Wrand(struct _Wrng*rng){
uint64_t ret;
/*9:*/
#line 307 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 473 "weaver-random.tex"

rng->last_value= 0xfa346cbfd5890825*rng->last_value+rng->c;
ret= rng->last_value;
/*10:*/
#line 320 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 476 "weaver-random.tex"

return ret;
}
#endif
/*:17*//*20:*/
#line 606 "weaver-random.tex"

#ifdef W_RNG_MERSENNE_TWISTER
uint64_t _Wrand(struct _Wrng*rng){
uint64_t ret;
/*9:*/
#line 307 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 610 "weaver-random.tex"

if(rng->offset%2==0){
/*19:*/
#line 554 "weaver-random.tex"

#ifdef __SIZEOF_INT128__
unsigned __int128 result,tmp;
uint32_t aux[4];
int i,index= rng->offset/2;
result= ((unsigned __int128*)(rng->w))[index];
result= result<<8;
result= result^((unsigned __int128*)(rng->w))[index];
i= (index+122)%156;
aux[0]= *(((uint32_t*)&(((unsigned __int128*)(rng->w))[i]))+0);
aux[1]= *(((uint32_t*)&(((unsigned __int128*)(rng->w))[i]))+1);
aux[2]= *(((uint32_t*)&(((unsigned __int128*)(rng->w))[i]))+2);
aux[3]= *(((uint32_t*)&(((unsigned __int128*)(rng->w))[i]))+3);
aux[0]= (aux[0]>>11)&0xDFFFFFEF;
aux[1]= (aux[1]>>11)&0xDDFECB7F;
aux[2]= (aux[2]>>11)&0xBFFAFFFF;
aux[3]= (aux[3]>>11)&0xBFFFFFF6;
memcpy(&tmp,aux,16);
result= result^tmp;
i= (index+156-2)%156;
result= result^(((unsigned __int128*)(rng->w))[i]>>8);
i= (index+156-1)%156;
aux[0]= *(((uint32_t*)&(((unsigned __int128*)(rng->w))[i]))+0);
aux[1]= *(((uint32_t*)&(((unsigned __int128*)(rng->w))[i]))+1);
aux[2]= *(((uint32_t*)&(((unsigned __int128*)(rng->w))[i]))+2);
aux[3]= *(((uint32_t*)&(((unsigned __int128*)(rng->w))[i]))+3);
aux[0]= (aux[0]<<18);
aux[1]= (aux[1]<<18);
aux[2]= (aux[2]<<18);
aux[3]= (aux[3]<<18);
memcpy(&tmp,aux,16);
result= result^tmp;
((unsigned __int128*)(rng->w))[index]= result;
#else
#error "Mersenne Twister unsupported without 128 bit integer support."
#endif
/*:19*/
#line 612 "weaver-random.tex"

}
ret= ((uint64_t*)(rng->w))[rng->offset];
rng->offset= (rng->offset+1)%(128*156/64);
/*10:*/
#line 320 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 616 "weaver-random.tex"

return ret;
}
#endif
/*:20*//*30:*/
#line 860 "weaver-random.tex"

#ifdef W_RNG_SPLITMIX
uint64_t _Wrand(struct _Wrng*rng){
uint64_t ret;
/*9:*/
#line 307 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 864 "weaver-random.tex"

ret= splitmix_next(&(rng->state),rng->gamma);
/*10:*/
#line 320 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 866 "weaver-random.tex"

return ret;
}
#endif
/*:30*//*37:*/
#line 1006 "weaver-random.tex"

#ifdef W_RNG_XOSHIRO
uint64_t _Wrand(struct _Wrng*rng){
uint64_t ret;
/*9:*/
#line 307 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 1010 "weaver-random.tex"

/*36:*/
#line 992 "weaver-random.tex"

{
uint64_t tmp= rng->w[1]*5;
ret= ((tmp<<7)|(tmp>>57))*9;
}
/*:36*/
#line 1011 "weaver-random.tex"

/*35:*/
#line 970 "weaver-random.tex"

{
uint64_t t= rng->w[1]<<17;
rng->w[2]^= rng->w[0];
rng->w[3]^= rng->w[1];
rng->w[1]^= rng->w[2];
rng->w[0]^= rng->w[3];
rng->w[2]^= t;
rng->w[3]= ((rng->w[3]<<45)|(rng->w[3]>>19));
}
/*:35*/
#line 1012 "weaver-random.tex"

/*10:*/
#line 320 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 1013 "weaver-random.tex"

return ret;
}
#endif
/*:37*//*44:*/
#line 1157 "weaver-random.tex"

#ifdef W_RNG_PCG
uint64_t _Wrand(struct _Wrng*rng){
uint64_t ret;
/*9:*/
#line 307 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 1161 "weaver-random.tex"

/*42:*/
#line 1125 "weaver-random.tex"

{
unsigned __int128 multiplier;
multiplier= 2549297995355413924ULL;
multiplier= multiplier<<64;
multiplier+= 4865540595714422341ULL;
rng->state= rng->state*multiplier+rng->increment;
}
/*:42*/
#line 1162 "weaver-random.tex"

/*43:*/
#line 1143 "weaver-random.tex"

{
uint64_t xorshifted,rot;
xorshifted= (((uint64_t)(rng->state>>64u))^((uint64_t)rng->state));
rot= rng->state>>122u;
ret= (xorshifted>>rot)|(xorshifted<<((-rot)&63));
}
/*:43*/
#line 1163 "weaver-random.tex"

/*10:*/
#line 320 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 1164 "weaver-random.tex"

return ret;
}
#endif
/*:44*//*55:*/
#line 1466 "weaver-random.tex"

#ifdef W_RNG_CHACHA20
/*48:*/
#line 1293 "weaver-random.tex"

#ifdef W_RNG_CHACHA20
static void chacha_padding(uint64_t input[6],uint32_t output[16]){
int i,j;
output[0]= ('e'<<24)+('x'<<16)+('p'<<8)+'a';
output[1]= ('n'<<24)+('d'<<16)+(' '<<8)+'3';
output[2]= ('2'<<24)+('-'<<16)+('b'<<8)+'y';
output[3]= ('t'<<24)+('e'<<16)+(' '<<8)+'k';
for(j= 4,i= 0;i<6;i++,j+= 2){
output[j]= (input[i]/4294967296llu);
output[j+1]= input[i]%4294967296llu;
}
}
#endif
/*:48*/
#line 1468 "weaver-random.tex"

/*49:*/
#line 1319 "weaver-random.tex"

#ifdef W_RNG_CHACHA20
void quarter_round(uint32_t*a,uint32_t*b,uint32_t*c,uint32_t*d){
*a= *a+*b;
*d= *d^*a;
*d= (*d<<16)|(*d>>16);
*c= *c+*d;
*b= *b^*c;
*b= (*b<<12)|(*b>>20);
*a= *a+*b;
*d= *d^*a;
*d= (*d<<8)|(*d>>24);
*c= *c+*d;
*b= *b^*c;
*b= (*b<<7)|(*b>>25);
}
#endif
/*:49*/
#line 1469 "weaver-random.tex"

/*50:*/
#line 1343 "weaver-random.tex"

#ifdef W_RNG_CHACHA20
void chacha_permutation(uint32_t elements[16]){
int i;
for(i= 0;i<10;i++){
quarter_round(&elements[0],&elements[4],&elements[8],&elements[12]);
quarter_round(&elements[1],&elements[5],&elements[9],&elements[13]);
quarter_round(&elements[2],&elements[6],&elements[10],&elements[14]);
quarter_round(&elements[3],&elements[7],&elements[11],&elements[15]);
quarter_round(&elements[0],&elements[5],&elements[10],&elements[15]);
quarter_round(&elements[1],&elements[6],&elements[11],&elements[12]);
quarter_round(&elements[2],&elements[7],&elements[8],&elements[13]);
quarter_round(&elements[3],&elements[4],&elements[9],&elements[14]);
}
}
#endif
/*:50*/
#line 1470 "weaver-random.tex"

uint64_t _Wrand(struct _Wrng*rng){
uint64_t ret;
/*9:*/
#line 307 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 1473 "weaver-random.tex"

if(rng->index%16==0){
rng->index= 0;
/*56:*/
#line 1494 "weaver-random.tex"

{
int i;
uint32_t padded_array[16],copied_array[16];
chacha_padding(rng->array,padded_array);
for(i= 0;i<16;i++)
copied_array[i]= padded_array[i];
chacha_permutation(padded_array);
for(i= 0;i<16;i++)
rng->generated_values[i]= padded_array[i]+copied_array[i];
}
/*:56*/
#line 1476 "weaver-random.tex"

rng->array[4]++;
}
ret= rng->generated_values[rng->index];
ret= ret<<32;
ret+= rng->generated_values[rng->index+1];
rng->index+= 2;
/*10:*/
#line 320 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 1483 "weaver-random.tex"

return ret;
}
#endif
/*:55*/
#line 1519 "weaver-random.tex"

/*14:*/
#line 416 "weaver-random.tex"

#ifdef W_RNG_LCG
struct _Wrng*_Wcreate_rng(void*(*allocator)(size_t),size_t size,
uint64_t*seed){
struct _Wrng*rng= (struct _Wrng*)allocator(sizeof(struct _Wrng));
if(rng!=NULL){

rng->last_value= ((size> 0)?(seed[0]):(0x1c3b9d10b1d41adc));
rng->c= ((size> 1)?(seed[1]):(1));
rng->last_value|= (1u);
rng->c|= (1u);
/*8:*/
#line 294 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 427 "weaver-random.tex"

/*15:*/
#line 440 "weaver-random.tex"

#if defined(W_DEBUG_RNG)
{
size_t s;
printf("DEBUG: Creating LCG Random Number Generator ");
printf("(mutiplier 0xfa346cbfd5890825)...\n");
printf("DEBUG: RNG Seed: ");
/*16:*/
#line 457 "weaver-random.tex"

for(s= 0;s<size;s++){
printf("[%llx]",seed[s]);
}
printf("\n");
/*:16*/
#line 447 "weaver-random.tex"

}
#endif
/*:15*/
#line 428 "weaver-random.tex"

}
return rng;
}
#endif
/*:14*//*21:*/
#line 631 "weaver-random.tex"

#ifdef W_RNG_MERSENNE_TWISTER
static uint32_t f1(uint32_t x){
return(x^(x>>27))*(uint32_t)1664525UL;
}
static uint32_t f2(uint32_t x){
return(x^(x>>27))*(uint32_t)1566083941UL;
}
#endif
/*:21*//*22:*/
#line 649 "weaver-random.tex"

#ifdef W_RNG_MERSENNE_TWISTER
struct _Wrng*_Wcreate_rng(void*(*allocator)(size_t),size_t size,
uint64_t*seed){
struct _Wrng*rng= (struct _Wrng*)allocator(sizeof(struct _Wrng));
if(rng!=NULL){
uint32_t*dst= (uint32_t*)(rng->w),*origin= (uint32_t*)seed;
size_t size_dst= 128*156/32,size_origin= size*2;
int count,r,i,j,mid= 306,lag= 11;

memset(rng->w,0x8b,128*156/8);
count= ((size_origin+1>=size_dst)?(size_origin+1):(size_dst));
r= f1(dst[0]^dst[mid]^dst[size_dst-1]);
dst[mid]+= r;
r+= size*2;
dst[mid+11]+= r;
dst[0]= r;
count--;
for(i= 1,j= 0;j<count&&j<size_origin;j++){
r= f1(dst[i]^dst[(i+mid)%size_dst]^
dst[(size_dst-1+i)%size_dst]);
dst[(i+mid)%size_dst]+= r;
r+= origin[j]+i;
dst[(i+mid+11)%size_dst]+= r;
dst[i]= r;
i= (i+1)%size_dst;
}
for(;j<count;j++){
r= f1(dst[i]^dst[(i+mid)%size_dst]^
dst[(i+size_dst-1)%size_dst]);
dst[(i+mid)%size_dst]+= r;
r+= i;
dst[(i+mid+11)%size_dst]+= r;
dst[i]= r;
i= (i+1)%size_dst;
}
for(j= 0;j<size_dst;j++){
r= f2(dst[i]+dst[(i+mid)%size_dst]+
dst[(i+size_dst-1)%size_dst]);
dst[(i+mid)%size_dst]^= r;
r-= i;
dst[(i+mid+lag)%size_dst]^= r;
dst[i]= r;
i= (i+1)%size_dst;
}
rng->offset= 0;
/*23:*/
#line 713 "weaver-random.tex"

{

uint32_t parity= (dst[0]&0x00000001U)^(dst[3]&0xc98e126aU);

for(i= 16;i> 0;i>>= 1)
parity^= parity>>i;
parity= parity&1;
if(parity!=1)
dst[0]= dst[0]^1;
}
/*:23*/
#line 695 "weaver-random.tex"

/*8:*/
#line 294 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 696 "weaver-random.tex"

/*24:*/
#line 731 "weaver-random.tex"

#if defined(W_DEBUG_RNG)
{
size_t s;
printf("DEBUG: Creating SFMT19937 Random Number Generator...");
printf("DEBUG: RNG Seed: ");
/*16:*/
#line 457 "weaver-random.tex"

for(s= 0;s<size;s++){
printf("[%llx]",seed[s]);
}
printf("\n");
/*:16*/
#line 737 "weaver-random.tex"

}
#endif
/*:24*/
#line 697 "weaver-random.tex"

}
return rng;
}
#endif
/*:22*//*31:*/
#line 880 "weaver-random.tex"

#ifdef W_RNG_SPLITMIX
struct _Wrng*_Wcreate_rng(void*(*allocator)(size_t),size_t size,
uint64_t*seed){
struct _Wrng*rng= (struct _Wrng*)allocator(sizeof(struct _Wrng));
if(rng!=NULL){
if(size<1)
rng->state= 0x32147198b5436569;
else
rng->state= seed[0];
if(size<2)
rng->gamma= 0x9e3779b97f4a7c15;
else
rng->gamma= (seed[1]|1);
/*8:*/
#line 294 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 894 "weaver-random.tex"

/*33:*/
#line 919 "weaver-random.tex"

#if defined(W_DEBUG_RNG)
{
size_t s;
printf("DEBUG: Creating SplitMix64 Random Number Generator...");
printf("DEBUG: RNG Seed: ");
/*16:*/
#line 457 "weaver-random.tex"

for(s= 0;s<size;s++){
printf("[%llx]",seed[s]);
}
printf("\n");
/*:16*/
#line 925 "weaver-random.tex"

}
#endif
/*:33*/
#line 895 "weaver-random.tex"

}
return rng;
}
#endif
/*:31*//*38:*/
#line 1029 "weaver-random.tex"

#ifdef W_RNG_XOSHIRO
struct _Wrng*_Wcreate_rng(void*(*allocator)(size_t),size_t size,
uint64_t*seed){
int i;
struct _Wrng*rng= (struct _Wrng*)allocator(sizeof(struct _Wrng));
if(rng!=NULL){
if(size>=4){
for(i= 0;i<4;i++)
rng->w[i]= seed[i];
}
else{
uint64_t state= 0x32147198b5436569,gamma= 0x9e3779b97f4a7c15;
for(i= 0;i<size-1;i++)
rng->w[i]= seed[i];
if(size> 1)
state= seed[i];
for(;i<4;i++)
rng->w[i]= splitmix_next(&state,gamma);
}
/*8:*/
#line 294 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 1049 "weaver-random.tex"

/*40:*/
#line 1073 "weaver-random.tex"

#if defined(W_DEBUG_RNG)
{
size_t s;
printf("DEBUG: Creating Xoshiro** Random Number Generator...");
printf("DEBUG: RNG Seed: ");
/*16:*/
#line 457 "weaver-random.tex"

for(s= 0;s<size;s++){
printf("[%llx]",seed[s]);
}
printf("\n");
/*:16*/
#line 1079 "weaver-random.tex"

}
#endif
/*:40*/
#line 1050 "weaver-random.tex"

}
return rng;
}
#endif
/*:38*//*45:*/
#line 1188 "weaver-random.tex"

#ifdef W_RNG_PCG
struct _Wrng*_Wcreate_rng(void*(*allocator)(size_t),size_t size,
uint64_t*seed){
struct _Wrng*rng= (struct _Wrng*)allocator(sizeof(struct _Wrng));
if(rng!=NULL){
if(size>=4){
unsigned __int128 multiplier;
multiplier= 2549297995355413924ULL;
multiplier= multiplier<<64;
multiplier+= 4865540595714422341ULL;
unsigned __int128 initstate= seed[0],initseq;
initstate= initstate<<64;
initstate+= seed[1];
initseq= seed[2];
initseq= initseq<<64;
initseq+= seed[3];
rng->state= 0U;
rng->increment= (initseq<<1)|1;
rng->state= rng->state*multiplier+rng->increment;
rng->state+= initstate;
rng->state= rng->state*multiplier+rng->increment;
}
else if(size>=2){
rng->state= seed[0];
rng->state= (rng->state<<64);
rng->state+= seed[1];
if(size==3){
rng->increment= seed[2];
rng->increment= (rng->increment)|1;
}
else
rng->increment= 1;
}
else{
uint64_t state,gamma= 0x9e3779b97f4a7c15;
if(size> 0)
state= seed[0];
else
state= 0x32147198b5436569;
rng->state= splitmix_next(&state,gamma);
rng->state= (rng->state<<64);
rng->state+= splitmix_next(&state,gamma);
rng->increment= 1;
}
/*8:*/
#line 294 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 1233 "weaver-random.tex"

/*47:*/
#line 1258 "weaver-random.tex"

#if defined(W_DEBUG_RNG)
{
size_t s;
printf("DEBUG: Creating PCG64 Random Number Generator...");
printf("DEBUG: RNG Seed: ");
/*16:*/
#line 457 "weaver-random.tex"

for(s= 0;s<size;s++){
printf("[%llx]",seed[s]);
}
printf("\n");
/*:16*/
#line 1264 "weaver-random.tex"

}
#endif
/*:47*/
#line 1234 "weaver-random.tex"

}
return rng;
}
#endif
/*:45*//*53:*/
#line 1406 "weaver-random.tex"

#ifdef W_RNG_CHACHA20
struct _Wrng*_Wcreate_rng(void*(*allocator)(size_t),size_t size,
uint64_t*seed){
int i,j;
struct _Wrng*rng= (struct _Wrng*)allocator(sizeof(struct _Wrng));
if(rng!=NULL){
for(i= 0;i<4;i++){
if(i<((int)size)-1||size>=4)
rng->array[i]= seed[i];
else{

uint64_t state,gamma= 0x9e3779b97f4a7c15;
if(size> 0)
state= seed[i];
else
state= 0x32147198b5436569;
for(j= i;j<4;j++)
rng->array[j]= splitmix_next(&state,gamma);
break;
}
}
rng->array[4]= 0;
if(size> 4)
rng->array[5]= seed[4];
else
rng->array[5]= 0;
rng->index= 0;
/*8:*/
#line 294 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 1434 "weaver-random.tex"

/*54:*/
#line 1447 "weaver-random.tex"

#if defined(W_DEBUG_RNG)
{
size_t s;
printf("DEBUG: Creating ChaCha20 Random Number Generator...");
printf("DEBUG: RNG Seed: ");
/*16:*/
#line 457 "weaver-random.tex"

for(s= 0;s<size;s++){
printf("[%llx]",seed[s]);
}
printf("\n");
/*:16*/
#line 1453 "weaver-random.tex"

}
#endif
/*:54*/
#line 1435 "weaver-random.tex"

}
return rng;
}
#endif
/*:53*/
#line 1520 "weaver-random.tex"

/*11:*/
#line 337 "weaver-random.tex"

bool _Wdestroy_rng(void(*free)(void*),struct _Wrng*rng){
bool ret;
#if defined(__unix__) || defined(__APPLE__)
ret= pthread_mutex_destroy(&(rng->mutex));
#elif defined(_WIN32)
DeleteCriticalSection(&(rng->mutex));
ret= true;
#endif
if(free!=NULL)
free(rng);
return ret;
}
/*:11*/
#line 1521 "weaver-random.tex"

/*:57*/
