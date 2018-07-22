#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define BACKSPACE	8
#define MAX_LINE	100

void
main(int argc, char** argv)
{
	char line[MAX_LINE];
	int h, n;
	char* p;
	char* file;

	if(argc < 2) {
		printf("specify filename\n");
		exit(1);
	}
	file = argv[1];

	h = open(file, O_CREAT);
	if(h == -1) {
		printf("file creation failed.\n");
		return;
	}

	p = line;
	n = 0;
	while(1) {
		char ch = getch();
		if(!ch)
			continue;
		if(ch == '.') {
			break;
		}
		if(ch == 0x0D) {
			putc('\n');
			*p = 0x0D;
			n++;
			if(write(h, line, n) < 0) {
				printf("write failed\n");
				break;
			}
			p = line;
			n = 0;
			continue;
		}
         	if(ch == BACKSPACE) {
            		if( n ) {
               			ungetc();
               			n--;
				p--;
            		}
            		continue;
         	}
		*p++ = ch;
		putc(ch);
		if(++n >= MAX_LINE) {
			printf("buffer overflow\n");
			break;
		}
	}
	close(h);
}

