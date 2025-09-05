#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "SievesShared.h"
#include "Timing.h"
#include "SieveFunctions.h"
#include "BenchmarkSieves.h"
#include "Options.h"

#define PR_DEBUG
#include "Logging.h"


Settings settings = (Settings) {
	.limit 		= 1000,
	 .runc		= 1,
	  .warmupc	= 0,
	   .printprimes	= false,
	    .evalrtc	= false,
	     .verbose	= false,
};


static void RegisterSieves();
static void BenchmarkSieves();
static void PrintBenchmarkResults();
static void CleanupSieves();

static void REGISTER_SIEVE(Sieve sieve);


static Sieve *Sieves;
static int SieveCount = 0;
static int SievesSize = SIEVES_INIT_SZ;


int main(int argc, char** argv) {
	log_debug("Debug logs enabled!\n");
	ParseArguments(argc, argv);
	log_verbose("Verbose output selected!\n");
	RegisterSieves();
	BenchmarkSieves();
	PrintBenchmarkResults();
	CleanupSieves();

	exit(EXIT_SUCCESS);
}


static void RegisterSieves() {
	Sieves = malloc(sizeof(Sieve) * SIEVES_INIT_SZ);
	if (!Sieves) log_fatal("MALLOC - Failed to allocate Sieves buffer.");

	RegisterSieve(
	    NaiveSieve,
	    "Checks all i<N && i>2 to see if it divides N.",
	    DONT_IGNORE_EVEN,
	    EXPECTS_ZALLOC
	);

}

uint64_t uint64_ceildiv(uint64_t a, uint64_t b) {
	assert(b != 0); // undefined for b<0
	return a / b + (a % b != 0);
}

void BenchmarkSieves() {
	// do some checks to make sure settings are valid.
	assert(settings.limit > 2);

	log_debug("BENCHMARKING...\n"); // make animated when I add multithreading
	for (int i = 0; i < SieveCount; i++) {
		Sieve sieve = Sieves[i];
		if (sieve.RunSieve == NULL) {
			log_warning("Sieve \"%s\" has null function ptr.\n\tSkipping...\n", sieve.name);
			sieve.isValid = false;
			continue;
		}


		// DO WARMUPS
		for (int run = 0; run < settings.warmupc; run++) {
			wc_timeval before, after;
			RecordTime(before);
			sieve.RunSieve(settings.limit, sieve.results[run].primes, settings.printprimes);
			RecordTime(after);
		}

		// DO REAL RUNS
		for (int run = 0; run < settings.runc; run++) {
			wc_timeval before, after;

			RecordTime(before);
			sieve.RunSieve(settings.limit, sieve.results[run].primes, settings.printprimes);
			RecordTime(after);

			sieve.results[run].runtime = GetTimespan(before, after);
		}
	}

	log_debug("BENCHMARKING FINISHED!\n");


}

void PrintBenchmarkResults() {
	log_default("Solving for primes up to N=%llu. %llu runs (+%llu warmup)\n", settings.limit, settings.runc, settings.warmupc);

	for (int i = 0; i < SieveCount; i++) {
		Sieve sieve = Sieves[i];
		log_default("SIEVE [%d] = %s:\n", i, sieve.name);
		if (sieve.isValid) {
		}
	}
}



// called by function style macro RegisterSieve() for convinience, i.e auto set name
// *INDENT-OFF*
static void REGISTER_SIEVE(Sieve sieve) {
	if (strlen(sieve.name) > SIEVE_MAX_NAME_LEN){
		log_fatal_cleanup(CleanupSieves, "Sieve name: '%.*s...' is too long! (%d>%d) ",
		    SIEVE_MAX_NAME_LEN, sieve.name,strlen(sieve.name), SIEVE_MAX_NAME_LEN);
	}
	if (strlen(sieve.description) > SIEVE_MAX_DESC_LEN){
		log_fatal_cleanup(CleanupSieves, "Sieve description belonging to '%s' is too long! (%d>%d) ",
		    sieve.name, strlen(sieve.description), SIEVE_MAX_DESC_LEN);
	}

	int N = settings.limit;
	if (SieveCount >= SievesSize) {
		SievesSize *= SIEVES_RESIZE_FACTOR;
		void *resized = realloc(Sieves, SievesSize);
		if (resized == NULL) log_fatal_cleanup(CleanupSieves, "REALLOC - Failed to resize Sieves.");
		log_debug("\n\tResize required, %d->%d\n", SievesSize / 2, SievesSize);
	}
	uint64_t bitcount;
	if (sieve.ignoreEven) bitcount = (N - 1ULL) / 2ULL;
	else bitcount = (N - 1ULL);

//	uint64_t bytecount = uint64_ceildiv(bitcount, 8ULL);
	uint64_t bytecount = (bitcount / 8ULL) + (bitcount / 8ULL) % 8ULL;
	// bytecount should be divisible by 8 and also >bitcount/8
	log_debug("allocating %lluB for will be used to store the required %llu bits.\n", bytecount, bitcount);

	sieve.results = malloc(sizeof(SieveResult) * settings.runc);
	sieve.results->primes = calloc(bytecount, 1);
	sieve.results[0].numIntegersInPrimes = bytecount / 8ULL;
	log_debug("Adding sieve[%d]:\n", SieveCount);
	//PrintSieveInfo(sieve);
	Sieves[SieveCount++] = sieve;
}

static void CleanupSieves() {
	free(Sieves);
}
