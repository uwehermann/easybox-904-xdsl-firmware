
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <ctype.h>
#include <linux/autoconf.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

#include "libArcComApi.h"


#define	OS_PROC_DIR			"/proc/"
#define	OS_PROC_STAT_FILE	"stat"
#define	OS_PROC_SPAWN_FLAG	( SIGCHLD | CLONE_VM | CLONE_FS | CLONE_FILES )

#define	OS_TIME_UPTIME_FILE	"/proc/uptime"

typedef struct _osProcCb {
	unsigned long	lProcId;
	fnProc			fnEntry;
	unsigned long	bSuspend;
	void*			pStack;
	unsigned char	byData[0];
} _osProcCb;

unsigned long		_glOsProcInited = ARC_COM_FALSE;
unsigned long		_glChldCnt = 0;
_osProcCb	_gstOsChldCb[ DEF_OS_PROC_MAXCHLD ];


/*******************************************************************************
 *                           Internal functions
 ******************************************************************************/

void _osProcInit( void )
{
	if (_glOsProcInited == ARC_COM_FALSE)
	{
		_glChldCnt = 0;
		osBZero( _gstOsChldCb, sizeof(_gstOsChldCb) );
		_glOsProcInited = ARC_COM_TRUE;
	}
}

int _osProcLogChild( _osProcCb* pCb )
{
	int		iCnt;

	if (pCb == ARC_COM_NULL)
		return ARC_COM_ERROR;

	_osProcInit();

	for (iCnt=0; iCnt<DEF_OS_PROC_MAXCHLD; iCnt++)
	{
		if (_gstOsChldCb[iCnt].lProcId == 0) {
			_gstOsChldCb[iCnt] = *pCb;
			_glChldCnt++;
			return iCnt;
		}
	}

	return ARC_COM_ERROR;
}


void _osProcChldHandler(int iSigNum)
{
	int		iCnt;
	int		bHit;

	if (iSigNum == SIGCHLD) {
		bHit = 0;
		for (iCnt=0; iCnt<DEF_OS_PROC_MAXCHLD; iCnt++)
		{
			if (_gstOsChldCb[iCnt].lProcId == 0)
				continue;
			switch ( _gstOsChldCb[iCnt].lProcId )
			{
			  case eOsProcStat_ready:
			  case eOsProcStat_sleeping:
			  case eOsProcStat_suspended:
			  case eOsProcStat_blocked:
					continue;
			  case eOsProcStat_unknown:
			  case eOsProcStat_hung:
			  case eOsProcStat_error:
			  default:
					if (_gstOsChldCb[iCnt].pStack != 0)
						free( _gstOsChldCb[iCnt].pStack );
					_gstOsChldCb[iCnt].lProcId = 0;
					_glChldCnt--;
					bHit = 1;
					break;
			}
		}

		while ( waitpid( -1, 0, WNOHANG ) > 0 )
			;
	}
}

int _osProcSpwanEntry( void* pArg )
{
	int			iRet;
	void*		pStack;
	_osProcCb*	pCb;

	osIoAdaptTty();

	pCb = (_osProcCb*)pArg;

	if ( pCb->bSuspend == ARC_COM_TRUE )
		osProcSuspendSelf();

	iRet = pCb->fnEntry( pCb->byData );

	pStack = pCb->pStack;

	free( pArg );

	return iRet;
}



/*******************************************************************************
 * Description
 *		Allocate a block of memory. pls refer to ANSI malloc()
 *
 ******************************************************************************/
void* osMalloc( size_t ulSize )
{
	return malloc( ulSize );
}

/*******************************************************************************
 * Description
 *		Allocates an array in memory with elements initialized to 0. pls refer to ANSI calloc()
 *
 ******************************************************************************/
void* osCalloc ( size_t elemCount, size_t elemSize )
{
	return calloc(1,elemCount*elemSize);
}

/*******************************************************************************
 * Description
 *		Free an allocated memory block. pls refer to ANSI free()
 *
 ******************************************************************************/
void osFree( void* pMem )
{
	free( pMem );
}

/*******************************************************************************
 * Description
 *		Sets a memory block to all 0's. pls refer to bzero()
 *
 ******************************************************************************/
void* osBZero(void* pMem, size_t ulCnt)
{
	if (pMem!=ARC_COM_NULL && ulCnt>0)
		return memset(pMem,0,ulCnt);

	return 0;
}

/*******************************************************************************
 * Description
 *		Execute shell command
 *
 * Parameters
 *		sShellCmd:	shell command to execute
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 ******************************************************************************/
long osSystem( char* sShellCmd )
{
	int ret_value=0;

	if (sShellCmd == ARC_COM_NULL)
		return ARC_COM_ERROR;

	if ((ret_value=system( sShellCmd )) == -1){
#if 1   //patch from 724, also see http://blog.csdn.net/taolinke/article/details/8057335 for alternative
		int err_number=errno;	 

		switch(err_number){
			case ECHILD: /* 10:ECHILD No Child Process, but the shell command was launched completely, ignore this case. */
				return ARC_COM_OK;
			default:
				break;
	  	}
		#if 0
		{
			/* For debugging purpose, and it should be checked later [Jess Hsieh] */
			char return_buffer_result_cmd[512];
			sprintf(return_buffer_result_cmd,"echo \"%s[%d]: return value [%d]: Command [%s] : errno (%d):(%d) \" > /dev/console",__FUNCTION__,__LINE__,ret_value,sShellCmd,err_number, errno);
			system(return_buffer_result_cmd);
  		}
		#endif
#endif
		return ARC_COM_ERROR;
	}
	//else if(ret_value==127){
	//	printf("[osSystem] run /bin/sh failed\n");
	//}

	while ( waitpid( -1, 0, WNOHANG ) > 0 )
	{
		/* NOOP */;
	}

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Execute shell command and get the output
 *
 * Parameters
 *		sShellCmd:	shell command to execute
 *		sBuf:		buffer to be filled with execution output. It can be NULL.
 *		iBufSz:		length of sBuf.
 *
 * Returns
 *		* >=0:					output size
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *		* only up to iBufSz-1 bytes are copied into sBuf if output size is larger than iBufSz
 *
 ******************************************************************************/
long osSystem_GetOutput ( char* sShellCmd, char* sBuf, int iBufSz )
{
	char	sCmd[512];
	char	sTmpFn[32];
	FILE*	pFile;
	long	lCnt;

	if (sShellCmd == ARC_COM_NULL)
		return ARC_COM_ERROR;

	if (strlen(sShellCmd) > (sizeof(sCmd)-32))
		return ARC_COM_ERROR;

	sprintf( sTmpFn, "/tmp/%ld.XXXXXX", (long)getpid() );
	close(mkstemp(sTmpFn));

	sprintf( sCmd, "%s > %s", sShellCmd, sTmpFn );

	if (osSystem( sCmd ) != ARC_COM_OK)
	{
		unlink( sTmpFn );
		printf("[osSystem_GetOutput] osSystem failed, cmd=%s\n", sCmd);
		return ARC_COM_ERROR;
	}

	if ( (pFile=fopen(sTmpFn,"r")) == NULL ){
	    printf("[osSystem_GetOutput] fopen failed, cmd=%s\n", sCmd);
		return ARC_COM_ERROR;
	}
	if (sBuf && iBufSz>0)
	{
		osBZero( sBuf, iBufSz);
		fread( sBuf, 1, iBufSz-1, pFile );
	}

	fclose( pFile );

	lCnt = osFileSize( sTmpFn );

	unlink( sTmpFn );

	return lCnt;
}

/*******************************************************************************
 * Description
 *		Execute shell command in background by system "runtask.sh"
 *
 * Parameters
 *		sShellCmd:	shell command to execute
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Notes
 *		* The max length of sShellCmd is 226
 *
 ******************************************************************************/
long osRunTask( char* sShellCmd )
{
	//char	sCmd[256];
    char	sCmd[512];

	if (sShellCmd == ARC_COM_NULL)
		return ARC_COM_ERROR;

	//if ( strlen( sShellCmd ) > 226 )
	if ( strlen( sShellCmd ) > 482 )
		return ARC_COM_ERROR;

	sprintf( sCmd, "/etc/init.d/runtask.sh add \"%s\"", sShellCmd );

	if (system( sCmd ) == -1)
		return ARC_COM_ERROR;

	while ( waitpid( -1, 0, WNOHANG ) > 0 )
	{
		/* NOOP */;
	}

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Execute shell script file in background by system "runtask.sh"
 *
 * Parameters
 *		sScriptFile:	shell script file to execute
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Notes
 *		* The max length of sScriptFile file name is 222
 *
 ******************************************************************************/
long osRunTaskFile( char* sScriptFile )
{
	char	sFile[256];

	if (sScriptFile == ARC_COM_NULL)
		return ARC_COM_ERROR;

	if ( strlen( sScriptFile ) > 222 )
		return ARC_COM_ERROR;

	sprintf( sFile, "/etc/init.d/runtask.sh addfile \"%s\"", sScriptFile );

	if (system( sFile ) == -1)
		return ARC_COM_ERROR;

	while ( waitpid( -1, 0, WNOHANG ) > 0 )
	{
		/* NOOP */;
	}

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Find the next token in a string (reentrant). pls refer to ANSI strtok()
 *
 * Parameters
 *		sStr:			string to break into tokens
 *		sDelimiters:	set of delimiter characters
 *		ppLast:			pointer to serve as string index. It can be NULL.
 *
 ******************************************************************************/
char* osStrTok_r(char* sStr, const char* sDelimiters, char** ppLast)
{
	return strtok_r( sStr, sDelimiters, ppLast );
}

/*******************************************************************************
 * Description
 *		Get the size of a file.
 *
 * Parameters
 *		sFileName:	file name
 *
 * Returns
 *		* ARC_COM_ERROR(-1):	file does not exist or is not readable
 *		* others:				size of file
 *
 ******************************************************************************/
long osFileSize ( const char* sFileName )
{
	struct stat	stBuf;

	if (sFileName == ARC_COM_NULL)
		return ARC_COM_ERROR;

	if (stat(sFileName, &stBuf) < 0)
		return ARC_COM_ERROR;

	return (long)stBuf.st_size;
}

/*******************************************************************************
 * Description
 *		Print formatted output to the standard output stream. pls refer to ANSI printf()
 *
 ******************************************************************************/
int osPrintf(const char* sFmt, ...)
{
	va_list	vlVars;
	char	sBuf[512];
	int		iRet;

	if (sFmt == ARC_COM_NULL)
		return 0;

	sBuf[sizeof(sBuf)-1] = '\0';

	va_start(vlVars,sFmt);

	iRet = vsnprintf(sBuf,sizeof(sBuf),sFmt,vlVars);

	va_end(vlVars);

	if (sBuf[sizeof(sBuf)-1] != '\0')
		printf("NOTE: osPrintf() overflow!!!\n");

	sBuf[sizeof(sBuf)-1] = '\0';

	printf("%s",sBuf);

	fflush(stdout);

	return iRet;
}

/*******************************************************************************
 * Description
 *		Print formatted output to CONSOLE
 *
 ******************************************************************************/
int osPrintfConsole(const char* sFmt, ...)
{
	va_list		vlVars;
	char		sBuf[512];
	int			iRet;
	FILE*		fnOut;

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
		fprintf( fnOut, "NOTE: osPrintfConsole() overflow!!!\n");
	}

	sBuf[sizeof(sBuf)-1] = '\0';

	fprintf( fnOut, "%s", sBuf );
	fflush( fnOut );
	fclose( fnOut );

	return iRet;
}

/*******************************************************************************
 * Description
 *		Adapt TTY for a process. If a process is created by init task (PID=1),
 *		its TTY will be re-directed to console. If not, nothing to do.
 *
 * Parameters
 *
 * Returns
 *		* ARC_COM_OK(0):		adapt TTY successfully
 *		* ARC_COM_ERROR(-1):	reopen TTY failed
 *
 * Note
 *		* This function close standard input, output and error first.
 *		  And then new open again with new TTY path name
 *
 ******************************************************************************/
long osIoAdaptTty( void )
{
	/* reopen TTY if init task start this task, or print-out will disappear */
	if (osProcParentId() == VAL_OS_PROC_ROOT_ID)
		return osIoReopenTty( 0 );

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Reopen TTY for a process. This function is for initialization task to
 *		print out to CONSOLE normally.
 *
 * Parameters
 *		bTty:	0-/dev/console, others-/dev/tty
 *
 * Returns
 *		* ARC_COM_OK(0):		reopen TTY successfully
 *		* ARC_COM_ERROR(-1):	reopen TTY failed
 *
 * Note
 *		* This function close standard input, output and error first.
 *		  And then new open again with new TTY path name
 *
 ******************************************************************************/
long osIoReopenTty( long bTty )
{
	int		iStdin, iStdout, iStderr;

	close( 0 );
	close( 1 );
	close( 2 );

	if (bTty )
	{
		iStdin  = open( "/dev/tty", O_RDONLY );
		iStdout = open( "/dev/tty", O_WRONLY );
		iStderr = open( "/dev/tty", O_WRONLY );
	}
	else
	{
		iStdin  = open( "/dev/console", O_RDONLY );
		iStdout = open( "/dev/console", O_WRONLY );
		iStderr = open( "/dev/console", O_WRONLY );
	}

	if (iStdin < 0 || iStdout < iStdin || iStderr < 0)
		return ARC_COM_ERROR;

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		get the syslog content
 *
 * Parameters
 *		ppPtr:		pointer to a pointer for being filled with an address of returned buffer.
 *					In this function will allocated a block of buffer
 *					and return allocated buffer to this arguments.
 *					THE CALLER MUST FREE THIS BUFFER AFTER USE.
 *		lMaxRecord:	max entries of syslog records.
 * Returns
 *		* ARC_COM_ERROR(-1):	failed or ppPtr is NULL or lMaxRecord is less than 1 or no record or system error.
 *		* others:				length of returned buffer
 *
 * Note
 *		* THE CALLER MUST FREE THIS BUFFER AFTER USE.
 *		* this function refers:
 *			1. umng_syslog_cli show | head -n <row_num>
 *
 ******************************************************************************/
long osSysLogGet( char** ppPtr, long lMaxRecord )
{
	char	sCmd[128];
	char	sTmpFn[32];
	FILE*	pFile = 0;
	char	sLineBuf[128];
	long	lCnt;
	char*	pPtr = 0;
	long	lRet = ARC_COM_ERROR;

	if (ppPtr == ARC_COM_NULL || lMaxRecord < 0)
		return ARC_COM_ERROR;

	if (lMaxRecord == 0)
		return 0;

	sprintf( sTmpFn, "/tmp/%ld.XXXXXX", (long)getpid() );
	close(mkstemp(sTmpFn));

	sprintf( sCmd, "umng_syslog_cli show | head -n %ld > %s", lMaxRecord, sTmpFn );

	if (osSystem( sCmd ) != ARC_COM_OK)
		return ARC_COM_ERROR;

	if ( (lCnt=osFileSize( sTmpFn )) <= 0 )
		goto osSysLogGet_Return;

	if ( (pPtr=malloc(lCnt+lMaxRecord)) == ARC_COM_NULL )
		goto osSysLogGet_Return;

	if ( (pFile=fopen(sTmpFn,"r")) == ARC_COM_NULL )
		goto osSysLogGet_Return;

	pPtr[0] = '\0';
	while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
	{
		/**/
		utilStrRTrim( sLineBuf );
		/**/
		strcat( pPtr, sLineBuf );
		strcat( pPtr, "\\n" );
	}

	lRet = ARC_COM_OK;

osSysLogGet_Return:

	if (pFile != ARC_COM_NULL)
		fclose( pFile );

	unlink( sTmpFn );

	if (lRet != ARC_COM_OK)
	{
		if (pPtr != ARC_COM_NULL)
			free( pPtr );
		return ARC_COM_ERROR;
	}
	else
	{
		*ppPtr = pPtr;
		return lCnt;
	}
}

/*******************************************************************************
 * Description
 *		Get the ID of this process
 *
 * Parameters
 *
 * Returns
 *		* the ID of this process
 *
 * See Also
 *		osProcCreate
 *
 ******************************************************************************/
long osProcId( void )
{
	return (long)getpid();
}

/*******************************************************************************
 * Description
 *		Get the ID of parent process
 *
 * Parameters
 *
 * Returns
 *		* the ID of parent process
 *
 * See Also
 *		osProcId
 *
 ******************************************************************************/
long osProcParentId( void )
{
	return (long)getppid();
}

/*******************************************************************************
 * Description
 *		Create a process and run a function
 *
 * Parameters
 *		fnEntry:	entry function of new-created process
 *		lStackSz:	the size of stack. 0 or MIN_OS_PROC_STACK ~ MAX_OS_PROC_STACK.
 *					0 means default size, DEF_OS_PROC_STACK.
 *		bSuspend:	suspend the spawn process or not
 *						ARC_COM_TRUE	(suspend it after spwaning)
 *						others		(not suspend it after spwaning)
 *		pArg:		pointer to data structure of arguments passing into fnEntry. It may be null.
 *		lArgSz:		size of pArg
 *
 * Returns
 *		* >  0:	successfully, return process ID
 *		* <= 0:	failed
 *
 * Note
 *		* Default scheduling priority of new spawn process is DEF_OS_PROC_PRI.
 *		  Please use osProcPrioritySet() to change it
 *		* Default stack size of new spawn process is DEF_OS_PROC_STACK.
 *		  It is just the stack size of main thread in the new process.
 *		* A NEW-CREATED-SUSPENDED CHILD (bSuspend is true) MAY BE NOT PUT INTO TO STOPPEND QUEUE AFTER CREATION.
 *		  SO PLEASE CALL osProcStat TO CONFIRM IF THIS CHILD IS SUSPENDED AND THEN DO osProcResume
 *
 * See Also
 *		osProcDelete, osProcSuspend, osProcResume, osProcStat,
 *		osProcPrioritySet, osProcStackSet
 *
 ******************************************************************************/
long osProcSpawn(fnProc fnEntry, long lStackSz, int bSuspend, void* pArg, unsigned long lArgSz)
{
	long		iCPid;
	int			iErrNo;
	void*		pStack;
	_osProcCb*	pCb;
	long		lCnt;
	long		lPriority;
	static int	bTtyInited = 0;

	if ( fnEntry == ARC_COM_NULL )
		return ARC_COM_ERROR;

	if ( lStackSz == 0 )
		lStackSz = DEF_OS_PROC_STACK;

	if ( !CHK_OS_PROC_STACK(lStackSz) || lArgSz > MAX_OS_PROC_ARGSZ )
		return ARC_COM_ERROR;

	if (bTtyInited == ARC_COM_FALSE)
	{
		osIoAdaptTty();
		bTtyInited = ARC_COM_TRUE;
	}

	if ( (pStack = malloc( (size_t)lStackSz )) == ARC_COM_NULL )
		return ARC_COM_ERROR;

	if ( pArg == ARC_COM_NULL )
		lArgSz = 0;

	if ( (pCb = malloc( lArgSz+sizeof(_osProcCb) )) == ARC_COM_NULL)
	{
		free( pStack );
		return ARC_COM_ERROR;
	}

	pCb->lProcId = 0;
	pCb->fnEntry  = fnEntry;
	pCb->bSuspend = (unsigned long)bSuspend;
	pCb->pStack = pStack;

	if ( lArgSz > 0 )
		memcpy( pCb->byData, pArg, (size_t)lArgSz );

	signal( SIGCHLD, _osProcChldHandler );

    iCPid = clone( _osProcSpwanEntry, ((char*)pStack+lStackSz-1), OS_PROC_SPAWN_FLAG, pCb );

	if (iCPid <= 0)
	{
		iErrNo = errno;
		free( pStack );
		free( pCb );
		switch (iErrNo)
		{
		  case EAGAIN:
		  case ENOMEM:
				return ARC_COM_ERROR;
		  default:
				return ARC_COM_ERROR;
		}
	}
	else
	{
		pCb->lProcId = (unsigned long)iCPid;
		_osProcLogChild( pCb );
		/* wait until this process is really suspended */
		if (bSuspend == ARC_COM_TRUE)
		{
			lPriority = osProcPrioritySelf();
			osProcPrioritySetSelf( VAL_OS_PROC_PRI_LOWEST );
			for (lCnt=0; lCnt<10; lCnt++)
			{
				if (osProcStat( iCPid ) == eOsProcStat_suspended)
					break;
				osProcSleep( 0 );
			}
			osProcPrioritySetSelf( lPriority );
		}
	}

	return iCPid;
}

/*******************************************************************************
 * Description
 *		Delete a process
 *
 * Parameters
 *		iPid:	process ID returned at invoking osProcCreate()
 *
 * Returns
 *		* ARC_COM_OK(0):	successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * See Also
 *		osProcCreate
 *
 ******************************************************************************/
long osProcDelete( long iPid )
{
	if (osProcStat(iPid) == eOsProcStat_error)
		return ARC_COM_ERROR;

	if (kill(iPid,SIGKILL) != 0)
		return ARC_COM_ERROR;

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Delete calling process
 *
 * Parameters
 *		iRetCode:	exit code while deleting self. Only the lower 8-bits is used.
 *
 * Returns
 *
 * See Also
 *		osProcCreate, osProcDelete
 *
 ******************************************************************************/
void osProcDeleteSelf( long iExitCode )
{
	_exit( iExitCode );
}

/*******************************************************************************
 * Description
 *		Suspend a runnable process
 *
 * Parameters
 *		iPid:	process ID returned at invoking osProcCreate()
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *		* A process can be suspended only when it is in ready state.
 *
 * See Also
 *		osProcCreate, osProcResume
 *
 ******************************************************************************/
long osProcSuspend( long iPid )
{
	switch (osProcStat(iPid))
	{
	  case eOsProcStat_error:
			return ARC_COM_ERROR;
	  case eOsProcStat_ready:
			if (kill(iPid,SIGSTOP) != 0)
				return ARC_COM_ERROR;
			return ARC_COM_OK;
	  default:
			return ARC_COM_ERROR;
	}
}

/*******************************************************************************
 * Description
 *		Suspend current running process
 *
 * Parameters
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *
 * See Also
 *		osProcSuspend, osProcResume
 *
 ******************************************************************************/
long osProcSuspendSelf( void )
{
	osProcSuspend( osProcId() );

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Resume a suspended process
 *
 * Parameters
 *		iPid:	process ID returned at invoking osProcCreate()
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *		* A process can be resumed only when it is in suspended state.
 *
 * See Also
 *		osProcCreate, osProcSuspend
 *
 ******************************************************************************/
long osProcResume( long iPid )
{
	switch (osProcStat(iPid))
	{
	  case eOsProcStat_error:
			return ARC_COM_ERROR;
	  case eOsProcStat_suspended:
			if (kill(iPid,SIGCONT) != 0)
				return ARC_COM_ERROR;
			return ARC_COM_OK;
	  default:
			return ARC_COM_ERROR;
	}
}

/*******************************************************************************
 * Description
 *		Let the calling process sleep a while, in milli-second
 *
 * Parameters
 *		iMilliSecond:	the sleeping time, in milli-seconds.
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *		* If iMilliSecond is less than 10 milli-seconds,
 *		  system will adapt it to that value.
 *
 * See Also
 *		osProcSleepSec
 *
 ******************************************************************************/
long osProcSleep( unsigned long iMilliSecond )
{
	long	iSec;
	long	iUSec;

	if (iMilliSecond == 0)
		iMilliSecond = 1;

	iSec = iMilliSecond / 1000;
	iUSec = (iMilliSecond % 1000) * 1000;

	if (iSec > 0)
		if (sleep(iSec) != 0)
			return ARC_COM_ERROR;

	if (iUSec > 0)
		if (usleep(iUSec) != 0)
			return ARC_COM_ERROR;

	return ARC_COM_OK;
}


/*******************************************************************************
 * Description
 *		Let the calling process sleep a while, in second.
 *
 * Parameters
 *		sec:   second
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *
 * See Also
 *		osProcSleep
 *
 ******************************************************************************/
long osProcSleepSec( int sec )
{
    if (sec <= 0)
    {
        return ARC_COM_ERROR;
    }

    if (sleep(sec) != 0)
		return ARC_COM_ERROR;

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Get the running state of a process
 *
 * Parameters
 *		iPid:	process ID returned at invoking osProcCreate()
 *
 * Returns
 *		* eOsProcStat_error:	iPid is error
 *		* eOsProcStat_unknown:	process is in a unknown state
 *		* others:				running state of process
 *
 * See Also
 *		osProcCreate
 *
 ******************************************************************************/
enum eOsProcStat osProcStat(long iPid)
{
	FILE*	pfStat;
	char	sTmp[80];
	char*	sPtr;
	char*	sLast;

	if (!CHK_OS_PROC_ID(iPid))
		return eOsProcStat_error;

	sprintf( sTmp, OS_PROC_DIR "%ld/" OS_PROC_STAT_FILE, iPid );
	if ( (pfStat = fopen( sTmp, "r" ) ) == ARC_COM_NULL)
		return eOsProcStat_error;

	if (fgets(sTmp,sizeof(sTmp),pfStat) == ARC_COM_NULL)
	{
		fclose(pfStat);
		return eOsProcStat_error;
	}

	fclose(pfStat);

	/* second field, /proc/<PID>/stat format: "PID (name) state ..." */
	sTmp[sizeof(sTmp)-1] = '\0';
	if ( (sPtr = osStrTok_r(sTmp," ",&sLast)) == ARC_COM_NULL)
		return eOsProcStat_error;
	if ( (sPtr = osStrTok_r(sLast," ",&sLast)) == ARC_COM_NULL)
		return eOsProcStat_error;
		/**/
	switch (toupper(*sPtr)) {
	  case 'R':	return eOsProcStat_ready;
	  case 'S': return eOsProcStat_sleeping;
	  case 'T': return eOsProcStat_suspended;
	  case 'D': return eOsProcStat_blocked;
	  case 'Z': return eOsProcStat_hung;
	  default:	return eOsProcStat_unknown;
	}
}

/*******************************************************************************
 * Description
 *		Get the name of a process
 *
 * Parameters
 *		iPid:	process ID returned at invoking osProcCreate()
 *		sBuf:	pointer to a user-offered buffer for filling with process name.
 *		iBufSz:	size of sBuf. it MUST be greater than 1.
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *		* If the length of process name is equal to or greater than iBufSz,
 *		  this function will copy the first (iBufSz-1) bytes into sBuf with
 *		  an ending '\0'.
 *
 * See Also
 *		osProcCreate
 *
 ******************************************************************************/
long osProcName(long iPid, char* sBuf, unsigned long iBufSz)
{
	FILE*	pfStat;
	char	sTmp[80];
	char*	sPtr;
	char*	sLast;

	if (!CHK_OS_PROC_ID(iPid))
		return ARC_COM_ERROR;

	if (iBufSz <= 1)
		return ARC_COM_ERROR;

	if (sBuf == ARC_COM_NULL)
		return ARC_COM_ERROR;

	sprintf( sTmp, OS_PROC_DIR "%ld/" OS_PROC_STAT_FILE, iPid );
	if ( (pfStat = fopen( sTmp, "r" ) ) == ARC_COM_NULL)
		return ARC_COM_ERROR;

	if (fgets(sTmp,sizeof(sTmp),pfStat) == ARC_COM_NULL)
	{
		fclose(pfStat);
		return ARC_COM_ERROR;
	}

	fclose(pfStat);

	/* second field, /proc/<PID>/stat format: "PID (name) state ..." */
	sTmp[sizeof(sTmp)-1] = '\0';
	if ( (sPtr = osStrTok_r(sTmp," ",&sLast)) == ARC_COM_NULL)
		return ARC_COM_ERROR;
	if ( (sPtr = osStrTok_r(sLast," ",&sLast)) == ARC_COM_NULL)
		return ARC_COM_ERROR;
		/**/
	if (strlen(sPtr) <= 2)
		return ARC_COM_ERROR;
	sPtr[strlen(sPtr)-2] = '\0';
	utilStrMCpy( sBuf, sPtr+1, iBufSz );

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Get the stack size of a process' main thread
 *
 * Parameters
 *
 * Returns
 *		* MIN_OS_PROC_STACK ~ MAX_OS_PROC_STACK:	successfully. Please see Note
 *		* ARC_COM_ERROR(-1):						failed
 *
 * Note
 *		* Because process priority may be a negative value, please use
 *		  CHK_OS_PROC_PRI() to check if return value is a valid priority
 *		  or an error code.
 *
 * See Also
 *		osProcCreate, osProcStackSizeSet
 *
 ******************************************************************************/
long osProcStackSize(void)
{
	struct rlimit	stStack;

	if (getrlimit(RLIMIT_STACK,&stStack) != 0)
		return ARC_COM_ERROR;

	return (long)stStack.rlim_cur;
}

/*******************************************************************************
 * Description
 *		Set the stack size of a process' main thread
 *
 * Parameters
 *		lStackSz:	the size of stack, MIN_OS_PROC_STACK ~ MAX_OS_PROC_STACK.
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * See Also
 *		osProcCreate, osProcStackSize
 *
 ******************************************************************************/
long osProcStackSizeSet(long lStackSz)
{
	struct rlimit	stStack;

	if (!CHK_OS_PROC_PRI(lStackSz))
		return ARC_COM_ERROR;

	stStack.rlim_max = 0x7fffffff;

	if (getrlimit(RLIMIT_STACK,&stStack)!=0 || stStack.rlim_cur!=(unsigned long)lStackSz)
	{
		stStack.rlim_cur = (unsigned long)lStackSz;
		if (setrlimit(RLIMIT_STACK,&stStack) != 0)
			return ARC_COM_ERROR;
	}

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Get the priority of a process
 *
 * Parameters
 *		iPid:	process ID returned at invoking osProcCreate()
 *
 * Returns
 *		* MIN_OS_PROC_PRI ~ MAX_OS_PROC_PRI}:	successfully. Please see Note
 *		* ARC_COM_ERROR(-1):					failed
 *
 * Note
 *		* Because process priority may be a negative value, please use
 *		  CHK_OS_PROC_PRI() to check if return value is a valid priority
 *		  or an error code.
 *
 * See Also
 *		osProcCreate, osProcPrioritySet
 *
 ******************************************************************************/
long osProcPriority(long iPid)
{
	long	iPriority;

	if (!CHK_OS_PROC_ID(iPid))
		return ARC_COM_ERROR;

	errno = 0;

	iPriority = getpriority(PRIO_PROCESS,iPid);

	switch (errno != 0)
	{
	  case 0:		return iPriority;
	  case ESRCH:	return ARC_COM_ERROR;
	  default:		return ARC_COM_ERROR;
	}
}

/*******************************************************************************
 * Description
 *		Set the priority of a process
 *
 * Parameters
 *		iPid:	process ID returned at invoking osProcCreate()
 *		iPri:	new priority for a process, MIN_OS_PROC_PRI ~ MAX_OS_PROC_PRI.
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * See Also
 *		osProcCreate, osProcPriority
 *
 ******************************************************************************/
long osProcPrioritySet(long iPid, long iPri)
{
	if (!CHK_OS_PROC_ID(iPid))
		return ARC_COM_ERROR;

	if (!CHK_OS_PROC_PRI(iPri))
		return ARC_COM_ERROR;

	if (setpriority(PRIO_PROCESS,iPid,iPri) != 0)
	{
		if (errno == ESRCH)
				return ARC_COM_ERROR;
		  else	return ARC_COM_ERROR;
	}
	else
	{
		return ARC_COM_OK;
	}
}



/***************** Time functions ***************************/


/*******************************************************************************
 *                           Internal functions
 ******************************************************************************/

void _osTimeFloatSec( unsigned long* pulSec, unsigned long* pulUSec )
{
  #if 0
	FILE*	pfUptime;
	char	sTmp[64];
	char*	sLast;
	char*	sInt;
	char*	sFrac;

	if (pulSec != ARC_COM_NULL)
		*pulSec = 0;

	if (pulUSec != ARC_COM_NULL)
		*pulUSec = 0;

	pfUptime = fopen(OS_TIME_UPTIME_FILE,"r");

	if (pfUptime == ARC_COM_NULL)
		return;

	sTmp[sizeof(sTmp)-1] = '\0';

	if (fgets(sTmp,sizeof(sTmp)-1,pfUptime) == ARC_COM_NULL) {
		fclose(pfUptime);
		return;
	}

	fclose(pfUptime);

	sInt = osStrTok_r(sTmp,". ",&sLast);
	sFrac = osStrTok_r(sLast,". ",&sLast);

	if (sInt==ARC_COM_NULL || sFrac==ARC_COM_NULL)
		return;

	if (pulSec != ARC_COM_NULL)
		*pulSec = strtoul(sInt,ARC_COM_NULL,10);

	if (pulUSec != ARC_COM_NULL)
		*pulUSec = strtoul(sFrac,ARC_COM_NULL,10)*10000;

  #else

	struct timespec	tm_buf;

	clock_gettime( CLOCK_MONOTONIC, &tm_buf );

	if (pulSec != ARC_COM_NULL)
		*pulSec = (unsigned long)tm_buf.tv_sec;

	if (pulUSec != ARC_COM_NULL)
		*pulUSec = (unsigned long)tm_buf.tv_nsec/1000;

  #endif
}


/*******************************************************************************
 * Description
 *		Convert milli-seconds to ticks
 *
 * Parameters
 *		ulMSec:		milli-seconds
 *
 * Returns
 *		system ticks
 *
 * Note
 *
 * See Also
 *
 ******************************************************************************/
unsigned long osTimeMSec2Tick(unsigned long ulMSec)
{
	return ulMSec*osTimeTicksPerSec() / 1000;
}

/*******************************************************************************
 * Description
 *		Gets the system time. pls refer to ANSI time()
 *
 * Note
 *		* This routine returns the implementation's best approximation of
 *		  current calendar time in seconds. If timer is non-NULL, the return
 *		  value is also copied to the location timer points to.
 *		* The current calendar time in seconds, or -1 if the calendar time is not available
 *
 ******************************************************************************/
time_t osTime(time_t* pTimer)
{
	time_t	tmTmp;

	tmTmp = time(&tmTmp);

	if (pTimer != ARC_COM_NULL)
		*pTimer = tmTmp;

	return tmTmp;
}

/*******************************************************************************
 * Description
 *		Get the number of ticks per second
 *
 * Parameters
 *
 * Returns
 *		the number of ticks per second
 *
 * Note
 *
 * See Also
 *
 ******************************************************************************/
unsigned long osTimeTicksPerSec(void)
{
	return (unsigned long)CONFIG_HZ; /*defined in <linux/autoconf.h>. Now, it's 1000.*/
}

/*******************************************************************************
 * Description
 *		Get system up time, in ticks.
 *
 * Parameters
 *
 * Returns
 *		the system up time in ticks
 *
 * Note
 *
 * See Also
 *		osTimeMSec
 *
 ******************************************************************************/
unsigned long osTimeTick(void)
{
	unsigned long	ulSec;
	unsigned long	ulUSec;
	unsigned long	ulTickRate;

	_osTimeFloatSec(&ulSec,&ulUSec);

	ulTickRate = osTimeTicksPerSec();

	return ulSec*ulTickRate + ulUSec*ulTickRate/1000000UL;
}

/*******************************************************************************
 * Description
 *		Get system up time, in milli-seconds.
 *
 * Parameters
 *
 * Returns
 *		* system up time in milli-seconds.
 *
 * See Also
 *		osTimeTick
 *
 ******************************************************************************/
unsigned long osTimeMSec(void)
{
	unsigned long	ulSec;
	unsigned long	ulUSec;

	_osTimeFloatSec(&ulSec,&ulUSec);

	return ulSec*1000UL + ulUSec/1000UL;
}

/*******************************************************************************
 * Description
 *		Get system up time, in centi-seconds.
 *
 * Parameters
 *
 * Returns
 *		the system up time in centi-seconds
 *
 * Note
 *
 * See Also
 *		osTimeMSec
 *
 ******************************************************************************/
unsigned long osTimeCSec(void)
{
	unsigned long	ulSec;
	unsigned long	ulUSec;

	_osTimeFloatSec(&ulSec,&ulUSec);

	return ulSec*100UL + ulUSec/10000UL;
}

/*******************************************************************************
 * Description
 *		Get system up time, in seconds.
 *
 * Parameters
 *
 * Returns
 *		the system up time in seconds
 *
 * Note
 *
 * See Also
 *		osTimeMSec
 *
 ******************************************************************************/
unsigned long osTimeSec(void)
{
	unsigned long	ulSec;

	_osTimeFloatSec(&ulSec,ARC_COM_NULL);

	return ulSec;
}

/*******************************************************************************
 * Description
 *		Check if time is up.
 *
 * Parameters
 *		ulRef:		the starting time, or called reference time
 *		ulPeriod:	time period to expired
 *		ulNow:		the time to be checked
 *
 * Returns
 *		* ARC_COM_FALSE:	time is not up yet.
 *		* ARC_COM_TRUE:		time is up.
 *
 * See Also
 *
 ******************************************************************************/
unsigned long osTimeIsUp(unsigned long ulRef, unsigned long ulPeriod, unsigned long ulNow)
{
	unsigned long	ulTimeup;

	ulTimeup = ulRef + ulPeriod;

	if ( ulTimeup >= ulRef )
	{
		if ( ulNow >= ulTimeup ) /*ulRef <= uiTimeup <= ulNow*/
		{
			return ARC_COM_TRUE;
		}
		else if ( ulNow < ulRef ) /*NOW is time-line wrap*/
		{
			if ( ulNow < ulPeriod )
			{
				return ARC_COM_FALSE;
			}
			else
			{
				return ARC_COM_TRUE;
			}
		}
		else /*ulRef <= ulNow < uiTimeup*/
		{
			return ARC_COM_FALSE;
		}
	}
	else /*Timeup is time-line wrap*/
	{
		if ( ulNow >= ulRef ) /*NOW is still not time-line wrap*/
		{
			return ARC_COM_FALSE;
		}
		else /*NOW is also time-line wrap*/
		{
			if ( ulNow < ulTimeup )
			{
				return ARC_COM_FALSE;
			}
			else
			{
				return ARC_COM_TRUE;
			}
		}
	}
}



/********************* Semaphore function *************************/


typedef struct {
	int			valid;
	long		usrId;
	long		ownerProcId;
	long		ownerThreadId;
	long		waitNum;
} stOsItcMap;


#define		ITC_TYPE_MUTEX			1

#define		ITC_SYS_MUTEX			(&_stOsItcSysMutex)
#define		ITC_USR_MUTEX(idx)		(&_stOsItcMutex[idx])
#define		ITC_USR_MUTEX_MAP(idx)	(&_stOsItcMutexMap[idx])

#define		ITC_NO_OWNER			(-1)


int				_bOsItcInited = ARC_COM_FALSE;

pthread_mutex_t		_stOsItcSysMutex;

stOsItcMap			_stOsItcMutexMap[VAL_OS_ITC_MUTEX_NUM];
pthread_mutex_t		_stOsItcMutex[VAL_OS_ITC_MUTEX_NUM];
long				_giOsItcMutexLast;

unsigned long				_glItcDebugLevel = 0;
#define			ITC_DEBUG( level, ... )										\
				{															\
					if ( level <= _glItcDebugLevel )						\
					{														\
						osPrintf( "%s::%s(): ", __FILE__, __FUNCTION__  );	\
						osPrintf( __VA_ARGS__ );							\
					}														\
				}



/*******************************************************************************
 *                           Internal functions
 ******************************************************************************/

void _osItcInit(void)
{
	if (_bOsItcInited == ARC_COM_FALSE) {
		/**/
		osBZero(_stOsItcMutexMap,sizeof(_stOsItcMutexMap));
		_giOsItcMutexLast = -1;
		/**/
		pthread_mutex_init(ITC_SYS_MUTEX,ARC_COM_NULL);
		/**/
		_bOsItcInited = ARC_COM_TRUE;
	}
}

void _osItcShutdown(void)
{
	int		iCnt;

	if (_bOsItcInited == ARC_COM_TRUE) {
		/**/
		pthread_mutex_destroy(ITC_SYS_MUTEX);
		/**/
		for (iCnt=0; iCnt<=_giOsItcMutexLast; iCnt++) {
			if (_stOsItcMutexMap[iCnt].valid == ARC_COM_TRUE)
				osItcMutexDelete(_stOsItcMutexMap[iCnt].usrId);
		}
		/**/
		_bOsItcInited = ARC_COM_FALSE;
	}
}

long _osItcMutexLock(pthread_mutex_t* pMutex)
{
	_osItcInit();

	if (pMutex == ARC_COM_NULL)
		return ARC_COM_ERROR;

	if (pthread_mutex_lock(pMutex) != 0)
		return ARC_COM_ERROR;

	return ARC_COM_OK;
}

long _osItcMutexTryLock(pthread_mutex_t* pMutex)
{
	_osItcInit();

	if (pMutex == ARC_COM_NULL)
		return ARC_COM_ERROR;

	switch (pthread_mutex_trylock(pMutex))
	{
	  case 0:
			return ARC_COM_OK;
	  case EBUSY:
			return ARC_COM_ERROR;
	  default:
			return ARC_COM_ERROR;
	}
}

long _osItcMutexUnlock(pthread_mutex_t* pMutex)
{
	_osItcInit();

	if (pMutex == ARC_COM_NULL)
		return ARC_COM_ERROR;

	if (pthread_mutex_unlock(pMutex) != 0)
		return ARC_COM_ERROR;

	return ARC_COM_OK;
}

stOsItcMap* _osItcMapData(long iType, long usrId, long* iIdx)
{
	long		iCnt;
	long		iLast;
	long		iChk;
	stOsItcMap*	pMap;

	_osItcInit();

	switch (iType) {
	  case ITC_TYPE_MUTEX:
				iChk = CHK_OS_ITC_MUTEX_ID(usrId);
				iLast = _giOsItcMutexLast;
				pMap = _stOsItcMutexMap;
				break;
	  default:
				return ARC_COM_NULL;
	}

	if (!iChk)
		return ARC_COM_NULL;

	for (iCnt=0; iCnt<=iLast; iCnt++,pMap++) {
		if (pMap->valid == ARC_COM_FALSE)
			continue;
		if (pMap->usrId == usrId) {
			if (iIdx != ARC_COM_NULL)
				*iIdx = iCnt;
			return pMap;
		}
	}

	return ARC_COM_NULL;
}

long _osItcMapIdx(long iType, long usrId)
{
	long	iIdx;

	if (_osItcMapData(iType,usrId,&iIdx) != ARC_COM_NULL)
		return iIdx;

	return ARC_COM_ERROR;
}

long _osItcMapGetFree(long iType)
{
	long		iCnt;
	long		iMax;
	long*		piLast;
	stOsItcMap*	pMap;

	_osItcInit();

	switch (iType) {
	  case ITC_TYPE_MUTEX:
				iMax = VAL_OS_ITC_MUTEX_NUM;
				piLast = &_giOsItcMutexLast;
				pMap = _stOsItcMutexMap;
				break;
	  default:
				return ARC_COM_ERROR;
	}

	for (iCnt=0; iCnt<iMax; iCnt++,pMap++) {
		if (pMap->valid == ARC_COM_FALSE) {
			pMap->valid = ARC_COM_TRUE;
			pMap->usrId = ARC_COM_ERROR;
			pMap->ownerProcId   = ITC_NO_OWNER;
			pMap->ownerThreadId = ITC_NO_OWNER;
			pMap->waitNum = 0;
			if (iCnt > *piLast)
				*piLast = iCnt;
			return iCnt;
		}
	}

	return ARC_COM_ERROR;
}

long _osItcMapIsFree(long iType, long iIdx)
{
	long		iMax;
	stOsItcMap*	pMap;

	_osItcInit();

	switch (iType) {
	  case ITC_TYPE_MUTEX:
				iMax = VAL_OS_ITC_MUTEX_NUM;
				pMap = _stOsItcMutexMap;
				break;
	  default:
				return ARC_COM_ERROR;
	}

	if (iIdx >= iMax)
		return ARC_COM_ERROR;

	return (pMap[iIdx].valid == ARC_COM_TRUE ? ARC_COM_TRUE : ARC_COM_FALSE);
}

long _osItcMapFree(long iType, long iIdx)
{
	long*		piLast;
	stOsItcMap*	pMap;

	if (_osItcMapIsFree(iType,iIdx) != ARC_COM_FALSE)
		return ARC_COM_ERROR;

	switch (iType) {
	  case ITC_TYPE_MUTEX:
				piLast = &_giOsItcMutexLast;
				pMap = _stOsItcMutexMap;
				break;
	  default:
				return ARC_COM_ERROR;
	}

	if (pMap[iIdx].valid == ARC_COM_TRUE)
		pMap[iIdx].valid = ARC_COM_FALSE;

	if (iIdx == *piLast) {
		for (iIdx--, pMap--; iIdx>=0; iIdx--) {
			if (pMap[iIdx].valid == ARC_COM_TRUE)
				break;
		}
		if (iIdx >= 0)
			*piLast = iIdx;
	}

	return ARC_COM_OK;
}


/*******************************************************************************
 *                           Mutex
 ******************************************************************************/

/*******************************************************************************
 * Description
 *		Create a ITC mutex
 *
 * Parameters
 *		mtxId:	user-specified ID of mutex
 *
 * Returns
 *		* ARC_COM_OK:	successfully
 *		* others:	failed, pls see ERR_OS_ITC_MUTEX_xxx in errno.h.
 *					Where xxx might be ID, EXIST, NOMEM, NORSC, DEL, SYSTEM.
 *
 * See Also
 *		OsItcMutexDelete, OsItcMutexLock, OsItcMutexUnlock
 *
 ******************************************************************************/
long osItcMutexCreate(long mtxId)
{
	long	iIdx;
	long	iRet;

	if (!CHK_OS_ITC_MUTEX_ID(mtxId))
		return ARC_COM_ERROR;

	_osItcMutexLock(ITC_SYS_MUTEX);

	if (_osItcMapIdx(ITC_TYPE_MUTEX,mtxId) != ARC_COM_ERROR) {
		_osItcMutexUnlock(ITC_SYS_MUTEX);
		return ARC_COM_ERROR;
	}

	iIdx = _osItcMapGetFree(ITC_TYPE_MUTEX);

	if (iIdx == ARC_COM_ERROR) {
		_osItcMutexUnlock(ITC_SYS_MUTEX);
		return ARC_COM_ERROR;
	}

	iRet = pthread_mutex_init(ITC_USR_MUTEX(iIdx),ARC_COM_NULL);

	if (iRet < 0) {
		_osItcMapFree(ITC_TYPE_MUTEX,iIdx);
		_osItcMutexUnlock(ITC_SYS_MUTEX);
		switch (errno) {
		  case ENOMEM:		return ARC_COM_ERROR;
		  default:
							return ARC_COM_ERROR;
		}
	}

	ITC_USR_MUTEX_MAP(iIdx)->usrId = mtxId;

	_osItcMutexUnlock(ITC_SYS_MUTEX);

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Delete an ITC mutex
 *
 * Parameters
 *		mtxId:	user-specified ID of mutex
 *
 * Returns
 *		* ARC_COM_OK:	successfully
 *		* others:	failed, pls see ERR_OS_ITC_MUTEX_xxx in errno.h.
 *					Where xxx might be ID, SYSTEM.
 *
 * See Also
 *		OsItcMutexCreate
 *
 ******************************************************************************/
long osItcMutexDelete(long mtxId)
{
	long		iIdx;
	long		iRet;

	_osItcMutexLock(ITC_SYS_MUTEX);

	iIdx = _osItcMapIdx(ITC_TYPE_MUTEX,mtxId);

	if (iIdx == ARC_COM_ERROR) {
		_osItcMutexUnlock(ITC_SYS_MUTEX);
		return ARC_COM_ERROR;
	}

	iRet = pthread_mutex_destroy(ITC_USR_MUTEX(iIdx));

	_osItcMapFree(ITC_TYPE_MUTEX,iIdx);

	_osItcMutexUnlock(ITC_SYS_MUTEX);

	if (iRet != 0)
		return ARC_COM_ERROR;

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Lock an ITC mutex
 *
 * Parameters
 *		mtxId:		user-specified ID of mutex
 *		bNoWait:	wait for mutex being unlocked.
 *						ARC_COM_FALSE	(wait),
 *						others		(never wait)
 *
 * Returns
 *		* ARC_COM_OK:	successfully
 *		* others:	failed, pls see ERR_OS_ITC_MUTEX_xxx in errno.h.
 *					Where xxx might be ID, AGAIN, TOOBIG, SYSTEM.
 *
 * See Also
 *		OsItcMutexCreate, OsItcMutexUnlock
 *
 ******************************************************************************/
long osItcMutexLock(long mtxId, int bNoWait)
{
	long		iIdx;
	long		iProcId;
	long		iThreadId;

	_osItcMutexLock(ITC_SYS_MUTEX);

	iIdx = _osItcMapIdx(ITC_TYPE_MUTEX,mtxId);

	if (iIdx == ARC_COM_ERROR) {
		_osItcMutexUnlock(ITC_SYS_MUTEX);
		return ARC_COM_ERROR;
	}

	iProcId   = osProcId();
	iThreadId = pthread_self();

	if (ITC_USR_MUTEX_MAP(iIdx)->ownerProcId   == iProcId
	 && ITC_USR_MUTEX_MAP(iIdx)->ownerThreadId == iThreadId)
	{
		_osItcMutexUnlock(ITC_SYS_MUTEX);
		return ARC_COM_ERROR;
	}

	if (bNoWait != ARC_COM_FALSE)
	{
	  #if 0
		switch (_osItcMutexTryLock( ITC_USR_MUTEX(iIdx) ) )
		{
		  case ARC_COM_OK:
				ITC_USR_MUTEX_MAP(iIdx)->ownerProcId   = iProcId;
				ITC_USR_MUTEX_MAP(iIdx)->ownerThreadId = iThreadId;
				_osItcMutexUnlock(ITC_SYS_MUTEX);
				return ARC_COM_OK;
		  case ERR_OS_ITC_MUTEX_BUSY:
				_osItcMutexUnlock(ITC_SYS_MUTEX);
				return ERR_OS_ITC_MUTEX_AGAIN;
		  default:
				_osItcMutexUnlock(ITC_SYS_MUTEX);
				return ERR_OS_ITC_MUTEX_SYSTEM;
		}
	  #else
		if (ITC_USR_MUTEX_MAP(iIdx)->ownerProcId   != ITC_NO_OWNER
		 || ITC_USR_MUTEX_MAP(iIdx)->ownerThreadId != ITC_NO_OWNER)
		{
			_osItcMutexUnlock(ITC_SYS_MUTEX);
			return ARC_COM_ERROR;
		}

		ITC_USR_MUTEX_MAP(iIdx)->waitNum++;

		_osItcMutexUnlock(ITC_USR_MUTEX(iIdx)); /* conservatived design, prevent still be locked by another thread */

		if (_osItcMutexLock(ITC_USR_MUTEX(iIdx)) != ARC_COM_OK)
			return ARC_COM_ERROR;

		_osItcMutexLock(ITC_SYS_MUTEX);

		ITC_USR_MUTEX_MAP(iIdx)->ownerProcId   = iProcId;
		ITC_USR_MUTEX_MAP(iIdx)->ownerThreadId = iThreadId;

		ITC_USR_MUTEX_MAP(iIdx)->waitNum--;

		_osItcMutexUnlock(ITC_SYS_MUTEX);

		return ARC_COM_OK;
	  #endif
	}
	else
	{
		ITC_USR_MUTEX_MAP(iIdx)->waitNum++;

		_osItcMutexUnlock(ITC_SYS_MUTEX);

		if (_osItcMutexLock(ITC_USR_MUTEX(iIdx)) != ARC_COM_OK)
			return ARC_COM_ERROR;

		_osItcMutexLock(ITC_SYS_MUTEX);

		ITC_USR_MUTEX_MAP(iIdx)->ownerProcId   = iProcId;
		ITC_USR_MUTEX_MAP(iIdx)->ownerThreadId = iThreadId;

		ITC_USR_MUTEX_MAP(iIdx)->waitNum--;

		_osItcMutexUnlock(ITC_SYS_MUTEX);

		return ARC_COM_OK;
	}
}

/*******************************************************************************
 * Description
 *		Unlock an ITC mutex
 *
 * Parameters
 *		mtxId:	user-specified ID of mutex
 *		bForce:	forcedly or safely unlock a mutex
 *					ARC_COM_TRUE	(forced unlocking)
 *					others		(unlocking only if the owner)
 *
 * Returns
 *		* ARC_COM_OK:	successfully
 *		* others:	failed, pls see ERR_OS_ITC_MUTEX_xxx in errno.h.
 *					Where xxx might be ID, BADSTATE, SYSTEM.
 *
 * See Also
 *		OsItcMutexCreate, OsItcMutexLock
 *
 ******************************************************************************/
long osItcMutexUnlock(long mtxId, int bForce)
{
	long		iIdx;

	_osItcMutexLock(ITC_SYS_MUTEX);

	iIdx = _osItcMapIdx(ITC_TYPE_MUTEX,mtxId);

	if (iIdx == ARC_COM_ERROR) {
		_osItcMutexUnlock(ITC_SYS_MUTEX);
		return ARC_COM_ERROR;
	}

	if (ITC_USR_MUTEX_MAP(iIdx)->ownerProcId   != osProcId()
	 || ITC_USR_MUTEX_MAP(iIdx)->ownerThreadId != pthread_self() )
	{
		if (bForce != ARC_COM_TRUE) {
			_osItcMutexUnlock(ITC_SYS_MUTEX);
			return ARC_COM_ERROR;
		}
	}

	ITC_USR_MUTEX_MAP(iIdx)->ownerProcId   = ITC_NO_OWNER;
	ITC_USR_MUTEX_MAP(iIdx)->ownerThreadId = ITC_NO_OWNER;

	if (_osItcMutexUnlock(ITC_USR_MUTEX(iIdx)) != ARC_COM_OK)
	{
		_osItcMutexUnlock(ITC_SYS_MUTEX);
		return ARC_COM_ERROR;
	}

	_osItcMutexUnlock(ITC_SYS_MUTEX);

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Get the process ID which locked an ITC mutex
 *
 * Parameters
 *		mtxId:		user-specified ID of mutex
 *
 * Returns
 *		* = 0:		successfully, but not locked yet
 *		* > 0:		successfully, owner's thread ID
 *		* others:	failed, pls see ERR_OS_ITC_MUTEX_xxx in errno.h.
 *					Where xxx might be ID.
 *
 * See Also
 *		OsItcMutexCreate, OsItcMutexLock, OsItcMutexUnlock
 *
 ******************************************************************************/
long osItcMutexOwnerProc(long mtxId)
{
	long		iIdx;

	_osItcMutexLock(ITC_SYS_MUTEX);

	iIdx = _osItcMapIdx(ITC_TYPE_MUTEX,mtxId);

	if (iIdx == ARC_COM_ERROR) {
		_osItcMutexUnlock(ITC_SYS_MUTEX);
		return ARC_COM_ERROR;
	}

	_osItcMutexUnlock(ITC_SYS_MUTEX);

	return ITC_USR_MUTEX_MAP(iIdx)->ownerProcId;
}

/*******************************************************************************
 * Description
 *		Get the thread ID which locked an ITC mutex
 *
 * Parameters
 *		mtxId:		user-specified ID of mutex
 *
 * Returns
 *		* = 0:		successfully, but not locked yet
 *		* > 0:		successfully, owner's thread ID
 *		* others:	failed, pls see ERR_OS_ITC_MUTEX_xxx in errno.h.
 *					Where xxx might be ID.
 *
 * See Also
 *		OsItcMutexCreate, OsItcMutexLock, OsItcMutexUnlock
 *
 ******************************************************************************/
long osItcMutexOwnerThread(long mtxId)
{
	long		iIdx;

	_osItcMutexLock(ITC_SYS_MUTEX);

	iIdx = _osItcMapIdx(ITC_TYPE_MUTEX,mtxId);

	if (iIdx == ARC_COM_ERROR) {
		_osItcMutexUnlock(ITC_SYS_MUTEX);
		return ARC_COM_ERROR;
	}

	_osItcMutexUnlock(ITC_SYS_MUTEX);

	return ITC_USR_MUTEX_MAP(iIdx)->ownerThreadId;
}

/*******************************************************************************
 * Description
 *		Get the total number of threads which wait an ITC mutex to be unlocked
 *
 * Parameters
 *		mtxId:		user-specified ID of mutex
 *
 * Returns
 *		* >= 0:		successfully, total number of waiting processes
 *		* others:	failed, pls see ERR_OS_ITC_MUTEX_xxx in errno.h.
 *					Where xxx might be ID.
 *
 * See Also
 *		OsItcMutexCreate, OsItcMutexLock, OsItcMutexUnlock
 *
 ******************************************************************************/
long osItcMutexWaitNum(long mtxId)
{
	long		iIdx;

	_osItcMutexLock(ITC_SYS_MUTEX);

	iIdx = _osItcMapIdx(ITC_TYPE_MUTEX,mtxId);

	if (iIdx == ARC_COM_ERROR) {
		_osItcMutexUnlock(ITC_SYS_MUTEX);
		return ARC_COM_ERROR;
	}

	_osItcMutexUnlock(ITC_SYS_MUTEX);

	return ITC_USR_MUTEX_MAP(iIdx)->waitNum;
}

/*******************************************************************************
 * Description
 *		Print formatted output to CONSOLE
 *		debug_act: 0 => turn off debug mode
 *				   1 => turn on debug mode
 *
 ******************************************************************************/
int debug_act = 1;
int arc_dprintf(const char* sFmt, ...)
{
	va_list		vlVars;
	char		sBuf[512];
	int			iRet;
	FILE*		fnOut;

	if (sFmt == ARC_COM_NULL || debug_act == 0)
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
		fprintf( fnOut, "NOTE: osPrintfConsole() overflow!!!\n");
	}

	sBuf[sizeof(sBuf)-1] = '\0';

	fprintf( fnOut, "%s", sBuf );
	fflush( fnOut );
	fclose( fnOut );

	return iRet;
}

void CreatePIDFile(unsigned char strPIDFile)
{
	FILE *pidfile = NULL;

	pidfile = fopen(strPIDFile, "w");
	if (pidfile) {
		fprintf(pidfile, "%d\n", getpid());
		fclose(pidfile);
	}
}

void RemovePIDFile(unsigned char strPIDFile)
{
	remove(strPIDFile);
}
