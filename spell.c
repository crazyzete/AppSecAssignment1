
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dictionary.h"

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
	char * dumpBuffer = NULL;

	// m modifier automatically allocates sufficent memory
	while (0 < fscanf(fp, "%100ms", &readBuffer)) {
		// Need to strip ending punctuation. Middile-punctuation may be a mis-spelling or correct,
		// in teh case of '. While not in the test wordlist, a - could also be a valid char. But the key
		// is to find cases where normal word ending punct such as a period, comma, exclaimation point, semicolin, colin,
		// are read. 
		
		//printf("Read: %s\n", readBuffer);
		if (strlen(readBuffer) == 100) {

			// Not sure if last next char is a word terminator. Read it and seek back 1 position.
			char nextChar = fgetc(fp);
			fseek(fp,ftell(fp)-1,0); // set file pointer back 1 position. This is like a peek.

			if (nextChar != '\n' && nextChar != ' ') {
			
			     while (0 < fscanf(fp, "%100ms", &dumpBuffer)) {
				     if (strlen(dumpBuffer) < 100)
					     break;
				     else if (strlen(dumpBuffer) == 100) {
					// Need to ensure this last read didn't fill up the bufer and hit the end of the word.
					char next = fgetc(fp);    // peek forward
					fseek(fp, ftell(fp)-1,0); // seek back one
					if (next == '\n' || next == ' ')
						break;

				     }
//printf("Dump Buffer\n");
				     free(dumpBuffer);
			     }
			     //printf("Done dumping buffers\n");
			     free(dumpBuffer);

			}
		}
		
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

			bool truncated = false;
			if (bytesToCopy > LENGTH) {
				// Too large of a word. Dont' check spelling copy ony LENGTH
				bytesToCopy = LENGTH;
				truncated = true;

			}

			char * translateBuffer = malloc(LENGTH + 1);
			if (NULL == translateBuffer)
				return false;

			strncpy(translateBuffer, &readBuffer[firstAlphaLeft], bytesToCopy);
			// Null Terminate
			translateBuffer[bytesToCopy] = '\0';

			if (truncated || (!check_word(translateBuffer, hashtable))) {
			    
				misspelled[wrongCount++] = translateBuffer;
				
			    
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

	int hash = abs(hash_function(lowerWord));   // added abs() after fuzzer test showed negative hash possible

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

	// Have to assume caller sized hashtable appropriately. There is no way at runtime
	// in C to determine the sizeof hashtable array within the function. 

	// Init the hashtable with null pointers
	int i;
	for (i = 0; i < HASH_SIZE; i++) {
		hashtable[i] = NULL;
	}

	char readword[LENGTH + 1];
	int wordhash;
	//while (fgets(readword, sizeof(readword), wordlist)) { 
   	while (0 < fscanf(wordlist, "%45s", readword)) {	

		// Convert to lower. Do all checks against lowercase strings.
		stringToLower(readword);

		// Hash word
		wordhash = abs(hash_function(readword));	// added abs() per fuzzer crash. neg hash possible.
		
		// Allocate a new node
		hashmap_t newNode = (hashmap_t) malloc(sizeof(node));
		if (NULL == newNode)
			return false;
		newNode->next = NULL;
		strncpy(newNode->word, readword, LENGTH);
		


		//printf("Word [%s] in hash[%d]\n", readWord, wordhash);
		
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

		// Need to determine if more chars on line and discard. if read less than 45 chars, still can be spaces
		// and more chars. discard anything on a line after 45 chars.
		// Not sure if last next char is a word terminator. Read it and seek back 1 position.
		char * dumpBuffer = NULL;

		char nextChar = fgetc(wordlist); // peek
		fseek(wordlist,ftell(wordlist)-1,0); // set file pointer back 1 position. This is like a peek.

		if (nextChar != '\n') {
			while (0 < fscanf(wordlist, "%500ms", &dumpBuffer)) {
			    char next = fgetc(wordlist);    // peek forward
			    fseek(wordlist, ftell(wordlist)-1,0); // seek back one
			    free(dumpBuffer);
			    if (next == '\n') 
				break;
			     } // end while

		}  // end if

	} // end while

	fclose(wordlist);

	return true;
}
