#include <stdlib.h>
#include <bitio.h>
#include <inttypes.h>
#include <error_handler.h>
#include <wavreader.h>
#include <run_length.h>

int main(int argc, char* argv[]){ 
    bitbuffer *b = malloc(sizeof(bitbuffer));
    bitbuffer *bout = malloc(sizeof(bitbuffer));
    bitbuffer *bout2 = malloc(sizeof(bitbuffer));
    uint32_t bits_run = 0;
    uint32_t  bits_code = 0;
    FILE *fp;
    uint8_t c, i;
    wavheader *wh = malloc(sizeof(wavheader));
    uint32_t reads = 0;
    
    if((fp = fopen(argv[1], "rb"))==NULL) {
        IO_OPEN_ERROR;
        return 1;
    }
     
    if(getHeader(wh, fp)==1) { 
        ERROR("getHeader");
        fclose(fp);
        free(wh);
        return 1;
    }
    
    printWavHeader(wh);

    binit(b, 1);
    binit(bout, 1);
    binit(bout2, 1);
/*    for(i=0;i<72;i++)
    fread(&c, 1, 1, fp);*/

    do{
        if(fread(&c, sizeof(uint8_t), 1, fp)!=1){
            IO_READ_ERROR;
            printf("Aqui");
     /*       fclose(fp);
            free(wh);
            bdestroy(b);
            bdestroy(bout);
            bdestroy(bout2);
            free(bout);
            free(bout2);
            free(b);
            return 1;*/
            c=EOF;
        }
        reads+=8;
        if(c!=EOF){
            bwritev(b, (uint32_t) c, 8);

            for(i=1; i < wh->bitsPerSample/8; i++){
                if(fread(&c, sizeof(uint8_t), 1, fp)!=1){
                    IO_READ_ERROR;
                    fclose(fp);
                    free(wh);
                    bdestroy(b);
                    bdestroy(bout);
                    bdestroy(bout2);
                    free(bout);
                    free(bout2);
                    free(b);
                    return 1;
                }
                bwritev(b, (uint32_t) c, 8);
                reads+=8;
           }
        }
    }while((c!=EOF)&&(reads<wh->bitsPerSample*80));


    rl_encode(b, bout, wh->bitsPerSample, &bits_run, &bits_code);

    bprint(b);
    bprint(bout);

    printf("bits run: %"PRIu32"\n", bits_run);
    printf("bits code: %"PRIu32"\n", bits_code);
 
    rl_decode(wh->bitsPerSample, bits_code, bits_run, bout, bout2);
    bprint(bout2);
    
    fclose(fp);
    free(wh);
    
    bdestroy(b);
    bdestroy(bout);
    bdestroy(bout2);
    free(bout);
    free(bout2);
    free(b);
    return 0;
}
