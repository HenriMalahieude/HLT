#ifndef __LL1_H__
#define __LL1_H__

//Parsing Table Entry 2d
//One row in a parsing table, 
//	nonterm is the rule name (on left)
//	term is all the terminals in which the "rule_idx" is applied to when both nonterm and then this term is seen
struct ll1_entry {
	char *nonterm;
	char **term;
	int rule_idx;
};

//within stc_book
struct stc_ll1_table_entry {
	struct ll1_entry *entries;
	int entry_cnt;
};

#endif
