#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "SievesShared.h"
#include "Timing.h"
#include "SieveFunctions.h"
#include "BenchmarkSieves.h"

#define DEFAULT_LIMIT 100000
#define DEFAULT_RUNCOUNT 1 
#define DEFAULT_WARMUP 0 
#define DEFAULT_PRINT_PRIMES 0

// TODO: implement arg parsing
Settings ParseArguments(int argc, char** argv){
	Settings settings = (Settings){
		.limit = DEFAULT_LIMIT,
		.runcount = DEFAULT_RUNCOUNT,
		.warmupcount = DEFAULT_WARMUP,
		.printprimes = DEFAULT_PRINT_PRIMES,
	};
	// while getopt, etc..
	return settings;
}

void PrintSettings(Settings s){
	printf("\tLIMIT\ts.limit\n");
	printf("\tRUNCOUNT\ts.runcount\n");
	printf("\tWARMUPCOUNT\ts.warmupcount\n");
	printf("\tPRINTPRIMES?\ts.printprimes\n");
}

#define GET_RESULT(bit_index) GETBIT64(result[bit_index/BITSPERBYTE], bit_index%BITSPERBYTE)
//NOTE:
//--------------------------------
//-----------TIME SHIT------------
//--------------------------------

static void RegisterAllSieves();
static void REGISTER_SIEVE(Sieve s);


static Sieve* Sieves;
static int SieveCount = 0;
static int SievesSize = SIEVES_INIT_SZ;


static int TotalRunCount = 0;

static void PrintSieveInfo(Sieve s){
	printf("%s (%p):\n\tIGNORES_EVEN:%d\n\tEXPECTS_ZALLOC:%d\n\tDESCRIPTION: %s\n",
		s.name, s.RunSieve, s.ignoreEven, s.zalloc, s.description);
}
/*
  NOTE: Ensure you add your sieves declaration to include/sieves.h   
 */
static void RegisterAllSieves(){
	Sieves = malloc(sizeof(Sieve) * SIEVES_INIT_SZ); 
	if (!Sieves) LOG_FATAL_ERROR("MALLOC", "Failed to allocate Sieves buffer.");

	// for testing nullchecks and stuff, we give a malformed one 
	REGISTER_SIEVE((Sieve){
		.RunSieve = NULL,
		.name = "FakeSieve1",
		.description = "I am a fake sieve !" });

	RegisterSieve(
		NaiveSieve,
		"Checks all 1> N <N to see if it divides N.",
		DONT_IGNORE_EVEN, EXPECTS_ZALLOC
	);
	
}

uint64_t uint64_ceildiv(uint64_t a, uint64_t b){
	// undefined for b<0
	assert(b!=0);
	return a/b + (a%b!=0);
}

void BenchmarkAllSieves(Settings settings){
	printf("Benchmarking...\n"); // make animated when i add multithreading
	for (int i = 0; i<SieveCount; i++){
		Sieve sieve = Sieves[i];
		if (sieve.RunSieve==NULL){
			fprintf(stderr, "Error! Sieve \"%s\" has invalid function ptr.\n\tSkipping...\n",sieve.name);
			sieve.isValid=false;
			continue;
		}
		

		for (int run = 0; run<settings.warmupcount; run++){
			Timespan span; timeval before, after;

			RecordTime(before);
				sieve.RunSieve(settings.limit, sieve.results[run].primes, settings.printprimes);
			RecordTime(after);

			// dont record warmups 
		}
		for (int run = 0; run<settings.runcount; run++){
			Timespan span; timeval before, after;

			RecordTime(before);
				sieve.RunSieve(settings.limit, sieve.results[run].primes, settings.printprimes);
			RecordTime(after);

			sieve.results[run].runtime = GetTimespan(before, after);
		}
	}

	printf("Benchmarking finished!\n");
	printf("Solving for %llu primes per run, %d runs (+%d warmup)\n", settings.limit, settings.runcount, settings.warmupcount);
//	printf("Sieve Name\t|Found | Total time (%d runs) | Avg runtime (%llu primes) | µs/solve| \n",TotalRunCount,settings.limit);
	printf("Sieve Name\t|Found |\n");
	for (int i = 0; i<SieveCount; i++){
		Sieve sieve = Sieves[i];
		if (sieve.isValid){
			uint64_t count = 1;
			for (uint64_t j = 0; j<settings.limit; j++){
				uint64_t* result = sieve.results[0].primes;
				if(GET_RESULT(j)){
					// TODO: something is broken here, probably in the macro
					// getting weirdly incorrect results when assesing the bitfield.
//					printf("%llu,",j);
					//					// ok nvm it just fixed 
					count++;
				}
			}
			printf("%s\t|%llu\n",sieve.name,count);
		}
	}


}

int main(int argc, char** argv){
	settings = ParseArguments(argc, argv);
	assert(settings.limit>2);
	TotalRunCount = settings.warmupcount + settings.runcount;
	RegisterAllSieves();
	BenchmarkAllSieves(settings);
	
	
	free(Sieves);
	exit(EXIT_SUCCESS);
}



// TODO: imeplement 
int ValidateResult(SieveResult r){
	return -1;
}



static void REGISTER_SIEVE(Sieve sieve){
	int N = settings.limit;
	if (SieveCount>= SievesSize){
		SievesSize *= SIEVES_RESIZE_FACTOR;
		void* resized = realloc(Sieves, SievesSize);
		if (resized == NULL){
			free(Sieves);
			LOG_FATAL_ERROR("REALLOC", "Failed to resize Sieves.");
		}
		printf("	Resize required, %d->%d\n",SievesSize/2, SievesSize);
	}
	uint64_t bitcount;
	if (sieve.ignoreEven) bitcount = (N-1ULL)/2ULL;
	else bitcount = (N-1ULL);

	uint64_t bytecount = uint64_ceildiv(bitcount, 8ULL); 
	printf("Allocated %lluB for %llu bits.\n",bytecount, bitcount);

	sieve.results = malloc(sizeof(SieveResult) * settings.runcount);
	sieve.results->primes = calloc(bytecount, 1);
	//printf("Adding sieve[%d]:\n", SieveCount);
	//PrintSieveInfo(sieve);
	Sieves[SieveCount++] = sieve;
}

