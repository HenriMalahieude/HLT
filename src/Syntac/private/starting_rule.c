#include "syntac_internal.h"

int find_starting_rule_from_none(struct stc_book *book) {
	int start_rule = -1;	

	for (int i = 0; i < book->rule_count; i++) {
		bool contained = false;
		struct stc_rule *rule1 = book->rules + i;
		for (int j = 0; j < book->rule_count; j++) {
			if (i == j) continue;
			struct stc_rule *rule2 = book->rules + j;

			contained = SetContains(rule2->elements, rule1->name);
			if (contained) break;
		}
		if (!contained) {
			if (start_rule < 0) start_rule = i;
			else HLT_ERR("Grammar has multiple starting/top-level rules (#%d & #%d)? (Nonterm not in any other production)", start_rule, i);
		}
	}

	return start_rule;
}

int find_starting_rule_from_follows(struct stc_book *book) {
	char * id[2] = {ENDMRKR, NULL}; //no need to use heap here
	
	int start_rule = -1;
	for (int i = 0; i < book->rule_count; i++) {
		if (SetEquality(id, book->rules[i].follow_set)) { //identified the starting rule
			if (start_rule != -1) HLT_ERR("Grammar has multiple starting/top-level rules? (Follow Set = { $ })");
			start_rule = i;
		}
	}
	
	return start_rule;
}

int find_starting_rule(struct stc_book *book) {
	if (book == NULL) HLT_AERR("Got null book?");
	if (book->rule_count <= 0) return 0;

	int start_rule = -1;
	if (book->rules[0].follow_set != NULL) 	start_rule = find_starting_rule_from_none(book);
	else 					start_rule = find_starting_rule_from_follows(book); //faster/easier to calculate

	if (start_rule == -1) {
		HLT_WRN(HLT_STDWRN, "Could not identify a starting symbol for grammar. Defaulting to first rule.");
		return 0;
	}

	return start_rule;
}
