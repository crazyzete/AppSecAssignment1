spell : test_main.o dictionary.o spell.o 
	gcc -o spell test_main.o dictionary.o spell.o -lcheck_pic -lpthread -lrt -lm -lsubunit

test_main.o : test_main.c 
	gcc -c test_main.c

dictionary.o : dictionary.c 
	gcc -c dictionary.c

spell.o : spell.c 
	gcc -c spell.c

clean :
	rm -f spell test_main.o dictionary.o spell.o 


