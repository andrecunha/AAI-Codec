#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>
#include <bitio.h>
#include <delta.h>


int main(int argc, char* argv[]){
    uint32_t coisa[10] = {100, 120, 150, 255, 290, 390, 99, 8, 55, 76};
    uint32_t coisa2[10];

    int i;

    bitbuffer *treco;
    treco = malloc(sizeof(bitbuffer));
    binit(treco, 1);

    uint32_t x = dt_encode(coisa,treco,10, 16, 0);

    bprint(treco);

    printf("Max. Res. %"PRId32"\n",x);

    dt_decode(treco, x, coisa2, 10, 16, 0);

    for(i=0; i<10; i++)
        printf("Treco descomprimido %"PRId32"\n", coisa2[i]);

    return 0;

}

