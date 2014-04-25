/*******************************************************************
  * Implementation of Periodical Timer Callback Function Utilities *
  *******************************************************************/

#include "libArcComApi.h"


/* static definition*/


typedef struct {
	unsigned long				bUsed;
	unsigned long				lTaskId;
	fnTmrProc	fnProc;
	unsigned long				ulPrevTime;	/*counted by msec*/
	unsigned long				ulPeriod;	/*counted by msec*/
	unsigned long				ulPriority;
	unsigned long				ulLoopCnt;
	unsigned long				lArgs[TMR_MAX_ARGV];
} stTmrCB;

	/* variables */
unsigned long			_glTmrInited = ARC_COM_FALSE;
stTmrCB*	_gpUtilTmrCB = ARC_COM_NULL;
long		_glTmrTaskId = 0;
long		_glTmrMutexId = 0x532;
long		_glTmrIdInRun = 0;

	/*internal functions*/

	/*internal task functions*/
void _tmrTask ( void );

long _tmrMutexLock()
{
	return osItcMutexLock( _glTmrMutexId,  ARC_COM_FALSE );
}

long _tmrMutexUnlock()
{
	return osItcMutexUnlock( _glTmrMutexId, ARC_COM_TRUE );
}


/* API declaration */


/*******************************************************************************
 * Description
 *		Initialize the utilities of periodical timer callback function.
 *
 * Parameters
 *
 * Returns
 *		* ARC_COM_OK:			initialization is successful.
 *		* ERR_COM_INITED:	utilities have ever been initialized before.
 *		* ERR_COM_NOMEMORY:	memory not enough.
 *
 * Notes
 *		* use tmrShutdown() to shutdown utilities
 *		* use tmrRegister() and tmrUnregister() to
 *		  register or unregister a timer
 *
 * See Also
 *		tmrShutdown, tmrRegister, tmrUnregister
 *
 ******************************************************************************/
long tmrInit ( void )
{
	long	lRet;
	/*long	lCnt;*/

	if (tmrIsInited() == ARC_COM_TRUE)
		return ARC_COM_ERROR;

	/**/
	_gpUtilTmrCB = osMalloc(TMR_MAX_ID*sizeof(stTmrCB));

	if (_gpUtilTmrCB == ARC_COM_NULL)
		return ARC_COM_ERROR;

	osBZero(_gpUtilTmrCB, TMR_MAX_ID*sizeof(stTmrCB));

	/**/
	lRet = osItcMutexCreate( _glTmrMutexId );

	if (lRet != ARC_COM_OK) {
		osFree(_gpUtilTmrCB);
		return lRet;
	}

	_tmrMutexLock();

	/**/
	_glTmrTaskId = osProcSpawn( (fnProc)_tmrTask
									, 32L*1024
									, ARC_COM_TRUE
									, ARC_COM_NULL
									, 0 );

	if (_glTmrTaskId <= 0) {
		osItcMutexDelete(_glTmrMutexId);
		osFree(_gpUtilTmrCB);
		return lRet;
	}

	/**/

	_glTmrInited = ARC_COM_TRUE;

	_tmrMutexUnlock();

	osProcResume( _glTmrTaskId );

	return ARC_COM_OK;
}


/*******************************************************************************
 * Description
 *		Shutdown the utilities of periodical timer callback function.
 *
 * Parameters
 *
 * Returns
 *		* ARC_COM_OK:		shutdown utilities successfully.
 *		* ERR_COM_NOTINIT:	utilities are not initialized yet.
 *
 * Notes
 *		* use tmrInit() to initialize utilities
 *
 * See Also
 *		tmrInit
 *
 ******************************************************************************/
long tmrShutdown ( void )
{
	if ( tmrIsInited() == ARC_COM_FALSE )
		return ARC_COM_ERROR;

	_glTmrInited = ARC_COM_FALSE;

	osProcSuspend( _glTmrTaskId );
	osProcDelete( _glTmrTaskId );

	osItcMutexDelete(_glTmrMutexId);

	osFree(_gpUtilTmrCB);

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Check if periodical timer callback function is initialized
 *
 * Parameters
 *
 * Returns
 *		* ARC_COM_FALSE:	periodical timer callback function is not initialized yet
 *		* ARC_COM_TRUE:		periodical timer callback function is initialized
 *
 * Note
 *
 * See Also
 *		tmrInit, tmrShutdown
 *
 ******************************************************************************/
long tmrIsInited(void)
{
	return (_glTmrInited == ARC_COM_FALSE ? ARC_COM_FALSE : ARC_COM_TRUE);
}


/*******************************************************************************
 * Description
 *		Register a periodical timer callback function.
 *
 * Parameters
 *		ulTmrID:	timer ID, 0~TMR_MAX_ID-1
 *		ulMSecs:	the time-out value, in milliseconds
 *		fnProc:		pointer to the function to be notified when the time-out
 *					value elapses. For more information about the function,
 *					see fnTmrProc.
 *		ulLoopCnt:	iteration of timer. TMR_LOOPFOREVER means periodically
 *					invoking timer callback function forever.
 *
 * Returns
 *		* ARC_COM_OK:		register timer successfully.
 *		* ARC_COM_ERROR:	utilities are not initialized yet.
 *						 or	ulTmrID is invalid.
 *						 or	fnProc is NULL.
 *						 or	ulMSecs is less than TMR_MIN_TMRINTERVAL
 *							or ulLoopCnt is 0.
 *						 or ulTmrID has been registered before.
 *						 or other system error.
 *
 * Notes
 *		* REMEMBER TO USE tmrUnregister() TO UNREGISTER TIMER
 *		* curretly, maximum number of arguments is TMR_MAX_ARGV
 *		* DO NOT use "register" in arguments
 *
 * See Also
 *		tmrUnregister
 *
 ******************************************************************************/
long tmrRegister ( unsigned long ulTmrID, unsigned long ulMSecs, unsigned long ulLoopCnt, fnTmrProc fnProc, unsigned long ulArg1, unsigned long ulArg2, unsigned long ulArg3, unsigned long ulArg4, unsigned long ulArg5, unsigned long ulArg6, unsigned long ulArg7, unsigned long ulArg8, unsigned long ulArg9 )
{
	long	lRet;

	lRet = tmrRegisterPriority(  ulTmrID
								,ulMSecs
								,ulLoopCnt
								,TMR_NO_RPIORITY
								,fnProc
								,ulArg1
								,ulArg2
								,ulArg3
								,ulArg4
								,ulArg5
								,ulArg6
								,ulArg7
								,ulArg8
								,ulArg9 );

	return lRet;
}

/*******************************************************************************
 * Description
 *		Register a periodical timer callback function.
 *
 * Parameters
 *		ulTmrID:	timer ID, 0~TMR_MAX_ID-1
 *		ulMSecs:	the time-out value, in milliseconds
 *		fnProc:		pointer to the function to be notified when the time-out
 *					value elapses. For more information about the function,
 *					see fnTmrProc.
 *		ulLoopCnt:	iteration of timer. TMR_LOOPFOREVER means periodically
 *					invoking timer callback function forever.
 *		ulPriority:	task priority when executing this callback function
 *							TMR_NO_RPIORITY					(don't change priority)
 *							VAL_OS_PROC_PRI_TIME_CRITICAL	(critical task).
 *							VAL_OS_PROC_PRI_HIGHEST			(highest priority except for critical task).
 *							VAL_OS_PROC_PRI_ABOVE_NORMAL	(priority which just higher than normal one).
 *							VAL_OS_PROC_PRI_NORMAL			(normal/general priority).
 *							VAL_OS_PROC_PRI_BELOW_NORMAL	(priority which just lower than normal one).
 *							VAL_OS_PROC_PRI_LOWEST			(lowest priority except for idle task).
 *							VAL_OS_PROC_PRI_IDLE			(execute only when CPU idle).
 *							others							(viewed as VAL_OS_PROC_PRI_NORMAL)
 *
 * Returns
 *		* ARC_COM_OK:			register timer successfully.
 *		* ARC_COM_ERROR:	utilities are not initialized yet.
 *						 or	ulTmrID is invalid.
 *						 or fnProc is NULL.
 *						 or ulMSecs is less than TMR_MIN_TMRINTERVAL
 *							or ulLoopCnt is 0 or
 *						 or ulTmrID has been registered before.
 *						 or other system error.
 *
 * Notes
 *		* REMEMBER TO USE tmrUnregister() TO UNREGISTER TIMER
 *		* curretly, maximum number of arguments is TMR_MAX_ARGV
 *		* DO NOT use "register" in arguments
 *
 * See Also
 *		tmrUnregister
 *
 ******************************************************************************/
long tmrRegisterPriority( unsigned long ulTmrID, unsigned long ulMSecs, unsigned long ulLoopCnt, unsigned long ulPriority, fnTmrProc fnProc, unsigned long ulArg1, unsigned long ulArg2, unsigned long ulArg3, unsigned long ulArg4, unsigned long ulArg5, unsigned long ulArg6, unsigned long ulArg7, unsigned long ulArg8, unsigned long ulArg9 )
{
	long			lRet;
	stTmrCB*		pCB;
	unsigned long*	plArgs;

	if ( tmrIsInited() == ARC_COM_FALSE )
	{
		if (tmrInit() != ARC_COM_OK)
			return ARC_COM_ERROR;
		return ARC_COM_ERROR;
	}

	if (ulTmrID >= TMR_MAX_ID)
		return ARC_COM_ERROR;

	if (fnProc == NULL)
		return ARC_COM_ERROR;

	if (ulMSecs<TMR_MIN_TMRINTERVAL || ulLoopCnt == 0)
		return ARC_COM_ERROR;

	if (tmrIsFree(ulTmrID) != ARC_COM_TRUE)
		return ARC_COM_ERROR;

	if ((lRet=_tmrMutexLock()) != ARC_COM_OK)
		return lRet;

	pCB = &_gpUtilTmrCB[ulTmrID];

	pCB->lTaskId = osProcId();
	pCB->fnProc = fnProc;
	pCB->ulPrevTime = osTimeMSec();
	pCB->ulPeriod = ulMSecs;
	plArgs = pCB->lArgs;
	plArgs[0] = ulArg1;
	plArgs[1] = ulArg2;
	plArgs[2] = ulArg3;
	plArgs[3] = ulArg4;
	plArgs[4] = ulArg5;
	plArgs[5] = ulArg6;
	plArgs[6] = ulArg7;
	plArgs[7] = ulArg8;
	plArgs[8] = ulArg9;
	if (ulPriority != TMR_NO_RPIORITY)
			pCB->ulPriority = ulPriority;
	  else	pCB->ulPriority = TMR_TASK_RPIORITY | (unsigned long)osProcPriority((long)pCB->lTaskId);
	pCB->ulLoopCnt = ulLoopCnt;
	pCB->bUsed = ARC_COM_TRUE;

	_tmrMutexUnlock();

	return ARC_COM_OK;
}


/*******************************************************************************
 * Description
 *		Unregister a periodical timer callback function.
 *
 * Parameters
 *		ulTmrID:		timer ID, 0~TMR_MAX_ID-1
 *
 * Returns
 *		* ARC_COM_OK:		register timer successfully.
 *		* ARC_COM_ERROR:	utilities are not initialized yet.
 *						 or	ulTmrID is invalid.
 *						 or	ulTmrID is not registered yet.
 *						 or	other system error.
 *
 * Notes
 *		* use tmrRegister() to register timer
 *
 * See Also
 *		tmrRegister
 *
 ******************************************************************************/
long tmrUnregister ( unsigned long ulTmrID )
{
	long	lRet;

	if ( tmrIsInited() == ARC_COM_FALSE )
	{
		if (tmrInit() != ARC_COM_OK)
			return ARC_COM_ERROR;
		return ARC_COM_ERROR;
	}

	if ( ulTmrID >= TMR_MAX_ID )
		return ARC_COM_ERROR;

	if (tmrIsFree(ulTmrID) != ARC_COM_FALSE)
		return ARC_COM_ERROR;

	if ((lRet=_tmrMutexLock()) != ARC_COM_OK)
		return lRet;

	_gpUtilTmrCB[ulTmrID].bUsed = ARC_COM_FALSE;

	_tmrMutexUnlock();

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Find out a free periodical timer callback entry ID.
 *
 * Parameters
 *
 * Returns
 *		* ERR_COM_NOTINIT:	utilities are not initialized yet.
 *		* ERR_COM_NOENTRY:	no free entry.
 *		* <= ERR_COM_BASE:	system error.
 *		* others:			free timer ID.
 *
 * Notes
 *		* REMEMBER TO USE tmrUnregister() TO UNREGISTER TIMER
 *		* curretly, maximum number of arguments is TMR_MAX_ARGV
 *		* use tmrGetFreeID() to get a free timer,
 *		  and then use tmrUnregister() to unregister it,
 *		  and finally use tmrRegister() or tmrRegisterPriority() to register it
 *
 * See Also
 *		tmrIsFree
 *
 ******************************************************************************/
long tmrGetFreeID( void )
{
	long		lRet;
	long		lID;
	stTmrCB*	pCB;

	if ( tmrIsInited() == ARC_COM_FALSE )
	{
		if (tmrInit() != ARC_COM_OK)
			return ARC_COM_ERROR;
	}

	if ((lRet=_tmrMutexLock()) != ARC_COM_OK)
		return lRet;

	for (lID=0; lID<TMR_MAX_ID; lID++) {
		pCB = &(_gpUtilTmrCB[lID]);
		if (pCB->bUsed == ARC_COM_FALSE)
			break;
	}

	if (lID == TMR_MAX_ID) {
		_tmrMutexUnlock();
		return ARC_COM_ERROR;
	}

	pCB->fnProc = ARC_COM_NULL;
	pCB->bUsed = ARC_COM_TRUE;

	_tmrMutexUnlock();

	return lID;
}

/*******************************************************************************
 * Description
 *		Check if a periodical timer is not in use.
 *
 * Parameters
 *
 * Returns
 *		* ARC_COM_TRUE:		timer is free.
 *		* ARC_COM_FALSE:	timer is in used
 *						or timerID is out of range.
 *						or module is not initialized yet.
 *
 * Notes
 *
 * See Also
 *		tmrGetFreeID
 *
 ******************************************************************************/
long tmrIsFree( unsigned long ulTmrID )
{
	if ( tmrIsInited() == ARC_COM_FALSE )
	{
		if (tmrInit() != ARC_COM_OK)
			return ARC_COM_ERROR;
		return ARC_COM_ERROR;
	}

	if (ulTmrID >= TMR_MAX_ID)
		return ARC_COM_FALSE;

	if (_gpUtilTmrCB[ulTmrID].bUsed == ARC_COM_FALSE)
		return ARC_COM_TRUE;

	return ARC_COM_FALSE;
}

/*******************************************************************************
 * Description
 *		Register a run-once timer callback function.
 *
 * Parameters
 *		ulMSecs:	the time-out value, in milliseconds
 *		fnProc:		pointer to the function to be notified when the time-out
 *					value elapses. For more information about the function,
 *					see fnTmrProc.
 *
 * Returns
 *		* ARC_COM_OK:			register timer successfully.
 *		* ARC_COM_ERROR:	utilities are not initialized yet.
 *						 or	fnProc is NULL.
 *						 or	ulMSecs is less than TMR_MIN_TMRINTERVAL
 *							or ulLoopCnt is 0.
 *						 or	no free entry.
 *						 or	other system error.
 *
 * Notes
 *		* use tmrUnregister() to unregister timer
 *		* curretly, maximum number of arguments is TMR_MAX_ARGV
 *		* DO NOT use "register" in arguments
 *
 * See Also
 *		tmrUnregister
 *
 ******************************************************************************/
long tmrRegisterRunOnce ( unsigned long ulMSecs, fnTmrProc fnProc, unsigned long ulArg1, unsigned long ulArg2, unsigned long ulArg3, unsigned long ulArg4, unsigned long ulArg5, unsigned long ulArg6, unsigned long ulArg7, unsigned long ulArg8, unsigned long ulArg9 )
{
	long	lRet;

	lRet = tmrGetFreeID();
	if (lRet <= ARC_COM_ERROR)
		return lRet;

	tmrUnregister(lRet);

	lRet = tmrRegisterPriority( (unsigned long)lRet
									,ulMSecs
									,1
									,TMR_NO_RPIORITY
									,fnProc
									,ulArg1
									,ulArg2
									,ulArg3
									,ulArg4
									,ulArg5
									,ulArg6
									,ulArg7
									,ulArg8
									,ulArg9 );

	return lRet;
}


/*internal timer task functions*/
void _tmrTask ( void )
{
	unsigned long	lSleep;
	unsigned long	lUsed;
	unsigned long	lNow;
	unsigned long	lCnt;
	stTmrCB*		pCB;
	unsigned long	lTaskId;
	unsigned long	lOldPriority;
	unsigned long	bPriorityChanged;
	unsigned long*	plArgs;

	lSleep = 100;

	for ( lCnt=0; lCnt<10; lCnt++ )
	{
		if ( tmrIsInited() == ARC_COM_TRUE )
			break;
		osProcSleep( 0 );
	}

	while ( ARC_COM_TRUE )
	{
		if ( tmrIsInited() == ARC_COM_FALSE ) {
			osProcDeleteSelf( 0 );
			return;
		}

		lNow = osTimeMSec();

		if (_tmrMutexLock() != ARC_COM_OK)
			continue;

		lTaskId = osProcId();
		lOldPriority = osProcPriority(lTaskId);

		for (lCnt=0; lCnt<TMR_MAX_ID ; lCnt++)
		{
			if (tmrIsFree(lCnt) != ARC_COM_FALSE)
				continue;

			pCB = &(_gpUtilTmrCB[lCnt]);

			if (pCB->fnProc == ARC_COM_NULL)
				continue;

			if (osTimeIsUp(pCB->ulPrevTime, pCB->ulPeriod, lNow) == ARC_COM_TRUE)
			{

				bPriorityChanged = ARC_COM_FALSE;
				if ((pCB->ulPriority & TMR_TASK_RPIORITY) == TMR_TASK_RPIORITY) {
					if (pCB->ulPriority != TMR_NO_RPIORITY) {
						osProcPrioritySet(lTaskId,(pCB->ulPriority&TMR_TASK_RPIORITY));
						bPriorityChanged = ARC_COM_TRUE;
					}
				} else {
					osProcPrioritySet(lTaskId,pCB->ulPriority);
					bPriorityChanged = ARC_COM_TRUE;
				}

				_glTmrIdInRun = lCnt;

				plArgs = pCB->lArgs;
				(*pCB->fnProc)(plArgs[0],plArgs[1],plArgs[2],plArgs[3],plArgs[4],plArgs[5],plArgs[6],plArgs[7],plArgs[8]);

				if (bPriorityChanged == ARC_COM_TRUE)
					osProcPrioritySet(lTaskId,lOldPriority);

				if (pCB->ulLoopCnt != TMR_LOOPFOREVER)
					pCB->ulLoopCnt--;

				if (pCB->ulLoopCnt>0 || pCB->ulLoopCnt==TMR_LOOPFOREVER)
						pCB->ulPrevTime += pCB->ulPeriod;
				  else	pCB->bUsed = ARC_COM_FALSE;
			}
		}

		_tmrMutexUnlock();

		if ( tmrIsInited() == ARC_COM_FALSE ) {
			osProcDeleteSelf( 0 );
			return;
		}

		if (osTimeMSec() < lNow) {
			osProcSleep(lSleep);
		} else {
			lUsed = osTimeMSec() - lNow;
			if (lUsed < lSleep)
					osProcSleep( lSleep - lUsed );
			  else	osProcSleep( 0 );
		}

	}

}
