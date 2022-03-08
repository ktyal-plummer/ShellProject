CC=gcc
CFLAGS=-I.

wish: main.o wish.o
	$(CC) -o wish main.o wish.o