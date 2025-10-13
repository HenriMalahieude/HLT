#include "syntac_private.h"

SyntacTreeNode * SyntacTreeFromTokens(void *stream, Syntac *book) {
	if (stream == NULL) {
		HLT_WRN(HLT_MJRWRN, "Token stream provided is empty?");
		return NULL;
	}

	if (book == NULL) {
		HLT_WRN(HLT_MJRWRN, "Rule Book null?");
		return NULL;
	}

	SyntacTreeType typ = book->type;
	switch (typ) {
		case (STC_LL1):
			return LL1ParseTokens(stream, book);
		case (STC_LR1):
			return LR1ParseTokens(stream, book);
		default:
			HLT_ERR("Unknown Parsing Type!?!? (%d)", (int)typ);
			break;
	}
	
	HLT_AERR("Unreachable!?!?");
	return NULL;
}

SyntacTreeNode * SyntacTreeFromStream(char **stream, SyntacBook *book) {
	if (stream == NULL || *stream == NULL) {
		HLT_WRN(HLT_MJRWRN, "String stream provided is empty?");
		return NULL;
	}

	if (book == NULL) {
		HLT_WRN(HLT_MJRWRN, "Rule Book null?");
		return NULL;
	}

	SyntacTreeType typ = book->type;
	switch (typ) {
		case (STC_LL1):
			return LL1ParseStream(stream, book);
		case (STC_LR1):
			return LR1ParseStream(stream, book);
		default:
			HLT_ERR("Unknown Parsing Type!?!? (%d)", (int)typ);
			break;
	}
	
	HLT_AERR("Unreachable!?!?");
	return NULL;
}

void SyntacTreeFree(SyntacTreeNode *tree) {
	if (tree == NULL) {
		HLT_WRN(HLT_MJRWRN, "Supplied tree pointer was null?");
		return;
	}

	if (tree->children[0] != NULL) {
		for (int i = 0; tree->children[i] != NULL; i++) {
			SyntacTreeFree(tree->children[i]);
		}
	}

	free(tree->rule_name); tree->rule_name = NULL;
	free(tree);
}
