#ifndef SIEVES_SHARED_H
	#define SIEVES_SHARED_H

#include <stdint.h>
#include "Timing.h"
/* too lazy to add stdbool to everything*/
typedef _Bool bool;
#define true 1
#define false 0

#define GETBIT64(x, k)  (x>>k)&1ULL

typedef struct Settings{
	uint64_t limit;
	int runcount;
	int warmupcount;
	bool printprimes;
}Settings;

Settings settings;

typedef struct SieveResult{
	uint64_t* primes;
	Timespan runtime;
}SieveResult;

#define BITSPERBYTE 8ULL

#endif 
