CC=g++
CPPFLAGS=-Wall

all: test

test.o: test.cpp bitfile.hpp

test: test.o
	$(CC) $+ -o $@

clean:
	rm -rf *.o test

