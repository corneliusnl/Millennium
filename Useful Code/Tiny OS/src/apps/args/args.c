#include <types.h>
#include <stdio.h>

void
main(int argc, char** argv)
{
	int c;
	printf("dumping args...(%d)...\n", argc);

	for(c = 0; c < argc; c++)
		printf("arg[%d]: %s\n", c, argv[c]);

	exit(10);
}
