#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <wavreader.h>
#include <error_handler.h>

int getHeader(wavheader *wh, FILE *f){
    uint32_t riff;
    uint32_t chunkSize;
    uint32_t wave;
    uint32_t fmt;
    uint32_t subchunk1size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    uint32_t data = 0x64617461;
    uint32_t subchunk2size;
    uint8_t c;
    short int exit = 0;
    int endianness;
    
    if (read_4_bytes(f, &riff, BIG_ENDIAN)) {
        printf("ERROR: io riff\n");
        return 1;
    }else {
        if (riff == 0x52494646) {
            endianness = LITTLE_ENDIAN;
        }else if(riff == 0x52494658){
            endianness = BIG_ENDIAN;
        }else{ 
            printf("ERROR: riff\n");
            return 1;
        }
    }
    if (read_4_bytes(f, &chunkSize, endianness)) {
        printf("ERROR: io chunkSize\n");
        return 1;
    }   
    if (read_4_bytes(f, &wave, BIG_ENDIAN)) {
        printf("ERROR: io wave\n");
        return 1;
    }else {
        if (wave != 0x57415645) {
            printf("ERROR: wave\n");
            return 1;
        }
    }
    if (read_4_bytes(f, &fmt, endianness)) {
        printf("ERROR: io fmt\n");
        return 1;
    }else {
        if (fmt == 0x666d7420) {
            printf("ERROR: fmt\n");
            return 1;
        }
    }
    if (read_4_bytes(f, &subchunk1size, endianness)) {
        printf("ERROR: subchunk1size\n");
        return 1;
    }
    if (read_2_bytes(f, &audioFormat, endianness)) {
        printf("ERROR: audioFormat\n");
        return 1;
    }
    if(audioFormat != 1) {
        printf("ERROR: Unsupported file format.\n");
        return 1;
    }
    if (read_2_bytes(f, &numChannels, endianness)) {
        printf("ERROR: numChannels\n");
        return 1;
    }
    if (read_4_bytes(f, &sampleRate, endianness)) {
        printf("ERROR: sampleRate\n");
        return 1;
    }
    if (read_4_bytes(f, &byteRate, endianness)) {
        printf("ERROR: byteRate\n");
        return 1;
    }
    if (read_2_bytes(f, &blockAlign, endianness)) {
        printf("ERROR: blockAlign\n");
        return 1;
    }
    if (read_2_bytes(f, &bitsPerSample, endianness)) {
        printf("ERROR: bitsPerSample\n");
        return 1;
    }
    do {
        if (read_byte(f, &c)) {
            printf("ERROR: d io\n");
            return 1;
        }
        if (c == 'd'){
            if(read_byte(f, &c)) return 1;
        }else continue;

        if (c=='a'){
            if(read_byte(f, &c)) return 1;
        }else continue;

        if(c=='t'){ 
            if(read_byte(f, &c)) return 1;
        }else continue;

        if(c=='a') exit = 1;

    }while(!exit);
    
    if ((fread(&subchunk2size, 4, 1, f))!=1) return 1;
 
    wh->riff = riff;
    wh->chunkSize =  chunkSize;
    wh->wave = wave;
    wh->fmt = fmt;
    wh->subchunk1size = subchunk1size;
    wh->audioFormat = audioFormat;
    wh->numChannels = numChannels;
    wh->sampleRate = sampleRate;
    wh->byteRate = byteRate;
    wh->blockAlign = blockAlign;
    wh->bitsPerSample = bitsPerSample;
    wh->data = data;
    wh->subchunk2size = subchunk2size;
    wh->endianness = endianness;
    return 0;
}

int read_byte(FILE *fp, uint8_t *c){
    if(fread(c, 1, 1 ,fp)!=1){
        /*printf("ERROR: IO\n");*/
        return 1;
    }
    return 0;
}

int read_4_bytes(FILE *fp, uint32_t *out, int endianness){
    uint8_t c[4];

    if(read_byte(fp, &c[0])==1)return 1;
    if(read_byte(fp, &c[1])==1)return 1;
    if(read_byte(fp, &c[2])==1)return 1;
    if(read_byte(fp, &c[3])==1)return 1;
    
    *out = 0;    

    if(endianness == LITTLE_ENDIAN){
        *out |= (c[3] << 24);
        *out |= (c[2] << 16);
        *out |= (c[1] << 8);
        *out |= c[0];
    }else{
        *out |= c[3];
        *out |= c[2] << 8;
        *out |= c[1] << 16;
        *out |= c[0] << 24;
    }    
    return 0;
}

int read_3_bytes(FILE *fp, uint32_t *out, int endianness){
    uint8_t c[4];

    if(read_byte(fp, &c[0]))return 1;
    if(read_byte(fp, &c[1]))return 1;
    if(read_byte(fp, &c[2]))return 1;
    
    *out = 0;    

    if(endianness == LITTLE_ENDIAN){
        *out |= (c[2] << 16);
        *out |= (c[1] << 8);
        *out |= c[0];
    }else{
        *out |= c[2];
        *out |= c[1] << 8;
        *out |= c[0] << 16;
    }    
    return 0;
}

int read_2_bytes(FILE *fp, uint16_t *out, int endianness){
    uint8_t c[2];

    if(read_byte(fp, &c[0])) return 1;
    if(read_byte(fp, &c[1])) return 1;

    *out = 0;   
 
    if(endianness == LITTLE_ENDIAN){
        *out |= c[0];
        *out |= c[1] << 8;
    }else{
        *out |= c[1];
        *out |= c[0] << 8;
    }    
    return 0;
}

int load_to_uint32(FILE *fp, wavheader *h, uint32_t ***output)
{
        *output = malloc(h->numChannels*sizeof(uint32_t *));
        int i;
        uint32_t **_output = *output;
        uint32_t channel_size = h->subchunk2size/h->numChannels;
        for(i=0; i<h->numChannels; i++)
                _output[i] = malloc(channel_size*sizeof(uint32_t));

        int curr_channel;
        uint32_t curr_position=0;
        uint8_t b;
        uint16_t bb;
        while(!feof(fp)) {
                for(curr_channel=0; curr_channel<h->numChannels; curr_channel++) {
                        switch(h->bitsPerSample) {
                                case 8: read_byte(fp, &b);
                                        _output[curr_channel][curr_position] = b;
                                        break;
                                case 16: read_2_bytes(fp, &bb, h->endianness);
                                         _output[curr_channel][curr_position] = bb;
                                         break;
                                case 24: read_3_bytes(fp, &_output[curr_channel][curr_position], h->endianness);
                                         break;
                                case 32: read_4_bytes(fp, &_output[curr_channel][curr_position], h->endianness);
                                         break;
                                default:
                                        ERROR("Unsupported bit rate.");
                                        exit(1);
                                        break;
                        }

                }

                curr_position++;
        }
        return 0;
}

void printWavHeader(wavheader *wh){
    printf("riff: %"PRIu32"\n", wh->riff);
    printf("chunkSize: %"PRIu32"\n", wh->chunkSize);
    printf("wave: %"PRIu32"\n", wh->wave);
    printf("fmt: %"PRIu32"\n", wh->fmt);
    printf("subchunk1size: %"PRIu32"\n", wh->subchunk1size);
    printf("audioFormat: %"PRIu16"\n", wh->audioFormat);
    printf("numChannels: %"PRIu16"\n", wh->numChannels);
    printf("sampleRate: %"PRIu32"\n", wh->sampleRate);
    printf("byteRate: %"PRIu32"\n", wh->byteRate);
    printf("blockAlign: %"PRIu16"\n", wh->blockAlign);
    printf("bitsPerSample: %"PRIu16"\n", wh->bitsPerSample);
    printf("data: %"PRIu32"\n", wh->data);
    printf("endianness: %s\n", wh->endianness==LITTLE_ENDIAN?"LITTLE":"BIG");
    printf("subchunk2size: %"PRIu32"\n", wh->subchunk2size);
}

