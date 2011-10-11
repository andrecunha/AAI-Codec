#include <stdlib.h>
#include <string.h>
#include <huffman.h>
#include <priority_queue.h>

int pq_get_priority(NODE_TYPE *node)
{
	return node->frequency;
}

void hf_compute_frequencies (uint32_t *input, unsigned long *output, unsigned long input_length)
{
	int i;
	for(i=0; i<input_length; i++)
		output[input[i]]++;
}


void hf_build_tree (hf_tree_node **result, unsigned long *frequencies, unsigned long length)
{
	priority_queue *queue;
	pq_init(queue, length);

	/* Firstly, we put all the elements in the queue, along with their respective frequency. */
	int i;
	for(i=0; i<length; i++) {
		hf_tree_node *node = malloc(sizeof(hf_tree_node));
		node->frequency = frequencies[i];
		node->left = node->right = NULL;
		node->value = i;
		node->is_leaf = 1;
		pq_insert(queue, node);
	}

	/* Then, we build the tree. */
	while(queue->size > 1) {
		/* First, we extract the two less frequent nodes. */
		hf_tree_node *left = pq_remove(queue);
		hf_tree_node *right = pq_remove(queue);

		/* Then, we join them into one single node. */
		hf_tree_node *new_node = malloc(sizeof(hf_tree_node));

		/* Whose frequency is the sum of the frequencies of the other two. */
		new_node->frequency = left->frequency + right->frequency;
		/* The other two are now children of the new node. */
		new_node->left = left;
		new_node->right = right;
		/* The new node has children, so it is not a leaf node. */
		new_node->is_leaf = 0;

		/* Finally, we insert the new node back in the queue. */
		pq_insert(queue, new_node);
	}

	/* When there is only one node in the queue, so that node contains the whole Huffman tree, and it is returned. */
	*result = pq_remove(queue);
	pq_destroy(queue);
}


void recursive_traverse (hf_tree_node *root, unsigned long level, uint8_t* curr_code, uint8_t* output[])
{
	if(root->is_leaf) {
		/* If we reached a leaf node, we put a terminator at the current code. */
		curr_code[level] = 0xFF;

		/* Then, we compute the length of the current code. */
		int code_length=0;
		while(curr_code[code_length] != 0xFF)
			code_length++;

		/* And we put the code (including the terminator) at the output. */
		output[root->value] = malloc(code_length);
		memcpy(output[root->value], curr_code, code_length);
	} else{
		/*If we did not reach a leaf node, we must go down recursively at the left and right children of the current node.*/

		curr_code[level] = 0;
		recursive_traverse(root->left, level+1, curr_code, output);

		curr_code[level] = 1;
		recursive_traverse(root->right, level+1, curr_code, output);
	}
}

void hf_traverse (hf_tree_node *root, uint8_t* output[])
{
	uint8_t *code = malloc(/*??????*/);
	recursive_traverse(root, code, output);
	free(code);
}


void hf_encode (uint32_t *input, bitbuffer *output, unsigned long length)
{
	/* To encode the input vector, we first calculate the frequencies of the symbols in it.*/
	unsigned long *frequencies = calloc(length, sizeof(unsigned long));
	hf_compute_frequencies(input, frequencies, length);


	/* Then, we build the Huffman tree. */
	hf_tree_node *root;
	hf_build_tree(&root, frequencies, length);

	/* Then, we traverse the tree to obtain the code for each symbol. */
	uint8_t **codes = malloc(length*sizeof(uint8_t*));
	hf_traverse(root, codes);

	int i,j;
	int code_length;
	for (i=0; i<length; i++) {
		code_length = 0;
		while(codes[input[i]][code_length] != 0xFF)
			code_length++;
		code_length--;

		for(j=0; j<code_length; j++)
			bwrite(output, codes[input[i]][j]);
	}
}

void hf_decode (bitbuffer *input, unsigned long *frequencies, uint32_t *output, unsigned long length)
{
	/* To decode the input vector, we need to build the Huffman tree that was used to generate it. */
	hf_tree_node *root, *curr_node;
	hf_build_tree(&root, frequencies, length);
	curr_node = root;

	unsigned long i_output=0;
	uint8_t *b = malloc(1);
	/* While there is more symbols to read. */
	while(!bempty(input)) {
		/* If we did not reach a leaf node yet: */
		if(!curr_node->is_leaf) {
			/* Read the next bit. */
			bread(input, b);
			/* If it is one, go to the right child of the current node. To the contrary, go to the left one. */
			if(*b)
				curr_node = curr_node->right;
			else
				curr_node = curr_node->left;
		} else{
			/* If we reached a leaf node, write the symbol found to the output and return to the root node of the tree. */
			output[i_output++] = curr_node->value;
			curr_node = root;
		}
	}
}

