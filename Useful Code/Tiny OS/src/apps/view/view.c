#include <types.h>
#include <stdio.h>

void
main(int argc, char** argv)
{
	char buf[201];
	FILE* f = fopen(argv[1]);
	if(!f)
		return;

	while(fgets(buf, 200, f))
		printf("%s", buf);

	fclose(f);
}
