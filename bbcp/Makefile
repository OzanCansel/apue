CC=gcc
CFLAGS=-I. -Wall

bbcp: bbcp.o
	$(CC) $(CFLAGS) -o bbcp bbcp.o

bbcp.o : bbcp.c
	$(CC) $(CFLAGS) -g -c bbcp.c -o bbcp.o
