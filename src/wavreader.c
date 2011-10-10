#include <wavreader.h>
#include <string.h>


int getHeader(wavheader *wh, FILE *f){
    char riff[5];
    unsigned long chunkSize;
    char wave[5];
    char fmt[5];
    unsigned long subchunk1size;
    unsigned short audioFormat;
    unsigned short numChannels;
    unsigned long sampleRate;
    unsigned long byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    char data[5] = "data\0";
    unsigned long subchunk2size;
    char c;
    short int exit = 0;

    if ((fread(riff,4, 1, f))!=1) {
        printf("ERROR: io riff\n");
        return 1;
    }else {
        riff[4] = '\0';
        if (strcmp(riff, "RIFF\0")) {
            printf("ERROR: riff\n");
            return 1;
        }
    }
    if ((fread(&chunkSize, 4, 1, f))!=1) {
        printf("ERROR: io chunkSize\n");
        return 1;
    }   
    if ((fread(wave, 4, 1, f))!=1) {
        printf("ERROR: io wave\n");
        return 1;
    }else {
        wave[4] = '\0';
        if (strcmp(wave, "WAVE\0")) {
            printf("ERROR: wave\n");
            return 1;
        }
    }
    if ((fread(fmt, 4, 1, f))!=1) {
        printf("ERROR: io fmt\n");
        return 1;
    }else {
        fmt[4] = '\0';
        if (strcmp(fmt, "fmt \0")) {
            printf("ERROR: fmt\n");
            return 1;
        }
    }
    if ((fread(&subchunk1size, 4, 1, f))!=1) {
        printf("ERROR: subchunk1size\n");
        return 1;
    }
    if ((fread(&audioFormat, 2, 1, f))!=1) {
        printf("ERROR: audioFormat\n");
        return 1;
    }
    if ((fread(&numChannels, 2, 1, f))!=1) {
        printf("ERROR: numChannels\n");
        return 1;
    }
    if ((fread(&sampleRate, 4, 1, f))!=1) {
        printf("ERROR: sampleRate\n");
        return 1;
    }
    if ((fread(&byteRate, 4, 1, f))!=1) {
        printf("ERROR: byteRate\n");
        return 1;
    }
    if ((fread(&blockAlign, 2, 1, f))!=1) {
        printf("ERROR: blockAlign\n");
        return 1;
    }
    if ((fread(&bitsPerSample, 2, 1, f))!=1) {
        printf("ERROR: bitsPerSample\n");
        return 1;
    }
    do {
        if ((fread(&c, 1, 1, f))!=1) {
            printf("ERROR: d io\n");
            return 1;
        }
        if (c == 'd'){
            if((fread(&c, 1, 1 ,f))!=1) return 1;
        }else continue;

        if (c=='a'){
            if((fread(&c, 1, 1 ,f))!=1) return 1;
        }else continue;

        if(c=='t'){ 
            if((fread(&c, 1, 1 ,f))!=1) return 1;
        }else continue;

        if(c=='a') exit = 1;

    }while(!exit);
    
    if ((fread(&subchunk2size, 4, 1, f))!=1) return 1;
 
    strcpy(wh->riff, riff);
    wh->chunkSize =  chunkSize;
    strcpy(wh->wave, wave);
    strcpy(wh->fmt, fmt);
    wh->subchunk1size = subchunk1size;
    wh->audioFormat = audioFormat;
    wh->numChannels = numChannels;
    wh->sampleRate = sampleRate;
    wh->byteRate = byteRate;
    wh->blockAlign = blockAlign;
    wh->bitsPerSample = bitsPerSample;
    strcpy(wh->data, data);
    wh->subchunk2size = subchunk2size;
    return 0;
}

void printWavHeader(wavheader *wh){
    printf("riff: %s\n", wh->riff);
    printf("chunkSize: %lu\n", wh->chunkSize);
    printf("wave: %s\n", wh->wave);
    printf("fmt: %s\n", wh->fmt);
    printf("subchunk1size: %lu\n", wh->subchunk1size);
    printf("audioFormat: %u\n", wh->audioFormat);
    printf("numChannels: %u\n", wh->numChannels);
    printf("sampleRate: %lu\n", wh->sampleRate);
    printf("byteRate: %lu\n", wh->byteRate);
    printf("blockAlign: %u\n", wh->blockAlign);
    printf("bitsPerSample: %u\n", wh->bitsPerSample);
    printf("data: %s\n", wh->data);
    printf("subchunk2size: %lu\n", wh->subchunk2size);
}

