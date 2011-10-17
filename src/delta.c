#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>
#include <bitio.h>

uint32_t dt_encode(uint32_t *input, bitbuffer *out, uint32_t lenght, uint16_t bits_per_sample, uint32_t first)
{

    int32_t i;
    unsigned int maxbit;
    int32_t diff,res;
    int32_t max_diff = 0;
    int32_t min_diff = 0;
    int32_t *output = calloc(lenght,sizeof(int32_t));

    /*The basic delta encoding algorithm*/
    for(i=0; i< lenght; i++){
        if(i!=0)
            output[i] = input[i]-input[i-1];
        else output[i] = input[i]-first;
    }

    /*Routine to get the largest diffference between two consecutive samples in the audio file.*/
    for(i=1; i < lenght; i++){
        diff = input[i] - input[i-1];
        if(diff > max_diff)
            max_diff = diff;
        else if(diff < min_diff)
            min_diff = diff;
    }

    if(abs(max_diff)>abs(min_diff))
        res = max_diff;
        else res = min_diff;


    maxbit = ceil(log(abs(res))/log(2));

    for(i=0; i<lenght; i++){
        if(output[i] >= 0)
            bwritev(out, 1, 1);

        else
            bwritev(out, 0, 1);

        bwritev(out,abs(output[i]), maxbit);
    }

    free(output);
    return maxbit;
}

/*Decode function. It will receive a delta parameter, and length information of the file that it will decde.*/
void dt_decode_int(int32_t *input, uint32_t *output,uint32_t length, uint32_t first)
{
    uint32_t i;

    for(i=0; i < length; i++){
        if(i!=0)
            output[i] = output[i-1]+input[i];
        else output[0] = first+input[i];
    }
}


void dt_decode(bitbuffer *input, uint32_t maxbit, uint32_t *output, uint32_t length, uint16_t bits_per_sample, uint32_t first){
    uint32_t i, signal, diff;
    int32_t *input_int = malloc(sizeof(int32_t)*length);


    for (i=0; i<length; i++){
        breadv(input, &signal, 1);
        breadv(input, &diff, maxbit);
        if(signal == 1)
            input_int[i] = (int32_t) diff;
        else
            input_int[i] = (int32_t) -diff;

    }

    dt_decode_int(input_int, output, length, first);

}

