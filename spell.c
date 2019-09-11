
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dictionary.h"

extern int hash_function(const char* word);

// Convert the string, char by char, to lowercase
void stringToLower(char * str) {

	if (NULL != str) {

		int len = strlen(str);

		int pos;
		for (pos = 0; pos < len; pos++) {
			str[pos] = tolower(str[pos]);
		}

	}
	
}

/**
 * Returns true if all words are spelled correctly, false otherwise. Array misspelled is populated with words that are misspelled.
 */
int check_words(FILE* fp, hashmap_t hashtable[], char * misspelled[]) {

	int wrongCount = 0;
		
	char *readBuffer = NULL;

	// m modifier automatically allocates sufficent memory
	while (0 < fscanf(fp, "%ms", &readBuffer)) {
		// Need to strip ending punctuation. Middile-punctuation may be a mis-spelling or correct,
		// in teh case of '. While not in the test wordlist, a - could also be a valid char. But the key
		// is to find cases where normal word ending punct such as a period, comma, exclaimation point, semicolin, colin,
		// are read. 
		
		//printf("READ BUFFFER: %s\n", readBuffer);
		int charpos;
		int firstAlphaLeft = -1;
		int firstAlphaRight = -1;

		// Strip left non-alpha and right non-alpha by finding the first alphabetic
		// position from the left and the first alphabetic position from the right and
		// writing that to a new string
		for (charpos = 0; charpos <  strlen(readBuffer); charpos++) {
		    if (isalpha(readBuffer[charpos])) {
			firstAlphaLeft = charpos;
			break;
		    } // endif
		} //end for

		for (charpos = strlen(readBuffer) - 1; charpos >= 0; charpos--) {
		    if (isalpha(readBuffer[charpos])) {
			firstAlphaRight = charpos;
			break;
		    }
		}

		//printf("First Alpha Left %d; First Alpha Right %d\n", firstAlphaLeft, firstAlphaRight);
		// If there were no alphabetic characters, assume this is all numbers,
		// all punctuation, or some combo of punct and numbers, and consider it
		// spelled correctly by not searching the dictionary.
		if (firstAlphaLeft >= 0) {
			int bytesToCopy = (firstAlphaRight - firstAlphaLeft) + 1;
			char * translateBuffer = malloc(strlen(readBuffer) + 1);
			strncpy(translateBuffer, &readBuffer[firstAlphaLeft], bytesToCopy);
			// Null Terminate
			translateBuffer[bytesToCopy] = '\0';

			if ((strlen(translateBuffer) > LENGTH) || (!check_word(translateBuffer, hashtable))) {
			    if (wrongCount < 20) {
				misspelled[wrongCount++] = translateBuffer;
			    }	
			} 
			else {
		   	     // Free translate buffer space if not adding it to the the mis-spelling
		   	     // return list.
		   	     free(translateBuffer);
			}

		}

		free(readBuffer);
	}
	
	return wrongCount;
}

/**
 * Returns true if word is in dictionary else false.
 */
bool check_word(const char* word, hashmap_t hashtable[]) {

	if ((NULL == word) || (NULL == hashtable)) {
		//printf("Hashmap or word is null\n");
		return false; // don't accept null words/hash tables
	}

	// Convert to lower. Do all checks against lowercase strings.
	char lowerWord[strlen(word)]; 
	sprintf(lowerWord, "%s", word); // copying string to not impact original stringg
	stringToLower(lowerWord);

	int hash = hash_function(lowerWord);

	hashmap_t hashes = hashtable[hash];

	if (NULL == hashes)  {
		//printf("No hashes for input word: %s, %d\n", lowerWord, hash);
		return false; // No hash entry at this word's hash, so no match.
	}

	bool matchFound = false;
	// Iterate through hashes looking for a match.
	hashmap_t currentNode = hashes;
	int equal;
	while (NULL != currentNode->next) {
		equal = strncmp(lowerWord, currentNode->word, strlen(currentNode->word));
		if (0 == equal) {
		    // word found
		    matchFound = true;
		    //printf("MATCH for hash[%d] word[%s] dictionary[%s]\n", hash, lowerWord, currentNode->word);
	            break;
		} 
		else {
		   //printf("No match for hash[%d] word[%s] dictionary[%s]\n", hash, lowerWord, currentNode->word);
		}
	
		currentNode = currentNode->next;
	}

	// Check the last node if a match hasn't been found.
	if (!matchFound) {
		equal = strncmp(lowerWord, currentNode->word, strlen(currentNode->word));
		if (0 == equal) {
	 	   // word found
	 	   matchFound = true;
		}
		else {
			 //printf("No match for hash[%d] word[%s] dictionary[%s]\n", hash, lowerWord, currentNode->word);
		}
	}

	return matchFound;
}

/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load_dictionary(const char* dictionary_file, hashmap_t hashtable[]) {

	// Unable to check hashtable size for smaller table on an array
	// passed in. Assuming HASH_SIZE, may need to revisit.	

	FILE *wordlist;
	wordlist = fopen(dictionary_file, "r");

 	if(wordlist == NULL)
  	 {
      		return false;             
   	}

	// Init the hashtable with null pointers
	int i;
	for (i = 0; i < HASH_SIZE; i++) {
		hashtable[i] = NULL;
	}

	char *rawReadWord = NULL;
	int wordhash;
	
	// m modifier automatically allocates sufficent memory
   	while (0 < fscanf(wordlist, "%ms", &rawReadWord)) {	


		char readword[LENGTH + 1];

		// Copy up to the max length. Will truncate any word longer than the
		// max word length.
		strncpy(readword, rawReadWord, LENGTH);
		
		// Convert to lower. Do all checks against lowercase strings.
		stringToLower(readword);

		free(rawReadWord); // Free memory allocated by fscanf. Next loop will allocate more.

		// Hash word
		wordhash = hash_function(readword);
		
		// Allocate a new node
		hashmap_t newNode = (hashmap_t) malloc(sizeof(node));
		newNode->next = NULL;
		strncpy(newNode->word, readword, LENGTH);
		


		//printf("Word [%s] in hash[%d]\n", readword, wordhash);
		
		if (NULL == hashtable[wordhash]) {
		    // Beginning of linked list
		    hashtable[wordhash] = newNode;
		}	
		else {
		    // Linked list for hash exits, insert at end.
		    hashmap_t curPtr = hashtable[wordhash];
		    hashmap_t nextPtr = curPtr->next;
		    while (NULL != nextPtr) {
			curPtr = nextPtr;
			nextPtr = curPtr->next;
		    } // end wile

		    // At the end. Add it
		    curPtr->next = newNode;

		} // end else
		
	} // end while

	fclose(wordlist);

	return true;
}

