#include <string.h>
#include <bitio.h>

int binit(bitbuffer *b, size_t size)
{
	b->size = size;
	b->n_bytes=0;
	b->bits_last=0;
	b->data = malloc(size);
	memset(b->data, 0, size);

	if (b->data)
		return 0;
	return 1;
}

int bwrite(bitbuffer *b, byte data)
{
	if (!b->size || bfull(b))
		return 1;

	if(b->bits_last == 8){
		b->n_bytes++;
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

	*out = ((b->data[b->n_bytes-1]) >> (8-b->bits_last)) & 1;
	
	if(--(b->bits_last)){
		byte aux = ~(1 << (8-b->bits_last-1));
		b->data[b->n_bytes-1] &= aux;
	}else{
		b->bits_last = 8;
		(b->n_bytes)--;
	}
	
	return 0;
}

int bflush(bitbuffer *b, FILE *f)
{
	fwrite(b, sizeof(b->size)+sizeof(b->n_bytes)+sizeof(b->bits_last), 1, f);
	fwrite(b->data, sizeof(byte), b->size, f);

	return 0;
}

void bprint(bitbuffer *b)
{
	printf("bprint: size = %d\n", b->size);
	printf("bprint: n_bytes = %d\n", b->n_bytes);
	printf("bprint: bits_last = %d\n", b->bits_last);
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
	free(b->data);
}

int bfull(bitbuffer *b)
{
	return (b->n_bytes == b->size) && (b->bits_last == 8);
}

int bempty(bitbuffer *b)
{
	return !b->n_bytes;
}

