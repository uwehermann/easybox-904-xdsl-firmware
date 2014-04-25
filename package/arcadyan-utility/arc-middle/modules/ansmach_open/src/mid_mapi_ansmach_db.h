/*
 * Arcadyan middle layer header file of APIs for Answering Machine Database
 *
 * Copyright 2012, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _MID_MAPI_ANSMACHINE_DB_H_
#define _MID_MAPI_ANSMACHINE_DB_H_

#include "mid_ansmach.h"

#define _TEST_MODE_
#ifdef _TEST_MODE_
#define _TEST_MODE_DB_CMSGHDR_PATH "/tmp/voicemail/.testdb/chdr.bin"
#define _TEST_MODE_DB_GMSGHDR_PATH "/tmp/voicemail/.testdb/ghdr.bin"
#define _TEST_MODE_DB_GMSGSTA_PATH "/tmp/voicemail/.testdb/gsta.bin"
#define _TEST_MODE_DB_CMSGSTA_PATH "/tmp/voicemail/.testdb/csta.bin"
#define _TEST_MODE_DB_CMSGDATA_PATH "/tmp/voicemail/.testdb/cdata.bin"
#define _TEST_MODE_DB_ISINIT_FILE "/tmp/voicemail/.testdb/init"
#endif

#define MAX_GMSG_NAME_SIZE		20
#define MAX_CMSG_CALLERID_SIZE		15
#define MAX_CMSG_CALLEEID_SIZE		15
#define MAX_DATE_TIME_SIZE		15

#define VOICE_MAIL_UNREAD		0x1
#define VOICE_MAIL_READ			0x0

#define AnsMach_TRUE			0x1
#define AnsMach_FAIL			0x0

#define ANSMACH_TRUE		1
#define ANSMACH_FALSE		0

typedef enum _ANSMACH_RET {
	ANSMACH_RET_SUCCESS,
	ANSMACH_RET_FAIL,
	ANSMACH_RET_INVALID_PARAM
} AnsMachRet ;

typedef struct {
	int count;
	int max;
} stAnsMachGmsgDBStatus;

typedef struct {
	int count;
	int max;
} stAnsMachCmsgDBStatus;

typedef struct {
	int mid;
	char name[MAX_GMSG_NAME_SIZE];
	int size;
} stAnsMachDBGmsgHdr;

typedef struct {
	stAnsMachDBGmsgHdr hdr;
	char* data;
} stAnsMachDBGmsg;

typedef struct {
	int mid;
	char when[MAX_DATE_TIME_SIZE];
	char caller[MAX_CMSG_CALLERID_SIZE];
	char callee[MAX_CMSG_CALLEEID_SIZE];
	char unread; // boolean
	int size;
} stAnsMachDBCmsgHdr;

typedef struct {
	stAnsMachDBCmsgHdr hdr;
	char* data;
} stAnsMachDBCmsg;

//extern int maip_ansmach_gmsgdb_update(int mid, mapi_ansmach_gmsg_hdr_t hdr, char* filepath);

//extern int mapi_ansmach_db_selectByIndex(int mid, int tbl_name);

//extern int maip_ansmach_cmsgdb_add(mapi_ansmach_cmsg_hdr_t hdr, char* filepath);
//extern int maip_ansmach_cmsgdb_get(mapi_ansmach_cmsg_hdr_t hdr, char* filepath);
extern int mapi_ansmach_cmsgdb_get_by_time(char *time, char* filepath);
extern int mapi_ansmach_cmsgdb_get(int idx, char* filepath);
extern int mapi_ansmach_cmsgdb_hdr_getCount(void);
extern int mapi_ansmach_cmsgdb_hdr_get(int idx, stAnsMachDBCmsgHdr* hdr);
extern int mapi_ansmach_cmsgdb_save(stAnsMachDBCmsgHdr* hdr, char* filepath);

#endif // _MID_MAPI_ANSMACHINE_DB_H_
