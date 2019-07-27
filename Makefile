CC=gcc
CFLAGS=-Wall -g

lisp: lisp.o mpc.o
	$(CC) $(CFLAGS) -o lisp lisp.o mpc.o

parser_test: parser_test.o mpc.o
	$(CC) $(CFLAGS) -o parser_test mpc.o parser_test.o

parser_test.o: parser_test.c
	$(CC) $(CFLAGS) -c parser_test.c

lisp.o: lisp.c
	$(CC) $(CFLAGS) -c lisp.c

mpc.o: mpc.c
	$(CC) $(CFLAGS) -c mpc.c
