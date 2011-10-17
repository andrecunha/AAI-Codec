#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <binheader.h>

unsigned int first_enc = 0,
             sec_enc = 0,
             third_enc = 0;

/* The data divided in channels. */
uint32_t **data_vector = NULL, *firsts = NULL,
         *max_bits = NULL;

bitbuffer *data_buffer = NULL, *output_buffer = NULL;

/* The header of the file being encoded. */
binheader *input_file_header;

/* The frequencies vector, generated by the Huffman encoding.  */
uint64_t *frequencies;

uint32_t nbits_run, nbits_code;

void dec_prepare_input_file(FILE *fp)
{
    input_file_header = malloc(sizeof(binheader));
    binh_get_header(input_file_header, fp, &frequencies, &firsts);
    first_enc = input_file_header->firstEncoding;
    sec_enc = input_file_header->secondEncoding;
    third_enc = input_file_header->thirdEncoding;
    nbits_run = input_file_header->rlBitsRun;
    nbits_code = input_file_header->rlBitsCode;
}


void dec_prepare_output_file (FILE *fp)
{

}

void dec_huffman(int previous, FILE *in_fp)
{
        printf("Applying Huffman decoding...\n");

        switch (previous) {
                case (-1):
                        {
                        }
                        break;

                case RUN_LENGTH:
                        {
                        }
                        break;

                case DELTA:
                        {
                        }
                        break;
        }
        }
}

void enc_run_length(int previous, FILE *in_fp)
{
    printf("Applying run-length decoding...\n");

    switch (previous) {
            case -1:
                    {
                    }
                    break;

            case HUFFMAN:
                    {
                    }
                    break;

            case DELTA:
                    {
                    }

                    }
                    break;


    }
}

void dec_delta(int previous, FILE *in_fp)
{

        printf("Delta decoding...\n");
        switch (previous) {
            case -1:
                    {
                    }
                    break;

            case RUN_LENGTH:
                    {
                    
                    }
                    break;

            case HUFFMAN:
                    {
                    }
                    break;
        }

}


int main(int argc, char *argv[])
{
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
