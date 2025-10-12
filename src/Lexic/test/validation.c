#include <stdio.h>

#include "regex_helper.h"

bool TEST_VALIDATE(char *rgx, bool result_to) {
	bool val = (RegexValidate(rgx) == result_to);

#ifndef VERBOSE
	if (!val) printf("[X] Regex Validation Fail: %s\n", rgx);
#else
	printf("[?] Regex Validation Test: ");
	if (val) printf("[+]\n");
	else printf("[X] w/ %s\n", rgx);
#endif
}

bool ValidationTest() {
	int total = 0;
#ifdef VERBOSE
	printf("[!] Regex Validation Test Start\n");
#endif
	total += TEST_VALIDATE("-?[0-9]+(\\.[0-9]+)?", true);
	total += TEST_VALIDATE("?", false);
	total += TEST_VALIDATE("a*?", false);
	total += TEST_VALIDATE("[]]", false);
	total += TEST_VALIDATE("[[]", false);
	total += TEST_VALIDATE("(()", false);
	total += TEST_VALIDATE("())", false);
	total += TEST_VALIDATE("|", false);
	total += TEST_VALIDATE("abc|(|)", false);
	total += TEST_VALIDATE("abc|*", false);
	total += TEST_VALIDATE("abc|", false);

#ifdef VERBOSE
	printf("[!] Regex Validation Test Finished\n");
#endif
	return total == 0;
}

#ifndef ALL_TESTS
#include "../../helpers/log/warn.h"

int regex_line_no = 0;
int regex_colu_no = 0;

int main(void) {
	warn_level = HLT_SILENT;
	ValidationTest();
	return 0;
}
#endif
