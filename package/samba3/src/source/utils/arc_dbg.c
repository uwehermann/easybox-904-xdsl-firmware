
#ifdef SAMBA_DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <unistd.h>

#define	_LOG_DIR			"/var/log/"
#define	_LOG_FILE			"smbd.log"
#define	_LOG_PATH			_LOG_DIR _LOG_FILE
#define	_LOG_FILE_MAXSZ		50000
#define	_LOG_FILE_MAXNUM	3

void _fLimitFile()
{
	long	lOffset;
	FILE*	fp;
	int		iMax;
	int		iCnt;
	char	sFnOld[64];
	char	sFnNew[64];

	fp = fopen(_LOG_PATH, "r");
	if (!fp)
		return;

	if ( fseek( fp, 0, SEEK_END ) != 0 )
	{
		fclose(fp);
		return;
	}

	lOffset = ftell(fp);

	fclose(fp);

	if (lOffset >= _LOG_FILE_MAXSZ)
	{
		memset( sFnOld, 0, sizeof(sFnOld) );
		memset( sFnNew, 0, sizeof(sFnNew) );
		memcpy( sFnOld, _LOG_PATH ".0", sizeof(_LOG_PATH)+1 );
		memcpy( sFnNew, _LOG_PATH ".0", sizeof(_LOG_PATH)+1 );
		/**/
		if (_LOG_FILE_MAXNUM > 9)
			iMax = 9;
		else if (_LOG_FILE_MAXNUM >= 1)
			iMax = _LOG_FILE_MAXNUM;
		else
			iMax = 1;
		/**/
		sFnNew[ sizeof(_LOG_PATH) ] = '0' + iMax;
		unlink( sFnNew );
		/**/
		for (iCnt=_LOG_FILE_MAXNUM; iCnt>1; iCnt--)
		{
			sFnOld[ sizeof(_LOG_PATH) ] = '0' + iCnt - 1;
			sFnNew[ sizeof(_LOG_PATH) ] = '0' + iCnt;
			rename(sFnOld, sFnNew);
		}
		/**/
		sFnNew[ sizeof(_LOG_PATH) ] = '1';
		rename(_LOG_PATH , sFnNew);
	}
}

void _fLog(char *fmt, ...)
{
	va_list	va;
	FILE*	fp;
	time_t	t = time(0); /* LOG */

	_fLimitFile();

	fp = fopen(_LOG_PATH, "a");

	if (!fp) return;

	fprintf(fp, " [%d] %02u:%02u  ", getpid(), (t / 60) % 60, t % 60);
	va_start(va, fmt);
	vfprintf(fp, fmt, va);
	va_end(va);
	fprintf(fp, "\n");
	fclose(fp);
}

void _fDebug(char *fmt, ...)
{
	va_list va;
	FILE *fp;
	time_t t = time(0); /* LOG */

	_fLimitFile();

	fp = fopen(_LOG_PATH, "a");

	if (!fp) return;

	fprintf(fp, " [%d] %02u:%02u ", getpid(), (t / 60) % 60, t % 60);
	va_start(va, fmt);
	vfprintf(fp, fmt, va);
	va_end(va);
	fclose(fp);
}

void _fDebugNoTime(char *fmt, ...)
{
	va_list va;
	FILE *fp;

	_fLimitFile();

	fp = fopen(_LOG_PATH, "a");

	if (!fp)
		return;

	va_start(va, fmt);
		vfprintf(fp, fmt, va);
	va_end(va);

	fclose(fp);
}

#endif
