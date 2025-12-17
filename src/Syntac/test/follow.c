#define TEST_NAME "Follow Sets"

#include "syn_helper.h"

bool FollowTest() {
	test_count = 0;
	int valid = 0;

	print_test("Start!");

	//Step 1: Trivial
	SyntacBook *book1 = SyntacBookAllocate();
	SyntacBookRuleAdd(book1, "A", "B");
	SyntacBookRuleAdd(book1, "A", "C:B");
	SyntacBookRuleAdd(book1, "C", "");
	if (book1->rule_count != 3) {
		print_test("SyntacBookRuleAdd failed to add 3 rules as requested?");
		return false;
	}

	firsts_of_book(book1);
	follow_of_book(book1);

	char **set_start = SetCreate(1, ENDMRKR);
	valid += TEST_SET(book1->rules[0].follow_set, set_start);
	valid += TEST_SET(book1->rules[1].follow_set, set_start);
	
	char **set_c_t1 = SetCreate(1, "B");
	valid += TEST_SET(book1->rules[2].follow_set, set_c_t1);
	SetFree(set_c_t1); set_c_t1 = NULL;

	SyntacBookFree(book1); book1 = NULL;
	if (valid != test_count) {
		print_test("Failed Trivial Case");
		return false;
	}
	
	print_test("Passed Trivial Case\n");

	//Step 1.5: Recursion
	book1 = SyntacBookAllocate();
	SyntacBookRuleAdd(book1, "A", "B:A:A");
	SyntacBookRuleAdd(book1, "B", "c");
	SyntacBookRuleAdd(book1, "B", "");
	SyntacBookRuleAdd(book1, "C", "B:C:d");
	SyntacBookRuleAdd(book1, "D", "B:D:D:D:D:D:D:a");
	valid += TEST(book1->rule_count, 5);
	
	firsts_of_book(book1);
	follow_of_book(book1);

	//char **rec_a_b1 = SetCreate(1, ENDMRKR);
	valid += TEST_SET(book1->rules[0].follow_set, set_start);
	//SetFree(rec_a_b1); rec_a_b1 = NULL;

	char **rec_b_b1 = SetCreate(4, "c", "d", "a", ENDMRKR);
	valid += TEST_SET(book1->rules[1].follow_set, rec_b_b1);
	valid += TEST_SET(book1->rules[2].follow_set, rec_b_b1);
	SetFree(rec_b_b1); rec_b_b1 = NULL;

	char **rec_c_b1 = SetCreate(1, "d");
	valid += TEST_SET(book1->rules[3].follow_set, rec_c_b1);
	SetFree(rec_c_b1); rec_c_b1 = NULL;

	char **rec_d_b1 = SetCreate(1, "a");
	valid += TEST_SET(book1->rules[4].follow_set, rec_d_b1);
	SetFree(rec_d_b1); rec_d_b1 = NULL;

	SyntacBookFree(book1); book1 = NULL;
	if (valid != test_count) {
		print_test("Failed recursion case!");
		return false;
	}

	print_test("Passed recursion case\n");

	//Step 2: Test a simple book (from internet)
	SyntacBook *book2 = SyntacBookAllocate();

	SyntacBookRuleAdd(book2, "E", "T:E'"); //0
	SyntacBookRuleAdd(book2, "E'", "+:T:E'"); //1
	SyntacBookRuleAdd(book2, "E'", ""); //2
	SyntacBookRuleAdd(book2, "T", "F:T'"); //3
	SyntacBookRuleAdd(book2, "T'", "*:F:T'"); //4
	SyntacBookRuleAdd(book2, "T'", ""); //5
	SyntacBookRuleAdd(book2, "F", "(:E:)"); //6
	SyntacBookRuleAdd(book2, "F", "id"); //7
	if (book2->rule_count != 8) {
		print_test("SyntacBookRuleAdd failed to add 8 rules as requested?");
		return false;
	}

	firsts_of_book(book2);
	follow_of_book(book2);

	char **set_e_t2 = SetCreate(2, ")", ENDMRKR);
	valid += TEST_SET(book2->rules[0].follow_set, set_e_t2);
	valid += TEST_SET(book2->rules[1].follow_set, set_e_t2);
	valid += TEST_SET(book2->rules[2].follow_set, set_e_t2);
	SetFree(set_e_t2); set_e_t2 = NULL;

	char **set_t_t2 = SetCreate(3, "+", ")", ENDMRKR);
	valid += TEST_SET(book2->rules[3].follow_set, set_t_t2);
	valid += TEST_SET(book2->rules[4].follow_set, set_t_t2);
	valid += TEST_SET(book2->rules[5].follow_set, set_t_t2);
	SetFree(set_t_t2); set_t_t2 = NULL;

	char **set_f_t2 = SetCreate(4, "*", "+", ")", ENDMRKR);
	valid += TEST_SET(book2->rules[6].follow_set, set_f_t2);
	valid += TEST_SET(book2->rules[7].follow_set, set_f_t2);
	SetFree(set_f_t2); set_f_t2 = NULL;

	if (valid != test_count) {
		print_test("Failed Simple Case");
		return false;
	}

	print_test("Passed Simple Case!\n");

	//Step 3: Test Grammar.stc
	char path[] = "../grammar.stc";
	SyntacBook *book3 = SyntacBookFromFile(path);

	firsts_of_book(book3);
	follow_of_book(book3);

	valid += TEST_SET(book3->rules[0].follow_set, set_start); //func

	char **set_args_t3 = SetCreate(1, "RIGHTPAREN");
	valid += TEST_SET(book3->rules[1].follow_set, set_args_t3); //args
	valid += TEST_SET(book3->rules[2].follow_set, set_args_t3);
	SetFree(set_args_t3); set_args_t3 = NULL;

	valid += TEST_SET(book3->rules[3].follow_set, set_start); //body
	valid += TEST_SET(book3->rules[4].follow_set, set_start);

	char **set_stmnts_t3 = SetCreate(1, "RIGHTCURLY");
	valid += TEST_SET(book3->rules[5].follow_set, set_stmnts_t3); //statements
	SetFree(set_stmnts_t3); set_stmnts_t3 = NULL;
	
	char **set_stmnt_t3 = SetCreate(3, "RIGHTCURLY", "WORD", ENDMRKR);
	valid += TEST_SET(book3->rules[6].follow_set, set_stmnt_t3); //statement
	valid += TEST_SET(book3->rules[7].follow_set, set_stmnt_t3);
	SetFree(set_stmnt_t3); set_stmnt_t3 = NULL;

	char **set_null_t3 = SetCreate(4, "RIGHTPAREN", "RIGHTCURLY", "WORD", ENDMRKR);
	valid += TEST_SET(book3->rules[8].follow_set, set_null_t3);
	SetFree(set_null_t3); set_null_t3 = NULL;

	if (valid != test_count) {
		print_test("Failed file case!");
		return false;
	}

	print_test("Passed file case\n");
	
	char path2[] = "../grammar2.stc";
	SyntacBook *book4 = SyntacBookFromFile(path2);

	firsts_of_book(book4);
	follow_of_book(book4);

	valid += TEST_SET(book4->rules[0].follow_set, set_start); //S
	valid += TEST_SET(book4->rules[1].follow_set, set_start); //S
	valid += TEST_SET(book4->rules[2].follow_set, set_start); //S
	
	char **set_a1_b4 = SetCreate(3, "h", "g", ENDMRKR);
	valid += TEST_SET(book4->rules[3].follow_set, set_a1_b4);
	valid += TEST_SET(book4->rules[4].follow_set, set_a1_b4);
	SetFree(set_a1_b4); set_a1_b4 = NULL;

	char **set_b1_b4 = SetCreate(4, ENDMRKR, "a", "h", "g");
	valid += TEST_SET(book4->rules[5].follow_set, set_b1_b4);
	valid += TEST_SET(book4->rules[6].follow_set, set_b1_b4);
	SetFree(set_b1_b4); set_b1_b4 = NULL;

	char **set_c1_b4 = SetCreate(4, "g", ENDMRKR, "b", "h");
	valid += TEST_SET(book4->rules[7].follow_set, set_c1_b4);
	valid += TEST_SET(book4->rules[8].follow_set, set_c1_b4);
	SetFree(set_c1_b4); set_c1_b4 = NULL;

	valid += TEST_SET(book4->rules[9].follow_set, set_start);

	print_test("Finished all cases!");
	SetFree(set_start); set_start = NULL;
	return valid != test_count;
}

#ifndef ALL_TESTS
int test_count = 0;
int main(void) {
	//warn_level = HLT_DEBUG;
	return !FollowTest();
}
#endif
