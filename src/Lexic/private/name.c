#include <stdlib.h>
#include <string.h>

#include "lexic_internal.h"

char ** LexicTokenNamesFromFile(char *file_name, LexicVocabulary *vocab) {
	if (file_name == NULL || file_name[0] == '\0') {
		HLT_WRN(HLT_MJRWRN, "Given empty file name?");
		return NULL;
	}
	
	if (vocab == NULL) {
		HLT_WRN(HLT_MJRWRN, "Given NULL vocabulary?");
		return NULL;
	}

	char *strm = ftostr(file_name);

	char **namestrm = LexicTokenNamesFromString(strm, vocab);

	free(strm);
	return namestrm;
}

char ** LexicTokenNamesFromString(char *stream, LexicVocabulary *vocab) {
	if (stream == NULL || stream[0] == '\0') {
		HLT_WRN(HLT_MJRWRN, "Given empty stream?");
		return NULL;
	}
	if (vocab == NULL) {
		HLT_WRN(HLT_MJRWRN, "Given NULL vocabulary?");
		return NULL;
	}

	struct lxc_token *tstrm = LexicTokensFromString(stream, vocab);
	if (tstrm == NULL || tstrm[0].definition_name == NULL || tstrm[0].definition_name[0] == '\0') {
		HLT_WRN(HLT_VERBSE, "Token Stream from character stream is empty");
		return NULL;
	}

	char **namestrm = NULL;
	int name_strm_cnt = 0;

	for (size_t t_ind = 0; tstrm[t_ind].definition_name != NULL; t_ind++) {
		char *name = tstrm[t_ind].definition_name;
		
		name_strm_cnt += 1;
		namestrm = realloc(namestrm, name_strm_cnt * sizeof(char*));
		namestrm[name_strm_cnt-1] = (char*)calloc(strlen(name)+1, sizeof(char));
		strcpy(namestrm[name_strm_cnt-1], name);
	}

	namestrm = realloc(namestrm, (name_strm_cnt+1) * sizeof(char*));
	namestrm[name_strm_cnt] = NULL; //Properly delineate the array
	
	LexicTokensFree(tstrm);
	return namestrm;
}

void LexicTokenNamesFree(char **nstrm) {
	if (nstrm == NULL) return;

	for (char **tptr = nstrm; *tptr != NULL; tptr++) {
		free(*tptr);
	}

	free(nstrm);
}
