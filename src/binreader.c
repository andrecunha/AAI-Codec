#include <binreader.h>
#include <math.h>
#include <error_handler.h>
#include <wavreader.h>

uint8_t find_freq_size (uint64_t *frequencies, uint64_t length)
{
        uint32_t max=0, i;
        for(i=0; i<length; i++)
                if(frequencies[i] > max)
                        max = frequencies[i];
        return ceil(log(max)/log(2));
}

int binh_make_wave_header(binheader *bh, wavheader *wh, uint8_t first_enc, uint8_t sec_enc, uint8_t third_enc, uint64_t *frequencies, uint32_t frequency_length, uint32_t rl_bits_code, uint32_t rl_bits_run)
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

        if ((first_enc==HUFFMAN)||(sec_enc==HUFFMAN)||(third_enc==HUFFMAN)) {
                bh->hfFreqSize = find_freq_size(frequencies, frequency_length);
                bh->hfFreqLength = frequency_length;
        }
        
        if ((first_enc==RUN_LENGTH)||(sec_enc==RUN_LENGTH)||(third_enc==RUN_LENGTH)) {
                bh->rlBitsCode = rl_bits_code;
                bh->rlBitsRun = rl_bits_run;
        }

        return 0;
}

int binh_put_header(binheader *wh, FILE *f, uint64_t *frequencies, uint32_t *firsts)
{
       if(fwrite(&(wh->chunkSize), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot write chunkSize to file.\n")
               exit(1);
       }

       if(fwrite(&(wh->subchunk1size), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot write subchunk2size to file.\n")
               exit(1);
       }

       if(fwrite(&(wh->numChannels), sizeof(uint16_t), 1, f) != 1) {
               ERROR("Cannot write numChannels to file.\n")
               exit(1);
       }

       if(fwrite(&(wh->sampleRate), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot write sampleRate to file.\n")
               exit(1);
       }

       if(fwrite(&(wh->byteRate), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot write byteRate to file.\n")
               exit(1);
       }

       if(fwrite(&(wh->blockAlign), sizeof(uint16_t), 1, f) != 1) {
               ERROR("Cannot write blockAlign to file.\n")
               exit(1);
       }

       if(fwrite(&(wh->bitsPerSample), sizeof(uint16_t), 1, f) != 1) {
               ERROR("Cannot write bitsPerSample to file.\n")
               exit(1);
       }

       if(fwrite(&(wh->subchunk2size), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot write subchunk2size to file.\n")
               exit(1);
       }

       if(fwrite(&(wh->firstEncoding), sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot write firstEncoding to file.\n")
               exit(1);
       }

       if(fwrite(&(wh->secondEncoding), sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot write secondEncoding to file.\n")
               exit(1);
       }

       if(fwrite(&(wh->thirdEncoding), sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot write thirdEncoding to file.\n")
               exit(1);
       }

       uint8_t first_enc = wh->firstEncoding,
               sec_enc = wh->secondEncoding,
               third_enc = wh->thirdEncoding;

       if ((first_enc==HUFFMAN)||(sec_enc==HUFFMAN)||(third_enc==HUFFMAN)) {
            if(fwrite(&(wh->hfFreqSize), sizeof(uint8_t), 1, f) != 1) {
                    ERROR("Cannot write hfFreqSize to file.\n")
                    exit(1);
            }

            if(fwrite(&(wh->hfFreqLength), sizeof(uint32_t), 1, f) != 1) {
                    ERROR("Cannot write hfFreqLength to file.\n")
                    exit(1);
            }

            /*XXX PEDIR PARA ALGUÉM FAZER!!!*/
            int i;
            for(i=0; i<wh->hfFreqLength; i++) {
                    switch(wh->hfFreqSize) {
                            case 8:
                                    break;
                            case 16:
                                    break;
                            case 24:
                                    break;
                            case 32:
                                    break;
                    }
            }
       }
       
       if ((first_enc==RUN_LENGTH)||(sec_enc==RUN_LENGTH)||(third_enc==RUN_LENGTH)) {

                if(fwrite(&(wh->rlBitsRun), sizeof(uint32_t), 1, f) != 1) {
                    ERROR("Cannot write rlBitsRun to file.\n")
                    exit(1);
                }

                if(fwrite(&(wh->rlBitsCode), sizeof(uint32_t), 1, f) != 1) {
                    ERROR("Cannot write rlBitsCode to file.\n")
                    exit(1);
                }
       }

       if ((first_enc==DELTA)||(sec_enc==DELTA)||(third_enc==DELTA)) {
               int i;
               for(i=0; i<wh->numChannels; i++) {
                       /*XXX: FAZER!!!*/
               }
       }
       return 0;
}

int binh_get_header(binheader *wh, FILE *f, uint64_t **frequencies, uint32_t **firsts)
{
       if(fread(&(wh->chunkSize), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot read chunkSize from file.\n")
               exit(1);
       }

       if(fread(&(wh->subchunk1size), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot read subchunk2size from file.\n")
               exit(1);
       }

       if(fread(&(wh->numChannels), sizeof(uint16_t), 1, f) != 1) {
               ERROR("Cannot read numChannels from file.\n")
               exit(1);
       }

       if(fread(&(wh->sampleRate), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot read sampleRate to file.\n")
               exit(1);
       }

       if(fread(&(wh->byteRate), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot read byteRate from file.\n")
               exit(1);
       }

       if(fread(&(wh->blockAlign), sizeof(uint16_t), 1, f) != 1) {
               ERROR("Cannot read blockAlign from file.\n")
               exit(1);
       }

       if(fwrite(&(wh->bitsPerSample), sizeof(uint16_t), 1, f) != 1) {
               ERROR("Cannot read bitsPerSample from file.\n")
               exit(1);
       }

       if(fwrite(&(wh->subchunk2size), sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot read subchunk2size from file.\n")
               exit(1);
       }

       if(fread(&(wh->firstEncoding), sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot read firstEncoding from file.\n")
               exit(1);
       }

       if(fread(&(wh->secondEncoding), sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot read secondEncoding from file.\n")
               exit(1);
       }

       if(fread(&(wh->thirdEncoding), sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot read thirdEncoding from file.\n")
               exit(1);
       }

       uint8_t first_enc = wh->firstEncoding,
               sec_enc = wh->secondEncoding,
               third_enc = wh->thirdEncoding;

       if ((first_enc)||(sec_enc==HUFFMAN)||(third_enc==HUFFMAN)) {
            if(fread(&(wh->hfFreqSize), sizeof(uint8_t), 1, f) != 1) {
                    ERROR("Cannot read hfFreqSize from file.\n")
                    exit(1);
            }

            if(fread(&(wh->hfFreqLength), sizeof(uint32_t), 1, f) != 1) {
                    ERROR("Cannot read hfFreqLength from file.\n")
                    exit(1);
            }

            /*XXX PEDIR PARA ALGUÉM FAZER!!!*/
            int i;
            for(i=0; i<wh->hfFreqLength; i++) {
                    switch(wh->hfFreqSize) {
                            case 8:
                                    break;
                            case 16:
                                    break;
                            case 24:
                                    break;
                            case 32:
                                    break;
                    }
            }
       }
       
       if ((first_enc==RUN_LENGTH)||(sec_enc==RUN_LENGTH)||(third_enc==RUN_LENGTH)) {

                if(fread(&(wh->rlBitsRun), sizeof(uint32_t), 1, f) != 1) {
                    ERROR("Cannot read rlBitsRun from file.\n")
                    exit(1);
                }

                if(fread(&(wh->rlBitsCode), sizeof(uint32_t), 1, f) != 1) {
                    ERROR("Cannot read rlBitsCode from file.\n")
                    exit(1);
                }
       }

       if ((first_enc==DELTA)||(sec_enc==DELTA)||(third_enc==DELTA)) {
               int i;
               for(i=0; i<wh->numChannels; i++) {
                       /*XXX: FAZER!!!*/
               }
       }

       return 0;
}

