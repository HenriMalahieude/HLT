#define TEST_NAME "Or"
#include "regex_helper.h"

bool OrTest() {
	int total = 0;
#ifdef VERBOSE
	printf("[!] Regex Or Test Start\n");
#endif

	//Direct Or
	char or1[] = "int|double";
	total += TEST_REGEX(or1, "int", true);
	total += TEST_REGEX(or1, "double", true);
	total += TEST_REGEX(or1, "integer", false);
	total += TEST_REGEX(or1, "doubley", false);
	total += TEST_REGEX(or1, "not", false);

	//Triple Or
	char or2[] = "ab|cd|ef";
	total += TEST_REGEX(or2, "ab", true);
	total += TEST_REGEX(or2, "cd", true);
	total += TEST_REGEX(or2, "ef", true);
	total += TEST_REGEX(or2, "in", false);
	total += TEST_REGEX(or2, "oubl", false);
	total += TEST_REGEX(or2, "har", false);
	total += TEST_REGEX(or2, "bruh", false);

	//Escaped Or
	char or4[] = "\\d|a";
	total += TEST_REGEX(or4, "0", true);
	total += TEST_REGEX(or4, "5", true);
	total += TEST_REGEX(or4, "9", true);
	total += TEST_REGEX(or4, "a", true);
	total += TEST_REGEX(or4, "b", false);
	total += TEST_REGEX(or4, "01", false);
	total += TEST_REGEX(or4, "ab", false);

	//Qualified Or
	char or3[] = "0+|1";
	total += TEST_REGEX(or3, "", false);
	total += TEST_REGEX(or3, "1", true);
	total += TEST_REGEX(or3, "0001", false);
	total += TEST_REGEX(or3, "111", false);
	total += TEST_REGEX(or3, "10", false);//*/

	//Group Or
	char or5[] = "(abc|def)*";
	total += TEST_REGEX(or5, "", true);
	total += TEST_REGEX(or5, "a", false);
	total += TEST_REGEX(or5, "abc", true);
	total += TEST_REGEX(or5, "acb", false);
	total += TEST_REGEX(or5, "def", true);
	total += TEST_REGEX(or5, "dfe", false);
	total += TEST_REGEX(or5, "abcabc", true);
	total += TEST_REGEX(or5, "abccba", false);
	total += TEST_REGEX(or5, "defdef", true);
	total += TEST_REGEX(or5, "defdfa", false);
	total += TEST_REGEX(or5, "abcdefdefabc", true);
	total += TEST_REGEX(or5, "abcdfadfaabc", false); //*/

	//Nested Group Or
	char or6[] = "abc|(def+|(ghi|jkl+)|mno)";
	total += TEST_REGEX(or6, "abc", true);
	total += TEST_REGEX(or6, "defff", true);
	total += TEST_REGEX(or6, "ghi", true);
	total += TEST_REGEX(or6, "jklll", true);
	total += TEST_REGEX(or6, "mno", true);
	total += TEST_REGEX(or6, "jk", false);//*/

#ifdef VERBOSE
	printf("[!] Regex Or Test Finished\n");
#endif
	return total == (test_count-1);
}

#ifndef ALL_TESTS
//int regex_line_no = 0;
//int regex_colu_no = 0;
int test_count = 1;

int main(void) {
	OrTest();
	return 0;
}
#endif
