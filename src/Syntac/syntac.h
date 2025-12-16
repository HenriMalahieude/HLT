#ifndef __HEPHAESTUS_SYNTACTICAL_ANALYZER__
#define __HEPHAESTUS_SYNTACTICAL_ANALYZER__
#include <stdbool.h>

//Public facing container of grammatical rules
typedef struct stc_book SyntacBook;

//Types of Grammar Trees
typedef enum stc_parsing_style {
	STC_NON = 0,
	STC_LL1,	//WIP
	STC_LR1,	//Unimplemented
	STC_EARLEY, 	//Unimplemented
	STC_PARSE_CNT,
} SyntacParseType;

typedef struct stc_tree_node {
	SyntacParseType type;
	struct stc_tree_node *children; //null-terminated
	char *rule_name; //Grammar Rule Name
	bool is_token; //grammar node, or token leaf

	int line_start, col_start;
	int line_end,   col_end;
} SyntacTreeNode;

/*----------------------------Books----------------------------*/

//Allocate and Populate a rule book from a file
//Parse type must be on first line of file
SyntacBook * SyntacBookFromFile(char *file_name);

//Allocate and Populate a rule book from a string stream
SyntacBook * SyntacBookFromString(char *stream, SyntacParseType type);

//Validate grammar book can be built into table
//TODO: check that this is needed
//bool SyntacBookValidate(SyntacBook *);

//Allocate an empty rule book
SyntacBook * SyntacBookAllocate();

//Add a new definition to a rule book (dynamic allocation of left and right)
void SyntacBookRuleAdd(SyntacBook *book, char *left, char *right);

//Free a rule book
void SyntacBookFree(SyntacBook *);

/*----------------------------Trees----------------------------*/

//Generate a Syntactical Tree representation of tokens 
//Expects token stream to be null-terminated (stream[x].name == NULL || stream[x].name[0] == '\0')
typedef struct lxc_token LexicToken;
SyntacTreeNode * SyntacTreeFromTokens(LexicToken *stream, SyntacBook *book);

//NOTE: What value could this have if you don't have the raw item?
//Generate a Syntactical Tree representation of string stream
//Expects token string stream to be null-terminated (stream[x] == NULL || stream[x][0] == 0)
//SyntacTreeNode * SyntacTreeFromStream(char **stream, SyntacBook *book);

//Free a null-terminated Syntactical Tree
void SyntacTreeFree(SyntacTreeNode *);

#endif
