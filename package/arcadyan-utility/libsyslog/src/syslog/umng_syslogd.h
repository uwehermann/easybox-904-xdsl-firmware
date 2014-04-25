#ifndef _ARCADYAN_SYSLOGD_H_
#define _ARCADYAN_SYSLOGD_H_

#include "time.h"
#include "logstr.h"

#include "umng_syslog_data.h"

enum {
	false	= 0,
	true	= 1
};

struct timeBase_s {
	unsigned long tm_year;
	unsigned long tm_mon;
	unsigned long tm_mday;
	unsigned long tm_wday;
	unsigned long tm_hour;
	unsigned long tm_min;
	unsigned long tm_sec;
};

/* free systme log pool */
typedef struct {
    char *head, *tail;
    char *free;
    unsigned short count;
} SystemLogPool_Info;

int syslog_reload_backup(char* in);
int syslog_reload_restore(char* in);
char* GetSystemLogBuffer(void);
char* GetCurrentSysLog(void);
char RelSystemLogBuffer(char* buffer);

extern unsigned long logTypeMappingTbl[96];

extern char syslogd_SetSystemLogAndMsgType(unsigned char log_type, unsigned char log_level, unsigned char msg_type, char *message);
extern char syslogd_SetSystemLog(unsigned char log_type, unsigned char log_level, unsigned char log_facility, char* message);
extern char syslogd_SetSystemLog2(unsigned char log_type, unsigned char log_level, unsigned char log_facility, unsigned char lang_type, char *message);
extern char syslogd_sprintf_SetSystemLog(unsigned char log_type, unsigned char log_level, unsigned char log_facility, char *fmt, ...);

extern int initSystemLogPool(void);
extern void ClearSystemLogPool(void);
extern short GetSysLogCurrentIndex(void);

#endif // _ARCADYAN_SYSLOGD_H_
