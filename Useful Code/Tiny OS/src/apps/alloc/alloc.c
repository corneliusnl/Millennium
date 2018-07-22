#include <types.h>
#include <stdio.h>

void
main(int argc, char** argv)
{
	int c;
	int count, sz;

	printf("dumping args...(%d)...\n", argc);

	for(c = 0; c < argc; c++)
		printf("arg[%d]: %s\n", c, argv[c]);

	count = atoi(argv[1]);
	sz = atoi(argv[2]);

	for(c = 0; c < count; c++) {
		uchar* ptr;
		int k;

		printf("calling malloc()...");
		ptr = (uchar*)malloc(sz);
		printf("testing (ptr = %x)...", (ulong)ptr);
		for(k = 0; k < sz; k++)
			*ptr++ = 0xFA;
		printf("ok\n");
	}
}
