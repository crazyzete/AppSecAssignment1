testspell : test_main.o dictionary.o spell.o 
	gcc -o testspell test_main.o dictionary.o spell.o -lcheck_pic -lpthread -lrt -lm -lsubunit

spell: main.o dictionary.o spell.o
	gcc -o spell main.o dictionary.o spell.o

test_main.o : test_main.c 
	gcc -c test_main.c

dictionary.o : dictionary.c 
	gcc -c dictionary.c

spell.o : spell.c 
	gcc -c spell.c

main.o : main.c
	gcc -c main.c

clean :
	rm -f spell main.o dictionary.o spell.o
	rm -f testspell test_main.o

all: clean testspell spell


