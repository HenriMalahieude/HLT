#define TEST_NAME "LL1_Table"
#include "syn_helper.h"

bool LL1TableTest() {
	test_count = 0;
	int valid = 0;

	print_test("Start!");

	//Trivial Case
	SyntacBook *book1 = SyntacBookAllocate();
	book1->type = STC_LL1;
	SyntacBookRuleAdd(book1, "A", "B"); //A -> B
	SyntacBookRuleAdd(book1, "A", "C");//A ->  C
	SyntacBookRuleAdd(book1, "C", "D:E");  //C -> D E
	SyntacBookRuleAdd(book1, "D", "");
	if (book1->rule_count != 4) {
		print_test("SyntacBookRuleAdd failed to add 4 rules as requested?");
		return false;
	}

	firsts_of_book(book1);
	//Test my math
	char **set = SetCreate(1, "B");
	valid += TEST_SET(book1->rules[0].first_set, set);
	SetFree(set); set = NULL;
	
	set = SetCreate(1, "E");
	valid += TEST_SET(book1->rules[1].first_set, set);
	valid += TEST_SET(book1->rules[2].first_set, set);
	SetFree(set); set = NULL;

	set = SetCreate(1, EPSILON);
	valid += TEST_SET(book1->rules[3].first_set, set);
	SetFree(set); set = NULL;
	if (valid != test_count) {
		print_test("Failed Trivial Case, First Sets?");
		return false;
	}

	follow_of_book(book1);
	//Test my math 2
	set = SetCreate(1, ENDMRKR);
	valid += TEST_SET(book1->rules[0].follow_set, set);
	valid += TEST_SET(book1->rules[1].follow_set, set);
	valid += TEST_SET(book1->rules[2].follow_set, set);
	SetFree(set); set = NULL;

	set = SetCreate(1, "E");
	valid += TEST_SET(book1->rules[3].follow_set, set);
	SetFree(set); set = NULL;
	if (valid != test_count) {
		print_test("Failed Trivial Case, Follow Sets?");
		return false;
	}

	LL1TableCalculate(book1);
	/*printf("LL1 Entry Cnt: %d\n", book1->ll1_table->entry_cnt);
	for (int i = 0; i < book1->ll1_table->entry_cnt; i++) {
		struct stc_ll1_entry *ent = book1->ll1_table->entries + i;
		printf("(%d): %s -> ", ent->rule_idx, ent->nonterm);
		SetPrint(ent->term);
	}*/
	valid += TEST(book1->ll1_table->entry_cnt, 4);
	valid += TEST(book1->ll1_table->entries[0].rule_idx, 0);
	valid += TEST(book1->ll1_table->entries[1].rule_idx, 1);
	valid += TEST(book1->ll1_table->entries[2].rule_idx, 2);
	valid += TEST(book1->ll1_table->entries[3].rule_idx, 3);
	valid += TEST_STRING(book1->ll1_table->entries[0].nonterm, book1->rules[0].name);
	valid += TEST_STRING(book1->ll1_table->entries[1].nonterm, book1->rules[1].name);
	valid += TEST_STRING(book1->ll1_table->entries[2].nonterm, book1->rules[2].name);
	valid += TEST_STRING(book1->ll1_table->entries[3].nonterm, book1->rules[3].name);
	
	set = SetCreate(1, "B");
	valid += TEST_SET(book1->ll1_table->entries[0].term, set);
	SetFree(set); set = NULL;

	set = SetCreate(1, "E");
	valid += TEST_SET(book1->ll1_table->entries[1].term, set);
	valid += TEST_SET(book1->ll1_table->entries[2].term, set);
	valid += TEST_SET(book1->ll1_table->entries[3].term, set);
	SetFree(set); set = NULL;

	SyntacBookFree(book1); book1 = NULL;
	if (valid != test_count) {
		print_test("Failed Trivial Case");
		return false;
	}

	print_test("Passed Trivial Case\n");

	//GeeksForGeeks Case (from ll1 parsing table example)
	book1 = SyntacBookAllocate();
	book1->type = STC_LL1;
	SyntacBookRuleAdd(book1, "E", "T:E'"); 		//0
	SyntacBookRuleAdd(book1, "E'", "+:T:E'"); 	//1
	SyntacBookRuleAdd(book1, "E'", ""); 		//2
	SyntacBookRuleAdd(book1, "T", "F:T'"); 		//3
	SyntacBookRuleAdd(book1, "T'", "*:F:T'"); 	//4
	SyntacBookRuleAdd(book1, "T'", ""); 		//5
	SyntacBookRuleAdd(book1, "F", "(:E:)"); 	//6
	SyntacBookRuleAdd(book1, "F", "id"); 		//7
	if (book1->rule_count != 8) {
		print_test("SyntacBookRuleAdd failed to add 8 rules as requested?");
		return false;
	}

	firsts_of_book(book1);
	follow_of_book(book1);
	LL1TableCalculate(book1);

	valid += TEST(book1->ll1_table->entry_cnt, 8);
	
	set = SetCreate(2, "id", "(");
	valid += TEST_SET(book1->ll1_table->entries[0].term, set);
	valid += TEST_SET(book1->ll1_table->entries[3].term, set);
	SetFree(set); set = NULL;

	set = SetCreate(1, "+");
	valid += TEST_SET(book1->ll1_table->entries[1].term, set);
	SetFree(set); set = NULL;
	
	set = SetCreate(2, ")", ENDMRKR);
	valid += TEST_SET(book1->ll1_table->entries[2].term, set);
	SetFree(set); set = NULL;

	set = SetCreate(1, "*");
	valid += TEST_SET(book1->ll1_table->entries[4].term, set);
	SetFree(set); set = NULL;

	set = SetCreate(3, "+", ")", ENDMRKR);
	valid += TEST_SET(book1->ll1_table->entries[5].term, set);
	SetFree(set); set = NULL;

	set = SetCreate(1, "(");
	valid += TEST_SET(book1->ll1_table->entries[6].term, set);
	SetFree(set); set = NULL;
	
	set = SetCreate(1, "id");
	valid += TEST_SET(book1->ll1_table->entries[7].term, set);
	SetFree(set); set = NULL;

	SyntacBookFree(book1); book1 = NULL;
	if (valid != test_count) {
		print_test("Failed GeeksForGeeks Case");
		return false;
	}

	print_test("Passed GeeksForGeeks Case");
	return true;

	//grammar2.stc
	book1 = SyntacBookFromFile("../grammar2.stc");

	firsts_of_book(book1);
	follow_of_book(book1);

	//TODO: test grammar2.stc
	
	SyntacBookFree(book1); book1 = NULL;
	if (valid != test_count) {
	
	}

	//grammer.stc Case (subset of C)
	book1 = SyntacBookFromFile("../grammar.stc");

	firsts_of_book(book1);
	follow_of_book(book1);

	//TODO: Test Grammer.stc
}

#ifndef ALL_TESTS
int test_count = 0;
int main(void) {
	warn_level = HLT_DEBUG;
	return !LL1TableTest();
}
#endif
