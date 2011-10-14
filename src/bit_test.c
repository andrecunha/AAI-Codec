#include <bitio.h>
#include <stdlib.h>
#include <inttypes.h>

int main (int argc, char* argv[])
{/*
	FILE *fp_in, *fp_out;
	
	if((fp_out = fopen("test_out.txt", "wb")) == NULL) return 1;		*/
	bitbuffer *b = malloc(sizeof(bitbuffer));
	binit(b, 2);
	
	int i;
	for(i=0; i<33; i++){
		if(i<8)
			bwrite(b, 0);
		else
			bwrite(b, 1);
    }

	bprint(b);/*
	uint8_t d;
    i=0;
	while(!bempty(b)){
		bread(b,&d);
	}
	
    
	bflush(b, fp_out);
	fclose(fp_out);
	
	if((fp_in = fopen("test_out.txt", "rb"))==NULL) return 1;

	bitbuffer *bin = malloc(sizeof(bitbuffer));
	bget(bin, fp_in);*/
	/*bprint(b);
	bprint(bin);*/
/*
    bprint(bin);
	fclose(fp_in);

	bdestroy(bin);
	free(bin);*/

    uint32_t *out, tam;
    
    tam = b_to_uint32(b, &out, 2);
    
    for (i = 0; i<tam; i++){
        printf("out: %x\n", out[i]);
    }

    bitbuffer *b2 = malloc(sizeof(bitbuffer));
    binit(b2, 1);
    b_from_uint32(b2, out, tam, 2, b->bits_last);   
    bprint(b2);

    free(out);
    
    bdestroy(b2);
    free(b2);
	bdestroy(b);
	free(b);

    

	return 0;
}
