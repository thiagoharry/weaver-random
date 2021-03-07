/*43:*/
#line 1221 "weaver-random.tex"

/*6:*/
#line 273 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
#include <pthread.h> 
#endif
#if defined(_WIN32)
#include <windows.h> 
#endif
/*:6*/
#line 1222 "weaver-random.tex"

#include "random.h"
#include <string.h>  
/*15:*/
#line 443 "weaver-random.tex"

#ifdef W_RNG_LCG
uint64_t _Wrand(struct _Wrng*rng){
uint64_t ret;
/*9:*/
#line 314 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 447 "weaver-random.tex"

rng->last_value= 0xfa346cbfd5890825*rng->last_value+rng->c;
ret= rng->last_value;
/*10:*/
#line 327 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 450 "weaver-random.tex"

return ret;
}
#endif
/*:15*//*19:*/
#line 580 "weaver-random.tex"

#ifdef W_RNG_MERSENNE_TWISTER
uint64_t _Wrand(struct _Wrng*rng){
uint64_t ret;
/*9:*/
#line 314 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 584 "weaver-random.tex"

if(rng->offset%2==0){
/*18:*/
#line 532 "weaver-random.tex"

#ifdef __SIZEOF_INT128__
unsigned __int128 result,tmp;
uint32_t aux[4];
int i,index= rng->offset/2;
result= ((unsigned __int128*)(rng->w))[index];
result= result<<8;
result= result^((unsigned __int128*)(rng->w))[index];
i= (index+122)%_N;
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
i= (index+_N-2)%_N;
result= result^(((unsigned __int128*)(rng->w))[i]>>8);
i= (index+_N-1)%_N;
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
/*:18*/
#line 586 "weaver-random.tex"

}
ret= ((uint64_t*)(rng->w))[rng->offset];
rng->offset= (rng->offset+1)%(_W*_N/64);
/*10:*/
#line 327 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 590 "weaver-random.tex"

return ret;
}
#endif
/*:19*//*25:*/
#line 745 "weaver-random.tex"

#ifdef W_RNG_XORSHIRO
uint64_t _Wrand(struct _Wrng*rng){
uint64_t ret;
/*9:*/
#line 314 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 749 "weaver-random.tex"

uint64_t t= rng->w[1]<<17;
/*24:*/
#line 733 "weaver-random.tex"

{
uint64_t tmp= rng->w[1]*5;
ret= ((tmp<<7)|(tmp>>57))*9;
}
/*:24*/
#line 751 "weaver-random.tex"


rng->w[2]^= rng->w[0];
rng->w[3]^= rng->w[1];
rng->w[1]^= rng->w[2];
rng->w[0]^= rng->w[3];
rng->w[2]^= t;
rng->w[3]= ((rng->w[3]<<45)|(rng->w[3]>>19));
/*10:*/
#line 327 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 759 "weaver-random.tex"

return ret;
}
#endif
/*:25*//*31:*/
#line 892 "weaver-random.tex"

#ifdef W_RNG_PCG
static uint64_t pcg_rand32(uint64_t*state){
const uint64_t initseq= 0xd6b9e1860000218e;
uint64_t inc= (initseq<<1)|1;
uint64_t oldstate= *state,value,rot;
*state= *state*6364136223846793005ull+inc;
value= ((oldstate>>18u)^oldstate)>>27u;
rot= oldstate>>59u;
return(value>>rot)|(value<<((-rot)&31));
}
#endif
/*:31*//*34:*/
#line 978 "weaver-random.tex"

#ifdef W_RNG_PCG
uint64_t _Wrand(struct _Wrng*rng){
unsigned __int128 multiplier;
uint64_t ret,xorshifted,rot;
multiplier= 2549297995355413924ULL;
multiplier= multiplier<<64;
multiplier+= 4865540595714422341ULL;
/*9:*/
#line 314 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 986 "weaver-random.tex"


rng->state= rng->state*multiplier+rng->increment;
xorshifted= (((uint64_t)(rng->state>>64u))^((uint64_t)rng->state));
rot= rng->state>>122u;
ret= (xorshifted>>rot)|(xorshifted<<((-rot)&63));
/*10:*/
#line 327 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 992 "weaver-random.tex"

return ret;
}
#endif
/*:34*//*41:*/
#line 1175 "weaver-random.tex"

#ifdef W_RNG_CHACHA20
/*35:*/
#line 1024 "weaver-random.tex"

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
/*:35*/
#line 1177 "weaver-random.tex"

/*36:*/
#line 1048 "weaver-random.tex"

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
/*:36*/
#line 1178 "weaver-random.tex"

/*37:*/
#line 1070 "weaver-random.tex"

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
/*:37*/
#line 1179 "weaver-random.tex"

uint64_t _Wrand(struct _Wrng*rng){
uint64_t ret;
/*9:*/
#line 314 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_lock(&(rng->mutex));
#endif
#if defined(_WIN32)
EnterCriticalSection(&(rng->mutex));
#endif
/*:9*/
#line 1182 "weaver-random.tex"

if(rng->index%16==0){
rng->index= 0;
/*42:*/
#line 1202 "weaver-random.tex"

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
/*:42*/
#line 1185 "weaver-random.tex"

rng->array[4]++;
}
ret= rng->generated_values[rng->index];
ret= ret<<32;
ret+= rng->generated_values[rng->index+1];
rng->index+= 2;
/*10:*/
#line 327 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_unlock(&(rng->mutex));
#endif
#if defined(_WIN32)
LeaveCriticalSection(&(rng->mutex));
#endif
/*:10*/
#line 1192 "weaver-random.tex"

return ret;
}
#endif
/*:41*/
#line 1225 "weaver-random.tex"

/*14:*/
#line 420 "weaver-random.tex"

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
#line 301 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 431 "weaver-random.tex"

}
return rng;
}
#endif
/*:14*//*20:*/
#line 604 "weaver-random.tex"

#ifdef W_RNG_MERSENNE_TWISTER
static uint32_t f1(uint32_t x){
return(x^(x>>27))*(uint32_t)1664525UL;
}
static uint32_t f2(uint32_t x){
return(x^(x>>27))*(uint32_t)1566083941UL;
}

#endif
/*:20*//*21:*/
#line 626 "weaver-random.tex"

#ifdef W_RNG_MERSENNE_TWISTER
struct _Wrng*_Wcreate_rng(void*(*allocator)(size_t),size_t size,
uint64_t*seed){
struct _Wrng*rng= (struct _Wrng*)allocator(sizeof(struct _Wrng));
if(rng!=NULL){
uint32_t*dst= (uint32_t*)(rng->w),*origin= (uint32_t*)seed;
size_t size_dst= _W*_N/32,size_origin= size*2;
int count,r,i,j,mid= 306,lag= 11;

memset(rng->w,0x8b,_W*_N/8);
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
/*22:*/
#line 689 "weaver-random.tex"

{

uint32_t parity= (dst[0]&0x00000001U)^(dst[3]&0xc98e126aU);

for(i= 16;i> 0;i>>= 1)
parity^= parity>>i;
parity= parity&1;
if(parity!=1)
dst[0]= dst[0]^1;
}
/*:22*/
#line 672 "weaver-random.tex"

/*8:*/
#line 301 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 673 "weaver-random.tex"

}
return rng;
}
#endif
/*:21*//*27:*/
#line 788 "weaver-random.tex"

#ifdef W_RNG_XORSHIRO
struct _Wrng*_Wcreate_rng(void*(*allocator)(size_t),size_t size,
uint64_t*seed){
int i;
struct _Wrng*rng= (struct _Wrng*)allocator(sizeof(struct _Wrng));
if(rng!=NULL){
for(i= 0;i<4;i++){
if(i<size)
rng->w[i]= seed[i];
else{
if(i==0)
rng->w[i]= 0;
else{
uint64_t z;
rng->w[i-1]+= 0x9e3779b97f4a7c15;
z= rng->w[i-1];
z= (z^(z>>30))*0xbf58476d1ce4e5b9;
z= (z^(z>>27))*0x94d049bb133111eb;
rng->w[i]= z^(z>>31);
}
}
}
/*8:*/
#line 301 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 811 "weaver-random.tex"

}
return rng;
}
#endif
/*:27*//*30:*/
#line 872 "weaver-random.tex"

#ifdef W_RNG_PCG


static void pcg_init32(uint64_t seed,uint64_t*state){
const uint64_t initseq= 0xd6b9e1860000218e;
uint64_t inc= (initseq<<1)|1;
*state= 0;
*state= *state*6364136223846793005ull+inc;
*state+= seed;
*state= *state*6364136223846793005ull+inc;
}
#endif
/*:30*//*32:*/
#line 912 "weaver-random.tex"

#ifdef W_RNG_PCG
struct _Wrng*_Wcreate_rng(void*(*allocator)(size_t),size_t size,
uint64_t*seed){
struct _Wrng*rng= (struct _Wrng*)allocator(sizeof(struct _Wrng));
unsigned __int128 initstate,initseq,multiplier;
if(rng!=NULL){
if(size> 0)
initstate= seed[0];
else
initstate= 0xf1168ab461218bb5;
initstate= initstate<<64;
if(size> 1)
initstate+= seed[1];
else{
uint64_t state,value;
pcg_init32(seed[0],&state);
value= pcg_rand32(&state);
value= value<<32;
value+= pcg_rand32(&state);
initstate+= value;;
}
if(size> 2)
initseq= seed[2];
else
initseq= 0x4ecbb136926970a4;
initseq= initseq<<64;
if(size> 3)
initseq+= seed[3];
else
initseq+= 0x25b22530f844f87b;

multiplier= 2549297995355413924ULL;
multiplier= multiplier<<64;
multiplier+= 4865540595714422341ULL;
rng->state= 0;
rng->increment= (initseq<<1)|1;
rng->state= rng->state*multiplier+rng->increment;
rng->state+= initstate;
rng->state= rng->state*multiplier+rng->increment;
/*8:*/
#line 301 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 952 "weaver-random.tex"

}
return rng;
}
#endif
/*:32*//*40:*/
#line 1132 "weaver-random.tex"

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
#line 301 "weaver-random.tex"

#if defined(__unix__) || defined(__APPLE__)
pthread_mutex_init(&(rng->mutex),NULL);
#endif
#if defined(_WIN32)
InitializeCriticalSection(&(rng->mutex));
#endif
/*:8*/
#line 1161 "weaver-random.tex"

}
return rng;
}
#endif
/*:40*/
#line 1226 "weaver-random.tex"

/*11:*/
#line 341 "weaver-random.tex"

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
#line 1227 "weaver-random.tex"

/*:43*/
