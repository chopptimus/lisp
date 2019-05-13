CC=gcc
CFLAGS=-Wall

clispy: clispy.o
	$(CC) -o clispy clispy.o

clispy.o: clispy.c
	$(CC) -c clispy.c
