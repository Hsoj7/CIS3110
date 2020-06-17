all: igoodcnt.o igoodcnt

igoodcnt.o: igoodcnt.c
	gcc -pedantic -lpthread -lrt -c igoodcnt.c -o igoodcnt.o

igoodcnt: igoodcnt.o
	gcc -pedantic -lpthread -lrt igoodcnt.o -o igoodcnt

clean:
	rm *.o
