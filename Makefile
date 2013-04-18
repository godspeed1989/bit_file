CC=g++
CPPFLAGS=-Wall -g

all: test

test.o: test.cpp bitfile.hpp

test: test.o
	$(CC) $+ -o $@
	./test $<
	diff $< $<.out


clean:
	rm -rf *.o test

