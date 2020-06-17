all: ish

ish: ish.o lex.yy.o
	gcc -o ish -g ish.o

ish.o: ish.c
	gcc -Wall -g -c ish.c

clean:
	rm -f *.o
	rm -f ish
