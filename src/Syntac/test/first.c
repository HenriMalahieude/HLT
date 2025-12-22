#define TEST_NAME "First Sets"

#include "syn_helper.h"

bool FirstTest() {
	test_count = 0;
	int valid = 0;

	print_test("Start!");
	// /*	
	//Step 0: Test trivial/edge-cases (includes epsilon)
	SyntacBook *book1 = SyntacBookAllocate();
	SyntacBookRuleAdd(book1, "A", "B");
	SyntacBookRuleAdd(book1, "A", "C");
	SyntacBookRuleAdd(book1, "C", "");
	if (book1->rule_count != 3) {
		print_test("SyntacBookRuleAdd failed to add 3 rules as requested?");
		return false;
	}

	firsts_of_book(book1);

	char **set_a1_b1 = SetCreate(1, "B");
	valid += TEST_SET(book1->rules[0].first_set, set_a1_b1);
	valid += TEST_STRING(book1->rules[0].first_set[0], "B");
	SetFree(set_a1_b1); set_a1_b1 = NULL;

	char **set_a2_b1 = SetCreate(1, EPSILON);
	valid += TEST_SET(book1->rules[1].first_set, set_a2_b1);	
	valid += TEST_SET(book1->rules[2].first_set, set_a2_b1);
	SetFree(set_a2_b1); set_a2_b1 = NULL;

	SyntacBookFree(book1); book1 = NULL;
	if (valid != test_count) {
		print_test("Failed trivial/edge-cases");
		return false;
	}

	print_test("Passed trivial/edge-cases\n");
	
	//Step 0.5: Recursion
	book1 = SyntacBookAllocate();
	SyntacBookRuleAdd(book1, "A", "A:B");
	SyntacBookRuleAdd(book1, "C", "B:C:B");
	SyntacBookRuleAdd(book1, "B", "c");
	SyntacBookRuleAdd(book1, "B", "");
	if (book1->rule_count != 4) {
		print_test("SyntacBookRuleAdd did not add 4 rules as requested?");
		return false;
	}

	firsts_of_book(book1);
		
	char **rec_a_b1 = SetCreate(0);
	valid += TEST_SET(book1->rules[0].first_set, rec_a_b1);
	SetFree(rec_a_b1); rec_a_b1 = NULL;

	char **set_c_t05 = SetCreate(2, "c", EPSILON);
	valid += TEST_SET(book1->rules[1].first_set, set_c_t05);
	SetFree(set_c_t05); set_c_t05 = NULL;

	char **set_b_t05 = SetCreate(1, "c");
	valid += TEST_SET(book1->rules[2].first_set, set_b_t05);
	SetFree(set_b_t05); set_b_t05 = NULL;

	set_b_t05 = SetCreate(1, EPSILON);
	valid += TEST_SET(book1->rules[3].first_set, set_b_t05);
	SetFree(set_b_t05); set_b_t05 = NULL;

	SyntacBookFree(book1); book1 = NULL;
	if (valid != test_count) {
		print_test("Failed recursion case!");
		return false;
	}

	print_test("Passed recursion case\n");

	//Step 1.5: Simple grammar without epsilon rules
	//	NOTE: This would not be a valid Top Down Grammar
	book1 = SyntacBookAllocate();
	SyntacBookRuleAdd(book1, "E", "E:+:T");
	SyntacBookRuleAdd(book1, "E", "E:-:T");
	SyntacBookRuleAdd(book1, "E", "T");
	SyntacBookRuleAdd(book1, "T", "T:*:F");
	SyntacBookRuleAdd(book1, "T", "T:/:F");
	SyntacBookRuleAdd(book1, "T", "F");
	SyntacBookRuleAdd(book1, "F", "(:E:)");
	SyntacBookRuleAdd(book1, "F", "id");
	if (book1->rule_count != 8) {
		print_test("SyntacBookRuleAdd failed to add 8 rules as requested?");
		return false;
	}
	
	firsts_of_book(book1);

	char **set_t1_b1 = SetCreate(2, "(", "id");
	valid += TEST_SET(book1->rules[0].first_set, set_t1_b1);
	valid += TEST_SET(book1->rules[1].first_set, set_t1_b1);
	valid += TEST_SET(book1->rules[2].first_set, set_t1_b1);
	valid += TEST_SET(book1->rules[3].first_set, set_t1_b1);
	valid += TEST_SET(book1->rules[4].first_set, set_t1_b1);
	valid += TEST_SET(book1->rules[5].first_set, set_t1_b1);
	SetFree(set_t1_b1); set_t1_b1 = NULL;
	
	char **set_f1_b1 = SetCreate(1, "(");
	valid += TEST_SET(book1->rules[6].first_set, set_f1_b1);
	SetFree(set_f1_b1); set_f1_b1 = NULL;

	char **set_f2_b1 = SetCreate(1, "id");
	valid += TEST_SET(book1->rules[7].first_set, set_f2_b1);
	SetFree(set_f2_b1); set_f2_b1 = NULL;

	SyntacBookFree(book1); book1 = NULL;
	if (valid != test_count) {
		print_test("Failed simple w/o epsilon");
		return false;
	}
	print_test("Failed simple w/o epsilon\n");

	//Step 1: Test a simple book (from internet) (this time w/ epsilon)
	SyntacBook *book0 = SyntacBookAllocate();

	SyntacBookRuleAdd(book0, "E", "T:E'");
	SyntacBookRuleAdd(book0, "E'", "+:T:E'");
	SyntacBookRuleAdd(book0, "E'", "");
	SyntacBookRuleAdd(book0, "T", "F:T'");
	SyntacBookRuleAdd(book0, "T'", "*:F:T'");
	SyntacBookRuleAdd(book0, "T'", "");
	SyntacBookRuleAdd(book0, "F", "(:E:)");
	SyntacBookRuleAdd(book0, "F", "id");
	if (book0->rule_count != 8) {
		print_test("SyntacBookRuleAdd failed to add 8 rules as requested?");
		return false;
	}

	firsts_of_book(book0);

	char **set_e_b0 = SetCreate(2, "(", "id");
	valid += TEST_SET(book0->rules[0].first_set, set_e_b0);

	char **set_ep_b0 = SetCreate(1, "+");
	valid += TEST_SET(book0->rules[1].first_set, set_ep_b0);

	char **set_ep2_b0 = SetCreate(1, EPSILON);
	valid += TEST_SET(book0->rules[2].first_set, set_ep2_b0);
	
	valid += TEST_SET(book0->rules[3].first_set, set_e_b0);

	char **set_tp_b0 = SetCreate(1, "*");
	valid += TEST_SET(book0->rules[4].first_set, set_tp_b0);
	
	valid += TEST_SET(book0->rules[5].first_set, set_ep2_b0);
	
	char **set_f1_b0 = SetCreate(1, "(");
	valid += TEST_SET(book0->rules[6].first_set, set_f1_b0);
	
	char **set_f2_b0 = SetCreate(1, "id");
	valid += TEST_SET(book0->rules[7].first_set, set_f2_b0);

	SetFree(set_e_b0); set_e_b0 = NULL;
	SetFree(set_ep_b0); set_ep_b0 = NULL;
	SetFree(set_ep2_b0); set_ep2_b0 = NULL;
	SetFree(set_tp_b0); set_tp_b0 = NULL;
	SetFree(set_f1_b0); set_f1_b0 = NULL;
	SetFree(set_f2_b0); set_f2_b0 = NULL;
	SyntacBookFree(book0); book0 = NULL;
	if (valid != test_count) {
		print_test("Failed simple case");
		return false;
	}

	print_test("Passed simple case!\n");
	// */
	//Step 2: Test File Grammar
	SyntacBook *book = SyntacBookFromFile("../grammar.stc");
	valid += TEST(book->rule_count, 9);

	firsts_of_book(book);
	
	char **set_func_b = SetCreate(1, "TYPE");
	valid += TEST_SET(book->rules[0].first_set, set_func_b); //func
	valid += TEST_SET(book->rules[1].first_set, set_func_b); //args
	SetFree(set_func_b); set_func_b = NULL;

	char **set_args2_b = SetCreate(1, EPSILON);
	valid += TEST_SET(book->rules[2].first_set, set_args2_b); //args2
	
	char **set_body1_b = SetCreate(1, "LEFTCURLY");
	valid += TEST_SET(book->rules[3].first_set, set_body1_b); //body1

	char **set_body2_b = SetCreate(1, "WORD");
	valid += TEST_SET(book->rules[4].first_set, set_body2_b); //body2
	valid += TEST_SET(book->rules[5].first_set, set_body2_b); //statements

	char **set_stmnt1_b = SetCreate(1, "WORD");
	valid += TEST_SET(book->rules[6].first_set, set_stmnt1_b); //statement

	valid += TEST_SET(book->rules[7].first_set, set_args2_b); //statement 2
	valid += TEST_SET(book->rules[8].first_set, set_args2_b); //null

	SetFree(set_args2_b); set_args2_b = NULL;
	SetFree(set_body1_b); set_body1_b = NULL;
	SetFree(set_body2_b); set_body2_b = NULL;
	SetFree(set_stmnt1_b); set_stmnt1_b = NULL;
	SyntacBookFree(book); book = NULL;
	if (valid != test_count) {
		print_test("Failed file case");
		return false;
	} // */
	
	print_test("Passed file case\n");

	//Step 3: Complicated File with many first tokens and some Epsilon tokens
	SyntacBook *book3 = SyntacBookFromFile("../grammar2.stc");
	if (book3->rule_count != 10) {
		print_test("SyntacBookFromFile did not add 10 rules as requested?");
		return false;
	}
	valid += TEST(book3->rule_count, 10);

	firsts_of_book(book3);

	char **rule0 = SetCreate(4, "d", "g", "h", EPSILON);
	valid += TEST_SET(book3->rules[0].first_set, rule0);

	char **rule1 = SetCreate(2, "h", "b");
	valid += TEST_SET(book3->rules[1].first_set, rule1);
	
	char **rule2 = SetCreate(2, "g", "a");
	valid += TEST_SET(book3->rules[2].first_set, rule2);

	char **rule3 = SetCreate(1, "d");
	valid += TEST_SET(book3->rules[3].first_set, rule3);

	char **rule4 = SetCreate(3, "g", "h", EPSILON);
	valid += TEST_SET(book3->rules[4].first_set, rule4);

	valid += TEST(SetCount(book3->rules[5].first_set), 1);
	valid += TEST_STRING(book3->rules[5].first_set[0], "g");

	valid += TEST(SetCount(book3->rules[6].first_set), 1);
	valid += TEST_STRING(book3->rules[6].first_set[0], EPSILON);
	
	valid += TEST(SetCount(book3->rules[7].first_set), 1);
	valid += TEST_STRING(book3->rules[7].first_set[0], "h");

	valid += TEST(SetCount(book3->rules[8].first_set), 1);
	valid += TEST_STRING(book3->rules[8].first_set[0], EPSILON);

	char **rule9 = SetCreate(6, "d", "g", "h", "b", "a", EPSILON);
	valid += TEST_SET(book3->rules[9].first_set, rule9);

	SyntacBookFree(book3); book3 = NULL;
	print_test("Finished!");

	return valid == test_count;
}

#ifndef ALL_TESTS
int test_count = 0;
int main(void) {
	//warn_level = HLT_DEBUG;
	return !FirstTest();
}
#endif
