#ifndef _ARC_COM_OS_H_
#define _ARC_COM_OS_H_


#include <stdlib.h>
#include <time.h>


#define		VAL_OS_PROC_ROOT_ID			1

#define		CHK_OS_PROC_ID(pid)			((long)(pid)>0) 	/*check if process ID is valid*/

#define		MIN_OS_PROC_STACK			(8*1024L)			/*minimum size of process stack, in bytes.*/
#define		MAX_OS_PROC_STACK			(256*1024L)			/*maximum size of process stack, in bytes.*/
#define		CHK_OS_PROC_STACK(sz)		((sz)>=MIN_OS_PROC_STACK && (sz)<=MAX_OS_PROC_STACK) /*check if process stack size is valid*/
#define		DEF_OS_PROC_STACK			(32*1024L)			/*default size of process stack, in bytes.*/

#define		MAX_OS_PROC_ARGSZ			(1024)				/*maximum size of process arguments, in bytes.*/

#define		MIN_OS_PROC_PRI				(-20)				/*minimum value of process priority. lower value mean higher priority*/
#define		MAX_OS_PROC_PRI				(20)				/*maximum value of process priority. higher value mean lower priority*/
#define		CHK_OS_PROC_PRI(pri)		((pri)>=MIN_OS_PROC_PRI && (pri)<=MAX_OS_PROC_PRI) /*check if process priority is valid*/

#define		VAL_OS_PROC_PRI_IDLE		MAX_OS_PROC_PRI								/*a priority for a process executing only when CPU idle*/
#define		VAL_OS_PROC_PRI_LOWEST		((MAX_OS_PROC_PRI*7+MIN_OS_PROC_PRI*1)/8)	/*lowest scheduling priority*/
#define		VAL_OS_PROC_PRI_LOWER		((MAX_OS_PROC_PRI*6+MIN_OS_PROC_PRI*2)/8)	/*lower scheduling priority*/
#define		VAL_OS_PROC_PRI_LOW			((MAX_OS_PROC_PRI*5+MIN_OS_PROC_PRI*3)/8)	/*low scheduling priority*/
#define		VAL_OS_PROC_PRI_NORMAL		((MAX_OS_PROC_PRI  +MIN_OS_PROC_PRI  )/2)	/*normal scheduling priority*/
#define		VAL_OS_PROC_PRI_HIGH		((MAX_OS_PROC_PRI*3+MIN_OS_PROC_PRI*5)/8)	/*high scheduling priority*/
#define		VAL_OS_PROC_PRI_HIGHER		((MAX_OS_PROC_PRI*2+MIN_OS_PROC_PRI*6)/8)	/*higher scheduling priority*/
#define		VAL_OS_PROC_PRI_HIGHEST		((MAX_OS_PROC_PRI*1+MIN_OS_PROC_PRI*7)/8)	/*highest scheduling priority*/
#define		VAL_OS_PROC_PRI_URGENT		MIN_OS_PROC_PRI								/*a priority for an urgetn process*/

#define		DEF_OS_PROC_PRI				VAL_OS_PROC_PRI_NORMAL	/*default priority at spawning a new process*/

#define		DEF_OS_PROC_MAXCHLD			16		/*maximum spwaned child processes by a parent process*/

#define		MIN_OS_ITC_MUTEX_ID			0x0000001L	/*minimum value of mutex ID specified by user*/
#define		MAX_OS_ITC_MUTEX_ID			0x7ffffffL	/*maximum value of mutex ID specified by user*/
#define		VAL_OS_ITC_MUTEX_NUM		32			/*maximum number of mutex.*/
#define		CHK_OS_ITC_MUTEX_ID(qId)	((qId)>=MIN_OS_ITC_MUTEX_ID && (qId)<=MAX_OS_ITC_MUTEX_ID) /*check if semaphore ID is valid*/


enum eOsProcStat {
	eOsProcStat_unknown = 0,		/*unknown state*/
	eOsProcStat_ready = 1,			/*in ready state*/
	eOsProcStat_sleeping = 2,		/*in sleeping*/
	eOsProcStat_suspended = 3,		/*suspended by others or itself*/
	eOsProcStat_blocked = 4,		/*blocked and wait for some event occuring*/
	eOsProcStat_hung = 5,			/*process is dead but not deleted yet*/
	eOsProcStat_error = 6			/*process is error*/
}; /*execution state of process*/

typedef int (*fnProc)( void* );


#ifdef __cplusplus
extern "C" {
#endif


extern void*	osCalloc( size_t count, size_t size );
extern void		osFree( void* ptr );
extern void*	osMalloc( size_t size );

extern void*	osBZero(void* pMem, size_t ulCnt);
extern char*	osStrTok_r ( char* sStr, const char* sDelimiters, char** ppLast );

extern long		osFileSize ( const char* sFileName );

extern long		osSysLogGet( char** ppPtr, long lMaxRecord );

extern int		osPrintf(const char* sFmt, ...);
extern int		osPrintfConsole(const char* sFmt, ...);
extern long		osIoAdaptTty( void );
extern long		osIoReopenTty( long bTty );
#ifdef DEBUG
  #define		osDebugPrintf	osPrintfConsole
#else
  #define		osDebugPrintf
#endif

extern long		osSystem ( char* sShellCmd );
extern long		osSystem_GetOutput ( char* sShellCmd, char* sBuf, int iBufSz );
extern long		osRunTask ( char* sShellCmd );
extern long		osRunTaskFile ( char* sScriptFile );

extern long		osProcId ( void );
extern long		osProcParentId ( void );
extern long		osProcSpawn ( fnProc fnEntry, long lStackSz, int bSuspend, void* pArg, unsigned long lArgSz );
extern long		osProcDelete ( long iPid );
extern void		osProcDeleteSelf ( long iExitCode );
extern long		osProcSuspend ( long iPid );
extern long		osProcSuspendSelf ( void );
extern long		osProcResume ( long iPid );
extern long		osProcSleep ( unsigned long iMilliSecond );
extern long		osProcSleepSec( int sec );

extern enum eOsProcStat	osProcStat ( long iPid );
extern long		osProcName ( long iPid, char* sBuf, unsigned long iBufSz );
extern long		osProcStackSize ( void );
extern long		osProcStackSizeSet ( long lStackSz );
extern long		osProcPriority ( long iPid );
extern long		osProcPrioritySet ( long iPid, long iPri );

#define			osProcStatSelf()			osProcStat(osProcId())		/*get calling process status*/
#define			osProcPrioritySelf()		osProcPriority(osProcId())	/*get calling process scheduling priority*/
#define			osProcPrioritySetSelf(pri)	osProcPrioritySet(osProcId(),(pri))	/*get calling process scheduling priority*/

extern unsigned long	osTimeMSec2Tick( unsigned long ulMSec );
extern time_t			osTime( time_t* pTimer );
extern unsigned long	osTimeTicksPerSec( void );
extern unsigned long	osTimeTick( void );
extern unsigned long	osTimeMSec( void );
extern unsigned long	osTimeCSec( void );
extern unsigned long	osTimeSec( void );
extern unsigned long	osTimeIsUp( unsigned long ulRef, unsigned long ulPeriod, unsigned long ulNow );

extern long		osItcMutexCreate ( long mtxId );
extern long		osItcMutexDelete ( long mtxId );
extern long		osItcMutexLock ( long mtxId, int bNoWait );
extern long		osItcMutexUnlock ( long mtxId, int bForce );
extern long		osItcMutexOwnerProc ( long mtxId );
extern long		osItcMutexOwnerThread ( long mtxId );
extern long		osItcMutexWaitNum ( long mtxId );

#define			osItcMutexIsLocked(mtxId)	(osItcMutexOwnerProc(mtxId) > 0)	/*check if a mutex is locked*/


extern int 		arc_dprintf(const char* sFmt, ...);
extern void CreatePIDFile(unsigned char strPIDFile);
extern void RemovePIDFile(unsigned char strPIDFile);

#ifdef __cplusplus
}
#endif


#endif /* _ARC_COM_OS_H_ */
