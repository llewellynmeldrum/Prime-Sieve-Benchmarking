#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "Logging.h"

void LOG_STRING(int noprefix, LOG_LEVEL log_level, const char* body);

void LOG(int noprefix, LOG_LEVEL log_level, const char* fmt, ...) {
	char *body = malloc(sizeof(char) * MAX_BODY_LEN);
	va_list args;
	va_start(args, fmt);
	vsprintf(body, fmt, args);
	va_end(args);
	LOG_STRING(noprefix, log_level, body);
}

void LOG_VERBOSE(int verbose, LOG_LEVEL log_level, const char* fmt, ...) {
	if (verbose) {
		char *body = malloc(sizeof(char) * MAX_BODY_LEN);
		va_list args;
		va_start(args, fmt);
		vsprintf(body, fmt, args);
		va_end(args);
		LOG_STRING(0, log_level, body);
	}
}


//*INDENT-OFF*
void LOG_STRING(int noprefix, LOG_LEVEL log_level, const char* body){
	FILE* restrict out; 

	char prefix[MAX_PREFIX_LEN];	prefix[0] = '\0';
	char indent[MAX_INDENT_LEN];	indent[0] = '\0';
	char postfix[MAX_POSTFIX_LEN];	postfix[0] = '\0';


	switch(log_level){
	case DEBUG:
		strcpy(prefix, "\033[32m[DEBUG] \033[0m");
		strcpy(indent, DEBUG_INDENT);
		out = stderr;
		break;

	case DEFAULT:

		strcpy(indent, DEFAULT_INDENT);
		out = stdout;
		break;

	case WARNING:
		strcpy(prefix, "\033[33m[WARNING] \033[0m");
		strcpy(indent, WARNING_INDENT);
		out = stderr;
		break;

	case FATAL:
		strcpy(prefix, "\033[1;91m[FATAL ERROR]: \033[0m \033[91m");
		strcpy(postfix, "\033[91m\t- Calling cleanup function... \033[0m\n");
		strcat(postfix, "\033[91m\t- Exiting... (EXIT_FAILURE) \033[0m\n");
		out = stderr;
		break;
	default:
		fprintf(stderr, "Warning: LOG called with invalid log level (%d).\n",log_level);
		break;
	}

	int len = strlen(indent) + strlen(prefix) + strlen(body) + strlen(postfix);
	char* full_str = malloc(len + 1);
	full_str[0] = '\0';

	strcat(full_str, indent);
	if (noprefix==0){
		strcpy(full_str, prefix);
	}
	strcat(full_str, body);
	strcat(full_str, postfix);

	fprintf(out, "%s", full_str);
}

void LOG_SET_INDENT(LOG_LEVEL log_level, const char* indent){
	switch(log_level){
	case DEBUG:
		strcpy(DEBUG_INDENT,indent);
		break;

	case DEFAULT:
		strcpy(DEFAULT_INDENT,indent);
		break;

	case WARNING:
		strcpy(WARNING_INDENT,indent);
		break;

	default:
		fprintf(stderr, "Warning: LOG_SET_INDENT called with invalid log level (%d).\n",log_level);
		break;
	}

}
void testing_logs(){
	log_default("testing default.\n");
	log_warning("testing warning.\n");
	log_fatal("testing fatal.\n");
}
