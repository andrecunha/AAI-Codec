#ifndef WAV_HEADER_H
#define WAV_HEADER_H

/*! \file wavheader.h
 *  \brief Defines operations over umcompressed wav files' headers.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bitio.h"

#define LITTLE_ENDIAN 0
#define BIG_ENDIAN 1

/*! \struct _wavheader
 *  \brief A struct that stores the header of a WAV file.
 */
struct _wavheader {
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
    uint32_t data;
    uint32_t subchunk2size;
    uint32_t endianness;
};

/*! \var typedef struct _wavheader wavheader.
 *  \brief A type definition to represent the WAV header.
 */
typedef struct _wavheader wavheader;

/*! \fn int getHeader(wavheader *wh, FILE *f)
 *  \brief Reads a header from a WAV file.
 *
 *  \param wh The wav header to be initialized.
 *  \param f A file to get the header from.
 */
int getHeader(wavheader *wh, FILE *f);

/*! \fn int read_byte(FILE *fp, uint8_t *c)
 *  \brief Reads a byte from a file;
 *
 *  \param fp A file to get the byte from.
 *  \param c The variable to store the byte.
 */
int read_byte(FILE *fp, uint8_t *c);

int read_4_bytes(FILE *fp, uint32_t *out, int endianness);

int read_3_bytes(FILE *fp, uint32_t *out, int endianness);

int read_2_bytes(FILE *fp, uint16_t *out, int endianness);

int load_to_uint32(FILE *fp, wavheader *h, uint32_t ***output);

int load_to_bitbuffer (FILE *fp, wavheader *h,  bitbuffer **output);

/*! \fn void printWavHeader(wavheader *wh);
 *  \brief Prints the WAV header.
 *
 *  \param wh The wav header to be printed.
 */
void printWavHeader(wavheader *wh);

#endif /* WAV_HEADER_H */
