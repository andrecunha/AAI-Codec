#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>
#include <bitio.h>

uint32_t dt_encode(int32_t *input, int32_t *output, uint32_t lenght){

    uint32_t i;

    output[0]=input[0];

    /*The basic delta encoding algorithm*/
    for(i=1; i< lenght; i++)
        output[i] = input[i]-input[i-1];


    /*Routine to get the largest difference between two consecutive samples in the audio file.*/
    int32_t max_dif = 0, dif;
    for(i=1; i < lenght; i++){
        dif = input[i] - input[i-1];
        if(dif > max_dif)
            max_dif = dif;
    }

    /*return ceil(log(max_dif)/log(2));*/
    return 0;

}


/*Decode function. It will receive a delta parameter, and lenght information of the file that it will decde.*/
void dt_decode(int32_t *input, int32_t *output,uint32_t lenght){
    uint32_t i;

    output[0] = input[0];

    for(i=1; i < lenght; i++)
        output[i] = output[i-1]+input[i];
}
