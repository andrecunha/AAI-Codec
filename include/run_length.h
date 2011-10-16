#ifndef RUN_LENGTH_H
#define RUN_LENGTH_H

/*! \file run_length.h
 *  \brief The main file for run_length encoding/decoding.
 */

void rl_encode_var(bitbuffer *input, bitbuffer *output,
        uint16_t bits_per_sample, uint32_t *nbits_run, uint32_t *nbits_code);

/*! \fn void rl_encode(bitbuffer *input, bitbuffer *output)
 *  \brief Reads input bifbuffer, finds the best run size and encode.
 *
 *  \param input The input bifbuffer.
 *  \param output The output bitbuffer.
 */
void rl_encode(bitbuffer *input, bitbuffer *output, uint16_t bits_per_sample, uint32_t *nbits_run, uint32_t *nbits_code);

/*! \fn uint32_t rl_encode_nbits(uint8_t nbits, uint32_t *input, bitbuffer *output, uint32_t input_length);
 *  \brief Encodes a uint32_t vector with an specific size of code.
 *
 *  \param nbits Code number of bits.
 *  \param input Input vector of codes.
 *  \param output Output encoded.
 *  \param input_length Size of the input vector.
 *
 *  \return The number of bits used to represent the run.
 */
uint32_t rl_encode_nbits(uint8_t nbits, uint32_t *input, 
                    bitbuffer *output, uint32_t input_length);

void print_encoded(bitbuffer *input, uint32_t nbits_run, uint32_t nbits_code);

/*! \fn uint32_t find_longest_run(uint32_t *input, uint32_t input_length)
 *  \brief Finds the longest run in a vector of codes.
 *
 *  \param input The input vector of codes
 *  \param input_length Size of the input vector.
 *
 *  \return The size of the longest_run.
 */
uint32_t find_longest_run(uint32_t *input, uint32_t input_length);

/*! \fn void rl_decode(uint16_t bits_per_sample, uint32_t nbits_code, uint32_t nbits_run, bitbuffer *input, bitbuffer *output)
 *  \brief Decodes the input bitbuffer using run length.
 *
 *  \param bits_per_sample The original number of bits per sample.
 *  \param nbits_code The number of bits used to store the codes.
 *  \param nbits_run The number of bits used to store the run.
 *  \param input The input bitbuffer.
 *  \param output The decoded bifbuffer.
 */
void rl_decode(uint16_t bits_per_sample, 
            uint32_t nbits_code, uint32_t nbits_run, 
            bitbuffer *input, bitbuffer *output);

void rl_decode_var(uint16_t bits_per_sample, 
                    bitbuffer *input, bitbuffer *output);
#endif
