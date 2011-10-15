#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <bitio.h>
#include <wavreader.h>
#include <error_handler.h>
#include <huffman.h>
#include <delta.h>
#include <run_length.h>

#define HUFFMAN 1
#define DELTA 2
#define RUN_LENGTH 3

typedef int boolean;
#define TRUE 1
#define FALSE 0

/* Global variables, a necessary evil. */

/* The data divided in channels. */

uint32_t **data_vector; 
bitbuffer **data_buffer;
wavheader *input_file_header;
uint64_t *frequencies;

void enc_prepare_input_file(FILE *fp)
{
        input_file_header = malloc(sizeof(wavheader));
        getHeader(input_file_header, fp);
}

void enc_multiplex ()
{

}

void enc_huffman(int previous, FILE *in_fp)
{
        if (previous == -1) {
                /* Huffman is the first encoding option. */
                printf("Applying Huffman encoding ...\n");

                load_to_uint32(in_fp, input_file_header, &data_vector);

                data_buffer = calloc(input_file_header->numChannels, sizeof(bitbuffer*));
                frequencies = calloc(pow(2,input_file_header->bitsPerSample), sizeof(uint64_t));

                int curr_channel=0;
                for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
                        binit(data_buffer[curr_channel], input_file_header->subchunk2size);
                        /* We encode each channel in a separate bit buffer. */
                        hf_encode(data_vector[curr_channel], data_buffer[curr_channel], &frequencies, input_file_header->subchunk2size, pow(2,input_file_header->bitsPerSample));
                }
        }
}

void enc_run_length()
{
    printf("Run-length encoding...\n");
}

void enc_delta()
{
    printf("Difference encoding...\n");
}

int main (int argc, char* argv[])
{
    unsigned int n_encodings = 0,
                 first_enc = 0, 
                 sec_enc = 0,
                 third_enc = 0;
    boolean optimize = FALSE;
    FILE *in_fp, *out_fp;
    int get_opt;

    while((get_opt = getopt(argc, argv, "cdho"))!= -1){
        switch(get_opt){
            case 'o': optimize = TRUE; break;
            case 'd': 
                n_encodings++;
                if(first_enc == 0) first_enc = DELTA;
                else if(sec_enc == 0) sec_enc = DELTA;
                else if(third_enc == 0) third_enc = DELTA;
                else {
                    FORMAT_ERROR; return 1;
                }
                break;
            case 'h':
                n_encodings++;
                if(first_enc == 0) first_enc = HUFFMAN;
                else if(sec_enc == 0) sec_enc = HUFFMAN;
                else if(third_enc == 0) third_enc = HUFFMAN;
                else {
                    FORMAT_ERROR; return 1;
                }
                break;
            case 'c':
                n_encodings++;
                if(first_enc == 0) first_enc = RUN_LENGTH;
                else if(sec_enc == 0) sec_enc = RUN_LENGTH;
                else if(third_enc == 0) third_enc = RUN_LENGTH;
                else {
                    FORMAT_ERROR; return 1;
                }
                break;
            default: FORMAT_ERROR; return 1;
        }
    }
    
    if(n_encodings == 0){
        FORMAT_ERROR; return 1;
    }
    
    if((argv[optind] == NULL) || (argv[optind+1] == NULL)){
        FORMAT_ERROR; return 1;
    }

    printf("AAI-Codec version 0.0.0.0.1");
    printf("Encoding file \"%s\" to \"%s\"...\n", argv[optind], argv[optind+1]);
        
    if(optimize == TRUE) printf("Optimization on.\n");

    if((in_fp = fopen(argv[optind], "rb"))==NULL){
        IO_OPEN_ERROR;
        return 1;
    }
    
    if((out_fp = fopen(argv[optind+1], "wb+"))==NULL){
        IO_OPEN_ERROR;
        fclose(in_fp);
        return 1;
    }
    
    printf("Starting encoding process..");

    enc_prepare_input_file(in_fp);

    if(first_enc == HUFFMAN)
            enc_huffman(-1, in_fp);
    else if(first_enc == DELTA)
            enc_delta(-1);
        else 
            enc_run_length(-1);

    /*XXX: Acertar os índices.*/
    if(sec_enc == HUFFMAN)
            enc_huffman(0, in_fp);
    else if(sec_enc == DELTA)
            enc_delta();
        else if(sec_enc == RUN_LENGTH) 
                enc_run_length();
    
    /*XXX: Acertar os índices.*/
    if(third_enc == HUFFMAN)
            enc_huffman(0, in_fp);
    else if(third_enc == DELTA)
            enc_delta();
    else if(third_enc == RUN_LENGTH)
            enc_run_length();
   
    fclose(in_fp);
    fclose(out_fp);
    free(input_file_header);
    return 0;
}

