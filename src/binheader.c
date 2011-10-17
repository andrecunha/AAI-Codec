#include <binheader.h>
#include <math.h>
#include <error_handler.h>

uint8_t find_freq_size (uint64_t *frequencies, uint64_t length)
{
        uint32_t max=0, i;
        for(i=0; i<length; i++)
                if(frequencies[i] > max)
                        max = frequencies[i];
        return ceil(log(max)/log(2));
}

int binh_make_wave_header(binheader *bh, wavheader *wh, uint64_t frequencies, uint32_t frequency_length, uint32_t rl_bits_code, uint32_t rl_bits_run, uint32_t *firsts)
{
        bh.chunkSize = wh.chunkSize;
        bh.subchunk1size = wh.subchunk1size;
        bh.numChannels = wh.numChannels;
        bh.sampleRate = wh.sampleRate;
        bh.byteRate = wh.byteRate;
        bh.blockAlign = wh.blockAlign;
        bh.bitsPerSample = wh.bitsPerSample;
        bh.subchunk2size = wh.subchunk2size;
        bh.endianness = wh.endianness;
        bh.firstEncoding = first_enc;
        bh.secondEncoding = sec_enc;
        bh.thirdEncoding = third_enc;
        bh.hfFreqSize = find_freq_size(frequencies, frequency_length);
        bh.hfFreqLength = frequency_length;
        bh.rlBitsCode = rl_bits_code;
        bh.rl_bits_run = rl_bits_run;
        bh.firsts = firsts;
        return 0;
}

int binh_put_header(binheader *wh, FILE *f)
{
       if(fwrite(wh->chunkSize, sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot write chunkSize to file.\n")
               exit(1);
       }

       if(fwrite(wh->subchunk1size, sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot write subchunk2size to file.\n")
               exit(1);
       }

       if(fwrite(wh->numChannels, sizeof(uint16_t), 1, f) != 1) {
               ERROR("Cannot write numChannels to file.\n")
               exit(1);
       }

       if(fwrite(wh->sampleRate, sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot write sampleRate to file.\n")
               exit(1);
       }

       if(fwrite(wh->byteRate, sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot write byteRate to file.\n")
               exit(1);
       }

       if(fwrite(wh->blockAlign, sizeof(uint16_t), 1, f) != 1) {
               ERROR("Cannot write blockAlign to file.\n")
               exit(1);
       }

       if(fwrite(wh->bitsPerSample, sizeof(uint16_t), 1, f) != 1) {
               ERROR("Cannot write bitsPerSample to file.\n")
               exit(1);
       }

       if(fwrite(wh->subchunk2size, sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot write subchunk2size to file.\n")
               exit(1);
       }

       if(fwrite(wh->endianness, sizeof(uint32_t), 1, f) != 1) {
               ERROR("Cannot write endianness to file.\n")
               exit(1);
       }
       
       if(fwrite(wh->firstEncoding, sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot write firstEncoding to file.\n")
               exit(1);
       }

       if(fwrite(wh->secondEncoding, sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot write secondEncoding to file.\n")
               exit(1);
       }

       if(fwrite(wh->thirdEncoding, sizeof(uint8_t), 1, f) != 1) {
               ERROR("Cannot write thirdEncoding to file.\n")
               exit(1);
       }

}
