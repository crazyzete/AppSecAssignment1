#include <check.h>
#include "dictionary.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define DICTIONARY "wordlist.txt"
#define TESTDICT "test_wordlist.txt"
#define BOGUSDICT "bogus.txt"
#define TEMPDICT "temp_dict.txt"

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
 This test uses check_words to veriffy a single word with multiple punctuation marks before the word results in
 the punctuation stripped and no misspellings.
*/
START_TEST(test_check_words_before_punct)
{
	hashmap_t hashtable[HASH_SIZE];
	load_dictionary(DICTIONARY, hashtable);
	char *misspelled[MAX_MISSPELLED];
	// Rather than a bunch of input files, lets dynamically write the file.
	// This will open file for read/write, write a test string, and the program will read that string.
	FILE *fp = fopen("test_tmp.txt", "rw"); 
	if (NULL != fp) {
		fprintf(fp, "%s", ",(hello");
		rewind(fp); // Set file pointer back to start.

		int num_misspelled = check_words(fp, hashtable, misspelled);
		ck_assert(num_misspelled == 0);

		fclose(fp);
	}
	
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
    tcase_add_test(check_words_case, test_check_words_before_punct);
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

