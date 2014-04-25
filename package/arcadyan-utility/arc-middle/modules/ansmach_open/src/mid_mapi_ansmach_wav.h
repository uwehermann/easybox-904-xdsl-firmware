/*
 * Arcadyan middle layer header file of APIs for Answering Machine Wav file convert
 *
 * Copyright 2012, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _MID_MAPI_ANSMACHINE_WAV_H_
#define _MID_MAPI_ANSMACHINE_WAV_H_

#include "mid_types.h"
/*
typedef unsigned char	U8;
typedef unsigned short	U16;
typedef unsigned int	U32;
*/

#pragma pack(1)		/* set alignment to 1 byte boundary */
					/* so that sizeof(struct FORMAT_CHUNK)=26, instead of 28 */

struct RIFF_CHUNK
{
	U8		ckID[4];	//"RIFF"
	U32		ckSize;		//4 + 26 + 12 + (8 + M * Nc * Ns + (0 or 1)) = 50 + 1 * 1 * Ns + 0 = 50 + Ns
	U8		WAVEID[4];	//"WAVE"
};

struct FORMAT_CHUNK
{
	U8		ckID[4];	//"fmt "
	U32		ckSize;		//18
	U16		wFormatTag;	//Format Code = 0x0006 (WAVE_FORMAT_ALAW) (8-bit ITU-T G.711 A-law)
	U16		nChannels;	//Nc = 1
	U32		nSamplesPerSec;		//F = 8000
	U32		nAvgBytesPerSec;	//F * M * Nc = 8000 * 1 * 1 = 8000
	U16		nBlockAlign;	//M * Nc = 1 * 1 = 1
	U16		wBitsPerSample;	//8 * M = 8 * 1 = 8
	U16		cbSize;			//0
};

struct FACT_CHUNK
{
	U8		ckID[4];	//"fact"
	U32		ckSize;		//4
	U32		dwSampleLength;	//Nc * Ns
};

struct DATA_CHUNK
{
	U8		ckID[4];	//"data"
	U32		ckSize;		//M * Nc * Ns
	//char	*sampled_data;	//original *.711 data
	//U8	pad;
};

struct WAVE_HEADER	//total: 58 bytes
{
	struct RIFF_CHUNK		riff_chunk;
	struct FORMAT_CHUNK	fmt_chunk;
	struct FACT_CHUNK		fact_chunk;
	struct DATA_CHUNK		data_chunk;
};

typedef union {
	int i;
	char c[4];
} EndianTest;


/************************/
/* G.711 to Wav convert */
/************************/
extern int G7112wav(char *g711_file, char *wav_file);

#endif // _MID_MAPI_ANSMACHINE_WAV_H_
