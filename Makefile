CC=gcc
CFLAGS=-Wall -g

lisp: lisp.o mpc.o
	$(CC) $(CFLAGS) -o lisp lisp.o mpc.o

lisp.o: lisp.c
	$(CC) $(CFLAGS) -c lisp.c

mpc.o: mpc.c
	$(CC) $(CFLAGS) -c mpc.c
