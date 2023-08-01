CC = gcc 
CFLAGS = -Wall -g
LFLAGS = -lreadline

all: linkedlist.o PMan

linkedlist.o: linkedlist.c linkedlist.h
	$(CC) $(CFLAGS) -c linkedlist.c

PMan: PMan.c linkedlist.o
	$(CC) $(CFLAGS) PMan.c $(LFLAGS) linkedlist.o -o PMan

clean:
	rm -f linkedlist.o PMan
