#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexic_internal.h"

//returns the index of the matching token definition to input, or -1 if no match
int match_within_vocabulary(struct lxc_vocabulary *vocab, char *input) {	
	int match_index = -1;
	
	for (int defi = 0; defi < vocab->def_count; defi++) {
		struct lxc_definition tdef = vocab->definitions[defi];
		if (RegexMatch(tdef.regex, input)) {
			match_index = defi;
			break;
		}
	}

	return match_index;
}

void add_token_to_tstream(struct lxc_token **tstrm, size_t *tlen, struct lxc_definition tdef, char *match) {
	HLT_WRN(HLT_DEBUG, "Adding '%s' to tstream!\n", tdef.name);
	*tlen += 1;
	*tstrm = realloc(*tstrm, *tlen * sizeof(struct lxc_token));

	if (*tstrm == NULL) HLT_ERRLC(regex_line_no, regex_colu_no, "Reallocation failed?");


	(*tstrm)[*tlen - 1].definition_name = NULL; //delineate
	
	if (*tlen > 1) {
		struct lxc_token *tok = &(*tstrm)[*tlen - 2];
		tok->definition_name = calloc(strlen(tdef.name)+1, sizeof(char)); strcpy(tok->definition_name, tdef.name);
		tok->matching_input = calloc(strlen(match)+1, sizeof(char)); strcpy(tok->matching_input, match);

		int old = regex_colu_no; //move "column" number to front of instance
		regex_colu_no -= strlen(match)-1; if (regex_colu_no <= 0) regex_colu_no = 1;
		//printf("column at %d, w/ strlen %d (- 1) and max so now at %d\n", old, strlen(match), regex_colu_no);

		tok->line = regex_line_no;
		tok->col = regex_colu_no;

		regex_colu_no = old; //restore for future tokens
	}
}

LexicToken * LexicTokensFromFile(char *file_name, LexicVocabulary *vocab) {
	if (file_name == NULL || file_name[0] == '\0') {
		HLT_WRN(HLT_MJRWRN, "Given empty file name?");
		return NULL;
	}

	if (vocab == NULL) {
		HLT_WRN(HLT_MJRWRN, "Given NULL Vocab?");
		return NULL;
	}

	char *strm = ftostr(file_name);

	struct lxc_token *tstrm = LexicTokensFromString(strm, vocab);

	free(strm);
	return tstrm;
}

//Random TODO Idea: What if we made a regex tree, would it reduce this O(m*n) procedure down? Would it improve time complexity?
LexicToken * LexicTokensFromString(char *stream, LexicVocabulary *vocab) {
	if (stream == NULL) {
		HLT_WRN(HLT_MJRWRN, "Given NULL stream?");
		return NULL;
	}

	if (vocab == NULL) {
		HLT_WRN(HLT_MJRWRN, "Given NULL Vocab?");
		return NULL;
	}

	regex_line_no = 1;
	regex_colu_no = 1;

	struct lxc_token *tstrm = malloc(sizeof(struct lxc_token)); tstrm->definition_name = NULL;
	size_t tlen = 1; //includes "null" termination

	size_t stlen = strlen(stream);
	char substring[100];
	size_t nconsumed_ind = 0; //inclusive not consumed idx

	int lst_match = -1; //negative signifies no match
	int lst_start = -1; //used to warn of dropped characters

	for (size_t i = 0; i < stlen; i++) {
		int sublen = i - nconsumed_ind + 1; //this means i is inclusive
		if (sublen <= 0) continue; //don't support 0-string matches
		if (sublen > 99) HLT_ERRLC(regex_line_no, regex_colu_no, "Regex doesn't support tokens longer than 99");

		strncpy(substring, stream+nconsumed_ind, sublen);
		substring[sublen] = '\0';

		int cur_match = match_within_vocabulary(vocab, substring);
		int cur_start = nconsumed_ind;

		if (cur_match > 0) HLT_WRNLC(regex_line_no, regex_colu_no, HLT_VERBSE, "We found %d match(es) in vocab", cur_match);
		if (cur_match < 0 && sublen > 1) { //need to drop characters at front?
			for (int j = 1; j < sublen; j++) { //just don't drop last character
				char *subsub = substring + j;
				cur_match = match_within_vocabulary(vocab, subsub);
				if (cur_match > -1) {
					cur_start = nconsumed_ind+j;
					break;
				}
			}
			HLT_WRN(HLT_VERBSE, "Dropped characters at front: Found %d match(es) in vocab\n", cur_match);
		}

		if (cur_match >= 0) { //did we match at 'this' character?
			if (lst_match > -1 && lst_match != cur_match && lst_start < cur_start) {		
				regex_colu_no -= 1; //since we don't use this "new" char
				substring[sublen-1] = '\0'; //don't "match" the new character
				add_token_to_tstream(&tstrm, &tlen, vocab->definitions[lst_match], substring+(lst_start-nconsumed_ind));
				nconsumed_ind = i;
				regex_colu_no += 1;

				strncpy(substring, stream+nconsumed_ind, 1); //incase of edge case like last is dif or somthn
				substring[1] = '\0';
			}

			if (i < stlen-1) { //reset to new stuff
				lst_match = cur_match;
				lst_start = cur_start;
			} else { //eof, consume regardless
				if (cur_start != (int)nconsumed_ind) HLT_WRNLC(regex_line_no, regex_colu_no, HLT_STDWRN, "Dropping unmatched characters at front of unconsumed character sequence! (EOF)");
				
				add_token_to_tstream(&tstrm, &tlen, vocab->definitions[cur_match], substring+(cur_start-nconsumed_ind));
				nconsumed_ind = i+1;
			}
		} else if (lst_match >= 0) { //did we match on the last character?
			if (lst_start != (int)nconsumed_ind) HLT_WRNLC(regex_line_no, regex_colu_no, HLT_STDWRN, "Dropping unmatched characters at front of unconsumed character sequence!");
			if (sublen <= 1) HLT_ERRLC(regex_line_no, regex_colu_no, "How did we match a 0-length character? Fatal Error!");

			regex_colu_no -= 1; //since we don't use this "new" char
			substring[sublen-1] = '\0'; //don't "match" the new character
			add_token_to_tstream(&tstrm, &tlen, vocab->definitions[lst_match], substring+(lst_start-nconsumed_ind));
			nconsumed_ind = i;
			regex_colu_no += 1;
			
			//reset
			lst_match = -1;
			lst_start = nconsumed_ind;
		} //otherwise keep going

		if (stream[i] == '\n') {
			regex_line_no++;
			regex_colu_no = 1;
		} else {
			regex_colu_no++;
		}
	}

	return tstrm;
}

//assuming that definition_name and matching_input fields are malloc copies
void LexicTokensFree(LexicToken *tstrm) {
	if (tstrm == NULL) return;

	struct lxc_token current = tstrm[0];
	size_t cur_ind = 0;
	while(current.definition_name != NULL) {
		free(current.definition_name);
		free(current.matching_input);
		current = tstrm[++cur_ind];
	}
	
	free(tstrm);
}
