CC=g++
CPPFLAGS=-Wall -g

all: test test1

test.o: test.cpp bitfile.hpp
test1.o: test1.cpp bitfile.hpp

test: test.o
	$(CC) $+ -o $@

test1: test1.o
	$(CC) $+ -o $@

FILE=test
run:
	@echo ---run test---
	./test
	@echo ---run test1---
	./test1

clean:
	rm -rf *.o test test1

