#ifndef HUFFMAN_H
#define HUFFMAN_H

/*! \file huffman.h
 *  \brief The main file for Huffman encoding/decoding.
 */

#include <stdint.h>
#include <bitio.h>

/*! \struct _hf_tree_node
 *  \brief A struct that represents a node in the Huffman tree.
 *
 */
struct _hf_tree_node {
	uint64_t frequency; /*!< The frequency associated with the current node. */
	struct _hf_tree_node 	*left,  /*!< The left child of this node. */
		     		*right; /*!< The right child of this node.. */
	uint32_t value; /*!< If this is a leaf node, stores que value associated with the node. */
	int is_leaf; /*!< Whether this is a leaf node. */
};

typedef struct _hf_tree_node hf_tree_node;

/*! \fn void hf_compute_frequencies (uint32_t *input, uint64_t *output, uint32_t input_length)
 *  \brief Reads input vector, computes the frequency of each symbol in it, and stores the results in the output vector.
 *
 * \param input The input vector.
 * \param output The output vector.
 * \param input_length The number of elements to be read in "input".
 *
 *  \warning "output" is assumed to be large enough to store all the frequencies, and all of its elements are assumed to be zero.
 *
 */
void hf_compute_frequencies (uint32_t *input, uint64_t *output, uint32_t input_length);

/*! \fn void hf_build_tree (hf_tree_node *root, uint64_t *frequencies, unsigned long length)
 *  \brief Builds the Huffman tree corresponding to the given frequencies.
 *
 *  \param result The root node of the built tree.
 *  \param frequencies A vector containing the frequencies of each symbol.
 *  \param length The number of elements in "frequencies".
 */
void hf_build_tree (hf_tree_node **result, uint64_t *frequencies, unsigned long length);

/*! \fn uint8_t* hf_traverse (hf_tree_node *root, uint8_t* output[])
 *  \brief Traverses a Huffman tree, and returns the code corresponding to all the symbols.
 *
 *  \param root The root node of the tree.
 *  \param output The Huffman code for each symbol in "root".
 *  \param length The maximum length of the a code (can be the length of the frequency vector).
 *
 */
void hf_traverse (hf_tree_node *root, uint8_t* output[], unsigned long length);

/*! \fn void hf_encode (uint32_t *input, bitbuffer *output, unsigned long input_length, unsigned long frequency_length)
 *  \brief Encodes the input vector, storing the result in the output bit buffer.
 *
 *  \param input The input vector.
 *  \param output The output bif buffer.
 *  \param input_length The length of the input vector.
 *  \param frequency_length The length of the frequency vector (the number of different symbols in the input).
 *
 *  \return The Huffman tree generated during the encoding process.
 */

void hf_encode (uint32_t *input, bitbuffer *output, unsigned long input_length, unsigned long frequency_length);

/*! \fn void hf_decode (bitbuffer *input, uint64_t frequencies, uint32_t *output, unsigned long length);
 *  \brief Decodes the bit sequence stored in the input bit buffer, and stores the result in the output vector.
 *
 *  \param input The input bit sequence.
 *  \param frequencies The frequency vector for the data.
 *  \param output The output vector.
 *  \param frequency_length The length of the frequency vector.
 *
 *  \warning "output" is assumed to be large enough to store all the uncompressed data.
 */
void hf_decode (bitbuffer *input, uint64_t *frequencies, uint32_t *output, unsigned long frequency_length);

/*! \fn void hf_print_tree (hf_tree_node *tree, unsigned long level)
 *  \brief Prints a Huffman tree. Useful for debugging.
 *
 *  \param tree The tree to print out.
 *  \param level Used internally. Call with 0.
 *
 */
void hf_print_tree (hf_tree_node *tree, unsigned long level);

/*! \fn void hf_destroy_tree (hf_tree_node *root)
 *  \brief Deallocates a Huffman tree.
 *
 *  \param root The tree to destroy.
 *
 */
void hf_destroy_tree (hf_tree_node *root);

#endif /* HUFFMAN_H */
