#include <string.h>
#include <stdio.h> //while printf is being used
#include <ctype.h>

#include "lexic_internal.h"

enum regex_type {
	RT_UNDEFINED,
	RT_DIRECT, 
	RT_ESCAPED,
	RT_OR,
	RT_BRACKETS,
	RT_QUALIFIER,
	RT_CGROUP,
};


#define BTSTCKMAX 10
size_t btstck[BTSTCKMAX];
int sp = -1; 

void psh_bt(size_t st) {
	if (sp >= BTSTCKMAX-1) HLT_AERR("Stack Overflow attempted, maximum group nesting is %d right now!", BTSTCKMAX);

	btstck[++sp] = st;
}

size_t pop_bt() {
	if (sp < 0) HLT_AERR("Stack Underflow attempted, nothing in stack?");

	return btstck[sp--];
}

bool escaped_match(char esc, char in) {
	if (esc == 's') return isspace(in);
	if (esc == 'S') return !isspace(in);
	if (esc == 'd') return in >= (int)'0' && in <= (int)'9';
	if (esc == 'D') return in < (int)'0' || in > (int)'9';
	if (esc == '0') return in == '\0'; //dunno if this is really useful
	if (esc == 'n') return in == '\n';
	if (esc == 't') return in == '\t';
	if (esc == 'r') return in == '\r';

	return esc == in;
}

bool is_qualifier(char q) {
	return q == '*' || q == '?' || q == '+';
}

//places regex index on matching right parenthesis of this cgroup
void forward_cgroup(char *reg, size_t *ri) {
	int paren_lvl = 0;
	while (*ri < strlen(reg)) {
		char prv = (*ri > 0) ? reg[*ri-1] : '\0';
		char cur = reg[*ri];
		if (prv != '\\') {
			if (cur == '(') paren_lvl++;
			else if (cur == ')' && paren_lvl == 0) return;
			else if (cur == ')') {
				paren_lvl--;
				if (paren_lvl < 0) HLT_AERR("Missing Right Parenthesis for capture group?");
			}
		}
		(*ri)++;
	}
	
	HLT_AERR("Could not leave capture group?");
}

//expects ri to be at ) of cgroup we are attempting to restart
void restart_cgroup(char *reg, size_t *ri) {
	*ri -= 1; //move before )
	int paren_lvl = 0;
	while (*ri > 0) { //travelling backwards
		char prv = (*ri > 0) ? reg[*ri-1] : '\0';
		char cur = reg[*ri];
		//printf("at '%c'\n", cur);
		if (prv != '\\') {
			if (cur == ')') paren_lvl++;
			else if (cur == '(' && paren_lvl == 0) return;
			else if (cur == '(') {
				paren_lvl--;
				if (paren_lvl < 0) HLT_AERR("Missing Left Parenthesis for capture group?");
			}
		}

		(*ri)--;
	}
	
	if (reg[*ri] != '(' || paren_lvl != 0) HLT_AERR("Could not restart capture group?");
}

//If failed escaped, DO NOT move past \ pls, let this function handle it
//returns whether or not it recovered properly
bool recover_failed_qualifier(char *reg, size_t *ri, bool *plus_match) {
	if (*ri >= strlen(reg)) HLT_AERR("Regex Index already at end of str?");

	char cur = reg[*ri];
	if (is_qualifier(cur)) HLT_AERR("Incorrect use of this function on qualifier?");
	
	size_t q_ind = (cur == '\\') ? *ri+2 : *ri+1;
	char qualifier = reg[q_ind];
	if (qualifier == '+' && !(*plus_match)) return false;
	
	HLT_WRN(HLT_DEBUG, "Recovering from failed qualifier");

	*plus_match = false;
	*ri = q_ind+1; //skip past this modifier
	return true;
}

bool recover_fail(char *reg, size_t *ri, size_t *ii, int grp_lvl, bool *plus_quali, bool *lst_grp_fail) {
	HLT_AWRN(HLT_DEBUG, "Recovering a failure!");

	char nxt = reg[*ri+1];
	if (is_qualifier(nxt)) {
		HLT_AWRN(HLT_DEBUG, "Was a qualified failure!");
		bool rec = recover_failed_qualifier(reg, ri, plus_quali);
		if (!rec) { //check for future '|'
			(*ri)++;
			if (is_qualifier(reg[*ri+1])) HLT_AERR("Qualified qualifier?");
			return recover_fail(reg, ri, ii, grp_lvl, plus_quali, lst_grp_fail);
		}
		return rec;
	}


	char cc = '\0'; size_t ind = *ri;
	while (ind < strlen(reg)) {
		char pv = (ind > 0) ? reg[ind-1] : '\0';
		cc = reg[ind];
		if (cc == '|') break;
		if (pv != '\\' && (cc == '(' || cc == ')')) break;
		ind++;
	}

	//printf("cc:%c\n", cc);

	if (cc == '|') {
		HLT_WRN(HLT_DEBUG, "Was an or_statement failure!");
		*ri = ind + 1;
		*plus_quali = false;
		*lst_grp_fail = false;
		return true;
	}

	HLT_WRN(HLT_DEBUG, "Was a cgroup failure!");
	if (grp_lvl <= 0) return false;
	*lst_grp_fail = true;
	forward_cgroup(reg, ri);
	*ii = pop_bt();
	*ri+=1;
	
	return true;
}

bool RegexMatch(char *reg, char *input) {
	HLT_WRN(HLT_VERBSE, "Comparing '%s' with regex: '%s'", input, reg);
	size_t rlen = strlen(reg);
	size_t ilen = strlen(input);

	bool plus_quali = false;
	bool lst_grp_fail = false;
	int grp_lvl = 0; //capture group recursion lvl
	size_t ii = 0; //Input Index
	size_t ri = 0; //Regex Index

	enum regex_type mode = RT_UNDEFINED;
	while (ii <= ilen && ri < rlen) { //NOTE: <= for if we need to catch ?'s at the end
		char prv = (ri > 0) ? reg[ri-1] : ' ';
		char cur = reg[ri];
		char nxt = reg[ri+1]; if (nxt == '\0') nxt = ' ';

		if (mode == RT_UNDEFINED) {
			if (cur == '\\') {mode = RT_ESCAPED;}
			else if (cur == '|') {mode = RT_OR;}
			else if (cur == '[') {mode = RT_BRACKETS;}
			else if (is_qualifier(cur)) {mode = RT_QUALIFIER;}
			else if (cur == '(' || cur == ')') {mode = RT_CGROUP;}
			else {mode = RT_DIRECT;} //the default mode
		} 

		if (mode == RT_DIRECT) {
			HLT_WRN(HLT_DEBUG, "In direct mode!");
			if (cur == input[ii] || (cur == '.' && input[ii] != '\n')) {
				ii++; //consume character
				ri++; //onto nxt regex
			} else {
				if (!recover_fail(reg, &ri, &ii, grp_lvl, &plus_quali, &lst_grp_fail)) break;
			}
		} else if (mode == RT_ESCAPED) {
			HLT_WRN(HLT_DEBUG, "In escaped mode!");
			if (escaped_match(nxt, input[ii])) {
				ii++;
				ri+=2; //skip \\ and escaped char
			} else {
				ri++; //Is expecting the failure on the LAST section of individual regex
				if (!recover_fail(reg, &ri, &ii, grp_lvl, &plus_quali, &lst_grp_fail)) break;
			}
		} else if (mode == RT_OR) { 
			HLT_WRN(HLT_DEBUG, "In or mode!");

			if (lst_grp_fail && prv == ')') { //failed the group before
				ri++;	
			} else { //otherwise it was a success
				if (grp_lvl <= 0) return ii >= ilen;
				forward_cgroup(reg, &ri);
			}
			lst_grp_fail = false;
		} else if (mode == RT_BRACKETS) {
			HLT_WRN(HLT_DEBUG, "In brackets mode!");
			char matching = input[ii];
			bool match = false;
			ri++; //Assuming that we started at '[' of the brackets
			while (ri < rlen) {
				char bcur = reg[ri];

				if (bcur == ']') {ri++; break;}
				if (bcur == '\\') {
					ri++;
					match = escaped_match(reg[ri], matching);
				} else if (bcur == '-' && reg[ri-1] != '\\') {
					if (reg[ri-2] == '\\' || reg[ri+1] == '\\') HLT_ERR("This lexical analyzer does not support escaped characters in sequences.");
					match = (int)matching >= (int)reg[ri-1] && (int)matching <= (int)reg[ri+1];
					ri++; //skip the sequence we've already evaluated
				} else {
					match = (matching == bcur);
				}

				if (match) {
					HLT_WRN(HLT_DEBUG, "Found a match leaving brackets!");
					break;
				}
				
				ri++;
			}

			if (match) {
				ii++;
				while (ri < rlen) {
					if (reg[ri] == ']') {ri++; break;}
					ri++;
				}
			} else {
				ri--; //expecting final input that is erroring
				if (!recover_fail(reg, &ri, &ii, grp_lvl, &plus_quali, &lst_grp_fail)) break;
			}

		} else if (mode == RT_QUALIFIER) {
			HLT_WRN(HLT_DEBUG, "In qualifier mode!");
			if (prv == '\\') HLT_ERR("Cannot enter qualifier mode if previous is escaped, what?");
			if (cur == '?') {
				ri++;
				mode = RT_UNDEFINED;
				continue;
			} else {
				//printf("1\n");
				plus_quali = true;
				if (prv == ')' && reg[ri-2] != '\\') {
					//printf("2\n");
					ri--; //restart group and recover_fail expecting to be on ')'
					if (lst_grp_fail) {
						if (!recover_fail(reg, &ri, &ii, grp_lvl, &plus_quali, &lst_grp_fail)) break;
					} else {
						restart_cgroup(reg, &ri);
					}
				} else if (prv == ']' && reg[ri-2] != '\\') {
					//printf("3\n");
					while (ri > 0) {
						if (reg[ri] == '[' && reg[ri-1] != '\\') break;
						ri--;
					}
				} else {
					//printf("4\n");
					if (reg[ri-2] == '\\') ri--;
					ri--;
				}
			}
		} else if (mode == RT_CGROUP) {
			HLT_WRN(HLT_DEBUG, "In cgroup mode!");
			if (cur == '(') {
				grp_lvl++;
				psh_bt(ii);
			} else if (cur == ')') {
				if (lst_grp_fail) {
					if (!recover_fail(reg, &ri, &ii, grp_lvl, &plus_quali, &lst_grp_fail)) break;
				}
				grp_lvl--;
				pop_bt();
			} 
			ri++;
			lst_grp_fail = false;
		}
		mode = RT_UNDEFINED;
	}

	return ii >= ilen && ri >= rlen;
}
