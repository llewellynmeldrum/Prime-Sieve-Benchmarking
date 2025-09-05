
#include "Globals.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "Options.h"

#define PR_DEBUG
#include "Logging.h"
#include <getopt.h>

// private
void ConcatUsageString(Option_t o, char* usage_string);
void AddToOptString(Option_t o);
int optionCount = 0;

char optstring[128] = ":";
void strToUpper(char* str) {
	int i = 0;
	char ch = str[i];
	while(ch != '\0') {
		ch = str[i];
		str[i++] = toupper(ch);
	}

}
void ConcatUsageString(Option_t o, char* usage_string) {
	// Option struct is still small enough to fit in 99% of cache lines (its 40B atm),
	// if it was like >128B maybe passing ptr is worth
	char formatted_usage_line[512];
	char *longopt_UPPER = malloc(sizeof(char) * (strlen(o.longopt) +1));
	strcpy(longopt_UPPER, o.longopt);
	strToUpper(longopt_UPPER);

	int err;
	if (o.argRequired == no_argument) {
		err = sprintf(formatted_usage_line,
		              "\t-%c, --%s\t%s\tDEFAULT:",
		              o.shortopt, o.longopt, o.description);
		if (o.Default == 0) {
			strcat(formatted_usage_line, " false\n");
		} else {
			strcat(formatted_usage_line, " true\n");
		}
	} else {
		err = sprintf(formatted_usage_line,
		              "\t-%c, --%s=%s\t%s\tDEFAULT: %llu",
		              o.shortopt, o.longopt, longopt_UPPER, o.description, o.Default);
		if (o.Default == 0) {
			strcat(formatted_usage_line, " (NONE)\n");
		} else {
			strcat(formatted_usage_line, "\n");
		}
	}
	if (err <= 0) {
		fprintf(stderr, "ERROR: less than 1 char written to format string, check ConcatUsageString for Option '%s'.\n", o.longopt);
		fprintf(stderr, "Exiting...\n");
		free(longopt_UPPER);
		exit(EXIT_FAILURE);
	}
	int addlen = strlen(formatted_usage_line);
	int curlen = strlen(usage_string);
	if (curlen + addlen >= USAGE_MAX_LEN) {
		fprintf(stderr, "ERROR: Option %s's usage data cannot fit in UsageString.\n", o.longopt);
		fprintf(stderr, "Exiting...\n");
		free(longopt_UPPER);
		exit(EXIT_FAILURE);
	}

	free(longopt_UPPER);
	strcat(usage_string, formatted_usage_line);
}

void AddToOptString(Option_t o) {
	char optstring_tok[3];
	if (o.argRequired == required_argument) {
		sprintf(optstring_tok, "%c:", o.shortopt);
	} else if (o.argRequired == no_argument) {
		sprintf(optstring_tok, "%c", o.shortopt);
	}
	strcat(optstring, optstring_tok);

}



void RegisterOption(Option_t o) {

	if(o.settingptr != NULL) { // exclude '--help' like options from g
		o.Default = *o.settingptr; // defaults are assigned to the settings value on settings init
		long_options[optionCount] = (struct option) {
			o.longopt, o.argRequired, NULL, o.shortopt
		};
		Options[optionCount++] = o;
	}
	AddToOptString(o);
	ConcatUsageString(o, UsageString);

}

void PrintUsage() {
	fprintf(stderr, "Usage:\n %s [opt=arg...] | [opt]\n"
	                "\n"
	                "Options:\n%s\n", ExecutablePath, UsageString);
}

void RegisterOptions() {
	RegisterOption((Option_t) {
		.shortopt = 'l',
		.longopt = "limit",
		.argRequired = true,
		.settingptr = &settings.limit,
		.Default = settings.limit,
		.description = "Limit sieve to this num. \t ",
	});

	RegisterOption((Option_t) {
		.shortopt = 'r',
		.longopt = "runc",
		.argRequired = true,
		.settingptr = &settings.runc,
		.Default = settings.runc,
		.description = "\tNumber of real runs to complete.",
	});

	RegisterOption((Option_t) {
		.shortopt = 'w',
		.longopt = "warmupc",
		.argRequired = true,
		.settingptr = &settings.warmupc,
		.Default = settings.warmupc,
		.description = "Number of warmup runs to complete.",
	});


	RegisterOption((Option_t) {
		.shortopt = 'p',
		.longopt = "printprimes",
		.settingptr = &settings.printprimes,
		.Default = settings.printprimes,
		.description = "Prints each prime that is found.",
	});

	RegisterOption((Option_t) {
		.shortopt = 'e',
		.longopt = "evalrtc",
		.settingptr = &settings.evalrtc,
		.Default = settings.evalrtc,
		.description = "\tEstimate O(?) for each sieve.\t",
	});

	RegisterOption((Option_t) {
		.shortopt = 'v',
		.longopt = "verbose",
		.argRequired = 0,
		.settingptr = &settings.verbose,
		.Default = settings.verbose,
		.description = "\tIncrease verbosity of output.\t",
	});

	RegisterOption((Option_t) {
		.shortopt = 'h',
		.longopt = "help",
		.Default = false,
		.description = "\tDisplays this menu.\t\t",
	});
}



int SearchOptionIndex(char opt) {
	// optionCount-1 to ignore "help" the last option
	for (int i = 0; i < optionCount ; i++) {
		Option_t o = Options[i];
		if (opt != o.shortopt) {
		} else {
			return i;
		}
	}
	// we should not reach this. reaching here means the earlier switch statement failed
	// to detect an unknown option, we just didnt find it in Options
	log_fatal("Options array contains something not defined in optstr\n");
	return 0;
}

char pop_option_char(int argc, char** argv) {
	return getopt_long(argc, argv, optstring, long_options, 0);
}

#define OPT_ARG optopt
void ParseArguments(int argc, char** argv) {
	log_debug("argc=%d\n", argc);
	log_debug("argv:\n");
	for (int i = 0; i < argc ; i++)
		log_debug("[%d]='%s'\n", i, argv[i]);
	RegisterOptions();

	char opt_ch;
	while ( (opt_ch = pop_option_char(argc, argv)) != OPT_EMPTY) {
		switch (opt_ch) { // handle special cases first
		case 'h':
			PrintUsage();
			exit(EXIT_SUCCESS);
			break;

		case OPT_MISSING_ARG:
			log_fatal_cleanup(PrintUsage, "Option '-%c' expects argument.\n", OPT_ARG);
			break;

		case OPT_UNKNOWN:
			log_fatal_cleanup(PrintUsage, "Unknown option supplied: '-%c'\n", OPT_ARG);
			break;

		default:
			break;

		}
		int i = SearchOptionIndex(opt_ch);
		Option_t foundOption = Options[i];
		log_debug("foundOption=-%c (%s)", foundOption.shortopt, foundOption.longopt);
		if (foundOption.argRequired) {
			// skip first char if it's '=' (--arg=69)
			uint64_t arg = atoll(optarg + (optarg[0] == '='));
			log_debug_noprefix(" --> set to : %llu\n", arg);
			*foundOption.settingptr = arg;
		} else {
			log_debug_noprefix(" --> set to : true\n");
			*foundOption.settingptr = true;
		}


	}
}

