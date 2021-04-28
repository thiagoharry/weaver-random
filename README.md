# weaver-random

A Random Number Generator API written in literary programming.

Read the code [here (in portuguese)](https://github.com/thiagoharry/weaver-random/blob/master/weaver-random.pdf) or 
[here (in english)](https://github.com/thiagoharry/weaver-random/blob/master/weaver-random_en.pdf)

This random number generator API is self contained in the files present in src/ folder. To use it, just put these files in your project.

This random number generator code was created to be used even if you use non-standard memory managers. 
You can choose which allocation and disallocation function it uses. It is also designed to be flexible about
which random number algorithm it uses. Changing the algorithm is as easy as defining a new macro. And you also can 
initialize it using seeds with arbitrary size.

This implementation is thread-safe and is expected to compile correctly and run in Linux, OpenBSD, Windows and Web Assembly.

# Functions

This API defines the following functions:

* struct _Wrng *_Wcreate_rng(void *(*alloc)(size_t), size_t size, uint64_t *seed)

This creates a new random generator struct. You will need to pass this struct to the function that produces new random numbers.

The first argument 'alloc' is an allocation function. It can be `malloc' from the standard library if you are not using a custom memory manager.

The second argument is the size of the seed. The quantity of 64-bit numbers that compose our seed.

The third argument  is the seed: an array composed by 64-bit numbers. You should pass values chosen uniformly at random. In environments
where it is hard to generate initial random numbers, you could pass a single 64-bit number. Usually it is not necessary to pass more than 4 or 5
numbers in the seed, even if you have lots of initial randomness. Even cryptographic applications use at most four or five 64-bit numbers.

For example, you could initialize our random number generator using:

```c
uint64_t seed = {0x32147198b5436569, 0x260287febfeb34e9, 0x0b6cc94a91a265e4};
struct _Wrng *my_rng = _Wcreate_rng(malloc, sizeof(seed)/sizeof(uint64_t), seed);
```

If tou want suggestions about the recommended seed size, you can check macros `_W_RNG_MINIMUM_RECOMMENDED_SEED_SIZE' and 
`_W_RNG_MAXIMUM_RECOMMENDED_SEED_SIZE'. 

The minimum recommended size is the number recommended in the reference implementation
for the algorithm that we are using. If no minimum recommended size exist in the reference implementation, then we
use the minimum size needed to initialize the generator initial state without using another RNG to stretch the initial randomness.

If you pass a seed greater than the maximum recommended size, the algorithm will ignore some of your initial randomness.

* uint64_t _Wrand(struct _Wrng *rng)

This function produces a new pseudo-random number with 64-bits given a RNG struct produced with the previous function.

* bool _Wdestroy_rng(void (*free)(void *), struct _Wrng *rng)

This function disallocates the random number generator struct and closes any file or mutex associated with it.
You should use this function after using your generator.
Do not try to generate new random numbers after calling this function using your generator.

The first argument is the disallocation function. If you are not using a custom memory manager, you should pass `free'. 
You can pass `NULL' if you do not want to disallocate the struct. Perhaps your custom memory manager will take care
of the memory without needing to explicitly disallocate.

The second argument is the random number struct.

# Algorithms

If you just use this API, it will automatically choose some random number generator algorithm requiring a minimal
quality. The quality required is what we see as adequate to a non-serious video-game: it is more important
being fast than being completely unpredictable to a very determined attacker. If you want to know which algorithm
the API is choosing, this is printed in the screen if you define macro `W_DEBUG_RNG'. If you want to force
some specific algorithm, you can define one of the following macros:

* W_RNG_LCG: The faster option. A Linear Congruent Generator. This choice is if you want something faster than
 the default choice at the cost of having less quality and being more predictable.
* W_RNG_MERSENNE_TWISTER: The SFMT19937 random number generator.
* W_RNG_SPLITMIX: The SplitMix64 random number generator.
* W_RNG_PCG: The PCG64 random number generator.
* W_RNG_XOSHIRO: The Xoshiro** random number generator.
* W_RNG_CHACHA20: The ChaCha20 random number generator. If you really want
 the greatest possible quality and unpredictability, even at the cost of being between 2x and 10x slower than the default choice,
 this is the algorithm.
