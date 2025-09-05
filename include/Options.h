#ifndef OPTIONS_H
#define OPTIONS_H

#include <getopt.h>
#include "BenchmarkSieves.h"

#define OPT_UNKNOWN ((char)'?')
#define OPT_MISSING_ARG ((char)':')
#define OPT_EMPTY (-1)
#define MAX_OPTIONS 64 // at this point, something has gone horribly wrong.
void strToUpper(char* );

static char *ExecutablePath = "./BenchmarkSieves";



typedef struct OptionWrapper {
	char shortopt;
	char *longopt;
	bool argRequired;
	uint64_t Default; // might be annoying if i want non integer options
	uint64_t *settingptr;
	char *description;
} Option_t;

void ParseArguments(int argc, char** argv);
void RegisterOption(Option_t o);
struct option long_options[MAX_OPTIONS];

char UsageString[1024];
Option_t Options[MAX_OPTIONS];


void PRINTBIN32(void* vp, bool bigEndian);

void PRINTBIN64(uint64_t x, bool bigEndian);

#endif // OPTIONS_H
