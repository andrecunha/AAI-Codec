#include <wavreader.h>
#include <stdio.h>

int main (int argc, char* argv[]){
    FILE *fp;
    wavheader *wh = malloc(sizeof(wavheader));
    
    if((fp = fopen("test3.wav", "rb"))==NULL) {
        printf("ERROR: fopen\n");
        return 1;
    }
    
    if(getHeader(wh, fp)==1) { 
        printf("ERROR: getHeader\n");
        fclose(fp);
        free(wh);
        return 1;
    }
    
    printWavHeader(wh);
    
    fclose(fp);
    free(wh);

    return 0;
}
