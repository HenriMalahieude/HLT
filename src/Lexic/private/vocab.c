#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lexic_internal.h"

int regex_line_no = 0; //arbitrary location
int regex_colu_no = 0;

void add_def(struct lxc_vocabulary *dict, struct lxc_definition def) {
	dict->def_count++;
	dict->definitions = realloc(dict->definitions, dict->def_count * sizeof(struct lxc_definition));
	dict->definitions[dict->def_count-1] = def;
}

LexicVocabulary * LexicVocabularyAllocate() {
	struct lxc_vocabulary *allocated = malloc(sizeof(struct lxc_vocabulary));

	allocated->definitions = NULL;
	allocated->def_count = 0;

	return allocated;
}

bool LexicVocabularyDefinitionAdd(LexicVocabulary *vocab, char *nme, char *rgx) {
	if (vocab == NULL) {
		HLT_WRNLC(regex_line_no, regex_colu_no, HLT_MJRWRN, "Given NULL Vocab?");
		return false;
	}
	if (nme == NULL || nme[0] == '\0') {
		HLT_WRNLC(regex_line_no, regex_colu_no, HLT_MJRWRN, "Given empty name?");
		return false;
	}
	if (rgx == NULL || rgx[0] == '\0') {
		HLT_WRNLC(regex_line_no, regex_colu_no, HLT_MJRWRN, "Given empty regex?");
		return false;
	}

	if (!RegexValidate(rgx)) return false;

	char *name_copy = calloc(strlen(nme)+1, sizeof(char)); strcpy(name_copy, nme);
	char *regex_copy = calloc(strlen(rgx)+1, sizeof(char)); strcpy(regex_copy, rgx);

	struct lxc_definition temp = {.name = name_copy, .regex = regex_copy};
	add_def(vocab, temp);
	return true;
}

void LexicVocabularyFree(LexicVocabulary *vocab) {
	if (vocab == NULL) return;

	for (int i = 0; i < vocab->def_count; i++) {
		struct lxc_definition thing = vocab->definitions[i];
		free(thing.name); thing.name = NULL;
		free(thing.regex); thing.regex = NULL;
	}
	
	free(vocab->definitions); vocab->definitions = NULL;
	vocab->def_count = 0;
	free(vocab);
}

LexicVocabulary * LexicVocabularyFromFile(char *file_name) {
	if (file_name == NULL || file_name[0] == '\0') {
		HLT_WRN(HLT_MJRWRN, "Given empty file name?");
		return NULL;
	}

	if (strlen(file_name) <= 0) return NULL;

	char *strm = ftostr(file_name);
	
	struct lxc_vocabulary *dictionary = LexicVocabularyFromString(strm);

	free(strm);
	return dictionary;
}

//Expecting an input of "name: regex\n"+
LexicVocabulary * LexicVocabularyFromString(char *stream) {
	if (stream == NULL || stream[0] == '\0') {
		HLT_WRN(HLT_MJRWRN, "Stream is null?");
		return NULL;
	}

	regex_line_no = 1;
	regex_colu_no = 1;
	
	struct lxc_vocabulary dictionary = {.definitions = NULL, .def_count = 0};

	char substr[100]; size_t nconsumed_ind = 0;
	char *nme = NULL;
	int slen = strlen(stream);
	for (int i = 0; i < slen; i++) {
		if (stream[i] == ':') { //acquire the name
			HLT_WRNLC(regex_line_no, regex_colu_no, HLT_DEBUG, "Found Name.");

			int sublen = (i-1) - nconsumed_ind + 1;
			if (sublen <= 0) HLT_ERRLC(regex_line_no, regex_colu_no, "Empty Token Name.");
			if (sublen >= 100) HLT_ERRLC(regex_line_no, regex_colu_no, "Lexic does not support names longer than 99 characters!");

			strncpy(substr, stream+nconsumed_ind, sublen);
			substr[sublen] = '\0';
			if (!trim(substr, &nme)) HLT_ERRLC(regex_line_no, regex_colu_no, "Empty Token Name.");
			nconsumed_ind = i+1;

			HLT_WRNLC(regex_line_no, regex_colu_no, HLT_VERBSE, "Token Name found: %s", nme);	
		} else if (stream[i] == '\n' || i >= slen-1) { //new line or eof
			HLT_WRNLC(regex_line_no, regex_colu_no, HLT_DEBUG, "Found Definition.");

			if (nme == NULL) continue; //means it was an empty line

			int sublen = (i-1) - nconsumed_ind + 1;	
			if (i >= slen-1) sublen++;
			if (sublen <= 0) HLT_ERRLC(regex_line_no, regex_colu_no, "Empty Token Definition.");
			if (sublen >= 100) HLT_ERRLC(regex_line_no, regex_colu_no, "Lexic does not support definitions longer than 99 characters!");
			strncpy(substr, stream+nconsumed_ind, sublen);
			substr[sublen] = '\0';

			struct lxc_definition dd = {.name = nme, .regex = NULL};
			if (!trim(substr, &dd.regex)) HLT_ERRLC(regex_line_no, regex_colu_no,"Empty Token Definition.");
			if (!RegexValidate(dd.regex)) HLT_ERRLC(regex_line_no, regex_colu_no, "Could not create Token Dictionary.");
			add_def(&dictionary, dd);

			HLT_WRNLC(regex_line_no, regex_colu_no, HLT_VERBSE, "Token '%s' assigned definition '%s'", nme, dd.regex);
			
			nme = NULL;
			nconsumed_ind = i+1;
			regex_line_no++; regex_colu_no = 0;
		}
		regex_colu_no++;
	}

	struct lxc_vocabulary *out = malloc(sizeof(struct lxc_vocabulary)); *out = dictionary;
	return out;
}
