#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <bitio.h>
#include <wavreader.h>
#include <binreader.h>
#include <error_handler.h>
#include <huffman.h>
#include <delta.h>
#include <run_length.h>

typedef int boolean;
#define TRUE 1
#define FALSE 0

/* Global variables, a necessary evil. */
unsigned int n_encodings = 0,
             first_enc = 0, 
             sec_enc = 0,
             third_enc = 0;

/* The data divided in channels. */
uint32_t **data_vector = NULL;

bitbuffer *data_buffer = NULL,
          *output_buffer = NULL;

/* The header of the file being encoded. */
wavheader *input_file_header;

/* The frequencies vector, generated by the Huffman encoding.  */
uint64_t **frequencies;

/* Specific parameters of each encoding. One value per channel. */
uint32_t *nbits_run = NULL,
         *firsts = NULL,
         *max_bits = NULL,
         *nbits_code = NULL,
         frequency_length;
uint8_t flag = 0;

void enc_prepare_input_file(FILE *fp)
{
        input_file_header = malloc(sizeof(wavheader));
        if (getHeader(input_file_header, fp)) {
                ERROR("Error while reading header");
                exit(1);
        }
        printWavHeader(input_file_header);
        output_buffer = calloc(input_file_header->numChannels, sizeof(bitbuffer));
        nbits_run = calloc(input_file_header->numChannels, sizeof(uint32_t));
        nbits_code = calloc(input_file_header->numChannels, sizeof(uint32_t)); 
        max_bits = calloc(input_file_header->numChannels, sizeof(uint32_t));
        firsts = calloc(input_file_header->numChannels, sizeof(uint32_t));
        frequencies = calloc(input_file_header->numChannels, sizeof(uint64_t *));
        data_vector = calloc(input_file_header->numChannels, sizeof(uint32_t *));
}

void enc_prepare_output_file (FILE *fp)
{
        int curr_channel;

        /* Writing the header. */
        binheader h;

        binh_make_bin_header(&h, input_file_header, first_enc, sec_enc,third_enc);

        binh_put_header(&h, fp, frequencies, frequency_length, firsts, max_bits, nbits_run, nbits_code);
        /*if(fwrite("BUFFER", sizeof("BUFFER"), 1, fp)!=1)
            ERROR("BUFFER");*/

        for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++){
                bflush(&output_buffer[curr_channel], fp);
                if((first_enc==HUFFMAN)||(sec_enc==HUFFMAN)||(third_enc==HUFFMAN))
                    free(frequencies[curr_channel]);
        }

        /*TODO: Verificar essas desalocações malucas.*/
        for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
                if(data_buffer)
                    bdestroy(&data_buffer[curr_channel]);
                bdestroy(&output_buffer[curr_channel]);
                if(data_vector && data_vector[curr_channel])
                {
                        if(flag){
                            free(data_vector[curr_channel]);
                        }else if (((n_encodings==1)&&(first_enc == DELTA)) || ((n_encodings==2)&&(sec_enc == DELTA)) || ((n_encodings==3)&&(third_enc == DELTA))){
                                free(--data_vector[curr_channel]);
                        }
                        else if (((n_encodings==1)&&(first_enc == HUFFMAN)) || ((n_encodings==2)&&(sec_enc == HUFFMAN)) || ((n_encodings==3)&&(third_enc == HUFFMAN))){
                           free(data_vector[curr_channel]);
                        }
                }
        }
        
        free(data_vector);
        if (data_buffer)
            free(data_buffer);
        free(output_buffer);
        free(firsts);
        free(max_bits);
        free(nbits_code);
        free(nbits_run);
        free(frequencies);
        free(input_file_header);
}

void enc_huffman(int previous, FILE *in_fp)
{
        printf("Applying Huffman encoding...\n");

        /* Number of samples per channel. */
        uint32_t channel_n_samples = (input_file_header->subchunk2size)/(input_file_header->numChannels)/((input_file_header->bitsPerSample)/8);
        int curr_channel=0;
        switch (previous) {
                case (-1):
                        {       /* Huffman is the first encoding option. */

                                /* Loads the file into data_vector. */
                                free(data_vector);
                                load_to_uint32(in_fp, input_file_header, &data_vector);


                                for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
                                        /* Initialize the bitbuffer of the current channel. */
                                        binit(&output_buffer[curr_channel], input_file_header->subchunk2size/input_file_header->numChannels);

                                        frequency_length = pow(2,input_file_header->bitsPerSample);
                                        
                                        hf_encode(data_vector[curr_channel], &(output_buffer[curr_channel]), &(frequencies[curr_channel]), channel_n_samples, pow(2,input_file_header->bitsPerSample));
                                }
                        }
                        break;

                case RUN_LENGTH:
                        {
                                for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
                                        /* The size, in bits and blocks, of the run-length encoding output. */
                                        uint32_t output_size_bits = (output_buffer[curr_channel].n_bytes*8 - (8-output_buffer[curr_channel].bits_last)),
                                                 output_size_blocks = output_size_bits/(nbits_run[curr_channel] + nbits_code[curr_channel]);

                                        /* Converts the output bitbuffer from run-length to data_vector. */
                                        b_to_uint32(&(output_buffer[curr_channel]), &(data_vector[curr_channel]), nbits_run[curr_channel] + nbits_code[curr_channel], 0);
                                        bdestroy(&(output_buffer[curr_channel]));
                                        binit(&output_buffer[curr_channel], input_file_header->subchunk2size/input_file_header->numChannels);

                                        frequency_length = pow(2,nbits_run[curr_channel] + nbits_code[curr_channel]);

                                        hf_encode(data_vector[curr_channel], &(output_buffer[curr_channel]), &(frequencies[curr_channel]), output_size_blocks , pow(2,nbits_run[curr_channel] + nbits_code[curr_channel]));
                                }
                        }
                        break;

                case DELTA:
                        {
                                /*XXX: Torce para não dar problema aqui!!!*/

                                for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
                                                if(!flag) 
                                                        free(--(data_vector[curr_channel]));
                                                else free(data_vector[curr_channel]);
                                }

                                for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
                                        /* We encode the output of the delta encoding taking blocks of max_bits bits. */
                                        uint32_t block_size = ((output_buffer[curr_channel].n_bytes*8 - (8-output_buffer[curr_channel].bits_last))/(max_bits[curr_channel]));
                                        b_to_uint32(&(output_buffer[curr_channel]), &(data_vector[curr_channel]), max_bits[curr_channel], 0);
                                        bdestroy(&(output_buffer[curr_channel]));
                                        binit(&output_buffer[curr_channel], input_file_header->subchunk2size/input_file_header->numChannels);
                                        frequency_length = pow(2, max_bits[curr_channel]);
                                        hf_encode(data_vector[curr_channel], &(output_buffer[curr_channel]), &(frequencies[curr_channel]), block_size, pow(2, max_bits[curr_channel]));

                                }
                        break;
        }
        }

        /*XXX: Está certa a brincadeira!!!*/
}

void enc_run_length(int previous, FILE *in_fp)
{
    printf("Applying run-length encoding...\n");

    int curr_channel=0;
    switch (previous) {
            case -1:
                    {       /* Run-length is the first encoding. */

                            load_to_bitbuffer(in_fp, input_file_header, &data_buffer);

                            for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
                                    binit(&(output_buffer[curr_channel]), input_file_header->subchunk2size/input_file_header->numChannels);
                                    rl_encode(&(data_buffer[curr_channel]), &(output_buffer[curr_channel]), input_file_header->bitsPerSample, &(nbits_run[curr_channel]), &(nbits_code[curr_channel]));
                            }
                    }
                    break;

            case HUFFMAN:
                    {
                            ERROR("Unsupported ordering. Cannot apply any compression after Huffman.");
                            exit(EXIT_FAILURE);
                    }
                    break;

            case DELTA:
                    {
                            for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
                                            if(!flag)
                                                free(--(data_vector[curr_channel]));
                                            else
                                                free(data_vector[curr_channel]);
                            }

                            data_buffer = calloc(input_file_header->numChannels, sizeof(bitbuffer));

                            for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
                                binit(&(data_buffer[curr_channel]), input_file_header->subchunk2size/input_file_header->numChannels);
                                bit_buffer_cpy(&(data_buffer[curr_channel]), &(output_buffer[curr_channel]), output_buffer[curr_channel].n_bytes*8 - (8 - output_buffer[curr_channel].bits_last));
                                bdestroy(&(output_buffer[curr_channel]));
                                binit(&(output_buffer[curr_channel]), input_file_header->subchunk2size/input_file_header->numChannels);
                                rl_encode(&(data_buffer[curr_channel]), &(output_buffer[curr_channel]), max_bits[curr_channel], &(nbits_run[curr_channel]), &(nbits_code[curr_channel]));
                            }

                    }
                    break;
    }
}

void enc_delta(int previous, FILE *in_fp)
{

        int curr_channel;
        uint32_t channel_n_samples = (input_file_header->subchunk2size)/(input_file_header->numChannels)/((input_file_header->bitsPerSample)/8);
        printf("Delta encoding...\n");
        switch (previous) {
            case -1:
                    {       /* Delta encoding is the first tecnique applied. */
                            free(data_vector);
                            load_to_uint32(in_fp, input_file_header, &data_vector);

                            for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
                                    firsts[curr_channel] = data_vector[curr_channel][0];
                                    /*data_vector[curr_channel] = &(data_vector[curr_channel][1]);*/
                                    data_vector[curr_channel]++;
                                    binit(&(output_buffer[curr_channel]), input_file_header->subchunk2size/input_file_header->numChannels);
                                    max_bits[curr_channel] = dt_encode(data_vector[curr_channel], &(output_buffer[curr_channel]), channel_n_samples-1, input_file_header->bitsPerSample, firsts[curr_channel]);
                            }
                    }
                    break;

            case RUN_LENGTH:
                    {
                            /*memset(firsts, 0, input_file_header->numChannels*sizeof(uint32_t));
                            flag = 1;*/

                            for(curr_channel=0; curr_channel<input_file_header->numChannels; curr_channel++) {
                                uint32_t size = b_to_uint32(&(output_buffer[curr_channel]), &(data_vector[curr_channel]), nbits_code[curr_channel] + nbits_run[curr_channel], 0);
                                bdestroy(&(output_buffer[curr_channel]));
                                firsts[curr_channel] = data_vector[curr_channel][0];
                                data_vector[curr_channel]++;
                                binit(&(output_buffer[curr_channel]), input_file_header->subchunk2size/input_file_header->numChannels);
                                max_bits[curr_channel] = dt_encode(data_vector[curr_channel], &(output_buffer[curr_channel]), size-1, nbits_run[curr_channel] + nbits_code[curr_channel], firsts[curr_channel]);
                            }

                    }
                    break;

            case HUFFMAN:
                    {
                            ERROR("Unsupported ordering. Cannot apply any compression after Huffman.");
                            exit(1);
                    }
                    break;
        }

}

int main (int argc, char* argv[])
{

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
    return 0;
}

