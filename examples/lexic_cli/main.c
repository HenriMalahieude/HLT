#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lexic.h"

void flushstdin() {
	int c;
	while((c = getchar() != '\n' && c != EOF)) {}
}

LexicVocabulary * vocabularyCreationLoop(char *argvFileName) {
	if (argvFileName != NULL) return LexicVocabularyFromFile(argvFileName);

	char line[1024]; //shameless rip from stack overflow

	int option = 0;
	while (true) {
		printf("No Vocabulary File Given. Please select a creation method from the following options:\n");
		printf("\t1 -> Input a file name to retrieve an already made vocabulary\n");
		printf("\t2 -> Input each token name and definition manually\n");
		printf("\n\tOption: ");

		scanf("%d", &option);
		printf("\n");
		if (option == 1) { //File Name
			printf("Input a file name: ");
			flushstdin();
			char *str = fgets(line, 1024, stdin); //will this prevent system() to use this cli?

			if (str == NULL) {
				printf("Could not read line?\n");
				continue;
			}

			line[1023] = '\0';
			line[strcspn(line, "\n")] = '\0';

			//printf("Here is the line we have:\n%s\n", line);
			return LexicVocabularyFromFile(line);
		} else if (option == 2) { //Manual formatting
			printf("Manually creating vocab:\n\n");
			char smaller_line[512]; line[0] = '\0';

			int count = 0;
			while (true) {
				if (count >= 1023) break;

				printf("Token Name: ");
				flushstdin();
				if (!fgets(smaller_line, 512, stdin)) {
					printf("Could not read line for token name?\n");
					exit(1);
				}

				smaller_line[511] = '\0';
				int lnloc = strcspn(smaller_line, "\n");
				
				if (lnloc >= 511) {
					printf("Line provided was too long!\n");
					exit(1);
				} else {
					smaller_line[lnloc] = ':'; //necessary for the lexic function
					smaller_line[lnloc+1] = '\0'; //because we want to include the new line for the token regex to be appended
				}
				
				count += strlen(smaller_line);
				if (count >= 1023) break;
				if (count >= 512) printf("Warning: Really large string! This CLI only supports 1024 length strings for manual formatting!\n\tCLI will auto exit on reaching end!\n");
				strcat(line, smaller_line);
				
				printf("Token Regex: ");
				if (!fgets(smaller_line, 512, stdin)) {
					printf("Could not read line for token regex?\n");
					exit(1);
				}

				smaller_line[511] = '\0';
				lnloc = strcspn(smaller_line, "\n");
				
				if (lnloc >= 511) {
					printf("Line provided was too long!\n");
					exit(1);
				} else {
					smaller_line[lnloc+1] = '\0'; //because we want to include the new line for the token regex to be appended
				}
				
				count += strlen(smaller_line);
				if (count >= 1023) break;
				if (count >= 512) printf("Warning: Really large string! This CLI only supports 1024 length strings for manual formatting!\n\tCLI will auto exit on reaching end!\n");
				strcat(line, smaller_line);

				printf("Would you like to stop adding (y/n)? ");
				char r = '\0';
				scanf("%c", &r);
				printf("\n");
				if (tolower(r) == (int)'y') break;
			}

			//printf("Here is the line we have:\n%s\n", line);
			return LexicVocabularyFromString(line);
		} else {
			printf("Invalid option provided.\n");
		}
	}
}

int main(int argc, char *argv[]) {
	char *vocabFileName = NULL;
	if (argc == 2) { //we've been given a file that's a vocabulary
		vocabFileName = argv[1];
	} else if (argc > 2) {
		printf("Usage: `lexic_cli` or `lexic_cli [file_name]`\n");
		exit(1);
	}
	
	LexicVocabulary *vocab = vocabularyCreationLoop(vocabFileName);
	//printf("Vocab provided? %p\n", vocab);
	//printf("How many definitions are there? %d\n", vocab->def_count);
	
	
	char line[1024];
	printf("Vocabulary generated. Input characters to be tokenized once you press [Enter].\n\t Note that since [Enter] (or \\n) is a control character vocabs that use it won't tokenize it!\n");
	while (true) {
		printf("\nLine to be tokenized:\n");
		if (!fgets(line, 2014, stdin)) {
			printf("Could not read line!\n");
			break;
		}
		printf("\n");

		line[1023] = '\0';
		line[strcspn(line, "\n")] = '\0'; //would this work on systems with \n\r returns?
		LexicToken *tstrm = LexicTokensFromString(line, vocab);
		if (tstrm != NULL && tstrm->definition_name != NULL) {
			for (int i = 0; tstrm[i].definition_name != NULL; i++) {
				printf("%s: '%s' @ %d\n", tstrm[i].definition_name, tstrm[i].matching_input, tstrm[i].col);
			}
		} else if (tstrm == NULL) {
			printf("Error during Token Stream creation, tstrm was null!\n");
			break;
		}

		LexicTokensFree(tstrm); tstrm = NULL;
	} 

	//Not Reachable without Ctrl + C or Error

	return 0;
}
