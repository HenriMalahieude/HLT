#define TEST_NAME "Direct"
#include "regex_helper.h"

bool DirectTest() {
	int total = 0;
#ifdef VERBOSE
	printf("[!] Regex Direct Test Start\n");
#endif

	char match[4] = "int";
#ifdef VERBOSE
	printf("[.] Initialized Direct Regex\n");
#endif

	total += TEST_REGEX(match, "", false);
	total += TEST_REGEX(match, "i", false);
	total += TEST_REGEX(match, "in", false);
	total += TEST_REGEX(match, "int", true);
	total += TEST_REGEX(match, "integer", false);
	total += TEST_REGEX(match, "double", false);

#ifdef VERBOSE
	printf("[!] Regex Direct Test Finished\n");
#endif
	return total == (test_count-1);
}

#ifndef ALL_TESTS

//int regex_line_no = 0;
//int regex_colu_no = 0;
int test_count = 1;

int main(void) {
	//warn_level = HLT_DEBUG;
	return !DirectTest();
}
#endif
