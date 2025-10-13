#include "../syntac.h"

//Helpers
#include "sets/sets.h"
#include "string/handy.h"
#include "log/warn.h"

#ifndef __HEPH_INTERNAL_PRIVATE_SYNTAC__
#define __HEPH_INTERNAL_PRIVATE_SYNTAC__

// separator for the right production of rules
#define RIGHT_DELIM ':'

//Using chars that should be impossible to acquire
#define EPSILON "\1" //Used for firsts (empty string)
#define ENDMRKR "\2" //Used for follows (end of string/eof)

//Limitations
#define FIRSTS_CHG_LIM 1000
#define SUBSTR_SZ 200

//Rule Definition
// left -> right 
// a -> b : c:xx :
// which is [ 'a' consumes tokens 'b' 'c' 'xx' in sequence ]
struct stc_rule {
	char *name; //left,
	char **elements; //right, production (NULL-terminated)

	char **first_set; //set, see sets.h
	char **follow_set; //diddo
};

//Parsing Tables
struct stc_ll1_table_entry; //ll1.h
struct stc_lr1_table_entry; //lr1.h

//Rule Book
struct stc_book {
	enum stc_parsing_style type;
	struct stc_rule *rules;
	int rule_count;

	union {
		struct stc_ll1_table_entry *ll1_table;
		struct stc_lr1_table_entry *lr1_table;
	};
};

bool is_terminal(struct stc_book *book, char *element); //terminal.c

void firsts_of_book(struct stc_book *book); //firsts.c
void follow_of_book(struct stc_book *book); //follows.c

//ll1.c
struct stc_tree_node * LL1ParseTokens(struct stc_book *book, void *stream);
struct stc_tree_node * LL1ParseStream(struct stc_book *book, char **stream);

//lr1.c
struct stc_tree_node * LR1ParseTokens(struct stc_book *book, void *stream);
struct stc_tree_node * LR1ParseStream(struct stc_book *book, char **stream);
#endif
