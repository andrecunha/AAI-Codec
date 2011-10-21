#include <binreader.h>
#include <math.h>
#include <error_handler.h>
#include <wavreader.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

uint8_t find_freq_size (uint64_t *frequencies, uint64_t length)
{
        uint32_t max=0, i;
        uint8_t ret;

        for(i=0; i<length; i++)
                if(frequencies[i] > max)
                        max = frequencies[i];
            
        ret = ceil(log(max)/log(2));
        if (ret<9)
            return (uint8_t)8;
        else if(ret<17)
            return (uint8_t)16;
        else if(ret<25)
            return (uint8_t)24;
        else if(ret<33)
            return (uint8_t)32;
        else if(ret<41)
            return (uint8_t)40;
        else if(ret<49)
            return (uint8_t)48;
        else if(ret<57)
            return (uint8_t)56;
        else if(ret<65)
            return (uint8_t)64;

        else ERROR("Unexpected frequency size");

        return (uint8_t) 64;
 
}

int binh_make_bin_header(binheader *bh, wavheader *wh, uint8_t first_enc, uint8_t sec_enc, uint8_t third_enc)
{
        bh->chunkSize = wh->chunkSize;
        bh->subchunk1size = wh->subchunk1size;
        bh->numChannels = wh->numChannels;
        bh->sampleRate = wh->sampleRate;
        bh->byteRate = wh->byteRate;
        bh->blockAlign = wh->blockAlign;
        bh->bitsPerSample = wh->bitsPerSample;
        bh->subchunk2size = wh->subchunk2size;
        bh->firstEncoding = first_enc;
        bh->secondEncoding = sec_enc;
        bh->thirdEncoding = third_enc;

        if (wh->riff == 0x52494646)
            bh->endianness = LITTLE_ENDIAN;
        else if(wh->riff == 0x52494658)
            bh->endianness = BIG_ENDIAN;

        return 0;
}

int binh_put_header(binheader *wh, FILE *f, uint64_t **frequencies, uint32_t frequency_length, uint32_t *firsts, uint32_t *max_bits, uint32_t *nbits_run, uint32_t *nbits_code)
{
       if(fwrite(&(wh->chunkSize), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot write chunkSize to file.");
               exit(1);
       }

       if(fwrite(&(wh->subchunk1size), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot write subchunk2size to file.");
               exit(1);
       }

       if(fwrite(&(wh->numChannels), sizeof(uint16_t), 1, f) != 1) {
               ERROR("Cannot write numChannels to file.");
               exit(1);
       }

       if(fwrite(&(wh->sampleRate), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot write sampleRate to file.");
               exit(1);
       }

       if(fwrite(&(wh->byteRate), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot write byteRate to file.");
               exit(1);
       }

       if(fwrite(&(wh->blockAlign), sizeof(uint16_t), 1, f) != 1) {
               ERROR("Cannot write blockAlign to file.");
               exit(1);
       }

       if(fwrite(&(wh->bitsPerSample), sizeof(uint16_t), 1, f) != 1) {
               ERROR("Cannot write bitsPerSample to file.");
               exit(1);
       }

       if(fwrite(&(wh->subchunk2size), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot write subchunk2size to file.");
               exit(1);
       }

       if(fwrite(&(wh->endianness), sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot write endianness to file.");
               exit(1);
       }

       if(fwrite(&(wh->firstEncoding), sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot write firstEncoding to file.");
               exit(1);
       }

       if(fwrite(&(wh->secondEncoding), sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot write secondEncoding to file.");
               exit(1);
       }

       if(fwrite(&(wh->thirdEncoding), sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot write thirdEncoding to file.");
               exit(1);
       }

       uint8_t first_enc = wh->firstEncoding,
               sec_enc = wh->secondEncoding,
               third_enc = wh->thirdEncoding;

       int curr_channel=0;

       if ((first_enc==HUFFMAN)||(sec_enc==HUFFMAN)||(third_enc==HUFFMAN)) {

            uint8_t *freq_size = calloc(wh->numChannels, sizeof(uint8_t));
            for(curr_channel=0; curr_channel<wh->numChannels; curr_channel++){
                    freq_size[curr_channel] = find_freq_size(frequencies[curr_channel],  frequency_length);

                    if(fwrite(&(freq_size[curr_channel]), sizeof(uint8_t), 1, f) != 1) {
                        ERROR("Cannot write freq_size to file.");
                        exit(1);
                    }
            }

            
            if(fwrite(&(frequency_length), sizeof(uint32_t), 1, f) != 1) {
                    ERROR("Cannot write hfFreqLength to file.");
                    exit(1);
            }

            int i, j;
            union frequency{
                uint64_t freq64;
                uint8_t freq8[8];
            } curr_freq;
            

            for(curr_channel=0; curr_channel<wh->numChannels; curr_channel++)
               for(i=0; i<frequency_length; i++) {
                   curr_freq.freq64 = frequencies[curr_channel][i];
                   /*for (j=0; j<(freq_size[curr_channel]/8); j++)
                       if(fwrite(&curr_freq.freq8[7-j], sizeof(uint8_t), 1, f) != 1) {
                           ERROR("Cannot write frequency to file.");
                           exit(1);
                       }*/
                       /*XXX changed*/
                   if(fwrite(&curr_freq.freq64, sizeof(uint64_t), 1, f)!=1 ){
                       ERROR("Cannot write frequency to file.");
                       exit(1);
                   
                   }
                }

            free(freq_size);
       }
       
       if ((first_enc==RUN_LENGTH)||(sec_enc==RUN_LENGTH)||(third_enc==RUN_LENGTH)) {

            for(curr_channel=0; curr_channel<wh->numChannels; curr_channel++) {

                if(fwrite(&(nbits_run[curr_channel]), sizeof(uint32_t), 1, f) != 1) {
                    ERROR("Cannot write nbits_run to file.");
                    exit(1);
                }
                printf("Nbits_run %"PRIu32"\n", nbits_run[curr_channel]);

                if(fwrite(&(nbits_code[curr_channel]), sizeof(uint32_t), 1, f) != 1) {
                    ERROR("Cannot write nbits_code to file.");
                    exit(1);
                }
                printf("Nbits_code %"PRIu32"\n", nbits_code[curr_channel]);
            }
       }

       if ((first_enc==DELTA)||(sec_enc==DELTA)||(third_enc==DELTA)) {
               for(curr_channel=0; curr_channel<wh->numChannels; curr_channel++) {
                    if(fwrite(&(max_bits[curr_channel]), sizeof(uint32_t), 1, f) != 1) {
                        ERROR("Cannot write max_bits to file.");
                        exit(1);
                    }
                    printf("Max_bits %"PRIu32"\n", max_bits[curr_channel]);
                    if(fwrite(&(firsts[curr_channel]), sizeof(uint32_t), 1, f) != 1) {
                        ERROR("Cannot write firsts to file.");
                        exit(1);
                    }
                    printf("Firsts %"PRIu32"\n", firsts[curr_channel]);
               }
       }

       return 0;
}

int binh_get_header(binheader *wh, FILE *f, uint64_t ***frequencies, uint32_t *frequency_length, uint32_t **firsts, uint32_t **max_bits, uint32_t **nbits_run, uint32_t **nbits_code)
{
      
       if(fread(&(wh->chunkSize), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot read chunkSize from file.");
               exit(1);
       }

       if(fread(&(wh->subchunk1size), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot read subchunk2size from file.");
               exit(1);
       }

       if(fread(&(wh->numChannels), sizeof(uint16_t), 1, f) != 1) {
               ERROR("Cannot read numChannels from file.");
               exit(1);
       }

       if(fread(&(wh->sampleRate), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot read sampleRate from file.");
               exit(1);
       }

       if(fread(&(wh->byteRate), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot read byteRate from file.");
               exit(1);
       }

       if(fread(&(wh->blockAlign), sizeof(uint16_t), 1, f) != 1) {
               ERROR("Cannot read blockAlign from file.");
               exit(1);
       }

       if(fread(&(wh->bitsPerSample), sizeof(uint16_t), 1, f) != 1) {
               ERROR("Cannot read bitsPerSample from file.");
               exit(1);
       }

       if(fread(&(wh->subchunk2size), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot read subchunk2size from file.");
               exit(1);
       }

       if(fread(&(wh->endianness), sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot read endianness from file.");
               exit(1);
       }


    printf("chunkSize: %"PRIu32"\n", wh->chunkSize);
    printf("subchunk1size: %"PRIu32"\n", wh->subchunk1size);
    printf("numChannels: %"PRIu16"\n", wh->numChannels);
    printf("sampleRate: %"PRIu32"\n", wh->sampleRate);
    printf("byteRate: %"PRIu32"\n", wh->byteRate);
    printf("blockAlign: %"PRIu16"\n", wh->blockAlign);
    printf("bitsPerSample: %"PRIu16"\n", wh->bitsPerSample);
    printf("endianness: %s\n", wh->endianness==LITTLE_ENDIAN?"LITTLE":"BIG");
    printf("subchunk2size: %"PRIu32"\n", wh->subchunk2size);


       if(fread(&(wh->firstEncoding), sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot read firstEncoding from file.");
               exit(1);
       }

       if(fread(&(wh->secondEncoding), sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot read secondEncoding from file.");
               exit(1);
       }

       if(fread(&(wh->thirdEncoding), sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot read thirdEncoding from file.");
               exit(1);
       }

       uint8_t first_enc = wh->firstEncoding,
               sec_enc = wh->secondEncoding,
               third_enc = wh->thirdEncoding;


       printf("### First: %"PRIu8".\n", first_enc);
       printf("### Second:  %"PRIu8".\n", sec_enc);
       printf("### Third: %"PRIu8".\n", third_enc);

       int curr_channel=0;
       uint8_t *freq_size;

       if ((first_enc==HUFFMAN)||(sec_enc==HUFFMAN)||(third_enc==HUFFMAN)) {
            freq_size = calloc(wh->numChannels, sizeof(uint8_t));
            for(curr_channel=0; curr_channel<wh->numChannels; curr_channel++){
                   if(fread(&(freq_size[curr_channel]), sizeof(uint8_t), 1, f) != 1) {
                       ERROR("Cannot read freq_size from file.");
                       exit(1);
                   }
                    printf("Frequency size[%d] %"PRIu8"\n", curr_channel, freq_size[curr_channel]);
            }
            
            if(fread(frequency_length, sizeof(uint32_t), 1, f) != 1) {
                    ERROR("Cannot read hfFreqLength from file.");
                    exit(1);
            }

            printf("Frequency length: %"PRIu32"\n", *frequency_length);

            int i, j;

            union frequency{
                uint64_t freq64;
                uint8_t freq8[8];
            } curr_freq;

            *frequencies = calloc(wh->numChannels, sizeof(uint64_t *));

            printf("Reading frequencies...\n");

            for(curr_channel=0; curr_channel<wh->numChannels; curr_channel++){

                    (*frequencies)[curr_channel] = calloc(*frequency_length, sizeof(uint64_t));
                    for(i=0; i<(*frequency_length); i++) {
                                memset(&curr_freq.freq64, 0, sizeof(uint64_t));
/*                                for (j=0; j<(freq_size[curr_channel]/8); j++)
                                    if(fread(&curr_freq.freq8[7-j], sizeof(uint8_t), 1, f) != 1) {
                                        ERROR("Cannot read frequency from file.");
                                        exit(1);
                                    }*/
                          if(fread(&curr_freq.freq64,sizeof(uint64_t), 1, f)!=1){
                                        ERROR("Cannot read frequency from file.");
                                        exit(1);
                        }
                               (*frequencies)[curr_channel][i] = curr_freq.freq64;
                     }
            }
            free(freq_size);
       }
       
       if ((first_enc==RUN_LENGTH)||(sec_enc==RUN_LENGTH)||(third_enc==RUN_LENGTH)) {

            *nbits_run = calloc(wh->numChannels, sizeof(uint32_t));
            *nbits_code = calloc(wh->numChannels, sizeof(uint32_t));

            for(curr_channel=0; curr_channel<wh->numChannels; curr_channel++) {

                if(fread((nbits_run)[curr_channel], sizeof(uint32_t), 1, f) != 1) {
                    ERROR("Cannot read nbits_run from file.");
                    exit(1);
                }
                printf("Nbits_run %"PRIu32"\n", *nbits_run[curr_channel]);

                if(fread((nbits_code)[curr_channel], sizeof(uint32_t), 1, f) != 1) {
                    ERROR("Cannot read nbits_code from file.");
                    exit(1);
                }
                printf("Nbits_code %"PRIu32"\n", *nbits_code[curr_channel]);
            }
       }

       if ((first_enc==DELTA)||(sec_enc==DELTA)||(third_enc==DELTA)) {

               *max_bits = calloc(wh->numChannels, sizeof(uint32_t));
               *firsts = calloc(wh->numChannels, sizeof(uint32_t));
               for(curr_channel=0; curr_channel<wh->numChannels; curr_channel++) {
                    if(fread((max_bits[curr_channel]), sizeof(uint32_t), 1, f) != 1) {
                        ERROR("Cannot read max_bits from file.");
                        exit(1);
                    }
                    printf("Max_bits %"PRIu32"\n", *max_bits[curr_channel]);
                    if(fread((firsts[curr_channel]), sizeof(uint32_t), 1, f) != 1) {
                        ERROR("Cannot read firsts from file.");
                        exit(1);
                    }
                    printf("Firsts %"PRIu32"\n", *firsts[curr_channel]);
               }
       }

       return 0;
}

