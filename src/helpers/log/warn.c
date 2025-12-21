#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "warn.h"

enum hlt_warn_type warn_level = HLT_STDWRN; //Everything at this prio or higher

void append_line_no(int line, int col) {
	if (line < 0 && col < 0) return;

	fprintf(stderr, " @");
	if (line >= 0) fprintf(stderr, " Line %d", line);
	if (line >= 0 && col >= 0) fprintf(stderr, ",");
	if (col >= 0) fprintf(stderr, " Column %d", col);
}

void HLTError(int line, int col, const char *format, ...) {
	assert(format != NULL);

	va_list rest;
	va_start(rest, format);

	fprintf(stderr, "Error");
	append_line_no(line, col);
	fprintf(stderr, ": ");

	vfprintf(stderr, format, rest);
	fprintf(stderr, "\n");
	va_end(rest);
	exit(1);
}

void HLTWarn(int line, int col, enum hlt_warn_type type, const char *format, ...) {
	if (type == HLT_SILENT) HLT_AERR("Attempted to make a silent warning?");
	if (format == NULL) HLT_AERR("Attempted to output NULL?");

	va_list rest;
	va_start(rest, format);

	if ((int)type > (int)warn_level) return;
	if (type != HLT_DEBUG) fprintf(stderr, "Warning");
	else fprintf(stderr, "Log");

	append_line_no(line, col);
	fprintf(stderr, ": ");
	vfprintf(stderr, format, rest);
	fprintf(stderr, "\n");

	va_end(rest);
}

