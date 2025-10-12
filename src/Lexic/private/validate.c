#include <string.h>

#include "lexic_internal.h"

//Takes control of regex_colu_no
bool RegexValidate(char *reg) {
	int paren_count = 0;
	int bracket_count = 0;
	for (size_t i = 0; i < strlen(reg); i++) {
		char prv = (i > 0) ? reg[i-1] : '\0';
		char cur = reg[i];
		char nxt = reg[i+1];

		if (cur == '|') { //or mode
			if (i == 0 || prv == '|' || prv == '(' || nxt == '\0') {
				HLT_WRNLC(regex_line_no, regex_colu_no, HLT_MJRWRN, "This lexical analyzer does not support empty/0-length or expressions.");
				return false;
			} else if (prv == '*' || prv == '?') {
				HLT_WRNLC(regex_line_no, regex_colu_no, HLT_STDWRN, "This or statement may never evaluate due to '*' or '?' qualifier.");
			}
		}


		if (cur == '[' && prv != '\\') { 
			bracket_count++; 
			if (bracket_count > 1) {
				HLT_WRNLC(regex_line_no, regex_colu_no, HLT_MJRWRN, "This lexical analyzer does not support nested brackets. Did you mean '\\['?");
				return false;
			}
		} else if (cur == ']' && prv != '\\') {
			if (prv == '[') {
				HLT_WRNLC(regex_line_no, regex_colu_no, HLT_MJRWRN, "This lexical analyzer does not support empty/0-length expressions. Like '[]'.");
				return false;
			}
		
			bracket_count--;
			
			if (bracket_count < 0) {
				HLT_WRNLC(regex_line_no, regex_colu_no, HLT_MJRWRN, "Regex did not properly begin a brackets expression. Missing Left Bracket '['!");
				return false;
			}
		}

		if (cur == '(' && prv != '\\') { 
			paren_count++;
		} else if (cur == ')' && prv != '\\') {
			if (prv == '(') {
				HLT_WRNLC(regex_line_no, regex_colu_no, HLT_MJRWRN, "This lexical analyzer does not support empty/0-length expressions. Like '()'.");
				return false;
			}

			paren_count--;

			if (paren_count < 0) {
				HLT_WRNLC(regex_line_no, regex_colu_no, HLT_MJRWRN, "Regex did not properly begin a group structure. Missing Left Parenthesis '('!");
				return false;
			}
		}

		if (prv != '\\' && (cur == '*' || cur == '?' || cur == '+')) {
			if (i == 0) {
				HLT_WRNLC(regex_line_no, regex_colu_no, HLT_MJRWRN, "Cannot qualify empty/0-length string.");
				return false;
			}

			if (prv == '*' || prv == '?' || prv == '+') {
				HLT_WRNLC(regex_line_no, regex_colu_no, HLT_MJRWRN, "This lexical analyzer does not support qualified qualifiers.");
				return false;
			}
			
			if (prv == '|') {
				HLT_WRNLC(regex_line_no, regex_colu_no, HLT_MJRWRN, "Cannot qualify empty/0-length string (on or).");
				return false;
			}
		}

		if (bracket_count > 0 && cur == '-') {
			if ((int)prv > (int)nxt) HLT_WRNLC(regex_line_no, regex_colu_no, HLT_VERBSE, "Note that this sequence within brackets will exclude range specified.")
			else if (prv == nxt) HLT_WRNLC(regex_line_no, regex_colu_no, HLT_STDWRN, "Sequence within brackets has no range.");
		}
		regex_colu_no++;
	}

	if (paren_count != 0) {
		HLT_WRNLC(regex_line_no, regex_colu_no, HLT_MJRWRN, "Regex did not properly end a group structure. Missing Right Parenthesis ')'!");
		return false;
	}
	if (bracket_count > 0) {
		HLT_WRNLC(regex_line_no, regex_colu_no, HLT_MJRWRN, "Regex did not properly end a brackets expression. Missing Right Bracket ']'!");
		return false;
	}

	return true;
}
