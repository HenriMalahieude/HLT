#include <stdlib.h>

#define TEST_NAME "Group"
#include "regex_helper.h"

bool GroupTest(){
	int total = 0;
#ifdef VERBOSE
	printf("[!] Regex Group Test Start\n");
#endif

	char manual_construct[] = "(\\.[0-9]+)*";
	total += TEST_REGEX(manual_construct, "", true);
	total += TEST_REGEX(manual_construct, ".0", true);
	total += TEST_REGEX(manual_construct, ".00.000.01238.1239.487", true);
	total += TEST_REGEX(manual_construct, "092", false);
	total += TEST_REGEX(manual_construct, ".12309a098", false);
	total += TEST_REGEX(manual_construct, ".01.a", false);

	char built[] = "-?[0-9]+(\\.[0-9]+)?";
	total += TEST_REGEX(built, "9", true);
	total += TEST_REGEX(built, "-0", true);
	total += TEST_REGEX(built, "09.123", true);
	total += TEST_REGEX(built, "-10.005", true);
	total += TEST_REGEX(built, "10023213854.901293847", true);
	total += TEST_REGEX(built, "-123.123.123", false);
	total += TEST_REGEX(built, "-1.ab", false);
	total += TEST_REGEX(built, "asdf", false);
	total += TEST_REGEX(built, "--1-1", false);// */

	char nested[] = "ab(cd*(a*f*)?)+";
	total += TEST_REGEX(nested, "abcddddddd", true);
	total += TEST_REGEX(nested, "abcdaaa", true);
	total += TEST_REGEX(nested, "abcddfff", true);
	total += TEST_REGEX(nested, "abcdafcdafccd", true);
	total += TEST_REGEX(nested, "abcddfa", false);
	total += TEST_REGEX(nested, "abc", true);// */

#ifdef VERBOSE
	printf("[!] Regex Group Test Finished\n");
#endif
	return total == (test_count-1);
}

#ifndef ALL_TESTS
//int regex_line_no = 0;
//int regex_colu_no = 0;
int test_count = 1;
int main(void) {
	GroupTest();
	return 0;
}
#endif
