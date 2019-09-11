#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"

extern node* hashtable[HASH_SIZE];

int main( int argc, const char* argv[] )
{
	printf("Main\n");
	
	/*FILE *wordlist;
	wordlist = fopen("wordlist.txt", "r");

 	if(wordlist == NULL)
  	 {
      		printf("Error!");   
      		exit(1);             
   	}
	*/


	//hashmap_t hasht[HASH_SIZE];
	//char * misspelled[5000];
	
	bool loaded;
	loaded = load_dictionary(argv[2], hashtable);

	FILE *text;
	text = fopen(argv[1], "r");

 	if(text == NULL)
  	 {
		printf("Error open text.txt\n");
      		return -1;             
   	}

	char * spellErrors[5000];
	int errors;
	errors = check_words(text, hashtable, spellErrors);

	fclose(text);

	printf("Spelling Errors: %d\n", errors);

	int i;
	//for (i = 0; i < errors; i++) {
	//	printf("Misspelled: %s\n", spellErrors[i]);
	//}

	/*
	if (loaded) {
		printf("load_dictionary success!\n");

		// iterate check hashmap
		int hashiter;
		int totalWC = 0;
	
		for (hashiter = 0; hashiter < HASH_SIZE; hashiter++) {
			if (hashtable[hashiter] == NULL) {
				printf("HashMap %d = %d\n", hashiter, 0);
				continue;			
			} 
			else {
			   hashmap_t cur = hashtable[hashiter];
			   int hashWordCount = 1;
			   while (cur->next != NULL) {
				//printf("hash %d, word: %s", hashiter, cur->word);
				cur = cur->next;
				hashWordCount++;
			   }
			  printf("hash %d, word: %s", hashiter, cur->word);
			  //printf("HashMap %d = %d\n", hashiter, hashWordCount);
			  totalWC = totalWC + hashWordCount;
			}
		}

		printf("Total word count in hashmap is %d\n", totalWC);
	}
	else {
		printf("load_dictionary fail!\n");
	}	

	bool correct = check_word("Justice", hashtable);
	
	printf("Word Check: %d\n", correct);
	*/

	/*int correct;
	correct = check_words(wordlist, hasht, misspelled);

	if (correct) {
		printf("All words spelled correctly\n");
	}
	else {
		printf("There were misspellings\n");
	}
	*/

	//fclose(wordlist);


	printf("End Main\n");
}
