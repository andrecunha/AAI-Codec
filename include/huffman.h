#ifndef HUFFMAN_H
#define HUFFMAN_H

/*! \file huffman.h
 *  \brief The main file for Huffman encoding/decoding.
 */

#include <bitio.h>
#include <pq_definitions.h>

/*! \struct _hf_tree_node
 *  \brief A struct that represents a node in the Huffman tree.
 *
 */
struct _hf_tree_node {
	unsigned long frequency; /*!< The frequency associated with the current node. */
	hf_tree_node *left,  /*!< The left child of this node. */
		     *right; /*!< The right child of this node.. */
	uint32_t value; /*!< If this is a leaf node, stores que value associated with the node. */
	int is_leaf; /*!< Whether this is a leaf node. */
};

typedef struct _hf_tree_node hf_tree_node;

/*! \fn void hf_compute_frequencies (uint32_t *input, unsigned long *output, uint32_t input_length)
 *  \brief Reads input vector, computes the frequency of each symbol in it, and stores the results in the output vector.
 *
 * \param input The input vector.
 * \param output The output vector.
 * \param input_length The number of elements to be read in "input".
 *
 *  \warning "output" is assumed to be large enough to store all the frequencies, and all of its elements are assumed to be zero.
 *
 *
 */
void hf_compute_frequencies (uint32_t *input, unsigned long *output, unsigned long input_length);

/*! \fn void hf_build_tree (hf_tree_node *root, unsigned long *frequencies, unsigned long length)
 *  \brief Builds the Huffman tree corresponding to the given frequencies.
 *
 *  \param result The root node of the built tree.
 *  \param frequencies A vector containing the frequencies of each symbol.
 *  \param length The number of elements in "frequencies".
 */
void hf_build_tree (hf_tree_node **result, unsigned long *frequencies, unsigned long length);

/*! \fn uint8_t* hf_traverse (hf_tree_node *root, uint8_t* output[])
 *  \brief Traverses a Huffman tree, and returns the code corresponding to all the symbols.
 *
 *  \param root The root node of the tree.
 *  \param output The Huffman code for each symbol in "root".
 *
 */
void hf_traverse (hf_tree_node *root, uint8_t* output[]);

/*! \fn hf_tree_node* hf_encode (uint32_t *input, bitbuffer *output)
 *  \brief Encodes the input vector, storing the result in the output bit buffer.
 *
 *  \param input The input vector.
 *  \param output The output bif buffer.
 *  \param length The length of the input buffer.
 *
 *  \return The Huffman tree generated during the encoding process.
 */
void hf_encode (uint32_t *input, bitbuffer *output, unsigned long length);

/*! \fn void hf_decode (bitbuffer *input, hf_tree_node *tree, uint32_t *output, unsigned long length);
 *  \brief Decodes the bit sequence stored in the input bit buffer, and stores the result in the output vector.
 *
 *  \param input The input bit sequence.
 *  \param frequencies The frequency vector for the data.
 *  \param output The output vector.
 *  \param length The length of the frequency vector.
 *
 *  \warning "output" is assumed to be large enough to store all the uncompressed data.
 */
void hf_decode (bitbuffer *input, unsigned long *frequencies, uint32_t *output, unsigned long length);

#endif /* HUFFMAN_H */
