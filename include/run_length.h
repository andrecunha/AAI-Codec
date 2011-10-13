#ifndef RUN_LENGTH_H
#define RUN_LENGTH_H

void rl_encode(bitbuffer *input, bitbuffer *output);

uint32_t rl_encode_nbits(uint8_t nbits, uint32_t *input, 
                    bitbuffer *output, uint32_t input_length);

uint32_t find_longest_run(uint32_t *input, uint32_t input_length);

void rl_decode(uint16_t bits_per_sample, 
            uint32_t nbits_code, uint32_t nbits_run, 
            bitbuffer *input, bitbuffer *output);

#endif
