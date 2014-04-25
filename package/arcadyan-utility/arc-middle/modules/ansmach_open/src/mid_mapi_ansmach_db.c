/*
 * Arcadyan middle layer header file of APIs for Answering Machine Database
 *
 * Copyright 2012, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h> // should be removed.
#include <fcntl.h> // should be removed.
#include "mid_mapi_ansmach_db.h"

#ifdef _TEST_MODE_

char *gFlashData = NULL;

#endif

static char isInited = 0x0;


stAnsMachGmsgDBStatus g_gmsgStatus = {0};
stAnsMachCmsgDBStatus g_cmsgStatus = {0};

//stAnsMachDBCmsgHdr g_cmsgHdr = {0};
//stAnsMachDBGmsgHdr g_gmsgHdr = {0};

char g_cmsgHdr[512] = {0};
char g_gmsgHdr[512] = {0};

char * g_cmsgData = NULL;

int mapi_ansmach_isTestMode(void)
{
#ifdef _TEST_MODE_
	FILE *fp;

	fp = fopen(_TEST_MODE_DB_ISINIT_FILE, "r");
	if( fp == NULL )
		return ANSMACH_FALSE;
	else
		return ANSMACH_TRUE;
#else
	return ANSMACH_FALSE;
#endif
}

AnsMachRet mapi_ansmach_init(void)
{
	FILE *fp;

	//printf("isInited: %d\n", isInited);

	if( isInited )
		return ANSMACH_RET_SUCCESS;

	if( mapi_ansmach_isTestMode() )
	{
		//printf("Enter test mode\n");

		fp = fopen(_TEST_MODE_DB_GMSGSTA_PATH, "r");
		fread((char *)&g_gmsgStatus, 1, sizeof(stAnsMachGmsgDBStatus), fp);
		fclose(fp);

		fp = fopen(_TEST_MODE_DB_CMSGSTA_PATH, "r");
		fread((char *)&g_cmsgStatus, 1, sizeof(stAnsMachCmsgDBStatus), fp);
		fclose(fp);

		if( g_gmsgStatus.count != 0 )
		{
			fp = fopen(_TEST_MODE_DB_GMSGHDR_PATH, "r");
			fread(g_gmsgHdr, 1, (g_gmsgStatus.count*sizeof(stAnsMachDBGmsgHdr)), fp);
			fclose(fp);
		}
		else
		{
			memset(g_gmsgHdr, 0, sizeof(g_gmsgHdr));
		}

		if( g_cmsgStatus.count != 0 )
		{
			fp = fopen(_TEST_MODE_DB_CMSGHDR_PATH, "r");
			fread(g_cmsgHdr, 1, (g_cmsgStatus.count*sizeof(stAnsMachDBCmsgHdr)), fp);
			fclose(fp);

			//fp = fopen(_TEST_MODE_DB_CMSGDATA_PATH, "r");
			//fread(g_cmsgHdr, 1, (g_cmsgStatus.count*sizeof(stAnsMachDBCmsgHdr)), fp);
			//fclose(fp);
		}
		else
		{
			memset(g_cmsgHdr, 0, sizeof(g_cmsgHdr));
		}

#if 0
		printf("Size: %d\n", _TEST_MODE_MAX_DATA_*sizeof(stAnsMachDBCmsgHdr));

		gFlashData = (char *)malloc(_TEST_MODE_MAX_DATA_*sizeof(stAnsMachDBCmsgHdr));

		if( gFlashData == NULL )
		{
			printf("gFlashData is NULL\n");
		}

		g_cmsgStatus.count = 0;
		mapi_ansmach_cmsgdb_save((stAnsMachDBCmsgHdr *)&TDBhdr[0], "/tmp/ivr/voice_data/ZERO.711");
		mapi_ansmach_cmsgdb_save(TDBhdr[1], "/tmp/ivr/voice_data/ZERO.711");
		mapi_ansmach_cmsgdb_save(TDBhdr[2], "/tmp/ivr/voice_data/ZERO.711");
		mapi_ansmach_cmsgdb_save(TDBhdr[3], "/tmp/ivr/voice_data/ZERO.711");

		g_gmsgStatus.count = 1;
		g_gmsgStatus.max = 1;

		g_cmsgStatus.count = 1;
		g_cmsgStatus.max = 1;
		g_cmsgHdr.mid = 0;
		sprintf(g_cmsgHdr.when, "20120811102232");
		sprintf(g_cmsgHdr.caller, "0922345789");
		sprintf(g_cmsgHdr.callee, "035357000");
		g_cmsgHdr.unread = VOICE_MAIL_UNREAD;
		// /tmp/ivr/voice_files/PLEASE_WAIT.711
		g_cmsgHdr.size = 22196;
#endif
		printf("Init Test DB OK\n");
	}
	else
	{
		printf("Not implemented\n");
		// not impelment
	}

	isInited = AnsMach_TRUE;

	return ANSMACH_RET_SUCCESS;
}

int mapi_ansmach_cmsgdb_hdr_getCount(void)
{
	mapi_ansmach_init();

	return g_cmsgStatus.count;
}

int mapi_ansmach_cmsgdb_hdr_get(int idx, stAnsMachDBCmsgHdr* hdr)
{
	int mid = 0;
	stAnsMachDBCmsgHdr* ptr = (stAnsMachDBCmsgHdr *)g_cmsgHdr;

	if( hdr == NULL )
	{
		printf("Invalide parameter: hdr is null\n");
		return ANSMACH_RET_INVALID_PARAM;;
	}

	if( idx < 0 )
	{
		printf("Invalid parameter: idx is less than 0\n");
		return ANSMACH_RET_INVALID_PARAM;;
	}

	mapi_ansmach_init();

	for( mid=0; mid<g_cmsgStatus.count; mid++)
	{
		if( ptr->mid == idx )
		{
			memcpy(hdr, ptr, sizeof(stAnsMachDBCmsgHdr));
			break;
		}

		ptr++;
	}

	return ANSMACH_RET_SUCCESS;
}

int mapi_ansmach_cmsgdb_save(stAnsMachDBCmsgHdr* hdr, char* filepath)
{
	FILE* fp;
	int fd;
	struct stat filestat;
	char *fdata;
	stAnsMachDBCmsgHdr* ptr;

	if( hdr == NULL )
	{
		printf("Invalide parameter: hdr is null\n");
		return ANSMACH_RET_INVALID_PARAM;
	}

	if( filepath == NULL )
	{
		printf("Invalide parameter: filepath is null\n");
		return ANSMACH_RET_INVALID_PARAM;
	}

	mapi_ansmach_init();

	ptr = (stAnsMachDBCmsgHdr *)g_cmsgHdr;
	//printf("g_cmsgHdr addr: 0x%x\n", g_cmsgHdr);
	//printf("1. ptr addr: 0x%x\n", ptr);
	ptr += g_cmsgStatus.count;
	//printf("2. ptr addr: 0x%x\n", ptr);
	memcpy(ptr, hdr, sizeof(stAnsMachDBCmsgHdr));
	ptr->mid = g_cmsgStatus.count;

	//g_cmsgStatus.max = 1;

	printf("file size: %d\n", ptr->size);
	fdata = (char *)malloc(ptr->size);
	fp = fopen(filepath, "rb");
	if( fp == NULL )
	{
		printf("Invalide parameter: %s is not exist\n", filepath);
		free(fdata);
		return ANSMACH_RET_INVALID_PARAM;
	}
	else
	{
		fread(fdata, 1, ptr->size, fp);
		fclose(fp);
	}

	fd = open(_TEST_MODE_DB_CMSGDATA_PATH, O_RDONLY);
	fstat (fd,&filestat);
	g_cmsgData = (char *)malloc(filestat.st_size * sizeof(char)+ptr->size);
	read(fd, g_cmsgData, filestat.st_size);
	close(fd);

	memcpy((g_cmsgData+filestat.st_size), fdata, ptr->size);
	free(fdata);

	fp = fopen(_TEST_MODE_DB_CMSGDATA_PATH, "wb");
	if( fp == NULL )
	{
		printf("create %s faile\n", _TEST_MODE_DB_CMSGDATA_PATH);
		free(g_cmsgData);
		return ANSMACH_RET_FAIL;
	}
	else
	{
		fwrite(g_cmsgData, 1, (filestat.st_size * sizeof(char)+ptr->size), fp);
		fclose(fp);
		free(g_cmsgData);
	}

//	g_cmsgStatus.count++;

	// We should call save to flash here
	fp = fopen(_TEST_MODE_DB_CMSGHDR_PATH, "wb");
	if( fp == NULL )
	{
		printf("create csta.bin fail\n");
		return ANSMACH_RET_FAIL;
	}
	else
	{
		fwrite(g_cmsgHdr, 1, sizeof(g_cmsgHdr), fp);
		fclose(fp);
	}

	g_cmsgStatus.count++;

	fp = fopen(_TEST_MODE_DB_CMSGSTA_PATH, "wb");
	if( fp == NULL )
	{
		printf("create csta.bin fail\n");
		return ANSMACH_RET_FAIL;
	}
	else
	{
		fwrite((char *)&g_cmsgStatus, 1, sizeof(g_cmsgStatus), fp);
		fclose(fp);
	}

	return ANSMACH_RET_SUCCESS;
}

int mapi_ansmach_cmsgdb_get(int idx, char* filepath)
{
	int mid = 0;
	int totalsize = 0;
	FILE* fp;
	char* fdata = NULL;
	stAnsMachDBCmsgHdr* ptr = (stAnsMachDBCmsgHdr *)g_cmsgHdr;

	if( idx < 0 )
	{
		printf("Invalid parameter: idx is less than 0\n");
		return ANSMACH_RET_INVALID_PARAM;;
	}

	if( filepath == NULL )
	{
		printf("Invalide parameter: filepath is null\n");
		return ANSMACH_RET_INVALID_PARAM;
	}

	mapi_ansmach_init();

	for( mid=0; mid<g_cmsgStatus.count; mid++)
	{
		if( ptr->mid == idx )
		{
			//memcpy(hdr, ptr, sizeof(stAnsMachDBCmsgHdr));
			fdata = (char *)malloc(ptr->size);
			fp = fopen(_TEST_MODE_DB_CMSGDATA_PATH, "rb");
			fseek(fp, totalsize, SEEK_SET);
			fread(fdata, 1, ptr->size, fp);
			fclose(fp);
			sprintf(filepath, "/tmp/voicemail/%s.711", ptr->when);
			printf("A. File: %s\n", filepath);
			fp = fopen(filepath, "wb");
			fwrite(fdata, 1, ptr->size, fp);
			fclose(fp);
			free(fdata);
			break;
		}
		else
		{
			totalsize += ptr->size;
		}

		ptr++;
	}

	return ANSMACH_RET_SUCCESS;
}

int mapi_ansmach_cmsgdb_get_by_time(char *time, char* filepath)
{
	int mid = 0;
	int totalsize = 0;
	FILE* fp;
	char* fdata = NULL;
	stAnsMachDBCmsgHdr* ptr = (stAnsMachDBCmsgHdr *)g_cmsgHdr;

	//if( idx < 0 )
	//{
	//	printf("Invalid parameter: idx is less than 0\n");
	//	return ANSMACH_RET_INVALID_PARAM;;
	//}

	if( filepath == NULL )
	{
		printf("Invalide parameter: filepath is null\n");
		return ANSMACH_RET_INVALID_PARAM;
	}

	mapi_ansmach_init();

	for( mid=0; mid<g_cmsgStatus.count; mid++)
	{
		//if( ptr->mid == idx )
		if( strcmp(ptr->when, time) == 0 )
		{
			//memcpy(hdr, ptr, sizeof(stAnsMachDBCmsgHdr));
			fdata = (char *)malloc(ptr->size);
			fp = fopen(_TEST_MODE_DB_CMSGDATA_PATH, "rb");
			fseek(fp, totalsize, SEEK_SET);
			fread(fdata, 1, ptr->size, fp);
			fclose(fp);
			sprintf(filepath, "/tmp/voicemail/%s.711", ptr->when);
			printf("A. File: %s\n", filepath);
			fp = fopen(filepath, "wb");
			fwrite(fdata, 1, ptr->size, fp);
			fclose(fp);
			free(fdata);
			//break;
			goto found;
		}
		else
		{
			totalsize += ptr->size;
		}

		ptr++;
	}

	return ANSMACH_RET_FAIL;

found:
	return ANSMACH_RET_SUCCESS;
}
