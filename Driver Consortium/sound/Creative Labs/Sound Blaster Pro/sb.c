#include <stdio.h>
#include "dsp.h"

void main()
{
//	DSP_Initialize();
//	printf("DSP Version: %d\n", DSP_GetVersion);

	SB_SetBase(0x220);
	SB_SetIRQ(7);
	SB_SetDMA(1);
	SB_Initialize();
	SB_SpeakerOn(); // DAC Speaker
	SB_Play(DoubleBuffer);

	while(!GetCh());

	SB_Stop();
	SB_SpeakerOff(); // DAC Speaker
	SB_Terminate();
};

//
// int SB_Probe()
//
// Description
//
// Parameters
//
// Returns
//
int SB_Probe()
{
};s