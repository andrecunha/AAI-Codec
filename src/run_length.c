#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <bitio.h>
#include <run_length.h>

void rl_encode(bitbuffer *input, bitbuffer *output){
    uint32_t *to_encode, size_to_encode;
    uint8_t i = 0;

    /* TODO
     * for(i=0 i<bitsPerSample; i++)
     */
    /*size_to_encode = b_to_uint32(input, &to_encode, i);
    rl_encode_nbits(i, to_encode, output, size_to_encode);*/
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

    if(input[input_length-1]!=input[input_length-2]) count = 1;
    
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
            if(input[i]!=input[i-1]){
                if(curr_run>longest_run)
                    longest_run = curr_run;
            }
            curr_run = 1;
        }
    }
    return longest_run; 
}

void rl_decode(uint16_t bits_per_sample, uint32_t nbits_code, uint32_t nbits_run, 
            bitbuffer *input, bitbuffer *output){
    uint32_t code, run, i;

    while(!bempty(input)){
        breadv(input, &code, nbits_code);
        breadv(input, &run, nbits_run);
        for (i=0; i<run; i++)
            bwritev(output, code, bits_per_sample);
    }
}
