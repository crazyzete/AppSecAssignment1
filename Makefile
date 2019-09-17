testspell : test_main.o dictionary.o spell.o 
	gcc -Wall -o testspell test_main.o dictionary.o spell.o -lcheck_pic -lpthread -lrt -lm -lsubunit

spell: main.o dictionary.o spell.o
	gcc -Wall -o spell main.o dictionary.o spell.o

test_main.o : test_main.c 
	gcc -Wall -c test_main.c

dictionary.o : dictionary.c 
	gcc -Wall -c dictionary.c

spell.o : spell.c 
	gcc -Wall -c spell.c

main.o : main.c
	gcc -Wall -c main.c

clean :
	rm -f spell main.o dictionary.o spell.o
	rm -f testspell test_main.o

all: clean testspell spell


