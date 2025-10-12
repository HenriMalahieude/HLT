#define TEST_NAME "Brackets"
#include "regex_helper.h"

bool BracketTest() {
	int total = 0;
#ifdef VERBOSE
	printf("[!] Regex Bracket Test Start\n");
#endif

	char list[] = "[abc]";
	char seq[] = "[a-zA-Z_]";

	//Single characters
	total += TEST_REGEX(list, "a", true);
	total += TEST_REGEX(list, "b", true);
	total += TEST_REGEX(list, "c", true);
	total += TEST_REGEX(list, "ad", false);
	total += TEST_REGEX(list, "bd", false);
	total += TEST_REGEX(list, "cd", false);

	//Sequences
	total += TEST_REGEX(seq, "_", true);
	char send[2] = "a";
	for (int i = (int)'a'; i <= (int)'z'; i++) {
		send[0] = (char)i;
		total += TEST_REGEX(seq, send, true); 
	}

	send[0] = 'A';
	for (int i = (int)'A'; i <= (int)'Z'; i++) {
		send[0] = (char)i;
		total += TEST_REGEX(seq, send, true);
	}
	
	total += TEST_REGEX(seq, "9", false);
	total += TEST_REGEX(seq, "2", false); //*/
	
	total += TEST_REGEX("a[bc]", "ac", true);
	total += TEST_REGEX("a[bc]", "ab", true);
	total += TEST_REGEX("a[bc]", "b", false);
	total += TEST_REGEX("a[bc]", "ad", false);

#ifdef VERBOSE
	printf("[!] Regex Bracket Test Finished\n");
#endif
	return total == (test_count-1);
}

#ifndef ALL_TESTS

//int regex_line_no = 0;
//int regex_colu_no = 0;
int test_count = 1;

int main(void) {
	//warn_level = HLT_STDWRN;
	BracketTest();
	return 0;
}
#endif
