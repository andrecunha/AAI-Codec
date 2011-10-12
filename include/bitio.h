#ifndef BITIO_H
#define BITIO_H

/*! \file bitio.h
 *  \brief Defines a FIFO bit buffer, used for general purpose bitwise storing operations.
 */

#include <stdio.h>
#include <stdint.h>

/*! \struct _bitbuffer
 *  \brief A struct that stores a FIFO bit buffer.
 */
struct _bitbuffer {
	size_t size; /*!< The number of uint8_ts allocated for "data". */
	unsigned long n_bytes; /*!< The effective number of uint8_ts stored in the buffer (including the possibly incomplete first and last uint8_ts). */	
    unsigned int bits_last; /*!< The number of bits effectively used in the last uint8_t. */
	unsigned int bits_offset; /*!< The number of bits to be skipped in the first uint8_t. */
	uint8_t *data, *original_data; /*!< The data itself. */
};

/*! \var typedef struct _bitbuffer bitbuffer
 *  \brief A type definition to represent the FIFO bit buffer.
 */
typedef struct _bitbuffer bitbuffer;

/*! \fn int binit(bitbuffer *b, unsigned long size)
 *  \brief Initializes a bit buffer.
 *  
 *  \param b The bit buffer.
 *  \param size The maximum size of the buffer (the ammount of memory to be allocated).
 *  \return 0 if the buffer could be allocated; 1 otherwise.
 */
int binit(bitbuffer *b, size_t size);

/*! \fn int bwrite(bitbuffer *b, uint8_t data)
 *  \brief Write a bit at the end of the bit buffer.
 *
 *  \param b The bit buffer where the bit is to be stored.
 *  \param data The bit to the stored.
 *
 *  \warning "data" should be 0 or 1.
 *
 *  \return 0 on success; 1 otherwise.
 */
int bwrite(bitbuffer *b, uint8_t data);

/*! \fn int bread(bitbuffer *b, uint8_t *out)
 *  \brief Reads a bit from the beginning of the bit buffer.
 *
 *  \param b The bit buffer to be read.
 *  \param out Where the read bit will be stored.
 *
 *  \return 0 on success; 1 otherwise.
 */
int bread(bitbuffer *b, uint8_t *out);

/*! 
 * \fn void bwritev(bitbuffer b, uint32_t data, unsigned int count)
 * \breaf Writes a vector to the bit buffer.
 *
 * \param b The bit buffer where the data will be stored.
 * \param data The data to be stored.
 * \param count The number of bits from data that should be stored.
 *
 */
void bwritev(bitbuffer *b, uint32_t data, unsigned int count);

/*! 
 * \fn int breadv(bitbuffer b, uint32_t *output, unsigned int count)
 * \breaf Reads a vector from the bit buffer.
 *
 * \param b The bit buffer from which the data will be read.
 * \param output Where the data will be stored.
 * \param count The number of bits from the bit buffer that will be read and stored in "output".
 *
 * \return The number of bits that was read.
 */
 int breadv(bitbuffer *b, uint32_t *output, unsigned int count);

/*! \fn int bflush(bitbuffer *b, FILE *f)
 *  \brief Flushes the buffer to a file.
 *
 *  \param b The bit buffer to be flushed.
 *  \param f The file where the buffer will be flushed to.
 *
 *  \warning Do not use it after bread() of bit buffer b.
 *
 *  \return 0 on success; 1 otherwise.
 */
int bflush(bitbuffer *b, FILE *f);

/*! \fn int bget(bitbuffer *b, FILE *f)
 *
 *  \param b The bit buffer to be flushed.
 *  \param f The file where the buffer will be flushed to.
 *
 * \return 0 on success; 1 otherwise.
 */
int bget(bitbuffer *b, FILE *f);

/*! \fn void bprint(bitbuffer *b)
 *  \brief Prints the buffer to the standard output. Useful for debugging.
 *
 *  \param b The bit buffer to be printed out.
 */
void bprint(bitbuffer *b);

/*! \fn void bdestroy(bitbuffer *b)
 *  \brief Deallocates the allocated memory for the buffer.
 *
 *  \param b The bit buffer to the freed.
 */
void bdestroy(bitbuffer *b);

/*! \fn int bfull(bitbuffer *b)
 *  \brief Verifies whether the buffer is full.
 *
 *  \param b The bit buffer.
 *  
 *  \return 1 if "b" is full; 0 otherwise.
 */
int bfull(bitbuffer *b);

/*! \fn int bempty(bitbuffer *b)
 *  \brief Verifies whether the buffer is empty.
 *
 *  \param b The bit buffer.
 *  
 *  \return 1 if "b" is empty; 0 otherwise.
 */
int bempty(bitbuffer *b);

#endif /* BITIO_H */
