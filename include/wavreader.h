#ifndef WAVREADER_H
#define WAVREADER_H

/*! \file wavereader.h
 *  \brief Defines operations over wav files' headers.
 */

#include <stdio.h>
#include <stdlib.h>

/*! \struct _wavheader
 *  \brief A struct that stores the header of a WAV file.
 */
struct _wavheader {
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
	char data[5];
	unsigned long subchunk2size;
};

/*! \var typedef struct _wavheader wavheader;
 *  \brief A type definition to represent the WAV header;
 */
typedef struct _wavheader wavheader;

/*! \fn int getHeader(wavheader *wh, FILE *f)
 *  \brief Reads a header from a WAV file.
 *
 *  \param wh The wav header to be initialized.
 *  \param f A file to get the header from.
 */
int getHeader(wavheader *wh, FILE *f);

/*! \fn void printWavHeader(wavheader *wh);
 *  \brief Prints the WAV header.
 *
 *  \param wh The wav header to be printed.
 */
void printWavHeader(wavheader *wh);

#endif
