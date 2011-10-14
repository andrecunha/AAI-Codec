CC = gcc
INCLUDE = ../include/
CFLAGS = -ansi -Wall -g -lm

all: bit_test


clean:
	rm -f ../bin/*

pq_test: pq_test.c priority_queue.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o ../bin/pq_test pq_test.c priority_queue.c

bit_test: bit_test.c bitio.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o bit_test bit_test.c bitio.c

wav_test: wav_test.c wavreader.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o wav_test wav_test.c wavreader.c

encoder: encoder.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o encode encoder.c

run_length_test: run_length_test.c run_length.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o run_length_test run_length_test.c run_length.c bitio.c

hf_test: hf_test.c bitio.c priority_queue.c huffman.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o hf_test hf_test.c bitio.c priority_queue.c huffman.c

