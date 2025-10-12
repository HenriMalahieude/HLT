#define TEST_NAME "Qualifier"
#include "regex_helper.h"

bool QualifierTest() {
	int total = 0;
#ifdef VERBOSE
	printf("[!] Regex Qualifier Test Start\n");
#endif

	char q1[] = "a*";
	total += TEST_REGEX(q1, "", true);
	total += TEST_REGEX(q1, "a", true);
	total += TEST_REGEX(q1, "aa", true);
	total += TEST_REGEX(q1, "aaaaaaa", true);
	total += TEST_REGEX(q1, "aaab", false);
	total += TEST_REGEX(q1, "b", false);

	char q2[] = "b?";
	total += TEST_REGEX(q2, "", true);
	total += TEST_REGEX(q2, "b", true);
	total += TEST_REGEX(q2, "bb", false);
	total += TEST_REGEX(q2, "a", false);
	total += TEST_REGEX(q2, "aaaaaaaaa", false);
	total += TEST_REGEX(q2, "ba", false);

	char q3[] = "a+";
	total += TEST_REGEX(q3, "", false);
	total += TEST_REGEX(q3, "a", true);
	total += TEST_REGEX(q3, "aaaaaa", true);
	total += TEST_REGEX(q3, "ababa", false);//*/

	char q4[] = "[ab]*";
	total += TEST_REGEX(q4, "", true);
	total += TEST_REGEX(q4, "ab", true);
	total += TEST_REGEX(q4, "ba", true);
	total += TEST_REGEX(q4, "abababababababababbbaaabababaaa", true);
	total += TEST_REGEX(q4, "abd", false);
	total += TEST_REGEX(q4, "d", false);//*/

	char q5[] = "\\s+";
	total += TEST_REGEX(q5, "", false);
	total += TEST_REGEX(q5, " ", true);
	total += TEST_REGEX(q5, "        \t ", true);
	total += TEST_REGEX(q5, "   b", false);//*/

#ifdef VERBOSE
	printf("[!] Regex Qualifier Test Finished\n");
#endif
	return total == (test_count-1);
}

#ifndef ALL_TESTS

//int regex_line_no = 0;
//int regex_colu_no = 0;
int test_count = 1;

int main(void) {
	QualifierTest();
	return 0;
}
#endif
