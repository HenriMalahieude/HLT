#ifndef __HEPH_INTERNAL_PRIVATE_LEXIC__
#define __HEPH_INTERNAL_PRIVATE_LEXIC__

#include "../lexic.h"

//Helpers
#include "string/handy.h"
#include "log/warn.h"

extern int regex_line_no, regex_colu_no;

/*Definition of a Token*/
struct lxc_definition {
	char *name;
	char *regex;
};

/*Entire Vocabulary Rules*/
struct lxc_vocabulary {
	struct lxc_definition *definitions;
	int def_count;
};

bool RegexValidate(char *reg);

bool RegexMatch(char *reg, char *input);

#endif
