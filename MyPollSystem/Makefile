
CC = gcc
CFLAGS = -pedantic -w -g


myhie: myhie.o sorter.o toolkit.o
	$(CC) $(CFLAGS) -o myhie myhie.o sorter.o toolkit.o

myhie.o: myhie.c sorter.h toolkit.h
	$(CC) $(CFLAGS) -c myhie.c

sorter.o: sorter.c sorter.h toolkit.h
	$(CC) $(CFLAGS) -c sorter.c

toolkit.o: toolkit.c toolkit.h
	$(CC) $(CFLAGS) -c toolkit.c

clean:
	rm -f *.o myhie
