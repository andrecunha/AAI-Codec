#include <bitio.h>
#include <stdio.h>

int main (int argc, char* argv[])
{
	FILE *fp_in, *fp_out;
	
	if((fp_out = fopen("test_out.txt", "wb")) == NULL) return 1;		
	bitbuffer *b = malloc(sizeof(bitbuffer));
	binit(b, 2);
	
	int i;
	for(i=0; i<14; i++)
		if(i<8)
			bwrite(b, 0);
		else
			bwrite(b, 1);

	bprint(b);
	

	bflush(b, fp_out);
	fclose(fp_out);
	
	if((fp_in = fopen("test_out.txt", "rb"))==NULL) return 1;

	bitbuffer *bin = malloc(sizeof(bitbuffer));
	bget(bin, fp_in);
	bprint(b);
	bprint(bin);

	byte d;
	while(!bempty(bin)){
		bread(bin,&d);
		printf("%x\n", d);
	}
	fclose(fp_in);

	bdestroy(bin);
	bdestroy(b);
	free(bin);
	free(b);

	return 0;
}
