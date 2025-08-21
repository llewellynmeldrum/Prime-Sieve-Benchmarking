#ifndef BENCHMARK_SIEVES_H
	#define BENCHMARK_SIEVES_H
#include <stdint.h>
#include "SievesShared.h"

#define DONT_IGNORE_EVEN 0
#define IGNORE_EVEN 1

#define EXPECTS_ZALLOC 0
#define EXPECTS_1ALLOC 1

#define SIEVES_INIT_SZ 2
#define SIEVES_RESIZE_FACTOR 2

#define PRIMES10000_PATH "primes10000.txt"
#define PRIME_PER_LINE 10

// function=which function returned this value?
#define LOG_FATAL_ERROR(function, ...)\
	{fprintf(stderr, "\n\033[1;33;41mFATAL ERROR:\033[0m\n\tIN: %s\n\t", function);\
	fprintf(stderr, __VA_ARGS__);\
	fprintf(stderr, "\nExiting...\n");\
	exit(EXIT_FAILURE);}



uint64_t validPrimes[10000]; 


typedef struct Sieve{
	bool isValid;
	void (*RunSieve)(int N, uint64_t * primes, bool printPrimes);		// sieve function.
	char name[128];                                         // Used when printing results (or debug/verbose mode)
	char description[256];                                  // Used in debug/verbose mode.
        bool ignoreEven;                                        // See ## BITARRAY for details.
//	bool threadSafe;                                        // Implement later
        bool zalloc;                                            // whether the sieve expects a 0 allocated or 1 allocaeted array.
	SieveResult* results;
}Sieve;

#define RegisterSieve(function, desc, ignore_even, zero_alloc)\
	REGISTER_SIEVE((Sieve){\
		.isValid = true,\
		.RunSieve = function,\
		.name = #function,\
		.description = desc,\
		.ignoreEven =ignore_even,\
		.zalloc= zero_alloc\
	})
#endif
