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
