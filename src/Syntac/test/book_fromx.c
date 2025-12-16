#define TEST_NAME "FromX"
#include "syn_helper.h"

bool BookFromTest() {
	test_count = 0;
	int valid = 0;

	print_test("Start!");

	//Simple0
	char string0[] = "null-> ";
	SyntacBook *book0 = SyntacBookFromString(string0, STC_LL1);
	valid += TEST(book0->rule_count, 1);
	valid += TEST_STRING(book0->rules[0].name, "null");
	valid += TEST_NULL(book0->rules[0].elements[0]);

	//Simple1
	char string1[] = "one->two";
	SyntacBook *book1 = SyntacBookFromString(string1, STC_LL1);
	valid += TEST((int)book1->type, (int)STC_LL1);
	valid += TEST(book1->rule_count, 1);
	valid += TEST_STRING(book1->rules[0].name, "one");
	valid += TEST_STRING(book1->rules[0].elements[0], "two");
	valid += TEST_NULL(book1->rules[0].elements[1]);
	SyntacBookFree(book1); book1 = NULL;
	if (valid != test_count) goto bottom;
	
	//Multiple, and Spacing
	char string2[] = "thr->fou\na->b:x:c\n spaceing ->  ll :xdr : a";
	SyntacBook *book2 = SyntacBookFromString(string2, STC_LL1);
	valid += TEST(book2->rule_count, 3);
	valid += TEST_NULL(book2->rules[1].elements[3]);
	valid += TEST_STRING(book2->rules[2].name, "spaceing");
	valid += TEST_STRING(book2->rules[2].elements[0], "ll");
	valid += TEST_STRING(book2->rules[2].elements[1], "xdr");
	valid += TEST_STRING(book2->rules[2].elements[2], "a");
	SyntacBookFree(book2); book2 = NULL;
	if (valid != test_count) goto bottom;

	//Comments/non-rule lines
	char string3[] = "comment ahaha\nitem->none\n\nanother comment\n\n\na->b\n";
	SyntacBook *book3 = SyntacBookFromString(string3, STC_LL1);
	valid += TEST(book3->rule_count, 2);
	valid += TEST_NULL(book3->rules[0].elements[1]);
	valid += TEST_STRING(book3->rules[0].name, "item");
	valid += TEST_STRING(book3->rules[1].name, "a");
	SyntacBookFree(book3); book3 = NULL;
	if (valid != test_count) goto bottom;

	//File Loading
	char path[] = "../grammar.stc";
	SyntacBook *book4 = SyntacBookFromFile(path);
	valid += TEST((int)book4->type, (int)STC_LL1);
	valid += TEST(book4->rule_count, 9);
	valid += TEST_STRING(book4->rules[0].name, "FUNC");
	valid += TEST_STRING(book4->rules[1].name, "ARGS");
	valid += TEST_STRING(book4->rules[2].name, "ARGS");
	valid += TEST_STRING(book4->rules[3].name, "BODY");
	valid += TEST_STRING(book4->rules[4].name, "BODY");
	valid += TEST_STRING(book4->rules[5].name, "STATEMENTS");
	valid += TEST_STRING(book4->rules[6].name, "STATEMENT");
	valid += TEST_NULL(book4->rules[0].elements[6]);
	valid += TEST_STRING(book4->rules[6].elements[0], "WORD");

bottom:
	print_test("Finished!");
	return valid == test_count;
}

#ifndef ALL_TESTS
int test_count = 0;
int main(void) {
	//warn_level = HLT_DEBUG;
	return !BookFromTest();
}
#endif
