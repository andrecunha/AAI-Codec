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
void dt_encode(uint32_t *buffer, uint32_t lenght);



/*! \fn dt_decode(uint32_t delta, uint32_t lenght)
 *  \brief Defines operations over wav files' headers.
 *  \param delta
 *  \param lenght
 */
void dt_decode(uint32_t delta, uint32_t lenght);

#endif
