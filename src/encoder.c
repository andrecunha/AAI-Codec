#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <wavreader.h>
#include <error_handler.h>

#define HUFFMAN 1
#define DELTA 2
#define RUN_LENGTH 3

#define boolean int
#define TRUE 1
#define FALSE 0

void huffman(){
    printf("Huffman encoding...\n");
}

void run_length(){
    printf("Run-length encoding...\n");
}

void delta(){
    printf("Difference encoding...\n");
}

int main (int argc, char* argv[]){
    unsigned int n_encodings = 0, first_enc = 0, 
                    sec_enc = 0, third_enc = 0;
    boolean optimize = FALSE;
    FILE *fp_in, *fp_out;
    int get_opt;

    while((get_opt = getopt(argc, argv, "cdho"))!= -1){
        switch(get_opt){
            case 'o': optimize = TRUE; break;
            case 'd': 
                n_encodings++;
                if(first_enc == 0) first_enc = DELTA;
                else if(sec_enc == 0) sec_enc = DELTA;
                else if(third_enc == 0) third_enc = DELTA;
                else {
                    FORMAT_ERROR; return 1;
                }
                break;
            case 'h':
                n_encodings++;
                if(first_enc == 0) first_enc = HUFFMAN;
                else if(sec_enc == 0) sec_enc = HUFFMAN;
                else if(third_enc == 0) third_enc = HUFFMAN;
                else {
                    FORMAT_ERROR; return 1;
                }
                break;
            case 'c':
                n_encodings++;
                if(first_enc == 0) first_enc = RUN_LENGTH;
                else if(sec_enc == 0) sec_enc = RUN_LENGTH;
                else if(third_enc == 0) third_enc = RUN_LENGTH;
                else {
                    FORMAT_ERROR; return 1;
                }
                break;
            default: FORMAT_ERROR; return 1;
        }
    }
    
    if(n_encodings == 0){
        FORMAT_ERROR; return 1;
    }
    
    if((argv[optind] == NULL) || (argv[optind+1] == NULL)){
        FORMAT_ERROR; return 1;
    }

    printf("Input file: %s\nOutput file = %s\n", argv[optind], argv[optind+1]);
        
    if(optimize == TRUE) printf("Optimization on.\n");

    if((fp_in = fopen(argv[optind], "rb"))==NULL){
        IO_OPEN_ERROR;
        return 1;
    }
    
    if((fp_out = fopen(argv[optind+1], "wb+"))==NULL){
        IO_OPEN_ERROR;
        fclose(fp_in);
        return 1;
    }
    
    if(first_enc == HUFFMAN) huffman();
    else if(first_enc == DELTA) delta();
    else run_length();

    if(sec_enc == HUFFMAN) huffman();
    else if(sec_enc == DELTA) delta();
    else if(sec_enc == RUN_LENGTH) run_length();
    
    if(third_enc == HUFFMAN) huffman();
    else if(third_enc == DELTA) delta();
    else if(third_enc == RUN_LENGTH) run_length();
   
    fclose(fp_in);
    fclose(fp_out);

    return 0;
}

