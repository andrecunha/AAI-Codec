#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <bitio.h>
#include <run_length.h>
#include <error_handler.h>

#define H_NBITS_CODE 5
#define H_NBITS_RUN 5
#define H_NBITS_NCODES 10
#define H_NBITS_TOTAL H_NBITS_NCODES+H_NBITS_RUN+H_NBITS_CODE

void rl_encode_var(bitbuffer *input, bitbuffer *output,
        uint16_t bits_per_sample, uint32_t *nbits_run, uint32_t *nbits_code){
    bitbuffer *curr, *out, *new_input, *last;
    uint32_t i, run, code, runl, codel, li;
    uint32_t size = input->size, encoded = 0;
    unsigned long n_bytes = input->n_bytes;
    unsigned int bits_last = input->bits_last;
    unsigned int bits_offset = input->bits_offset;
    uint8_t leave = 0;
    uint32_t sizen;
    unsigned long n_bytesn;
    unsigned int bits_lastn;
    unsigned int bits_offsetn;
    uint32_t sizeo;
    unsigned long n_byteso;
    unsigned int bits_lasto;
    unsigned int bits_offseto;

    out = malloc(sizeof(bitbuffer));
    curr = malloc(sizeof(bitbuffer));
    new_input = malloc(sizeof(bitbuffer));
    last = malloc(sizeof(bitbuffer));
    
    binit(curr, 1);
    binit(out, 1);
    binit(new_input, 1);
    binit(last,1);

    while(encoded < size){
        breload(input, size, n_bytes, bits_last, bits_offset);
        input->data = input->original_data + encoded;
        
        bdestroy(new_input);
        binit(new_input, 1);

        bit_buffer_cpy(new_input, input, (size-encoded)*8); 


        sizen = new_input->size;
        n_bytesn = new_input->n_bytes;
        bits_lastn = new_input->bits_last;
        bits_offsetn = new_input->bits_offset; 
 
        i = 0;
        leave = 0;
        while((i<(size-encoded))&&!leave){
            i++;

            bdestroy(curr);
            binit(curr, 1);
            bit_buffer_cpy(curr, new_input, i*8);

            breload(new_input, sizen, n_bytesn, bits_lastn, bits_offsetn);
            
            bdestroy(out);
            binit(out, 1);

            rl_encode(curr, out, bits_per_sample, &run, &code);

            /*If NSAMPLES overflowed*/
            if((out->n_bytes*8-(8-out->bits_last)/(run+code) > 
                (pow(H_NBITS_NCODES,2)-1))){

/*                ERROR("Sample overflow");
                printf("i: %"PRIu32"\n", i);*/

                if(last->n_bytes == 0){
                    ERROR("NAO TEM LAST");
                    continue;
                }else{
                    /*if there is a last, write it*/
                    leave = 1;
                    encoded += li;
                    bwritev(output, codel, H_NBITS_CODE);
                    bwritev(output, runl, H_NBITS_RUN);
                    bwritev(output, (last->n_bytes*8-
                            (8-last->bits_last))/(codel+runl), 
                                        H_NBITS_NCODES);
/*                    printf("code: %"PRIu32"\trun: %"PRIu32
                        "\tsamples %"PRIu32"\n", 
                        codel, runl, 
                        (last->n_bytes*8-(8-last->bits_last))/(codel+runl) );
                    print_encoded(last, runl, codel);*/
                    bit_buffer_cpy(output, last, last->n_bytes*8-(8-last->bits_last));
                    continue;
                }
            }else{
                bdestroy(last);
                binit(last, 1);
                li = i;
                codel = code;
                runl = run;

                sizeo = out->size;
                n_byteso = out->n_bytes;
                bits_lasto = out->bits_last;
                bits_offseto = out->bits_offset; 

                bit_buffer_cpy(last, out, out->n_bytes*8-(8-out->bits_last));
                breload(out, sizeo, n_byteso, bits_lasto, bits_offseto );
            }

            if((H_NBITS_TOTAL+out->n_bytes*8-(8-out->bits_last)) <
                (i*8)){
                leave = 1;
                encoded += i;
                bwritev(output, code, H_NBITS_CODE);
                bwritev(output, run, H_NBITS_RUN);
                bwritev(output, (out->n_bytes*8-(8-out->bits_last))/(code+run), 
                                        H_NBITS_NCODES);
/*                printf("code: %"PRIu32"\trun: %"PRIu32"\tsamples %"PRIu32"\n", code, run, (out->n_bytes*8-(8-out->bits_last))/(code+run) );
                print_encoded(out, run, code);*/
                bit_buffer_cpy(output, out, out->n_bytes*8-(8-out->bits_last));
            }else if(i+1 >= size-encoded){
                encoded += i;
                bwritev(output, code, H_NBITS_CODE);
                bwritev(output, run, H_NBITS_RUN);
                bwritev(output, (out->n_bytes*8-(8-out->bits_last))/(code+run), 
                                        H_NBITS_NCODES);
/*                printf("code: %"PRIu32"\trun: %"PRIu32"\tsamples %"PRIu32"\n", code, run, (out->n_bytes*8-(8-out->bits_last))/(code+run) );
                print_encoded(out, run, code);*/
                bit_buffer_cpy(output, out, out->n_bytes*8-(8-out->bits_last));
            }
        }       
    }
    bdestroy(curr);
    bdestroy(out);
    bdestroy(new_input);
    bdestroy(last);
    free(last);
    free(curr);
    free(out);
    free(new_input);
}

void rl_encode(bitbuffer *input, bitbuffer *output,
        uint16_t bits_per_sample, uint32_t *nbits_run, uint32_t *nbits_code){
    uint32_t *to_encode, size_to_encode;
    uint32_t i = 0, run;
    bitbuffer *smaller, *curr;
    uint32_t size = input->size;
    unsigned long n_bytes = input->n_bytes;
    unsigned int bits_last = input->bits_last;
    unsigned int bits_offset = input->bits_offset;
    
    smaller = malloc(sizeof(bitbuffer));
    curr = malloc(sizeof(bitbuffer));
    binit(smaller, size);
    binit(curr, size);

    /*Increases smaller curr2 and decreases input pointers*/
    /*bit_buffer_cpy(curr2, input, input->n_bytes*8-(8-input->bits_last));*/


    for(i=1; i<=bits_per_sample; i++){
        if((n_bytes*8-(8-bits_last))%i)
            continue;
        /*Allocates memory to to_encode.*/
        /*Decreases input pointers.*/
        size_to_encode = b_to_uint32(input, &to_encode, i, 0);
        
        /*Input needs to be reloaded, since b_to_uint32 uses breadv,
 * which changes the pointers.*/
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
            binit(smaller, size);

            /*Increases smaller pointers and decreases curr pointers*/
            bit_buffer_cpy(smaller, curr, curr->n_bytes*8-(8-curr->bits_last));
            *nbits_run = run;
            *nbits_code = i;
            
        }
        /*Needs to be deallocated*/
        bdestroy(curr);
        binit(curr, size);
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
        printf("Code %"PRIu32" %"PRIu32" times--", code, run);
    }
    
    printf("\n\n");

    breload(input, sizet, n_bytes, bits_last, bits_offset);
}

uint32_t rl_encode_nbits(uint8_t nbits, uint32_t *input,
                    bitbuffer *output, uint32_t input_length){
    uint32_t i, count, longest_run;

    longest_run = find_longest_run(input, input_length);
    longest_run = ceil((float)log(longest_run)/log(2));
    if(longest_run>H_NBITS_RUN)
        longest_run = H_NBITS_RUN;
    
    
    if(longest_run == 0) longest_run = 1;
    

    for(i=0; i<input_length; i++){
        if(i!=0 && input[i] == input[i-1] && count<(pow(H_NBITS_RUN,2))-1){
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
    if((input_length==1)||((input[input_length-1] == input[input_length-2]) && curr_run > longest_run))
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

void rl_decode_var(uint16_t bits_per_sample, 
                    bitbuffer *input, bitbuffer *output){

    bitbuffer *new_input = malloc(sizeof(bitbuffer));
    uint32_t code, run, nsamples, i = 0;
    
    binit(new_input, 1);
    uint32_t size = input->n_bytes*8-(8-input->bits_last)-input->bits_offset; 

    while(i<size){
        bdestroy(new_input);
        binit(new_input, 1);        

        breadv(input, &code, H_NBITS_CODE);
        breadv(input, &run, H_NBITS_RUN);
        breadv(input, &nsamples, H_NBITS_NCODES);
        i += H_NBITS_TOTAL;
        if(code == 0 || run == 0 || nsamples==0) {
            if(code ==0)
                ERROR("Code = 0");
            if(run ==0)
                ERROR("Run = 0");
            if(nsamples ==0)
                ERROR("Nsamples = 0");
            break; 
        } 
        i+=nsamples*(code+run);
   
        bit_buffer_cpy(new_input, input, nsamples*(code+run));
            
/*        print_encoded(new_input, run, code);*/

        rl_decode(bits_per_sample, code, run, new_input, output);
    }
    
    bdestroy(new_input);
    free(new_input);
}
