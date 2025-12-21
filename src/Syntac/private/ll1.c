#include <stdlib.h>
#include <string.h>

#include "../../Lexic/lexic.h"

#include "syntac_internal.h"
#include "ll1.h"

struct stc_ll1_table * LL1TableAlloc() {
	struct stc_ll1_table *ll1_table = (struct stc_ll1_table *)malloc(sizeof(struct stc_ll1_table));
	ll1_table->entries = NULL;
	ll1_table->entry_cnt = 0;

	return ll1_table;
}

void LL1TableFree(struct stc_ll1_table *tbl) {
	for (int i = 0; i < tbl->entry_cnt; i++) {
		struct stc_ll1_entry *ent = tbl->entries + i;
		free(ent->nonterm);
		SetFree(ent->term);
		ent->rule_idx = 0;
	}

	free(tbl->entries);
}

//New Entry
void LL1TableEntryAdd(struct stc_ll1_table *tbl, char *nontrm, char *trm, int rule_index) {
	tbl->entries = (struct stc_ll1_entry *)realloc(tbl->entries, sizeof(struct stc_ll1_entry) * ++(tbl->entry_cnt));
	if (tbl->entries == NULL) HLT_ERR("Realloc failed to allocate?");
	
	struct stc_ll1_entry *entry = tbl->entries + (tbl->entry_cnt-1);
	
	entry->nonterm = (char *)calloc(strlen(nontrm)+1, sizeof(char));
	if (entry->nonterm == NULL) HLT_ERR("Calloc failed to allocate?");
	strcpy(entry->nonterm, nontrm);
	
	entry->term = SetCreate(1, trm);
	
	entry->rule_idx = rule_index;
}

//Update Entry
void LL1TableEntryInsert(struct stc_ll1_table *tbl, char *nontrm, char *trm, int rule_index) {
	struct stc_ll1_entry *match = NULL;	
	for (int i = 0; i < tbl->entry_cnt; i++) {
		struct stc_ll1_entry *entry = tbl->entries + i;
		if (entry->rule_idx == rule_index && strcmp(entry->nonterm, nontrm) == 0) {
			match = entry;
			break;
		}
	}

	if (!match) LL1TableEntryAdd(tbl, nontrm, trm, rule_index);
	else {
		if (SetContains(match->term, trm)) HLT_WRN(HLT_STDWRN, "Terminal (%s) already exists for LL1 Entry of Rule #%d (%s)?", trm, rule_index, nontrm);
		SetAdd(&match->term, trm);
	}
}

void LL1TableCalculate(SyntacBook *book) {
	if (book->type != STC_LL1) HLT_AERR("Attempted to calculate LL1 table for non-LL1 grammar book?");
	HLT_WRN(HLT_VERBSE, "Generating LL1 Parsing Table for the book!");

	//Validate
	for (int i = 0; i < book->rule_count; i++) {
		struct stc_rule *rule = book->rules + i;

		//Rule 1: No Left Recursion (A -> A : B)
		for (int j = 0; rule->elements[j] != NULL; j++) {
			if (strcmp(rule->elements[j], rule->name) == 0 && rule->elements[j+1] != NULL)
				HLT_ERR("LL1 Grammar has left recursion on %s (#%d). Cannot calculate table.", rule->name, i);
		}
	}

	//Generate entries
	book->ll1_table = LL1TableAlloc();
	
	//For each rule
	for (int i = 0; i < book->rule_count; i++) {
		struct stc_rule *rule = book->rules + i; 

		bool epsi_rule = false; //SetContains(rule->first_set, EPSILON); //for each element in the first set, create an entry
		for (int j = 0; rule->first_set[j] != NULL; j++) { 
			if (strcmp(rule->first_set[j], EPSILON) == 0) {
				epsi_rule = true;
				continue;
			}
			LL1TableEntryInsert(book->ll1_table, rule->name, rule->first_set[j], i);
			HLT_WRN(HLT_DEBUG, "%s & %s apply %d (firsts)", rule->name, rule->first_set[j], i);
		}

		//If epsilon in firsts, then we must apply rule to all follows as well
		if (epsi_rule) {
			for (int j = 0; rule->follow_set[j] != NULL; j++) { //may contain ENDMRKR, which is intentional behavior
				LL1TableEntryInsert(book->ll1_table, rule->name, rule->follow_set[j], i);
				HLT_WRN(HLT_DEBUG, "%s & %s apply %d (epsi? %d)", rule->name, rule->follow_set[j], i, (int)epsi_rule);
			}
		}
	}

	//Validate the table, make sure no rules overlap
	bool ambiguity = false;
	for (int i = 0; i < book->ll1_table->entry_cnt; i++) {
		struct stc_ll1_entry *ent1 = book->ll1_table->entries + i;
		for (int j = i+1; j < book->ll1_table->entry_cnt; j++) {
			struct stc_ll1_entry *ent2 = book->ll1_table->entries + j;
			if (ent1->rule_idx != ent2->rule_idx 
					&& strcmp(ent1->nonterm, ent2->nonterm) == 0 
					&& SetOverlaps(ent1->term, ent2->term)) {
				if (warn_level >= HLT_VERBSE) {
					printf("Rule %d:\n", ent1->rule_idx);
					SetPrint(ent1->term);

					printf("Rule %d:\n", ent2->rule_idx);
					SetPrint(ent2->term);
				}

				HLT_WRN(HLT_MJRWRN, "Rule %d (%s) and %d (%s) have ambiguity (both rules can be applied when nonterm and specific term on stack)", ent1->rule_idx, ent1->nonterm, ent2->rule_idx, ent2->nonterm);
				ambiguity = true;
			}
		}
	}

	if (ambiguity) HLT_ERR("There was ambiguity with this LL1 Grammar. Fatal.");
}

/*
//The only dynamic mem part is the node itself
struct stc_tree_node * InitTreeNode(enum stc_parsing_style typ, char *rule) {
	struct stc_tree_node *node = (struct stc_tree_node *)malloc(sizeof(struct stc_tree_node));

	node->type = typ;
	node->children = NULL;
	node->rule_name = rule;
	node->is_token = false;
	node->line_start = node->line_end = 0;
	node->col_start = node->col_end = 0;
}


int LL1RuleFind(struct stc_book *book, char *stack_top, char *look_ahead) {
	for (int i = 0; i < book->ll1_table->entry_cnt; i++) {
		struct stc_ll1_entry *ent = book->ll1_table->entries + i;
		
		if (strcmp(ent->nonterm, stack_top) == 0 && strcmp(ent->term, look_ahead) == 0) {
			return ent->rule_idx;
		}
	}

	return -1;
}

struct stc_tree_node * LL1RescursiveParse(struct lxc_token *stream, struct stc_book *book, int stk_top, int pos) {
	if (stk_top < 0) HLT_AERR("Stack top negative? (%d)", stk_top);
	if (pos < 0) HLT_AERR("Stream pos negative? (%d)", pos);

	struct lxc_token *token = stream + pos;
	if (token->definition_name == NULL) return NULL;
	
	char *stk_nme = book->rules[stk_top]->name;
	if (rule == NULL) HLT_AERR("Was supplied NULL Rule???");
	
	int to_apply = LL1RuleFind(book, stk_nme, token->definition_name);
	if (to_apply < 0) to_apply = LL1RuleFind(book, stk_name, EPSILON);
	if (to_apply < 0) HLT_ERR("LL1 Grammar has no destruction rule (epsilon) for '%s'. Next token: '%s'", stk_nme, token->definition_name);

	struct stc_rule *rule_to_apply = book->rules[to_apply];
	if (rule_to_apply == NULL) HLT_AERR("Located rule was null?");
	
	//struct stc_tree_node *cur = InitTreeNode(STC_LL1, stk_nme);

	int cnt = SetCount(rule_to_apply->elements); //kind of an abuse of the func, but whatever
	for (int i = (cnt-1); i >= 0; i--) {
		char *elm = rule_to_apply->elements[i];
		//TODO: check if token, otherwise, call recursive func
	}
}

struct stc_tree_node * TopParseTokens(struct lxc_token *stream, SyntacBook *book) {
	if (book->ll1_table == NULL) LL1TableCalculate(book);

	char **stack[SUBSTR_SZ]; //we will not be dynamically creating memory here
	int stk_ptr = -1;
	
	int start_rule = FindStartingRule();

	//First element on stack must be starting rule
	stack[++stk_ptr] = book->rules[start_rule].name;
	struct stc_tree_node *top = InitTreeNode(STC_LL1, stack[stk_ptr]);
	struct stc_tree_node *cur = top;

	for (int i = 0; stream[i] != NULL; i++) {
		if (stk_ptr < 0) {
			HLT_WRN(HLT_STDWRN, "Finished Parsing, but not the end of the stream?");
			break;
		} else if (stk_ptr > (SUBSTR_SZ - 1)) 
			HLT_AERR("Reached the stack limit on LL1 Parsing, aborting. (%d)", SUBSTR_SZ);

		//char *stack_top = stack[stk_ptr];
		//char *char_for = stream[i];

		int ridx = LL1RuleFind(book, stack[stk_ptr], stream[i]);

	}

	if (stk_ptr > 0) HLT_ERR("Finished Stream, but not done Parsing? Top of stack: '%s'", stack[stk_ptr]);
	else if (stk_ptr == 0) {
		int ridx = LL1RuleFind(book, stack[stk_ptr], ENDMRKR);
		if (ridx < 0) HLT_ERR("Finished Stream, Parsing on Last elm, but no EOF ($) rule for '%s'", stack[stk_ptr]);
	}
} */
