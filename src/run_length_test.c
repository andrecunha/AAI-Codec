#include <stdlib.h>
#include <bitio.h>
#include <run_length.h>

int main(int argc, char* argv[]){ 
    uint32_t v[10] = {0, 0, 0, 0, 0, 1, 0, 1, 1, 1}, nbits_run;
    bitbuffer *b = malloc(sizeof(bitbuffer));
    bitbuffer *bout = malloc(sizeof(bitbuffer));

    binit(b, 1);
    binit(bout, 1);

    nbits_run = rl_encode_nbits(1, v, b, 10);
    bprint(b);

    rl_decode(16, 1, nbits_run, b, bout);
    bprint(bout);
    
    bdestroy(b);
    bdestroy(bout);
    free(bout);
    free(b);
    return 0;
}
