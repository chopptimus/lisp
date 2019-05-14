CC=gcc
CFLAGS=-Wall

lisp: lisp.o
	$(CC) -o lisp lisp.o

lisp.o: lisp.c
	$(CC) -c lisp.c
