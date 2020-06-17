all: idispatcher.o idispatcher

idispatcher.o: idispatcher.c
	gcc -Wall -g -I -std=C99 -c idispatcher.c -o idispatcher.o -lm

idispatcher: idispatcher.o
	gcc -Wall -g -I -std=C99 idispatcher.o -o idispatcher -lm

clean:
	rm *.o
