#include <stdio.h>
#include <types.h>

static uchar dummy[50] = {0};

void main(int argc, char** argv)
{
	unsigned short* p = (void*)0x200000;
	ulong c;
	ulong loop;
	int pos;

	if(argc < 3) {
		printf("usage: blink loop[*1000] pos\n");
		exit(1);
	}

	loop = atoi(argv[1]);
	pos = atoi(argv[2]);

	printf("params: loop %d, pos %d\n", loop, pos);

	while(1) {
		c = loop*1000;
		while(c--);
		p[pos]++;
	}
}

