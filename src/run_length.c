#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <bitio.h>
#include <run_length.h>

#define H_NBITS_CODE 6
#define H_NBITS_RUN 5
#define H_NBITS_NCODES 5
#define H_NBITS_TOTAL 16

void rl_encode(bitbuffer *input, bitbuffer *output, 
        uint16_t bits_per_sample, uint32_t *nbits_run, uint32_t *nbits_code){
    uint32_t *to_encode, size_to_encode;
    uint32_t i = 0, run;
    bitbuffer *smaller, *curr, *curr2;
    uint32_t size = input->size;
    unsigned long n_bytes = input->n_bytes;
    unsigned int bits_last = input->bits_last;
    unsigned int bits_offset = input->bits_offset;
    
    smaller = malloc(sizeof(bitbuffer));
    curr = malloc(sizeof(bitbuffer));
    curr2 = malloc(sizeof(bitbuffer));
    binit(smaller, 1);
    binit(curr, 1);    
    binit(curr2, 1);    

    /*Increases smaller curr2 and decreases input pointers*/
    /*bit_buffer_cpy(curr2, input, input->n_bytes*8-(8-input->bits_last));*/


    for(i=1; i<=bits_per_sample; i++){
        /*Allocates memory to to_encode.*/
        /*Decreases input pointers.*/
        size_to_encode = b_to_uint32(input, &to_encode, i);
        
        /*Input needs to be reloaded, since b_to_uint32 uses breadv,
            which changes the pointers.*/
        breload(input, size, n_bytes, bits_last, bits_offset);
        
        /*Increases curr pointers.*/
        run = rl_encode_nbits(i, to_encode, curr, size_to_encode);
        /*Needs do be deallocated.*/
        free(to_encode);
        
        if(i==1){
            bit_buffer_cpy(smaller, curr, curr->n_bytes*8-(8-curr->bits_last));
            *nbits_run = run;
            *nbits_code = i;
        }else if((curr->n_bytes*8-(8-curr->bits_last))<
                (smaller->n_bytes*8-(8-smaller->bits_last))){
            /*Deallocates last smaller*/
            bdestroy(smaller);
            binit(smaller, 1);

            /*Increases smaller pointers and decreases curr pointers*/
            bit_buffer_cpy(smaller, curr, curr->n_bytes*8-(8-curr->bits_last));
            *nbits_run = run;
            *nbits_code = i;
            
        }
        /*Needs to be deallocated*/
        bdestroy(curr);
        binit(curr, 1);
    }
    bit_buffer_cpy(output, smaller, smaller->n_bytes*8-(8-smaller->bits_last));
    bdestroy(smaller);
    bdestroy(curr);
    free(smaller);
    free(curr);
}

void print_encoded(bitbuffer *input, uint32_t nbits_run, 
            uint32_t nbits_code){
    uint32_t code, run, i, size;
    size = input->n_bytes*8-(8-input->bits_last);
    uint32_t sizet = input->size;
    unsigned long n_bytes = input->n_bytes;
    unsigned int bits_last = input->bits_last;
    unsigned int bits_offset = input->bits_offset;
    

    for(i=0; i< size; i+=(nbits_run+nbits_code)){
        breadv(input, &code, nbits_code);
        breadv(input, &run, nbits_run);
        printf("Encoded: code: %"PRIu32"\ttimes: %"PRIu32"\n", code, run);
    }

    breload(input, sizet, n_bytes, bits_last, bits_offset);
}

uint32_t rl_encode_nbits(uint8_t nbits, uint32_t *input, 
                    bitbuffer *output, uint32_t input_length){
    uint32_t i, count, longest_run;

    longest_run = find_longest_run(input, input_length);
    longest_run = ceil((float)log(longest_run)/log(2));
    
    
    if(longest_run == 0) longest_run = 1;
    

    for(i=0; i<input_length; i++){   
        if(i!=0 && input[i] == input[i-1]){
            count++;
        }else {
            if(i!=0){
                bwritev(output, input[i-1], nbits);
                bwritev(output, count, longest_run);
            }
            count = 1;
        }
    }
    if(input_length>1){
        if(input[input_length-1]!=input[input_length-2]) count = 1;
    }

    bwritev(output, input[input_length-1], nbits);
    bwritev(output, count, longest_run); 
    
    return longest_run;
}

uint32_t find_longest_run(uint32_t *input, uint32_t input_length){
    uint32_t longest_run = 1, curr_run = 1, i;

    for (i=0; i<input_length; i++){
        if((i!=0) && (input[i]==input[i-1])){
            curr_run++;
        }else{
            if((i!=0) && input[i]!=input[i-1]){
                if(curr_run>longest_run)
                    longest_run = curr_run;
            }
            curr_run = 1;
        }
    }
    if((input[input_length-1] == input[input_length-2]) && curr_run > longest_run)
        longest_run = curr_run;
    return longest_run; 
}

void rl_decode(uint16_t bits_per_sample, uint32_t nbits_code, 
        uint32_t nbits_run, bitbuffer *input, bitbuffer *output){
         uint32_t size = input->n_bytes*8-(8-input->bits_last);

   uint32_t code, run, i, j;

    for(j=0;j<size; j+=(nbits_run+nbits_code)){
        code = 0;
        run = 0;
        breadv(input, &code, nbits_code);
        breadv(input, &run, nbits_run);
        for (i=0; i<run; i++){
            bwritev(output, code, nbits_code);
        }
    }
}
