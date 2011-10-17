#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <binheader.h>

int main(int argc, char *argv[])
{
    binheader *wh = malloc(sizeof(binheader));
    FILE *in_fp, *out_fp;

        
    if((in_fp = fopen(argv[1], "rb"))==NULL){
        IO_OPEN_ERROR;
        return 1;
    }
    
    if((out_fp = fopen(argv[2], "wb+"))==NULL){
        IO_OPEN_ERROR;
        return 1;
    }

    printf("Starting decoding process...\n");

    binh_get_header(wh, in_fp);
    
    switch (wh.firstEncoding){
            case HUFFMAN:
                    dec_huffman();
                    break;
            case RUN_LENGTH:
                    dec_length();
                    break;
            case DELTA: 
                    dec_delta();
    }

    switch(wh.secondEncoding){
            case HUFFMAN:
                    dec_huffman();
                    break;
            case RUN_LENGTH:
                    dec_length();
                    break;
            case DELTA: 
                    dec_delta();
            
    }

    switch(wh.thirdEncoding){
            case HUFFMAN:
                    dec_huffman();
                    break;
            case RUN_LENGTH:
                    dec_length();
                    break;
            case DELTA: 
                    dec_delta();
            
    }

    enc_prepare_output_file(out_fp);

    fclose(in_fp);
    fclose(out_fp);
    free(input_file_header);
    return 0;
}
