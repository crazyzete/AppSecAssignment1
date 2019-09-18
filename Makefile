testspell : test_main.o dictionary.o spell.o 
	gcc -g -Wall -o testspell test_main.o dictionary.o spell.o -lcheck_pic -lpthread -lrt -lm -lsubunit

spell: main.o dictionary.o spell.o
	gcc -g -Wall -o spell main.o dictionary.o spell.o

fuzzyspell: 
	afl-gcc -o fuzzmain main.c spell.c dictionary.c dictionary.h

test_main.o : test_main.c 
	gcc -Wall -c -g test_main.c

dictionary.o : dictionary.c 
	gcc -Wall -c -g dictionary.c

spell.o : spell.c 
	gcc -Wall -c -g spell.c

main.o : main.c
	gcc -Wall -c -g main.c

clean :
	rm -f spell main.o dictionary.o spell.o
	rm -f testspell test_main.o

all: clean testspell spell


