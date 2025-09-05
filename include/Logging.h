#ifndef KLL_LOGS
#define KLL_LOGS

#include <stdio.h>

#define NOP do{ } while(0) // safe noop, accounts for unbraced control flow

#define MAX_PREFIX_LEN 64
#define MAX_INDENT_LEN 32
#define MAX_POSTFIX_LEN 128
#define MAX_BODY_LEN 2048

typedef enum {
	DEBUG = -1,
	DEFAULT = 0,
	WARNING = 1,
	FATAL = 2,
} LOG_LEVEL;

char DEBUG_INDENT[MAX_INDENT_LEN];
char DEFAULT_INDENT[MAX_INDENT_LEN];
char WARNING_INDENT[MAX_INDENT_LEN];

void LOG(int noprefix, LOG_LEVEL level, const char* fmt, ...);
void LOG_SET_INDENT(LOG_LEVEL level, const char* indent);
void LOG_VERBOSE(int verbose, LOG_LEVEL log_level, const char* fmt, ...);

// This one is runtime-variable, and thus cant be ifdefd.
#define log_verbose(fmt, ...)		LOG_VERBOSE(0, settings.verbose, DEFAULT, fmt, ##__VA_ARGS__)

// print IFF PR_SUPPRESS
#ifdef PR_DEBUG
	#define log_debug(fmt, ...)		LOG(0, DEBUG, fmt, ##__VA_ARGS__)
	#define log_debug_noprefix(fmt, ...)	LOG(1, DEBUG, fmt, ##__VA_ARGS__)
	#define log_debug_set_indent(indent)	LOG_SET_INDENT(DEBUG, indent)
#else
	#define log_debug(fmt, ...)	NOP
	#define log_debug_noprefix(fmt, ...)	NOP
	#define log_debug_set_indent(indent)	NOP
#endif


// print UNLESS PR_SUPPRESS
#ifndef PR_SUPPRESS
	#define log_default(fmt, ...)	LOG(0, DEFAULT, fmt, ##__VA_ARGS__)
	#define log_default_set_indent(indent)	LOG(0, DEFAULT, indent)
#else
	#define log_default(fmt, ...)	NOP
	#define log_default_set_indent(DEFAULT, indent)	NOP
#endif


// you cant supress warnings
#define log_warning(fmt, ...)			LOG(0, WARNING, fmt, ##__VA_ARGS__)
#define log_warning_set_indent(indent)		LOG(0, WARNING, indent)

// you cant supress FATAL errors
#define log_fatal(fmt, ...)do{ \
	LOG(0, FATAL, fmt, ##__VA_ARGS__);\
	exit(EXIT_FAILURE);\
	} while(0)\

#define log_fatal_cleanup(cleanup_func, fmt, ...)do{\
	LOG(0, FATAL, fmt, ##__VA_ARGS__);\
	cleanup_func();\
	exit(EXIT_FAILURE);\
	}while(0)



#endif // KLL_LOGS
