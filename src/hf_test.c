#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <huffman.h>

int main(int argc, char* argv[])
{
	uint32_t values[12] = {0,0,1,1,1,1,1,1,1,2,2,2};
	uint64_t frequencies[3] = {0,0,0};

	printf("Values: [0,0,1,1,1,1,1,1,1,2,2,2]\n");

	hf_compute_frequencies(values, frequencies, 12);

	printf("Frequencies: ");
	int i;
	for(i=0; i<3; i++)
		printf("%u ", frequencies[i]);
	printf("\n");

	hf_tree_node *root;
	hf_build_tree(&root, frequencies, 3);

	hf_print_tree(root, 0);
	uint8_t **codes = malloc(3*sizeof(uint8_t *));
	hf_traverse(root, codes, 3);

	printf("codes[0] = [%x %x %x]\n", codes[0][0], codes[0][1], codes[0][2]);
	printf("codes[1] = [%x %x %x]\n", codes[1][0], codes[1][1], codes[1][2]);
	printf("codes[2] = [%x %x %x]\n", codes[2][0], codes[2][1], codes[2][2]);

	bitbuffer *buffer = malloc(sizeof(bitbuffer));
	binit(buffer, 500);
	
	/*int j;
	int code_length;
	for (i=0; i<12; i++) {
		code_length = 0;
		while(codes[values[i]][code_length] != 0xFF)
			code_length++;
		printf("code_length: %d\n", code_length);

		for(j=0; j<code_length; j++)
			bwrite(buffer, codes[values[i]][j]);
	}
	*/
	
	hf_encode(values, buffer, 12, 3);
	bprint(buffer);

	uint32_t output[12];
	hf_decode(buffer, frequencies, output, 3);

	printf("Decoded: ");
	for(i=0; i<12; i++)
		printf("%d ", output[i]);
	printf("\n");

	bdestroy(buffer);

	return 0;
}
