/******************************************************************************

                              Copyright (c) 2009
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/**
   \file drv_tapi_tone.c
   Contains TAPI Tone Services.
*/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_tapi.h"
#include "drv_tapi_errno.h"
#include "drv_tapi_cid.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/* tone coefficient maximums */
#if 1 // ctc merged charles
#define MAX_TONE_TIME          64000  /* ms */
#else
#define MAX_TONE_TIME          16383  /* ms */ /* maximum: (2^14)-1 */
#endif
#define MAX_TONE_FREQ          32768
#define MIN_TONE_POWER         -300
#define MAX_TONE_POWER         0
#define MAX_CADENCE_TIME      64000   /* ms */
#define MAX_VOICE_TIME        64000   /* ms */

#ifdef TAPI_HAVE_TIMERS
#define MAX_CMD_WORD             31
#endif /* TAPI_HAVE_TIMERS */

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* Predefined tones frequencies */
static
const IFX_uint32_t TAPI_PredefinedToneFreq [IFX_TAPI_TONE_INDEX_MIN - 1][2] =
{
    /* Freq1/Freq2(Hz) *  Freq1/Freq2(Hz) - Index - DTMF Digit */
    {697, 1209},   /*   697/1209 Hz,   1,       1 */
    {697, 1336},   /*   697/1336 Hz,   2,       2 */
    {697, 1477},   /*   697/1477 Hz,   3,       3 */
    {770, 1209},   /*   770/1209 Hz,   4,       4 */
    {770, 1336},   /*   770/1336 Hz,   5,       5 */
    {770, 1477},   /*   770/1477 Hz,   6,       6 */
    {852, 1209},   /*   852/1209 Hz,   7,       7 */
    {852, 1336},   /*   852/1336 Hz,   8,       8 */
    {852, 1477},   /*   852/1477 Hz,   9,       9 */
    {941, 1209},   /*   941/1209 Hz,   10,      * */
    {941, 1336},   /*   941/1336 Hz,   11,      0 */
    {941, 1477},   /*   941/1477 Hz,   12,      # */
    {800,    0},   /*   800/0    Hz,   13,      - */
    {1000,   0},   /*   1000/0   Hz,   14,      - */
    {1250,   0},   /*   1250/0   Hz,   15,      - */
    {950,    0},   /*   950/0    Hz,   16,      - */
    {1100,   0},   /*   1100/0   Hz,   17,      - */ /* --> CNG Tone */
    {1400,   0},   /*   1400/0   Hz,   18,      - */
    {1500,   0},   /*   1500/0   Hz,   19,      - */
    {1600,   0},   /*   1600/0   Hz,   20,      - */
    {1800,   0},   /*   1800/0   Hz,   21,      - */
    {2100,   0},   /*   2100/0   Hz,   22,      - */ /* --> CED Tone */
    {2300,   0},   /*   2300/0   Hz,   23,      - */
    {2450,   0},   /*   2450/0   Hz,   24,      - */
    {350,  440},   /*   350/440  Hz,   25,      - */  /* --> Dial Tone */
    {440,  480},   /*   440/480  Hz,   26,      - */  /* --> Ring Back */
    {480,  620},   /*   480/620  Hz,   27,      - */  /* --> Busy Tone */
    {697, 1633},   /*   697/1633 Hz,   28,      A */
    {770, 1633},   /*   770/1633 Hz,   29,      B */
    {852, 1633},   /*   852/1633 Hz,   30,      C */
    {941, 1633},   /*   941/1633 Hz,   31,      D */
};

/* One static tone table array variable that is shared by all devices. */
/* Zero values indicate that a tone table entry is unconfigured. */
static COMPLEX_TONE toneTable[TAPI_MAX_TONE_CODE];


/* ============================= */
/* Local function declaration    */
/* ============================= */

static IFX_void_t ComplexTone_Conf (TAPI_TONE_DATA_t *pData,
                                     COMPLEX_TONE *toneCoeffs,
                                     COMPLEX_TONE const *pTone);
static IFX_void_t Event_SimpleTone   (TAPI_CHANNEL *pChannel,
                                      IFX_uint8_t nResID);
static IFX_void_t Event_ComposedTone (TAPI_CHANNEL *pChannel,
                                      IFX_uint8_t nResID);
#if (TAPI_CFG_FEATURES & TAPI_FEAT_DECT)
static IFX_int32_t TAPI_DECT_Tone_Stop_Unprot (TAPI_CHANNEL *pChannel);
#endif /* (TAPI_CFG_FEATURES & TAPI_FEAT_DECT) */
static IFX_int32_t TAPI_Phone_Tone_Stop_Unprot (TAPI_CHANNEL *pChannel,
                                                IFX_TAPI_TONE_PLAY_t *pTone,
                                                TAPI_TONE_DST nDirection);
#ifdef TAPI_HAVE_TIMERS
static IFX_void_t Tone_OnTimer (Timer_ID Timer, IFX_ulong_t nArg);
#endif /* TAPI_HAVE_TIMERS */

static IFX_int32_t TAPI_Tone_ResIdGet (IFX_TAPI_DRV_CTX_t *pDrvCtx,
               TAPI_CHANNEL* pChannel,
               IFX_TAPI_TONE_PLAY_t* pTone, IFX_TAPI_TONE_RES_t *pRes);

/**
   If the LL function for resource information is not available
   this function is called to set the resource ID by using the parameters
   src and dst.

   \param pDrvCtx      - pointer to low-level device driver context
   \param pChannel     - handle to TAPI_CHANNEL structure
   \param  pTone        Pointer to tone to be played.
   \param  pRes         The resource information of the tone generator.
                        The structure is filled in this function.
   \return
   - TAPI_statusInvalidToneRes If the resource can not support a requested
     feature. This is the case when a resource supports only local tones and
     the destination TAPI_TONE_DST_NET was requested.
   - TAPI_statusLLNotSupp LL driver does not support any tone playing
   - TAPI_statusOk if the resource was successfully specified.

   \remarks
   This function is called when protection is needed for
   TAPI_Phone_Tone_Play_Unprot
*/
static IFX_int32_t TAPI_Tone_ResIdGet (IFX_TAPI_DRV_CTX_t *pDrvCtx,
               TAPI_CHANNEL* pChannel,
               IFX_TAPI_TONE_PLAY_t* pTone, IFX_TAPI_TONE_RES_t *pRes)
{
   /* select which TAPI data storage to use */
   /* current assignment is still a mix of destination and resource
      default is 0 */
   if (pTone->module == IFX_TAPI_MODULE_TYPE_ALM)
   {
      if (pDrvCtx->ALM.TG_Play != IFX_NULL)
      {
         pRes->nResID = 1;
         pRes->sequenceCap = IFX_TAPI_TONE_RESSEQ_FREQ;
      }
      else
      {
         RETURN_STATUS (TAPI_statusLLNotSupp, 0);
      }
   }
   else if (pTone->module == IFX_TAPI_MODULE_TYPE_DECT)
   {
      pRes->nResID = 2;
      pRes->sequenceCap = IFX_TAPI_TONE_RESSEQ_SIMPLE;
   }
   else
   {
      pRes->nResID = 0;
      pRes->sequenceCap = IFX_TAPI_TONE_RESSEQ_SIMPLE;
      if (pDrvCtx->SIG.UTG_Start != IFX_NULL)
      {
         /* default res = 0 is used or different one if 2 UTGs availalble */
         if ((pDrvCtx->SIG.UTG_Count_Get != IFX_NULL) &&
              pDrvCtx->SIG.UTG_Count_Get (pChannel->pLLChannel) == 2)
         {
            /* When there are 2 UTG per channel we bind them fix to directions */
            if (pTone->external == IFX_FALSE)
               /* use UTG 2 to play tones in local direction  */
               pRes->nResID = 1;
         }
      }
      else
      {
         RETURN_STATUS (TAPI_statusLLNotSupp, 0);
      }
   }
   return TAPI_statusOk;
}

/* ============================= */
/* Global function definition    */
/* ============================= */

/**
   Initialise tone service on the given channel.

   Initialise the data structures and resources needed for the tone service.

   \param   pChannel    Pointer to TAPI_CHANNEL structure.

   \return
     TAPI_statusOk if successful, otherwise error code

   \remarks This function is not protected. The global protection is done
   in the calling function with TAPI_OS_MutexGet (&pChannel->semTapiChDataLock)
*/
IFX_int32_t IFX_TAPI_Tone_Initialise_Unprot(TAPI_CHANNEL *pChannel)
{
   IFX_uint32_t i;

   /* Initialise tone only on channels which include tone capabilities. */
   if (pChannel->nChannel >= pChannel->pTapiDevice->nResource.ToneCount)
   {
      /* not a tone capable channel -> nothing to do  --
         this is not an error */
      return IFX_SUCCESS;
   }

   /* allocate memory for tone resources */
   pChannel->pToneRes = TAPI_OS_Malloc (sizeof(TAPI_TONERES) * TAPI_TONE_MAXRES);
   if (pChannel->pToneRes == IFX_NULL)
   {
      RETURN_STATUS (TAPI_statusNoMem, 0);
   }
   memset (pChannel->pToneRes, 0, sizeof(TAPI_TONERES) * TAPI_TONE_MAXRES);

   /* allocate timer and set status for tone resources */
   for (i = 0; i < TAPI_TONE_MAXRES; ++i)
   {
      /* start complex tone generation in idle state */
      pChannel->TapiComplexToneData[i].nToneState = TAPI_CT_IDLE;
#ifdef TAPI_HAVE_TIMERS
      /* create a timer for each of the tone generator resources */
      if (pChannel->pToneRes[i].Tone_Timer == 0)
      {
         /* initialize (create) voice path teardown timer */
         pChannel->pToneRes[i].Tone_Timer =
           TAPI_Create_Timer((TIMER_ENTRY)Tone_OnTimer,
                             (IFX_uintptr_t)&pChannel->pToneRes[i]);
         pChannel->pToneRes[i].pTapiCh = pChannel;
         pChannel->pToneRes[i].nRes = i;
         if (pChannel->pToneRes[i].Tone_Timer == 0)
         {
            RETURN_STATUS (TAPI_statusTimerFail, 0);
         }
      }
#endif /* TAPI_HAVE_TIMERS */
   }

  return TAPI_statusOk;
}


/**
   Cleanup Tone service on the given channel.

   Free the resources needed for the tone service.

   \param   pChannel    Pointer to TAPI_CHANNEL structure.
*/
IFX_void_t IFX_TAPI_Tone_Cleanup(TAPI_CHANNEL *pChannel)
{
   IFX_int32_t i;

   TAPI_OS_MutexGet (&pChannel->semTapiChDataLock);

   if (pChannel->pToneRes != IFX_NULL)
   {
      /* delete the tone timer resources */
      for (i = 0; i < TAPI_TONE_MAXRES; ++i)
      {
#ifdef TAPI_HAVE_TIMERS
         if (pChannel->pToneRes[i].Tone_Timer != 0)
         {
            TAPI_Delete_Timer (pChannel->pToneRes[i].Tone_Timer);
         }
#endif /* TAPI_HAVE_TIMERS */
      }

      /* free tone resource */
      TAPI_OS_Free (pChannel->pToneRes);
      pChannel->pToneRes = IFX_NULL;
   }

   TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);
}


/**
   Configures predefined tones from Index 1 to 31 and additionally reserved
   tones for CID.

   \return
   - TAPI_statusOk tones configured successfully.
   - TAPI_statusParam Parameter outside of allowed range.

   \remarks
   Constant Table TAPI_PredefinedToneFreq defines the frequencies used for
   the predefined tones in index range 1 to 31. In case this table is changed
   in the future, this function must be adapted accordingly.
   The current table status is :
      - Either Freq2 > Freq 1 => DTMF,
      - or Freq2 = 0 (e.g CNG tone)
*/
IFX_int32_t TAPI_Phone_Tone_Predef_Config (IFX_void_t)
{
   IFX_TAPI_TONE_SIMPLE_t predef_tone;
   IFX_uint32_t i;
   IFX_int32_t ret = TAPI_statusOk;

   /* setup tone structure for predefined tone */
   memset (&predef_tone, 0, sizeof (predef_tone));
   /* add all predefined tones to tone table */
   for (i = 0; i < (IFX_TAPI_TONE_INDEX_MIN - 1); i++)
   {
      /* read frequencies from contant table */
      predef_tone.freqA       = TAPI_PredefinedToneFreq[i][0];
      predef_tone.freqB       = TAPI_PredefinedToneFreq[i][1];
      /* tone indexes start from 1 on */
      predef_tone.index       = i + 1 ;
      /* set frequencies to play and levels */
      switch (predef_tone.freqB)
      {
         case  0:
            /* play only freqA */
            predef_tone.frequencies [0] = IFX_TAPI_TONE_FREQA;
            /* -9 dB = -90 x 0.1 dB for the higher frequency */
            predef_tone.levelA          = -90;
            predef_tone.levelB          = 0;
            break;
         default:
            /* In this case, freqB > freqA according to table
               TAPI_PredefinedToneFreq : Play both frequencies as dtmf  */
            predef_tone.frequencies [0] = (IFX_TAPI_TONE_FREQA |
                                           IFX_TAPI_TONE_FREQB);
            /* -11 dB = -110 x 0.1 dB for the lower frequency ,
               -9  dB = -90  * 0.1 dB for the higher frequency
                        (to attenuate the higher tx loss) */
            predef_tone.levelA          = -110;
            predef_tone.levelB          = -90;
            break;
      }
      /* set cadences and loop */
      switch (predef_tone.index)
      {
         case 17:  /* CNG */
         case 22:  /* CED */
            predef_tone.cadence [0] = 1000;
            predef_tone.cadence [1] = 0;
            /* tone played continuously */
            predef_tone.loop = 0;
            break;
         case 25:  /* (dialtone) */
            predef_tone.cadence [0] = 1000;
            predef_tone.cadence [1] = 0;
            /* tone played continuously */
            predef_tone.loop = 0;
            break;
         case 26:  /* (ringback tone) */
            predef_tone.cadence [0] = 1000;
            predef_tone.cadence [1] = 4000;
            /* tone played continuously */
            predef_tone.loop = 0;
            break;
         case 27:  /* (busy tone) */
            predef_tone.cadence [0] = 500;
            predef_tone.cadence [1] = 500;
            /* tone played continuously */
            predef_tone.loop = 0;
            break;
         default:
            /* all other tones will be played only once for 100 ms */
            predef_tone.cadence [0] = 100;
            predef_tone.cadence [1] = 0;
            /* tone played only once */
            predef_tone.loop = 1;
            break;
      }
      ret = TAPI_Phone_Add_SimpleTone (&predef_tone);
   }

#if (TAPI_CFG_FEATURES & TAPI_FEAT_CID)
   if (TAPI_SUCCESS (ret))
   {
      /* setup predefined CID alert tones */
      ret = IFX_TAPI_CID_SetPredefAlertTones ();
   }
#endif /* (TAPI_CFG_FEATURES & TAPI_FEAT_CID) */

   return ret;
}

/**
   Sets the tone level of tone currently played

   \param  pChannel     Pointer to TAPI_CHANNEL structure.
   \param  pToneLevel   Struct with the tone level specification.

   \return
   IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t TAPI_Phone_Tone_Set_Level(TAPI_CHANNEL *pChannel,
                                      IFX_TAPI_PREDEF_TONE_LEVEL_t const *pToneLevel)
{
   IFX_TAPI_DRV_CTX_t *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   IFX_int32_t ret;

   if (pDrvCtx->SIG.UTG_Level_Set != NULL)
   {
      ret = pDrvCtx->SIG.UTG_Level_Set (pChannel->pLLChannel, pToneLevel);
   }
   else
   {
      RETURN_STATUS (TAPI_statusLLNotSupp, 0);
   }

   return ret;
}

/**
   Gets the tone playing state.

   \param pChannel        - handle to TAPI_CHANNEL structure
   \param pToneState      - 0: no tone is played
                     1: tone is played (on-time)
                     2: silence (off-time)
   \return
   TAPI_statusOk - success
*/
IFX_int32_t TAPI_Phone_Tone_Get_State(TAPI_CHANNEL *pChannel,
                                      IFX_uint32_t *pToneState)
{
   *pToneState = pChannel->TapiTgToneData.nToneState;
   return TAPI_statusOk;
}

/**
   Add simple tone to internal tone coefficients table

   \param  pSimpleTone  Simple tone to be set.

   \return
   - TAPI_statusOk - success
   - TAPI_statusToneIdxOutOfRange - Tone index out of range
   - TAPI_statusToneOnTimeOutOfRange - Simple tone on-time time exceeds the
         maximum range
   - TAPI_statusToneInitialTimeZero - Simple tone time of first step may not
         be zero
   - TAPI_statusTonePauseTimeOutOfRange - Simple tone pause time exceeds the
         maximum supported pause time
   - TAPI_statusToneFrequencyOutOfRange - Simple tone frequency exceeds the
         maximum supported frequency
   - TAPI_statusTonePowerLevelOutOfRange - Simple tone power level exceeds the
         supported range
*/
IFX_int32_t TAPI_Phone_Add_SimpleTone (IFX_TAPI_TONE_SIMPLE_t const *pSimpleTone)
{
   IFX_TAPI_TONE_SIMPLE_t *pToneCfg = IFX_NULL;
   IFX_uint32_t i;

   if (pSimpleTone->index >= TAPI_MAX_TONE_CODE || pSimpleTone->index == 0)
   {
      /* errmsg: Tone index out of range */
      return TAPI_statusToneIdxOutOfRange;
   }
   /* check on time to set */
   for (i=0; i < IFX_TAPI_TONE_STEPS_MAX; ++i)
   {
      if (pSimpleTone->cadence[i] > MAX_TONE_TIME)
      {
         /* errmsg: Simple tone on-time time exceeds the maximum range */
         return TAPI_statusToneOnTimeOutOfRange;
      }
   }
   /* check for zero time on frequency A */
   if (pSimpleTone->cadence[0] == 0)
   {
      /* errmsg: Simple tone time of first step may not be zero */
      return TAPI_statusToneInitialTimeZero;
   }
   /* check pause time to set */
   if (pSimpleTone->pause > MAX_CADENCE_TIME)
   {
      /* errmsg: Simple tone pause time exceeds the maximum supported pause time */
      return TAPI_statusTonePauseTimeOutOfRange;
   }

   /* check frequencies to set */
   if (pSimpleTone->freqA > MAX_TONE_FREQ ||
       pSimpleTone->freqB > MAX_TONE_FREQ ||
       pSimpleTone->freqC > MAX_TONE_FREQ ||
       pSimpleTone->freqD > MAX_TONE_FREQ)
   {
      /* errmsg: Simple tone frequency exceeds the maximum supported frequency */
      return TAPI_statusToneFrequencyOutOfRange;
   }
   /* check power level to set */
   if (pSimpleTone->levelA < MIN_TONE_POWER ||
       pSimpleTone->levelA > MAX_TONE_POWER ||
       pSimpleTone->levelB < MIN_TONE_POWER ||
       pSimpleTone->levelB > MAX_TONE_POWER ||
       pSimpleTone->levelD < MIN_TONE_POWER ||
       pSimpleTone->levelD > MAX_TONE_POWER ||
       pSimpleTone->levelC < MIN_TONE_POWER ||
       pSimpleTone->levelC > MAX_TONE_POWER)
   {
      /* errmsg: Simple tone power level exceeds the supported range */
      return TAPI_statusTonePowerLevelOutOfRange;
   }
   /* Set ptr to tone to be configured */
   pToneCfg = &toneTable[pSimpleTone->index].tone.simple;
   /* Add entry to internal tone table */
   toneTable[pSimpleTone->index].type = TAPI_TONE_TYPE_SIMPLE;
   /* Is the tone out of predefined tone range ? */
   if (pSimpleTone->index >= IFX_TAPI_TONE_INDEX_MIN)
   {
      *pToneCfg = *pSimpleTone;
   }
   else
   {
      /* add tone to tone table in case the tone is configured the first time */
      if (pToneCfg->frequencies [0] == IFX_TAPI_TONE_FREQNONE)
      {
         *pToneCfg = *pSimpleTone;
      }
      else
      {
         /* for predefined tones, adapt only cadence and level */
         pToneCfg->levelA      = pSimpleTone->levelA;
         pToneCfg->levelB      = pSimpleTone->levelB;
         /* tone on time */
         pToneCfg->cadence [0] = pSimpleTone->cadence [0];
         /* tone off time */
         pToneCfg->cadence [1] = pSimpleTone->cadence [1];
         /* set loop. 0 means continuous tone */
         pToneCfg->loop        = pSimpleTone->loop;
      }
   }

   return TAPI_statusOk;
}

/**
   Add composed tone to internal tone coefficients table

   \param pComposedTone  Entry for internal tone table.

   \return
   - TAPI_statusOk - tone successfuly added  or IFX_ERROR
   - TAPI_statusToneIdxOutOfRange - Tone index out of range
   - TAPI_statusToneAltVoiceTimeOutOfRange - Composed tone max alternate voice
         time exceeds the maximum
   - TAPI_statusToneLoopCountInvalid - Composed tone single repetition not
         allowed when alternate voice time is non-zero
   - TAPI_statusToneCountOutOfRange - Composed tone number of simple tone
         fields is out of range
   - TAPI_statusToneSimpleIdxOutOfRange - Composed tone contains a simple tone
         index that is out of range
   - TAPI_statusToneSimpleToneUnconfigured - Composed tone contains a simple
         tone that is not configured
*/
IFX_int32_t TAPI_Phone_Add_ComposedTone (IFX_TAPI_TONE_COMPOSED_t const *pComposedTone)
{
   IFX_uint32_t i;

   if (pComposedTone->index >= TAPI_MAX_TONE_CODE || pComposedTone->index == 0)
   {
      /* errmsg: Tone index out of range */
      return TAPI_statusToneIdxOutOfRange;
   }
   if (pComposedTone->index < IFX_TAPI_TONE_INDEX_MIN)
   {
      return TAPI_statusTonePredefFail;
   }

   /* check alternate voice time to set */
   if (pComposedTone->alternatVoicePath > MAX_VOICE_TIME)
   {
      /* errmsg: Composed tone max alternate voice time exceeds the maximum */
      return TAPI_statusToneAltVoiceTimeOutOfRange;
   }

   /* check loop count against alternate voice time */
   if (pComposedTone->loop == 1 && pComposedTone->alternatVoicePath != 0)
   {
      /* errmsg: Composed tone single repetition not allowed when
                 alternate voice time is non-zero */
      return TAPI_statusToneLoopCountInvalid;
   }

   /* Validate the number of simple tones allowed within composed tone */
   if (pComposedTone->count == 0 ||
       pComposedTone->count > IFX_TAPI_TONE_SIMPLE_MAX)
   {
      /* errmsg: Composed tone number of simple tone fields is out of range */
      return TAPI_statusToneCountOutOfRange;
   }

   /* Validate if all simple tone codes are configured within the composed tone */
   for (i = 0; i < pComposedTone->count; i++)
   {
      if (pComposedTone->tones[i] >= TAPI_MAX_TONE_CODE ||
         pComposedTone->tones[i] == 0)
      {
         /* errmsg: Composed tone contains a simple tone index that is out of range */
         return TAPI_statusToneSimpleIdxOutOfRange;
      }
      if (toneTable[pComposedTone->tones[i]].type == TAPI_TONE_TYPE_NONE)
      {
         /* errmsg: Composed tone contains a simple tone that is not configured */
         return TAPI_statusToneSimpleToneUnconfigured;
      }
   }

   /* Add entry to internal tone table */
   toneTable[pComposedTone->index].type = TAPI_TONE_TYPE_COMP;
   memcpy ((void *)&toneTable[pComposedTone->index].tone.composed,
           (const void *)pComposedTone, sizeof(IFX_TAPI_TONE_COMPOSED_t));

   return TAPI_statusOk;
}

/**
   Add tone to internal tone coefficients table

   \param  pTone        Tone to be inserted into the internal tone table.

   \return
   - TAPI_statusOk - tone successfully added to the table
   - TAPI_statusInvalidToneRes - invalid tone resource specified or
                                 return specific error code
*/
IFX_int32_t TAPI_Phone_Tone_TableConf (IFX_TAPI_TONE_t const *pTone)
{
   IFX_int32_t ret = TAPI_statusOk;

   switch (pTone->simple.format)
   {
      case  IFX_TAPI_TONE_TYPE_SIMPLE:
         ret = TAPI_Phone_Add_SimpleTone (&pTone->simple);
      break;
      case  IFX_TAPI_TONE_TYPE_COMPOSED:
         ret = TAPI_Phone_Add_ComposedTone (&pTone->composed);
      break;
      default:
         ret = TAPI_statusInvalidToneRes;
   }

   return ret;
}

/**
   Configure complex tone play

   \param  pData           Pointer to TAPI tone resource data.
   \param  pToneCoeffTable Pointer to TAPI complex tone table.
   \param  pToneCoeff      Pointer to tone to be played.
*/
static IFX_void_t ComplexTone_Conf (TAPI_TONE_DATA_t * pData,
                                    COMPLEX_TONE *pToneCoeffTable,
                                    COMPLEX_TONE const *pToneCoeff)
{
   IFX_uint32_t firstToneCode = 0;

   /* Setup the tone code type */
   pData->nType = pToneCoeff->type;

   /* This is a composed tone so get the first simple tone sequence to play */
   if (pToneCoeff->type == TAPI_TONE_TYPE_COMP)
   {
      /* Retrieve composed configuration from internal tone coefficients table */
      pData->nComposedMaxReps    = pToneCoeff->tone.composed.loop;
      pData->nComposedCurrReps   = 0;
      pData->nAlternateVoiceTime = pToneCoeff->tone.composed.alternatVoicePath;
      pData->nComposedToneCode   = pToneCoeff->tone.composed.index;
      pData->nMaxToneCount       = pToneCoeff->tone.composed.count;
      pData->nToneCounter        = 0;

      /* First simple tone sequence to play */
      firstToneCode = pToneCoeff->tone.composed.tones[0];

      /* Use the simple tone code as an index into internal tone coefficients table */
      pToneCoeff = &pToneCoeffTable[firstToneCode];
   }

   /* Retrieve simple tone configuration from internal tone coefficients table */
   pData->nSimpleMaxReps      = pToneCoeff->tone.simple.loop;
   pData->nSimpleCurrReps     = 0;
   pData->nPauseTime          = pToneCoeff->tone.simple.pause;
   pData->nSimpleToneCode     = pToneCoeff->tone.simple.index;
}

/**
   Detects a tone based on the complex (simple or composed) tone

   \param pChannel    - handle to TAPI_CHANNEL structure
   \param pSignal     - tone code reference for internal tone table

   \return:
   TAPI_statusOk - tone successfully detected
   TAPI_statusToneIdxOutOfRange - Tone index out of range
   TAPI_statusParam - parameter unsupported or out of range
   \ remarks
   The function returns an error, when the tone is not previously defined
*/
IFX_int32_t TAPI_Phone_DetectToneStart (TAPI_CHANNEL *pChannel,
                                        IFX_TAPI_TONE_CPTD_t const *pSignal)
{
   IFX_TAPI_DRV_CTX_t *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   COMPLEX_TONE *pToneCoeff;
   IFX_int32_t retLL;

   /* Use the tone code as an index into internal tone coefficients table */
   pToneCoeff = &(toneTable[pSignal->tone]);

   /* check if tone code is configured */
   if (pToneCoeff->type == TAPI_TONE_TYPE_NONE)
   {
      /* Reference to unconfigured tone code entry */
      RETURN_STATUS (TAPI_statusParam, 0);
   }
   if (pSignal->tone >= TAPI_MAX_TONE_CODE || pSignal->tone <= 0)
   {
      /* errmsg: Tone index out of range */
      RETURN_STATUS (TAPI_statusToneIdxOutOfRange, 0);
   }
   if (pToneCoeff->type == TAPI_TONE_TYPE_COMP)
   {
      /* Reference to unsupported tone code entry */
      RETURN_STATUS (TAPI_statusParam, 0);
   }

   if (IFX_TAPI_PtrChk (pDrvCtx->SIG.CPTD_Start))
   {
      retLL = pDrvCtx->SIG.CPTD_Start (pChannel->pLLChannel,
                                       &pToneCoeff->tone.simple,
                                       pSignal);
      if (!(TAPI_SUCCESS (retLL)))
      {
         RETURN_STATUS (TAPI_statusCPTDEnFail, retLL);
      }
   }
   /* update of old-style bitfield interface removed */

   return TAPI_statusOk;
}

/**
   Stops the detection a tone based on the complex (simple or composed) tone

   \param pChannel    - handle to TAPI_CHANNEL structure
   \param pSignal     - tone code reference for internal tone table

   \return
   TAPI_statusOk - tone successfully detected
   TAPI_statusToneIdxOutOfRange - Tone index out of range
*/
IFX_int32_t TAPI_Phone_DetectToneStop (TAPI_CHANNEL *pChannel,
                                       IFX_TAPI_TONE_CPTD_t const *pSignal)
{
   IFX_TAPI_DRV_CTX_t *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   IFX_int32_t retLL;

   if (pSignal->tone >= TAPI_MAX_TONE_CODE || pSignal->tone <= 0)
   {
      /* errmsg: Tone index out of range */
      RETURN_STATUS (TAPI_statusToneIdxOutOfRange, 0);
   }

   if (IFX_TAPI_PtrChk (pDrvCtx->SIG.CPTD_Stop))
   {
      retLL = pDrvCtx->SIG.CPTD_Stop (pChannel->pLLChannel, pSignal);
      if (!(TAPI_SUCCESS (retLL)))
      {
         RETURN_STATUS (TAPI_statusCPTDDisFail, retLL);
      }
   }
   else
   {
      RETURN_STATUS (TAPI_statusToneStop, 0);
   }
   return TAPI_statusOk;
}

/**
   Plays a dial tone.

   \param pChannel     - handle to TAPI_CHANNEL structure

   \return
   IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t TAPI_Phone_Tone_Dial(TAPI_CHANNEL *pChannel)
{
   /* play dial tone */
   return TAPI_Phone_Tone_Local_Play (pChannel, 25);
}

/**
   Plays a ringback tone.

   \param pChannel     - handle to TAPI_CHANNEL structure

   \return
   IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t TAPI_Phone_Tone_Ringback(TAPI_CHANNEL *pChannel)
{
   return TAPI_Phone_Tone_Local_Play (pChannel, 26);
}

/**
   Plays a busy tone.

   \param pChannel     - handle to TAPI_CHANNEL structure

   \return:
   TAPI_statusOk - plays busy tone correctly
   or error code returned from the called function
*/
IFX_int32_t TAPI_Phone_Tone_Busy (TAPI_CHANNEL *pChannel)
{
   return TAPI_Phone_Tone_Local_Play (pChannel, 27);
}

/**
   Plays/Stop a tone from the tone table

   \param pChannel   Handle to TAPI channel
   \param pTone      Handle to the tone descriptor

   \return TAPI_statusOk on success

   \remarks
      Tone index equal to zero will stop the tone.
*/
IFX_int32_t TAPI_Target_Tone_Play (TAPI_CHANNEL *pChannel,
                                   IFX_TAPI_TONE_PLAY_t *pTone)
{
   IFX_int32_t    ret;

   TAPI_OS_MutexGet (&pChannel->semTapiChDataLock);
   /* Tone index == 0 is stop */
   if (pTone->index == 0)
   {
      /* if index is 0 stop all tone generators in this direction */
      ret = TAPI_Phone_Tone_Stop_Unprot (pChannel, pTone, TAPI_TONE_DST_DEFAULT);
   }
   else
   {
      ret = TAPI_Phone_Tone_Play_Unprot (pChannel, pTone,
         IFX_TRUE /* send TONE_END event */);
   }
   TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);

   return ret;
}

/**
   Plays a tone from the tone table to the local direction

   \param pChannel     Handle to TAPI channel
   \param nToneIndex   index in the tone table

   \return TAPI_statusOk on success
*/
IFX_int32_t TAPI_Phone_Tone_Local_Play (TAPI_CHANNEL *pChannel,
   IFX_int32_t nToneIndex)
{
   return TAPI_Phone_Tone_Play (pChannel, nToneIndex, TAPI_TONE_DST_LOCAL);
}

/**
   Stops tone generation on local direction

   \param pChannel     Handle to TAPI channel
   \param nToneIndex   index in the tone table

   \return TAPI_statusOk on success
*/
IFX_int32_t TAPI_Phone_Tone_Local_Stop (TAPI_CHANNEL *pChannel,
   IFX_int32_t nToneIndex)
{
   IFX_TAPI_TONE_PLAY_t  tmp;

   memset(&tmp, 0, sizeof(tmp));
   tmp.index = (IFX_uint32_t)nToneIndex;
   tmp.module = IFX_TAPI_MODULE_TYPE_COD;
   return TAPI_Phone_Tone_Stop(pChannel, &tmp, TAPI_TONE_DST_LOCAL);
}

/**
   Plays a tone from the tone table to the network direction

   \param pChannel     Handle to TAPI channel
   \param nToneIndex   index in the tone table

   \return TAPI_statusOk on success
*/
IFX_int32_t TAPI_Phone_Tone_Net_Play (TAPI_CHANNEL *pChannel,
   IFX_int32_t nToneIndex)
{
   return TAPI_Phone_Tone_Play (pChannel, nToneIndex, TAPI_TONE_DST_NET);
}

/**
   Stops tone generation on network direction

   \param pChannel     Handle to TAPI channel
   \param nToneIndex   index in the tone table

   \return TAPI_statusOk on success
*/
IFX_int32_t TAPI_Phone_Tone_Net_Stop (TAPI_CHANNEL *pChannel,
   IFX_int32_t nToneIndex)
{
   IFX_TAPI_TONE_PLAY_t  tmp;

   memset(&tmp, 0, sizeof(tmp));
   tmp.index = (IFX_uint32_t)nToneIndex;
   tmp.module = IFX_TAPI_MODULE_TYPE_COD;
   return TAPI_Phone_Tone_Stop(pChannel, &tmp, TAPI_TONE_DST_NET);
}

/**
   Stops tone generation on default direction

   \param pChannel     Handle to TAPI channel
   \param pTone      Handle to the tone descriptor

   \return TAPI_statusOk on success
*/
IFX_int32_t TAPI_Phone_Tone_Def_Stop (TAPI_CHANNEL *pChannel,
   IFX_TAPI_TONE_PLAY_t *pTone)
{
   return TAPI_Phone_Tone_Stop (pChannel, pTone, TAPI_TONE_DST_DEFAULT);
}

/**
   Plays a tone from the tone table (protected from multiple access).

   This function is called when protection is needed for
   TAPI_Phone_Tone_Play_Unprot()
   It does the protection before the function for playing a tone is called.

   \param pChannel     - handle to TAPI_CHANNEL structure
   \param nToneIndex   - index in the tone table
   \param dst          - direction where to play the tone: external or internal

   \return
   IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t TAPI_Phone_Tone_Play(TAPI_CHANNEL *pChannel,
                                 IFX_int32_t nToneIndex,
                                 TAPI_TONE_DST dst)
{
   IFX_int32_t    ret, src;
   IFX_TAPI_TONE_PLAY_t tone;
   IFX_TAPI_DRV_CTX_t *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;

   memset (&tone, 0, sizeof(tone));

   /* Filter out the source for playing out the tone from parameter nToneIndex.
      Note: The parameter nToneIndex is not well documented in TAPI document for
      this purpose. Therefore it is also checked if the LL exclusively provides
      a function pointer for tone generator usage. In this case the appropriate
      ressource is selected.  */

   src = nToneIndex & (IFX_TAPI_TONE_SRC_TG |
                       IFX_TAPI_TONE_SRC_DSP | IFX_TAPI_TONE_SRC_DECT);
   nToneIndex &=     ~(IFX_TAPI_TONE_SRC_TG |
                       IFX_TAPI_TONE_SRC_DSP | IFX_TAPI_TONE_SRC_DECT);

   tone.index = (IFX_uint32_t)nToneIndex;

   if (pDrvCtx->ALM.TG_Play == IFX_NULL)
   {
      /* The LL does not provide a tone generator. Clear the TG bit.
         Below we will see if another tone generator can be used instead. */
      src &= ~IFX_TAPI_TONE_SRC_TG;
   }
   if (pDrvCtx->SIG.UTG_Start == IFX_NULL && pDrvCtx->ALM.TG_Play != IFX_NULL)
   {
      /* The LL does not provide UTG but TG. Set the TG bit and try to use
         this below. */
      src |= IFX_TAPI_TONE_SRC_TG;
   }

   tone.external = IFX_FALSE;
   tone.internal = IFX_FALSE;

   switch (src)
   {
      case IFX_TAPI_TONE_SRC_DECT:
         tone.module = IFX_TAPI_MODULE_TYPE_DECT;
         tone.external = IFX_TRUE;
         break;

      case IFX_TAPI_TONE_SRC_TG:
         tone.module = IFX_TAPI_MODULE_TYPE_ALM;
         /* Note: actually the cases for NET or NETLOCAL are not supported by
            any of the current TGs but they are implemented for completeness. */
         switch (dst)
         {
         case TAPI_TONE_DST_NET:
            tone.internal = IFX_TRUE;
            break;
         case TAPI_TONE_DST_DEFAULT:
         case TAPI_TONE_DST_LOCAL:
            tone.external = IFX_TRUE;
            break;
         case TAPI_TONE_DST_NETLOCAL:
            tone.external = IFX_TRUE;
            tone.internal = IFX_TRUE;
            break;
         } /* switch (dst) */
         break;

      case IFX_TAPI_TONE_SRC_DSP:
      default:
         tone.module = IFX_TAPI_MODULE_TYPE_COD;
         switch (dst)
         {
            case TAPI_TONE_DST_NET:
               tone.external = IFX_TRUE;
               break;
            case TAPI_TONE_DST_DEFAULT:
            case TAPI_TONE_DST_LOCAL:
               tone.internal = IFX_TRUE;
               break;
            case TAPI_TONE_DST_NETLOCAL:
               tone.external = IFX_TRUE;
               tone.internal = IFX_TRUE;
               break;
         } /* switch (dst) */
         break;
   } /* switch (src) */

   /* make sure the resource is not in use before allocating it */
   TAPI_OS_MutexGet (&pChannel->semTapiChDataLock);
   /* Tone index == 0 is stop */
   if (nToneIndex == 0)
   {
      /* if index is 0 stop all tone generators in this direction */
#if (TAPI_CFG_FEATURES & TAPI_FEAT_DECT)
      if (tone.module == IFX_TAPI_MODULE_TYPE_DECT)
      {
         ret = TAPI_DECT_Tone_Stop_Unprot (pChannel);
      }
      else
#endif /* (TAPI_CFG_FEATURES & TAPI_FEAT_DECT) */
      {
         ret = TAPI_Phone_Tone_Stop_Unprot (pChannel, &tone, TAPI_TONE_DST_DEFAULT);
      }
   }
   else
   {
      /* Send TAPI event for all tones played through IOCTL. */
      ret = TAPI_Phone_Tone_Play_Unprot (pChannel, &tone,
         IFX_TRUE /* send TONE_END event */);
   }
   TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);

   return ret;
}

/**
   Plays a tone from the tone table (without protection).

   This function is to be called only when the channel protection semaphore
   is already held by the caller. Only internal functions may call this.

   \param pChannel      - handle to TAPI_CHANNEL structure
   \param pTone         - module, tone index and channel
   \param bSendEndEvent - generate event IFX_TAPI_EVENT_TONE_GEN_END

   \return
   TAPI_statusOk - plays tone correctly
   TAPI_statusToneIdxOutOfRange - Tone index out of range
   TAPI_statusToneNotAvail - tone not configured
   TAPI_statusLLNotSupp - LL code not available
   TAPI_statusToneNoRes - invalid tone specified to play the tone
*/
IFX_int32_t TAPI_Phone_Tone_Play_Unprot (TAPI_CHANNEL *pChannel,
                                IFX_TAPI_TONE_PLAY_t *pTone,
                                IFX_boolean_t bSendEndEvent)
{
   IFX_TAPI_DRV_CTX_t *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   IFX_int32_t ret = TAPI_statusOk;
   IFX_int32_t nToneIndex = pTone->index;
   TAPI_TONE_DATA_t *pData;
   COMPLEX_TONE *pToneCoeff;
   IFX_TAPI_TONE_SIMPLE_t *pToneSimple;
   IFX_uint32_t nSimpleToneIndex;
   IFX_TAPI_TONE_RES_t ToneRes;
   IFX_TAPI_LL_TONE_DIR_t toneDir;
   TAPI_TONE_DST dst;

   memset (&ToneRes, 0, sizeof(IFX_TAPI_TONE_RES_t ));

   /* check tone index parameter */
   if (nToneIndex >= TAPI_MAX_TONE_CODE || nToneIndex < 0)
   {
      /* errmsg: Tone index out of range */
      RETURN_STATUS (TAPI_statusToneIdxOutOfRange, 0);
   }
   /* Use the tone code as an index into internal tone coefficients table */
   pToneCoeff = &(toneTable[nToneIndex]);
   /* Check the kind of tone and get the index of the first simple tone */
   switch (pToneCoeff->type)
   {
   case TAPI_TONE_TYPE_COMP:
      /* For composed tones fetch the first simple tone */
      nSimpleToneIndex = pToneCoeff->tone.composed.tones[0];
      break;
   case TAPI_TONE_TYPE_SIMPLE:
      /* For simple tones the index already points to a simple tone */
      nSimpleToneIndex = (IFX_uint32_t)nToneIndex;
      break;
   case TAPI_TONE_TYPE_NONE:
      RETURN_STATUS (TAPI_statusInvalidToneRes, 0);
   default:
      /* Reference to unconfigured tone code entry */
      RETURN_STATUS (TAPI_statusToneNotAvail, 0);
   }

   /* get the right resource for playing out this tone */
   if (pDrvCtx->SIG.ToneGen_ResIdGet != IFX_NULL)
   {
      /* if function ResIdGet is supported the type of tone defined
         by the ResID and played out with only one function instead
         of diferentiating between ALM, DECT, UTG */
      if (pDrvCtx->SIG.UTG_Start == IFX_NULL)
         RETURN_STATUS (TAPI_statusLLNotSupp, 0);

      if (pTone->external == IFX_TRUE)
         toneDir = IFX_TAPI_LL_TONE_EXTERNAL;
      else
         toneDir = IFX_TAPI_LL_TONE_INTERNAL;

      ret = pDrvCtx->SIG.ToneGen_ResIdGet (pChannel->pLLChannel,
               pTone->module, toneDir, &ToneRes);
      if (!TAPI_SUCCESS(ret))
         RETURN_STATUS (TAPI_statusToneNoRes, ret);
   }
   else
   {
      /**\todo Shift to LL */
      ret = TAPI_Tone_ResIdGet (pDrvCtx, pChannel, pTone, &ToneRes);
      if (!TAPI_SUCCESS (ret))
         return ret;
   }

   if (ToneRes.nResID >= TAPI_TONE_MAXRES)
      RETURN_STATUS (TAPI_statusToneNoRes, 0);

   pData = &pChannel->TapiComplexToneData [ToneRes.nResID];
   if (!((pData->nToneState == TAPI_CT_IDLE) ||
       (pData->nToneState == TAPI_CT_DEACTIVATED)))
   {
      /* tone is already playing */
      RETURN_STATUS (TAPI_statusTonePlayAct, 0);
   }

   /* preparation for complex tones */
   ComplexTone_Conf (pData, toneTable, pToneCoeff);
   /* retrieve simple tone from internal tone coefficients table */
   pToneSimple = &(toneTable[nSimpleToneIndex].tone.simple);

   if (pTone->internal && pTone->external)
      dst = TAPI_TONE_DST_NETLOCAL;
   else
   {
      if (pTone->external)
      {
         if (IFX_TAPI_MODULE_TYPE_ALM == pTone->module)
            dst = TAPI_TONE_DST_LOCAL;
         else
            dst = TAPI_TONE_DST_NET;
      }
      else /* make pTone->internal default */
      {
         if (IFX_TAPI_MODULE_TYPE_ALM == pTone->module)
            dst = TAPI_TONE_DST_NET;
         else
            dst = TAPI_TONE_DST_LOCAL;
      }
   }

   /* Try to play the first simple tone */
   if ((pTone->module == IFX_TAPI_MODULE_TYPE_DECT) &&
       IFX_TAPI_PtrChk (pDrvCtx->DECT.UTG_Start))
   {
      /* Play the first simple tone */
      ret = pDrvCtx->DECT.UTG_Start (pChannel->pLLChannel,
                                     pToneSimple, dst, 0);
   }
   else if (IFX_TAPI_PtrChk (pDrvCtx->SIG.UTG_Start))
   {
      /* Play the first simple tone */
      ret = pDrvCtx->SIG.UTG_Start (pChannel->pLLChannel,
                                    pToneSimple, dst, ToneRes.nResID);
   }
   /* if no UTG is available play tone on TG (if available) */
   else if (IFX_TAPI_PtrChk (pDrvCtx->ALM.TG_Play))
   {
      /* Play the first simple tone */
      ret = pDrvCtx->ALM.TG_Play (pChannel->pLLChannel,
                            ToneRes.nResID, pToneSimple, dst);
#ifdef TAPI_HAVE_TIMERS
      /* start the timer for the first cadence of the simple tone. */
      TAPI_SetTime_Timer(pChannel->pToneRes[ToneRes.nResID].Tone_Timer,
                           1 /* start immediately */, IFX_FALSE, IFX_TRUE);
#endif /* TAPI_HAVE_TIMERS */
   }
   else
   {
      RETURN_STATUS (TAPI_statusLLNotSupp, 0);
   }

   if (!TAPI_SUCCESS(ret))
   {
      /* errmsg: Playing tone in LL driver failed */
      RETURN_STATUS (TAPI_statusTonePlayLLFailed, ret);
   }

   /* store the information */
   pData->dst = dst;
   pData->sequenceCap = ToneRes.sequenceCap;
   pData->nToneIndex = nToneIndex;
   pData->nToneState = TAPI_CT_ACTIVE;
   pData->module     = pTone->module;
   pData->bSendToneGenEndTapiEvent = bSendEndEvent;

   return TAPI_statusOk;
}

/**
   Stops Tone Generation, only in connection with "IFX_TAPI_TONE_STOP"

   \param pChannel     - handle to TAPI_CHANNEL structure
   \param nToneIndex   - tone to be stopped
   \param nDirection   - direction into which tone is played

   \return
   TAPI_statusOk - tone stopped successfully
*/
IFX_int32_t TAPI_Phone_Tone_Stop(TAPI_CHANNEL *pChannel,
                                 IFX_TAPI_TONE_PLAY_t *pTone,
                                 TAPI_TONE_DST nDirection)
{
   IFX_int32_t ret;

   TAPI_OS_MutexGet (&pChannel->semTapiChDataLock);
   if (pTone->external && pTone->internal)
   {
      /* TAPI_Phone_Tone_Stop_Unprot can't stop both directions at the same
       * time. nDirection not used because external/internal has priority over
       * it for TAPIv4. */
      pTone->external = 0;
      ret = TAPI_Phone_Tone_Stop_Unprot(pChannel, pTone, nDirection);
      pTone->external = 1;
      pTone->internal = 0;
      ret = TAPI_Phone_Tone_Stop_Unprot(pChannel, pTone, nDirection);
   }
   else
   {
      ret = TAPI_Phone_Tone_Stop_Unprot(pChannel, pTone, nDirection);
   }
   TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);

   return ret;
}

/**
   Stops Tone Generation, only in connection with "IFX_TAPI_TONE_STOP"

   \param pChannel     - handle to TAPI_CHANNEL structure
   \param nToneIndex   - tone to be stopped
   \param nDirection   - direction into which tone is played

   \return
   TAPI_statusOk - tone stopped successfully
   TAPI_statusParam - parameter not allowed

   \remarks No protection against concurrent access. If you need protection
            TAPI_Phone_Tone_Stop must be called.
*/
static IFX_int32_t TAPI_Phone_Tone_Stop_Unprot (TAPI_CHANNEL *pChannel,
                                                IFX_TAPI_TONE_PLAY_t *pTone,
                                                TAPI_TONE_DST nDirection)
{
   IFX_TAPI_DRV_CTX_t *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   TAPI_TONE_DATA_t *pData;
   IFX_TAPI_TONE_RES_t ToneRes;
   IFX_int32_t ret = TAPI_statusOk;
   IFX_int32_t nToneIndex = pTone->index;
   IFX_boolean_t bResSpec = IFX_FALSE;
   IFX_TAPI_LL_TONE_DIR_t toneDir;

   memset (&ToneRes, 0, sizeof(IFX_TAPI_TONE_RES_t ));
   /* get the right resource for playing out this tone */
   if (pDrvCtx->SIG.ToneGen_ResIdGet != IFX_NULL &&
       pTone->module != IFX_TAPI_MODULE_TYPE_NONE)
   {
      /* get a resource ID if specified by application and supported by
         the low level */
      if (pTone->external == IFX_TRUE)
         toneDir = IFX_TAPI_LL_TONE_EXTERNAL;
      else
         toneDir = IFX_TAPI_LL_TONE_INTERNAL;
      ret = pDrvCtx->SIG.ToneGen_ResIdGet (pChannel->pLLChannel,
               pTone->module, toneDir, &ToneRes);
      if (!TAPI_SUCCESS(ret))
         RETURN_STATUS (TAPI_statusParam, ret);
      /* the resource is specified */
      bResSpec = IFX_TRUE;
   }

   do
   {
      /* Skip DECT resouce as it is handled in a separate function below */
#if 0
      if (nResID == 2)
      {
         nResID++;
         continue;
      }
#endif
      /* get tone definition */
      pData = &pChannel->TapiComplexToneData[ToneRes.nResID];
      /* stop given tone index played into the given direction
         a direction of TAPI_TONE_DST_DEFAULT means don't care for the direction
         if no index is specified every tone currently played is stopped */
      if ((nToneIndex != 0 && pData->nToneIndex == nToneIndex &&
           (nDirection == TAPI_TONE_DST_DEFAULT || pData->dst == nDirection)) ||
          (nToneIndex == 0 && pData->nToneState != TAPI_CT_IDLE &&
           (nDirection == TAPI_TONE_DST_DEFAULT || pData->dst == nDirection)))
      {
         if (pData->nToneState == TAPI_CT_IDLE)
            return TAPI_statusOk;
         /* Set tone generation back to initial state */
         pData->nToneState = TAPI_CT_IDLE;

#ifdef TAPI_HAVE_TIMERS
         /* Stop any voice path teardowm timer that may be running */
         TAPI_Stop_Timer (pChannel->pToneRes[ToneRes.nResID].Tone_Timer);
#endif /* TAPI_HAVE_TIMERS */

         if (IFX_TAPI_PtrChk (pDrvCtx->SIG.UTG_Stop))
         {
            ret = pDrvCtx->SIG.UTG_Stop(pChannel->pLLChannel, ToneRes.nResID);
         }
         else if (IFX_TAPI_PtrChk (pDrvCtx->ALM.TG_Stop))
         {
            ret = pDrvCtx->ALM.TG_Stop(pChannel->pLLChannel, ToneRes.nResID);
         }
         if (!(TAPI_SUCCESS (ret)))
            RETURN_STATUS (TAPI_statusToneStop, ret);

         /* reset index afterwards anyway */
         pData->nToneIndex = 0;
      }
      ToneRes.nResID++;
   }
   while (ToneRes.nResID < TAPI_TONE_MAXRES && bResSpec == IFX_FALSE);

   return (ret);
}

#if (TAPI_CFG_FEATURES & TAPI_FEAT_DECT)
/**
   This service plays a tone on the DECT channel.

   \param pChannel     Handle to TAPI channel
   \param nToneIndex   index in the tone table

   \return TAPI_statusOk on success
*/
IFX_int32_t TAPI_DECT_Tone_Play (TAPI_CHANNEL *pChannel,
   IFX_int32_t nToneIndex)
{
   return TAPI_Phone_Tone_Net_Play (pChannel,
      nToneIndex | IFX_TAPI_TONE_SRC_DECT);
}

/**
   Stops Tone Generation on a DECT channel

   \param pChannel     - handle to TAPI_CHANNEL structure

   \return
   TAPI_statusOk - tone stopped successfully
*/
IFX_int32_t TAPI_DECT_Tone_Stop(TAPI_CHANNEL *pChannel)
{
   IFX_int32_t ret;

   TAPI_OS_MutexGet (&pChannel->semTapiChDataLock);
   ret = TAPI_DECT_Tone_Stop_Unprot(pChannel);
   TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);

   return (ret);
}

/**
   Stops Tone Generation on a DECT channel

   \param pChannel     - handle to TAPI_CHANNEL structure

   \return
   TAPI_statusOk - success
   TAPI_statusToneStop - unable to stop the tone playing

   \remarks No protection against concurrent access. If you need protection
            TAPI_DECT_Tone_Stop must be called.
*/
static IFX_int32_t TAPI_DECT_Tone_Stop_Unprot(TAPI_CHANNEL *pChannel)
{
   IFX_TAPI_DRV_CTX_t *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   TAPI_TONE_DATA_t *pData;
   IFX_int32_t res = 2; /* DECT is always resource 2 by definition */
   IFX_int32_t ret = TAPI_statusToneStop;

#ifdef TAPI_HAVE_TIMERS
   /* Stop any voice path teardowm timer that may be running */
   TAPI_Stop_Timer (pChannel->pToneRes[res].Tone_Timer);
#endif /* TAPI_HAVE_TIMERS */

   if (IFX_TAPI_PtrChk (pDrvCtx->DECT.UTG_Stop))
   {
      ret = pDrvCtx->DECT.UTG_Stop(pChannel->pLLChannel, 0);
   }

   /* Get tone definition */
   pData = &pChannel->TapiComplexToneData[res];
   /* Set tone generation back to initial state */
   pData->nToneState = TAPI_CT_IDLE;
   /* Reset index afterwards anyway */
   pData->nToneIndex = 0;

   return (ret);
}
#endif /* (TAPI_CFG_FEATURES & TAPI_FEAT_DECT) */

#ifdef TAPI_HAVE_TIMERS
/*
 * Tone State machine
 */
/**
   Function called from the teardown voice path timer.
   The timer expiry indicates the following:
     - Stop the voice path recording(teardown) in order for the next
       tone playing sequence step to be executed on that channel
\param
   Timer  - TimerID of timer that exipres
\param
   nArg   - Argument of timer including the VINETIC_CHANNEL structure
           (as integer pointer)
\return
*/
static IFX_void_t Tone_OnTimer(Timer_ID Timer, IFX_ulong_t nArg)
{
   TAPI_TONERES *pRes = (TAPI_TONERES*)nArg;
   TAPI_CHANNEL *pChannel = pRes->pTapiCh;
   TAPI_DEV     *pTapiDev = pChannel->pTapiDevice;
   IFX_TAPI_DRV_CTX_t   *pDrvCtx = pTapiDev->pDevDrvCtx;

   IFX_uint32_t toneCode           = 0;
   IFX_TAPI_TONE_SIMPLE_t *pToneSimple;
   TAPI_TONE_DATA_t *pData;

   IFX_uint8_t  nToneStep = 1;
   IFX_int32_t ret = TAPI_statusErr;

   IFX_UNUSED (Timer);

   TAPI_OS_MutexGet (&pChannel->semTapiChDataLock);
   pData = &pChannel->TapiComplexToneData[pRes->nRes];

   /* Ignore timer when state is idle.
      May be caused when timer is processed with lower priority
      than an application that does a tone stop at the same time. */
   if (pData->nToneState == TAPI_CT_IDLE)
   {
      TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);
      return;
   }

   /* check if mailbox supports all commands, otherwise return immediately */
   if (pData->sequenceCap == IFX_TAPI_TONE_RESSEQ_SIMPLE)
   {
      IFX_uint8_t nCmdMbxSize;
      IFX_TAPI_LL_DEV_t *pDev = pTapiDev->pLLDev;

      if (IFX_TAPI_PtrChk (pDrvCtx->GetCmdMbxSize))
      {
         if(((IFX_int32_t)pDrvCtx->GetCmdMbxSize (pDev, &nCmdMbxSize)) ==
               TAPI_statusOk)
         {
            if (nCmdMbxSize < MAX_CMD_WORD)
            {
               TAPI_SetTime_Timer(Timer,
                  1 /* start immediately */, IFX_FALSE, IFX_FALSE);
               goto exit;
            }
         }
      }
   }

   /* Get the simple tone code */
   if (pData->nType == TAPI_TONE_TYPE_SIMPLE)
   {
      toneCode = pData->nSimpleToneCode;
   }
   else if (pData->nType == TAPI_TONE_TYPE_COMP)
   {
      /* Get the simple tone to play within the composed tone */
      IFX_TAPI_TONE_COMPOSED_t *pComposedTone;

      pComposedTone = &toneTable[pData->nComposedToneCode].tone.composed;
      toneCode = pComposedTone->tones[pData->nToneCounter];
   }

   /* Any errors on the simple tone code index is taken care of here */
   if (toneCode >= TAPI_MAX_TONE_CODE || toneCode == 0)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
            ("DRV_ERROR: Repeat complex tone code out of range\n"));
      goto exit;
   }

   /* Retrieve simple tone configuration from internal tone coefficients table */
   pToneSimple = &(toneTable[toneCode].tone.simple);
   pData->nSimpleMaxReps      = pToneSimple->loop;
   pData->nPauseTime          = pToneSimple->pause;
   pData->nSimpleToneCode     = pToneSimple->index;

   if (pData->sequenceCap == IFX_TAPI_TONE_RESSEQ_FREQ)
   {
      /* when the next simple tone in a composed tone must be played, call
         the low level function to set up the tone */
      if ((pData->nToneState == TAPI_CT_IDLE) &&
          (pData->nType == TAPI_TONE_TYPE_COMP))
      {
         /* tone has finished */
         if (IFX_TAPI_PtrChk (pDrvCtx->ALM.TG_Play))
            ret = pDrvCtx->ALM.TG_Play (pChannel->pLLChannel, pRes->nRes,
                                  pToneSimple, pData->dst);
         if (ret != TAPI_statusOk)
         {
            TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Setup of next simple"
                  " tone failed\n\r"));

            goto exit;
         }

         /* store the information */
         pData->nToneIndex = toneCode;
         pData->nToneState = TAPI_CT_ACTIVE;
      }

      /* at the end of the simple tone pause, set back to state active */
      if (pData->nToneState == TAPI_CT_ACTIVE_PAUSE)
         pData->nToneState = TAPI_CT_ACTIVE;

      /* tone has finished */
      if (IFX_TAPI_PtrChk (pDrvCtx->ALM.TG_ToneStep))
      {
         /* important release lock to avoid deadlocks as the TG_Step
            jumps back into TAPI context via the evtDispatch... */
         TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);
         ret = pDrvCtx->ALM.TG_ToneStep (pChannel->pLLChannel,
            pToneSimple, pRes->nRes, &nToneStep);
         TAPI_OS_MutexGet (&pChannel->semTapiChDataLock);
         /* don't modify pData->nToneState anymore, it has been
            handled in the event dispatcher already, i.e. we'll
            ignore the return value of nToneStep (!) */

         if (ret != IFX_SUCCESS)
         {
            goto exit;
         }
         if (
            /* only restart the timer if we are active
               and continuously played tone -
               otherwise the tone has completed already */
            (pData->nToneState == TAPI_CT_ACTIVE) &&
            (pToneSimple->cadence[1] || pToneSimple->loop)
            )
         {
            if (nToneStep < 1 || nToneStep > IFX_TAPI_TONE_STEPS_MAX)
            {
              TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
                  ("DRV_ERROR: tone step out of range\n"));
              goto exit;
            }
            /* set tone timer */
            TAPI_SetTime_Timer (Timer, pToneSimple->cadence[nToneStep - 1],
                                 IFX_FALSE, IFX_TRUE);
         }
      }
   }
   else /* UTG */
   {
      /* Activate the universal tone generator with simple tone sequence */
      if (pRes->nRes == 2)
      {
         /* Play tone on DECT UTG */
         if (IFX_TAPI_PtrChk (pDrvCtx->DECT.UTG_Start))
            (IFX_void_t)pDrvCtx->DECT.UTG_Start(pChannel->pLLChannel,
                                    pToneSimple, pData->dst, 0);
      }
      else
      {
         /* Play tone on SIG UTGs */
         if (IFX_TAPI_PtrChk (pDrvCtx->SIG.UTG_Start))
            (IFX_void_t)pDrvCtx->SIG.UTG_Start(pChannel->pLLChannel,
                                   pToneSimple, pData->dst, pRes->nRes);
      }
      pChannel->TapiComplexToneData[pRes->nRes].nToneState = TAPI_CT_ACTIVE;
   }

exit:
   TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);
   return;
}
#endif /* TAPI_HAVE_TIMERS */

/**
   UTG deactivated event handling function. Is called upon event.

   \param pChannel     - handle to TAPI_CHANNEL structure
   \param nResID       - index of resource which is reporting this

   \return none
*/
IFX_void_t TAPI_Tone_Step_Completed (TAPI_CHANNEL *pChannel, IFX_uint8_t nResID)
{
   IFX_TAPI_DRV_CTX_t   *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   TAPI_TONE_DATA_t     *pData   = IFX_NULL;
   IFX_TAPI_EVENT_t     tapiEvent;

   if (nResID >= TAPI_TONE_MAXRES)
      return;

   /* handle firmware related stuff here */
   if (IFX_TAPI_PtrChk (pDrvCtx->SIG.UTG_Event_Deactivated))
   {
      pDrvCtx->SIG.UTG_Event_Deactivated(pChannel->pLLChannel, nResID);
   }

   TAPI_OS_MutexGet (&pChannel->semTapiChDataLock);

   /* get pointer to TAPI data storage for this resource  */
   pData = &pChannel->TapiComplexToneData[nResID];

   /* Ignore events when state is not active.
      May be caused by stop after the event was already dispatched. */
   if (pData->nToneState != TAPI_CT_ACTIVE)
   {
      TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);
      return;
   }

   /* Tone generation is in deactivated state */
   pData->nToneState = TAPI_CT_DEACTIVATED;

   /* Set tone generation state */
   Event_SimpleTone(pChannel, nResID);

   if (pData->nType == TAPI_TONE_TYPE_COMP)
   {
      /* This is a composed tone so retrieve the next (if any) simple tone
      sequence to play */
      /* check if no more simple tone is played out. */
      if (pData->nToneState == TAPI_CT_DEACTIVATED)
      {
         /* Init the simple tone repetition counter again */
         pData->nSimpleCurrReps = 0;
         /* Setup the next simple tone to play within the composed tone */
         pData->nToneCounter++;
         /* There are no more simple tones to play within the composed tone,
            apply pause times now */
         if (pData->nToneCounter >= pData->nMaxToneCount)
         {
            /* Init the tone counter again to play repetative composed tones */
            pData->nToneCounter = 0;
            /* Prepare for the for next repetition (if any) of a composed tone */
            Event_ComposedTone(pChannel, nResID);

            if ((pData->nComposedMaxReps != 0) &&
                (pData->nComposedCurrReps >= pData->nComposedMaxReps))
            {
               /* This was the last composed tone sequence to be played */
               /* Set tone generation back to initial state */
               pData->nToneState = TAPI_CT_DEACTIVATED;
            }
         }
         /* Kick off the next simple tone sequence to play */
         else
         {
            if (pData->nPauseTime == 0)
            {
               TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);
               pData->nToneState = TAPI_CT_ACTIVE;

#ifdef TAPI_HAVE_TIMERS
               /* For DxT (TG) the next step already started */
               if (! pChannel->pTapiDevice->pDevDrvCtx->ALM.TG_Play)
                  Tone_OnTimer (pChannel->pToneRes[nResID].Tone_Timer,
                     (IFX_uintptr_t)&pChannel->pToneRes [nResID]);
#endif /* TAPI_HAVE_TIMERS */
               return;
            }
            else
            {
               pData->nToneState = TAPI_CT_ACTIVE_PAUSE;
#ifdef TAPI_HAVE_TIMERS
               /* Start the voice path teardown timer */
               TAPI_SetTime_Timer (pChannel->pToneRes[nResID].Tone_Timer,
                                pData->nPauseTime, IFX_FALSE, IFX_FALSE);
#endif /* TAPI_HAVE_TIMERS */
            }
         }
      }
   }

   if (pData->nToneState == TAPI_CT_DEACTIVATED)
   {
#ifdef TAPI_HAVE_TIMERS
      /* Stop the voice path teardowm timer */
      TAPI_Stop_Timer(pChannel->pToneRes[nResID].Tone_Timer);
#endif /* TAPI_HAVE_TIMERS */
      /**\todo avoid call to UTG in TAPI and free resources in LL driver */
      if (pDrvCtx->SIG.UTG_Stop != IFX_NULL)
      {
         (IFX_void_t)pDrvCtx->SIG.UTG_Stop(pChannel->pLLChannel, nResID);
      }

      pData->nToneState = TAPI_CT_IDLE;
      if (pData->bSendToneGenEndTapiEvent)
      {
         /* issue TAPI event to the application */
         memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
         tapiEvent.id = IFX_TAPI_EVENT_TONE_GEN_END;
         switch (pData->dst)
         {
            case TAPI_TONE_DST_DEFAULT:
            case TAPI_TONE_DST_LOCAL:
               if (IFX_TAPI_MODULE_TYPE_ALM == pData->module)
                  TAPI_ALM_EXTERNAL (tapiEvent.data.tone_gen) = 1;
               else
                  TAPI_COD_INTERNAL (tapiEvent.data.tone_gen) = 1;
               break;
            case TAPI_TONE_DST_NET:
               if (IFX_TAPI_MODULE_TYPE_ALM == pData->module)
                  TAPI_ALM_INTERNAL (tapiEvent.data.tone_gen) = 1;
               else
                  TAPI_COD_EXTERNAL (tapiEvent.data.tone_gen) = 1;
               break;
            case TAPI_TONE_DST_NETLOCAL:
               TAPI_COD_EXTERNAL (tapiEvent.data.tone_gen) = 1;
               TAPI_COD_INTERNAL (tapiEvent.data.tone_gen) = 1;
               break;
         }

#ifdef TAPI_VERSION4
         tapiEvent.module = pData->module;
#endif /* TAPI_VERSION4 */
         tapiEvent.data.tone_gen.index = (IFX_uint32_t)pData->nToneIndex;
         IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
      }
   }

   TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);
   return;
}

TAPI_CMPLX_TONE_STATE_t TAPI_ToneState (TAPI_CHANNEL *pChannel, IFX_uint8_t nResId)
{
   return pChannel->TapiComplexToneData[nResId].nToneState;
}


/**
   Simple tone deactivated event handling function.

   \param  pChannel     Pointer to TAPI_CHANNEL structure.
   \param  nResID       Resource ID of the tone.

   \remarks
   This function is called from interrupt level. Is handles
   tone resource 0 or 1.
*/
static IFX_void_t Event_SimpleTone(TAPI_CHANNEL *pChannel, IFX_uint8_t nResID)
{
   TAPI_TONE_DATA_t *pData;

   /* get context for this resource */
   pData = &pChannel->TapiComplexToneData[nResID];

   /* One simple tone sequence has been applied on this channel */
   pData->nSimpleCurrReps++;

   if (pData->nPauseTime == 0)
   {
      /* Special case: A zero pause time was specified on this channel,
      therefore ignore the repetition count.
      The simple tone sequence must be applied only once.

      For DxT and xT16 (TG) this special treatment is wrong, as we need to
      continue with the next step via Tone_OnTimer...
      */
      if (! pChannel->pTapiDevice->pDevDrvCtx->ALM.TG_Play)
         return;
   }

   /* An infinite tone sequence repetition was requested
      on this channel or there are still simple tone sequences to repeat.
      Repeat tone sequence again after the voice path timer expires.
      Special case for loop = 1, no pause to be played on simple tone
      but on composed tone. Handling of that pause is done on higher
      level */
   if ((pData->nSimpleMaxReps == 0) ||
       (pData->nSimpleCurrReps < pData->nSimpleMaxReps))
   {
      /* Tone generation is in pause state */
      pData->nToneState = TAPI_CT_ACTIVE_PAUSE;
#ifdef TAPI_HAVE_TIMERS
      /* Start the voice path teardown timer */
      TAPI_SetTime_Timer(pChannel->pToneRes[nResID].Tone_Timer,
                         pData->nPauseTime, IFX_FALSE, IFX_FALSE);
#endif /* TAPI_HAVE_TIMERS */
   }
}

/**
   Composed tone deactivated event handling function.

   \param  pChannel     Pointer to TAPI_CHANNEL structure.
   \param  nResID       Resource ID of the tone.
*/
static IFX_void_t Event_ComposedTone (TAPI_CHANNEL *pChannel,
                                      IFX_uint8_t nResID)
{
   TAPI_TONE_DATA_t *pData;

   pData = &pChannel->TapiComplexToneData[nResID];
   /* One composed tone sequence has been applied on this channel */
   pData->nComposedCurrReps++;
   if (pData->nAlternateVoiceTime == 0)
   {
   /* Special case: A zero alternate voice time was specified on this channel,
     therefore ignore the repetition count.
     The composed tone sequence must be applied only once.   */

      /* Set tone generation back to initial state */
   }
   /* An infinite tone sequence repetition was requested
      on this channel or there are still composed tone sequences to repeat.
      Repeat tone sequence again after the voice path timer expires.
      Consider also the pause time of the last simple tone if any */
   if ((pData->nComposedMaxReps == 0) ||
       (pData->nComposedCurrReps < pData->nComposedMaxReps) ||
       pData->nPauseTime != 0)
   {
      /* Tone generation is in pause state */
      pData->nToneState = TAPI_CT_ACTIVE_PAUSE;
#ifdef TAPI_HAVE_TIMERS
      TAPI_SetTime_Timer(pChannel->pToneRes[nResID].Tone_Timer,
                         pData->nAlternateVoiceTime + pData->nPauseTime,
                         IFX_FALSE, IFX_FALSE);
#endif /* TAPI_HAVE_TIMERS */
   }
}

#ifdef TAPI_VERSION3
/* Note: this function shall be replaced by TAPI_Tone_ResIdGet */
IFX_void_t TAPI_Tone_Set_Source (TAPI_CHANNEL *pChannel, IFX_uint8_t res,
                                 IFX_int32_t src)
{
   pChannel->TapiComplexToneData[res].sequenceCap = src;
}
#endif /* TAPI_VERSION3 */

/**
   Gets the duration it will take to play a specified tone.

   This function calculates the duration that the given tone will play and
   returns it to the caller.

   \param   nToneIndex  Tone table index of the tone to be calculated.

   \return  Duration in ms.
            The value 0 is special and indicates an infinite sequence.
*/
IFX_uint32_t IFX_TAPI_Tone_DurationGet(IFX_uint32_t nToneIndex)
{
   IFX_uint32_t         nSimpleToneIndex;
   IFX_TAPI_TONE_SIMPLE_t *pToneSimple;
   COMPLEX_TONE         *pToneCoeff;
   IFX_uint16_t         nSimpleStep,
                        nComplexStep = 0;
   IFX_uint32_t         nSimpleDuration,
                        nTotalDuration = 0;
   IFX_boolean_t        nMoreSimpleTones = IFX_TRUE;

   /* check tone index parameter */
   if (nToneIndex >= TAPI_MAX_TONE_CODE)
   {
      return 0;
   }

   /* Use the tone code as an index into the internal tone coefficients table */
   pToneCoeff = &(toneTable[nToneIndex]);

   while (nMoreSimpleTones == IFX_TRUE)
   {
      switch (pToneCoeff->type)
      {
      case TAPI_TONE_TYPE_COMP:
         /* For composed tones fetch the next simple tone */
         nSimpleToneIndex = pToneCoeff->tone.composed.tones[nComplexStep];
         nComplexStep++;
         if ((nComplexStep >= IFX_TAPI_TONE_SIMPLE_MAX) ||
             (nComplexStep >= pToneCoeff->tone.composed.count))
         {
            nMoreSimpleTones = IFX_FALSE;
         }
         break;
      case TAPI_TONE_TYPE_SIMPLE:
         /* For simple tones the index already points to a simple tone */
         nSimpleToneIndex = nToneIndex;
         nMoreSimpleTones = IFX_FALSE;
         break;
      default:
         /* Reference to unconfigured tone code entry */
         return 0;
      }

      /* retrieve simple tone from internal tone coefficients table */
      pToneSimple = &(toneTable[nSimpleToneIndex].tone.simple);

      if (pToneSimple->loop == 0)
      {
         /* a loop of 0 defines an infinite repetition -> duration infinite */
         return 0;
      }

      /* Calculate the duration of this simple tone*/
      /* Duration is the sum of all cadences plus the pause multiplied by
         the loop factor.*/
      nSimpleDuration = 0;
      for (nSimpleStep = 0; (nSimpleStep < IFX_TAPI_TONE_STEPS_MAX) &&
                            (pToneSimple->cadence[nSimpleStep] != 0); nSimpleStep++)
      {
         nSimpleDuration += pToneSimple->cadence[nSimpleStep];
      }
      nSimpleDuration += pToneSimple->pause;
      nSimpleDuration *= pToneSimple->loop;
      /* Add the partial result after multiplication to the total result */
      nTotalDuration  += nSimpleDuration;

   }

   if (pToneCoeff->type == TAPI_TONE_TYPE_COMP)
   {
      /* a loop of 0 defines an infinite repetition -> duration infinite */
      nTotalDuration  *= pToneCoeff->tone.composed.loop;
   }

   return nTotalDuration;
}
