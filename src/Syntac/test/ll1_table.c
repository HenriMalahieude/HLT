#define TEST_NAME "LL1_Table"
#include "syn_helper.h"

bool LL1TableTest() {
	test_count = 0;
	int valid = 0;

	print_test("Start!");

	//Trivial Case
	SyntacBook *book1 = SyntacBookAllocate();
	SyntacBookRuleAdd(book1, "A", "B"); //A -> B
	SyntacBookRuleAdd(book1, "A", "C");//A ->  C
	SyntacBookRuleAdd(book1, "C", "D:E");  //C -> D E
	SyntacBookRuleAdd(book1, "D", "");
	if (book1->rule_count != 3) {
		print_test("SyntacBookRuleAdd failed to add 3 rules as requested?");
		return false;
	}

	firsts_of_book(book1);
	//Test my math
	char **set = SetCreate(1, "B");
	valid += TEST_SET(book1->rules[0].first_set, set);
	SetFree(set); set = NULL;

	set = SetCreate(1, EPSILON);
	valid += TEST_SET(book1->rules[1].first_set, set);
	valid += TEST_SET(book1->rules[2].first_set, set);
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
	SetFree(set); set = NULL;

	set = SetCreate(1, "E");
	valid += TEST_SET(book1->rules[2].follow_set, set);
	valid += TEST_SET(book1->rules[3].follow_set, set);
	SetFree(set); set = NULL;
	if (valid != test_count) {
		print_test("Failed Trivial Case, Follow Sets?");
		return false;
	}

	LL1TableCalculate(book1);
	//TODO: Test the Trivial Case
	
	SyntacBookFree(book1); book1 = NULL;
	if (valid != test_count) {
		print_test("Failed Trivial Case");
		return false;
	}

	print_test("Passed Trivial Case\n");
	return true;

	//GeeksForGeeks Case (from ll1 parsing table example)
	book1 = SyntacBookAllocate();
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

	//TODO: Test the Internet Case
	
	SyntacBookFree(book1); book1 = NULL;
	if (valid != test_count) {
		print_test("Failed GeeksForGeeks Case");
		return false;
	}

	print_test("Passed GeeksForGeeks Case");

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
