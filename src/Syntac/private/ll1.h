#include "../syntac.h" //SyntacBook && stc_tree_node

#ifndef __LL1_H__
#define __LL1_H__

//Parsing Table Entry 2d
//One row in a parsing table, 
//	nonterm is the rule name (on left)
//	term is all the terminals in which the "rule_idx" is applied to when both nonterm and then this term is seen
struct stc_ll1_entry {
	int rule_idx;
	char *nonterm;
	char **term;  //set
};

//within stc_book
struct stc_ll1_table {
	struct stc_ll1_entry *entries;
	int entry_cnt;
};

void LL1TableCalculate(SyntacBook *book);
void LL1TableFree(struct stc_ll1_table *tbl);

struct stc_tree_node * LL1ParseTokens(struct stc_book *book, void *stream);
struct stc_tree_node * LL1ParseStream(struct stc_book *book, char **stream);

#endif
