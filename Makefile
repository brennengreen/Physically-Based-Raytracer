CC=g++
CFLAGS=-I.

tracer: main.cpp
	$(CC) -o tracer main.cpp -I.