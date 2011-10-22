#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <wavreader.h>
#include <error_handler.h>

int putHeader(binheader *wh, FILE *f){
    uint32_t riff = (wh->endianness == LITTLE_ENDIAN)?0x52494646:0x52494658;
    uint32_t chunkSize = wh->chunkSize;
    uint32_t wave = 0x57415645;
    uint32_t fmt = 0x666d7420;
    uint32_t subchunk1size = wh->subchunk1size;
    uint16_t audioFormat = 0x0001;
    uint16_t numChannels = wh->numChannels;
    uint32_t sampleRate = wh->sampleRate;
    uint32_t byteRate = wh->byteRate;
    uint16_t blockAlign = wh->blockAlign;
    uint16_t bitsPerSample = wh->bitsPerSample;
    uint32_t data = 0x64617461;
    uint32_t subchunk2size = wh->subchunk2size;
    uint8_t endianness = wh->endianness;
    
    if(write_4_bytes(f, &riff, BIG_ENDIAN)){
        ERROR("Couldn't write RIFF to file");
        return 1;
    }

    if (write_4_bytes(f, &chunkSize, endianness)) {
        printf("ERROR: io chunkSize\n");
        return 1;
    }   

    if (write_4_bytes(f, &wave, BIG_ENDIAN)) {
        printf("ERROR: io wave\n");
        return 1;
    }

    if (write_4_bytes(f, &fmt, BIG_ENDIAN)) {
        printf("ERROR: io fmt\n");
        return 1;
    }

    if (write_4_bytes(f, &subchunk1size, endianness)) {
        printf("ERROR: subchunk1size\n");
        return 1;
    }

    if (write_2_bytes(f, &audioFormat, endianness)) {
        printf("ERROR: audioFormat\n");
        return 1;
    }

    if (write_2_bytes(f, &numChannels, endianness)) {
        printf("ERROR: numChannels\n");
        return 1;
    }

    if (write_4_bytes(f, &sampleRate, endianness)) {
        printf("ERROR: sampleRate\n");
        return 1;
    }

    if (write_4_bytes(f, &byteRate, endianness)) {
        printf("ERROR: byteRate\n");
        return 1;
    }

    if (write_2_bytes(f, &blockAlign, endianness)) {
        printf("ERROR: blockAlign\n");
        return 1;
    }

    if (write_2_bytes(f, &bitsPerSample, endianness)) {
        printf("ERROR: bitsPerSample\n");
        return 1;
    }

    int x=0;
    if(fwrite(&x, 1, subchunk1size-16, f)!=(subchunk1size-16)){
        ERROR("Garbage before DATA");   
        return 1;
    }
    
    if(write_4_bytes(f, &data, BIG_ENDIAN)){
        ERROR("Data");
        return 1;
    }
    
    if(write_4_bytes(f, &subchunk2size, endianness)){
        ERROR("Subchunk2size");
        return 1;
    }

    return 0;
}

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
    if (read_4_bytes(f, &fmt, BIG_ENDIAN)) {
        printf("ERROR: io fmt\n");
        return 1;
    }else {
        if (fmt != 0x666d7420) {
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
    
    if (read_4_bytes(f, &subchunk2size, endianness)) {
        printf("ERROR: subchunk2size\n");
        return 1;
    }
 
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

int write_byte(FILE *fp, uint8_t *c){
    if(fwrite(c, 1, 1 ,fp)!=1){
        ERROR("IO error at wavreader.write_byte");
        return 1;
    }
    return 0;
}

int write_4_bytes(FILE *fp, uint32_t *out, int endianness){
    uint8_t c[4];

    if(endianness == BIG_ENDIAN){
        c[3] = (*out) & 0x000000ff;
        c[2] = (*out >> 8) & 0x000000ff;
        c[1] = (*out >> 16) & 0x000000ff;
        c[0] = (*out >> 24) & 0x000000ff;
    }else{
        c[0] = (*out) & 0x000000ff;
        c[1] = (*out >> 8) & 0x000000ff;
        c[2] = (*out >> 16) & 0x000000ff;
        c[3] = (*out >> 24) & 0x000000ff;
    }

    if(write_byte(fp, &c[0])==1){
        ERROR("IO error at wavreader.read_4_bytes");
        return 1;
    }

    if(write_byte(fp, &c[1])==1){
        ERROR("IO error at wavreader.read_4_bytes");
        return 1;
    }

    if(write_byte(fp, &c[2])==1){
        ERROR("IO error at wavreader.read_4_bytes");
        return 1;
    }

    if(write_byte(fp, &c[3])==1){
        ERROR("IO error at wavreader.read_4_bytes");
        return 1;
    }

    return 0;
}

int write_3_bytes(FILE *fp, uint32_t *out, int endianness){
    uint8_t c[4];

    if(endianness == BIG_ENDIAN){
        c[2] = (*out) & 0x0000ff;
        c[1] = (*out >> 8) & 0x0000ff;
        c[0] = (*out >> 16) & 0x0000ff;
    }else{
        c[0] = (*out) & 0x0000ff;
        c[1] = (*out >> 8) & 0x0000ff;
        c[2] = (*out >> 16) & 0x0000ff;
    }

    if(write_byte(fp, &c[0])){
        ERROR("IO error at wavreader.read_3_bytes");
        return 1;
    }
    if(write_byte(fp, &c[1])){
        ERROR("IO error at wavreader.read_3_bytes");
        return 1;
    }
    if(write_byte(fp, &c[2])){
        ERROR("IO error at wavreader.read_3_bytes");
        return 1;
    }
    
    return 0;
}

int write_2_bytes(FILE *fp, uint16_t *out, int endianness){
    uint8_t c[2];

    if(endianness == BIG_ENDIAN){
        c[1] = (*out) & 0x00ff;
        c[0] = (*out >> 8) & 0x00ff;
    }else{
        c[0] = (*out) & 0x00ff;
        c[1] = (*out >> 8) & 0x00ff;
    }

    if(write_byte(fp, &c[0])){
        ERROR("IO error at wavreader.read_2_bytes");
        return 1;
    }
    if(write_byte(fp, &c[1])){
        ERROR("IO error at wavreader.read_2_bytes");
        return 1;
    }

    return 0;
}

int read_byte(FILE *fp, uint8_t *c){
    if(fread(c, 1, 1 ,fp)!=1){
        ERROR("IO error at wavreader.read_byte");
        return 1;
    }
    return 0;
}

int read_4_bytes(FILE *fp, uint32_t *out, int endianness){
    uint8_t c[4];

    if(read_byte(fp, &c[0])==1){
        ERROR("IO error at wavreader.read_4_bytes");
        return 1;
    }

    if(read_byte(fp, &c[1])==1){
        ERROR("IO error at wavreader.read_4_bytes");
        return 1;
    }

    if(read_byte(fp, &c[2])==1){
        ERROR("IO error at wavreader.read_4_bytes");
        return 1;
    }

    if(read_byte(fp, &c[3])==1){
        ERROR("IO error at wavreader.read_4_bytes");
        return 1;
    }

    
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

    if(read_byte(fp, &c[0])){
        ERROR("IO error at wavreader.read_3_bytes");
        return 1;
    }
    if(read_byte(fp, &c[1])){
        ERROR("IO error at wavreader.read_3_bytes");
        return 1;
    }
    if(read_byte(fp, &c[2])){
        ERROR("IO error at wavreader.read_3_bytes");
        return 1;
    }
    
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

    if(read_byte(fp, &c[0])){
        ERROR("IO error at wavreader.read_2_bytes");
        return 1;
    }
    if(read_byte(fp, &c[1])){
        ERROR("IO error at wavreader.read_2_bytes");
        return 1;
    }

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
        /*TODO: É aqui o pobrema!!!!!!*/
        uint32_t channel_size = (h->subchunk2size)/(h->numChannels)/((h->bitsPerSample)/8);
        printf("channel_size = %"PRIu32"\n", channel_size);
        for(i=0; i<h->numChannels; i++)
                _output[i] = malloc(channel_size*sizeof(uint32_t));

        int curr_channel;
        uint32_t curr_position=0;
        uint8_t b;
        uint16_t bb;
        unsigned long count;
        for(count=0; count<channel_size; count++) {
                for(curr_channel=0; curr_channel<h->numChannels; curr_channel++) {
                        switch(h->bitsPerSample) {
                                case 8: read_byte(fp, &b);
                                        _output[curr_channel][curr_position] = b;
                                        break;
                                case 16: if (!read_2_bytes(fp, &bb, h->endianness))
                                                 _output[curr_channel][curr_position] = bb;
                                         else {
                                                 printf("ERROR: Error while loading sample %"PRIu32" from channel %d.\n", curr_position, curr_channel);
                                                 return 1;
                                         }
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

int load_to_bitbuffer (FILE *fp, wavheader *h,  bitbuffer **output)
{
        uint32_t ** uint32_data;
        load_to_uint32(fp, h, &uint32_data);
        *output = calloc(h->numChannels, sizeof(bitbuffer));

        uint32_t channel_size_bits = ((h->subchunk2size)/(h->numChannels))*8;
        uint32_t channel_size_samples = (h->subchunk2size)/(h->numChannels)/((h->bitsPerSample)/8);

        int curr_channel;
        for(curr_channel=0; curr_channel<h->numChannels; curr_channel++) {
                binit(&(*output)[curr_channel], channel_size_bits);
                b_from_uint32(&(*output)[curr_channel], uint32_data[curr_channel], channel_size_samples, h->bitsPerSample, 8); /*XXX: 8 mesmo?*/
                free(uint32_data[curr_channel]);
        }

        free(uint32_data);
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

