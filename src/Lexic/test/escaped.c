#define TEST_NAME "Escaped"
#include "regex_helper.h"

bool EscapedTest() {
	int total = 0;
#ifdef VERBOSE
	printf("[!] Regex Escaped Test Start\n");
#endif

	char nl[3] = "\\n";
	char ws[3] = "\\s";
	char nws[3] = "\\S";

	total += TEST_REGEX(nl, "\n", true);
	total += TEST_REGEX(nl, "a", false);

	total += TEST_REGEX(ws, " ", true);
	total += TEST_REGEX(ws, "\t", true);
	total += TEST_REGEX(ws, "a", false);
	total += TEST_REGEX(ws, "A", false);
	total += TEST_REGEX(ws, "9", false);

	total += TEST_REGEX(nws, "a", true);
	total += TEST_REGEX(nws, "A", true);
	total += TEST_REGEX(nws, "0", true);
	total += TEST_REGEX(nws, "\t", false);
	total += TEST_REGEX(nws, " ", false);

#ifdef VERBOSE
	printf("[!] Regex Escaped Test Finished\n");
#endif
	return total == (test_count-1);
}

#ifndef ALL_TESTS

//int regex_line_no = 0;
//int regex_colu_no = 0;
int test_count = 1;

int main(void) {
	EscapedTest();
	return 0;
}
#endif
