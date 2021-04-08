/*50:*/
#line 1394 "weaver-random.tex"

/*6:*/
#line 264 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
#include <pthread.h> 
#endif
#if defined(_WIN32)
#include <windows.h> 
#endif
/*:6*/
#line 1395 "weaver-random.tex"

#include "random.h"
#include <string.h>  
/*25:*/
#line 779 "weaver-random.tex"

#if defined(W_RNG_SPLITMIX) || defined(W_RNG_XOSHIRO) || defined(W_RNG_PCG)
static inline uint64_t splitmix_next(uint64_t*state,uint64_t gamma){
uint64_t ret;
/*23:*/
#line 753 "weaver-random.tex"

{
uint64_t tmp= *state;
tmp= (tmp^(tmp>>33))*0xff51afd7ed558ccdl;
tmp= (tmp^(tmp>>33))*0xc4ceb9fe1a85ec53l;
ret= tmp^(tmp>>33);
}
/*:23*/
#line 783 "weaver-random.tex"

/*24:*/
#line 768 "weaver-random.tex"

{
*state+= gamma;
}
/*:24*/
#line 784 "weaver-random.tex"

return ret;
}
#endif
/*:25*/
#line 1398 "weaver-random.tex"

/*15:*/
#line 437 "weaver-random.tex"

#ifdef W_RNG_LCG
uint64_t _Wrand(struct _Wrng*rng){
uint64_t ret;
/*9:*/
#line 305 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 441 "weaver-random.tex"

rng->last_value= 0xfa346cbfd5890825*rng->last_value+rng->c;
ret= rng->last_value;
/*10:*/
#line 318 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 444 "weaver-random.tex"

return ret;
}
#endif
/*:15*//*18:*/
#line 574 "weaver-random.tex"

#ifdef W_RNG_MERSENNE_TWISTER
uint64_t _Wrand(struct _Wrng*rng){
uint64_t ret;
/*9:*/
#line 305 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 578 "weaver-random.tex"

if(rng->offset%2==0){
/*17:*/
#line 522 "weaver-random.tex"

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
/*:17*/
#line 580 "weaver-random.tex"

}
ret= ((uint64_t*)(rng->w))[rng->offset];
rng->offset= (rng->offset+1)%(128*156/64);
/*10:*/
#line 318 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 584 "weaver-random.tex"

return ret;
}
#endif
/*:18*//*27:*/
#line 810 "weaver-random.tex"

#ifdef W_RNG_SPLITMIX
uint64_t _Wrand(struct _Wrng*rng){
uint64_t ret;
/*9:*/
#line 305 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 814 "weaver-random.tex"

ret= splitmix_next(&(rng->state),rng->gamma);
/*10:*/
#line 318 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 816 "weaver-random.tex"

return ret;
}
#endif
/*:27*//*33:*/
#line 939 "weaver-random.tex"

#ifdef W_RNG_XOSHIRO
uint64_t _Wrand(struct _Wrng*rng){
uint64_t ret;
/*9:*/
#line 305 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 943 "weaver-random.tex"

/*32:*/
#line 925 "weaver-random.tex"

{
uint64_t tmp= rng->w[1]*5;
ret= ((tmp<<7)|(tmp>>57))*9;
}
/*:32*/
#line 944 "weaver-random.tex"

/*31:*/
#line 903 "weaver-random.tex"

{
uint64_t t= rng->w[1]<<17;
rng->w[2]^= rng->w[0];
rng->w[3]^= rng->w[1];
rng->w[1]^= rng->w[2];
rng->w[0]^= rng->w[3];
rng->w[2]^= t;
rng->w[3]= ((rng->w[3]<<45)|(rng->w[3]>>19));
}
/*:31*/
#line 945 "weaver-random.tex"

/*10:*/
#line 318 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 946 "weaver-random.tex"

return ret;
}
#endif
/*:33*//*39:*/
#line 1075 "weaver-random.tex"

#ifdef W_RNG_PCG
uint64_t _Wrand(struct _Wrng*rng){
uint64_t ret;
/*9:*/
#line 305 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 1079 "weaver-random.tex"

/*37:*/
#line 1043 "weaver-random.tex"

{
unsigned __int128 multiplier;
multiplier= 2549297995355413924ULL;
multiplier= multiplier<<64;
multiplier+= 4865540595714422341ULL;
rng->state= rng->state*multiplier+rng->increment;
}
/*:37*/
#line 1080 "weaver-random.tex"

/*38:*/
#line 1061 "weaver-random.tex"

{
uint64_t xorshifted,rot;
xorshifted= (((uint64_t)(rng->state>>64u))^((uint64_t)rng->state));
rot= rng->state>>122u;
ret= (xorshifted>>rot)|(xorshifted<<((-rot)&63));
}
/*:38*/
#line 1081 "weaver-random.tex"

/*10:*/
#line 318 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 1082 "weaver-random.tex"

return ret;
}
#endif
/*:39*//*48:*/
#line 1348 "weaver-random.tex"

#ifdef W_RNG_CHACHA20
/*42:*/
#line 1197 "weaver-random.tex"

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
/*:42*/
#line 1350 "weaver-random.tex"

/*43:*/
#line 1221 "weaver-random.tex"

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
/*:43*/
#line 1351 "weaver-random.tex"

/*44:*/
#line 1243 "weaver-random.tex"

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
/*:44*/
#line 1352 "weaver-random.tex"

uint64_t _Wrand(struct _Wrng*rng){
uint64_t ret;
/*9:*/
#line 305 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 1355 "weaver-random.tex"

if(rng->index%16==0){
rng->index= 0;
/*49:*/
#line 1375 "weaver-random.tex"

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
/*:49*/
#line 1358 "weaver-random.tex"

rng->array[4]++;
}
ret= rng->generated_values[rng->index];
ret= ret<<32;
ret+= rng->generated_values[rng->index+1];
rng->index+= 2;
/*10:*/
#line 318 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 1365 "weaver-random.tex"

return ret;
}
#endif
/*:48*/
#line 1399 "weaver-random.tex"

/*14:*/
#line 414 "weaver-random.tex"

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
#line 292 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 425 "weaver-random.tex"

}
return rng;
}
#endif
/*:14*//*19:*/
#line 599 "weaver-random.tex"

#ifdef W_RNG_MERSENNE_TWISTER
static uint32_t f1(uint32_t x){
return(x^(x>>27))*(uint32_t)1664525UL;
}
static uint32_t f2(uint32_t x){
return(x^(x>>27))*(uint32_t)1566083941UL;
}
#endif
/*:19*//*20:*/
#line 617 "weaver-random.tex"

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
/*21:*/
#line 680 "weaver-random.tex"

{

uint32_t parity= (dst[0]&0x00000001U)^(dst[3]&0xc98e126aU);

for(i= 16;i> 0;i>>= 1)
parity^= parity>>i;
parity= parity&1;
if(parity!=1)
dst[0]= dst[0]^1;
}
/*:21*/
#line 663 "weaver-random.tex"

/*8:*/
#line 292 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 664 "weaver-random.tex"

}
return rng;
}
#endif
/*:20*//*28:*/
#line 830 "weaver-random.tex"

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
#line 292 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 844 "weaver-random.tex"

}
return rng;
}
#endif
/*:28*//*34:*/
#line 962 "weaver-random.tex"

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
#line 292 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 982 "weaver-random.tex"

}
return rng;
}
#endif
/*:34*//*40:*/
#line 1106 "weaver-random.tex"

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
#line 292 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 1151 "weaver-random.tex"

}
return rng;
}
#endif
/*:40*//*47:*/
#line 1305 "weaver-random.tex"

#ifdef W_RNG_CHACHA20
struct _Wrng*_Wcreate_rng(void*(*allocator)(size_t),size_t size,
uint64_t*seed){
int i;
struct _Wrng*rng= (struct _Wrng*)allocator(sizeof(struct _Wrng));
if(rng!=NULL){
for(i= 0;i<4;i++){
if(i<size)
rng->array[i]= seed[i];
else{
if(i==0)
rng->array[i]= 0;
else{
uint64_t z;
rng->array[i-1]+= 0x9e3779b97f4a7c15;
z= rng->array[i-1];
z= (z^(z>>30))*0xbf58476d1ce4e5b9;
z= (z^(z>>27))*0x94d049bb133111eb;
rng->array[i]= z^(z>>31);
}
}
}
rng->array[4]= 0;
if(size> 4)
rng->array[5]= seed[4];
else
rng->array[5]= 0;
rng->index= 0;
/*8:*/
#line 292 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 1334 "weaver-random.tex"

}
return rng;
}
#endif
/*:47*/
#line 1400 "weaver-random.tex"

/*11:*/
#line 335 "weaver-random.tex"

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
#line 1401 "weaver-random.tex"

/*:50*/
