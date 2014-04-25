/***************************************************************
  * Definition of Periodical Timer Callback Function Utilities *
  ***************************************************************/

#ifndef _ARC_COM_TMR_H_
#define _ARC_COM_TMR_H_


#define		TMR_MAX_ID				128			/*the maximum number of timer callback functions*/
#define		TMR_MAX_ARGV			9			/*the maximum number of arguments*/
#define		TMR_MIN_TMRINTERVAL		100			/*the minimum time interval of timer callback function*/
#define		TMR_LOOPFOREVER			0xFFFFFFFF	/*periodically invoke timer callback function forever*/
#define		TMR_NO_RPIORITY			0xFFFFFFFF	/*no priority adaption*/
#define		TMR_TASK_RPIORITY		0x80000000	/*caller taks's priority*/

typedef void (*fnTmrProc) ( unsigned long lArg1, ... ); /*Timer callback function. uiTmrID is the ID of registered and timeout timer*/


#ifdef __cplusplus
extern "C" {
#endif


/*Registration*/

extern long	tmrRegister( unsigned long ulTmrID, unsigned long ulMSecs, unsigned long ulLoopCnt, fnTmrProc fnProc, unsigned long ulArg1, unsigned long ulArg2, unsigned long ulArg3, unsigned long ulArg4, unsigned long ulArg5, unsigned long ulArg6, unsigned long ulArg7, unsigned long ulArg8, unsigned long ulArg9 );
extern long	tmrUnregister( unsigned long ulTmrID );
extern long	tmrRegisterPriority( unsigned long ulTmrID, unsigned long ulMSecs, unsigned long ulLoopCnt, unsigned long ulPriority, fnTmrProc fnProc, unsigned long ulArg1, unsigned long ulArg2, unsigned long ulArg3, unsigned long ulArg4, unsigned long ulArg5, unsigned long ulArg6, unsigned long ulArg7, unsigned long ulArg8, unsigned long ulArg9 );
extern long	tmrGetFreeID( void );
extern long	tmrIsFree( unsigned long ulTmrID );
extern long	tmrRegisterRunOnce( unsigned long ulMSecs, fnTmrProc fnProc, unsigned long ulArg1, unsigned long ulArg2, unsigned long ulArg3, unsigned long ulArg4, unsigned long ulArg5, unsigned long ulArg6, unsigned long ulArg7, unsigned long ulArg8, unsigned long ulArg9 );


/*init & shutdown*/

extern long	tmrInit ( void );
extern long	tmrShutdown ( void );
extern long	tmrIsInited ( void );


#ifdef __cplusplus
}
#endif


#endif  /* _ARC_COM_TMR_H_ */
