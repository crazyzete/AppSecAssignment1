#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"

extern node* hashtable[HASH_SIZE];

int main( int argc, const char* argv[] )
{
	printf("Entered Main\n");
	
	bool loaded = load_dictionary(argv[2], hashtable);

	if (!loaded) {
	    printf("Failed to load dictionary\n");
	    return -1;
	}

	FILE *text;
	text = fopen(argv[1], "r");

 	if(text == NULL)
  	 {
		printf("Error open text.txt\n");
      		return -1;             
   	}

	char * spellErrors[MAX_MISSPELLED];
	int errors;
	errors = check_words(text, hashtable, spellErrors);

	fclose(text);

	printf("Spelling Errors: %d\n", errors);

	int i;
	for (i = 0; i < errors; i++) {
		printf("Misspelled: %s\n", spellErrors[i]);
	}

	// Cleanup (remove valgrind errors)
	for (i = 0; i < errors; i++) {
		free(spellErrors[i]);
	}
	
	for (i = 0; i < HASH_SIZE; i++) {

		if (NULL != hashtable[i]) {

			hashmap_t currentNode = hashtable[i];
			hashmap_t nextNode;
	
			do {
				nextNode = currentNode->next;
				free(currentNode);
				currentNode = nextNode;

			} while (NULL != currentNode);
		}
	}

		


	printf("Exit Main\n");
}
