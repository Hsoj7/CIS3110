all: unixio.o unixio

unixio.o: unixio.c
	gcc -Wall -pedantic -std=c99 -g -c unixio.c -o unixio.o

unixio: unixio.o
	gcc -Wall -pedantic -std=c99 -g unixio.o -o unixio

clean:
	rm *.o
