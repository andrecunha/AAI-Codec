#include <string.h>
#include <bitio.h>

int binit(bitbuffer *b, size_t size)
{
	b->data = b->original_data = malloc(size);

	if (b->data) {
		b->size = size;
		b->n_bytes = 0;
		b->bits_last = 0;
		b->bits_offset = 0;
		memset(b->data, 0, size);
		return 0;
	}
	return 1;
}

int bwrite(bitbuffer *b, byte data)
{
	/* If buffer is full, return error. */
	if (!b->size || bfull(b))
		return 1;

	/* If the last byte is fully occupied, try to store the bit on the next byte. */
	if (b->bits_last == 8) {
		b->n_bytes++;

		if (b->n_bytes > b->size)
			return 1;

		b->bits_last=0;
	}

	if(bempty(b))
		b->n_bytes = 1;

	if (data == 1)
		b->data[b->n_bytes-1] |= (1 << (8-b->bits_last-1));

	b->bits_last++;

	return 0;
}

int bread(bitbuffer *b, byte *out)
{
	if(bempty(b))
		return 1;

	/* If there is only one byte, and the offset is equal to the number of bits in the last byte, than there is no bit left in the buffer. */
	if ((b->n_bytes == 1) && (b->bits_offset+1 == b->bits_last)){
		b->n_bytes = 0;
		b->bits_offset = 0;
		return 1;
	}

	if (b->bits_offset == 8) {
		b->bits_offset = 0;
		b->data++;
		b->n_bytes--;
	}

	*out = b->data[0];

	*out >>= (8-(b->bits_offset)-1);

	*out &= 1;

	(b->bits_offset)++;

	return 0;
}

int bflush(bitbuffer *b, FILE *f)
{
	if (fwrite(b, sizeof(b->size)+sizeof(b->n_bytes)+sizeof(b->bits_last)+sizeof(b->bits_offset), 1, f) != 1)
		return 1;

	if (fwrite(b->data, sizeof(byte), b->size, f) != b->size)
		return 1;

	return 0;
}

void bprint(bitbuffer *b)
{
	printf("bprint: size = %lu\n", b->size);
	printf("bprint: n_bytes = %lu\n", b->n_bytes);
	printf("bprint: bits_last = %u\n", b->bits_last);
	printf("bprint: bits_offset = %u\n", b->bits_offset);
	printf("bprint: data = ");

	int i, data_h, data_l;
	for(i=0; i<b->n_bytes; i++){
		data_h = b->data[i] & 0xf0;
		data_h >>= 4;
		data_l = b->data[i] & 0x0f;

		printf("%x%x ", data_h, data_l);
	}

	printf("\n");
}

void bdestroy(bitbuffer *b)
{
	free(b->original_data);
}

int bfull(bitbuffer *b)
{
	return (b->n_bytes == b->size) && (b->bits_last == 8);
}

int bempty(bitbuffer *b)
{
	return (!b->n_bytes);
}

