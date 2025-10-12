#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sets.h"

char ** SetCreate(int count, ...) {
	if (count <= 0) return NULL;

	va_list elms;
	va_start(elms, count);

	char ** set = NULL;

	for (int i = 0; i < count; i++) {
		SetAdd(&set, va_arg(elms, char *));
	}

	va_end(elms);
	return set;
}

bool SetContains(char **set, char *item) {
	if (set == NULL || item == NULL) return false;

	int count = SetCount(set);
	for (int i = 0; i < count; i++) {
		char *elm = set[i];
		if (strcmp(item, elm) == 0) return true;
	}

	return false;
}

bool SetEquality(char **set1, char **set2) {
	if (set1 == set2) return true;
	if (set1 == NULL || set2 == NULL) return false;

	int cnt1 = SetCount(set1);
	if (cnt1 <= 0) return false;

	if (cnt1 != SetCount(set2)) return false;

	for (int i = 0; i < cnt1; i++) {
		if (!SetContains(set2, set1[i])) return false;
	}

	return true;
}

//Opposite of Disjoint
// empty set is disjoint to everything
bool SetOverlaps(char **set1, char **set2) {
	if (set1 == NULL || set2 == NULL) return false;

	int cnt1 = SetCount(set1);
	if (cnt1 <= 0) return false;
	if (SetCount(set2) <= 0) return false;

	for (int i = 0; i < cnt1; i++) {
		if (SetContains(set2, set1[i])) return true;
	}

	return false;
}

//One by one add, may not be totally efficient
//...
//But honestly who cares, I got better things to do
//NOTE: Could track capacity with special 0x1, 0x2, 0x0 "pointer" values
//		But that's a bit too overkill for this lmao
bool SetAdd(char ***set, char *item) {
	if (item == NULL || set == NULL) return false;
	if (SetContains(*set, item)) return false;

	int count = SetCount(*set);
	char **tmp = malloc(sizeof(char *) * (count+2));
	if (tmp == NULL) return false; //malloc failed?
	
	for (int i = 0; i < count; i++) {
		tmp[i] = (*set)[i]; //copy over the pointers to the strings
	}
	
	tmp[count] = calloc(strlen(item)+1, sizeof(char));
	strcpy(tmp[count], item);
	tmp[count+1] = NULL;

	if (*set != NULL) free(*set); //only free what has been jailed
	*set = tmp;

	return true;
}

bool SetRemove(char ***set, char *item) {
	if (set == NULL || item == NULL) return false;
	if (!SetContains(*set, item)) return false;

	int count = SetCount(*set);
	char **tmp = malloc(sizeof(char *) * (count));
	if (tmp == NULL) return false; //malloc failed?

	int sub = 0;
	char *ptr = NULL;
	for (int i = 0; i < count; i++) {
		//copy over, note the left shifting
		if (strcmp((*set)[i], item) != 0) tmp[i - sub] = (*set)[i];
		else {
			sub = 1;
			ptr = (*set)[i];
		}
	}

	tmp[count-1] = NULL;
	if (ptr != NULL) free(ptr);
	*set = tmp;

	return true;
}

char ** SetUnion(char **set1, char **set2) {
	int count1 = SetCount(set1);
	int count2 = SetCount(set2);
	if (count1 <= 0 && count2 <= 0) return NULL;

	//int countN = count1 + count2;
	char ** uset = NULL; 
	// NOTE: could be made faster if we preallocated the stuff (malloc), but whatever... that's more programming
	// 	 	 if you wanted speed/efficiency, go use LLVM or GNU Compiler Stack lol

	for (int i = 0; i < count1; i++) {
		SetAdd(&uset, set1[i]);
	}

	for (int i = 0; i < count2; i++) {
		SetAdd(&uset, set2[i]);
	}
	
	return uset;
}

int SetCount(char **set) {
	if (set == NULL) return 0;

	int cnt;
	for (cnt = 0; set[cnt] != NULL; cnt++);

	return cnt;
}

void SetPrint(char **set) {
	if (set == NULL) return;

	for (int i = 0; set[i] != NULL; i++) printf("'%s'\t", set[i]);
	printf("\n");
}

void SetFree(char **set) {
	if (set == NULL) return;

	int count = SetCount(set);
	for (int i = 0; i < count; i++) {
		free(set[i]);
		set[i] = NULL;
	}

	free(set);
}
