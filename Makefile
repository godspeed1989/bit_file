CC=g++
CPPFLAGS=-Wall -g

all: test

test.o: test.cpp bitfile.hpp

test: test.o
	$(CC) $+ -o $@

FILE=test
run:
	./test $(FILE)
	@echo compare $(FILE) and $(FILE).out
	diff $(FILE) $(FILE).out

clean:
	rm -rf *.o test

