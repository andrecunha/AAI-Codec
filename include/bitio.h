#ifndef BITIO_H
#define BITIO_H

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;

struct _bitbuffer {
	size_t size;
	unsigned long n_bytes;
	unsigned int bits_last;
	byte *data;
};

typedef struct _bitbuffer bitbuffer;

int binit(bitbuffer *b, unsigned long size);

int bwrite(bitbuffer *b, byte data);

int bread(bitbuffer *b, byte *out);

int bflush(bitbuffer *b, FILE *f);

void bprint(bitbuffer *b);

void bdestroy(bitbuffer *b);

int bfull(bitbuffer *b);

int bempty(bitbuffer *b);

#endif /* BITIO_H */
