#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netdb.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>
#include <limits.h>
/* Try to pull in PATH_MAX */
#include <limits.h>
#include <sys/param.h>
#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#include <syslog.h>
#include <sys/syscall.h>
#include <sys/utsname.h>

#include <paths.h>
#include <sys/un.h>
#include <sys/uio.h>

#include <netinet/in.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "libArcComDef.h"
#include "libArcComOs.h"

#include "umng_syslogd.h"
#include "umng_syslog.h"

int gEnableDebug = 0;

int die_sleep;
jmp_buf die_jmp;
int xfunc_error_retval = EXIT_FAILURE;

void xfunc_die(void)
{
	if (die_sleep) {
		if (die_sleep < 0
		) {
			/* Special case. We arrive here if NOFORK applet
			 * calls xfunc, which then decides to die.
			 * We don't die, but jump instead back to caller.
			 * NOFORK applets still cannot carelessly call xfuncs:
			 * p = xmalloc(10);
			 * q = xmalloc(10); // BUG! if this dies, we leak p!
			 */
			/* -2222 means "zero" (longjmp can't pass 0)
			 * run_nofork_applet() catches -2222. */
			longjmp(die_jmp, xfunc_error_retval ? xfunc_error_retval : -2222);
		}
		sleep(die_sleep);
	}
	exit(xfunc_error_retval);
}

void fflush_stdout_and_exit(int retval)
{
	if (fflush(stdout))
		bb_perror_msg_and_die("bb_msg_standard_output");

	if (die_sleep < 0) {
		/* We are in NOFORK applet. Do not exit() directly,
		 * but use xfunc_die() */
		xfunc_error_retval = retval;
		xfunc_die();
	}

	exit(retval);
}

int dprintf(const char* sFmt, ...)
{
	va_list		vlVars;
	char		sBuf[512];
	int			iRet;
	FILE*		fnOut;

	if(gEnableDebug==0)
		return;

	if (sFmt == ARC_COM_NULL)
		return 0;

	fnOut = fopen( "/dev/console", "w" );
	if (fnOut == ARC_COM_NULL)
		return 0;

	sBuf[sizeof(sBuf)-1] = '\0';

	va_start(vlVars,sFmt);

	iRet = vsnprintf(sBuf,sizeof(sBuf),sFmt,vlVars);

	va_end(vlVars);

	if (sBuf[sizeof(sBuf)-1] != '\0')
	{
		fprintf( fnOut, "NOTE: my_printf() overflow!!!\n");
	}

	sBuf[sizeof(sBuf)-1] = '\0';

	fprintf( fnOut, "%s", sBuf );
	fflush( fnOut );
	fclose( fnOut );

	return iRet;
}

unsigned char gbDisplayAll=1;
unsigned char gLevelFilterStringArray[LOG_LEVEL_NUMBER][MAX_LOG_LEVEL_STR_LENGTH]={LOG_LEVEL_DEFAULT_STR, LOG_LEVEL_DEBUG_STR, LOG_LEVEL_INFO_STR,
						LOG_LEVEL_WARNING_STR, LOG_LEVEL_ERROR_STR};
unsigned char gFacilityFilterStringArray[LOG_MESSAGE_TYPE_NUMBER][MAX_LOG_MESSAGE_STR_LENGTH]={LOG_MESSAGE_TYPE_DEFAULT_STR, LOG_MESSAGE_TYPE_SYSTEM_STR, LOG_MESSAGE_TYPE_WAN_STR,
						LOG_MESSAGE_TYPE_LAN_STR, LOG_MESSAGE_TYPE_VOICE_STR, LOG_MESSAGE_TYPE_DATA_STR, LOG_MESSAGE_TYPE_ACS_STR,
						LOG_MESSAGE_TYPE_UMTS_STR, LOG_MESSAGE_TYPE_USB_STR, LOG_MESSAGE_TYPE_FIREWALL_STR, LOG_MESSAGE_TYPE_CONNECTIONS_STR,
						LOG_MESSAGE_TYPE_UPNP_STR, LOG_MESSAGE_TYPE_SECURITY_STR, LOG_MESSAGE_TYPE_SERVICE_STR};
unsigned char gConfigLevelFilter[LOG_LEVEL_NUMBER]={0};
unsigned char gConfigFacilityFilter[LOG_MESSAGE_TYPE_NUMBER]={0};
int gConfigLevelFilterCount = 0;
int gConfigFacilityFilterCount = 0;

static int do_syslog_show()
{
//dprintf("[do_syslog_show]\n");
    SystemLogPool_s sysLog;
    char* buffer;
    struct AlertMessage_s alert_log;
    char tmp[150];
    time_t curtime;
    struct tm *loctime;
    int i, bDisplayThisEntry = 0, bFoundInLevelFilterList = 0, bFoundInFacilityFilterList = 0;

	OpenSystemLog();
	buffer = GetCurrentSysLog();
//dprintf("[do_syslog_show] buffer=0x%x\n", buffer);
    while(buffer != NULL)
    {
    	Log_Lock();
		sysLog = *((SystemLogPool_s*)buffer);
		if(gbDisplayAll==0)
		{	// by filter
			bDisplayThisEntry = 0;
			bFoundInLevelFilterList = 0;
			bFoundInFacilityFilterList = 0;
			for(i=0; i<gConfigLevelFilterCount; i++)
			{
				if(sysLog.log_level == gConfigLevelFilter[i])
				{
					bFoundInLevelFilterList = 1;
					break;
				}
			}
			for(i=0; i<gConfigFacilityFilterCount; i++)
			{
				if(sysLog.msg_type == gConfigFacilityFilter[i])
				{
					bFoundInFacilityFilterList = 1;
					break;
				}
			}
			if( (bFoundInLevelFilterList==1) && (bFoundInFacilityFilterList==1) )
				bDisplayThisEntry = 1;
			if(bDisplayThisEntry==0)
				goto do_syslog_show_next;
		}
        memset(&alert_log,0,sizeof(alert_log));
        curtime = sysLog.time;
    	loctime = localtime (&curtime);
        memcpy(&alert_log.time,loctime,sizeof(struct tm));
        strcpy(alert_log.message, sysLog.message);

				strftime(tmp, sizeof(tmp), "%H:%M:%S   %d %b", &alert_log.time);
//dprintf("[do_syslog_show] log_type=0x%x\n", SYSLOG_BUFFER_LOG_TYPE(buffer));
//        sprintf(tmp,"%02d/%02d/%04d%s%02d:%02d:%02d%s",
//                alert_log.time.tm_mon+1 , alert_log.time.tm_mday, alert_log.time.tm_year+1900, "  ",
//                alert_log.time.tm_hour, alert_log.time.tm_min, alert_log.time.tm_sec, " ");
        printf("%-20s", tmp);

    	printf("%s\n", alert_log.message);

do_syslog_show_next:
		buffer = SYSLOG_BUFFER_NEXT(buffer);
//dprintf("[do_syslog_show] next buffer=0x%x\n", buffer);
		Log_UnLock();
    }

	CloseSystemLog();

	return 0;
}

static int do_syslog_show_by_msg_type(char *msg_type_str)
{
//dprintf("[do_syslog_show_by_msg_type]\n");
    SystemLogPool_s sysLog;
    char* buffer;
    struct AlertMessage_s alert_log;
    char tmp[150];
    time_t curtime;
    struct tm *loctime;
    int input_msg_type = LOG_MESSAGE_TYPE_DEFAULT;
    int msg_type = LOG_MESSAGE_TYPE_DEFAULT;
    int log_type = LOG_TYPE_OTHER;
	char*			pPtr;
	char*			pLast;
	unsigned char tmpConfigFacilityFilter[LOG_MESSAGE_TYPE_NUMBER]={0};
	int tmpConfigFacilityFilterCount = 0, i;

	pLast=msg_type_str;
	while ( (pPtr=osStrTok_r( pLast, " ", &pLast )) != ARC_COM_NULL )
	{
		for(i=0; i<LOG_MESSAGE_TYPE_NUMBER; i++)
		{
			if(strcmp(pPtr, gFacilityFilterStringArray[i])==0)
			{
				dprintf("[do_syslog_show_by_msg_type] pPtr=%s\n", gFacilityFilterStringArray[i]);
				tmpConfigFacilityFilter[tmpConfigFacilityFilterCount] = i;
				tmpConfigFacilityFilterCount++;
				break;
			}
		}
	}
	dprintf("[do_syslog_show_by_msg_type] tmpConfigFacilityFilterCount=%d\n", tmpConfigFacilityFilterCount);

	OpenSystemLog();
	buffer = GetCurrentSysLog();
//dprintf("[do_syslog_show_by_msg_type] buffer=0x%x, msg_type=%d\n", buffer, msg_type);
    while(buffer != NULL)
    {
    	Log_Lock();
		sysLog = *((SystemLogPool_s*)buffer);
        memset(&alert_log,0,sizeof(alert_log));
        curtime = sysLog.time;
    	loctime = localtime (&curtime);
        memcpy(&alert_log.time,loctime,sizeof(struct tm));
        strcpy(alert_log.message, sysLog.message);
        log_type = SYSLOG_BUFFER_LOG_TYPE(buffer);
		msg_type = SYSLOG_BUFFER_MSG_TYPE(buffer);

		for(i=0; i<tmpConfigFacilityFilterCount; i++)
		{
			if(msg_type == tmpConfigFacilityFilter[i])
			{
				strftime(tmp, sizeof(tmp), "%H:%M:%S   %d %b", &alert_log.time);
//dprintf("[do_syslog_show_by_msg_type] log_type=0x%x, msg_type=0x%x\n", SYSLOG_BUFFER_LOG_TYPE(buffer), SYSLOG_BUFFER_MSG_TYPE(buffer));
//        	sprintf(tmp,"%02d/%02d/%04d%s%02d:%02d:%02d%s",
//                alert_log.time.tm_mon+1 , alert_log.time.tm_mday, alert_log.time.tm_year+1900, "  ",
//                alert_log.time.tm_hour, alert_log.time.tm_min, alert_log.time.tm_sec, " ");
				printf("%-20s", tmp);

	    		printf("%s\n", alert_log.message);

				break;
			}
		}

		buffer = SYSLOG_BUFFER_NEXT(buffer);
//dprintf("[do_syslog_show_by_msg_type] next buffer=0x%x\n", buffer);
		Log_UnLock();
    }

	CloseSystemLog();

	return 0;
}

static int do_syslog_show_by_level(char *level_str)
{
//dprintf("[do_syslog_show_by_level]\n");
    SystemLogPool_s sysLog;
    char* buffer;
    struct AlertMessage_s alert_log;
    char tmp[150];
    time_t curtime;
    struct tm *loctime;
    int level = LOG_LEVEL_DEFAULT;
    int log_type = LOG_TYPE_OTHER;
	char*			pPtr;
	char*			pLast;
	unsigned char tmpConfigLevelFilter[LOG_MESSAGE_TYPE_NUMBER]={0};
	int tmpConfigLevelFilterCount = 0, i;

	pLast=level_str;
	while ( (pPtr=osStrTok_r( pLast, " ", &pLast )) != ARC_COM_NULL )
	{
		for(i=0; i<LOG_LEVEL_NUMBER; i++)
		{
			if(strcmp(pPtr, gLevelFilterStringArray[i])==0)
			{
				dprintf("[do_syslog_show_by_level] pPtr=%s\n", gLevelFilterStringArray[i]);
				tmpConfigLevelFilter[tmpConfigLevelFilterCount] = i;
				tmpConfigLevelFilterCount++;
				break;
			}
		}
	}
	dprintf("[do_syslog_show_by_level] tmpConfigLevelFilterCount=%d\n", tmpConfigLevelFilterCount);

	OpenSystemLog();
	buffer = GetCurrentSysLog();
//dprintf("[do_syslog_show_by_level] buffer=0x%x, msg_type=%d\n", buffer, msg_type);
    while(buffer != NULL)
    {
    	Log_Lock();
		sysLog = *((SystemLogPool_s*)buffer);
        memset(&alert_log,0,sizeof(alert_log));
        curtime = sysLog.time;
    	loctime = localtime (&curtime);
        memcpy(&alert_log.time,loctime,sizeof(struct tm));
        strcpy(alert_log.message, sysLog.message);
        log_type = SYSLOG_BUFFER_LOG_TYPE(buffer);
		level = SYSLOG_BUFFER_LOG_LEVEL(buffer);

		for(i=0; i<tmpConfigLevelFilterCount; i++)
		{
			dprintf("[do_syslog_show_by_level] level=%d, tmpConfigLevelFilter[%d]=%d\n", level, i, tmpConfigLevelFilter[i]);
			if(level == tmpConfigLevelFilter[i])
			{
				strftime(tmp, sizeof(tmp), "%H:%M:%S   %d %b", &alert_log.time);
//dprintf("[do_syslog_show_by_level] log_type=0x%x, msg_type=0x%x\n", SYSLOG_BUFFER_LOG_TYPE(buffer), SYSLOG_BUFFER_MSG_TYPE(buffer));
//        	sprintf(tmp,"%02d/%02d/%04d%s%02d:%02d:%02d%s",
//                alert_log.time.tm_mon+1 , alert_log.time.tm_mday, alert_log.time.tm_year+1900, "  ",
//                alert_log.time.tm_hour, alert_log.time.tm_min, alert_log.time.tm_sec, " ");
				printf("%-20s", tmp);

	    		printf("%s\n", alert_log.message);

				break;
			}
		}

		buffer = SYSLOG_BUFFER_NEXT(buffer);
//dprintf("[do_syslog_show_by_level] next buffer=0x%x\n", buffer);
		Log_UnLock();
    }

	CloseSystemLog();

	return 0;
}

static int do_syslog_clear()
{
dprintf("[do_syslog_clear]\n");
	OpenSystemLog();

	SetSystemLog(LOG_TYPE_CLEAR_SYSLOG, LOG_LEVEL_DEFAULT, LOG_MESSAGE_TYPE_DEFAULT, "clear syslog buffer");
sleep(1);
	CloseSystemLog();

	do_syslog_show();
	return 0;
}

static int do_syslog_test()
{
dprintf("[do_syslog_test]\n");
	OpenSystemLog();

#if 1
	SetSystemLog(LOG_TYPE_OTHER, LOG_LEVEL_DEFAULT, LOG_MESSAGE_TYPE_DEFAULT, "test by bitonic 0");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_BIGPOND, LOG_LEVEL_DEBUG, LOG_MESSAGE_TYPE_SYSTEM, "test by bitonic 1");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_DDNS_TZO, LOG_LEVEL_INFO, LOG_MESSAGE_TYPE_WAN, "test by bitonic 2");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_DDNS_DYNDNS, LOG_LEVEL_WARNING, LOG_MESSAGE_TYPE_LAN, "test by bitonic 3");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_DDNS_DHIS, LOG_LEVEL_ERROR, LOG_MESSAGE_TYPE_VOICE, "test by bitonic 4");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_DDNS_SMC, LOG_LEVEL_DEBUG, LOG_MESSAGE_TYPE_DATA, "test by bitonic 5");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_HTTPD, LOG_LEVEL_INFO, LOG_MESSAGE_TYPE_ACS, "test by bitonic 6");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_IGD1, LOG_LEVEL_WARNING, LOG_MESSAGE_TYPE_UMTS, "test by bitonic 7");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_IGD1_A, LOG_LEVEL_ERROR, LOG_MESSAGE_TYPE_USB, "test by bitonic 8");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_LPD, LOG_LEVEL_DEBUG, LOG_MESSAGE_TYPE_FIREWALL, "test by bitonic 9");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_DHCPD, LOG_LEVEL_INFO, LOG_MESSAGE_TYPE_SYSTEM, "test by bitonic 10");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_DHCP_CLT, LOG_LEVEL_WARNING, LOG_MESSAGE_TYPE_WAN, "test by bitonic 11");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_DNS_PROXY, LOG_LEVEL_ERROR, LOG_MESSAGE_TYPE_LAN, "test by bitonic 12");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_MAIL, LOG_LEVEL_DEBUG, LOG_MESSAGE_TYPE_VOICE, "test by bitonic 13");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_MAIL, LOG_LEVEL_INFO, LOG_MESSAGE_TYPE_DATA, "test by bitonic 14");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_MAIL, LOG_LEVEL_WARNING, LOG_MESSAGE_TYPE_ACS, "test by bitonic 15");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_MAIL, LOG_LEVEL_ERROR, LOG_MESSAGE_TYPE_UMTS, "test by bitonic 16");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_MAIL, LOG_LEVEL_DEBUG, LOG_MESSAGE_TYPE_USB, "test by bitonic 17");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_MAIL, LOG_LEVEL_INFO, LOG_MESSAGE_TYPE_FIREWALL, "test by bitonic 18");
sleep(1);
	SetSystemLog(LOG_TYPE_AP_MAIL, LOG_LEVEL_WARNING, LOG_MESSAGE_TYPE_SYSTEM, "test by bitonic 19");
sleep(1);
#endif
	SetFormatSystemLog(LOG_TYPE_OTHER, LOG_LEVEL_ERROR, LOG_MESSAGE_TYPE_WAN, "abcdefghijklmnopq1234567890abcdefghijklmnopq1234567890abcdefghijklmnopq1234567890abcdefghijklmnopq1234567890abcdefghijklmnopq1234567890abcdefghijklmnopq1234567890abcdefghijklmnopq1234567890abcdefghijklmnopq1234567890abcdefghijklmnopq1234567890abcdefghijklmnopq1234567890");
sleep(1);
	SetFormatSystemLog(LOG_TYPE_OTHER, LOG_LEVEL_DEBUG, LOG_MESSAGE_TYPE_LAN, "abcdefghijklmnopq1234567890");
sleep(1);

	CloseSystemLog();

	do_syslog_show();
	return 0;
}

static int do_syslog_add( unsigned char* logTypeStr, unsigned char *logLevelStr, unsigned char* msgTypeStr, char* msg )
{
	int i;
	unsigned char logType, logLevel, msgType;

	dprintf("[do_syslog_add] logTypeStr=%s, logLevelStr=%s, msgTypeStr=%s\n", logTypeStr, logLevelStr, msgTypeStr);

	OpenSystemLog();

	logType= atoi(logTypeStr);

	for(i=0; i<LOG_LEVEL_NUMBER; i++)
	{
		if(strcmp(logLevelStr, gLevelFilterStringArray[i])==0)
		{
			logLevel = i;
			break;
		}
	}
	for(i=0; i<LOG_MESSAGE_TYPE_NUMBER; i++)
	{
		if(strcmp(msgTypeStr, gFacilityFilterStringArray[i])==0)
		{
			msgType = i;
			break;
		}
	}
	dprintf("[do_syslog_add] logType=%d, logLevel=%d, msgType=%d\n", logType, logLevel, msgType);
	SetSystemLog( logType, logLevel, msgType, msg );

	CloseSystemLog();

	return 0;
}

int main( int argc, const char *argv[] )
{
	int stat=1;
	int done=0;
	int i;
	char sCmd[64], szData[64];
	unsigned char config_level_filter_array[1024]="";
	unsigned char config_facility_filter_array[1024]="";
	char*			pPtr;
	char*			pLast;
	int 			bFindEnableDebugArgument = 0, nArgvStartIdx = 1;

	if(argc > 1)
	{
		if(strcmp(argv[1], "-d") == 0){
			bFindEnableDebugArgument = 1;
			gEnableDebug = 1;
		}
	}

	// collection config data
	snprintf( sCmd, sizeof(sCmd), "ccfg_cli get display_all@log");
	if (osSystem_GetOutput( sCmd, szData, sizeof(szData) ) <= 0)
   		goto main_exit;
	utilStrTrim( szData );
	if(strcmp(szData, "on") == 0)
		gbDisplayAll = 1;
	else
		gbDisplayAll = 0;
	dprintf("[syslog main] gbDisplayAll=%d\n", gbDisplayAll);

	snprintf( sCmd, sizeof(sCmd), "ccfg_cli get level_filter_array@log");
	if (osSystem_GetOutput( sCmd, config_level_filter_array, sizeof(config_level_filter_array) ) <= 0)
   		goto main_exit;
	utilStrTrim( config_level_filter_array );
	dprintf("[syslog main] config_level_filter_array=%s\n", config_level_filter_array);

	pLast=config_level_filter_array;
	while ( (pPtr=osStrTok_r( pLast, " ", &pLast )) != ARC_COM_NULL )
	{
		for(i=0; i<LOG_LEVEL_NUMBER; i++)
		{
			if(strcmp(pPtr, gLevelFilterStringArray[i])==0)
			{
				dprintf("[syslog main] pPtr=%s\n", gLevelFilterStringArray[i]);
				gConfigLevelFilter[gConfigLevelFilterCount] = i;
				gConfigLevelFilterCount++;
				break;
			}
		}
	}
	dprintf("[syslog main] gConfigLevelFilterCount=%d\n", gConfigLevelFilterCount);

	snprintf( sCmd, sizeof(sCmd), "ccfg_cli get facility_filter_array@log");
	if (osSystem_GetOutput( sCmd, config_facility_filter_array, sizeof(config_facility_filter_array) ) <= 0)
   		goto main_exit;
	utilStrTrim( config_facility_filter_array );
	dprintf("[syslog main] config_facility_filter_array=%s\n", config_facility_filter_array);

	pLast=config_facility_filter_array;
	while ( (pPtr=osStrTok_r( pLast, " ", &pLast )) != ARC_COM_NULL )
	{
		for(i=0; i<LOG_MESSAGE_TYPE_NUMBER; i++)
		{
			if(strcmp(pPtr, gFacilityFilterStringArray[i])==0)
			{
				dprintf("[syslog main] pPtr=%s\n", gFacilityFilterStringArray[i]);
				gConfigFacilityFilter[gConfigFacilityFilterCount] = i;
				gConfigFacilityFilterCount++;
				break;
			}
		}
	}
	dprintf("[syslog main] gConfigFacilityFilterCount=%d\n", gConfigFacilityFilterCount);
	//

	for( i=(1+bFindEnableDebugArgument); i<argc; i++ )
	{
		if(!strcmp(argv[i], "show")){
			stat=do_syslog_show();
			done++;
		}
		if(!strcmp(argv[i], "show_all")){
			gbDisplayAll = 1;
			stat=do_syslog_show();
			done++;
		}
		else if(!strcmp(argv[i], "show_by_level")){
			if( (i+1) >= argc) break;
			stat=do_syslog_show_by_level(argv[i+1]);
			done++;
			break;
		}
		else if(!strcmp(argv[i], "show_by_msg_type")){
			if( (i+1) >= argc) break;
			// check test flag
			stat=do_syslog_show_by_msg_type(argv[i+1]);
			done++;
			break;
		}
		else if(!strcmp(argv[i], "clear")){
			do_syslog_clear();
			done++;
		}
		else if(!strcmp(argv[i], "test")){
			do_syslog_test();
			done++;
		}
		else if(!strcmp(argv[i], "add"))
		{
			if( (i+4) >= argc)
				break;
			do_syslog_add( argv[i+1], argv[i+2], argv[i+3], argv[i+4] );
			done++;
		}
		else
			break;
		}

    if( !done )
	{
		fprintf(stderr,
			"Usage:\n"
			"	enable debug => umng_syslog_cli -d [show | show_all | show_by_level | show_by_msg_type | add | clear | test]\n"
			"	display by config filter => umng_syslog_cli show\n"
			"	display all => umng_syslog_cli show_all\n"
			"	display by level, don't care user config => umng_syslog_cli show_by_level <level strings>\n"
			"	display by facility, don't care user config => umng_syslog_cli show_by_msg_type <msg_type strings>\n"
			"	add one new system log => umng_syslog_cli add <log_type> <level> <msg_type> <message>\n"
			"	clear all system logs => umng_syslog_cli clear\n"
			"	run test script => umng_syslog_cli test\n"
		);

		stat=1;
	}

main_exit:
	fflush_stdout_and_exit(EXIT_SUCCESS);
}