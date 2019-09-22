testspell : test_main.o dictionary.o spell.o 
	gcc -g -fprofile-arcs -ftest-coverage -Wall -o testspell test_main.o dictionary.o spell.o -lcheck_pic -lpthread -lrt -lm -lsubunit

spell: main.o dictionary.o spell.o
	gcc -g -fprofile-arcs -ftest-coverage -Wall -o spell main.o dictionary.o spell.o

fuzzyspell: 
	afl-gcc -o fuzzmain main.c spell.c dictionary.c dictionary.h

test_main.o : test_main.c 
	gcc -Wall -fprofile-arcs -ftest-coverage -c -g -O0 test_main.c

dictionary.o : dictionary.c 
	gcc -Wall -fprofile-arcs -ftest-coverage -c -g -O0 dictionary.c

spell.o : spell.c 
	gcc -Wall -fprofile-arcs -ftest-coverage -c -g -O0 spell.c

main.o : main.c
	gcc -Wall -fprofile-arcs -ftest-coverage -c -g -O0 main.c

clean :
	rm -f spell main.o dictionary.o spell.o
	rm -f testspell test_main.o

all: clean testspell spell


