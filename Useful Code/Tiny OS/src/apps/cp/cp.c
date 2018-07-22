#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 4096
static char buf[BUF_SIZE];

void
main(int argc, char** argv)
{
	int in, out;
	char* src = argv[1];
	char* dst = argv[2];

	if(argc < 3) {
		printf("usage: cp src dest.\n");
		exit(0);
	}

	in = open(src, O_RDONLY);
	if(in == -1) {
		printf("open src failed.\n");
		return;
	}

	out = open(dst, O_CREAT);
	if(out == -1) {
		printf("open dest failed.\n");
		return;
	}

	while(1) {
		int rb = read(in, buf, BUF_SIZE);

		if(rb <= 0) {
			//printf("done.\n");
			break;
		}

		if(write(out, buf, rb) < 0) {
			printf("write failed\n");
			break;
		}
	}
	
	close(in);
	close(out);
}

