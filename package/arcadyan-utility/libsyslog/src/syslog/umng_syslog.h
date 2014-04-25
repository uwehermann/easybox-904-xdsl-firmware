#ifndef _ARCADYAN_SYSLOG_H_
#define _ARCADYAN_SYSLOG_H_

#include "umng_syslog_data.h"

extern void Log_Lock();
extern void Log_UnLock();
extern void OpenSystemLog();
extern char SetSystemLog(unsigned char log_type, unsigned char log_level, unsigned char log_facility, char* message);
extern char *GetCurrentSysLog();
extern void CloseSystemLog();
extern void ClearSystemLog();
extern void SetFormatSystemLog(unsigned char log_type, unsigned char log_level, unsigned char log_facility, const char *fmt, ...);

#endif // _ARCADYAN_SYSLOG_H_
