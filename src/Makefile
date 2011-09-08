CC = gcc
INCLUDE = ../include/
CFLAGS = -ansi -Wall

all: pq_test bit_test


clean:
	rm -f ../bin/*

pq_test: pq_test.c priority_queue.c
	$(CC) -I$(INCLUDE) -o ../bin/pq_test pq_test.c priority_queue.c

bit_test: bit_test.c bitio.c
	$(CC) -I$(INCLUDE) -o ../bin/bit_test bit_test.c bitio.c


