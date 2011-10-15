#include <stdio.h>
#include <wavreader.h>
#include <error_handler.h>
#include <huffman.h>

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

        free(wh);
        fclose(fp);
        return 0;
}
