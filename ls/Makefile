CC=cc
CFLAGS=-I. -Wall

ls: ls.o
	$(CC) $(CFLAGS) -o ls ls.o

ls.o : ls.c
	$(CC) $(CFLAGS) -g -c ls.c -o ls.o