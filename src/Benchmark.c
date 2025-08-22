#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <getopt.h>

#include "SievesShared.h"
#include "Timing.h"
#include "SieveFunctions.h"
#include "BenchmarkSieves.h"


#define DEFAULT_LIMIT 1000 
#define DEFAULT_RUNCOUNT 1 
#define DEFAULT_WARMUP 0 
#define DEFAULT_PRINT_PRIMES 0
#define DEFAULT_EVALUATE_RUNTIME_COMPLEXITY 0
#define DEFAULT_VERBOSE 0

// opt stuff
static char ExecutablePath[32];




static int optionCount = 0;
static char UsageString[1024];

typedef struct OptionWrapper{
	char ch;
	char* longopt;
	int argSpec; // 0 = no, 1 = required, 2 = optional
	char * description;

	uint64_t Default; // might be annoying if i want non integer options
	uint64_t* settingptr;
}Option;

static Option Options[MAX_OPTIONS];

void InitOptions(){
	UsageString[0] = '\0';
}

void RegisterOption(Option o);

char optstring[128] = ":";
void test(){
}


void AddToUsageString(Option o, char* usage_string){
	// Option struct is still small enough to fit in 99% of cache lines (its 40B atm), 
	// if it was like >128B maybe passing ptr is worth
	char formatted_usage_line[512];
	char* longopt_UPPER = malloc(sizeof(char) * (strlen(o.longopt)+1));
	strcpy(longopt_UPPER, o.longopt);
	strtou(longopt_UPPER);
	
	int err = sprintf(formatted_usage_line,
		   "\t-%c, --%s=%s\t%s\tDEFAULT=%llu\n", 
		   o.ch, o.longopt, longopt_UPPER, o.description, o.Default);
	if (err<=0){
		fprintf(stderr, "ERROR: less than 1 char written to format string, check AddToUsageString for Option '%s'.\n",o.longopt);
		fprintf(stderr, "Exiting...\n");
		free(longopt_UPPER);
		exit(EXIT_FAILURE);
	}
	int addlen = strlen(formatted_usage_line);
	int curlen = strlen(usage_string);
	if (curlen + addlen >= USAGE_MAX_LEN){
		fprintf(stderr, "ERROR: Option %s's usage data cannot fit in UsageString.\n",o.longopt);
		fprintf(stderr, "Exiting...\n");
		free(longopt_UPPER);
		exit(EXIT_FAILURE);
	}

	free(longopt_UPPER);
	strcat(usage_string, formatted_usage_line);
}

void AddToOptString(Option o){
	char optstring_tok[3];
	if (o.argSpec==required_argument){
		sprintf(optstring_tok, "%c:", o.ch);
	} else if (o.argSpec==optional_argument){
		sprintf(optstring_tok, "%c::", o.ch);
	} else if (o.argSpec==no_argument){
		sprintf(optstring_tok, "%c", o.ch);
	}
	strcat(optstring, optstring_tok);
}


struct option long_options[MAX_OPTIONS];

void RegisterOption(Option o){
	if (o.argSpec<0 || o.argSpec>2){
		fprintf(stderr, "ERROR: Option %s.argSpec is invalid.\n",o.longopt);
		fprintf(stderr, "Exiting...\n");
		exit(EXIT_FAILURE);
	}
	if (o.settingptr==NULL){
		fprintf(stderr, "ERROR: Undefined/null ptr to setting value, %s is invalid.\n",o.longopt);
		fprintf(stderr, "Exiting...\n");
		exit(EXIT_FAILURE);
	}

	AddToUsageString(o, UsageString);
	AddToOptString(o);
	long_options[optionCount] = (struct option){o.longopt, o.argSpec, NULL, o.ch};

	Options[optionCount++] = o;
}

void PrintUsage(){
	fprintf(stderr, "Usage:\n %s [opt=arg...] | [opt]\n"
		"\n"
		"Options:\n%s\n", ExecutablePath, UsageString);
}

Settings ParseArguments(int argc, char** argv){
	Settings settings = (Settings){
		.limit = DEFAULT_LIMIT,
		.runcount = DEFAULT_RUNCOUNT,
		.warmupcount = DEFAULT_WARMUP,
		.printprimes = DEFAULT_PRINT_PRIMES,
		.EvaluateRuntimeComplexity = DEFAULT_EVALUATE_RUNTIME_COMPLEXITY,
		.verbose = DEFAULT_VERBOSE,
	};

	RegisterOption((Option){
		.ch = 'l',
		.longopt = "limit",
		.argSpec = required_argument,
		.description = "Specify the number at which to stop sieving",
		.settingptr = &settings.limit,
		.Default = DEFAULT_LIMIT,
	});
	//
	
	// first char of optstring being a colon enables distinguishing between invalid options and missing argument errors.

	// 1. A colon (not including the first one) indicates that the preceding option REQUIRES an argument.
	// 2. Two colons indicates that the preceding option MAY take an argument.
	
	// Options THAT DONT REQUIRE an argument can be supplied in the forms, in any order.
	//	 shortOpt | longOpt
	//	 -o	  | --option 
	
	// Arguments can be supplied to certain options. This can be done in the following ways:
	//	shortOpt | longOpt
	//	-o 10	 | --option 10
	//	-o=10	 | --option=10
	// Only arguments whose optstring entry is proceeded by a ":" (mandatory) or a "::" (optional) may be supplied an argument.
	

	char ch; int optionIndex = 0;
        while ((ch = getopt_long(argc, argv, optstring, long_options, &optionIndex)) != -1) {
		bool foundOption = false;
		if (ch==OPTION_MISSING_ARG){
			fprintf(stderr, "Error: MISSING OPTION ARGUMENT, option %c/%s requires an argument. \n",
				long_options[optionIndex].val, long_options[optionIndex].name);
			PrintUsage();
			exit(EXIT_FAILURE);
		}

		for (int i = 0; i<optionCount; i++){
			Option o = Options[i];
			if (ch == o.ch){
				foundOption=true;
				printf("Option %c found, long_options[%d]=%s\n", ch, optionIndex, long_options[optionIndex].name);
				uint64_t val = 1; // if we are talking about a boolean thats been found, it will be TRUE/1
				if (o.argSpec == required_argument){
					printf("\tOptarg:%s\n", optarg);
					val = atoi(optarg);
				}
				uint64_t default_val = o.Default;
				*o.settingptr = val; // set the settings val to the arg
				printf("settings.limit=%llu\n",settings.limit);
				break;
			}
		}

		if (ch==OPTION_NOT_FOUND){
			if (foundOption == false){
				fprintf(stderr, "Error: unknown option supplied (%c)!\n", optopt); 
				exit(EXIT_FAILURE);
			} else {
				// very weird, because ch!=option not found but our loop missed it.
				fprintf(stderr, "Error: unknown option supplied (%c), but somehow exists in Options[].\n", optopt); 
				exit(EXIT_FAILURE);
			}
		}
        }
        return settings;
}

void PrintSettings(Settings s){
	printf("\tLIMIT\ts.limit\n");
	printf("\tRUNCOUNT\ts.runcount\n");
	printf("\tWARMUPCOUNT\ts.warmupcount\n");
	printf("\tPRINTPRIMES?\ts.printprimes\n");
}

#define GET_RESULT(bit_index) GETBIT64(result[bit_index/BITSPERBYTE], bit_index%BITSPERBYTE)

static void RegisterAllSieves();
static void REGISTER_SIEVE(Sieve s);


static Sieve* Sieves;
static int SieveCount = 0;
static int SievesSize = SIEVES_INIT_SZ;



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
		"Checks all i<N && i>2 to see if it divides N.",
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
	sprintf(ExecutablePath, "%s", argv[0]);
	settings = ParseArguments(argc, argv);

	exit(EXIT_SUCCESS);
	assert(settings.limit>2);
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

