#include <stdlib.h>
#include <string.h>

#include "syntac_internal.h"

//Allocate an Empty Rule Book
SyntacBook * SyntacBookAllocate() {
	SyntacBook *book = (SyntacBook*)malloc(sizeof(struct stc_book));
	if (book == NULL) HLT_ERR("Attempted malloc of a book failed?");
	
	book->type = STC_NON;
	book->rules = NULL;
	book->rule_count = 0;

	//This is a union, so it covers everything
	book->ll1_table = NULL;

	return book;
}

//Free a rule book (assumes everything within the rulebook and rules is dynamically allocated)
void SyntacBookFree(SyntacBook *book) {
	if (book == NULL) {
		HLT_WRN(HLT_MJRWRN, "Supplied book pointer was null?");
		return;
	}

	if (book->ll1_table != NULL) {
		if (book->type == STC_LL1) LL1TableFree(book->ll1_table);
		else HLT_AWRN(HLT_MJRWRN, "TODO: Parsing Table Freeing");
	}
	
	//Rule is not dynamically allocated, don't free it
	// see SyntacBookRuleAdd
	for (int i = 0; i< book->rule_count; i++){
		struct stc_rule *rule = &book->rules[i];
		if (rule == NULL) {
			HLT_WRN(HLT_STDWRN, "Rule pointer was null?");
			continue;
		}

		free(rule->name); rule->name = NULL;
		for (int j = 0; rule->elements[j] != NULL; j++) {
			free(rule->elements[j]); //free all elements inside
			rule->elements[j] = NULL;
		}
		free(rule->elements); rule->elements = NULL; //free container
		
		//Free the sets we allocated
		if (rule->first_set != NULL) SetFree(rule->first_set);
		if (rule->follow_set != NULL) SetFree(rule->follow_set);
		rule->first_set = NULL;
		rule->follow_set = NULL;
	}

	if (book->rules != NULL) free(book->rules); 
	book->rules = NULL; book->rule_count = 0;
	free(book);
	
	//HLT_AWRN(HLT_MJRWRN, "TODO: lr1 table freeing!");
}	

void SyntacBookRuleAdd(SyntacBook *book, char *left, char *right) {
	if (book == NULL) {
		HLT_WRN(HLT_MJRWRN, "Supplied book pointer was null?");
		return;
	}

	if (left == NULL || left[0] == '\0') {
		HLT_WRN(HLT_MJRWRN, "Supplied 'left' of rule was null/empty?");
		return;
	}

	if (right == NULL) {
		HLT_WRN(HLT_MJRWRN, "Supplied 'right' of rule was null/empty?");
		HLT_WRN(HLT_STDWRN, "If attempting an epsilon/empty rule, please pass empty string instead.");
		return;
	}

	//Count Amount of Elements (delimited by RIGHT_DELIM)
	int elm_cnt = 1;
	for (size_t i = 0; i < strlen(right); i++) if (right[i] == RIGHT_DELIM) elm_cnt++;

	//Initialize the rule + name
	struct stc_rule rule = {0}; 
	rule.name = NULL;
	rule.elements = NULL;
	rule.first_set = NULL;
	rule.follow_set = NULL;
	if (!trim(left, &rule.name)) {
		HLT_WRN(HLT_MJRWRN, "Attempted to insert rule with only whitespace for name?");
		return;
	}
	
	//Allocate amount of elements detected
	rule.elements = (char**)calloc((elm_cnt+1), sizeof(char *));//calloc nulls out entire thing

	//Parse the elements now
	int elm_i = 0;
	size_t nconsumeIdx = 0;
	char substr[SUBSTR_SZ];
	for (size_t i = 1; i <= strlen(right); i++) {
		int len = 0;
		if (right[i] == RIGHT_DELIM || right[i] == '\0') len = (i - nconsumeIdx); //non-inclusive

		if (len <= 0) continue;

		strncpy(substr, right + nconsumeIdx, len);
		substr[len] = '\0';
		if (!trim(substr, &rule.elements[elm_i])) {
			HLT_WRN(HLT_VERBSE, "Element was only whitespace? Skipping.");
			//strcpy(rule.elements[elm_i], substr);
		}

		elm_i += 1;
		nconsumeIdx = i+1; //skip delim
	}

	if (elm_i < elm_cnt) HLT_WRN(HLT_VERBSE, "Rulebook attempted to add '%s' (%d prev) expected %d elements, got %d?", left, book->rule_count, elm_cnt, elm_i);

	//Place into the book, unless you have a better idea, we will be reallocing 1 by 1
	book->rules = (struct stc_rule *)realloc(book->rules, sizeof(struct stc_rule) * ++(book->rule_count)); 
	book->rules[book->rule_count-1] = rule;
}

SyntacBook * SyntacBookFromFile(char *file_name){
	if (file_name == NULL || file_name[0] == '\0') {
		HLT_WRN(HLT_MJRWRN, "Supplied file_name was null/empty?");
		return NULL;
	}

	char *cntnts = ftostr(file_name);
	if (cntnts == NULL) {
		HLT_WRN(HLT_MJRWRN, "Could not convert file to string?");
		return NULL;
	}

	int nl = 0; //new line location
	for (nl = 0; cntnts[nl] != '\0' && cntnts[nl] != '\n'; nl++);
	if (nl == 0) HLT_ERR("File had no type specifier? (Or was empty?)");

	if (nl >= SUBSTR_SZ) HLT_ERR("Type specifier line for file was longer (%d) than substring max (%d)?", nl, SUBSTR_SZ);
	char substr[SUBSTR_SZ];
	strncpy(substr, cntnts, nl);
	substr[nl] = '\0';

	enum stc_parsing_style type = STC_NON;
	if 	(strncmp(substr, "LL1", 3) == 0) type = STC_LL1;
	else if (strncmp(substr, "LR1", 3) == 0) type = STC_LR1;
	else {
		HLT_WRN(HLT_MJRWRN, "Unimplemented parsing style: '%s'", substr);

		free(cntnts);
		return NULL;
	}
		
	SyntacBook *book = SyntacBookFromString(cntnts + nl + 1, type); //past the new line
	
	free(cntnts);
	return book;
}

SyntacBook * SyntacBookFromString(char *stream, SyntacParseType type) {
	if (stream == NULL || stream[0] == '\0') {
		HLT_WRN(HLT_MJRWRN, "Provided stream empty?");
		return NULL;
	}

	int lin = 0, col = 0;
	struct stc_book *book = SyntacBookAllocate();
	book->type = type;

	char substrn[SUBSTR_SZ]; substrn[0] = 0; //name or left consumption
	char substre[SUBSTR_SZ];		   		//elms or right production
	size_t nconsumeIdx = 0;
	int slen = strlen(stream);
	for (int i = 0; i <= slen; i++) {
		if (strncmp(stream+i, "->", 2) == 0) { //rule name found
			//HLT_WRNLC("Found Name!", lin, col, HLT_DEBUG);

			int sublen = i - nconsumeIdx;
			if (sublen <= 0) HLT_ERRLC(lin, col, "Empty Rule Name.");
			if (sublen >= SUBSTR_SZ) HLT_ERRLC(lin, col, "Syntac does not support names longer than SUBSTR_SZ characters!");

			strncpy(substrn, stream+nconsumeIdx, sublen);
			substrn[sublen] = '\0';
			nconsumeIdx = i+2;
			
			HLT_WRN(HLT_DEBUG, "Name found: '%s'\n", substrn);
		} else if (stream[i] == '\n' || stream[i] == '\0' || stream[i] == EOF) { //new line or eofS
			if (substrn[0] == '\0') {
				nconsumeIdx = i+1;
				substrn[0] = '\0';
				substre[0] = '\0';
				lin++; col = -1;
				continue; //no name, no rule
			}
			HLT_WRNLC(lin, col, HLT_DEBUG, "Found Definition.");

			int sublen = i - nconsumeIdx;
			if (stream[i] != '\n') sublen++; //include last char
			if (sublen <= 0) HLT_WRNLC(lin, col, HLT_VERBSE, "Empty Rule Definition.");
			if (sublen >= SUBSTR_SZ) HLT_ERRLC(lin, col, "Syntac does not support definitions longer than SUBSTR_SZ characters!");

			if (sublen > 0) strncpy(substre, stream+nconsumeIdx, sublen);
			substre[sublen] = 0;
			
			int rl_b4 = book->rule_count;
			SyntacBookRuleAdd(book, substrn, substre);
			if (book->rule_count <= rl_b4) HLT_WRNLC(rl_b4, -1, HLT_VERBSE, "Failed to add a rule?");
			HLT_WRN(HLT_DEBUG, "Definition found: '%s'\n", substre);

			//Reset
			nconsumeIdx = i+1;
			substrn[0] = '\0';
			substre[0] = '\0';
			lin++; col = -1;
		}
		col++;
	}

	return book;
}

