#ifndef __LL1_H__
#define __LL1_H__

//Parsing Table Entry 2d, nonterm row match, term column match, apply rule_index at loc 
struct ll1_entry {
	char *nonterm;
	char *term;
	int rule_idx;
};

//within stc_book
struct stc_ll1_table_entry {
	struct ll1_entry *entries;
	int entry_cnt;
};

#endif
