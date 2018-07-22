//
// Trio64V+ Test
//
#include <stdio.h>

extern int TRIO_Probe();

void main()
{
	if(TRIO_Probe())
   	printf("Probe discovered a S3 Trio64V+ Card!");
};
