#ifndef BENCHMARK_SIEVES_H
#define BENCHMARK_SIEVES_H
#include <stdint.h>
#include "SievesShared.h"

#define USAGE_MAX_LEN 2048


typedef struct Settings {
	uint64_t limit;
	uint64_t runc;
	uint64_t warmupc;
	uint64_t printprimes;
	uint64_t evalrtc;
	uint64_t verbose;
} Settings;




#define DONT_IGNORE_EVEN 0
#define IGNORE_EVEN 1

#define EXPECTS_ZALLOC 0
#define EXPECTS_1ALLOC 1

#define SIEVES_INIT_SZ 2
#define SIEVES_RESIZE_FACTOR 2


// function=which function returned this value?
#define LOG_FATAL_ERROR(function, ...)\
	{fprintf(stderr, "\n\033[1;33;41mFATAL ERROR:\033[0m\n\tIN: %s\n\t", function);\
	fprintf(stderr, __VA_ARGS__);\
	fprintf(stderr, "\nExiting...\n");\
	exit(EXIT_FAILURE);}


typedef struct SieveResult {
	uint64_t *primes;
	uint64_t numIntegersInPrimes;
	Timespan runtime;
} SieveResult;



#define SIEVE_MAX_NAME_LEN 32
#define SIEVE_MAX_DESC_LEN 128
typedef struct Sieve {
	bool isValid;
	void (*RunSieve)(int N, uint64_t * primes, bool printPrimes);		// sieve function.
	char name[SIEVE_MAX_NAME_LEN];                                         // Used when printing results (or debug/verbose mode)
	char description[SIEVE_MAX_DESC_LEN];                                  // Used in debug/verbose mode.
	bool ignoreEven;                                        // See ## BITARRAY for details.
//	bool threadSafe;                                        // Implement later
	bool zalloc;                                            // whether the sieve expects a 0 allocated or 1 allocaeted array.
	SieveResult *results;		// an array of results
} Sieve;


#define RegisterSieve(function, desc, ignore_even, zero_alloc)do{\
	REGISTER_SIEVE(\
		(Sieve){\
		.isValid = true,\
		.RunSieve = function,\
		.name = #function,\
		.description = desc,\
		.ignoreEven =ignore_even,\
		.zalloc= zero_alloc\
		}\
	);\
	}while(0)
#endif
