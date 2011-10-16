CC = gcc
INCLUDE = ../include/
CFLAGS = -ansi -Wall -g -lm

all: run_length_test

clean:
	rm -f *.o

distclean:
	rm -f ../bin/* *.o

################################# TESTS ################################

pq_test: pq_test.c priority_queue.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o ../bin/pq_test pq_test.c priority_queue.c

bit_test: bit_test.c bitio.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o bit_test bit_test.c bitio.c

wav_test: wav_test.c wavreader.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o wav_test wav_test.c wavreader.c

encoder: encode.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o encode encode.c

run_length_test: run_length_test.c run_length.c bitio.c wavreader.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o run_length_test run_length_test.c run_length.c bitio.c wavreader.c

hf_test: hf_test.c bitio.c priority_queue.c huffman.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o hf_test hf_test.c bitio.c priority_queue.c huffman.c

huffman_test: huffman.c huffman_test.c bitio.c priority_queue.c wavreader.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o huffman_test huffman_test.c bitio.c priority_queue.c huffman.c wavreader.c

############################# FINAL BUILDS ############################

bitio: bitio.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o bitio.o -c bitio.c

priority_queue: priority_queue.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o priority_queue.o -c priority_queue.c

wavreader: wavreader.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o wavreader.o -c wavreader.c

delta: delta.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o delta.o -c delta.c

run_length: run_length.c 
	$(CC) $(CFLAGS) -I$(INCLUDE) -o run_length.o -c run_length.c

huffman: huffman.c bitio priority_queue
	$(CC) $(CFLAGS) -I$(INCLUDE) -o huffman.o -c huffman.c

encode: encode.c huffman run_length delta bitio wavreader
	$(CC) $(CFLAGS) -I$(INCLUDE) -o encode encode.c huffman.o run_length.o delta.o bitio.o priority_queue.o wavreader.o

