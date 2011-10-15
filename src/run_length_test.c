#include <stdlib.h>
#include <bitio.h>
#include <inttypes.h>
#include <run_length.h>

int main(int argc, char* argv[]){ 
    bitbuffer *b = malloc(sizeof(bitbuffer));
    bitbuffer *bout = malloc(sizeof(bitbuffer));
    bitbuffer *bout2 = malloc(sizeof(bitbuffer));
    uint32_t bits_run = 0;
    uint32_t  bits_code = 0;

    binit(b, 1);
    binit(bout, 1);
    binit(bout2, 1);
 
	int i;
	for(i=0; i<33; i++){
		if(i<8)
			bwrite(b, 0);
		else
			bwrite(b, 1);
    }

    rl_encode(b, bout, 8, &bits_run, &bits_code);

    bprint(b);
    bprint(bout);

    printf("bits run: %"PRIu32"\n", bits_run);
    printf("bits code: %"PRIu32"\n", bits_code);
    
    rl_decode(8, bits_code, bits_run, bout, bout2);
    bprint(bout2);
    
    
    bdestroy(b);
    bdestroy(bout);
    bdestroy(bout2);
    free(bout);
    free(bout2);
    free(b);
    return 0;
}
