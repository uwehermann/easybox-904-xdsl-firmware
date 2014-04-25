/*
 *Copyright (c) 2007,西安邮电学院Linxu兴趣小组
 * All rights reserved.
 *
 * 文件名称：write_log.c
 * 摘    要：写日志
 * 当前版本：1.0
 * 作    者：董溥
 * 完成日期：2007年5月17日
 */

#include "xylftp.h"
#include <stdio.h>
#include <stdarg.h>

extern struct user_env user_env;

int write_log(char *message, int level)
{
	const char *levels;
	int  option;
	option = LOG_PID;

	if (level == 1) {
		levels = "XYLftp[NOTICE]";
	}
	else if (level == 0) {
		levels = "XYLftp[ERROR]";
	}
	else {
		write_log("wrong level", 0);
		return -1;
	}
	#ifdef DEBUG
	option = LOG_PERROR;
	#endif	
	openlog(levels,option, LOG_INFO);
	syslog(LOG_INFO|LOG_LOCAL1,"From IP:%s Port:%d User:%s [%s]",
		user_env.client_ip, user_env.client_port,
		user_env.user_name, message);
	closelog();

	return 0;
}

int printfConsole(const char *sFmt, ...)
{
	va_list		vlVars;
	char		sBuf[512];
	int			iRet;
	FILE*		fnOut;

	if (sFmt == 0)
		return 0;

	fnOut = fopen( "/dev/console", "w" );
	if (fnOut == 0)
		return 0;

	sBuf[sizeof(sBuf)-1] = '\0';

	va_start(vlVars,sFmt);

	iRet = vsnprintf(sBuf,sizeof(sBuf),sFmt,vlVars);

	va_end(vlVars);	

	if (sBuf[sizeof(sBuf)-1] != '\0')
	{
		fprintf( fnOut, "NOTE: printfConsole() overflow!!!\n");
	}

	sBuf[sizeof(sBuf)-1] = '\0';

	fprintf( fnOut, "%s", sBuf );
	fflush( fnOut );
	fclose( fnOut );

	return iRet;
}
