#include <stdio.h>
#include <wavreader.h>
#include <error_handler.h>
#include <huffman.h>
#include <math.h>

int main (int argc, char* argv[])
{
        wavheader *wh = malloc(sizeof(wavheader));
        FILE *fp;

        if((fp = fopen(argv[1], "rb"))==NULL) {
            IO_OPEN_ERROR;
            return 1;
        }

        if(getHeader(wh, fp)==1) { 
            ERROR("getHeader");
            fclose(fp);
            free(wh);
            return 1;
        }

        printWavHeader(wh);

        uint32_t **data;
        load_to_uint32(fp,wh,&data);

        bitbuffer b;
        binit(&b, wh->subchunk2size);

        uint64_t *frequencies = calloc(pow(2,16), sizeof(uint64_t));
        hf_compute_frequencies(data[0], frequencies, wh->subchunk2size);
        hf_encode(data[0], &b, wh->subchunk2size, pow(2,16));

        printf("n_bytes: %lu\n", b.n_bytes);

        uint32_t *output = calloc(wh->subchunk2size,sizeof(uint32_t));
        hf_decode(&b, frequencies, output, pow(2,16));

        int i;
        for(i=0; i<wh->subchunk2size; i++)
                if(data[0][i] != output[i])
                        printf("Xiiiiiiiiiiiii....i\n");

        bdestroy(&b);
        free(wh);
        fclose(fp);
        return 0;
}
