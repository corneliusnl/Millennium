#include <stdio.h>
#include "wav.h"

void main()
{
	FILE	*fp;
	WAVEFILE	Wave;

	fp = fopen("sample.wav", "rb");
	fread(&Wave, sizeof(WAVEFILE), 1, fp);

	fclose(fp);
};