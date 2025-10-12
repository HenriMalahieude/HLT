#include <stdio.h>
#include <string.h>

#include "regex_helper.h"

#ifndef ALL_TESTS
int test_count = 1;
#else
extern int test_count;
#endif


static bool ReportResult(char *msg, bool result) {
#ifdef VERBOSE
	printf("[%d] Vocab Test (%s): %s\n", test_count, msg, (result) ? "[+]" : "[X]");
#else
	if (!result) printf("[X] Vocab Test (%s) failed\n", msg);
#endif
	test_count++;

	return result;
}

bool VocabularyMakeTests(){
#ifdef VERBOSE
	printf("[!] Starting Vocabulary Make Tests\n");
#endif

	struct lxc_vocabulary *first = LexicVocabularyAllocate(); ReportResult("Allocation", first != NULL);
	
	bool succ = LexicVocabularyDefinitionAdd(first, "digit", "[0-9]"); 
	if (!succ) return false;
	if (!ReportResult("Add Def1", succ)) return false;
	if (!ReportResult("Add Def3", first->def_count == 1)) return false;
	if (!ReportResult("Add Def3", strcmp(first->definitions[0].name, "digit") == 0)) return false;
	if (!ReportResult("Add Def4", strcmp(first->definitions[0].regex, "[0-9]") == 0)) return false;

	succ = LexicVocabularyDefinitionAdd(first, "abc", "abc");
	if (!succ) return false;
	if(!ReportResult("Add Def5", succ)) return false;
	if(!ReportResult("Add Def6", first->def_count == 2)) return false;

	LexicVocabularyFree(first);
	
	//Note that this will be built in _build folder/directory
	first = LexicVocabularyFromFile("../example.vocab");
	if (first == NULL) return false;
	if (!ReportResult("File1", first->def_count == 6)) return false;
	if (!ReportResult("File2", strcmp(first->definitions[0].name, "TYPE") == 0)) return false;
	if (!ReportResult("File3", strcmp(first->definitions[3].name, "NUMBER") == 0)) return false;
	if (!ReportResult("File4", strcmp(first->definitions[5].name, "SEMICOLON") == 0)) return false;
	if (!ReportResult("File5", strcmp(first->definitions[5].regex, ";") == 0)) return false;
	if (!ReportResult("File6", strcmp(first->definitions[2].regex, "=") == 0)) return false;
	if (!ReportResult("File7", strcmp(first->definitions[3].regex, "-?[0-9]+\\.?[0-9]*") == 0)) return false;
	//printf("file: '%s' vs input '-?[0-9]+\\.[0-9]*' -> strcmp = %d\n", first->definitions[3].regex, strcmp(first->definitions[3].regex, "-?[0-9]+\\.[0-9]*"));

	LexicVocabularyFree(first);

	first = LexicVocabularyFromString("THING: 00\n\n ITEM: 11");
	if (first == NULL) return false;
	if (!ReportResult("String1", first->def_count == 2)) return false;
	if (!ReportResult("String2", strcmp(first->definitions[0].name, "THING") == 0)) return false;
	if (!ReportResult("String3", strcmp(first->definitions[1].regex, "11") == 0)) return false;

#ifdef VERBOSE
	printf("[!] Finished Vocabulary Make Tests Successfully\n");
#endif
	return true;
}

#ifndef ALL_TESTS
int main(void) {
	VocabularyMakeTests();
	return 0;
}
#endif
