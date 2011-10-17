#ifndef DELTA_H
#define DELTA_H

/*! \file wavreader.h
 *  \brief Defines operations over wav files' headers.
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>
#include <bitio.h>


/*! \fn void dt_encode(uint32_t *buffer, uint32_t lenght)
 *  \brief Defines operations over wav files' headers.
 *  \param buffer
 *  \param lenght
 */
uint32_t dt_encode(uint32_t *input, bitbuffer *out, uint32_t lenght, uint16_t bits_per_sample, uint32_t first);

/*! \fn dt_decode(uint32_t delta, uint32_t lenght)
 *  \brief Defines operations over wav files' headers.
 *  \param delta
 *  \param lenght
 */
void dt_decode(bitbuffer *input, uint32_t maxbit, uint32_t *output, uint32_t length, uint16_t bits_per_sample, uint32_t first);

void dt_decode_int(int32_t *input, uint32_t *output,uint32_t length, uint32_t first);

#endif
