#include <stdbool.h>
#include <stdarg.h>

#ifndef __HEPH_SETS__
#define __HEPH_SETS__

/*
 * NOTE: 
 * Debated creating a typedef for char** as set_t, 
 *  but that obscures the underlying type, maybe if it was a void *.
 *  But void * is way too complicated for what I need
 * 
 */

// Sets of strings (unique, case-sensitive)
// Sets must be NULL-Terminated

// Creates a new set of strings, 
// where count is the total elements within set (NULL termination not include)
char ** SetCreate(int count, ...);

// Set contains item
bool SetContains(char **set, char *item);

// Equivalency check (must be exact same, though not positionally)
bool SetEquality(char **set1, char **set2);

// Overlapping (set1 == set2 or has some similar items)
// opposite of disjoint
bool SetOverlaps(char **set1, char **set2);

// Outputs succesful (and unique) additions to the set
// If it's in the set before, it returns false
// Makes a (malloc) copy of the item
bool SetAdd(char ***set, char *item);

// Outputs successful removal from set
// If it's not in the set, it returns false
// Frees the item that matches in the set, (assumes it was malloced)
// NOTE: mallocs a new smaller set underneath, which replaces set given
bool SetRemove(char ***set, char *item);

// Returns a dynamically allocated set union between set1 & set2
char ** SetUnion(char **set1, char **set2);

// Amount of elements in set (null not included)
int SetCount(char **set);

// Output the elements in the set
void SetPrint(char **set);

void SetFree(char **set);

#endif
