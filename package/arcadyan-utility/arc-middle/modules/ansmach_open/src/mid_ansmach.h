/*
 * Arcadyan middle layer definition for Answering Machine
 *
 * Copyright 2012, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _MID_ANSMACHINE_H_
#define _MID_ANSMACHINE_H_
#include <mid_types.h>
#include "mid_mapi_ccfgsal_voip.h"

#define ASM_MSG(...) \
	do \
	{ \
		FILE *fp; \
		fp = fopen("/dev/console", "w"); \
		if(fp)	\
		{	\
			fprintf(fp, __VA_ARGS__); \
			fclose(fp); \
		}	\
	} while (0)

#ifdef MID_DEBUG
#define ASM_DBG  MID_TRACE
#else
//#define ASM_DEBUG
#ifdef ASM_DEBUG
#define ASM_DBG(...) \
	do { \
		FILE *fp; \
		char msgbuf[BUFSIZ]; \
		int len; \
		len = snprintf(msgbuf, sizeof(msgbuf), "%s(%d): ", __func__, __LINE__); \
		snprintf(msgbuf + len, sizeof(msgbuf) - len, __VA_ARGS__); \
		fp = fopen("/dev/console", "w"); \
		if(fp)	\
		{	\
			fprintf(fp, "%s", msgbuf); \
			fclose(fp); \
		}	\
	} while (0)
#else
#define ASM_DBG(...)
#endif
#endif

#define ASM_EXEC(...) \
	do { \
		char cmdbuf[BUFSIZ]; \
		snprintf(cmdbuf, sizeof(cmdbuf), __VA_ARGS__); \
		ASM_DBG("%s\n",cmdbuf); \
		system(cmdbuf); \
	} while (0)

/* Generic Definitions */
#ifndef SECONDS
#define SECONDS 1
#else
#error "SECONDS had been predined!!! Please Check!!!"
#endif
#ifndef MINUTES
#define MINUTES (60 * SECONDS)
#else
#error "MINUTES had been predined!!! Please Check!!!"
#endif

#ifndef BYTE
#define BYTE 1
#else
#error "BYRE had been predined!!! Please Check!!!"
#endif

#ifndef KB
#define KB (1024 * BYTE)
#else
#error "KB had been predined!!! Please Check!!!"
#endif

#ifndef MB
#define MB (1024 * KB)
#else
#error "MB had been predined!!! Please Check!!!"
#endif

/* G711 Spec. */
#define G711_SAMPLE_RATE ((8000 * BYTE) / SECONDS)
#define G711_BYTE_TO_SEC(byte) ((byte) / G711_SAMPLE_RATE)
#define G711_SEC_TO_BYTE(sec)  ((sec) * G711_SAMPLE_RATE)

/* Answering Machine */
#define MAPI_ASM_ANSMACH_SECTION_NAME	"ansmach"
#define MAPI_ASM_MAX_AMSG_DURATION_ALL   (40 * MINUTES)
#define MAPI_ASM_MAX_AMSG_DURATION		(3 * MINUTES)
#define MAPI_ASM_MAX_AMSG_COUNT		\
	((MAPI_ASM_MAX_AMSG_DURATION_ALL / MAPI_ASM_MAX_AMSG_DURATION) + \
	((MAPI_ASM_MAX_AMSG_DURATION_ALL % MAPI_ASM_MAX_AMSG_DURATION) ? 1 : 0))

#define MAPI_ASM_MAX_DATE_LEN			16
#define MAPI_ASM_MAX_TIME_LEN			8
#define MAPI_ASM_MAX_FILEPATH    		128
#define MAPI_ASM_CALLER_ID_LEN 			20
#define MAPI_ASM_INTERNAL_NUM_LEN 		20

/* Greeting Messages */
#define MAPI_ASM_GRTMSG_SECTION_NAME	"grt_msg"
#define MAPI_ASM_MAX_GMSG_DURATION		(3 * MINUTES)
#define MAPI_ASM_MAX_GMSG_COUNT			3
#define MAPI_ASM_NAME_LEN				20
#define MAPI_ASM_ASI_TO_COUNT    		MAPI_CCFG_VOIP_MAX_SIP_ACCOUNT_NUM+MAPI_CCFG_VOIP_ISDN_MAX_NUMBER
#define MAPI_ASM_COUNTRY_CODE_COUNT		50
#define MAPI_ASM_CODE_LEN				2
#define MAPI_ASM_PHONE_LEN				MAPI_CCFG_VOIP_CFG_MAX_USERID_LEN //10
#define MAPI_ASM_PHONE_NUMBER_COUNT		MAPI_CCFG_VOIP_MAX_PHONE_BOOK_NUM
#define MAPI_ASM_CODE_PHONE_NUM_LEN		MAPI_ASM_CODE_LEN+MAPI_ASM_PHONE_LEN+2

#define MAPI_ASM_GRTMSG_FIND_EMPTY_ID	123456789

/* Email Forwarding */
#define MAPI_ASM_EMAILFW_SECTION_NAME	"email_fw"
#define MAPI_ASM_EMAIL_ADDRESS_LEN		50
#define MAPI_ASM_PASSWORD_LEN			30
#define MAPI_ASM_SMTP_SERVER_LEN		512

/* Move from cgi_phone.c */
#define ASM_MSG_EMAILFW_PREFIX        "/tmp/email_fw/"
#define ASM_MSG_EMAILFW_DL_PREFIX     "/tmp/dl/"
#define ASM_MSG_EMAILFW_ZIP_PREFIX    "Sprachnachrichten"
#define ASM_MSG_EMAILFW_MAX_ATTACH    256
#define ASM_MSG_EMAILFW_SSMTP_CONF    "/tmp/etc/config/ssmtp/ssmtp_ansmach_emailfw.conf"
#define ASM_MSG_EMAILFW_SUBJECT       "Weiterleitung der Sprachnachrichten von Ihrer Vodafone EasyBox"
#define ASM_MSG_EMAILFW_SUBJECT_C     "Sprachnachricht von "
#define ASM_MSG_EMAILFW_CONTENT       "The name of the *.zip file which is to attach is the same as by local saving localy:"
#define ASM_MSG_EMAILFW_CONTENT_C_FMT "Fur Sie wurde eine Sprachnachricht von %s am %d.%d.%d um %02d:%02d hinterlassen."

#define DFT_SMTP_PORT "25"
#define TESTMAIL_SUBJECT "Testmail von Ihrer Vodafone EasyBox"
#define TESTMAIL_CONTEXT "Sehr geehrter Benutzer,\\nHerzlichen Gl=FCckwunsch! Ihr SMTP-Client ist richtig konfiguriert.\\nDa es sich um eine Testmail handelt, antworten Sie bitte nicht darauf.\\nIhre Vodafone EasyBox."

typedef struct {
	int tm_sec;		/* seconds */
	int tm_min;		/* minutes */
	int tm_hour;	/* hours */
	int tm_mday;	/* day of the month */
	int tm_mon;		/* month */
	int tm_year;	/* year */
} mapi_ansmach_time_t;

/* Answering Machine */
typedef struct mapi_ansmach_msg {
	U64 id;										/* Identifier for matching recording messages, should be the UTC (millisecond) of the recording message created */
	mapi_ansmach_time_t time;
	U32 size;									/* Size of this recording message */
	U32 read;
	char callee_num[MAPI_ASM_INTERNAL_NUM_LEN];
	char caller_num[MAPI_ASM_CALLER_ID_LEN];
} mapi_ansmach_msg_t;

typedef struct mapi_ansmach {
	U8		enable;	// Answering Machine enable/disable
	float		used_capacity;
	mapi_ansmach_msg_t *msgs;
} mapi_ansmach_t;

/* Greeting Messages */
typedef struct
{
	char code[MAPI_ASM_CODE_LEN+1]; //+1: add one char size for "/o".
	char num[MAPI_ASM_PHONE_LEN+1]; //+1: add one char size for "/o".
} caller_phone_num_t;

typedef struct mapi_grtmsg_msg {
	U8		Enable;															// Greeting Messages enable/disable
	char	Name[MAPI_ASM_NAME_LEN];										// Name
	U64		id;																// Unique ID
	int		filesize;														//for duration use
	U32		AfterSec;														// Anser Call After Sec.
	char	AsiTo[MAPI_ASM_ASI_TO_COUNT][MAPI_ASM_CODE_PHONE_NUM_LEN+2];	// AssignToTheseNumbers
	U8		CodeEnable;														// FromSpecificCountriesCallEnable
	char	CountryCode[MAPI_ASM_COUNTRY_CODE_COUNT][MAPI_ASM_CODE_LEN+2];	// FromSpecificCountryCode
	U8		NumEnable;														// FromSpecificNumberCallEnable
	char	PhoneNum[MAPI_ASM_PHONE_NUMBER_COUNT][MAPI_ASM_PHONE_LEN+2];	// FromSpecificNumber									// Only for callers
	U8		AsiToAll;														// Map to all callee number.
	U8		PhoneNumToAll;													// Map to all caller number.
} mapi_grtmsg_msg_t;

typedef struct mapi_grtmsg {
	mapi_grtmsg_msg_t grtmsg[MAPI_ASM_MAX_GMSG_COUNT];						// Greeting Messages
} mapi_grtmsg_t;

/* Email Forwarding */
typedef struct mapi_email_fw {
	int Enable;														// Automatic Forwarding enable/disable
	char EmailAddress[MAPI_ASM_EMAIL_ADDRESS_LEN];					// Email Address
	char Username[MAPI_ASM_EMAIL_ADDRESS_LEN];
	char Password[MAPI_ASM_PASSWORD_LEN];								// Password
	char SMTP_server[MAPI_ASM_SMTP_SERVER_LEN];						// SMTP Server
	int TLS;
	int port;
} mapi_email_fw_t;


/**************/
/* Phone part */
/**************/

typedef struct {
	U8 enable;		/* for upper layer use : default value is 1 when calling mapi_ansmach_recMsg_list_get function */
	U64 id;			/* for recmsg is time (UTC) in millisecond; for grtmsg is index*/
	U32 afterSec;	/* for grtmsg, get from config */
	mapi_ansmach_time_t time;
	U64 time_millisec;	/* for recmsg save, UTC time in millisecond */
	U32 filesize;	/* size of this message */
	U32 read;		/* readed or not : New=0, Old=1 */
	char callee[MAPI_ASM_INTERNAL_NUM_LEN];
	char caller[MAPI_ASM_CALLER_ID_LEN];
	char filepath[MAPI_ASM_MAX_FILEPATH];
} mapi_ansmach_phone_msg_hdr_t;

typedef struct {
	char callee[MAPI_ASM_INTERNAL_NUM_LEN];
	U32 hdr_num;
	U32 new_num;
	U32 old_num;
	mapi_ansmach_phone_msg_hdr_t *hdr;
} mapi_ansmach_phone_msg_list_t;


/************/
/* LCD part */
/************/

#define MAPI_ASM_VOIP_DISPLAY_NAME_MAX_LEN	48
#define MAPI_ASM_VOIP_USER_ID_MAX_LEN		48

typedef struct {
	char userId[MAPI_ASM_VOIP_USER_ID_MAX_LEN];				/* callee number */
	char displayName[MAPI_ASM_VOIP_DISPLAY_NAME_MAX_LEN];	/* display name */
	char userId_area[MAPI_ASM_VOIP_USER_ID_MAX_LEN];				/* callee number */
	char userId_local[MAPI_ASM_VOIP_USER_ID_MAX_LEN];				/* callee number */
	U32  totalMsgNum;
	U32  unreadMsgNum;
} mapi_ansmach_account_info_t;

typedef struct {
	U32 len;
	mapi_ansmach_account_info_t *account;
} mapi_ansmach_account_info_list_t;


#endif // _MID_ANSMACHINE_H_
