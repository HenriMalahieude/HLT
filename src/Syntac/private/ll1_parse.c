#include <stdlib.h>
#include <string.h>

#include "../../Lexic/lexic.h"

#include "syntac_internal.h"
#include "ll1.h"

//The only dynamic mem part is the node itself
struct stc_tree_node * InitTreeNode(enum stc_parsing_style typ, char *rule) {
	struct stc_tree_node *node = (struct stc_tree_node *)malloc(sizeof(struct stc_tree_node));
	if (node == NULL) HLT_ERR("Failed to malloc?");

	node->type = typ;
	node->children = NULL;
	node->rule_name = rule; //jus a ref
	node->is_token = false;
	node->line_start = node->line_end = 0;
	node->col_start = node->col_end = 0;
}

//zeros everything and then deletes
void FreeTreeNode(struct stc_tree_node *node) {
	node->type = (enum stc_parsing_style)0;
	node->rule_name = NULL;
	node->is_token = false;
	node->line_start = node->line_end = 0;
	node->col_start = node->col_end = 0;
	
	//TODO:
	//for each node->children: free();
	node->children = NULL;

	free(node);
}

struct stc_tree_node * LL1Parse(struct stc_book *book, void *stream, bool isTokens) {
	
}

struct stc_tree_node * LL1ParseTokens(struct stc_book *book, void *stream) {
	return LL1Parse(book, stream, true);
}
struct stc_tree_node * LL1ParseStream(struct stc_book *book, char **stream) {
	return LL1Parse(book, stream, false);
}

/*


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
