#include <bitio.h>

int main (int argc, char* argv[])
{
	bitbuffer *b = malloc(sizeof(bitbuffer));
	
	binit(b, 2);

	int i;
	for(i=0; i<14; i++)
		if(i<8)
			bwrite(b, 0);
		else
			bwrite(b, 1);

	bprint(b);

	byte d;
	while(!bempty(b)){
		bread(b,&d);
		printf("%x\n", d);
	}

	bdestroy(b);
	free(b);

	return 0;
}
