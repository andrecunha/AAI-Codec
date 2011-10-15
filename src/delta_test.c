#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>
#include <bitio.h>
#include <delta.h>


int main(int argc, char* argv[]){
    int32_t coisa[10] = {100, 120, 150, 255, 290, 390, 99, 8, 55, 76};
    int32_t treco[10], coisa2[10];

    int i;

    dt_encode(coisa,treco,10);

    for(i=0; i< 10; i++)
    printf("Treco %"PRId32"\n", treco[i]);

    dt_decode(treco, coisa2, 10);
    for(i=0; i<10; i++)
    printf("Treco descomprimido %"PRId32"\n", coisa2[i]);

    return 0;

}
