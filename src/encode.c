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

#define HUFFMAN     1
#define DELTA       2
#define RUN_LENGTH  3

typedef int boolean;
#define TRUE 1
#define FALSE 0

/* 
 * COISAS QUE TEM QUE IR NO CABEÇALHO:
 *
 *  -> Número de bits por amostra;
 *  -> Número de canais;
 *  -> Organização do arquivo (nCanais*bitbuffer ou nCanais*vetor_de_uint32_t)
 *  -> Número de amostras por segundo.
 *  -> Número de bytes nos dados.
 *  -> Primeira/segunda/terceira técnicas de compressão.
 *
 * */

/* Global variables, a necessary evil. */

/* The data divided in channels. */
uint32_t **data_vector = NULL; 
bitbuffer *data_buffer = NULL, *output_buffer = NULL;

/* The header of the file being encoded. */
wavheader *input_file_header;

/* The frequencies vector, generated by the Huffman encoding.  */
uint64_t *frequencies;

/* Indicates where the final data is. */
int final_data;

uint32_t nbits_run, nbits_code, max_bits;

#define DATA_VECTOR 0
#define OUTPUT_BUFFER 1

void enc_prepare_input_file(FILE *fp)
{
        frequencies = NULL;
        input_file_header = malloc(sizeof(wavheader));
        if (getHeader(input_file_header, fp)) {
                ERROR("Error while reading header");
                exit(1);
        }
        printWavHeader(input_file_header);
}

void enc_prepare_output_file (FILE *fp)
{
        int curr_channel;
        /* XXX: ESCREVER CABEÇALHO!!!*/
        if (final_data == OUTPUT_BUFFER)
                for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++)
                    bflush(&output_buffer[curr_channel], fp);

        if(frequencies)
                free(frequencies);
        
        for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
                if(data_buffer)
                    bdestroy(&data_buffer[curr_channel]);
                bdestroy(&output_buffer[curr_channel]);
                if(data_vector)
                    free(data_vector[curr_channel]);
        }
        
        if(data_vector)
            free(data_vector);
        if (data_buffer)
            free(data_buffer);
        free(output_buffer);
}

void enc_huffman(int previous, FILE *in_fp)
{
        printf("Applying Huffman encoding...\n");

        uint32_t channel_size = (input_file_header->subchunk2size)/(input_file_header->numChannels)/((input_file_header->bitsPerSample)/8);
        int curr_channel=0;
        switch (previous) {
                case (-1):
                        {
                                /* Huffman is the first encoding option. */

                                load_to_uint32(in_fp, input_file_header, &data_vector);

                                output_buffer = malloc(input_file_header->numChannels*sizeof(bitbuffer));

                                for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
                                        binit(&output_buffer[curr_channel], input_file_header->subchunk2size);
                                        /* We encode each channel in a separate bit buffer. */
                                        hf_encode(data_vector[curr_channel], &output_buffer[curr_channel], &frequencies, channel_size, pow(2,input_file_header->bitsPerSample));

                                        printf("nbytes of channel %d: %lu \n", curr_channel, output_buffer[curr_channel].n_bytes);
                                }
                        }
                        break;

                case RUN_LENGTH:
                        {

                                if(!data_vector) 
                                        data_vector = calloc(input_file_header->numChannels, sizeof(uint32_t *));

                                for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
                                        uint32_t size = (output_buffer[curr_channel].n_bytes*8 - (8-output_buffer[curr_channel].bits_last))/(nbits_run + nbits_code);
                                        b_to_uint32(&(output_buffer[curr_channel]), &(data_vector[curr_channel]), nbits_run + nbits_code);
                                        bdestroy(&(output_buffer[curr_channel]));
                                        binit(&output_buffer[curr_channel], input_file_header->subchunk2size);
                                        hf_encode(data_vector[curr_channel], &output_buffer[curr_channel], &frequencies, size, pow(2,nbits_run + nbits_code));
                                }
                        }
                        break;
        }

        final_data = OUTPUT_BUFFER;
}

void enc_run_length(int previous, FILE *in_fp)
{
    printf("Applying run-length encoding...\n");

    int curr_channel=0;

    switch (previous) {
            case -1:
                    {
                    /* Run-length is the first encoding. */
                    load_to_bitbuffer(in_fp, input_file_header, &data_buffer);

                    output_buffer = calloc(input_file_header->numChannels, sizeof(bitbuffer));

                    for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
                            binit(&(output_buffer[curr_channel]), input_file_header->subchunk2size);
                            rl_encode(&(data_buffer[curr_channel]), &(output_buffer[curr_channel]), input_file_header->bitsPerSample, &nbits_run, &nbits_code);
            }
                    }
                    break;

            case HUFFMAN:
                    {
                            if(!data_buffer) {
                                    data_buffer = calloc(input_file_header->numChannels, sizeof(bitbuffer));
                            }
                    for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
                            /* If Huffman has been applied before, so it's output is in output_buffer. We must copy it to data_buffer. */
                            bdestroy(&(data_buffer[curr_channel]));

                            binit(&(data_buffer[curr_channel]), input_file_header->subchunk2size);
                            bit_buffer_cpy(&(data_buffer[curr_channel]), &(output_buffer[curr_channel]), output_buffer[curr_channel].n_bytes*8 - (8 - output_buffer[curr_channel].bits_last));
                            bdestroy(&(output_buffer[curr_channel]));
                            binit(&(output_buffer[curr_channel]), input_file_header->subchunk2size);
                            rl_encode(&(data_buffer[curr_channel]), &(output_buffer[curr_channel]), input_file_header->bitsPerSample, &nbits_run, &nbits_code);
            }
                    }
                    break;


    }

    final_data = OUTPUT_BUFFER;
}

void enc_delta(int previous, FILE *in_fp)
{

        int curr_channel;
        uint32_t channel_size = (input_file_header->subchunk2size)/(input_file_header->numChannels)/((input_file_header->bitsPerSample)/8);
        printf("Delta encoding...\n");
        switch (previous) {
            case -1:
                    {
                        load_to_uint32(in_fp, input_file_header, &data_vector);

                        output_buffer = malloc(input_file_header->numChannels*sizeof(bitbuffer));

                         for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) 
                                dt_encode(data_vector[curr_channel], &(output_buffer[curr_channel]), channel_size, input_file_header->bitsPerSample);
                    }
                    break;
        }

        final_data = OUTPUT_BUFFER;
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

    printf("AAI-Codec version 0.0.0.0.1\n");
    printf("Encoding file \"%s\" to \"%s\"...\n", argv[optind], argv[optind+1]);
        
    if(optimize == TRUE) printf("Optimization on.\n");

    if((in_fp = fopen(argv[optind], "rb"))==NULL){
        IO_OPEN_ERROR;
        return 1;
    }
    
    if((out_fp = fopen(argv[optind+1], "wb+"))==NULL){
        IO_OPEN_ERROR;
        return 1;
    }
    
    printf("Starting encoding process...\n");

    enc_prepare_input_file(in_fp);

    switch (first_enc) {
            case HUFFMAN:
                enc_huffman(-1, in_fp);
                break;
            case RUN_LENGTH:
                enc_run_length(-1, in_fp);
                break;
            case DELTA:
                enc_delta(-1, in_fp);
    }

    switch (sec_enc) {
                case HUFFMAN:
                    switch(first_enc) {
                            case RUN_LENGTH:
                                    enc_huffman(RUN_LENGTH, in_fp);
                                    break;
                            case DELTA:
                                    enc_huffman(DELTA, in_fp);
                    }
                    break;

                case RUN_LENGTH:
                    switch(first_enc) {
                            case HUFFMAN:
                                    enc_run_length(HUFFMAN, in_fp);
                                    break;
                            case DELTA:
                                    enc_run_length(DELTA, in_fp);
                    }

                    break;
                case DELTA:
                    switch(first_enc) {
                            case HUFFMAN:
                                    enc_delta(HUFFMAN, in_fp);
                                    break;
                            case RUN_LENGTH:
                                    enc_delta(RUN_LENGTH, in_fp);
                    }
                    break;

     }

    switch (third_enc) {
                case HUFFMAN:
                    switch(sec_enc) {
                            case RUN_LENGTH:
                                    enc_huffman(RUN_LENGTH, in_fp);
                                    break;
                            case DELTA:
                                    enc_huffman(DELTA, in_fp);
                    }
                    break;

                case RUN_LENGTH:
                    switch(sec_enc) {
                            case HUFFMAN:
                                    enc_run_length(HUFFMAN, in_fp);
                                    break;
                            case DELTA:
                                    enc_run_length(DELTA, in_fp);
                    }

                    break;
                case DELTA:
                    switch(sec_enc) {
                            case HUFFMAN:
                                    enc_delta(HUFFMAN, in_fp);
                                    break;
                            case RUN_LENGTH:
                                    enc_delta(RUN_LENGTH, in_fp);
                    }
                    break;

     }


    enc_prepare_output_file(out_fp);

    fclose(in_fp);
    fclose(out_fp);
    free(input_file_header);
    return 0;
}

