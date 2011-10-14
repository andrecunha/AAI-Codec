#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <bitio.h>

int binit(bitbuffer *b, uint32_t size)
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

int bwrite(bitbuffer *b, uint8_t data)
{
    int difference;

	/* If buffer is full, realloc. */
	if (!b->size || bfull(b)){
        difference = b->data - b->original_data;
        b->original_data = realloc(b->original_data, ++b->size);
        b->data = b->original_data + difference;       
        b->n_bytes++;
        b->bits_last=0;
        b->bits_offset=0;
        memset(b->original_data+b->size-1, 0, sizeof(uint8_t));
    }

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

int bread(bitbuffer *b, uint8_t *out)
{
	if(bempty(b)){
		return 1;
    }

	/* If there is only one byte, and the offset is equal to the number of bits in the last byte, than there is no bit left in the buffer. */
	if ((b->n_bytes == 1) && (b->bits_offset == b->bits_last)){
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

	b->bits_offset++;
    
	return 0;
}

void bwritev(bitbuffer *b, uint32_t data, unsigned int count)
{
	if (count<1 || count>32)
		return;

	int i;
	for(i=0; i<count; i++){
		bwrite(b, (uint8_t)((data >> (count-i-1)) & 1));
	}
}

int breadv(bitbuffer *b, uint32_t *output, unsigned int count)
{
	if(count<1 || count>32)
		return 0;

	*output = 0;
	uint8_t aux8;
	uint32_t aux32;
	int i;
	for(i=0; i<count; i++){
		if (bread(b, &aux8))
			return i;
		aux32 = 0; /*XXX: Pode ser que seja inútil.*/
		aux32 = aux8;
		aux32 <<= (count-i-1);
		*output = *output | aux32;
	}

	return count;
}

int bflush(bitbuffer *b, FILE *f)
{	
	if(fwrite(&b->size, sizeof(uint32_t), 1, f)!=1) return 1;
	if(fwrite(&b->bits_last, sizeof(unsigned int), 1, f)!=1) return 1;

	if (fwrite(b->original_data, sizeof(uint8_t), b->size, f) != b->size)
		return 1;

	return 0;
}


uint32_t b_to_uint32(bitbuffer *b, uint32_t **output, uint8_t nbits){
    uint32_t tam = ceil((float)((b->n_bytes*8) - (8 - b->bits_last))/nbits), i;
    
    *output = malloc(tam*sizeof(uint32_t));
    
    for (i=0; i<tam; i++){
        (*output)[i] = 0;
    }

    for (i=0; i<tam; i++){
        breadv(b, (*output)+i, nbits);
    }
    return tam; 
}

void b_from_uint32(bitbuffer *b, uint32_t *input, 
        uint32_t input_length, uint8_t nbits,  unsigned int b_last){
    uint32_t i;

    for(i=0; i<input_length; i++){  
       bwritev(b, input[i], nbits);
    }
    b->bits_last = b_last;
}

int bget(bitbuffer *b, FILE *f)
{	
	if(fread(&b->size, sizeof(uint32_t), 1, f)!=1) return 1;	
	if(fread(&b->bits_last, sizeof(unsigned int), 1, f)!=1) return 1;
    b->n_bytes = b->size;
    b->bits_offset = 0;
	b->data = b->original_data = malloc(b->size);

	if (b->data) memset(b->data, 0, b->size);
	else return 1;	
	
	if (fread(b->data, sizeof(uint8_t), b->size, f) != b->size)
		return 1;

	return 0;
}

void bprint(bitbuffer *b)
{
	printf("bprint: size = %"PRIu32"\n", b->size);
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

