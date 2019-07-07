CC=gcc
CFLAGS=-Wall -g

lisp: lisp.o
	$(CC) $(CFLAGS) -o lisp lisp.o

lisp.o: lisp.c
	$(CC) $(CFLAGS) -c lisp.c
