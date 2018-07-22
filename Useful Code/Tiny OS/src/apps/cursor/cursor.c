#include <types.h>
#include <sys/io.h>
#include <sys/cons.h>

void
main(int argc, char** argv)
{
	uint start, end;

	if(argc < 3) {
		printf("usage: cursor start end\n");
		exit();
	}

	start = atoi(argv[1]);
	end = atoi(argv[2]);

	if(start > 16 || start < 0) {
		printf("cursor: start parameter is invalid.\n");
		exit();
	}
	if(end > 16 || end < 0) {
		printf("cursor: end parameter is invalid.\n");
		exit();
	}

	outb(CRT_INDEX, CR_CURSLINE1);
	outb(CRT_DATA, start);
	outb(CRT_INDEX, CR_CURSLINE2);
	outb(CRT_DATA, end);	
}
