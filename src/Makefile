CC = gcc
INCLUDE = ../include/
CFLAGS = -ansi -Wall -g

all: bit_test


clean:
	rm -f ../bin/*

pq_test: pq_test.c priority_queue.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o ../bin/pq_test pq_test.c priority_queue.c

bit_test: bit_test.c bitio.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o bit_test bit_test.c bitio.c


