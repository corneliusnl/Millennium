#include <types.h>

int dec = 1200;
int hex = 0xFE;
float flo = 3.14;

char buf2[200];

void 
main(int argc, char** argv)
{	
	int h;
	char buf[201] = "";

	if( chdir("../etc") <= 0 ) {	
		printf("chdir failed.\n");
		exit();
	}

	h = open("motd");
	if(h <= 0) {
		printf("open failed.\n");
		exit();
	}

	if(read(h, buf, 200) < 0) {
		printf("read failed.\n");
	}
        close(h);

	//printf("%s\n", buf);
	//conswrite(buf);
	printf(buf);
     

	//sprintf(buf2, "some tests: %10d, %4x\n", dec, hex);
	//conswrite(buf2);
	//write(-2, buf2, 199);
}

