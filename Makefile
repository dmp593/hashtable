.DEFAULT_GOAL=all


CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic

PROG=test.app


hashtable.o: hashtable.c hashtable.h
	$(CC) $(CFLAGS) -c hashtable.c

build: hashtable.o
	$(CC) $(CFLAGS) -o $(PROG) hashtable.o test/hashtable.c

test: build
	./$(PROG)

clean:
	rm -rf *.o $(PROG)

all: clean test
