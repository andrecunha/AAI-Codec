#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>
#include <bitio.h>

uint32_t dt_encode(uint32_t *input, uint32_t *output, uint32_t lenght){

    uint32_t delta = 0; /*The original delta is equal to zero.*/
    uint32_t original = input[0];
    uint32_t i;
    uint32_t max,min;


    max = input[0];
    for(i = 0; i <= lenght; i++){
    min = input[i];
    if (max < min)
    max = min; /* Get the bigger value among the samples in the audio file.*/
    }

    output[0]=input[0];

    /*The basic delta encoding algorithm*/
    for(i=1; i<= lenght; i++)
        output[i] = input[i]-input[i-1];


    /*Routine to get the largest difference between two consecutive samples in the audio file.*/
    uint32_t max_dif = 0, dif;
    for(i=0; i <= lenght; i++){
        dif = input[i] - input[i-1];
        if(dif > max_dif)
            max_dif = dif;
    }

    return ceil(log(max_dif)/log(2));

}


/*Decode function. It will receive a delta parameter, and lenght information of the file that it will decde.*/
void dt_decode(uint32_t *input, uint32_t *output,uint32_t lenght){
    uint32_t i;
    uint32_t delta = 0;

    output[0] = input[0];

    for(i=1; i <= lenght; i++)
        output[i] = input[i-1]+input[i];
}
