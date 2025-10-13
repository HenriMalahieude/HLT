#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "syntac_internal.h"

int *tracker = NULL;
int firsts_loop_limit = 0;

void firsts_of_rule(struct stc_book *book, int rule_index) {
	if (book == NULL) HLT_ERR("Book supplied is null?");
	if (rule_index < 0 || rule_index >= book->rule_count) HLT_ERR("Rule supplied is null?");

	struct stc_rule *rule = &book->rules[rule_index];

	if (tracker == NULL) HLT_AERR("Internal error, tracking state for 'dynamic' programming failed?");
	if (tracker[rule_index] >= firsts_loop_limit) return; //already calculated for this loop cycle
	tracker[rule_index] = firsts_loop_limit; //about to calculate it for this loop cycle

	//epsilon/null/empty rules
	if (rule->elements == NULL || rule->elements[0] == NULL) {
		SetAdd(&rule->first_set, EPSILON);
		return;
	}

	bool prev_epsi = false; //for recursion
	for (int i = 0; rule->elements[i] != NULL; i++) {
		char *elm = rule->elements[i];

		if (is_terminal(book, elm)) {
			SetAdd(&rule->first_set, elm); //Rule 1: First Element is a Token
			break;
		}
		
		//Rule 2: First Element is a Rule
		//Check each other (matching name/left) rule and get their first sets
		char **new_first = NULL;
		for (int j = 0; j < book->rule_count; j++) {
			if (strcmp(book->rules[j].name, elm) != 0) continue;
			if (rule_index == j) continue; //loops into same rule are ignored (a -> a b a c, only b & c considered unless there is another 'a' rule)
			
			firsts_of_rule(book, j);
			if (book->rules[j].first_set == NULL) continue; //failed to actually generate a first set, so we can't acquire it ourselves
			
			//At this point in the code:
			//  It's the rule we are looking for,
			//	not ourselves (rule), 
			//	and has a properly generated first set we can acquire

			//Acquire the first of this rule, add it to ourselves
			char **lcl_first = SetUnion(new_first, book->rules[j].first_set);
			
			if (new_first != NULL) SetFree(new_first);
			new_first = lcl_first;
		}

		bool cur_self_ref = (strcmp(rule->elements[i], rule->name) == 0);
		bool done = (!prev_epsi || !cur_self_ref);

		if (new_first != NULL) {
			//Rule 2.5: Rule located has Epsilon in it, check follow up element
			if (SetContains(new_first, EPSILON)) {
				done = false;
				prev_epsi = true;

				if (rule->elements[i+1] != NULL) {
					if (!SetRemove(&new_first, EPSILON)) HLT_AERR("Contains epsilon, but not removed?!?!");
				}

			//Rule 2.5: Rule is ourselves, and we had Epsi before, check follow up
			} else if (cur_self_ref && prev_epsi) {
				if (rule->elements[i+1] == NULL) SetAdd(&new_first, EPSILON); //last elm, add epsi as a possible first
				else done = false; //or check nxt

			} else {
				prev_epsi = false;
			}

			char **join = SetUnion(rule->first_set, new_first);
			if (rule->first_set != NULL) SetFree(rule->first_set);
			SetFree(new_first); new_first = NULL;

			rule->first_set = join;
		} else {
			//Rule 2.5: Rule is ourselves, and we had Epsi before, check follow up
			if (cur_self_ref && prev_epsi) {
				if (rule->elements[i+1] == NULL) SetAdd(&rule->first_set, EPSILON); //last elm, add epsi
				else done = false; //chk nxt
			} else {
				prev_epsi = false;
			}
		}

		if (done) break;
	}

	//NOTE: may be (rule->first_set == NULL) because it's waiting on other rules before it can calculate itself
}

void firsts_of_book(struct stc_book *book) { //local use, all asserts/ifs should error
	if (book == NULL) HLT_ERR("Book supplied is null?");

	HLT_WRN(HLT_DEBUG, "Calculating firsts of this book!");

	if (tracker != NULL) HLT_AERR("Failed to unallocate the tracker array properly?");
	tracker = calloc(book->rule_count, sizeof(int)); //calloc sets all to zero
	
	bool changed = true;
	while (changed) {
		if (firsts_loop_limit < FIRSTS_CHG_LIM) firsts_loop_limit++;
		else HLT_ERR("Recursion limit hit on changed status of first sets within grammar.\nTODO: Make this a cmd line arg");

		changed = false;
		for (int i = 0; i < book->rule_count; i++) { 
			int set_sz = SetCount(book->rules[i].first_set);
			firsts_of_rule(book, i);
		
			if (set_sz != SetCount(book->rules[i].first_set)) changed = true;
		}
	}
	
	//Then we check that we got everything to have a firsts set
	bool failed = false;
	for (int i = 0; i < book->rule_count; i++) {
		if (book->rules[i].first_set == NULL) {
			HLT_WRN(HLT_MJRWRN, "Couldn't generate the first set of rule '%s'?", book->rules[i].name);
			failed = true;
		}
	}
	
	//TODO: Some way to control this failed signal, and firsts_loop_limit
	if (failed) HLT_ERR("Some first sets were not generated, not told to ignore.");

	HLT_WRN(HLT_VERBSE, "Firsts took %d loops to calculate", firsts_loop_limit);
	firsts_loop_limit = 0;
	free(tracker); tracker = NULL;
}
