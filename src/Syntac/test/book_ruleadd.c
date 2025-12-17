#define TEST_NAME "RuleAdd"
#include "syn_helper.h"

bool RuleAddTest() {
	test_count = 0;
	int valid = 0;
	
	print_test("Start!");
	SyntacBook *book = SyntacBookAllocate();

	//Base Cases
	enum hlt_warn_type tmp = warn_level;
	if (warn_level != HLT_DEBUG) warn_level = HLT_SILENT; //silence other than debug mode
	SyntacBookRuleAdd(book, "", ""); //these will warn, hence the silence
	SyntacBookRuleAdd(book, "", "item");
	if (!TEST(book->rule_count, 0)) return false; //if any of these added 1, this is fatal
	valid += 1;
	
	//Test edge case (NULL/epsilon rules)
	SyntacBookRuleAdd(book, "NULL", "");
	if (!TEST_STRING(book->rules[0].name, "NULL")) return false;
	if (!TEST_STRING(book->rules[0].elements[0], EPSILON)) return false;
	if (!TEST_NULL(book->rules[0].elements[1])) return false;
	valid += 2;
	warn_level = tmp; //reset

	SyntacBookFree(book);
	book = SyntacBookAllocate();
	valid += (int)TEST_NULL(book->rules);

	SyntacBookRuleAdd(book, "A", "B");
	valid += (int)TEST(book->rule_count, 1);
	valid += (int)TEST_STRING(book->rules[0].name, "A");
	valid += (int)TEST_STRING(book->rules[0].elements[0], "B");
	valid += (int)TEST_NULL(book->rules[0].elements[1]);
	
	SyntacBookRuleAdd(book, "left", "right");
	valid += (int)TEST(book->rule_count, 2);
	valid += (int)TEST_STRING(book->rules[1].name, "left");
	valid += (int)TEST_STRING(book->rules[1].elements[0], "right");
	valid += (int)TEST_NULL(book->rules[1].elements[1]);

	SyntacBookRuleAdd(book, "thing", "left:right");
	valid += (int)TEST(book->rule_count, 3);
	valid += (int)TEST_STRING(book->rules[2].name, "thing");
	valid += (int)TEST_NULL(book->rules[2].elements[2]);
	valid += (int)TEST_STRING(book->rules[2].elements[0], "left");
	valid += (int)TEST_STRING(book->rules[2].elements[1], "right");
	
	SyntacBookRuleAdd(book, "thing2", "thing:left:right:thing:item:ahaha");
	valid += (int)TEST(book->rule_count, 4);
	valid += (int)TEST_STRING(book->rules[3].name, "thing2");
	valid += (int)TEST_NULL(book->rules[3].elements[6]); //there should be 6 elms
	valid += (int)TEST_STRING(book->rules[3].elements[3], "thing");
	valid += (int)TEST_STRING(book->rules[3].elements[5], "ahaha");

	SyntacBookRuleAdd(book, "B", "B:C:D:A:E");
	valid += (int)TEST(book->rule_count, 5);
	valid += (int)TEST_STRING(book->rules[4].name, "B");
	valid += (int)TEST(SetCount(book->rules[4].elements), 5);
	valid += (int)TEST_STRING(book->rules[4].elements[0], "B");
	valid += (int)TEST_STRING(book->rules[4].elements[1], "C");
	valid += (int)TEST_STRING(book->rules[4].elements[3], "A");
	
	SyntacBookFree(book);

	print_test("Finished!");
	return valid == test_count;
}

#ifndef ALL_TESTS
int test_count = 0;
int main(void) {
	//warn_level = HLT_DEBUG;
	return !RuleAddTest();
}
#endif
