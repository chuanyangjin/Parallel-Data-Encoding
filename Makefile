CC=gcc
CFLAGS=-g -pedantic -std=gnu11 -Wall -Wextra
LDFLAGS=-pthread

.PHONY: all
all: nyuenc

nyuenc: nyuenc.o

nyuenc.o: nyuenc.c global.h encode.h encode.c threadpool.h threadpool.c 
	gcc -pthread -o nyuenc nyuenc.c encode.c threadpool.c 

.PHONY: clean
clean:
	rm -f *.o nyuenc
