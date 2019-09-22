#include <check.h>
#include "dictionary.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define DICTIONARY "wordlist.txt"
#define TESTDICT "test_wordlist.txt"
#define BOGUSDICT "bogus.txt"
#define TEMPDICT "temp_dict.txt"
#define TEMPINPUT "test_tmp.txt"

START_TEST(test_dictionary_load_only)
{
    hashmap_t hashtable[HASH_SIZE];
    ck_assert(load_dictionary(TESTDICT, hashtable));

}
END_TEST

START_TEST(test_dictionary_file_not_found)
{
    hashmap_t hashtable[HASH_SIZE];
    ck_assert(!load_dictionary(BOGUSDICT, hashtable));

}
END_TEST

START_TEST(test_dictionary_null_filename)
{
    hashmap_t hashtable[HASH_SIZE];
    ck_assert(!load_dictionary(NULL, hashtable));

}
END_TEST

START_TEST(test_dictionary_word_overflow)
{
    hashmap_t hashtable[HASH_SIZE];

    const char * longword1 = "helloaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    const char * longword2 = "specialbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
    const char * normalword = "world";
    const char * padding1 = "xxxxxxxxxxxxxxxxxxxxxxx";
    const char * padding2 = "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyy";
   
    // Create temp dictionary and write 3 words to it, including long words.
    FILE *fp = fopen(TEMPDICT, "w"); 
	if (NULL != fp) {
		fprintf(fp, "%s%s\n", longword1, padding1);
		fprintf(fp, "%s\n", normalword);
		fprintf(fp, "%s%s\n", longword2, padding2);

		fclose(fp);
	}

    ck_assert(load_dictionary(TEMPDICT, hashtable));

    // Remove temp dict file
    remove(TEMPDICT);

    int hash1 = hash_function(longword1);
    int hash2 = hash_function(longword2);
    int hash3 = hash_function(normalword);

    // Take the cheap way out and there should be no collisions here,
    // so each of the 3 words should be stored in the first element. The
    // test is that long words do not overflow the buffer and that they
    // are truncated to the longword without the padding

    hashmap_t word1Node = hashtable[hash1];
    hashmap_t word2Node = hashtable[hash2];
    hashmap_t word3Node = hashtable[hash3];

    // Check these 3 hash nodes aren't null
    ck_assert(NULL != word1Node);
    ck_assert(NULL != word2Node);
    ck_assert(NULL != word3Node);

    // Check that the next pointer correctly initialized to null
    ck_assert(NULL == word1Node->next);
    ck_assert(NULL == word2Node->next);
    ck_assert(NULL == word3Node->next);

    // Check that the correct word truncating the padding is stored in hash table.
    ck_assert_msg(strncmp(word1Node->word, longword1, LENGTH) == 0);	
    ck_assert_msg(strncmp(word2Node->word, longword2, LENGTH) == 0);
    ck_assert_msg(strncmp(word3Node->word, normalword, LENGTH) == 0);

    

}
END_TEST

/**
   This test verifies that a longe word is truncated and not just split and added to dictionary as multiple words.
   Assumes the code is reading blocks of 45 char.
*/
START_TEST(test_dictionary_long_word_truncate)
{
    hashmap_t hashtable[HASH_SIZE];

    const char * longword1 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
    const char * word1 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
   
    // Create temp dictionary and write 1 long word
    FILE *fp = fopen(TEMPDICT, "w"); 
	if (NULL != fp) {
		fprintf(fp, "%s\n", longword1);
		fclose(fp);
	}

    ck_assert(load_dictionary(TEMPDICT, hashtable));

    // Remove temp dict file
    remove(TEMPDICT);

    // Confirm only 1 word in hash table and it matches hash of word1
    int hash1 = hash_function(word1);
    hashmap_t word1Node = hashtable[hash1];
    ck_assert(NULL != word1Node);
  
    // Count all the words in dict
    int wordCount = 0;
    for (int i = 0; i < HASH_SIZE; i++)
	if (NULL != hashtable[i])
		wordCount++;

    ck_assert_int_eq(1, wordCount);

}
END_TEST

/**
   This test verifies that characters after a space on a line are truncated, not considered seperate words if there is no newline.
*/
START_TEST(test_dictionary_truncate_after_space)
{
    hashmap_t hashtable[HASH_SIZE];

    const char * line1 = "the big brown cat is here";  // 6 words, but only 'the' gets counted on the line.
    const char * line2 = "sorry i am not counted";     // only 'sorry' is counted
    const char * word1 = "the";
    const char * word2 = "sorry";
   
    // Create temp dictionary and write 2 lines
    FILE *fp = fopen(TEMPDICT, "w"); 
	if (NULL != fp) {
		fprintf(fp, "%s\n%s\n", line1, line2);
		fclose(fp);
	}

    ck_assert(load_dictionary(TEMPDICT, hashtable));

    // Remove temp dict file
    remove(TEMPDICT);

    // Confirm only 2word in hash table and it matches hash of word1 and word2
    int hash1 = hash_function(word1);
    int hash2 = hash_function(word2);
    hashmap_t word1Node = hashtable[hash1];
    hashmap_t word2Node = hashtable[hash2];
    ck_assert(NULL != word1Node);
    ck_assert(NULL != word2Node);
  
    // Count all the words in dict
    int wordCount = 0;
    for (int i = 0; i < HASH_SIZE; i++)
	if (NULL != hashtable[i])
		wordCount++;

    ck_assert_int_eq(2, wordCount);

}
END_TEST


START_TEST(test_dictionary_normal_buckets)
{
    hashmap_t hashtable[HASH_SIZE];

    const int NUM_WORDS = 4;
    char * word[] = {"see", "spot", "run", "tops"};
   
    // Create temp dictionary and write 4 words to itj.
    FILE *fp = fopen(TEMPDICT, "w"); 
	if (NULL != fp) {
		int i;
		for (i = 0; i < NUM_WORDS; i++)
		    fprintf(fp, "%s\n", word[i]);
		fclose(fp);
	}

    ck_assert(load_dictionary(TEMPDICT, hashtable));

    // Remove temp dict file
    remove(TEMPDICT);

    int hash[NUM_WORDS];
    int i;
    for (i = 0; i < NUM_WORDS; i++)
	hash[i] = hash_function(word[i]);

    hashmap_t word1Node = hashtable[hash[0]];
    hashmap_t word2Node = hashtable[hash[1]]; // Should be same as 4
    hashmap_t word3Node = hashtable[hash[2]];
    hashmap_t word4Node = hashtable[hash[3]]; // Should be same as 2

    // Check these 3 hash nodes aren't null
    ck_assert(NULL != word1Node);
    ck_assert(NULL != word2Node);
    ck_assert(NULL != word3Node);
    ck_assert(NULL != word4Node);

    // Check that the next pointer correctly initialized to null
    ck_assert(NULL == word1Node->next);
    ck_assert(NULL == word3Node->next);

    // Check that Word 2 and Word 4 hashed to the same bucket
    ck_assert(word2Node == word4Node);

    // Check that the correct words are stored in buckets
    ck_assert_msg(strncmp(word1Node->word, word[0], LENGTH) == 0);	
    ck_assert_msg(strncmp(word2Node->word, word[1], LENGTH) == 0);
    ck_assert_msg(strncmp(word3Node->word, word[2], LENGTH) == 0);
    ck_assert_msg(strncmp(word4Node->next->word, word[3], LENGTH) == 0); // Check 2nd entry list in bucket

    

}
END_TEST

START_TEST(test_dictionary_normal_mixed_case)
{
    hashmap_t hashtable[HASH_SIZE];

    char * word[] = {"ANOTHER", "One", "BiTeS", "thE", "DUst", "i'LL", "BE", "bAcK"};
    char * expected[] = {"another", "one", "bites", "the", "dust", "i'll", "be", "back"};

    const int NUM_WORDS = sizeof(word) / sizeof(word[0]);
   
    // Create temp dictionary and write words to it
    FILE *fp = fopen(TEMPDICT, "w"); 
	if (NULL != fp) {
		int i;
		for (i = 0; i < NUM_WORDS; i++)
		    fprintf(fp, "%s\n", word[i]);
		fclose(fp);
	}

    ck_assert(load_dictionary(TEMPDICT, hashtable));

    // Remove temp dict file
    remove(TEMPDICT);

    int hash[NUM_WORDS];
    hashmap_t nodes[NUM_WORDS];
    int i;
    for (i = 0; i < NUM_WORDS; i++) {
	hash[i] = hash_function(expected[i]);
        nodes[i] = hashtable[hash[i]];
        ck_assert(NULL != nodes[i]);   // Check that node i is not null
        ck_assert_msg(strncmp(nodes[i]->word, expected[i], LENGTH) == 0); // Check work is lowercase representation of expected
        ck_assert(NULL == nodes[i]->next); // Check the node list for that hash has no other nodes.
    }
    

}
END_TEST


START_TEST(test_check_word_normal)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    const char* correct_word = "Justice";
    const char* punctuation_word_2 = "pl.ace";
    ck_assert(check_word(correct_word, hashtable));
    ck_assert(!check_word(punctuation_word_2, hashtable));
    // Test here: What if a word begins and ends with "?
}
END_TEST

/**
  This tests check_word handling of leading and trailing punctuation. Leading and trailing punctuation should
  be stripped by check_words. check_word merely converts to lowercase and checks the dictionary, so leading and
  trailing punctuation should fail. Certain middle punct such as apostrophe should pass.
*/
START_TEST(test_check_word_leading_trailing_punct)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char * correctEmbeddedPunct[] = {"I'll", "we'll","we'd","you're","can't","that's", "quadrilateral's"};
    char * incorrectPunct[] = {",duck","?duck","duck,","duck!","duck.","d'uck","re-use"};
    const int CHECK_SIZE = sizeof(correctEmbeddedPunct)/sizeof(correctEmbeddedPunct[0]);
    

    int i;
    for (i = 0; i < CHECK_SIZE; i++) {
	 ck_assert(check_word(correctEmbeddedPunct[i], hashtable));
         ck_assert(!check_word(incorrectPunct[i], hashtable));
    }

}
END_TEST

/**
  This tests check_word handling of numbers. Numbers should be stripped out prior to check_word and should not be in the
  dictionary, so all leading, trailing, and embedded numbers should fail, including all numerics - should be handled
  by check_word.
*/
START_TEST(test_check_word_numerics)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char * incorrect[] = {"1duck","2duck3","4du5ck6,","11duck22","123456","789ad","-123456","bc1200","a12345b"};
    const int CHECK_SIZE = sizeof(incorrect)/sizeof(incorrect);
    

    int i;
    for (i = 0; i < CHECK_SIZE; i++) {
         ck_assert(!check_word(incorrect[i], hashtable));
    }

}
END_TEST

/**
  This tests check_word handling of numbers NULL input parameters.
*/
START_TEST(test_check_word_null_inputs)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char * word = "duck";
    
	
    ck_assert(!check_word(word, NULL));
    ck_assert(!check_word(NULL, hashtable));
    ck_assert(!check_word(NULL, NULL));
    

}
END_TEST

/**
  This tests check_word handling when not finding any words for the word's given hash. Adding this test as a result of a code coverage
  finding not showing this line hit because the standard wordlist fills most of the buckets. To test this, i will provide an initialized, but 
  empty hashtable. the word will not match any hashes in the bucket and should return false.
*/
START_TEST(test_check_word_empty_hashtable)
{
    hashmap_t hashtable[HASH_SIZE];
    for (int i = 0; i < HASH_SIZE; i++)
	hashtable[i] = NULL;

    char * word = "empty";
    	
    ck_assert(!check_word(word, hashtable));
    

}
END_TEST

/**
 This test checks a specific cases with 200 character strings. My code reads in up to 100 charactes and then continues to read blocks o
 100 and throw them away, however, it needs to inspect the next character to determine if it is at a word boundary or not. This is testing
 a specific case i knew I had and already fixed. This verifies 200 characters with a space in the middle is 1 word.
*/
START_TEST(test_check_words_hundred_char_multiple_one_word)
{
	hashmap_t hashtable[HASH_SIZE];
	load_dictionary(DICTIONARY, hashtable);
	char *misspelled[MAX_MISSPELLED];
	// Rather than a bunch of input files, lets dynamically write the file.
	// This will open file for read/write, write a test string, and the program will read that string.
	FILE *fp = fopen(TEMPINPUT, "w+"); 
	if (NULL != fp) {
		const int CHAR_COUNT = 200;
		for (int i = 0; i < CHAR_COUNT; i++) 
		     fprintf(fp, "%c", 'a');
		fprintf(fp,"%c",'\n');
		rewind(fp); // Set file pointer back to start.

		int num_misspelled = check_words(fp, hashtable, misspelled);
		ck_assert(num_misspelled == 1); // one long string of a's should be misspelled

		fclose(fp);
		remove(TEMPINPUT);
	}
	else
	   ck_assert(NULL != fp);
	
}
END_TEST

/**
 This test checks a specific cases with 200 character strings. My code reads in up to 100 charactes and then continues to read blocks o
 100 and throw them away, however, it needs to inspect the next character to determine if it is at a word boundary or not. This is testing
 a specific case i knew I had and already fixed. This verified 200 characters with a space in the middle is 2 words.
*/
START_TEST(test_check_words_hundred_char_multiple_two_word)
{
	hashmap_t hashtable[HASH_SIZE];
	load_dictionary(DICTIONARY, hashtable);
	char *misspelled[MAX_MISSPELLED];
	// Rather than a bunch of input files, lets dynamically write the file.
	// This will open file for read/write, write a test string, and the program will read that string.
	FILE *fp = fopen(TEMPINPUT, "w+"); 
	if (NULL != fp) {
		const int CHAR_COUNT = 200;
		for (int i = 0; i < CHAR_COUNT-1; i++) {
		     if ((CHAR_COUNT/2 == i)) 
			fprintf(fp, "%c",' ');
		     else
			fprintf(fp, "%c", 'a');
		}	
		fprintf(fp,"%c",'\n');
		rewind(fp); // Set file pointer back to start.

		int num_misspelled = check_words(fp, hashtable, misspelled);
		ck_assert(num_misspelled == 2); // one long string of a's should be misspelled

		fclose(fp);
		remove(TEMPINPUT);
	}
	else
	   ck_assert(NULL != fp);
}
END_TEST

/**
 This test uses check_words to verify a single word with multiple punctuation marks before the word results in
 the punctuation stripped and no misspellings.
*/
START_TEST(test_check_words_before_punct)
{
	hashmap_t hashtable[HASH_SIZE];
	load_dictionary(DICTIONARY, hashtable);
	char *misspelled[MAX_MISSPELLED];
	// Rather than a bunch of input files, lets dynamically write the file.
	// This will open file for read/write, write a test string, and the program will read that string.
	FILE *fp = fopen(TEMPINPUT, "w+"); 
	if (NULL != fp) {
		fprintf(fp, "%s", ",(hello");
		rewind(fp); // Set file pointer back to start.

		int num_misspelled = check_words(fp, hashtable, misspelled);
		ck_assert(num_misspelled == 0);

		fclose(fp);
		remove(TEMPINPUT);
	}
	else
	   ck_assert(NULL != fp);
	
}
END_TEST

/**
 This test uses check_words to verify a single word with multiple punctuation marks after the word results in
 the punctuation stripped and no misspellings.
*/
START_TEST(test_check_words_after_punct)
{
	hashmap_t hashtable[HASH_SIZE];
	load_dictionary(DICTIONARY, hashtable);
	char *misspelled[MAX_MISSPELLED];
	// Rather than a bunch of input files, lets dynamically write the file.
	// This will open file for read/write, write a test string, and the program will read that string.
	FILE *fp = fopen(TEMPINPUT, "w+"); 
	if (NULL != fp) {
		fprintf(fp, "%s", "hello?!?,");
		rewind(fp); // Set file pointer back to start.

		int num_misspelled = check_words(fp, hashtable, misspelled);
		ck_assert(num_misspelled == 0);

		fclose(fp);
		remove(TEMPINPUT);
	}
	else
	   ck_assert(NULL != fp);
	
}
END_TEST

/**
 This test uses check_words to verify a single word with multiple punctuation marks after the word results in
 the punctuation stripped and no misspellings.
*/
START_TEST(test_check_words_before_and_after_punct)
{
	hashmap_t hashtable[HASH_SIZE];
	load_dictionary(DICTIONARY, hashtable);
	char *misspelled[MAX_MISSPELLED];
	// Rather than a bunch of input files, lets dynamically write the file.
	// This will open file for read/write, write a test string, and the program will read that string.
	FILE *fp = fopen(TEMPINPUT, "w+"); 
	if (NULL != fp) {
		fprintf(fp, "%s", ",(----hello----)?!.");
		rewind(fp); // Set file pointer back to start.

		int num_misspelled = check_words(fp, hashtable, misspelled);
		ck_assert(num_misspelled == 0);

		fclose(fp);
		remove(TEMPINPUT);
	}
	else
	   ck_assert(NULL != fp);
	
}
END_TEST

/**
 This test uses check_words to verify several numeric values passes.
*/
START_TEST(test_check_words_numeric)
{
	hashmap_t hashtable[HASH_SIZE];
	load_dictionary(DICTIONARY, hashtable);
	char *misspelled[MAX_MISSPELLED];
	// Rather than a bunch of input files, lets dynamically write the file.
	// This will open file for read/write, write a test string, and the program will read that string.
	FILE *fp = fopen(TEMPINPUT, "w+"); 
	if (NULL != fp) {
		fprintf(fp, "%s", "$100,000 50.75 -109.3 -$80.234 3.14159 -56.35E65 -2 0 0.00 12345,67890 59.65%");
		rewind(fp); // Set file pointer back to start.

		int num_misspelled = check_words(fp, hashtable, misspelled);
		ck_assert(num_misspelled == 0);

		fclose(fp);
		remove(TEMPINPUT);
	}
	else
	   ck_assert(NULL != fp);
	
}
END_TEST

/**
 This test uses check_words to verify several non-alphanmeric passes.
*/
START_TEST(test_check_words_non_alphanumeric)
{
	hashmap_t hashtable[HASH_SIZE];
	load_dictionary(DICTIONARY, hashtable);
	char *misspelled[MAX_MISSPELLED];
	// Rather than a bunch of input files, lets dynamically write the file.
	// This will open file for read/write, write a test string, and the program will read that string.
	FILE *fp = fopen(TEMPINPUT, "w+"); 
	if (NULL != fp) {
		fprintf(fp, "%s", "$$ / - ()^&% % -- ******** '' '' @# ^& == ~`-_++/,. <> ; '");
		rewind(fp); // Set file pointer back to start.

		int num_misspelled = check_words(fp, hashtable, misspelled);
		ck_assert(num_misspelled == 0);

		fclose(fp);
		remove(TEMPINPUT);
	}
	else
	   ck_assert(NULL != fp);
	
}
END_TEST

/**
 Test overflow case where two word several hundred thousand characters is processed and returned as 1 misspelling
*/
START_TEST(test_check_words_input_overflow)
{
	hashmap_t hashtable[HASH_SIZE];
	load_dictionary(DICTIONARY, hashtable);
	char *misspelled[MAX_MISSPELLED];
	// Rather than a bunch of input files, lets dynamically write the file.
	// This will open file for read/write, write a test string, and the program will read that string.
	FILE *fp = fopen(TEMPINPUT, "w+"); 
	if (NULL != fp) {
		for (int i = 0; i < 525123; i++)
		     fprintf(fp, "%c", 'a');
		fprintf(fp, " ");
		for (int i = 0; i < 250; i++)
	             fprintf(fp, "%c", 'b');
		fprintf(fp, "\n");
		rewind(fp); // Set file pointer back to start.

		int num_misspelled = check_words(fp, hashtable, misspelled);
		ck_assert_int_eq(num_misspelled, 2);

		fclose(fp);
		remove(TEMPINPUT);
	}
	else
	   ck_assert(NULL != fp);
	
}
END_TEST

/**
 This test uses check_words to verify several special characters.
*/
START_TEST(test_check_words_special)
{
	hashmap_t hashtable[HASH_SIZE];
	load_dictionary(DICTIONARY, hashtable);
	char *misspelled[MAX_MISSPELLED];
	// Rather than a bunch of input files, lets dynamically write the file.
	// This will open file for read/write, write a test string, and the program will read that string.
	FILE *fp = fopen(TEMPINPUT, "w+"); 
	if (NULL != fp) {
		unsigned char theChar;
		// Loop and write a byte followed by a space. Loop through all 1 byte values.
		for (unsigned int i = 0; i < 255; i++) {
		     theChar = i;
		     fprintf(fp, "%c ", theChar);
		}
		rewind(fp); // Set file pointer back to start.

		int num_misspelled = check_words(fp, hashtable, misspelled);
		ck_assert(num_misspelled == 0);

		fclose(fp);
		remove(TEMPINPUT);
	}
	else
	   ck_assert(NULL != fp);
	
}
END_TEST

/**
 This test uses check_words to verify several special characters surrounding a correctly word do not cause the word to be incorrectly reported as misspelled.
*/
START_TEST(test_check_words_special_correct)
{
	hashmap_t hashtable[HASH_SIZE];
	load_dictionary(DICTIONARY, hashtable);
	char *misspelled[MAX_MISSPELLED];
	// Rather than a bunch of input files, lets dynamically write the file.
	// This will open file for read/write, write a test string, and the program will read that string.

	     FILE *fp = fopen(TEMPINPUT, "w+"); 
	
	     if (NULL != fp) {
		// Loop through 3 ranges of non alpha ascii
		for (unsigned int i = 1; i < 65; i++) {
	   	     unsigned char theChar = i;
		     fprintf(fp, "%c%s%c\n ", theChar, "Welcome", theChar);
		}
		for (unsigned int i = 91; i < 97; i++) {
	   	     unsigned char theChar = i;
		     fprintf(fp, "%c%s%c\n ", theChar, "Welcome", theChar);
		}
		for (unsigned int i = 123; i < 256; i++) {
	   	     unsigned char theChar = i;
		     fprintf(fp, "%c%s%c\n ", theChar, "Welcome", theChar);
		}
		rewind(fp); // Set file pointer back to start.

		int num_misspelled = check_words(fp, hashtable, misspelled);
		
                ck_assert_msg(0 == num_misspelled);
		


		fclose(fp);
		remove(TEMPINPUT);
	     }
	     else
	        ck_assert(NULL != fp);
	
	
	
}
END_TEST

START_TEST(test_check_words_normal)
{
    hashmap_t hashtable[HASH_SIZE];
    load_dictionary(DICTIONARY, hashtable);
    char* expected[3];
    expected[0] = "sogn";
    expected[1] = "skyn";
    expected[2] = "betta";
    char *misspelled[MAX_MISSPELLED];
    FILE *fp = fopen("test1.txt", "r");
    int num_misspelled = check_words(fp, hashtable, misspelled);
    ck_assert(num_misspelled == 3);
    bool test = strlen(misspelled[0]) == strlen(expected[0]);
    int len1 = strlen(misspelled[0]);
    int len2 = strlen(expected[0]);
    ck_assert_msg(test, "%d!=%d", len1, len2);
    ck_assert_msg(strcmp(misspelled[0], expected[0]) == 0);
    ck_assert_msg(strcmp(misspelled[1], expected[1]) == 0);
    ck_assert_msg(strcmp(misspelled[2], expected[2]) == 0);
}
END_TEST

Suite *
check_word_suite(void)
{
    Suite * suite;
    TCase * check_word_case;
    suite = suite_create("check_word");
    check_word_case = tcase_create("Core");
    tcase_add_test(check_word_case, test_check_word_normal);
    tcase_add_test(check_word_case, test_check_word_leading_trailing_punct);
    tcase_add_test(check_word_case, test_check_word_numerics);
    tcase_add_test(check_word_case, test_check_word_null_inputs);
    tcase_add_test(check_word_case, test_check_word_empty_hashtable);
    suite_add_tcase(suite, check_word_case);

    return suite;
}

Suite *
check_words_suite(void)
{
    Suite * suite;
    TCase * check_words_case;
    suite = suite_create("check_words");
    check_words_case = tcase_create("Core");
    tcase_add_test(check_words_case, test_check_words_normal);
    tcase_add_test(check_words_case, test_check_words_numeric);
    tcase_add_test(check_words_case, test_check_words_non_alphanumeric);
    tcase_add_test(check_words_case, test_check_words_before_punct);
    tcase_add_test(check_words_case, test_check_words_after_punct);
    tcase_add_test(check_words_case, test_check_words_before_and_after_punct);
    tcase_add_test(check_words_case, test_check_words_hundred_char_multiple_one_word);
    tcase_add_test(check_words_case, test_check_words_hundred_char_multiple_two_word);
    tcase_add_test(check_words_case, test_check_words_special);
    tcase_add_test(check_words_case, test_check_words_special_correct);
    tcase_add_test(check_words_case, test_check_words_input_overflow);
    suite_add_tcase(suite, check_words_case);

    return suite;
}

Suite *
load_dictionary_suite(void)
{
    Suite * suite;
    TCase * check_dict_case;
    suite = suite_create("load_dictionary");
    check_dict_case = tcase_create("Core");
    tcase_add_test(check_dict_case, test_dictionary_load_only);
    tcase_add_test(check_dict_case, test_dictionary_file_not_found);
    tcase_add_test(check_dict_case, test_dictionary_null_filename);
    tcase_add_test(check_dict_case, test_dictionary_word_overflow);
    tcase_add_test(check_dict_case, test_dictionary_normal_buckets);
    tcase_add_test(check_dict_case, test_dictionary_normal_mixed_case);
    tcase_add_test(check_dict_case, test_dictionary_long_word_truncate);
    tcase_add_test(check_dict_case, test_dictionary_truncate_after_space);
    suite_add_tcase(suite, check_dict_case);

    return suite;
}

int
main(void)
{
    int failed;
    Suite *suite;
    SRunner *runner;
    
    // Initial suite
    suite = check_word_suite();
    runner = srunner_create(suite);
    
    // Add Check Words suite
    suite = check_words_suite();
    srunner_add_suite(runner, suite);

    // Add Load Dictionary Suite
    suite = load_dictionary_suite();
    srunner_add_suite(runner, suite);	

    srunner_run_all(runner, CK_NORMAL);
    failed = srunner_ntests_failed(runner);
    srunner_free(runner);



    return (failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}

