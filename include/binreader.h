#ifndef BIN_HEADER_H
#define BIN_HEADER_H

/*! \file binheader.h
 *  \brief Defines operations over bin compressed files' headers.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "wavreader.h"
#include "bitio.h"

#define HUFFMAN     1
#define RUN_LENGTH  2
#define DELTA       3

/*! \struct _binheader
 *  \brief A struct that stores the header of a BIN file.
 */
struct _binheader {
    uint32_t chunkSize;
    uint32_t subchunk1size;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    uint32_t subchunk2size;
    uint8_t  firstEncoding; /*<! The first encoding technique applied. */
    uint8_t  secondEncoding; /*<! The second encoding technique applied. */
    uint8_t  thirdEncoding; /*<! The third encoding technique applied. */
    uint8_t hfFreqSize; /*<! The number of bits used to represent the frequency of each symbol. Used in the Huffman encoding. */
    uint32_t hfFreqLength; /*<! The number of different symbols in the frequency vector. Used in the Huffman encoding. */
    uint32_t rlBitsRun; /*<! The number of bits used to store the run length. Used in the Run-length encoding. */
    uint32_t rlBitsCode;
    uint32_t dtBitsDelta;
};

/*! \var typedef struct _binheader binheader;
 *  \brief A type definition to represent the BIN header.
 */
typedef struct _binheader binheader;

/*! \fn int get_header(binheader *wh, FILE *f)
 *  \brief Reads a header from a BIN file.
 *
 *  \param wh The bin header to be initialized.
 *  \param f A file to get the header from.
 */
int binh_get_header(binheader *wh, FILE *f, uint64_t **frequencies, uint32_t **firsts);

int binh_put_header(binheader *wh, FILE *f, uint64_t *frequencies, uint32_t *firsts);

uint8_t find_freq_size (uint64_t *frequencies, uint64_t length);

int binh_make_wave_header(binheader *bh, wavheader *wh, uint8_t first_enc, uint8_t sec_enc, uint8_t third_enc, uint64_t *frequencies, uint32_t frequency_length, uint32_t rl_bits_code, uint32_t rl_bits_run);

#endif /* BIN_HEADER_H */
