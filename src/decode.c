#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <binreader.h>
#include <error_handler.h>
#include <huffman.h>
#include <delta.h>
#include <run_length.h>

/* Global variables, a necessary evil. */
unsigned int n_encodings = 0,
             first_enc = 0, 
             sec_enc = 0,
             third_enc = 0;

/* The data divided in channels. */
uint32_t **data_vector = NULL,
         **output_vector = NULL;

bitbuffer *data_buffer = NULL,
          *output_buffer = NULL;

/* The header of the file being encoded. */
binheader *input_file_header;

/* The frequencies vector, generated by the Huffman encoding.  */
uint64_t **frequencies;

/* Specific parameters of each encoding. One value per channel. */
uint32_t *nbits_run = NULL,
         *firsts = NULL,
         *max_bits = NULL,
         *nbits_code = NULL,
         frequency_length = 0;

int curr_channel;

void dec_prepare_input_file(FILE *fp)
{
    printf("Preparing input file...\n");

    input_file_header = malloc(sizeof(binheader));

    printf("Getting header...\n");

    binh_get_header(input_file_header, fp, &frequencies, &frequency_length, &firsts, &max_bits, &nbits_run, &nbits_code);
    first_enc = input_file_header->firstEncoding;
    sec_enc = input_file_header->secondEncoding;
    third_enc = input_file_header->thirdEncoding;

    output_buffer = calloc(input_file_header->numChannels, sizeof(bitbuffer));

    output_vector = calloc(input_file_header->numChannels, sizeof(uint32_t *));

    /* TODO: is this right???   
        memset(output_vector, 0, input_file_header->numChannels*sizeof(uint32_t *));
     */

    data_buffer = calloc(input_file_header->numChannels, sizeof(bitbuffer));

    data_vector = calloc(input_file_header->numChannels, sizeof(uint32_t *));

    for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++){
           printf("Reading data buffer");
           if( bget(&(output_buffer[curr_channel]), fp)){
                ERROR("Couldn't read bit buffer from input file");
            }
     }
}

void dec_prepare_output_file (FILE *fp)
{
     if(putHeader(input_file_header, fp)){
        ERROR("putHeader");
     }
   
    uint32_t k = 0, **vector = NULL, *size_vector;

    vector = calloc(input_file_header->numChannels, sizeof(uint32_t *));
    size_vector = calloc(input_file_header->numChannels, sizeof(uint32_t));

    for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
        size_vector[curr_channel] = b_to_uint32(&output_buffer[curr_channel], &vector[curr_channel], input_file_header->bitsPerSample, 0);
    }

    for (k=0; k<size_vector[0]; k++){
        for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
            switch (input_file_header->bitsPerSample/8){
            case 1:
            case 2:
                if (write_2_bytes32(fp, &vector[curr_channel][k], input_file_header->endianness)) {
                    printf("ERROR: io chunkSize\n");
                }break;   
            case 3:
                if (write_3_bytes(fp, &vector[curr_channel][k], input_file_header->endianness)) {
                    printf("ERROR: io chunkSize\n");
                }break;   
            case 4:
                if (write_4_bytes(fp, &vector[curr_channel][k], input_file_header->endianness)) {
                    printf("ERROR: io chunkSize\n");
                }   
            }   
/*                printf("k: %"PRIu32" curr: %d\n", k, curr_channel);*/
        }
    }

/*    bprint(&output_buffer[0]);
    bprint(&output_buffer[1]);*/
    


    for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
        
        /*bprint(&output_buffer[curr_channel]);*/
        if(output_vector[curr_channel])
                free(output_vector[curr_channel]);
        if(data_vector[curr_channel])
                free(data_vector[curr_channel]);
        bdestroy(&output_buffer[curr_channel]);
        bdestroy(&data_buffer[curr_channel]);
        if(frequencies && frequencies[curr_channel])
            free(frequencies[curr_channel]);
    }
    if(nbits_run)
        free(nbits_run);
    if(nbits_code)
        free(nbits_code);
    if(firsts)
        free(firsts);
    if(max_bits)
        free(max_bits);
    if(frequencies)
        free(frequencies);
    free(data_buffer);
    free(data_vector);
    free(output_buffer);
    free(output_vector);
    free(input_file_header);
}

void dec_huffman(FILE *in_fp)
{
    printf("Applying Huffman decoding...\n");

    /* First, we need to compute the length of the output vector. */
    uint32_t output_length=0, i;
    for(i=0; i<frequency_length; i++)
            output_length+=frequencies[0][i];

    for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
        if(output_vector[curr_channel])
                free(output_vector[curr_channel]);

        output_vector[curr_channel] = calloc(output_length, sizeof(uint32_t));
       
        hf_decode (&(output_buffer[curr_channel]), frequencies[curr_channel], output_vector[curr_channel], frequency_length);
       
        int prev_enc;
        if(sec_enc == HUFFMAN)
                prev_enc = first_enc == DELTA ? DELTA : RUN_LENGTH;
        else if (third_enc == HUFFMAN)
                prev_enc = sec_enc == DELTA ? DELTA : RUN_LENGTH;

        uint32_t nbits_block;

        if (first_enc == HUFFMAN)
                nbits_block = input_file_header -> bitsPerSample;
        else 
                nbits_block = (prev_enc == RUN_LENGTH) ? (nbits_code[curr_channel] + nbits_run[curr_channel]) : (max_bits[curr_channel]+1);

        bdestroy(&output_buffer[curr_channel]);
        binit(&output_buffer[curr_channel], input_file_header->subchunk2size);
        b_from_uint32(&output_buffer[curr_channel], output_vector[curr_channel], output_length, nbits_block,  0);

        bprint(&output_buffer[curr_channel]);
    }
}

void dec_run_length(FILE *in_fp)
{
    printf("Applying run-length decoding...\n");

    for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
        if(sec_enc==RUN_LENGTH){ 
            rl_decode(max_bits[curr_channel]+1, nbits_code[curr_channel], nbits_run[curr_channel], &data_buffer[curr_channel], &output_buffer[curr_channel]);

            /*bprint(&output_buffer[curr_channel]);*/

            bdestroy(&data_buffer[curr_channel]);

            binit(&data_buffer[curr_channel], output_buffer[curr_channel].n_bytes*8-(8-output_buffer[curr_channel].bits_last));

            bit_buffer_cpy(&data_buffer[curr_channel], &output_buffer[curr_channel], output_buffer[curr_channel].n_bytes*8-(8-output_buffer[curr_channel].bits_last));
         /*   bprint(&data_buffer[curr_channel]);*/
        }else{
            if(!sec_enc){
                binit(&output_buffer[curr_channel], input_file_header->subchunk2size);  
            }else{
                if(data_buffer){
                    bdestroy(&data_buffer[curr_channel]);

                    binit(&data_buffer[curr_channel], output_buffer[curr_channel].n_bytes*8-(8-output_buffer[curr_channel].bits_last));

                    bit_buffer_cpy(&data_buffer[curr_channel], &output_buffer[curr_channel], output_buffer[curr_channel].n_bytes*8-(8-output_buffer[curr_channel].bits_last));
                }
                bdestroy(&output_buffer[curr_channel]);
                binit(&output_buffer[curr_channel], input_file_header->subchunk2size);
            }
            rl_decode(input_file_header->bitsPerSample, nbits_code[curr_channel], nbits_run[curr_channel], &data_buffer[curr_channel], &output_buffer[curr_channel]);
        }
      /* bprint(&output_buffer[curr_channel]);*/
    }
}

void dec_delta(FILE *in_fp)
{
    printf("Delta decoding...\n");
    for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
        if(output_vector[curr_channel])
            free(output_vector[curr_channel]);
       
        uint32_t output_length;

        if(first_enc==DELTA) 
            output_length = ((output_buffer[curr_channel].n_bytes*8 - (8-output_buffer[curr_channel].bits_last))/(max_bits[curr_channel]+1)) + 1;
        else
            output_length = ((output_buffer[curr_channel].n_bytes*8 - (8-output_buffer[curr_channel].bits_last))/(nbits_run[curr_channel]+nbits_code[curr_channel])) + 1;

        output_vector[curr_channel] = calloc(output_length, sizeof(uint32_t));

        memset(output_vector[curr_channel], 0, output_length);

        if(first_enc==DELTA)
            dt_decode(&output_buffer[curr_channel], max_bits[curr_channel], output_vector[curr_channel], output_length, input_file_header->bitsPerSample, firsts[curr_channel]);
        else
            dt_decode(&output_buffer[curr_channel], max_bits[curr_channel], output_vector[curr_channel], output_length, nbits_run[curr_channel] + nbits_code[curr_channel], firsts[curr_channel]);

        bdestroy(&output_buffer[curr_channel]);
        binit(&output_buffer[curr_channel], output_length);

        if(first_enc==DELTA)
            b_from_uint32(&output_buffer[curr_channel], output_vector[curr_channel], output_length, input_file_header->bitsPerSample, 0);
        else
            b_from_uint32(&output_buffer[curr_channel], output_vector[curr_channel], output_length, nbits_code[curr_channel]+nbits_run[curr_channel], 0);

        bprint(&output_buffer[curr_channel]);
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

    dec_prepare_input_file(in_fp);

    switch (third_enc){
            case HUFFMAN:
                    dec_huffman(in_fp);
                    break;
            case RUN_LENGTH:
                    dec_run_length(in_fp);
                    break;
            case DELTA: 
                    dec_delta(in_fp);
            default:
                    break;
    }

    switch(sec_enc){
            case HUFFMAN:
                    dec_huffman(in_fp);
                    break;
            case RUN_LENGTH:
                    dec_run_length(in_fp);
                    break;
            case DELTA: 
                    dec_delta(in_fp);
            default:
                    break;
    }

    switch(first_enc){
            case HUFFMAN:
                    {
                    dec_huffman(in_fp);
                    }
                    break;
            case RUN_LENGTH:
                   dec_run_length(in_fp);
                    break;
            case DELTA: 
                    dec_delta(in_fp);
            default:
                    break;
    }

    dec_prepare_output_file(out_fp);

    fclose(in_fp);
    fclose(out_fp);
    return 0;
}
