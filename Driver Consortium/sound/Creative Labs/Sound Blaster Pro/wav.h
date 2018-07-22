#ifndef KERNEL_WAV_H
#define KERNEL_WAV_H
//
// Waveform
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//

//
// Wave chunk
//
typedef struct
{
	char	ChunkId[4];
	long	ChunkSize;
} WAVCHUNK;

//
// Wavefile structure
//
typedef struct
{
	char		Riff[4];
	long		FileSize;
	char		RiffType[4];
	WAVCHUNK	Format;
	short		FormatTag;
	short		Channels;
	long		SamplesPerSec;
	long		AvgBytesPerSec;
	short		BlockAlign;
	short		BitsPerSample;
	WAVCHUNK	Data;
} WAVEFILE;

#endif
