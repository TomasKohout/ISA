
CC=g++
CFLAGS=-std=c++11
LDFLAGS=-lssl -lcrypto
EXECUTABLE=popcl

.PHONY: all clean

all:
	g++ $(CFLAGS) -c *.cpp
	g++ $(CFLAGS) -o  popcl *.o *.h -lssl -lcrypto

clean:
	rm -rf *.o
