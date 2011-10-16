#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>
#include <bitio.h>

uint32_t dt_encode(int32_t *input, int32_t *output, uint32_t lenght)
{

    int32_t i;
    int32_t diff,res;
    int32_t max_diff = 0;
    int32_t min_diff = 0;

    output[0]=input[0];

    /*The basic delta encoding algorithm*/
    for(i=1; i< lenght; i++)
        output[i] = input[i]-input[i-1];

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

    return ceil(log(abs(res))/log(2));
}


/*Decode function. It will receive a delta parameter, and lenght information of the file that it will decde.*/
void dt_decode(int32_t *input, int32_t *output,uint32_t lenght)
{
    uint32_t i;

    output[0] = input[0];

    for(i=1; i < lenght; i++)
        output[i] = output[i-1]+input[i];
}
