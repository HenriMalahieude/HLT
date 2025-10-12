#include <stdio.h>
#include <string.h>

#include "regex_helper.h"

#ifndef ALL_TESTS
int test_count = 1;
#else 
extern int test_count;
#endif

static void ReportResult(int errCode, struct lxc_token out, struct lxc_token exp) {
	if (errCode > 0) {
		printf("[X] Token Stream Test %d: ", test_count);
		switch (errCode) {
			case 1:
				printf("def (out '%s' vs '%s')\n", out.definition_name, exp.definition_name);
				break;
			case 2:
				printf("mch (out '%s' vs '%s')\n", out.matching_input, exp.matching_input);
				break;
			case 3:
				printf("lne (out %d vs %d)\n", out.line, exp.line);
				break;
			case 4:
				printf("col (out %d vs %d)\n", out.col, exp.col);
				break;
			default:
				break;
		}

	} 
#ifdef VERBOSE
	else {
		printf("[.] Token Stream Test %d Pass\n", test_count);
	}
#endif

	test_count++;
}

int CompareTokens(struct lxc_token streamout, struct lxc_token expected) {
	if (strcmp(streamout.definition_name, expected.definition_name) != 0) return 1;
	if (expected.definition_name != NULL && strcmp(streamout.matching_input, expected.matching_input) != 0) return 2;
	if (expected.line >= 0 && streamout.line != expected.line) return 3;
	if (expected.col >= 0 && streamout.col != expected.col) return 4;

	return 0;
}

bool TokenStreamTest() {
#ifdef VERBOSE
	printf("[!] Token Stream Test Start!\n");
#endif

	LexicVocabulary *vocab = LexicVocabularyAllocate();
	LexicVocabularyDefinitionAdd(vocab, "digit", "[0-9]*");
	LexicVocabularyDefinitionAdd(vocab, "alpha", "[a-z]*");

	int result = 0; LexicToken expected;

	//one token type
	LexicToken *tstream = LexicTokensFromString("a", vocab);
	expected = (LexicToken){ .definition_name = "alpha", .matching_input = "a", .line = 1, .col = 1};
	result = CompareTokens(tstream[0], expected);
	ReportResult(result, tstream[0], expected); if (result != 0) return false;

	if (tstream[1].definition_name != NULL) return false;
	LexicTokensFree(tstream);

	//one token type
	tstream = LexicTokensFromString("0", vocab);
	expected = (LexicToken){ .definition_name = "digit", .matching_input = "0", .line = 1, .col = 1};
	result = CompareTokens(tstream[0], expected);
	ReportResult(result, tstream[0], expected); if (result != 0) return false;
	LexicTokensFree(tstream);

	//multi stream
	tstream = LexicTokensFromString("ab c", vocab);
	expected = (LexicToken){ .definition_name = "alpha", .matching_input = "ab", .line = 1, .col = 1};
	result = CompareTokens(tstream[0], expected);
	ReportResult(result, tstream[0], expected); if (result != 0) return false;

	expected = (LexicToken){ .definition_name = "alpha", .matching_input = "c", .line = 1, .col = 4};
	result = CompareTokens(tstream[1], expected);
	ReportResult(result, tstream[1], expected); if (result != 0) return false;
	LexicTokensFree(tstream);

	//multi stream
	tstream = LexicTokensFromString("01 123", vocab);
	expected = (LexicToken){ .definition_name = "digit", .matching_input = "01", .line = 1, .col = 1};
	result = CompareTokens(tstream[0], expected);
	ReportResult(result, tstream[0], expected); if (result != 0) return false;

	expected = (LexicToken){ .definition_name = "digit", .matching_input = "123", .line = 1, .col = 4};
	result = CompareTokens(tstream[1], expected);
	ReportResult(result, tstream[1], expected); if (result != 0) return false;
	LexicTokensFree(tstream);

	//segemented together
	tstream = LexicTokensFromString("0a\nhsdfbz183 0a f9", vocab);
	expected = (LexicToken){ .definition_name = "digit", .matching_input = "0", .line = 1, .col = 1};
	result = CompareTokens(tstream[0], expected);
	ReportResult(result, tstream[0], expected); if (result != 0) return false;

	expected = (LexicToken){ .definition_name = "alpha", .matching_input = "a", .line = 1, .col = 2};
	result = CompareTokens(tstream[1], expected);
	ReportResult(result, tstream[1], expected); if (result != 0) return false;

	expected = (LexicToken){ .definition_name = "alpha", .matching_input = "hsdfbz", .line = 2, .col = 1};
	result = CompareTokens(tstream[2], expected);
	ReportResult(result, tstream[2], expected); if (result != 0) return false;

	expected = (LexicToken){ .definition_name = "digit", .matching_input = "183", .line = 2, .col = 7};
	result = CompareTokens(tstream[3], expected);
	ReportResult(result, tstream[3], expected); if (result != 0) return false;

	expected = (LexicToken){ .definition_name = "digit", .matching_input = "0", .line = 2, .col = 11};
	result = CompareTokens(tstream[4], expected);
	ReportResult(result, tstream[4], expected); if (result != 0) return false;

	expected = (LexicToken){ .definition_name = "alpha", .matching_input = "a", .line = 2, .col = 12};
	result = CompareTokens(tstream[5], expected);
	ReportResult(result, tstream[5], expected); if (result != 0) return false;

	expected = (LexicToken){ .definition_name = "alpha", .matching_input = "f", .line = 2, .col = 14};
	result = CompareTokens(tstream[6], expected);
	ReportResult(result, tstream[6], expected); if (result != 0) return false;

	expected = (LexicToken){ .definition_name = "digit", .matching_input = "9", .line = 2, .col = 15};
	result = CompareTokens(tstream[7], expected);
	ReportResult(result, tstream[7], expected); if (result != 0) return false;

	if (tstream[8].definition_name != NULL) return false;
	if (result != 0) return false;
	LexicTokensFree(tstream);

#ifdef VERBOSE
	printf("[!] Token Stream Test Finished Successfully!\n");
#endif
	return true;
}

#ifndef ALL_TESTS
int main(void) {
	TokenStreamTest();
	return 0;
}
#endif
