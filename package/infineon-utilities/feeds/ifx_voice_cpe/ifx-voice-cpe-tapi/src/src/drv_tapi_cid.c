/******************************************************************************

                              Copyright (c) 2009
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

/**
   \file drv_tapi_cid.c
   Implementation of features related to CID (CID1/CID2/CID RX)

   \remarks
   All operations done by functions in this module are data
   related and assume a data channel file descriptor.
   Caller of anyone of the functions must make sure that a data
   channel is used.
*/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_tapi.h"

#if (TAPI_CFG_FEATURES & TAPI_FEAT_CID)

#include "drv_tapi_cid.h"
#include "drv_tapi_errno.h"
#include "drv_tapi_ppd.h"

/* ================================= */
/* FSK defines (transmission time)   */
/* ================================= */

/* CID FSK transmit time tolerance,
   set to 5 ms for cid request time and software processing */
#define CID_FSK_TOLERANCE_MS     5

/* CID FSK transmission bitrate,  1200 bits/sec */
#define CID_FSK_BITRATE_SEC      1200

/* Number of bits per FSK character data
  Note : The CID sender encode each character data byte in a 10 bit long byte  */
#define CID_FSK_BYTE_BITSNUM     10

/* ============================= */
/* FSK defines (ETSI)            */
/* ============================= */

/* size of date element */
#define CIDFSK_DATE_LEN          0x08

/* maximum length of Service Type elements (the element buffer size is
   independent and may be an additional restriction) */
#define CID_ST_MSGIDENT_LEN      3
#define CID_ST_DURATION_LEN      6
#define CID_ST_XOPUSE_LEN        10
#define CID_ST_CHARGE_LEN        16
#define CID_ST_NUM_LEN           20
#define CID_ST_TERMSEL_LEN       21
#define CID_ST_NAME_LEN          50
#if 1 /* ctc */
#define CID_ST_DISP_LEN          255	//Siemens request to extended to 255   253
#else
#define CID_ST_DISP_LEN          253
#endif

/* ============================= */
/* FSK defines (NTT)             */
/* ============================= */

/* positions in buffer */
#define CID_NTT_TYPE_POS         5
#define CID_NTT_MSGLEN_POS       6

/* parameter types with corresponding length */
#define CID_NTT_PT_NUMBER        0x02
#define CID_NTT_NUMBER_MAXLEN    0x14
#define CID_NTT_PT_NAME          0x03
#define CID_NTT_NAME_MAXLEN      0x14
#define CID_NTT_PT_ABSENCE       0x04

/* control codes <b7b6b5 b4b3b2b1> */
#define CID_NTT_DLE              0x10   /* 0b 001 0000 */
#define CID_NTT_SOH              0x01   /* 0b 000 0001 */
#define CID_NTT_HEADER           0x07   /* 0b 000 0111 */
#define CID_NTT_STX              0x02   /* 0b 000 0010 */
#define CID_NTT_TYPE1            0x40   /* 0b 100 0000 */
#define CID_NTT_TYPE2            0x41   /* 0b 100 0001 */
#define CID_NTT_ETX              0x03   /* 0b 000 0011 */
/*#define CID_NTT_SI             0x0F*/ /* 0b 000 1111 */
/*#define CID_NTT_SO             0x0E*/ /* 0b 000 1110 */

/* timing for second ack signal */
#define CID_NTT_ACK2_POLLTIME    500    /* ms */
#define CID_NTT_ACK2_DEF_TIMEOUT 7000   /* ms */

/* ============================= */
/* Local macro declaration       */
/* ============================= */

/* check value and limit against a maximum */
#define LIMIT_VAL(val,maxval) \
   do                         \
   {                          \
      if ((val) > (maxval))   \
         (val) = (maxval);    \
   } while(0)

#define MIN_TIMER_VAL   10

/* ============================= */
/* Local enum declaration        */
/* ============================= */

/** Status of state machines for upper layers. */
typedef enum
{
   /** state machine not ready */
   E_FSM_CONTINUE,
   /** abort with error */
   E_FSM_ERROR,
   /** state machine finished */
   E_FSM_COMPLETE
} FSM_STATUS_t;


typedef enum
{
   /** ready to start sequence */
   TAPI_CID_STATE_IDLE,
   /** alert active */
   TAPI_CID_STATE_ALERT,
   /** waiting for ack */
   TAPI_CID_STATE_ACK,
   /** fsk or dtmf sending active */
   TAPI_CID_STATE_SENDING,
   /** fsk or dtmf sending completed (or timed out) */
   TAPI_CID_STATE_SENDING_COMPLETE
} TAPI_CID_STATE;

/* ============================= */
/* Structure for CID data        */
/* ============================= */

/** Caller ID generator types. */
typedef enum
{
   IFX_TAPI_CID_GEN_TYPE_FSK,
   IFX_TAPI_CID_GEN_TYPE_DTMF,
   IFX_TAPI_CID_GEN_TYPE_MAX
} IFX_TAPI_CID_GEN_TYPE_t;

typedef struct
{
   /* Buffer for CID data coding  */
   IFX_uint8_t pBuf [IFX_TAPI_CID_TX_SIZE_MAX];
   /* number of CID data octets */
   IFX_uint16_t nLen;
   /* Calculated transmission time according to buffer size */
   IFX_uint32_t nTxTime;
} TAPI_CIDTX_BUFF_t;

typedef struct
{
   /* sets CID type 1 (ON-) or type 2 (OFF-HOOK) */
   IFX_TAPI_CID_HOOK_MODE_t    txHookMode;

   /* The type of CID data generator to be used */
   IFX_TAPI_CID_GEN_TYPE_t cidGenType;

   /* Buffer for CID parameter coding, separate for each generator */
   TAPI_CIDTX_BUFF_t       cidBuf[IFX_TAPI_CID_GEN_TYPE_MAX];

   /* use the state machine to play a sequence or send data without sequence */
   IFX_boolean_t           bUseSequence;
   /* flag to know if transmission/sequence is active (and data is new) */
   IFX_boolean_t           bActive;

   /* reverse line mode for standards with Line Reversal */
   IFX_uint32_t            nLineModeReverse;
   /* current line mode that is set when CID tx is started */
   IFX_uint32_t            nLineModeInitial;

#ifdef TAPI_HAVE_TIMERS
   /* Timer for caller id handling */
   Timer_ID                CidTimerID;
#endif /* TAPI_HAVE_TIMERS */

   /* CID transmission state */
   TAPI_CID_STATE          nCidState;
   /* common sub state (used by alert,...) */
   IFX_uint32_t            nCidSubState;

   /* alert type for sequence */
   IFX_TAPI_CID_ALERT_TYPE_t nAlertType;

   /* required acknowledge tone as chip specific code */
   IFX_uint8_t             ackToneCode;
   /* acknowledge flags */
   volatile IFX_boolean_t  bAck;
   volatile IFX_boolean_t  bAck2;
   /* flag showing that OSI linemode change caused a hook event */
   IFX_boolean_t           bOSIinducedOnhook;
   IFX_boolean_t           bOSIinducedOffhook;

   /* flag, if muting for off hook cid was done */
   IFX_boolean_t           bMute;
   /* flag for starting periodical ringing */
   IFX_boolean_t           bRingStart;
   /* stores the ring burst state during CID generated alert ring sequences */
   IFX_boolean_t           bAlertRingBurstState;

   /* Instance of phone channel mapped to
      this data channel */
   TAPI_CHANNEL            *pPhoneCh;
} TAPI_CIDTX_DATA_t;


typedef struct
{
   /* status */
   IFX_TAPI_CID_RX_STATUS_t   stat;
   /* ptr to actual cid rx buffer */
   IFX_TAPI_CID_RX_DATA_t     *pData;
   /* cid rx fifo */
   IFX_FIFO                   TapiCidRxFifo;
} TAPI_CIDRX_DATA_t;

struct TAPI_CID_DATA
{
   /* caller id config */
   IFX_TAPI_CID_CONF_t           ConfData;
   /* caller id receiver data */
   TAPI_CIDRX_DATA_t    RxData;
   /* caller id transmit data */
   TAPI_CIDTX_DATA_t    TxData;
};

/* ============================= */
/* Local functions declaration   */
/* ============================= */

/* DTMF (ETSI) */
static IFX_void_t   ciddtmf_abscli_etsi (
                        IFX_TAPI_CID_ABS_REASON_t *pAbs);

static IFX_uint32_t ciddtmf_codeabscli (
                        IFX_TAPI_CID_ABS_REASON_t const *pAbsence,
                        IFX_uint32_t nMsgElements,
                        IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage,
                        IFX_char_t tone,
                        IFX_uint8_t *pBuf);

static IFX_uint32_t ciddtmf_codetype (
                        IFX_TAPI_CID_SERVICE_TYPE_t type,
                        IFX_uint32_t nMsgElements,
                        IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage,
                        IFX_char_t tone,
                        IFX_uint8_t *pBuf);

static IFX_int32_t  ciddtmf_code (
                        TAPI_CIDTX_DATA_t *pTxData,
                        IFX_TAPI_CID_DTMF_CFG_t const *pCidDtmfConf,
                        IFX_TAPI_CID_ABS_REASON_t const *pDtmfAbsCli,
                        IFX_uint32_t nMsgElements,
                        IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage);

/* FSK (ETSI) */
static IFX_void_t   cidfsk_codedate (
                        IFX_TAPI_CID_SERVICE_TYPE_t type,
                        IFX_TAPI_CID_MSG_DATE_t const *date,
                        IFX_uint8_t *pCidBuf,
                        IFX_uint16_t *pPos);

static IFX_void_t   cidfsk_codetype (
                        IFX_TAPI_CID_SERVICE_TYPE_t type,
                        IFX_uint8_t const *buf,
                        IFX_uint8_t len,
                        IFX_uint8_t *pCidBuf,
                        IFX_uint16_t *pPos);

static IFX_uint8_t  cidfsk_calccrc (
                        IFX_uint8_t const *pCidBuf,
                        IFX_uint16_t len);

static IFX_int32_t  cidfsk_code (
                        TAPI_CIDTX_DATA_t *pTxData,
                        IFX_TAPI_CID_MSG_TYPE_t messageType,
                        IFX_uint32_t nMsgElements,
                        IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage);

/* FSK (NTT) */
static IFX_void_t   cidfskntt_setparity (
                        IFX_uint8_t *byte);

static IFX_void_t   cidfskntt_codetype (
                        IFX_uint8_t type,
                        IFX_uint8_t const *buf,
                        IFX_uint8_t len,
                        IFX_uint8_t *pCidBuf,
                        IFX_uint16_t *pPos);

static IFX_uint16_t cidfskntt_calccrc (
                        IFX_uint8_t const *pCidBuf,
                        IFX_uint16_t len);

static IFX_int32_t  cidfskntt_code (
                        TAPI_CIDTX_DATA_t *pTxData,
                        IFX_TAPI_CID_HOOK_MODE_t txMode,
                        IFX_uint32_t nMsgElements,
                        IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage);

/* CID local functions */
static IFX_void_t   cid_set_alerttone_time (
                        TAPI_CHANNEL *pChannel,
                        IFX_TAPI_CID_MSG_t const *pCidInfo);

static IFX_int32_t  cid_mutevoice (
                        TAPI_CHANNEL *pChannel,
                        IFX_boolean_t bMute);

static IFX_void_t   cidfsk_set_tx_time (
                        TAPI_CIDTX_DATA_t *pTxData,
                        IFX_TAPI_CID_FSK_CFG_t* pFskConf);

static IFX_int32_t  cid_lookup_transparent (
                        TAPI_CIDTX_DATA_t *pTxData,
                        IFX_TAPI_CID_CONF_t *pConfData,
                        IFX_uint32_t nMsgElements,
                        IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage);

static IFX_int32_t  cid_prepare_data (
                        TAPI_CHANNEL *pChannel,
                        IFX_TAPI_CID_MSG_t const *pCidInfo);

static IFX_int32_t  cid_determine_alert_type (
                        TAPI_CHANNEL *pChannel,
                        IFX_TAPI_CID_MSG_t const *pCidInfo);

/* State Machine callbacks */
#ifdef TAPI_HAVE_TIMERS
static FSM_STATUS_t cid_fsm_alert_exec (
                        TAPI_CHANNEL *pChannel);

static FSM_STATUS_t cid_fsm_ack_exec (
                        TAPI_CHANNEL *pChannel);

static FSM_STATUS_t cid_fsm_sending_exec (
                        TAPI_CHANNEL *pChannel);

static FSM_STATUS_t cid_fsm_sending_complete_exec (
                        TAPI_CHANNEL *pChannel);

static FSM_STATUS_t cid_fsm_exec (
                        TAPI_CHANNEL *pChannel);

static IFX_void_t   ifx_tapi_cid_OnTimer (
                        Timer_ID Timer, IFX_ulong_t nArg);
#endif /* TAPI_HAVE_TIMERS */

static IFX_void_t   ifx_tapi_cid_SetDefaultConfig (
                        TAPI_CHANNEL *pChannel);

static IFX_int32_t  ifx_tapi_cid_RxDataCollectedGet (
                        TAPI_CHANNEL *pChannel,
                        IFX_TAPI_CID_RX_DATA_t *pCidRxData);


/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* default settings for cid timing configuration */
static const IFX_TAPI_CID_TIMING_t default_cid_timing =
{
   300,  /* beforeData */
   300,  /* dataOut2restoreTimeOnhook */
   60,   /* dataOut2restoreTimeOffhook */
   100,  /* ack2dataOutTime */
   160,  /* cas2ackTime */
   50,   /* afterAckTimeout */
   600,  /* afterFirstRing */
   500,  /* afterRingPulse */
   45,   /* afterDTASOnhook */
   100,  /* afterLineReversal */
   300,  /* afterOSI */
};

/*
  According to CID type (onhook=Type1, Offhook=Type2), different
   Mark/Seizure configuration apply according to specifications:
   GR-30-CORE : Type 1 (Onhook)  => Seizure = 300 bits, Mark = 180 bits
                Type 2 (Offhook) => Seizure = 0 bits, Mark = 80 bits
   ETSI       : Type 1 (Onhook)  => Seizure = 300 bits, Mark = 180+/-25 bits
                Type 2 (Offhook) => Seizure = 0 bits, Mark =  80+/-25bits
   SIN227     : Type 1 (Onhook)  => Seizure = 96 to 315 bits, Mark >= 55 bits
                Type 2 (Offhook) => Seizure = 0 bits, Mark >= 55 bits

   To meet all these specifications, following is set as default:
      Type 1 (Onhook)  => Seizure = 300 bits, Mark = 180 bits
      Type 2 (Offhook) => Seizure =   0 bits, Mark = 80 bits

   Note: for CID2 there is no channel seizure neccessary. So the the seizure is
   always 0 by definition and hard coded in the LL drivers.
*/

/* default settings for cid fsk configuration */
static const IFX_TAPI_CID_FSK_CFG_t default_cid_fsk_conf =
{
   -140, /* levelTX */
   -300, /* levelRX */
   300 , /* seizureTX on-hook */
   200 , /* seizureRX on-hook */
   180 , /* markTXOnhook */
   80  , /* markTXOffhook */
   150 , /* markRXOnhook */
   50  , /* markRXOffhook */
   0   , /* stopTXOnhook */
   0     /* stopTXOffhook */
};

/* default settings for cid fsk NTT configuration */
static const IFX_TAPI_CID_FSK_CFG_t default_cid_fsk_ntt_conf =
{
   -140, /* levelTX */
   -300, /* levelRX */
      0, /* seizureTX */
      0, /* seizureRX */
     78, /* markTXOnhook */
     78, /* markTXOffhook */
     50, /* markRXOnhook */
     50, /* markRXOffhook */
      0, /* stopTXOnhook */
      0  /* stopTXOffhook */
};

/* default settings for cid dtmf configuration */
static const IFX_TAPI_CID_DTMF_CFG_t default_cid_dtmf_conf =
{
   'A',  /* startTone */
   'C',  /* stopTone */
   'B',  /* infoStartTone */
   'D',  /* redirStartTone */
   70,   /* digitTime */
   70    /* interDigitTime */
};

/* default settings for KPN cid dtmf configuration */
static const IFX_TAPI_CID_DTMF_CFG_t default_kpn_cid_dtmf_conf =
{
   'D',  /* startTone */
   'C',  /* stopTone */
   'B',  /* infoStartTone */
   'D',  /* redirStartTone */
   70,   /* digitTime */
   70    /* interDigitTime */
};

/* default settings for TDC cid dtmf configuration */
static const IFX_TAPI_CID_DTMF_CFG_t default_tdc_cid_dtmf_conf =
{
   'A',  /* startTone */
   '#',  /* stopTone */
   'B',  /* infoStartTone */
   'D',  /* redirStartTone */
   70,   /* digitTime */
   70    /* interDigitTime */
};

/* ============================= */
/* Local function definition     */
/* ============================= */

/**
   Sets ETSI parameters as default for ABSCLI reasons
   "unavailable" and "priv".
   \param  pAbs         Pointer to IFX_TAPI_CID_ABS_REASON_t structure,
                        must be valid.
   \remarks
   This function is called at CID configuration for the standard
   IFX_TAPI_CID_STD_ETSI_DTMF only.
*/
static IFX_void_t ciddtmf_abscli_etsi (IFX_TAPI_CID_ABS_REASON_t *pAbs)
{
   if (pAbs == IFX_NULL)
      return;
   memset (pAbs->priv, 0, sizeof(IFX_TAPI_CID_MSG_LEN_MAX));
   memset (pAbs->unavailable, 0, sizeof(IFX_TAPI_CID_MSG_LEN_MAX));
   /* ETSI DTMF : */
   pAbs->len = 2;
   /* ETSI DTMF : unavailable = "00" */
   pAbs->unavailable [0] = '0';
   pAbs->unavailable [1] = '0';
   /* ETSI DTMF : priv = "01" */
   pAbs->priv [0] = '0';
   pAbs->priv [1] = '1';
}

/**
   Codes data of IFX_TAPI_CID_ST_ABSCLI type in CID buffer for DTMF

   \param  pAbsence     Pointer to DTMF absence data coded at configuration time.
                        (Default is ETSI)
   \param  nMsgElements Number of message elements.
   \param  pMessage     Pointer to message elements.
   \param  tone         Signalling tone for element.
   \param  pBuf         Start position in cid data buffer.

   \return
   Length of encoded element or 0 on error / missing element.
*/
static IFX_uint32_t ciddtmf_codeabscli (IFX_TAPI_CID_ABS_REASON_t const *pAbsence,
                                        IFX_uint32_t nMsgElements,
                                        IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage,
                                        IFX_char_t tone,
                                        IFX_uint8_t *pBuf)
{
   IFX_TAPI_CID_MSG_ELEMENT_t const *pMsg;
   IFX_uint32_t         i;
   IFX_uint32_t         pos = 0;
   IFX_uint8_t          *reason = IFX_NULL;

   for (pMsg = pMessage; pMsg < pMessage + nMsgElements; pMsg++)
   {
      if (pMsg->value.elementType == IFX_TAPI_CID_ST_ABSCLI)
      {
         switch (pMsg->value.element)
         {
            case  IFX_TAPI_CID_ABSREASON_UNAV:
               reason = (IFX_uint8_t *)pAbsence->unavailable;
               break;
            case  IFX_TAPI_CID_ABSREASON_PRIV:
               reason = (IFX_uint8_t *)pAbsence->priv;
               break;
            default:
               /* do nothing */
               break;
         }

         /* if no data in this element check the next one */
         if (reason == IFX_NULL)
            break;

         /* set element signalling tone */
         pBuf[pos++] = (IFX_uint8_t)tone;

         /* copy data to internal CID buffer */
         for (i = 0; i < pAbsence->len; i++)
         {
            pBuf [pos++] = reason[i];
         }
         /* leave loop after copying the first element found of this type,
            only one (the first) element of a type is used */
         break;
      }
   }

   /* return the length of the copied element */
   return pos;
}

/**
   Codes data of indicated type in CID buffer for DTMF

   \param  type         Service type.
   \param  nMsgElements Number of message elements.
   \param  pMessage     Pointer to message elements.
   \param  tone         Signalling tone for element.
   \param  pBuf         Start position in cid data buffer.

   \return
   Length of copied element or 0 on error / missing element.
*/
static IFX_uint32_t ciddtmf_codetype (IFX_TAPI_CID_SERVICE_TYPE_t type,
                                      IFX_uint32_t nMsgElements,
                                      IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage,
                                      IFX_char_t tone,
                                      IFX_uint8_t *pBuf)
{
   IFX_TAPI_CID_MSG_ELEMENT_t const *pMsg;
   IFX_uint32_t         i, len;
   IFX_uint32_t         pos = 0;

   for (pMsg = pMessage; pMsg < pMessage + nMsgElements; pMsg++)
   {
      if (pMsg->string.elementType == type)
      {
         len = pMsg->string.len;
         LIMIT_VAL(len,CID_ST_NUM_LEN);
         /* set element signalling tone */
         pBuf[pos++] = (IFX_uint8_t)tone;
         /* copy data to internal CID buffer */
         for (i = 0; i < len; i++)
         {
            pBuf [pos++] = pMsg->string.element[i];
         }
         /* leave loop after copying the first element found of this type,
            only one (the first) element of a type is used */
         break;
      }
   }

   /* return the length of the copied element */
   return pos;
}

/**
   Code CID parameters for DTMF transmission.

   \param  pTxData      Pointer to TAPI_CIDTX_DATA_t structure.
   \param  pCidDtmfConf Pointer to IFX_TAPI_CID_DTMF_CFG_t structure.
   \param  pDtmfAbsCli  Pointer to IFX_TAPI_CID_ABS_REASON_t structure.
   \param  nMsgElements Number of message elements to add.
   \param  pMessage     Pointer to message elements.

   \return
   TAPI_statusOk - success
   TAPI_statusParam - no valid data available (len)
*/
static IFX_int32_t ciddtmf_code (TAPI_CIDTX_DATA_t *pTxData,
                                 IFX_TAPI_CID_DTMF_CFG_t const *pCidDtmfConf,
                                 IFX_TAPI_CID_ABS_REASON_t const *pDtmfAbsCli,
                                 IFX_uint32_t nMsgElements,
                                 IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage)
{
   TAPI_CIDTX_BUFF_t    *pCidData =
      &pTxData->cidBuf[IFX_TAPI_CID_GEN_TYPE_DTMF];
   IFX_uint8_t          *pCidBuf = pCidData->pBuf;
   IFX_uint16_t         *pPos    = &pCidData->nLen,
                        i;
   IFX_uint32_t         len;

   /* initialize pos */
   *pPos = 0;

   /* encode CLI data.
      Note : CLI or ABSCLI are mandatory */
   len = ciddtmf_codetype(IFX_TAPI_CID_ST_CLI, nMsgElements, pMessage,
                          pCidDtmfConf->startTone, &pCidBuf [*pPos]);
   /* No CLI data, encode ABSCLI data.
      Note : CLI or ABSCLI are mandatory  */
   if (len == 0)
   {
      /* ABSCLI is sent as Information message. No further information message
         should be code in the buffer. */
      len = ciddtmf_codeabscli (pDtmfAbsCli, nMsgElements, pMessage,
                                pCidDtmfConf->infoStartTone, &pCidBuf [*pPos]);
      /* No CLI, no ABSCLI : error, at least one of both must be present */
      if (len == 0)
      {
         TRACE (TAPI_DRV,DBG_LEVEL_NORMAL,
            ("\nDRV_ERROR: No valid data for DTMF CID, "
               "mandatory element CLI or ABSCLI missing!\n"));
         return TAPI_statusParam;
      }
   }
   *pPos += len;

   /* encode Redirection data */
   len = ciddtmf_codetype(IFX_TAPI_CID_ST_REDIR, nMsgElements, pMessage,
                          pCidDtmfConf->redirStartTone, &pCidBuf [*pPos]);
   *pPos += len;

   /* set the stop tone */
   pCidBuf [(*pPos)++] = (IFX_uint8_t)pCidDtmfConf->stopTone;

   /* ensure that all characters are valid DTMF digits */
   for (i=0; i< *pPos; i++)
   {
      /* The characters 0-9, A-D, *, # are ok. All others are rejected. */
      if (((pCidBuf[i] >= '0') && (pCidBuf[i] <= '9')) ||
          ((pCidBuf[i] >= 'A') && (pCidBuf[i] <= 'D')) ||
           (pCidBuf[i] == '*') || (pCidBuf[i] == '#') )
         continue;

      /* errmsg: Invalid character passed in DTMF CID message element */
      return TAPI_statusCIDInvalCharInDtmfMsgElem;
   }

   /* set transmission time now */
   pCidData->nTxTime =
      ((pCidDtmfConf->digitTime + pCidDtmfConf->interDigitTime) * (*pPos));

   return TAPI_statusOk;
}


/**
   Codes date in CID buffer.

   No check is done for valid values of date element.

   \param  type         Type.
   \param  date         Pointer to date message element.
   \param  pCidBuf      Pointer to CID data buffer.
   \param  pPos         Current data position in cid buffer.

   \remarks
   ref ETSI EN 300 659-1 V.1.3.1
*/
static IFX_void_t cidfsk_codedate (IFX_TAPI_CID_SERVICE_TYPE_t type,
                                   IFX_TAPI_CID_MSG_DATE_t const *date,
                                   IFX_uint8_t *pCidBuf,
                                   IFX_uint16_t *pPos)
{
   /* set date and time parameter type : 0x01 or 0x0F */
   pCidBuf [(*pPos)++] = type;
   /* set date and time parameter length : 8 */
   pCidBuf [(*pPos)++] = CIDFSK_DATE_LEN;
   /* set month most significant digit */
   pCidBuf [(*pPos)++] = ((IFX_uint8_t)date->month / 10) + '0';
   /* set month least significant digit */
   pCidBuf [(*pPos)++] = (date->month % 10) + '0';
   /* set day most significant digit */
   pCidBuf [(*pPos)++] = ((IFX_uint8_t)date->day / 10) + '0';
   /* set day least significant digit */
   pCidBuf [(*pPos)++] = (date->day % 10) + '0';
   /* set hour most significant digit */
   pCidBuf [(*pPos)++] = ((IFX_uint8_t)date->hour / 10) + '0';
   /* set hour least significant digit */
   pCidBuf [(*pPos)++] = (date->hour % 10) + '0';
   /* set minute most significant digit */
   pCidBuf [(*pPos)++] = ((IFX_uint8_t)date->mn / 10) + '0';
   /* set minute least significant digit */
   pCidBuf [(*pPos)++] = (date->mn % 10) + '0';
}

/**
   Codes data of indicated type in CID buffer

   \param  type         Service type.
   \param  buf          Buffer with information (octets).
   \param  len          Buffer size in octets (=bytes).
   \param  pCidBuf      Pointer to CID data buffer.
   \param  pPos         Current data position in cid buffer.
*/
static IFX_void_t cidfsk_codetype (IFX_TAPI_CID_SERVICE_TYPE_t type,
                                   IFX_uint8_t const *buf,
                                   IFX_uint8_t len,
                                   IFX_uint8_t *pCidBuf,
                                   IFX_uint16_t *pPos)
{
   IFX_uint8_t i;

   /* set service type */
   pCidBuf[(*pPos)++] = type;
   /* set element length  */
   pCidBuf[(*pPos)++] = len;
   /* copy element content */
   for (i = 0; i < len; i++)
   {
      pCidBuf[(*pPos)++] = buf[i];
   }
}

/**
   Calculate checksum of given CID buffer

   \param  pCidBuf      Pointer to CID buffer of which the checksum will be
                        calculated.
   \param  len          Length of the FSK buffer.

   \return Calculated CRC value.

   \remarks
   ref ETSI EN 300 659-1 V.1.3.1
   checksum calculation : twos complement of the modulo 256 sum
   of all the octets in the message starting from the Message type octet
   up to the end of message (excluding checksum it self)
*/
static IFX_uint8_t cidfsk_calccrc (IFX_uint8_t const *pCidBuf,
                                   IFX_uint16_t len)
{
   IFX_uint32_t sumCID = 0;
   IFX_uint16_t i;
   IFX_uint8_t  crc;

   /* sum all octets */
   for (i = 0; i < len; i++)
   {
      sumCID += pCidBuf [i];
   }
   /* calculate twos complement as crc value */
   crc = ((~(sumCID & 0xFF)) + 1);

   return crc;
}

/**
   Set the FSK transmission time according to mark bits, seizure bits
   and data size in characters (bytes) to transmit.

   \param  pTxData      Pointer to TAPI_CIDTX_DATA_t structure.
   \param  pFskConf     Pointer to IFX_TAPI_CID_FSK_CFG_t structure.

   /remarks
   CID baudrate = 150 Bytes/sec = 1200 bits/sec without Pause.
   /remarks
   bits_num = mark_bits + seizure_bits + (data_bytes_num * 10)
*/
static IFX_void_t cidfsk_set_tx_time (TAPI_CIDTX_DATA_t *pTxData,
                                      IFX_TAPI_CID_FSK_CFG_t* pFskConf)
{
   IFX_uint32_t bits_num;

   /* Calculate the number of bits to transmit, assuming each FSK data byte
      is encoded into CID_FSK_BYTE_BITSNUM bits by DSP before sending :
      bits_num = mark_bits + seizure_bits + (data_bytes_num * 10) */
   /* The number of mark bits depend on the hook mode for transmission. */
#if 1 /* Charles:Modify for Type 2 ID */ /* ctc merged */
   bits_num = (pTxData->cidBuf[IFX_TAPI_CID_GEN_TYPE_FSK].nLen * CID_FSK_BYTE_BITSNUM);
   if (pTxData->txHookMode == IFX_TAPI_CID_HM_ONHOOK)
      bits_num += (pFskConf->markTXOnhook + pFskConf->seizureTX);
   else
      bits_num += pFskConf->markTXOffhook;
#else
   bits_num  = (pTxData->txHookMode == IFX_TAPI_CID_HM_ONHOOK) ?
               pFskConf->markTXOnhook : pFskConf->markTXOffhook;
   bits_num += pFskConf->seizureTX +
      (pTxData->cidBuf[IFX_TAPI_CID_GEN_TYPE_FSK].nLen * CID_FSK_BYTE_BITSNUM);

#endif
   /* Calculate and set the FSK transmission time in ms,
      adding a tolerance for cid request time and software processing:
      tx_time_ms = ((tx_bits_num * 1000) / tx_rate) + tx_tolerance */
   pTxData->cidBuf[IFX_TAPI_CID_GEN_TYPE_FSK].nTxTime =
      ((bits_num * 1000) / CID_FSK_BITRATE_SEC) + CID_FSK_TOLERANCE_MS;
}


/**
   Code CID parameters for FSK transmission according to chosen services.
   (ref: Spec ETSI EN 300 659-3.)

   \param  pTxData      Pointer to TAPI_CIDTX_DATA_t structure.
   \param  messageType  Defines the message type (call setup, message waiting, ...).
   \param  nMsgElements Number of message elements to add.
   \param  pMessage     Pointer to message elements.

   \return
   - TAPI_statusOk - Success
   - TAPI_statusCIDBuffNoMem - Not enough memory in CID data buffer.
   - TAPI_statusCIDBuffNoSpaceForDATE - Not enough space in buffer for DATE element.
   - TAPI_statusCIDBuffNoSpaceForElement - Not enough space in buffer for element.
   - TAPI_statusCIDBuffNoSpaceForSTRING - Not enough space in buffer for STRING element.
   - TAPI_statusParam - element type not known.

   \remarks
   CID parameters buffer should be filled according to services.
   Specification used : ETSI EN 300 659-3 V1.3.1 (2001-01)
*/
static IFX_int32_t cidfsk_code(TAPI_CIDTX_DATA_t *pTxData,
                               IFX_TAPI_CID_MSG_TYPE_t messageType,
                               IFX_uint32_t nMsgElements,
                               IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage)
{
   IFX_boolean_t  mandatory_found = IFX_FALSE;
   IFX_uint8_t    *pCidBuf = pTxData->cidBuf[IFX_TAPI_CID_GEN_TYPE_FSK].pBuf;
   IFX_uint16_t   *pPos    = &pTxData->cidBuf[IFX_TAPI_CID_GEN_TYPE_FSK].nLen;
   IFX_uint8_t    crc;
   IFX_uint32_t   i;

   for (i=0; i<nMsgElements; i++)
   {
      switch ((pMessage+i)->string.elementType)
      {
      case IFX_TAPI_CID_ST_CLI:
      case IFX_TAPI_CID_ST_ABSCLI:
         if (messageType==IFX_TAPI_CID_MT_CSUP)
            mandatory_found = IFX_TRUE;
         break;
      case IFX_TAPI_CID_ST_VISINDIC:
         if (messageType==IFX_TAPI_CID_MT_MWI)
            mandatory_found = IFX_TRUE;
         break;
      case IFX_TAPI_CID_ST_CHARGE:
         if (messageType==IFX_TAPI_CID_MT_AOC)
            mandatory_found = IFX_TRUE;
         break;
      case IFX_TAPI_CID_ST_DISP:
         if (messageType==IFX_TAPI_CID_MT_SMS)
            mandatory_found = IFX_TRUE;
         break;
      default:
         break;
      }
      if (mandatory_found == IFX_TRUE)
         break;
   } /* for () */

   if (mandatory_found != IFX_TRUE)
   {
      return TAPI_statusParam;
   }

   /* message type */
   pCidBuf [0] = messageType;
   /* start filling data at position 2 of cid buffer */
   *pPos = 2;

   for (i=0; i < nMsgElements; i++)
   {
      IFX_TAPI_CID_MSG_ELEMENT_t const *pMsg = pMessage + i;
      IFX_TAPI_CID_SERVICE_TYPE_t type = pMsg->string.elementType;
      IFX_uint32_t len = pMsg->string.len;

      switch (type)
      {
      /* Elements with special coding (date, length 8) */
      case IFX_TAPI_CID_ST_DATE:
      case IFX_TAPI_CID_ST_CDATE:
         if (*pPos + 8 > IFX_TAPI_CID_TX_SIZE_MAX)
         {
            /* errmsg: Not enough space in buffer for DATE element */
            return TAPI_statusCIDBuffNoSpaceForDATE;
         }
         cidfsk_codedate ( type, &pMsg->date, pCidBuf, pPos);
         break;
      /* Elements with length 1 */
      case IFX_TAPI_CID_ST_ABSCLI:
      case IFX_TAPI_CID_ST_ABSNAME:
      case IFX_TAPI_CID_ST_VISINDIC:
      case IFX_TAPI_CID_ST_CT:
      case IFX_TAPI_CID_ST_MSGNR:
      case IFX_TAPI_CID_ST_FWCT:
      case IFX_TAPI_CID_ST_USRT:
      case IFX_TAPI_CID_ST_SINFO:
         if (*pPos + 1 > IFX_TAPI_CID_TX_SIZE_MAX)
         {
            /* errmsg: Not enough space in buffer for element */
            return TAPI_statusCIDBuffNoSpaceForElement;
         }
         cidfsk_codetype (type, (IFX_uint8_t*)&pMsg->value.element,
                          1, pCidBuf, pPos);
         break;
      /* Elements with variable size,
         do length limitation depending on type */
      case IFX_TAPI_CID_ST_MSGIDENT:
         LIMIT_VAL(len,CID_ST_MSGIDENT_LEN);
         goto code_string;
      case IFX_TAPI_CID_ST_DURATION:
         LIMIT_VAL(len,CID_ST_DURATION_LEN);
         goto code_string;
      case IFX_TAPI_CID_ST_XOPUSE:
         LIMIT_VAL(len,CID_ST_XOPUSE_LEN);
         goto code_string;
      case IFX_TAPI_CID_ST_CHARGE:
      case IFX_TAPI_CID_ST_ACHARGE:
         LIMIT_VAL(len,CID_ST_CHARGE_LEN);
         goto code_string;
      case IFX_TAPI_CID_ST_CLI:
      case IFX_TAPI_CID_ST_CDLI:
      case IFX_TAPI_CID_ST_LMSGCLI:
      case IFX_TAPI_CID_ST_CCLI:
      case IFX_TAPI_CID_ST_FIRSTCLI:
      case IFX_TAPI_CID_ST_REDIR:
      case IFX_TAPI_CID_ST_NTID:
      case IFX_TAPI_CID_ST_CARID:
         LIMIT_VAL(len,CID_ST_NUM_LEN);
         goto code_string;
      case IFX_TAPI_CID_ST_TERMSEL:
         LIMIT_VAL(len,CID_ST_TERMSEL_LEN);
         goto code_string;
      case IFX_TAPI_CID_ST_NAME:
         LIMIT_VAL(len,CID_ST_NAME_LEN);
         goto code_string;
      case IFX_TAPI_CID_ST_DISP:
      default:
code_string:
         LIMIT_VAL(len,CID_ST_DISP_LEN);
         if (*pPos + len > IFX_TAPI_CID_TX_SIZE_MAX)
         {
            /* errmsg: Not enough space in buffer for STRING element */
            return TAPI_statusCIDBuffNoSpaceForSTRING;
         }
         cidfsk_codetype (type, pMsg->string.element, len, pCidBuf, pPos);
         break;
      }
   }

   /* set message length without first 2 bytes */
   pCidBuf [1] = *pPos - 2;
   /* set crc */
   crc = cidfsk_calccrc (pCidBuf, *pPos);
   pCidBuf [(*pPos)++] = crc;

   return TAPI_statusOk;
}


/* ==================================== */
/* Local functions definition CID NTT */
/* ==================================== */

/**
   Codes data of indicated type in CID NTT buffer

   \param  type         Parameter type.
   \param  buf          Buffer with information data (number or name).
   \param  len          Buffer size in octets.
   \param  pCidBuf      Cid data buffer.
   \param  pPos         Current data position in cid buffer.
*/
static IFX_void_t cidfskntt_codetype (IFX_uint8_t type, IFX_uint8_t const *buf,
                                      IFX_uint8_t len, IFX_uint8_t *pCidBuf,
                                      IFX_uint16_t *pPos)
{
   IFX_uint8_t i;

   /* Parameter type */
   pCidBuf[(*pPos)++] = type;
   /* when data [001 0000] is outputted, DLE is added to the head
      of the data before output and is part of CRC calculation */
   if (len == CID_NTT_DLE)
   {
      pCidBuf[(*pPos)++] = CID_NTT_DLE;
      /* add this to message content length */
      pCidBuf[CID_NTT_MSGLEN_POS] += 1;
   }
   /* Data content length */
   pCidBuf[(*pPos)++] = len;
   /* Data content */
   for (i = 0; i < len; i++)
   {
      pCidBuf[(*pPos)++] = buf[i];
   }
   /* Message content length */
   pCidBuf [CID_NTT_MSGLEN_POS] += (2 + len);
}

/**
   Escape DLE inside the CID buffer length field.
   Background: The character CID_NTT_DLE inside the CID buffer has a special
               meaning. Therefore any occurance of CID_NTT_DLE inside the
               CID buffer (exactly between STX and ETX) must be escaped by
               doubling the character.
   Note:       The length fields of the inner blocks are already escaped
               within cidfskntt_codetype.

   \param  pCidBuf      CID data buffer.
   \param  pPos         Current data position in cid buffer.
*/
static IFX_void_t cidfskntt_dle_escape (IFX_uint8_t *pCidBuf,
                                        IFX_uint16_t *pPos)
{
   IFX_uint16_t i;

   /* if the CID buffer's length field has be be escaped, shift the data and
      insert the escape DLE */
   if (pCidBuf [CID_NTT_MSGLEN_POS] == CID_NTT_DLE)
   {
      /* shift the data one byte further in the buffer to get space
         for the DLE escape character
         pPos is the position of the next free byte at the end of the buffer
       */
      for (i = (*pPos) ; i > CID_NTT_MSGLEN_POS +1 ; i--)
      {
         pCidBuf[i] = pCidBuf[i-1];
      }
      /* ...insert the DLE escape... */
      pCidBuf[CID_NTT_MSGLEN_POS +1] = CID_NTT_DLE;
      /* ...and move the current position by one. */
      (*pPos)++;
   }
}

/**
   Add parity bit if applicable.

   \param  byte         Pointer to byte to be updated.

   \remarks
   \verbatim
   1- The byte to format is assumed to be represented in format <b7b6b5b4b3b2b1>.
      After transformation, it will be <bpb7b6b5b4b3b2b1>.
   2- refer to NTT Specification, Edition 5
   \endverbatim
*/
static IFX_void_t cidfskntt_setparity (IFX_uint8_t *byte)
{
   IFX_uint8_t i, ones_num = 0, tmpByte = *byte;

   /* check even parity for b7...b1 */
   for (i = 0; i < 7; i++)
   {
      /* count number of ones */
      if (tmpByte & 0x1)
         ones_num ++;
      tmpByte >>= 1;
   }
   /* set parity bit as MSB in case of odd num (i.e. lowest bit is set) */
   if (ones_num & 0x1)
      *byte |= 0x80;
}


/**
   Calculate CTC for NTT FSK encoding.

   \param  pCidBuf      CID data buffer.
   \param  len          Buffer size in octets.

   \return  calculated CRC

   \remarks
   \verbatim
   1- The CRC16 for NTT FSK Buffer is generated with the polynom
      X^16 + X^12 + X^5 + X^2 + X^0 (Hexadecimal: 0x8408)
   2- The algorithm is:
      a)initial value of CRC = 0x0000.
      b)for each byte (from header to ETX)
            compute the CRC with G(x) (CRC1)
            CRC= CRC + CRC1
   \endverbatim
*/
static IFX_uint16_t cidfskntt_calccrc (IFX_uint8_t const *pCidBuf,
                                          IFX_uint16_t len)
{
   IFX_uint16_t crc = 0, tmp;
   IFX_uint16_t i;
   IFX_uint8_t j, byte;

   for (i = 0; i < len; i++)
   {
      /* get reversed data byte */
      byte = pCidBuf [i];
      tmp   = (byte << 1);
      for (j = 8; j > 0; j--)
      {
         tmp  >>= 1;
         if ((tmp ^ crc) & 0x0001)
            crc = (crc >> 1) ^ 0x8408;
         else
            crc >>= 1;
      }
   }
   return crc;
}


/**
   NTT FSK encoding.

   \param  pTxData      Pointer to TAPI_CID_DATA structure.
   \param  txMode       TX mode (on - or off hook).
   \param  nMsgElements Number of message elements to add.
   \param  pMessage     Pointer to message elements.

   \return
   - TAPI_statusOk - success
   - TAPI_statusCIDBuffNoSpaceForCLI - not enough space in buffer for CLI element
   - TAPI_statusCIDBuffNoSpaceForNAME - not enough space in buffer for NAME element
   - TAPI_statusCIDBuffMandatoryElementMissing - No valid data for NTT CID, mandatory elements missing

   \remarks
   \verbatim
   - This specification complies with NTT standards
   - The structure of the frame is as follows:
   <DLE>
   <SOH>
   <header>
   <DLE>
   <STX>
   <type of message (0x40 for type 1, 0x41 for type 2, without their parity bit)>
   <length of message>
   <type of DATA>
   <length of DATA>
   <DATA>
   <DLE>
   <ETX>
   <CRC High reversed>
   <CRC Low  reversed>
   \endverbatim
*/
static IFX_int32_t  cidfskntt_code (TAPI_CIDTX_DATA_t *pTxData,
                                    IFX_TAPI_CID_HOOK_MODE_t txMode,
                                    IFX_uint32_t nMsgElements,
                                    IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage)
{
   static const IFX_uint8_t
      pNttHead [7] = {CID_NTT_DLE, CID_NTT_SOH, CID_NTT_HEADER,
                      CID_NTT_DLE, CID_NTT_STX, CID_NTT_TYPE1, 0/*len*/ },
      pNttTail [2] = {CID_NTT_DLE, CID_NTT_ETX};
   IFX_uint16_t   crc;
   IFX_uint8_t    *pCidBuf = pTxData->cidBuf[IFX_TAPI_CID_GEN_TYPE_FSK].pBuf;
   IFX_uint16_t   *pPos    = &pTxData->cidBuf[IFX_TAPI_CID_GEN_TYPE_FSK].nLen;
   IFX_uint16_t    i;
   IFX_boolean_t  mandatory_found = IFX_FALSE;
   IFX_TAPI_CID_MSG_ELEMENT_t const *pMsg;

   /* set head according to CID type */
   *pPos  = sizeof (pNttHead);
   memcpy (pCidBuf, pNttHead, sizeof (pNttHead));
   /* in case of type 2, overwrite type parameter */
   if (txMode == IFX_TAPI_CID_HM_OFFHOOK)
   {
      pCidBuf [CID_NTT_TYPE_POS] = CID_NTT_TYPE2;
   }

   /* search for CLI data */
   for (pMsg = pMessage; pMsg < pMessage + nMsgElements; pMsg++)
   {
      if (pMsg->string.elementType == IFX_TAPI_CID_ST_CLI)
      {
         IFX_uint32_t len = pMsg->string.len;
         LIMIT_VAL(len, CID_NTT_NUMBER_MAXLEN);
         if (*pPos + len > IFX_TAPI_CID_TX_SIZE_MAX)
         {
            /* errmsg: Not enough space in buffer for CLI element */
            return TAPI_statusCIDBuffNoSpaceForCLI;
         }
         cidfskntt_codetype (CID_NTT_PT_NUMBER, pMsg->string.element,
                             len, pCidBuf, pPos);
         mandatory_found = IFX_TRUE;
         break;
      }
   }

   /* search for reason for not informing of the originating telephon number */
   for (pMsg = pMessage; pMsg < pMessage + nMsgElements; pMsg++)
   {
      if (pMsg->value.elementType == IFX_TAPI_CID_ST_ABSCLI)
      {
         IFX_uint8_t message;

         /* Translate enum to message element */
         switch (pMsg->value.element)
         {
         case IFX_TAPI_CID_NTT_ABSREASON_PRIV:
            /* Private */
            message = 'P';
            break;
         case IFX_TAPI_CID_NTT_ABSREASON_UNAV:
            /* Unable to provice service */
            message = 'O';
            break;
         case IFX_TAPI_CID_NTT_ABSREASON_PUBPHONE:
            /* Public telephone originated */
            message = 'C';
            break;
         case IFX_TAPI_CID_NTT_ABSREASON_CONFLICT:
         default:
            /* Service conflict */
            message = 'S';
            break;
         }

         cidfskntt_codetype (CID_NTT_PT_ABSENCE, &message, 1, pCidBuf, pPos);
         mandatory_found = IFX_TRUE;
         break;
      }
   }

   /* search for NAME data */
   for (pMsg = pMessage; pMsg < pMessage + nMsgElements; pMsg++)
   {
      if (pMsg->string.elementType == IFX_TAPI_CID_ST_NAME)
      {
         IFX_uint32_t len = pMsg->string.len;
         LIMIT_VAL(len, CID_NTT_NAME_MAXLEN);
         if (*pPos + len > IFX_TAPI_CID_TX_SIZE_MAX)
         {
            /* errmsg: Not enough space in buffer for NAME element */
            return TAPI_statusCIDBuffNoSpaceForNAME;
         }
         cidfskntt_codetype (CID_NTT_PT_NAME, pMsg->string.element,
                             len, pCidBuf, pPos);
         mandatory_found = IFX_TRUE;
         break;
      }
   }
   if (mandatory_found != IFX_TRUE)
   {
      /* errmsg: No valid data for NTT CID, mandatory elements missing*/
      return TAPI_statusCIDBuffMandatoryElementMissing;
   }

   /* do DLE escaping */
   cidfskntt_dle_escape (pCidBuf, pPos);

   /* set tail */
   memcpy (&pCidBuf[*pPos], pNttTail, sizeof (pNttTail));
   *pPos  += sizeof (pNttTail);
   /* set bytes parity */
   for (i = 0; i < *pPos; i++)
   {
      cidfskntt_setparity (&pCidBuf [i]);
   }
   /* calculate crc from HEADER to ETX */
   crc = cidfskntt_calccrc (&pCidBuf[2], (*pPos - 2));

   /* add CRC to buffer */
   pCidBuf [(*pPos)++] = (crc & 0xFF);
   pCidBuf [(*pPos)++] = ((crc >> 8) & 0xFF);

   return TAPI_statusOk;
}

/**
   Looks up if there is CID transparent data to transmit and copies
   this data into internal buffer.

   \param  pTxData      Pointer to TAPI_CIDTX_DATA_t structure.
   \param  pConfData    Pointer to TAPI_CID_CONF structure.
   \param  nMsgElements Number of message elements.
   \param  pMessage     Pointer to IFX_TAPI_CID_MSG_ELEMENT_t structure.

   \return
   TAPI_statusOk - success
   TAPI_statusErr - to do: replace with specific message in ocde below
   TAPI_statusCIDBuffIncorrectTRANSPARENT - Wrong size for TRANSPARENT element
                    or TRANSPARENT together with other elements types

   \remarks
   An error is returned in following cases:
   -  apart from transparent data, there are other CID elements
   -  no memory could be allocated for the user data.
*/
static IFX_int32_t cid_lookup_transparent(TAPI_CIDTX_DATA_t *pTxData,
                                          IFX_TAPI_CID_CONF_t *pConfData,
                                          IFX_uint32_t   nMsgElements,
                                          IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage)
{
   IFX_int32_t    ret = TAPI_statusOk;
   IFX_boolean_t  transparent_found = IFX_FALSE;
   IFX_uint32_t   i;

   for (i=0; i < nMsgElements; i++)
   {
      if (pMessage[i].transparent.elementType == IFX_TAPI_CID_ST_TRANSPARENT)
      {
         /* transparent message must be first in the queue, and complete because
            no encoding is done. Size must also match the internal buffer size,
            otherwise error */
         if ((i == 0) &&
             (pMessage[i].transparent.len < IFX_TAPI_CID_TX_SIZE_MAX))
         {
            transparent_found = IFX_TRUE;
         }
         else
         {
            /* errmsg: Wrong size for TRANSPARENT element or
                       TRANSPARENT together with other elements types */
            ret = TAPI_statusCIDBuffIncorrectTRANSPARENT;
            TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("\nDRV_ERROR: Transparent data"
                   " mixed together with other elements types!\n"));
         }
      }
   }
   if (transparent_found == IFX_FALSE)
   {
      return ret;
   }

   switch (pTxData->cidGenType)
   {
      case IFX_TAPI_CID_GEN_TYPE_FSK:
         memcpy (pTxData->cidBuf[IFX_TAPI_CID_GEN_TYPE_FSK].pBuf,
            pMessage[0].transparent.data, pMessage[0].transparent.len);
         pTxData->cidBuf[IFX_TAPI_CID_GEN_TYPE_FSK].nLen =
            pMessage[0].transparent.len;

         cidfsk_set_tx_time (pTxData, &pConfData->TapiCidFskConf);

      case IFX_TAPI_CID_GEN_TYPE_DTMF:
         memcpy (pTxData->cidBuf[IFX_TAPI_CID_GEN_TYPE_DTMF].pBuf,
            pMessage[0].transparent.data, pMessage[0].transparent.len);
         pTxData->cidBuf[IFX_TAPI_CID_GEN_TYPE_DTMF].nLen =
            pMessage[0].transparent.len;

         pTxData->cidBuf[IFX_TAPI_CID_GEN_TYPE_DTMF].nTxTime =
            ((pConfData->TapiCidDtmfConf.digitTime +
               pConfData->TapiCidDtmfConf.interDigitTime) *
              pTxData->cidBuf[IFX_TAPI_CID_GEN_TYPE_DTMF].nLen) -
             pConfData->TapiCidDtmfConf.interDigitTime;
         break;
      default:
         break;
   }

   return ret;
}


/**
   Prepare the CID data buffer.

   \param  pChannel     Pointer to TAPI_CHANNEL structure.
   \param  pCidInfo     Contains the caller id settings (IFX_TAPI_CID_MSG_t).

   \return
   - TAPI_statusOk  Success
   - TAPI_statusParam  Wrong or not initialized parameter
   - TAPI_statusNoMem  Could not allocate required memory

   \remarks
   - operation is done on a data channel.
   - if an error is returned the CID buffer contains invalid data!
*/
static IFX_int32_t cid_prepare_data(TAPI_CHANNEL *pChannel,
                                    IFX_TAPI_CID_MSG_t const *pCidInfo)
{
   IFX_int32_t                   ret = TAPI_statusOk;
   IFX_uint32_t                  size;
   IFX_TAPI_CID_MSG_ELEMENT_t    *pMessage = IFX_NULL;
   IFX_TAPI_CID_CONF_t         *pConfData = &pChannel->pTapiCidData->ConfData;
   TAPI_CIDTX_DATA_t             *pTxData  = &pChannel->pTapiCidData->TxData;

   if (pCidInfo->nMsgElements == 0)
   {
      RETURN_STATUS (TAPI_statusParam, 0);
   }

   size = pCidInfo->nMsgElements * sizeof(IFX_TAPI_CID_MSG_ELEMENT_t);
   pMessage = TAPI_OS_Malloc (size);
   if (pMessage == IFX_NULL)
   {
      RETURN_STATUS (TAPI_statusNoMem, 0);
   }
   else
   {
      TAPI_OS_CpyUsr2Kern (pMessage, pCidInfo->message, size);
   }

   /* reset internal CID data structure */
   memset (&pTxData->cidBuf, 0, sizeof (pTxData->cidBuf));

   pTxData->txHookMode = pCidInfo->txMode;

   /* Determine CID transmission data format */
   switch (pConfData->nStandard)
   {
      case IFX_TAPI_CID_STD_TDC:
         /*lint -fallthrough*/
      case IFX_TAPI_CID_STD_ETSI_DTMF:
         pTxData->cidGenType = IFX_TAPI_CID_GEN_TYPE_DTMF;
         break;
      case IFX_TAPI_CID_STD_KPN_DTMF:
         /*lint -fallthrough*/
      case IFX_TAPI_CID_STD_KPN_DTMF_FSK:
         if (IFX_TAPI_CID_HM_ONHOOK == pTxData->txHookMode)
         {
            /* first data sending in KPN_DTMF_FSK should be DTMF */
            pTxData->cidGenType = IFX_TAPI_CID_GEN_TYPE_DTMF;
            break;
         }
         /* KPN CID Type 2 (off-hook) always using FSK */
         /*lint -fallthrough*/
      default:
         pTxData->cidGenType = IFX_TAPI_CID_GEN_TYPE_FSK;
         break;
   }
   ret = cid_lookup_transparent(pTxData, pConfData,
                                pCidInfo->nMsgElements, pMessage);

   /* In case there is no transparent CID data, look for CID message elements
      and code them accordingly */
   if ((TAPI_SUCCESS (ret)) &&
       (pTxData->cidBuf[pTxData->cidGenType].nLen == 0))
   {
      switch (pTxData->cidGenType)
      {
         case IFX_TAPI_CID_GEN_TYPE_DTMF:
            /* Code CID for DTMF */
            ret = ciddtmf_code(pTxData,
                               &pConfData->TapiCidDtmfConf,
                               &pConfData->TapiCidDtmfAbsCli,
                               pCidInfo->nMsgElements, pMessage);

            /* continue data preparation for FSK */
            if (IFX_TAPI_CID_STD_KPN_DTMF_FSK != pConfData->nStandard)
               break;

            /*lint -fallthrough*/
         case IFX_TAPI_CID_GEN_TYPE_FSK:

            if (IFX_TAPI_CID_STD_NTT == pConfData->nStandard)
            {
               /* Code CID for NTT (V.23)*/
               ret = cidfskntt_code(pTxData, pCidInfo->txMode,
                                    pCidInfo->nMsgElements, pMessage);
            }
            else
            {
               /* Code CID according to standard (V.23 or Bellcore) */
               ret = cidfsk_code(pTxData, pCidInfo->messageType,
                                 pCidInfo->nMsgElements, pMessage);
            }

            if (TAPI_SUCCESS (ret))
            {
               cidfsk_set_tx_time (pTxData, &pConfData->TapiCidFskConf);
            }

            break;
         default:
            break;
      }
   }

   TAPI_OS_Free (pMessage);

   RETURN_STATUS(ret, 0);
}


/**
   Determine alert type from configuration and current type of cid to send.

   \param  pChannel     Pointer to TAPI_CHANNEL structure.
   \param  pCidInfo     Contains the caller id settings (IFX_TAPI_CID_MSG_t).

   \return
   - TAPI_statusOk  Success
   - TAPI_statusParam  Wrong or not initialized parameter

   \remarks
   - operation is done on a data channel.
   - see tables in TAPI specification / documentation
*/
static IFX_int32_t cid_determine_alert_type(TAPI_CHANNEL *pChannel,
                                            IFX_TAPI_CID_MSG_t const *pCidInfo)
{
   IFX_TAPI_CID_CONF_t      *pConfData   = &pChannel->pTapiCidData->ConfData;
   IFX_TAPI_CID_TIMING_t      *pCidTiming  = &pConfData->TapiCidTiming;
   TAPI_CIDTX_DATA_t          *pTxData     = &pChannel->pTapiCidData->TxData;
   IFX_TAPI_CID_MSG_TYPE_t    messageType = pCidInfo->messageType;
   IFX_TAPI_CID_HOOK_MODE_t   txMode      = pCidInfo->txMode;
   IFX_TAPI_CID_ALERT_TYPE_t  nAlertType  = TAPI_CID_ALERT_NONE;
   IFX_int32_t                ret         = TAPI_statusOk;
   IFX_uint32_t               nTimeCidTxSeq;

   switch (pConfData->nStandard)
   {
   case IFX_TAPI_CID_STD_TELCORDIA:
      if (txMode == IFX_TAPI_CID_HM_OFFHOOK)
         nAlertType = TAPI_CID_ALERT_CAS;
      else
      {
         if (messageType == IFX_TAPI_CID_MT_CSUP)
            nAlertType = TAPI_CID_ALERT_FR;
         else
            nAlertType = TAPI_CID_ALERT_OSI;
      }
      break;
   case IFX_TAPI_CID_STD_ETSI_FSK:
   case IFX_TAPI_CID_STD_ETSI_DTMF:
      if (txMode == IFX_TAPI_CID_HM_OFFHOOK)
         nAlertType = TAPI_CID_ALERT_DTAS;
      else
      {
         IFX_TAPI_CID_ALERT_ETSI_t etsi_alert;
         /* get the right ETSI alert (ringing or not!) */
         if (messageType == IFX_TAPI_CID_MT_CSUP)
            etsi_alert = pConfData->nETSIAlertRing;
         else
            etsi_alert = pConfData->nETSIAlertNoRing;
         /* map the ETSI-only enum to internal overall-enum */
         switch (etsi_alert)
         {
         case IFX_TAPI_CID_ALERT_ETSI_FR:
            nAlertType = TAPI_CID_ALERT_FR;
            break;
         case IFX_TAPI_CID_ALERT_ETSI_DTAS:
            nAlertType = TAPI_CID_ALERT_DTAS;
            break;
         case IFX_TAPI_CID_ALERT_ETSI_RP:
            nAlertType = TAPI_CID_ALERT_RP;
            break;
         case IFX_TAPI_CID_ALERT_ETSI_LRDTAS:
            nAlertType = TAPI_CID_ALERT_LR_DTAS;
            break;
         }
      }
      break;
   case IFX_TAPI_CID_STD_SIN:
      if (txMode == IFX_TAPI_CID_HM_OFFHOOK)
         nAlertType = TAPI_CID_ALERT_DTAS;
      else
         nAlertType = TAPI_CID_ALERT_LR_DTAS;
      break;
   case IFX_TAPI_CID_STD_NTT:
      if (txMode == IFX_TAPI_CID_HM_OFFHOOK)
         nAlertType = TAPI_CID_ALERT_AS_NTT;
      else
         nAlertType = TAPI_CID_ALERT_CAR_NTT;
      break;
   case IFX_TAPI_CID_STD_KPN_DTMF:
   case IFX_TAPI_CID_STD_KPN_DTMF_FSK:
      if (txMode == IFX_TAPI_CID_HM_OFFHOOK)
         nAlertType = TAPI_CID_ALERT_DTAS;
      else
         nAlertType = TAPI_CID_ALERT_LR;
      break;
   case IFX_TAPI_CID_STD_TDC:
      /* none */
      break;
   }

   /* For ALERT_FR check if the configured ringpause time is sufficient for
      data transmission and also correct waiting time for pause generation.
      NOTE:
         KPN_DTMF_FSK standard using FR alert before second CID transmission
   */
   if ((TAPI_CID_ALERT_FR == nAlertType) ||
       ((IFX_TAPI_CID_STD_KPN_DTMF_FSK == pConfData->nStandard) &&
        (txMode == IFX_TAPI_CID_HM_ONHOOK)))
   {
      IFX_uint8_t cidGenType = pTxData->cidGenType;

      if (IFX_TAPI_CID_STD_KPN_DTMF_FSK == pConfData->nStandard)
      {
         /* should be updated FSK data, because second CID
            transmission phase using FSK */
         cidGenType = IFX_TAPI_CID_GEN_TYPE_FSK;
      }

      TAPI_ASSERT (cidGenType < IFX_TAPI_CID_GEN_TYPE_MAX);

      /* calculate complete sequence time */
      nTimeCidTxSeq = pCidTiming->afterFirstRing +
                      pTxData->cidBuf[cidGenType].nTxTime +
                      pCidTiming->dataOut2restoreTimeOnhook;
      /* check if ring cadence pause is enough for cid sequence */
      if (pConfData->cadenceRingPause >= nTimeCidTxSeq)
      {
         /* Increase the _individual_ CID transmission time (nTxTime
            for this CID transmission) in order to ensure the proper
            ring pause timing.

            +--1--+----2----+----3----+--------X------+
            |     |         |         |               |
            +-------pConfData->cadenceRingPause--------+
            |     |                         |         |
            +--1--+---- 2' -----------------+----3----+

            1)  pCidTiming->afterFirstRing
            2)  pTxData->nTxTime
            3)  pCidTiming->dataOut2restoreTimeOnhook
            X)  pConfData->cadenceRingPause

            so the effective cid transmission time will
            be increased to
            2') pTxData->nTxTime += X
         */
#if 0 // ctc merged
         /* NOTE: now cadenceRingPause contain ring pause time
            without cid transmission time */
         pConfData->cadenceRingPause -= nTimeCidTxSeq;
         pTxData->cidBuf[cidGenType].nTxTime += pConfData->cadenceRingPause;
#else
         pTxData->cidBuf[cidGenType].nTxTime += 220;
#endif
      }
      else
      {
         /* configured cadence pause time not sufficient for CID transmission */
#if 0 // ctc merged
         IFX_TAPI_EVENT_t tapiEvent;
#endif

         TRACE (TAPI_DRV,DBG_LEVEL_HIGH,
                ("Cadence pause of %d ms not sufficient for CID transmission "
                 "of %d ms duration.\n",
                 pConfData->cadenceRingPause, nTimeCidTxSeq));
#if 0 // ctc merged, Quantum marked out: should not generate this event would caused system failed and no way to recovery

         memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
         tapiEvent.id = IFX_TAPI_EVENT_CID_TX_RINGCAD_ERR;
         IFX_TAPI_Event_Dispatch(pChannel,&tapiEvent);
#endif
#if 1 // ctc merged, Quantum Added should change to ALERT_RP otherwise there would be no enough time to send CID
         nAlertType = TAPI_CID_ALERT_RP;
#else
         ret = TAPI_statusParam;
#endif
      }
   }

   pTxData->nAlertType = nAlertType;
   /* For self generated alert ringing sequence (NTT CAR) */
   pTxData->bAlertRingBurstState = IFX_FALSE;

   /* determine the time for playing alert tones */
   if (TAPI_SUCCESS (ret))
      cid_set_alerttone_time (pChannel, pCidInfo);

   /* Reset flags indicating to ignore hook events due to OSI */
   pTxData->bOSIinducedOffhook = IFX_FALSE;
   pTxData->bOSIinducedOnhook = IFX_FALSE;

   RETURN_STATUS(ret, 0);
}


/**
   Sets predifined alert tones for following CID alerts :
      - TAPI_CID_ALERT_CAS
      - TAPI_CID_ALERT_DTAS
      - TAPI_CID_ALERT_LR_DTAS
      - TAPI_CID_ALERT_AS_NTT

   \return
   - TAPI_statusOk
   - TAPI_statusCIDConfAlertToneFailed

   \remarks
   - For Caller ID alert Signal, predifined tones are defined accoding to
     indexes defined in enum _TAPI_CID_ALERTTONE_INDEX_DEFAULT.
     Theses indexes will be used as default for CID according to standard in
     case the user has not defined a suitable tone for his application.
*/
IFX_int32_t IFX_TAPI_CID_SetPredefAlertTones (IFX_void_t)
{
   IFX_TAPI_TONE_SIMPLE_t a_ptone;
   IFX_int32_t ret;

   /* reset tone structure */
   memset (&a_ptone, 0, sizeof (a_ptone));
   /* set parameters for CID alert tone onhook */
   a_ptone.index = TAPI_CID_ATONE_INDEXDEF_ONHOOK;
   a_ptone.frequencies [0] = (IFX_TAPI_TONE_FREQA | IFX_TAPI_TONE_FREQB);
   /* Frequencies for dual alert tone :
      f1 = 2130 Hz, f2 = 2750 Hz */
   a_ptone.freqA       = 2130;
   a_ptone.freqB       = 2750;
   /* For onhook, alert tone must be played at least 100 ms */
   a_ptone.cadence [0] = 100;
   a_ptone.cadence [1] = 0;
   /* Level for dual alert tone : -18 dB = -180 x 0.1 dB
      Reference : TIA  is  -22dB (dynamic range is -14 dB to -34 dB)
                  ETSI is -18dB */
   a_ptone.levelA      = -180;
   a_ptone.levelB      = -180;
   /* tone played only once */
   a_ptone.loop        = 1;
   ret = TAPI_Phone_Add_SimpleTone (&a_ptone);

   /* set parameters for CID alert tone offhook,
      same as for onhook except timing */
   if (ret == TAPI_statusOk)
   {
      a_ptone.index = TAPI_CID_ATONE_INDEXDEF_OFFHOOK;
      /* For offhook, alert tone must be played at least 80 ms */
      a_ptone.cadence [0] = 80;
      ret = TAPI_Phone_Add_SimpleTone (&a_ptone);
   }
   /* set parameters for CID alert tone AS NTT, cadence is :
      <f1_1/f2, a ms><no freq, b ms><f1_2/f2, c ms> whereby :
      f1_1 = 852 Hz
      f1_2 = 1633 Hz
      f2   = 941 Hz
      60 ms <= a < 120 ms
      40 ms <= b < 70 ms
      60 ms <= c < 120 ms
   */
   if (TAPI_SUCCESS (ret))
   {
      memset (&a_ptone, 0, sizeof (a_ptone));
      a_ptone.index = TAPI_CID_ATONE_INDEXDEF_OFFHOOKNTT;
      /* AS NTT tone freqs : f1_1 = 852 Hz, f1_2 = 941 Hz, f2 = 1633 Hz */
      a_ptone.freqA = 852;
      a_ptone.freqB = 1633;
      a_ptone.freqC = 941;
      /* AS NTT tone levels : -15 dB = -150 x 0.1 dB
         Lower frequency  (f1_x) : -15.5 - 0.8 L <= P <= -4.0 - 0.8 L
         Higher frequency (f2)   : -14.5 - L     <= P <= -3.0 - L
         Note : L is the subscriber line load for 1500 Hz. Nominal value 0 - 7 dB
                But there are cases of having more than 7 dB.
      */
      a_ptone.levelA = -150;
      a_ptone.levelB = -150;
      a_ptone.levelC = -150;
      /* AS NTT tone times : a = 100ms, b = 50ms, c = 100 ms */
      a_ptone.cadence [0] = 100;
      a_ptone.cadence [1] = 50;
      a_ptone.cadence [2] = 100;
      a_ptone.loop        = 1;
      /* AS NTT tone cadence */
      a_ptone.frequencies [0] = (IFX_TAPI_TONE_FREQA | IFX_TAPI_TONE_FREQB);
      a_ptone.frequencies [1] = 0;
      a_ptone.frequencies [2] = (IFX_TAPI_TONE_FREQC | IFX_TAPI_TONE_FREQB);
      ret = TAPI_Phone_Add_SimpleTone (&a_ptone);
   }

   if (!TAPI_SUCCESS(ret))
   {
      /* errmsg: Configuration of predefined CID alert tone failed */
      return TAPI_statusCIDConfAlertToneFailed;
   }

   return TAPI_statusOk;
}


/**
   Sets alert tone time according to transmission mode.

   \param  pChannel     Pointer to TAPI_CHANNEL structure, data channel.
   \param  pCidInfo     Contains the caller id settings (IFX_TAPI_CID_MSG_t).
*/
static IFX_void_t cid_set_alerttone_time (TAPI_CHANNEL *pChannel,
                                          IFX_TAPI_CID_MSG_t const *pCidInfo)
{
   IFX_TAPI_CID_CONF_t  *pConfData   = &pChannel->pTapiCidData->ConfData;

   switch (pCidInfo->txMode)
   {
      case IFX_TAPI_CID_HM_ONHOOK:
         pConfData->nAlertToneTime =
            IFX_TAPI_Tone_DurationGet(pConfData->nAlertToneOnhook);
         break;
      case IFX_TAPI_CID_HM_OFFHOOK:
         pConfData->nAlertToneTime =
            IFX_TAPI_Tone_DurationGet(pConfData->nAlertToneOffhook);
         break;
   }
}


/**
   Mutes/Restores Voice talking and listening path during off hook CID processing.

   \param  pChannel     Pointer to TAPI_CHANNEL structure, data channel.
   \param  bMute        IFX_TRUE = mute voice / IFX_FALSE = restore voice

   \return
   - TAPI_statusOk
   - TAPI_statusErr

   \remarks
   - Operation is done on a data channel.
   - When LL driver does not provide the functionality no error is raised.
   - The actual LL error is of no interest here and not logged.
*/
static IFX_int32_t cid_mutevoice (TAPI_CHANNEL *pChannel,
                                     IFX_boolean_t bMute)
{
   IFX_TAPI_DRV_CTX_t *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   IFX_int32_t retLL = TAPI_statusOk;

   if (IFX_TAPI_PtrChk (pDrvCtx->CON.Data_Channel_Mute))
   {
      retLL = pDrvCtx->CON.Data_Channel_Mute(pChannel->pLLChannel, bMute);
   }

   return TAPI_SUCCESS(retLL) ? TAPI_statusOk : TAPI_statusErr;
}

#ifdef TAPI_HAVE_TIMERS
/**
   State machine to handle the alert phase of the caller id sequence.

   \param  pChannel     Pointer to TAPI_CHANNEL structure.

   \return
   FSM_STATUS_t

   \remarks
   - operation is done on a data channel.
   - This function is called from the higher level fsm and assumes that the
   data_lock is taken and the initial line mode was saved.
*/
static FSM_STATUS_t cid_fsm_alert_exec(TAPI_CHANNEL *pChannel)
{
   IFX_TAPI_DRV_CTX_t  *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   IFX_TAPI_CID_CONF_t *pConfData = &pChannel->pTapiCidData->ConfData;
   TAPI_CIDTX_DATA_t *pTxData   = &pChannel->pTapiCidData->TxData;
   TAPI_CHANNEL *pPhoneCh       = pTxData->pPhoneCh;
   int channel = pChannel->nChannel+pChannel->pTapiDevice->nDevID*2;
   IFX_TAPI_TONE_PLAY_t tone;
   IFX_uint32_t   nTime       = 0;
   FSM_STATUS_t   ret         = E_FSM_CONTINUE;

   memset (&tone, 0, sizeof(tone));
   tone.internal = IFX_TRUE;
   if ((pDrvCtx->ALM.TG_Play != IFX_NULL) &&
       (pDrvCtx->SIG.UTG_Start == IFX_NULL))
      tone.module = IFX_TAPI_MODULE_TYPE_ALM;
   else
      tone.module = IFX_TAPI_MODULE_TYPE_COD;

   switch (pTxData->nAlertType)
   {
   case TAPI_CID_ALERT_NONE:
      nTime = MIN_TIMER_VAL;
      ret   = E_FSM_COMPLETE;
      break;
   case TAPI_CID_ALERT_OSI:
      if (pTxData->nCidSubState==0)
      {
         pTxData->bOSIinducedOnhook = IFX_TRUE;
         if (TAPI_statusOk != TAPI_Phone_Set_Linefeed (pPhoneCh,
                                                IFX_TAPI_LINE_FEED_DISABLED))
         {
            ret = E_FSM_ERROR;
            break;
         }
         nTime = pConfData->OSItime;
      }
      else
      {
         /* previous mode was high impedance mode, in which case the line
            was disabled. So set line back to standby before setting the last
            line mode. */
         pTxData->bOSIinducedOffhook = IFX_TRUE;
         if (TAPI_statusOk != TAPI_Phone_Set_Linefeed (pPhoneCh,
                                                IFX_TAPI_LINE_FEED_STANDBY))
         {
            ret = E_FSM_ERROR;
            break;
         }
         /* Because the DCCTRL needs time to set the linemode wait at least
            0.5 milliseconds before going from standby to active. This can
            be removed as soon as the DCCTRL changes the behaviour. */
         TAPI_OS_MSecSleep (1);
         if (TAPI_statusOk != TAPI_Phone_Set_Linefeed (pPhoneCh,
                                                IFX_TAPI_LINE_FEED_ACTIVE))
         {
            ret = E_FSM_ERROR;
            break;
         }
         nTime = pConfData->TapiCidTiming.afterOSI;
         ret = E_FSM_COMPLETE;
      }
      break;
   case TAPI_CID_ALERT_FR:
      if (pTxData->nCidSubState==0)
      {
         /* start ringing on the phone channel */
#if 0 // ctc merged
         IFX_TAPI_Ring_Engine_Start(pTxData->pPhoneCh, IFX_TRUE);
#else
         TAPI_Phone_Set_Linefeed (pPhoneCh, IFX_TAPI_LINE_FEED_RING_BURST);
#endif
         /* because we start ringing here there is no need to start it later */
#if 0 // ctc merged
//QTMW:Should not use Original implementation for T3.6
         pTxData->bRingStart = IFX_FALSE;
#endif
         /* Acutally we do not need the timer but wait that ringing calls
            TAPI_Phone_OnRingpause(). This way we get a better synchronised
            timing with ringing. Nevertheless we use the timer as a
            backup here. We set it to the times of the ring burst plus some
            safety to make sure that it does not hit before the calculated
            time when we expect the callback. */
#if 0 // ctc merged
         nTime = pConfData->cadenceRingBurst + 100;
#else
            nTime = 415;	//265;	//465;//225;	//465;
            if (channel==1) nTime = 495;
#endif
      }
      else
      {
         if (TAPI_statusOk != TAPI_Phone_Set_Linefeed (pPhoneCh,
                                                IFX_TAPI_LINE_FEED_ACTIVE))
         {
            ret = E_FSM_ERROR;
            break;
         }
         nTime = pConfData->TapiCidTiming.afterFirstRing;
         ret = E_FSM_COMPLETE;
      }
      break;
   case TAPI_CID_ALERT_RP:
      if ((pTxData->nCidSubState==0) &&
          (pConfData->ringPulseOnTime > 0))
      {
         if (TAPI_statusOk != TAPI_Phone_Set_Linefeed (pPhoneCh,
                                                IFX_TAPI_LINE_FEED_RING_BURST))
         {
            ret = E_FSM_ERROR;
            break;
         }
         nTime = pConfData->ringPulseOnTime;
      }
      else
      {
         if (TAPI_statusOk != TAPI_Phone_Set_Linefeed (pPhoneCh,
                                                IFX_TAPI_LINE_FEED_ACTIVE))
         {
            ret = E_FSM_ERROR;
            break;
         }
         nTime = pConfData->TapiCidTiming.afterRingPulse;
         ret = E_FSM_COMPLETE;
      }
      break;
   case TAPI_CID_ALERT_CAS:
      /* The Bellcore offhook sequence can include an SAS tone and also an
         optional OSI interval before and after. If parts are not applicable
         we fallthrough to the next sub-state until there is work to do. */
      switch (pTxData->nCidSubState)
      {
      case 0:
         /* mute voice path */
         if (cid_mutevoice(pChannel, IFX_TRUE) == TAPI_statusOk)
            pTxData->bMute = IFX_TRUE;

         if (pConfData->OSIoffhook && pConfData->nSAStone)
         {
            /* start OSI */
            pTxData->bOSIinducedOnhook = IFX_TRUE;
            TAPI_Phone_Set_Linefeed (pPhoneCh, IFX_TAPI_LINE_FEED_DISABLED);
            nTime = pConfData->OSItime;
            break;
         }
         pTxData->nCidSubState++;
         /*lint -fallthrough*/
      case 1:
         if (pConfData->OSIoffhook && pConfData->nSAStone)
         {
            /* end OSI */
            pTxData->bOSIinducedOffhook = IFX_TRUE;
            TAPI_Phone_Set_Linefeed (pPhoneCh, IFX_TAPI_LINE_FEED_STANDBY);
            TAPI_Phone_Set_Linefeed (pPhoneCh, IFX_TAPI_LINE_FEED_ACTIVE);
         }

         if (pConfData->nSAStone)
         {
            nTime = pConfData->beforeSAStime;
            break;
         }
         pTxData->nCidSubState++;
         /*lint -fallthrough*/
      case 2:
         if (pConfData->nSAStone)
         {
            /* Play SAS tone on data channel */
            tone.index = pConfData->nSAStone;
            if (TAPI_Phone_Tone_Play_Unprot (pChannel, &tone,
                IFX_FALSE /* do not send TONE_END event */) != TAPI_statusOk)
            {
               ret = E_FSM_ERROR;
               break;
            }

            /* Set time to wait as SAS tone duration + pause */
            nTime = pConfData->nSasToneTime + pConfData->SAS2CAStime;
            break;
         }
         pTxData->nCidSubState++;
         /*lint -fallthrough*/
      default:
         /* Play CAS tone on data channel, use unprotected function, protection
            is done around cid_fsm_alert_exec */
         tone.index = pConfData->nAlertToneOffhook;
         if (TAPI_Phone_Tone_Play_Unprot (pChannel, &tone,
             IFX_FALSE /* do not send TONE_END event */) != TAPI_statusOk)
         {
            ret = E_FSM_ERROR;
            break;
         }
         /* set time to wait considering tone play time (80 ms for Offhook)*/
         nTime = pConfData->TapiCidTiming.cas2ackTime +
                 pConfData->nAlertToneTime;
         ret = E_FSM_COMPLETE;
         break;
      }
      break;
   case TAPI_CID_ALERT_DTAS:
      switch (pTxData->txHookMode)
      {
         case  IFX_TAPI_CID_HM_ONHOOK:
            /* Play Onhook DTAS tone on data channel, use unprotected function,
               protection is done around cid_fsm_alert_exec */
            tone.index = pConfData->nAlertToneOnhook;
            if (TAPI_Phone_Tone_Play_Unprot (pChannel, &tone,
                IFX_FALSE /* do not send TONE_END event */) != TAPI_statusOk)
            {
               ret = E_FSM_ERROR;
               break;
            }

            /* set time to wait considering tone play time (100 ms for Onhook) */
            nTime = pConfData->TapiCidTiming.afterDTASOnhook +
                    pConfData->nAlertToneTime;
            break;
         case IFX_TAPI_CID_HM_OFFHOOK:
            /* mute voice path */
            if (cid_mutevoice(pChannel, IFX_TRUE) == TAPI_statusOk)
               pTxData->bMute = IFX_TRUE;
            /* use unprotected function, protection is done around
               cid_fsm_alert_exec */
            tone.index = pConfData->nAlertToneOffhook;
            if (TAPI_Phone_Tone_Play_Unprot (pChannel, &tone,
                IFX_FALSE /* do not send TONE_END event */) != TAPI_statusOk)
            {
               ret = E_FSM_ERROR;
               TRACE (TAPI_DRV,DBG_LEVEL_HIGH, ("DTAS play error!\n"));
               break;
            }

            /* set time to wait considering tone play time (80 ms for Offhook). */
            nTime = pConfData->TapiCidTiming.cas2ackTime +
                    pConfData->nAlertToneTime;
            break;
      }
      ret = E_FSM_COMPLETE;
      break;
   case TAPI_CID_ALERT_LR:
      /* Start this alert with line reversal */
      TAPI_Phone_Set_Linefeed (pPhoneCh, pTxData->nLineModeReverse);
      nTime = pConfData->TapiCidTiming.afterLineReversal;
      ret   = E_FSM_COMPLETE;
      break;
   case TAPI_CID_ALERT_LR_DTAS:
      switch (pTxData->nCidSubState)
      {
      case 0:
         /* Start this alert with line reversal */
         TAPI_Phone_Set_Linefeed (pPhoneCh, pTxData->nLineModeReverse);
         nTime = pConfData->TapiCidTiming.afterLineReversal;
         break;
      case 1:
         /* Play DTAS is onhook tone, use unprotected function, protection
            is done around cid_fsm_alert_exec */
         tone.index = pConfData->nAlertToneOnhook;
         if (TAPI_Phone_Tone_Play_Unprot (pChannel, &tone,
             IFX_FALSE /* do not send TONE_END event */) != TAPI_statusOk)
         {
            ret = E_FSM_ERROR;
            break;
         }
         /* set time to wait considering tone play time (100 ms for Onhook */
         nTime = pConfData->TapiCidTiming.afterDTASOnhook +
                 pConfData->nAlertToneTime;
         break;
      default:
         /* Substate 2 is used to catch hook events caused by a possible
            wetting pulse and validate them separately. */
         /* resume asap, using the minimum timer time */
         nTime = MIN_TIMER_VAL;
         ret   = E_FSM_COMPLETE;
      }
      break;
   case TAPI_CID_ALERT_AS_NTT:
      /* mute voice path */
      if (cid_mutevoice(pChannel, IFX_TRUE) == TAPI_statusOk)
         pTxData->bMute = IFX_TRUE;
      /* Play AS_NTT tone, use unprotected function, protection
         is done around cid_fsm_alert_exec */
      tone.index = pConfData->nAlertToneOffhook;
      if (TAPI_Phone_Tone_Play_Unprot (pChannel, &tone,
          IFX_FALSE /* do not send TONE_END event */) != TAPI_statusOk)
      {
         ret = E_FSM_ERROR;
         break;
      }
      /* set time to wait, considering tone play time */
      nTime = pConfData->TapiCidTiming.beforeData +
              pConfData->nAlertToneTime;
      ret = E_FSM_COMPLETE;
      break;
   case TAPI_CID_ALERT_CAR_NTT:
      if (pTxData->nCidSubState==0)
      {
         if (TAPI_statusOk != TAPI_Phone_Set_Linefeed (pPhoneCh,
                                                pTxData->nLineModeReverse))
         {
            ret = E_FSM_ERROR;
            break;
         }
         nTime = pConfData->TapiCidTiming.afterLineReversal;
      }
      else
      {
         if (pTxData->bAlertRingBurstState == IFX_FALSE)
         {
            if (TAPI_statusOk != TAPI_Phone_Set_Linefeed (pPhoneCh,
                                             IFX_TAPI_LINE_FEED_RING_BURST))
            {
               ret = E_FSM_ERROR;
               break;
            }
            pTxData->bAlertRingBurstState = IFX_TRUE;
            nTime = pConfData->ringPulseOnTime;
         }
         else
         {
            if (TAPI_statusOk != TAPI_Phone_Set_Linefeed (pPhoneCh,
                                             pTxData->nLineModeReverse))
            {
               ret = E_FSM_ERROR;
               break;
            }
            pTxData->bAlertRingBurstState = IFX_FALSE;
            nTime = pConfData->ringPulseOffTime;
            /* for each ring pulse sequence (ringing / pause), substate is
               incremented by 2 */
            if (((pConfData->ringPulseLoop << 1) == pTxData->nCidSubState) ||
                (pTxData->bAck == IFX_TRUE))
            {
               /* resume asap, using the minimum timer time */
               nTime = MIN_TIMER_VAL;
               ret   = E_FSM_COMPLETE;
            }
         }
      }
      break;
   }

   if (ret == E_FSM_CONTINUE)
      pTxData->nCidSubState++;

   if (ret != E_FSM_ERROR)
   {
      /* in case of no error, nTime must have been set accordingly. */
      TAPI_ASSERT (nTime != 0);
      TAPI_SetTime_Timer (pTxData->CidTimerID, nTime, IFX_FALSE, IFX_FALSE);
   }

   return ret;
}


/**
   State machine to handle the ACK-phase of the caller id sequence.
   Will be called for off hook cid (not NTT) and on hook (only NTT)!

   \param  pChannel     Pointer to TAPI_CHANNEL structure.

   \return
   FSM_STATUS_t

   \remarks
   - operation is done on a data channel.
   - This function is called from the higher level fsm and assumes that the
   data_lock is taken.
*/
static FSM_STATUS_t cid_fsm_ack_exec(TAPI_CHANNEL *pChannel)
{
   FSM_STATUS_t   ret         = E_FSM_CONTINUE;
   IFX_TAPI_CID_CONF_t *pConfData = &pChannel->pTapiCidData->ConfData;
   TAPI_CIDTX_DATA_t *pTxData = &pChannel->pTapiCidData->TxData;
   IFX_uint32_t   nTime       = MIN_TIMER_VAL;

   if (pTxData->bAck == IFX_TRUE)
   {
      nTime = pConfData->TapiCidTiming.ack2dataOutTime;
      ret   = E_FSM_COMPLETE;
   }
   else
   {
      nTime = pConfData->TapiCidTiming.afterAckTimeout;
      {
         IFX_TAPI_EVENT_t tapiEvent;

         memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
         tapiEvent.id = IFX_TAPI_EVENT_CID_TX_NOACK_ERR;
         ret = IFX_TAPI_Event_Dispatch(pChannel,&tapiEvent);
         TRACE (TAPI_DRV,DBG_LEVEL_HIGH, ("no CID ack received!\n"));
      }
      ret = E_FSM_ERROR;
   }

   if (ret != E_FSM_ERROR)
   {
      TAPI_SetTime_Timer (pTxData->CidTimerID, nTime, IFX_FALSE, IFX_FALSE);
   }

   return ret;
}


/**
   State machine to handle the cid sending phase of the caller id sequence.

   \param  pChannel     Pointer to TAPI_CHANNEL structure.

   \return
   FSM_STATUS_t

   \remarks
   - operation is done on a data channel.
   - This function is called from the higher level fsm and assumes that the
   data_lock is taken and the initial line mode was saved.
*/
static FSM_STATUS_t cid_fsm_sending_exec(TAPI_CHANNEL *pChannel)
{
   IFX_TAPI_DRV_CTX_t *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   IFX_TAPI_CID_CONF_t *pConfData  = &pChannel->pTapiCidData->ConfData;
   TAPI_CIDTX_DATA_t *pTxData  = &pChannel->pTapiCidData->TxData;
   IFX_uint32_t nTime          = pTxData->cidBuf[pTxData->cidGenType].nTxTime;
   FSM_STATUS_t ret            = E_FSM_CONTINUE;

   TAPI_ASSERT(nTime != 0);

   /* For NTT on hook transmission a special handling is done here to wait for
      the second ack signal after sending the data:
       -during the first call we start transmission
       -all successive calls wait for an acknowledge signal or an timeout
      For Bellcore when using OSI in an offhook sequence we get here
      three times to end the OSI sequence.
      All other standards arrive here only once */
   if (pTxData->nCidSubState == 0)
   {
      if (IFX_TAPI_PtrChk (pDrvCtx->SIG.CID_TX_Start))
      {
         IFX_TAPI_CID_TX_t  cidTxData;
         IFX_int32_t retLL;

         memset (&cidTxData, 0, sizeof (IFX_TAPI_CID_TX_t));

         /* pass only these parameters to the LL-driver */
         cidTxData.txHookMode   = pTxData->txHookMode;
         cidTxData.pFskConf     = &pConfData->TapiCidFskConf;
         cidTxData.pDtmfConf    = &pConfData->TapiCidDtmfConf;
         /* get parameter according to the data type */
         cidTxData.pCidParam    = pTxData->cidBuf[pTxData->cidGenType].pBuf;
         cidTxData.nCidParamLen = pTxData->cidBuf[pTxData->cidGenType].nLen;

         /* Determine CID transmission data format */
         if (IFX_TAPI_CID_GEN_TYPE_DTMF == pTxData->cidGenType)
         {
            cidTxData.cidDataType = IFX_TAPI_CID_DATA_TYPE_DTMF;
         }
         else if (IFX_TAPI_CID_STD_TELCORDIA == pConfData->nStandard)
         {
            cidTxData.cidDataType = IFX_TAPI_CID_DATA_TYPE_FSK_BEL202;
         }
         else
         {
            cidTxData.cidDataType = IFX_TAPI_CID_DATA_TYPE_FSK_V23;
         }

         if ((IFX_TAPI_CID_GEN_TYPE_DTMF ==  pTxData->cidGenType) &&
             (ptr_chk(pDrvCtx->SIG.DTMFD_Override, "")))
         {
            IFX_TAPI_LL_DTMFD_OVERRIDE_t cfgDtmf;

            memset (&cfgDtmf, 0, sizeof (cfgDtmf));

            cfgDtmf.nMod = IFX_TAPI_MODULE_TYPE_COD;
            cfgDtmf.direction = IFX_TAPI_LL_DTMFD_DIR_INTERNAL;
            cfgDtmf.bOverride = IFX_TRUE;
            cfgDtmf.nOperation = IFX_DISABLE;

            /* Deactivate DTMF detector so that possible reflections of
               the sent DTMF digits are not received. */
            (IFX_void_t)pDrvCtx->SIG.DTMFD_Override(
               pChannel->pLLChannel, &cfgDtmf);
         }

         retLL = pDrvCtx->SIG.CID_TX_Start(pChannel->pLLChannel, &cidTxData);

         if (!TAPI_SUCCESS(retLL))
         {
            ret = E_FSM_ERROR;
         }
      }
      else
      {
         ret = E_FSM_ERROR;
      }

      if (ret != E_FSM_ERROR)
      {
         if ((pConfData->nStandard == IFX_TAPI_CID_STD_NTT) &&
             (pTxData->txHookMode == IFX_TAPI_CID_HM_ONHOOK) )
         {
            /* NTT onhook case */
            /* reset the flag that we are going to wait for */
            pTxData->bAck2 = IFX_FALSE;
            /* delay after which we will check the flag for the first time */
            nTime += CID_NTT_ACK2_POLLTIME;
            /* stay in this state and return to this function */
            ret = E_FSM_CONTINUE;
         }
         else
         if ((pConfData->nStandard == IFX_TAPI_CID_STD_TELCORDIA) &&
             (pTxData->txHookMode == IFX_TAPI_CID_HM_OFFHOOK) &&
             (pConfData->OSIoffhook == IFX_TRUE) &&
             (pConfData->nSAStone != 0))
         {
            /* Telcordia offhook case with OSI usage */
            /* stay in this state and return to this function */
            ret = E_FSM_CONTINUE;
         }
         else
         {
            /* all other cases (including NTT offhook) */
            if (pTxData->txHookMode == IFX_TAPI_CID_HM_OFFHOOK)
            {
               nTime += pConfData->TapiCidTiming.dataOut2restoreTimeOffhook;
            }
            else
            {
               nTime += pConfData->TapiCidTiming.dataOut2restoreTimeOnhook;
            }

            /* do not return here but go to the next state */
            ret = E_FSM_COMPLETE;
         }
      }
   }
   else
   {
      if ((pConfData->nStandard == IFX_TAPI_CID_STD_NTT) &&
          (pTxData->txHookMode == IFX_TAPI_CID_HM_ONHOOK) )
      {
         /* this case handles only NTT standard in on hook transmission mode */
         /* go on if an ack has arrived in the meantime
            or check if timeout has expired and abort the state machine */
         if (pTxData->bAck2 == IFX_TRUE)
         {
            nTime = pConfData->TapiCidTiming.dataOut2restoreTimeOnhook;
            ret = E_FSM_COMPLETE;
         }
         else if ((pTxData->nCidSubState * CID_NTT_ACK2_POLLTIME) >=
                  pConfData->ack2Timeout)
         {
            IFX_TAPI_EVENT_t tapiEvent;

            memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
            tapiEvent.id = IFX_TAPI_EVENT_CID_TX_NOACK2_ERR;
            IFX_TAPI_Event_Dispatch(pChannel,&tapiEvent);
            TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("no CID ack2 received!\n"));
            ret = E_FSM_ERROR;
         }
         else
         {
            /* do another poll cycle */
            nTime = CID_NTT_ACK2_POLLTIME;
         }
      }

      if ((pConfData->nStandard == IFX_TAPI_CID_STD_TELCORDIA) &&
          (pTxData->txHookMode == IFX_TAPI_CID_HM_OFFHOOK) )
      {
         /* This cases handles Bellcore when ending the sequence with an OSI. */
         if (pTxData->nCidSubState == 1)
         {
            /* start OSI */
            pTxData->bOSIinducedOnhook = IFX_TRUE;
            TAPI_Phone_Set_Linefeed (pTxData->pPhoneCh,
                                     IFX_TAPI_LINE_FEED_DISABLED);
            nTime = pConfData->OSItime;
            ret = E_FSM_CONTINUE;
         }
         else
         {
            /* end OSI */
            pTxData->bOSIinducedOffhook = IFX_TRUE;
            TAPI_Phone_Set_Linefeed (pTxData->pPhoneCh,
                                     IFX_TAPI_LINE_FEED_STANDBY);
            TAPI_Phone_Set_Linefeed (pTxData->pPhoneCh,
                                     IFX_TAPI_LINE_FEED_ACTIVE);
            nTime = pConfData->TapiCidTiming.dataOut2restoreTimeOffhook;
            /* do not return here but go to the next state */
            ret = E_FSM_COMPLETE;
         }
      }
   }

   /* This timer will trigger the execution of the next step. */
   if (ret != E_FSM_ERROR)
   {
      TAPI_SetTime_Timer (pTxData->CidTimerID, nTime, IFX_FALSE, IFX_FALSE);
   }

   /* This counter keeps track how often we are called in here. */
   pTxData->nCidSubState++;

   return ret;
}


/**
   State machine to handle the phase after cid sending.
   It may start ringing or restore the voice path.

   \param  pChannel     Pointer to TAPI_CHANNEL structure.

   \return
   FSM_STATUS_t

   \remarks
   - operation is done on a data channel.
   - This function is called from the higher level fsm and assumes that the
   data_lock is taken and the initial line mode was saved.
*/
static FSM_STATUS_t cid_fsm_sending_complete_exec(TAPI_CHANNEL *pChannel)
{
   IFX_TAPI_DRV_CTX_t *pDrvCtx =  pChannel->pTapiDevice->pDevDrvCtx;
   TAPI_CIDTX_DATA_t *pTxData  = &pChannel->pTapiCidData->TxData;
   IFX_TAPI_CID_CONF_t *pConfData  = &pChannel->pTapiCidData->ConfData;
   FSM_STATUS_t ret = E_FSM_COMPLETE;

   if (IFX_TAPI_PtrChk (pDrvCtx->SIG.DTMFD_Override))
   {
      IFX_TAPI_LL_DTMFD_OVERRIDE_t cfgDtmf;

      memset (&cfgDtmf, 0, sizeof (cfgDtmf));

      cfgDtmf.nMod = IFX_TAPI_MODULE_TYPE_COD;
      cfgDtmf.direction = IFX_TAPI_LL_DTMFD_DIR_INTERNAL;
      cfgDtmf.bOverride = IFX_FALSE;
      cfgDtmf.nOperation = IFX_ENABLE;

      /* deactivate the DTMF override - last two params are ignored */
      (IFX_void_t)pDrvCtx->SIG.DTMFD_Override(
         pChannel->pLLChannel, &cfgDtmf);
   }

   if (((pConfData->nStandard == IFX_TAPI_CID_STD_KPN_DTMF) ||
        (pConfData->nStandard == IFX_TAPI_CID_STD_KPN_DTMF_FSK)) &&
       (pTxData->txHookMode == IFX_TAPI_CID_HM_ONHOOK) &&
       (pTxData->cidGenType == IFX_TAPI_CID_GEN_TYPE_DTMF))
   {
       /* restore line polarity */
       TAPI_Phone_Set_Linefeed (pTxData->pPhoneCh,
                              pTxData->nLineModeInitial);
#if (TAPI_CFG_FEATURES & TAPI_FEAT_PHONE_DETECTION)
      /* restore previous status of the FXS Phone Plug Detection
         state machine */
      IFX_TAPI_PPD_ServiceStop(pTxData->pPhoneCh);
#endif /* (TAPI_CFG_FEATURES & TAPI_FEAT_PHONE_DETECTION) */
   }

   if ((pConfData->nStandard == IFX_TAPI_CID_STD_KPN_DTMF_FSK) &&
       (pTxData->txHookMode == IFX_TAPI_CID_HM_ONHOOK) &&
       (pTxData->cidGenType == IFX_TAPI_CID_GEN_TYPE_DTMF))
   {
      /* switch to the next data transmission
       via FSK with "first ringing" alert */
      pTxData->cidGenType = IFX_TAPI_CID_GEN_TYPE_FSK;
      pTxData->nAlertType = TAPI_CID_ALERT_FR;
      pTxData->nCidState = TAPI_CID_STATE_ALERT;
      ret = E_FSM_CONTINUE;
   }
   else {
       if (pTxData->bMute != IFX_FALSE)
       {
          /* undo muting of voice path */
          if (cid_mutevoice(pChannel, IFX_FALSE) == TAPI_statusOk)
             pTxData->bMute = IFX_FALSE;
       }

       if (pTxData->bRingStart == IFX_TRUE)
       {
          /* start ringing on the phone channel */
          IFX_TAPI_Ring_Engine_Start(pTxData->pPhoneCh, IFX_TRUE);
          pTxData->bRingStart = IFX_FALSE;
       }
   }

   TAPI_SetTime_Timer (pTxData->CidTimerID, MIN_TIMER_VAL, IFX_FALSE, IFX_FALSE);

   return ret;
}


/**
   State machine to handle the overall caller id sequence.

   \param  pChannel     Pointer to TAPI_CHANNEL structure.

   \return
   FSM_STATUS_t

   \remarks
   - operation is done on a data channel.
   - This function is called from the higher level fsm and assumes that the
   data_lock is taken and the initial line mode was saved.
*/
static FSM_STATUS_t cid_fsm_exec(TAPI_CHANNEL *pChannel)
{
   IFX_TAPI_DRV_CTX_t *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   IFX_TAPI_CID_CONF_t *pConfData = &pChannel->pTapiCidData->ConfData;
   TAPI_CIDTX_DATA_t *pTxData = &pChannel->pTapiCidData->TxData;
   FSM_STATUS_t ret = E_FSM_CONTINUE;
   IFX_TAPI_LL_DTMFD_OVERRIDE_t cfgDtmf;

   memset (&cfgDtmf, 0, sizeof (cfgDtmf));
   cfgDtmf.nMod = IFX_TAPI_MODULE_TYPE_COD;
   cfgDtmf.direction = IFX_TAPI_LL_DTMFD_DIR_INTERNAL;

   /* execute action */
   switch (pTxData->nCidState)
   {
   case TAPI_CID_STATE_ALERT:
      ret = cid_fsm_alert_exec(pChannel);
      break;
   case TAPI_CID_STATE_ACK:
      ret = cid_fsm_ack_exec(pChannel);
      break;
   case TAPI_CID_STATE_SENDING:
      ret = cid_fsm_sending_exec(pChannel);
      break;
   case TAPI_CID_STATE_SENDING_COMPLETE:
      ret = cid_fsm_sending_complete_exec(pChannel);
      break;
   case TAPI_CID_STATE_IDLE:
       ret = E_FSM_COMPLETE;
       break;
   }

   /* update state */
   switch (ret)
   {
   case E_FSM_COMPLETE:
      pTxData->nCidSubState = 0;
      ret = E_FSM_CONTINUE;
      switch (pTxData->nCidState)
      {
      case TAPI_CID_STATE_ALERT:
         if (((pTxData->txHookMode == IFX_TAPI_CID_HM_ONHOOK) &&
              (pConfData->nStandard != IFX_TAPI_CID_STD_NTT)) ||
             ((pTxData->txHookMode == IFX_TAPI_CID_HM_OFFHOOK) &&
              (pConfData->nStandard == IFX_TAPI_CID_STD_NTT)))
         {
            /* skip ack phase for on hook (not ntt) and off hook (only ntt) */
            pTxData->nCidState = TAPI_CID_STATE_SENDING;
         }
         else
         {
            pTxData->nCidState = TAPI_CID_STATE_ACK;
            if (IFX_TAPI_PtrChk (pDrvCtx->SIG.DTMFD_Override))
            {
               cfgDtmf.bOverride = IFX_TRUE;
               cfgDtmf.nOperation = IFX_ENABLE;

               /* Activate DTMF detector to be able to receive ACK tones. */
               (IFX_void_t)pDrvCtx->SIG.DTMFD_Override(
                  pChannel->pLLChannel, &cfgDtmf);
            }
         }
         break;
      case TAPI_CID_STATE_ACK:
         if (IFX_TAPI_PtrChk (pDrvCtx->SIG.DTMFD_Override))
         {
            cfgDtmf.bOverride = IFX_FALSE;
            cfgDtmf.nOperation = IFX_ENABLE;

            /* deactivate the override - last two params are ignored */
            (IFX_void_t)pDrvCtx->SIG.DTMFD_Override(
                  pChannel->pLLChannel, &cfgDtmf);
         }
         pTxData->nCidState = TAPI_CID_STATE_SENDING;
         break;
      case TAPI_CID_STATE_SENDING:
         pTxData->nCidState = TAPI_CID_STATE_SENDING_COMPLETE;
         break;
      case TAPI_CID_STATE_SENDING_COMPLETE:
         /* end of cid tx sequence reset state variables to idle mode */
         pTxData->bActive = IFX_FALSE;
         pTxData->nCidState = TAPI_CID_STATE_IDLE;
         /* Reset flags indicating to ignore hook events due to OSI */
         pTxData->bOSIinducedOffhook = IFX_FALSE;
         pTxData->bOSIinducedOnhook = IFX_FALSE;
         break;
      case TAPI_CID_STATE_IDLE:
         ret = E_FSM_COMPLETE;
         /* no change */
         break;
      }
      break;

   case E_FSM_CONTINUE:
      /* do nothing */
      break;

   case E_FSM_ERROR:
      switch (pTxData->nCidState)
      {
      case TAPI_CID_STATE_ACK:
         if (ptr_chk(pDrvCtx->SIG.DTMFD_Override, ""))
         {
            cfgDtmf.bOverride = IFX_FALSE;
            cfgDtmf.nOperation = IFX_ENABLE;

            /* deactivate the override - last two params are ignored */
            (IFX_void_t)pDrvCtx->SIG.DTMFD_Override(
                  pChannel->pLLChannel, &cfgDtmf);
         }
         break;

      default:
         /* do nothing */
         break;
      }
      break;
   }

   /* do cleanup if this is the last transistion  */
   switch (ret)
   {
   case E_FSM_COMPLETE:
   case E_FSM_ERROR:
      /* do cleanup */
      if (pTxData->bMute != IFX_FALSE)
      {
         /* undo muting of voice path */
         if (cid_mutevoice(pChannel, IFX_FALSE) == TAPI_statusOk)
            pTxData->bMute = IFX_FALSE;
      }
      /* reset the global state */
      pTxData->bActive = IFX_FALSE;
      break;
   case E_FSM_CONTINUE:
      /* do nothing */
      break;
   }

   return ret;
}


/**
   Reset state machine variables.

   \param  pChannel     Pointer to TAPI_CHANNEL structure.

   \remarks
   - operation is done on a data channel.
*/
static void cid_fsm_reset(TAPI_CHANNEL *pChannel)
{
   TAPI_CIDTX_DATA_t *pTxData = &pChannel->pTapiCidData->TxData;

   pTxData->bActive = IFX_FALSE;
   pTxData->bAck = IFX_FALSE;
   pTxData->bAck2 = IFX_TRUE;
   pTxData->nCidState = TAPI_CID_STATE_ALERT;
   pTxData->nCidSubState = 0;
   pTxData->bMute = IFX_FALSE;
}


/**
   CID timer callback function

   \param  Timer        Timer ID.
   \param  nArg         Timer callback argument -> pointer to TAPI channel.

   \remarks
   Timer runs on a data channel
*/
static IFX_void_t ifx_tapi_cid_OnTimer(Timer_ID Timer, IFX_ulong_t nArg)
{
   TAPI_CHANNEL   *pChannel   = (TAPI_CHANNEL *) nArg;

   IFX_UNUSED (Timer);

   /* begin of protected area */
   TAPI_OS_MutexGet (&pChannel->semTapiChDataLock);

   /* execute cid state machine */
   /* ignore the return because here we are in asynchronous context */
   /* errors are reported as asynchronous events directly from this function */
   cid_fsm_exec(pChannel);

   /* end of protected area */
   TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);
}
#endif /* TAPI_HAVE_TIMERS */


/* ============================= */
/* Global function definition    */
/* ============================= */

/**
   Initialise CID on the given channel.

   Initialise the data structures and resources needed for the CID
   state machine.

   \param  pChannel     Pointer to TAPI_CHANNEL structure.

   \return
   - TAPI_statusOk - if successful
   - TAPI_statusTimerFail - timer creation failed
   - TAPI_statusNoMem - not able to allocate memory

   \remarks This function is not protected. The global protection is done
   in the calling function with TAPI_OS_MutexGet (&pChannel->semTapiChDataLock)
*/
IFX_int32_t IFX_TAPI_CID_Initialise_Unprot (TAPI_CHANNEL *pChannel)
{
#ifdef TAPI_HAVE_TIMERS
   IFX_TAPI_DRV_CTX_t *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
#endif /* TAPI_HAVE_TIMERS */
   TAPI_CID_DATA_t   *pTapiCidData = pChannel->pTapiCidData;
   IFX_int32_t ret = TAPI_statusOk;

   /* Allocate data storage on the channel if not already existing. */
   if (pTapiCidData == IFX_NULL)
   {
      /* Allocate CID data storage */
      if ((pTapiCidData = TAPI_OS_Malloc (sizeof(*pTapiCidData))) == IFX_NULL)
      {
         RETURN_STATUS (TAPI_statusNoMem, 0);
      }
      /* Store pointer to data in the channel or we lose it on exit. */
      pChannel->pTapiCidData = pTapiCidData;
      memset (pTapiCidData, 0x00, sizeof(*pTapiCidData));
   }
#ifdef TAPI_HAVE_TIMERS
   /* From here on pTapiCidData and pChannel->pTapiCidData are always valid. */
   if (!IFX_TAPI_PtrChk (pDrvCtx->SIG.CIDSM_Start))
   {
      /* Create CID tx engine timer if not already existing. */
      if (pTapiCidData->TxData.CidTimerID == 0)
      {
         /* Initialize (create) cid timer. */
         pTapiCidData->TxData.CidTimerID =
            TAPI_Create_Timer((TIMER_ENTRY)ifx_tapi_cid_OnTimer,
                              (IFX_uintptr_t)pChannel);
         if(pTapiCidData->TxData.CidTimerID == 0)
         {
            /* errmsg: Timer creation failed */
            RETURN_STATUS (TAPI_statusTimerFail, 0);
         }
      }
   }
#endif /* TAPI_HAVE_TIMERS */

#ifdef TAPI_VERSION3
   if (pChannel->nChannel < pChannel->pTapiDevice->nResource.FSKRCount)
#endif /* TAPI_VERSION3 */
   {
      IFX_void_t *pEnd;
      IFX_void_t *pStart;

      /* allocate memory for caller id data reception */
      pStart = TAPI_OS_Malloc (IFX_TAPI_CID_RX_FIFO_SIZE *
                               sizeof (IFX_TAPI_CID_RX_DATA_t));
      if (pStart == IFX_NULL)
      {
         RETURN_STATUS (TAPI_statusNoMem, 0);
      }
      pEnd = ((IFX_TAPI_CID_RX_DATA_t *)pStart) +
               IFX_TAPI_CID_RX_FIFO_SIZE - 1;
      IFX_Fifo_Init (&(pTapiCidData->RxData.TapiCidRxFifo),
                     pStart, pEnd, sizeof(IFX_TAPI_CID_RX_DATA_t));
   }

   /* By default, assume phone and data channel are the same.
      In case of other mappings, this will be changed at the runtime */
   pTapiCidData->TxData.pPhoneCh = pChannel;

   /* set all configurable values to defaults */
   ifx_tapi_cid_SetDefaultConfig(pChannel);

   return ret;
}

/**
   Cleanup CID on the given channel.

   Free the resources needed for the CID state machine.

   \param  pChannel     Pointer to TAPI_CHANNEL structure.
*/
IFX_void_t IFX_TAPI_CID_Cleanup(TAPI_CHANNEL *pChannel)
{
   TAPI_OS_MutexGet (&pChannel->semTapiChDataLock);

   if (pChannel->pTapiCidData != IFX_NULL)
   {
      TAPI_CID_DATA_t   *pTapiCidData = pChannel->pTapiCidData;

#ifdef TAPI_HAVE_TIMERS
      /* unconditionally destruct the CID engine timer if existing */
      if (pTapiCidData->TxData.CidTimerID != 0)
      {
         TAPI_Delete_Timer (pTapiCidData->TxData.CidTimerID);
         pTapiCidData->TxData.CidTimerID = 0;
      }
#endif /* TAPI_HAVE_TIMERS */
#ifdef TAPI_VERSION3
      if (pChannel->nChannel < pChannel->pTapiDevice->nResource.FSKRCount)
#endif /* TAPI_VERSION3 */
      {
         /* free the memory for the receive fifo */
         TAPI_OS_Free (pTapiCidData->RxData.TapiCidRxFifo.pStart);
      }

      /* free the data storage on the channel */
      TAPI_OS_Free (pChannel->pTapiCidData);
      pChannel->pTapiCidData = IFX_NULL;
   }

   TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);
}

/**
   Trigger function called by ringing

   \param  pChannel     Pointer to TAPI_CHANNEL structure.
*/
IFX_void_t IFX_TAPI_CID_OnRingpause(TAPI_CHANNEL *pChannel)
{
#ifdef TAPI_HAVE_TIMERS
   TAPI_CIDTX_DATA_t *pTxData = &pChannel->pTapiCidData->TxData;

   /* Please note: locking of semTapiChDataLock is already done by the caller */

   if ((pTxData->bActive == IFX_TRUE) &&
       (pTxData->nCidState == TAPI_CID_STATE_ALERT) &&
       (pTxData->nAlertType == TAPI_CID_ALERT_FR))
   {
      /* stop the safety timer because we provide the trigger here */
      TAPI_Stop_Timer (pTxData->CidTimerID);

      /* errors are reported as asynchronous events directly from this function */
      cid_fsm_exec(pChannel);
   }
#endif /* TAPI_HAVE_TIMERS */
}


/**
   Hook event handling function to trigger CID

   \param  pChannel     Pointer to TAPI_CHANNEL structure.
   \param  bOffhook     IFX_TRUE for off-hook, IFX_FALSE for on-hook.
   \param  bSendHookEvent Returns if hook event should be propagated or not.
   \param  bDoRingStop  Returns if ringing should be stopped or not.
*/
IFX_void_t IFX_TAPI_CID_OnHookEvent(TAPI_CHANNEL *pChannel,
                                    IFX_boolean_t bOffhook,
                                    IFX_boolean_t *bSendHookEvent,
                                    IFX_boolean_t *bDoRingStop)
{
   IFX_TAPI_DRV_CTX_t   *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   TAPI_CHANNEL         *pDataCh = IFX_NULL;

   /* Check for internal CID SM implementation */
   if (IFX_TAPI_PtrChk (pDrvCtx->SIG.CIDSM_Start))
   {
      return;
   }

   /* Check all connected data channels for active CID sequences
      of NTT or BT that need special handling. */
   while (IFX_TAPI_Module_Find_Connected_Data_Channel (
             pChannel, IFX_TAPI_MAP_TYPE_PHONE, &pDataCh),
          pDataCh != IFX_NULL)
   {
      TAPI_CIDTX_DATA_t *pTxData = &pDataCh->pTapiCidData->TxData;
      IFX_TAPI_CID_CONF_t *pConfData = &pDataCh->pTapiCidData->ConfData;

      /* During running NTT transmissions on-hook/off-hook is used as
         a special signal. In this case set a flag for CID and suppress
         the hook event.*/
      /* check for a running NTT CID sequence */
      if ((pTxData->bActive == IFX_TRUE) &&
          (pConfData->nStandard == IFX_TAPI_CID_STD_NTT))
      {
         /* off hook ("primary answer signal") is flagged in bAck */
         if ((bOffhook == IFX_TRUE) && (pTxData->bAck == IFX_FALSE))
         {
            pTxData->bAck = IFX_TRUE;
            *bDoRingStop = IFX_FALSE;
            *bSendHookEvent = IFX_FALSE;
         }
         /* on hook ("incoming successful signal") is flagged
            in bAck2 */
         if ((bOffhook == IFX_FALSE) && (pTxData->bAck2 == IFX_FALSE))
         {
            pTxData->bAck2 = IFX_TRUE;
            *bDoRingStop = IFX_FALSE;
            *bSendHookEvent = IFX_FALSE;
         }
      }

      /* When BT standard is currently in ALERT phase a wetting pulse
         can occur which is detected as off-hook. This must not stop
         the CID sequence. Because of this we suppress here the ring
         stop which does implicitly a CID stop. Instead the ring stop
         (with CID stop) for a hook event will be done after validation
         of the hook event by the hook-state-machine. */
      if ((pTxData->bActive == IFX_TRUE) &&
          (pConfData->nStandard == IFX_TAPI_CID_STD_SIN) &&
          (pTxData->nCidState == TAPI_CID_STATE_ALERT) &&
          (pTxData->nCidSubState >= 2))
      {
         *bDoRingStop = IFX_FALSE;
      }

      /* Ignore hook events that are the result of a OSI line mode
         change that was done as part of a CID sequence. */
      if ((bOffhook == IFX_TRUE) && (pTxData->bOSIinducedOffhook == IFX_TRUE))
      {
         pTxData->bOSIinducedOffhook = IFX_FALSE;
         *bDoRingStop = IFX_FALSE;
         *bSendHookEvent = IFX_FALSE;
      }
      if ((bOffhook == IFX_FALSE) && (pTxData->bOSIinducedOnhook == IFX_TRUE))
      {
         pTxData->bOSIinducedOnhook = IFX_FALSE;
         *bDoRingStop = IFX_FALSE;
         *bSendHookEvent = IFX_FALSE;
      }
   }
}


/**
   DTMF event handling function to trigger CID

   Sets the ack-flag in the FSK if the sequence is active and the DTMF tone
   is either exactly the configured one or any A-D tone if the configuration is
   set to zero.

   \param  pChannel     Pointer to TAPI_CHANNEL structure.
   \param  nDtmfAscii   DTMF digit in ASCII encoding

   \return
   - IFX_TRUE  if the event was processed
   - IFX_FALSE if the event was not processed
*/
IFX_boolean_t IFX_TAPI_CID_OnDtmfEvent(TAPI_CHANNEL *pChannel,
                                       IFX_uint8_t nDtmfAscii)
{
   if ((pChannel->pTapiCidData->TxData.bActive == IFX_TRUE) &&
       ((pChannel->pTapiCidData->TxData.ackToneCode == nDtmfAscii) ||
        ((pChannel->pTapiCidData->TxData.ackToneCode == 0x00) &&
         (nDtmfAscii >= 'A') && (nDtmfAscii <= 'D'))))
   {
      pChannel->pTapiCidData->TxData.bAck = IFX_TRUE;
      return IFX_TRUE;
   }

   return IFX_FALSE;
}


/**
   CID RX end event handling function

   \param  pChannel     Pointer to TAPI_CHANNEL structure.
*/
IFX_void_t IFX_TAPI_CID_OnCidRxEndEvent(TAPI_CHANNEL *pChannel)
{
   pChannel->pTapiCidData->RxData.stat.nStatus = IFX_TAPI_CID_RX_STATE_DATA_READY;
}


/**
   Processes user CID data according to chosen service and CID sending type
   (FSK, DTMF) and send it out directly.

   \param  pChannel     Pointer to TAPI_CHANNEL structure.
   \param  pCidInfo     Contains the caller id settings (IFX_TAPI_CID_MSG_t).

   \return
   - TAPI_statusOk If successful
   - TAPI_statusLLNotSupp CID Tx failure, no LL support
   - TAPI_statusCIDLineModeNotSuitable Line mode not suitable for CID info Tx
   - TAPI_statusCIDActive CID Tx already active
   - TAPI_statusInvalidCh: Cannel does not have the required resource

   \remarks
   - If a non supported service is set, error will be returned.
   - This implementation complies with ETSI standards.
   - This interface is assumed to be working on a data channel.
*/
IFX_int32_t TAPI_Phone_CID_Info_Tx (TAPI_CHANNEL *pChannel,
                                    IFX_TAPI_CID_MSG_t const *pCidInfo)
{
   IFX_TAPI_DRV_CTX_t   *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   TAPI_CIDTX_DATA_t    *pTxData = &pChannel->pTapiCidData->TxData;
   IFX_TAPI_CID_CONF_t    *pConfData = &pChannel->pTapiCidData->ConfData;
   IFX_int32_t          ret = TAPI_statusOk,
                        retLL = TAPI_statusOk;

#ifdef TAPI_VERSION3
   /* check if channel has the required generators */
   if ((pChannel->nChannel >= pChannel->pTapiDevice->nResource.DTMFGCount) &&
       (pChannel->nChannel >= pChannel->pTapiDevice->nResource.FSKGCount ))
   {
      /* errmsg: Service not supported on called channel context */
      RETURN_STATUS (TAPI_statusInvalidCh, 0);
   }
#endif /* TAPI_VERSION3 */

   /* begin of protected area */
   TAPI_OS_MutexGet (&pChannel->semTapiChDataLock);

   /* Attempt to find out if we are connected to a phone module. If a phone
      module is connected do some extra checks. Otherwise just skip this. */
   if (IFX_TAPI_PtrChk (pDrvCtx->CON.Data_Channel_Find_Connected_Module))
   {
      TAPI_CHANNEL            *pTapiPhoneCh;
      IFX_TAPI_MAP_TYPE_t     nModType;

      (IFX_void_t)pDrvCtx->CON.Data_Channel_Find_Connected_Module(
         pChannel->pLLChannel, &pTapiPhoneCh, &nModType);
      if (nModType == IFX_TAPI_MAP_TYPE_PHONE)
      {
         /* Connected module is a phone so do some extra checks */
         /* check for on hook/off hook mismatch with current state */
         if ( ((pCidInfo->txMode == IFX_TAPI_CID_HM_OFFHOOK) &&
               (pTapiPhoneCh->TapiOpControlData.bHookState == IFX_FALSE)) ||
              ((pCidInfo->txMode == IFX_TAPI_CID_HM_ONHOOK) &&
               (pTapiPhoneCh->TapiOpControlData.bHookState == IFX_TRUE)) )
         {
            TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("\nDRV_ERROR: "
                   "Hookstate not matching with transmission mode!\n"));
            /* errmsg: Hookstate not matching with transmission mode*/
            ret = TAPI_statusCIDHookMissmatch;
         }

#if 1 // ctc merged
		//Quantum Added for CID sending Line have to be set to active mode
		ret = TAPI_Phone_Set_Linefeed (pTapiPhoneCh, IFX_TAPI_LINE_FEED_ACTIVE);
#endif

         /* check line mode */
         if (TAPI_SUCCESS(ret))
         {
            IFX_uint8_t nLineMode;

#if (TAPI_CFG_FEATURES & TAPI_FEAT_PHONE_DETECTION)
            IFX_TAPI_PPD_ServiceStart(pTapiPhoneCh);
#endif /* (TAPI_CFG_FEATURES & TAPI_FEAT_PHONE_DETECTION) */
            TAPI_Phone_Linefeed_Get(pTapiPhoneCh, &nLineMode);

            /* To transmit data the line-mode must be active. Other than in
               TX_SEQ no automatic switching is done for the transmission. */
            switch (nLineMode)
            {
            case  IFX_TAPI_LINE_FEED_ACTIVE:
            case  IFX_TAPI_LINE_FEED_ACTIVE_REV:
            case  IFX_TAPI_LINE_FEED_NORMAL_LOW:
            case  IFX_TAPI_LINE_FEED_NORMAL_AUTO:
            case  IFX_TAPI_LINE_FEED_REVERSED_LOW:
            case  IFX_TAPI_LINE_FEED_REVERSED_AUTO:
               /* all the linemodes above are suitable for transmission */
               break;
            default:
               TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("\nDRV_ERROR: "
                      "Line mode not suitable for CID info transmission!\n"));
               /* errmsg: Line mode not suitable for CID sequence transmission */
               ret = TAPI_statusCIDLineModeNotSuitable;
               break;
            }
         }
      }
   }

   /* prevent transmission when caller id sequence is already active! */
   if (TAPI_SUCCESS(ret) && (pTxData->bActive == IFX_TRUE))
      ret = TAPI_statusCIDActive;

   /* flag that we use info_tx which is used for sending the correct event */
   if (TAPI_SUCCESS(ret))
   {
      pTxData->bUseSequence = IFX_FALSE;

      ret = cid_prepare_data(pChannel, pCidInfo);
   }

   if (TAPI_SUCCESS(ret))
   {
      if (IFX_TAPI_PtrChk (pDrvCtx->SIG.CID_TX_Start))
      {
         IFX_TAPI_CID_TX_t  cidTxData;

         memset (&cidTxData, 0, sizeof (IFX_TAPI_CID_TX_t));

         /* pass only these parameters to the LL-driver */
         cidTxData.txHookMode   = pTxData->txHookMode;
         cidTxData.pFskConf     = &pConfData->TapiCidFskConf;
         cidTxData.pDtmfConf    = &pConfData->TapiCidDtmfConf;
         /* get parameter according to the data type */
         cidTxData.pCidParam    = pTxData->cidBuf[pTxData->cidGenType].pBuf;
         cidTxData.nCidParamLen = pTxData->cidBuf[pTxData->cidGenType].nLen;

         /* Determine CID transmission data format */
         if (IFX_TAPI_CID_GEN_TYPE_DTMF == pTxData->cidGenType)
         {
            cidTxData.cidDataType = IFX_TAPI_CID_DATA_TYPE_DTMF;
         }
         else if (IFX_TAPI_CID_STD_TELCORDIA == pConfData->nStandard)
         {
            cidTxData.cidDataType = IFX_TAPI_CID_DATA_TYPE_FSK_BEL202;
         }
         else
         {
            cidTxData.cidDataType = IFX_TAPI_CID_DATA_TYPE_FSK_V23;
         }

         retLL = pDrvCtx->SIG.CID_TX_Start(pChannel->pLLChannel, &cidTxData);
      }
      else
      {
         TRACE(TAPI_DRV, DBG_LEVEL_HIGH,
               ("DRV_ERROR: CID Sending failed. No low level support \n"));
         ret = TAPI_statusLLNotSupp;
      }
   }

   /* end of protected area */
   TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);

   if (!TAPI_SUCCESS(ret))
      RETURN_STATUS(ret, retLL);
   if (!TAPI_SUCCESS(retLL))
      RETURN_STATUS (TAPI_statusCidInfoStartFail, retLL);

   RETURN_STATUS (ret, 0);
}


/**
   Processes user CID data according to chosen service and cid sending type
   (FSK, DTMF) and starts the complete CID sequence depending on the
   configuration.

   \param  pChannel     Pointer to TAPI_CHANNEL structure.
   \param  pCidInfo     Contains the caller id settings (IFX_TAPI_CID_MSG_t).

   \return
   - TAPI_statusOk If successful
   - TAPI_statusParam Wrong input parameter
   - TAPI_statusCIDLineModeNotSuitable Line mode not suitable for CID seq Tx
   - TAPI_statusErr Fail
   - TAPI_statusInvalidCh: Cannel does not have the required resource

   \remarks
   - If a non supported service is set, error will be returned.
   - This interface is assumed to be working on a data channel.
*/
IFX_int32_t TAPI_Phone_CID_Seq_Tx (TAPI_CHANNEL *pChannel,
                                   IFX_TAPI_CID_MSG_t const *pCidInfo)
{
   IFX_TAPI_DRV_CTX_t   *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   IFX_TAPI_CID_CONF_t    *pConfData = &pChannel->pTapiCidData->ConfData;
   TAPI_CIDTX_DATA_t    *pTxData = &pChannel->pTapiCidData->TxData;
   IFX_TAPI_MAP_TYPE_t  nModType;
   IFX_int32_t          ret = TAPI_statusOk, retLL = TAPI_statusOk;

#ifdef TAPI_VERSION3
   /* check if channel has the required generators */
   if ((pChannel->nChannel >= pChannel->pTapiDevice->nResource.DTMFGCount) &&
       (pChannel->nChannel >= pChannel->pTapiDevice->nResource.FSKGCount ))
   {
      /* errmsg: Service not supported on called channel context */
      RETURN_STATUS (TAPI_statusInvalidCh, 0);
   }
#endif /* TAPI_VERSION3 */

   /* begin of protected area */
   TAPI_OS_MutexGet (&pChannel->semTapiChDataLock);

   /* get phone channel on which this data channel is connected
      and update phone operations pointers accordingly */
   if (IFX_TAPI_PtrChk (pDrvCtx->CON.Data_Channel_Find_Connected_Module))
   {
      (IFX_void_t)pDrvCtx->CON.Data_Channel_Find_Connected_Module(
         pChannel->pLLChannel, &pTxData->pPhoneCh, &nModType);
      if (nModType != IFX_TAPI_MAP_TYPE_PHONE)
      {
         /* Connected module is not a phone so clear the association */
         pTxData->pPhoneCh = IFX_NULL;
      }
   }
   else
   {
      /* if LL makes no special connections we assume a straight connection */
      pTxData->pPhoneCh = pChannel;
   }
   if (pTxData->pPhoneCh == IFX_NULL)
   {
      TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
             ("\nDRV_ERROR: Data channel is not connected to a phone\n"));
      /* More specific: No phone is connected on the data channel main input. */
      TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);
      /* errmsg: No phone is connected on the data channel main input */
      RETURN_STATUS (TAPI_statusCIDNoPhoneAtDataCh, 0);
   }

   /* check for on hook/off hook mismatch with current state */
   if ( ((pCidInfo->txMode == IFX_TAPI_CID_HM_OFFHOOK) &&
         (pTxData->pPhoneCh->TapiOpControlData.bHookState == IFX_FALSE)) ||
        ((pCidInfo->txMode == IFX_TAPI_CID_HM_ONHOOK) &&
         (pTxData->pPhoneCh->TapiOpControlData.bHookState == IFX_TRUE))
      )
   {
      TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);
      /* errmsg: Hookstate not matching with transmission mode*/
      RETURN_STATUS (TAPI_statusCIDHookMissmatch, 0);
   }

   /* prevent another transmission when caller id sequence is already active! */
   if (pTxData->bActive == IFX_TRUE)
   {
      TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);
      RETURN_STATUS (TAPI_statusCIDActive, 0);
   }

   /* flag that we use seq_tx which is used for sending the correct event */
   pTxData->bUseSequence = IFX_TRUE;

   /* encode the data to be sent according to the selected standard */
   ret = cid_prepare_data(pChannel, pCidInfo);

   /* Check and prepare ringing engine if available */
   if (TAPI_SUCCESS (ret))
   {
      ret = IFX_TAPI_Ring_Prepare (pTxData->pPhoneCh);
   }

   /* retrieve the timing for alert ringing from ring cadence configuration */
   if (TAPI_SUCCESS (ret))
   {
      ret = IFX_TAPI_Ring_CalculateRingTiming(pTxData->pPhoneCh,
                                              &pConfData->cadenceRingBurst,
                                              &pConfData->cadenceRingPause);
   }

   /* select the alert sequence according to standard and hook state */
   if (TAPI_SUCCESS (ret))
   {
      ret = cid_determine_alert_type(pChannel, pCidInfo);
   }

   /* store the configured ack tone in ASCII format */
   pTxData->ackToneCode = (IFX_uint8_t)pConfData->ackTone;

   /* at the end of CID fsm start periodic ringing if onhook and message type
      is "Call Setup" */
   if ((pCidInfo->txMode == IFX_TAPI_CID_HM_ONHOOK) &&
       (pCidInfo->messageType == IFX_TAPI_CID_MT_CSUP))
   {
      pTxData->bRingStart = IFX_TRUE;
   }
   else
   {
      pTxData->bRingStart = IFX_FALSE;
   }

   /* check line mode and switch line to active for CID if neccessary */
   if (TAPI_SUCCESS (ret))
   {
      IFX_uint8_t nLineMode;

#if (TAPI_CFG_FEATURES & TAPI_FEAT_PHONE_DETECTION)
      IFX_TAPI_PPD_ServiceStart(pTxData->pPhoneCh);
#endif /* (TAPI_CFG_FEATURES & TAPI_FEAT_PHONE_DETECTION) */

      TAPI_Phone_Linefeed_Get(pTxData->pPhoneCh, &nLineMode);

      /* store the initial line mode before changing it below */
      pTxData->nLineModeInitial = nLineMode;
      /* Line must be either in standby mode or in active mode to send CID.
         Automatic switching and low power modes are also allowed.
         Any other mode is an error
         Additionally find the reversed mode that is needed for line reversal
         in the alert sequences of some CID modes. */
      switch (nLineMode)
      {
      case  IFX_TAPI_LINE_FEED_ACTIVE:
         pTxData->nLineModeReverse = IFX_TAPI_LINE_FEED_ACTIVE_REV;
         break;
      case  IFX_TAPI_LINE_FEED_NORMAL_LOW:
         pTxData->nLineModeReverse = IFX_TAPI_LINE_FEED_REVERSED_LOW;
         break;
      case  IFX_TAPI_LINE_FEED_NORMAL_AUTO:
         pTxData->nLineModeReverse = IFX_TAPI_LINE_FEED_REVERSED_AUTO;
         break;
      case  IFX_TAPI_LINE_FEED_ACTIVE_REV:
         pTxData->nLineModeReverse = IFX_TAPI_LINE_FEED_ACTIVE;
         break;
      case  IFX_TAPI_LINE_FEED_REVERSED_LOW:
         pTxData->nLineModeReverse = IFX_TAPI_LINE_FEED_NORMAL_LOW;
         break;
      case  IFX_TAPI_LINE_FEED_REVERSED_AUTO:
         pTxData->nLineModeReverse = IFX_TAPI_LINE_FEED_NORMAL_AUTO;
         break;
      case  IFX_TAPI_LINE_FEED_STANDBY:
      case  IFX_TAPI_LINE_FEED_RING_PAUSE:
         /* also allowed but automatically change to active */
         ret = TAPI_Phone_Set_Linefeed (pTxData->pPhoneCh, IFX_TAPI_LINE_FEED_ACTIVE);

         if (!TAPI_SUCCESS (ret))
            break;

         pTxData->nLineModeReverse = IFX_TAPI_LINE_FEED_ACTIVE_REV;
         break;
      default:
         TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);
         /* errmsg: Line mode not suitable for CID sequence transmission */
#if 0 // ctc merged
         RETURN_STATUS(TAPI_statusCIDLineModeNotSuitable,0);
#else
//Quantum: Add this default setting, in case of Danube read status failed...
		 pTxData->nLineModeReverse = IFX_TAPI_LINE_FEED_ACTIVE;
         break;
#endif
      }
   }

   /* If no error occured up to now start the CID state machine. */
   if (TAPI_SUCCESS (ret))
   {
      if (IFX_TAPI_PtrChk (pDrvCtx->SIG.CIDSM_Start))
      {
         IFX_TAPI_CID_SEQ_CONF_t  cidSeqData;

         /* Now the process can start */
         pTxData->bActive     = IFX_TRUE;
         pConfData->bRingOnly = IFX_FALSE;

         if (IFX_TAPI_CID_HM_OFFHOOK == pCidInfo->txMode)
         {
            /* mute voice path */
            if (cid_mutevoice(pChannel, IFX_TRUE) == TAPI_statusOk)
               pTxData->bMute = IFX_TRUE;
         }

         memset (&cidSeqData, 0, sizeof (IFX_TAPI_CID_SEQ_CONF_t));

         /* pass only these parameters to the LL-driver */
         cidSeqData.txHookMode   = pTxData->txHookMode;
         cidSeqData.pCidParam     = pTxData->cidBuf[pTxData->cidGenType].pBuf;
         cidSeqData.nCidParamLen = pTxData->cidBuf[pTxData->cidGenType].nLen;
         cidSeqData.nAlertType   = pTxData->nAlertType;
         cidSeqData.bRingStart   = pTxData->bRingStart;
         cidSeqData.pConfData    = pConfData;

         /* retrieve the cadence for from ring configuration */
         IFX_TAPI_Ring_CidCadencePrepare (pChannel, &cidSeqData);

         retLL = pDrvCtx->SIG.CIDSM_Start(pChannel->pLLChannel, &cidSeqData);

         if (!TAPI_SUCCESS (retLL))
         {
            pTxData->bActive = IFX_FALSE;

            if (IFX_TAPI_CID_HM_OFFHOOK == pCidInfo->txMode &&
               IFX_TRUE == pTxData->bMute)
            {
            /* undo muting of voice path */
            if (cid_mutevoice(pChannel, IFX_FALSE) == TAPI_statusOk)
               pTxData->bMute = IFX_FALSE;
            }

            ret = TAPI_statusCidSmStart;
         }
      }
#ifdef TAPI_HAVE_TIMERS
      else
      {
         /* reset the state machine */
         cid_fsm_reset(pChannel);

         /* Now the process can start */
         pTxData->bActive = IFX_TRUE;

         /* execute first step of state machine */
         if (cid_fsm_exec(pChannel) == E_FSM_ERROR)
         {
            ret = IFX_ERROR;
            pTxData->bActive = IFX_FALSE;
         }
      }
#endif /* TAPI_HAVE_TIMERS */
   }

   /* end of protected area */
   TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);

   RETURN_STATUS (ret, retLL);
}


/**
   Stops a previously started CID transmission regardless of the state it is
   currently in.

   \param pChannel      Pointer to TAPI_CHANNEL structure.

   \return
   - TAPI_statusOk If successful
   - TAPI_statusErr Fail
   - TAPI_statusInvalidCh: Cannel does not have the required resource

   \remarks
   This function is called internally when ring_stop detects that a CID
   sequence is running and also upon offhook of the phone channel.
   It is called from IOCTL CID_TX_INFO_STOP. And despite the name this would
   also work to stop a sequence currently playing.
*/
IFX_int32_t TAPI_Phone_CID_Stop_Tx(TAPI_CHANNEL *pChannel)
{
   IFX_TAPI_DRV_CTX_t   *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   TAPI_CIDTX_DATA_t    *pTxData = &pChannel->pTapiCidData->TxData;
   IFX_int32_t          ret = TAPI_statusOk,
                        retLL = TAPI_statusOk;

#ifdef TAPI_VERSION3
   /* check if channel has the required generators */
   if ((pChannel->nChannel >= pChannel->pTapiDevice->nResource.DTMFGCount) &&
       (pChannel->nChannel >= pChannel->pTapiDevice->nResource.FSKGCount ))
   {
      /* errmsg: Service not supported on called channel context */
      RETURN_STATUS (TAPI_statusInvalidCh, 0);
   }
#endif /* TAPI_VERSION3 */

   /* check that LL driver support CID functionality */
   if (!IFX_TAPI_PtrChk (pDrvCtx->SIG.CID_TX_Stop))
   {
      RETURN_STATUS (TAPI_statusLLNotSupp, 0);
   }

   /* begin of protected area */
   TAPI_OS_MutexGet (&pChannel->semTapiChDataLock);

   if (IFX_TRUE == pTxData->bUseSequence &&
       IFX_TAPI_PtrChk (pDrvCtx->SIG.CIDSM_Stop) &&
       IFX_TRUE == pTxData->bActive)
   {
      ret = pDrvCtx->SIG.CIDSM_Stop (pChannel->pLLChannel);

      pTxData->bActive = IFX_FALSE;

      if (pTxData->bMute == IFX_TRUE)
      {
         /* undo muting of voice path */
         if (cid_mutevoice(pChannel, IFX_FALSE) == TAPI_statusOk)
            pTxData->bMute = IFX_FALSE;
      }
   }
#ifdef TAPI_HAVE_TIMERS
   else    if (IFX_TRUE == pTxData->bUseSequence &&
               !IFX_TAPI_PtrChk (pDrvCtx->SIG.CIDSM_Stop))
   {
      IFX_TAPI_LL_DTMFD_OVERRIDE_t cfgDtmf;

      memset (&cfgDtmf, 0, sizeof (cfgDtmf));

      cfgDtmf.nMod = IFX_TAPI_MODULE_TYPE_COD;
      cfgDtmf.direction = IFX_TAPI_LL_DTMFD_DIR_INTERNAL;
      cfgDtmf.bOverride = IFX_FALSE;
      cfgDtmf.nOperation = IFX_ENABLE;

      /* When adding some action here please take into consideration that
         the state variable is already advanced to the next state when the
         FSM is waiting on a timer for the current state to be completed. */
      switch (pTxData->nCidState)
      {
      case TAPI_CID_STATE_ALERT:
         switch (pTxData->nAlertType)
         {
         case TAPI_CID_ALERT_FR:
         case TAPI_CID_ALERT_RP:
         case TAPI_CID_ALERT_LR:
         case TAPI_CID_ALERT_LR_DTAS:
         case TAPI_CID_ALERT_CAR_NTT:
            TAPI_Phone_Set_Linefeed (pTxData->pPhoneCh,
                                     pTxData->nLineModeInitial);
            break;
         default:
            /* do nothing */
            break;
         }
         /* deliberately fall through */
         /*lint -fallthrough*/
      case TAPI_CID_STATE_ACK:
         /* deactivate the DTMF override - last two params are ignored */
         if (ptr_chk(pDrvCtx->SIG.DTMFD_Override, ""))
         {
            /* deactivate the override - last two params are ignored */
            (IFX_void_t)pDrvCtx->SIG.DTMFD_Override(
               pChannel->pLLChannel, &cfgDtmf);
         }
         /* deliberately fall through */
         /*lint -fallthrough*/
      case TAPI_CID_STATE_SENDING:
         TAPI_Stop_Timer (pTxData->CidTimerID);
         break;
      case TAPI_CID_STATE_SENDING_COMPLETE:
         /* current state is sending and waiting for sending complete */
         if (IFX_TAPI_PtrChk (pDrvCtx->SIG.CID_TX_Stop))
            (IFX_void_t)pDrvCtx->SIG.CID_TX_Stop(pChannel->pLLChannel);
         /* If sending cannot be stopped we do not complain because it will
            stop after the sending anyway. */
         TAPI_Stop_Timer (pTxData->CidTimerID);
         break;
      case TAPI_CID_STATE_IDLE:
         /* unlock TAPI data mutex for protected function */
         TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);

         IFX_TAPI_Ring_Stop (pTxData->pPhoneCh);

         /* restore TAPI data mutex lock */
         TAPI_OS_MutexGet (&pChannel->semTapiChDataLock);
         break;
      }

      if (IFX_TAPI_PtrChk (pDrvCtx->SIG.DTMFD_Override))
      {
         /* deactivate the DTMF override - last two params are ignored */
         (IFX_void_t)pDrvCtx->SIG.DTMFD_Override(
               pChannel->pLLChannel, &cfgDtmf);
      }

      if (pTxData->bMute == IFX_TRUE)
      {
         /* undo muting of voice path */
         if (cid_mutevoice(pChannel, IFX_FALSE) == TAPI_statusOk)
            pTxData->bMute = IFX_FALSE;
      }

      /* reset to the initial state of state machine */
      cid_fsm_reset(pChannel);
   }
#endif /* TAPI_HAVE_TIMERS */
   else
   {
      /* TX_INFO */
      if (IFX_TAPI_PtrChk (pDrvCtx->SIG.CID_TX_Stop))
      {
         retLL = pDrvCtx->SIG.CID_TX_Stop(pChannel->pLLChannel);
      }
      else
      {
         ret = TAPI_statusLLNotSupp;
      }
   }

   /* end of protected area */
   TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);

   if (!TAPI_SUCCESS(ret))
   {
      RETURN_STATUS(ret, retLL);
   }
   if (!TAPI_SUCCESS(retLL))
   {
      RETURN_STATUS (TAPI_statusCIDStopTx, retLL);
   }
   RETURN_STATUS (ret, 0);
}


/* ============================= */
/* CID config functions          */
/* ============================= */

/**
   Set the default configuration for caller id.

   \param  pChannel     Pointer to TAPI_CHANNEL structure.
*/
static IFX_void_t ifx_tapi_cid_SetDefaultConfig (TAPI_CHANNEL *pChannel)
{
   IFX_TAPI_CID_CONF_t *pConfData = &pChannel->pTapiCidData->ConfData;

   /* locking has to be done by the calling function */

   pConfData->nStandard = IFX_TAPI_CID_STD_TELCORDIA;
   pConfData->nETSIAlertRing = IFX_TAPI_CID_ALERT_ETSI_FR;
   pConfData->nETSIAlertNoRing = IFX_TAPI_CID_ALERT_ETSI_RP;
   pConfData->nAlertToneOnhook  = 0;
   pConfData->nAlertToneOffhook  = 0;
   pConfData->ackTone = 'D'; /* ASCII encoding */
   pConfData->ringPulseOnTime = 500; /* ms */
   pConfData->ringPulseOffTime = 500; /* ms */
   pConfData->ringPulseLoop = 5;
   pConfData->OSIoffhook = IFX_FALSE;
   pConfData->OSItime = 200; /* ms */
   pConfData->ack2Timeout = 7000; /* ms */
   pConfData->nSasToneTime = 0; /* ms */
   pConfData->beforeSAStime = 20; /* ms */
   pConfData->SAS2CAStime = 20; /* ms */
   pConfData->nSAStone = 0;

   ciddtmf_abscli_etsi(&pConfData->TapiCidDtmfAbsCli);
   pConfData->TapiCidTiming   = default_cid_timing;
   pConfData->TapiCidFskConf  = default_cid_fsk_conf;
   pConfData->TapiCidDtmfConf = default_cid_dtmf_conf;
}


/**
   Set the configuration for caller id.

   \param  pChannel     Pointer to TAPI_CHANNEL structure.
   \param  pCidConf     Contains the new caller id configuration to be set.

   \return
   - TAPI_statusOk If successful
   - TAPI_statusErr Fail
   - TAPI_statusInvalidCh: Cannel does not have the required resource
*/
IFX_int32_t TAPI_Phone_CID_SetConfig (TAPI_CHANNEL *pChannel,
                                      IFX_TAPI_CID_CFG_t const *pCidConf)
{
   IFX_TAPI_CID_STD_TYPE_t    cfg;
   IFX_TAPI_CID_TIMING_t      *pCIDTiming  = IFX_NULL;
   IFX_TAPI_CID_FSK_CFG_t     *pFSKConf    = IFX_NULL;
   IFX_TAPI_CID_DTMF_CFG_t    *pDTMFConf   = IFX_NULL;
   IFX_TAPI_CID_ABS_REASON_t  *pDTMFAbsCli = IFX_NULL;
   IFX_TAPI_CID_CONF_t       *pTapiCidConf = &pChannel->pTapiCidData->ConfData;
   IFX_int32_t                ret           = TAPI_statusOk;
   IFX_TAPI_CID_DTMF_CFG_t const *pDefaultDtmfConf = &default_cid_dtmf_conf;

#ifdef TAPI_VERSION3
   /* check if channel has the required generators or receivers */
   if ((pChannel->nChannel >= pChannel->pTapiDevice->nResource.DTMFGCount) &&
       (pChannel->nChannel >= pChannel->pTapiDevice->nResource.DTMFRCount) &&
       (pChannel->nChannel >= pChannel->pTapiDevice->nResource.FSKGCount) &&
       (pChannel->nChannel >= pChannel->pTapiDevice->nResource.FSKRCount) )
   {
      /* errmsg: Service not supported on called channel context */
      RETURN_STATUS (TAPI_statusInvalidCh, 0);
   }
#endif /* TAPI_VERSION3 */

   /* begin of protected area */
   TAPI_OS_MutexGet (&pChannel->semTapiChDataLock);

   /* reset settings, to have default values if they are not provided
      by the new configuration */
   ifx_tapi_cid_SetDefaultConfig(pChannel);

   switch (pCidConf->nStandard)
   {
   case IFX_TAPI_CID_STD_TELCORDIA:
      if (pCidConf->cfg != IFX_NULL)
      {
         IFX_TAPI_CID_STD_TELCORDIA_t *telcordia = &cfg.telcordia;
         TAPI_OS_CpyUsr2Kern (&cfg, pCidConf->cfg,
                              sizeof(IFX_TAPI_CID_STD_TYPE_t));
         pTapiCidConf->nAlertToneOnhook = telcordia->nAlertToneOnhook;
         pTapiCidConf->nAlertToneOffhook = telcordia->nAlertToneOffhook;
         pTapiCidConf->ackTone = telcordia->ackTone;
#ifdef TAPI_VERSION3
         /* OSI usage is not supported */
#else /* non TAPI_VERSION3 */
         pTapiCidConf->OSIoffhook = telcordia->OSIoffhook;
#endif /* TAPI_VERSION3 */
         pTapiCidConf->OSItime = telcordia->OSItime;
         pCIDTiming = telcordia->pCIDTiming;
         pFSKConf = telcordia->pFSKConf;

         if (telcordia->beforeSAStime != 0)
         {
            pTapiCidConf->beforeSAStime = telcordia->beforeSAStime;
         }
         if (telcordia->SAS2CAStime != 0)
         {
            pTapiCidConf->SAS2CAStime = telcordia->SAS2CAStime;
         }
         if (telcordia->nSAStone)
         {
            IFX_uint32_t nSASduration;

            nSASduration = IFX_TAPI_Tone_DurationGet(telcordia->nSAStone);
            if (nSASduration == 0)
            {
               /* SAS tone does not have a finite duration */
               /* SAS tone definition from defaults is used (no SAS tone) */
               ret = IFX_ERROR;
            }
            else
            {
               pTapiCidConf->nSasToneTime = nSASduration;
               pTapiCidConf->nSAStone = telcordia->nSAStone;
            }
         }
      }
      break;
   case IFX_TAPI_CID_STD_ETSI_FSK:
      if (pCidConf->cfg != IFX_NULL)
      {
         IFX_TAPI_CID_STD_ETSI_FSK_t *etsiFSK = &cfg.etsiFSK;
         TAPI_OS_CpyUsr2Kern (&cfg, pCidConf->cfg,
                              sizeof(IFX_TAPI_CID_STD_TYPE_t));
         pTapiCidConf->nAlertToneOnhook = etsiFSK->nAlertToneOnhook;
         pTapiCidConf->nAlertToneOffhook = etsiFSK->nAlertToneOffhook;
         pTapiCidConf->ringPulseOnTime = etsiFSK->ringPulseTime;
         pTapiCidConf->ringPulseOffTime = etsiFSK->ringPulseTime;
         pTapiCidConf->ackTone = etsiFSK->ackTone;
         pTapiCidConf->nETSIAlertRing = etsiFSK->nETSIAlertRing;
         pTapiCidConf->nETSIAlertNoRing = etsiFSK->nETSIAlertNoRing;
         pCIDTiming = etsiFSK->pCIDTiming;
         pFSKConf = etsiFSK->pFSKConf;
      }
      break;
   case IFX_TAPI_CID_STD_ETSI_DTMF:
      if (pCidConf->cfg != IFX_NULL)
      {
         IFX_TAPI_CID_STD_ETSI_DTMF_t *etsiDTMF = &cfg.etsiDTMF;
         TAPI_OS_CpyUsr2Kern (&cfg, pCidConf->cfg,
                              sizeof(IFX_TAPI_CID_STD_TYPE_t));
         pTapiCidConf->nAlertToneOnhook = etsiDTMF->nAlertToneOnhook;
         pTapiCidConf->nAlertToneOffhook = etsiDTMF->nAlertToneOffhook;
         pTapiCidConf->ringPulseOnTime = etsiDTMF->ringPulseTime;
         pTapiCidConf->ringPulseOffTime = etsiDTMF->ringPulseTime;
         pTapiCidConf->ackTone = etsiDTMF->ackTone;
         pTapiCidConf->nETSIAlertRing = etsiDTMF->nETSIAlertRing;
         pTapiCidConf->nETSIAlertNoRing = etsiDTMF->nETSIAlertNoRing;
         pDTMFConf   = etsiDTMF->pDTMFConf;
         pDTMFAbsCli = etsiDTMF->pABSCLICode;
         pCIDTiming  = etsiDTMF->pCIDTiming;
      }
      break;
   case IFX_TAPI_CID_STD_SIN:
      if (pCidConf->cfg != IFX_NULL)
      {
         IFX_TAPI_CID_STD_SIN_t *sin = &cfg.sin;
         TAPI_OS_CpyUsr2Kern (&cfg, pCidConf->cfg,
                              sizeof(IFX_TAPI_CID_STD_TYPE_t));
         pTapiCidConf->nAlertToneOnhook = sin->nAlertToneOnhook;
         pTapiCidConf->nAlertToneOffhook = sin->nAlertToneOffhook;
         pTapiCidConf->ackTone = sin->ackTone;
         pCIDTiming = sin->pCIDTiming;
         pFSKConf = sin->pFSKConf;
      }
      break;
   case IFX_TAPI_CID_STD_NTT:
      /* for NTT set the default loop count to 6 - leave other std untouched */
      pTapiCidConf->ringPulseLoop = 6;
      if (pCidConf->cfg != IFX_NULL)
      {
         IFX_TAPI_CID_STD_NTT_t *ntt = &cfg.ntt;
         TAPI_OS_CpyUsr2Kern (&cfg, pCidConf->cfg,
                              sizeof(IFX_TAPI_CID_STD_TYPE_t));
         pTapiCidConf->nAlertToneOnhook = ntt->nAlertToneOnhook;
         pTapiCidConf->nAlertToneOffhook = ntt->nAlertToneOffhook;
         pTapiCidConf->ringPulseOnTime = ntt->ringPulseTime;
         pTapiCidConf->ringPulseOffTime = ntt->ringPulseOffTime ?
            ntt->ringPulseOffTime : ntt->ringPulseTime;
         pTapiCidConf->ringPulseLoop = ntt->ringPulseLoop;
         pCIDTiming = ntt->pCIDTiming;
         if (ntt->ringPulseLoop == 0)
            ret = TAPI_statusErr;
         pTapiCidConf->ack2Timeout = ntt->dataOut2incomingSuccessfulTimeout ?
            ntt->dataOut2incomingSuccessfulTimeout : CID_NTT_ACK2_DEF_TIMEOUT;
         pFSKConf = ntt->pFSKConf;
      }
      break;
   case IFX_TAPI_CID_STD_KPN_DTMF:
      if (pCidConf->cfg != IFX_NULL)
      {
         IFX_TAPI_CID_STD_KPN_DTMF_t *kpn = &cfg.kpnDTMF;
         TAPI_OS_CpyUsr2Kern (&cfg, pCidConf->cfg,
                              sizeof(IFX_TAPI_CID_STD_TYPE_t));
         pTapiCidConf->nAlertToneOffhook = kpn->nAlertToneOffhook;
         pTapiCidConf->ackTone = kpn->ackTone;
         pCIDTiming = kpn->pCIDTiming;
         pDTMFConf   = kpn->pDTMFConf;
         pDTMFAbsCli = kpn->pABSCLICode;
         pFSKConf = kpn->pFSKConf;
      }
      if (IFX_NULL == pCIDTiming)
      {
         /* for KPN set the default time to wait after line reversal
            is 350 ms - leave other std untouched */
         pTapiCidConf->TapiCidTiming.afterLineReversal = 350;
      }

      pDefaultDtmfConf = &default_kpn_cid_dtmf_conf;
      break;
   case IFX_TAPI_CID_STD_KPN_DTMF_FSK:
      if (pCidConf->cfg != IFX_NULL)
      {
         IFX_TAPI_CID_STD_KPN_DTMF_FSK_t *kpn = &cfg.kpnDTMF_FSK;
         TAPI_OS_CpyUsr2Kern (&cfg, pCidConf->cfg,
                              sizeof(IFX_TAPI_CID_STD_TYPE_t));
         pTapiCidConf->nAlertToneOffhook = kpn->nAlertToneOffhook;
         pTapiCidConf->ackTone = kpn->ackTone;
         pCIDTiming = kpn->pCIDTiming;
         pDTMFConf   = kpn->pDTMFConf;
         pDTMFAbsCli = kpn->pABSCLICode;
         pFSKConf = kpn->pFSKConf;
      }
      if (IFX_NULL == pCIDTiming)
      {
         /* for KPN set the default time to wait after line reversal
            is 350 ms - leave other std untouched */
         pTapiCidConf->TapiCidTiming.afterLineReversal = 350;
      }

      pDefaultDtmfConf = &default_kpn_cid_dtmf_conf;
      break;
   case IFX_TAPI_CID_STD_TDC:
      if (pCidConf->cfg != IFX_NULL)
      {
         IFX_TAPI_CID_STD_TDC_t *tdc = &cfg.tdc;
         TAPI_OS_CpyUsr2Kern (&cfg, pCidConf->cfg,
                              sizeof(IFX_TAPI_CID_STD_TYPE_t));
         pDTMFConf   = tdc->pDTMFConf;
         pDTMFAbsCli = tdc->pABSCLICode;
         pCIDTiming  = tdc->pCIDTiming;
      }

      pDefaultDtmfConf = &default_tdc_cid_dtmf_conf;
      break;
   default:
      ret = TAPI_statusErr;
      break;
   }

   if (TAPI_SUCCESS(ret))
   {
      /* copy from application pointers, if necessary */
      if (IFX_NULL != pCIDTiming)
      {
         TAPI_OS_CpyUsr2Kern (&pTapiCidConf->TapiCidTiming,
                              pCIDTiming, sizeof(IFX_TAPI_CID_TIMING_t));
      }

      if (pFSKConf == IFX_NULL)
      {
         /* FSK configuration for NTT has specific settings for seizure and
            mark bits - decide depending on the standard if the application
            didn't provide coefficients. */
         if (pCidConf->nStandard != IFX_TAPI_CID_STD_NTT)
            pTapiCidConf->TapiCidFskConf = default_cid_fsk_conf;
         else
            pTapiCidConf->TapiCidFskConf = default_cid_fsk_ntt_conf;
      }
      else
         TAPI_OS_CpyUsr2Kern (&pTapiCidConf->TapiCidFskConf,
                              pFSKConf, sizeof(IFX_TAPI_CID_FSK_CFG_t));

      if (pDTMFAbsCli == IFX_NULL)
         ciddtmf_abscli_etsi(&pTapiCidConf->TapiCidDtmfAbsCli);
      else
         TAPI_OS_CpyUsr2Kern (&pTapiCidConf->TapiCidDtmfAbsCli,
                              pDTMFAbsCli, sizeof(IFX_TAPI_CID_ABS_REASON_t));

      if (pDTMFConf == IFX_NULL)
         pTapiCidConf->TapiCidDtmfConf = *pDefaultDtmfConf;
      else
         TAPI_OS_CpyUsr2Kern (&pTapiCidConf->TapiCidDtmfConf,
                              pDTMFConf, sizeof(IFX_TAPI_CID_DTMF_CFG_t));

      /* finally save the standard */
      pTapiCidConf->nStandard = pCidConf->nStandard;
   }

   /* set default alert tones if applicable */
   if (TAPI_SUCCESS(ret))
   {
      /* set onhook alert tone */
      if (pTapiCidConf->nAlertToneOnhook == 0)
      {
         pTapiCidConf->nAlertToneOnhook = TAPI_CID_ATONE_INDEXDEF_ONHOOK;
      }
      /* set offhook alert tone */
      if (pTapiCidConf->nAlertToneOffhook == 0)
      {
         if (pCidConf->nStandard == IFX_TAPI_CID_STD_NTT)
            pTapiCidConf->nAlertToneOffhook = TAPI_CID_ATONE_INDEXDEF_OFFHOOKNTT;
         else
            pTapiCidConf->nAlertToneOffhook = TAPI_CID_ATONE_INDEXDEF_OFFHOOK;
      }
   }

   /* end of protected area */
   TAPI_OS_MutexRelease (&pChannel->semTapiChDataLock);

   RETURN_STATUS (ret, 0);
}


/**
   Aborts a running CID sequence due to errors

   \param  pChannel     Pointer to TAPI_CHANNEL structure.
*/
IFX_void_t TAPI_Cid_Abort (TAPI_CHANNEL *pChannel)
{
   if ((pChannel != IFX_NULL) && (pChannel->pTapiCidData != IFX_NULL))
   {
      pChannel->pTapiCidData->TxData.bRingStart = IFX_FALSE;
      /** \todo send cid transmission runtime error here */
   }
}


/**
   Check whether a CID sequence is running

   \param  pChannel     Pointer to TAPI_CHANNEL structure.

   \return
   - IFX_TRUE if active
   - IFX_FALSE if inactive
*/
IFX_boolean_t TAPI_Cid_IsActive (TAPI_CHANNEL *pChannel)
{
   if ((pChannel == IFX_NULL) || (pChannel->pTapiCidData == IFX_NULL))
   {
      return IFX_FALSE;
   }

   return pChannel->pTapiCidData->TxData.bActive;
}


/**
   Check whether a CID sequence or info is programmed

   \param  pChannel     Pointer to TAPI_CHANNEL structure.

   return
   - IFX_TRUE if sequence is used
   - IFX_FALSE if tx info is used
*/
IFX_boolean_t TAPI_Cid_UseSequence (TAPI_CHANNEL *pChannel)
{
   if ((pChannel == IFX_NULL) || (pChannel->pTapiCidData == IFX_NULL))
   {
      return IFX_FALSE;
   }
   return pChannel->pTapiCidData->TxData.bUseSequence;
}

/**
   Start ringing using CID state machine

   \param  pChannel     Pointer to TAPI_CHANNEL structure.

   return
   - TAPI_statusOk         on success
   - TAPI_statusCIDActive  CID Tx already active
   - TAPI_statusCidSmStart Error detected during CIDSM starting
   - TAPI_statusLLNotSupp  Ringing do not supported by LL CID state machine
*/
IFX_int32_t TAPI_Cid_RingStart (TAPI_CHANNEL *pChannel)
{
   IFX_int32_t          ret = TAPI_statusOk;
   IFX_TAPI_DRV_CTX_t   *pDrvCtx = IFX_NULL;
   IFX_TAPI_CID_CONF_t  *pConfData = IFX_NULL;
   TAPI_CIDTX_DATA_t    *pTxData = IFX_NULL;
   IFX_TAPI_CID_SEQ_CONF_t cidSeqData;

   TAPI_ASSERT (pChannel);
   TAPI_ASSERT (pChannel->pTapiCidData);
   TAPI_ASSERT (pChannel->pTapiDevice);
   TAPI_ASSERT (pChannel->pTapiDevice->pDevDrvCtx);

   pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;

   if (!IFX_TAPI_PtrChk (pDrvCtx->SIG.CIDSM_Start))
      RETURN_STATUS (TAPI_statusLLNotSupp, 0);

   if (TAPI_Cid_IsActive (pChannel))
      RETURN_STATUS (TAPI_statusCIDActive, 0);

   pTxData = &pChannel->pTapiCidData->TxData;
   pConfData = &pChannel->pTapiCidData->ConfData;

   /* start ringing through Caller ID State Machine without data transferring */
   pConfData->bRingOnly    = IFX_TRUE;
   pTxData->bActive        = IFX_TRUE;
   /* flag that we use seq_tx which is used for sending the correct event */
   pTxData->bUseSequence   = IFX_TRUE;

   memset (&cidSeqData, 0, sizeof (IFX_TAPI_CID_SEQ_CONF_t));

   /* pass only these parameters to the LL-driver */
   cidSeqData.txHookMode   = pTxData->txHookMode;
   cidSeqData.pCidParam    = pTxData->cidBuf[pTxData->cidGenType].pBuf;
   cidSeqData.nCidParamLen = pTxData->cidBuf[pTxData->cidGenType].nLen;
   cidSeqData.nAlertType   = pTxData->nAlertType;
   cidSeqData.bRingStart   = pTxData->bRingStart;
   cidSeqData.pConfData    = pConfData;

   /* retrieve the cadence for from ring configuration */
   IFX_TAPI_Ring_CidCadencePrepare (pChannel, &cidSeqData);

   ret = pDrvCtx->SIG.CIDSM_Start(pChannel->pLLChannel, &cidSeqData);

   if (!TAPI_SUCCESS (ret))
   {
      pTxData->bActive = IFX_FALSE;
      /* errmsg: Starting CID state machine failed  */
      RETURN_STATUS (TAPI_statusCidSmStart, ret);
   }

   return TAPI_statusOk;
}

/* ============================= */
/* CID Receiver global functions */
/* ============================= */

/**
   Starts caller id FSK receiver.

   This functionality starts only the FSK receiver. For CID with DTMF data
   transmission please use the DTMF detector. This function does also not
   handle a CID sequence as the \ref TAPI_Phone_CID_Seq_Tx function does
   for sending. Any sequence has to be taken care of by the application.

   \param  pChannel     Pointer to TAPI channel structure.
   \param  pCidRxCfg    Pointer to the CID receiver configuration

   \return
   - TAPI_statusCIDStartRx - case of failure
   - TAPI_statusOk - otherwise
   - TAPI_statusInvalidCh: Cannel does not have the required resource

   \remarks
   The caller id status is updated after successful start at low level and the
   internal data buffer is reset.
*/
IFX_int32_t TAPI_Phone_CidRx_Start (TAPI_CHANNEL *pChannel,
                                    IFX_TAPI_CID_RX_CFG_t const *pCidRxCfg)
{
   IFX_TAPI_DRV_CTX_t *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   IFX_int32_t ret = TAPI_statusOk;

#ifdef TAPI_VERSION3
   /* check if channel has the required receiver */
   if (pChannel->nChannel >= pChannel->pTapiDevice->nResource.FSKRCount)
   {
      /* errmsg: Service not supported on called channel context */
      RETURN_STATUS (TAPI_statusInvalidCh, 0);
   }
#endif /* TAPI_VERSION3 */

   if (pCidRxCfg->hookMode != IFX_TAPI_CID_HM_ONHOOK &&
       pCidRxCfg->hookMode != IFX_TAPI_CID_HM_OFFHOOK)
   {
      RETURN_STATUS (TAPI_statusCIDStartRx, 0);
   }

   /* Flush the data fifo whenever called */
   IFX_Fifo_Clear (&(pChannel->pTapiCidData->RxData.TapiCidRxFifo));

   if (IFX_TAPI_PtrChk (pDrvCtx->SIG.CID_RX_Start))
   {
      IFX_TAPI_CID_RX_t cidRxData;

      cidRxData.nStandard  = pChannel->pTapiCidData->ConfData.nStandard;
      cidRxData.txHookMode = pCidRxCfg->hookMode;
      cidRxData.pFskConf   = &pChannel->pTapiCidData->ConfData.TapiCidFskConf;
      cidRxData.module = pCidRxCfg->module;
      ret = pDrvCtx->SIG.CID_RX_Start(pChannel->pLLChannel, &cidRxData);
   }
   else
   {
      ret = TAPI_statusCIDStartRx;
   }

   if (TAPI_SUCCESS (ret))
   {
      pChannel->pTapiCidData->RxData.pData = IFX_NULL;
      pChannel->pTapiCidData->RxData.stat.nStatus =IFX_TAPI_CID_RX_STATE_ACTIVE;
      pChannel->pTapiCidData->RxData.stat.nError = IFX_TAPI_CID_RX_ERROR_NONE;
   }

   RETURN_STATUS (ret, 0);
}


/**
   Stop caller id FSK receiver.

   \param  pChannel     Pointer to TAPI channel structure.
   \param  pCidRxCfg    Pointer to the CID receiver configuration

   \return
   - TAPI_statusInvalidCh Cannel does not have the required resource
   - TAPI_statusCIDStopRx CID Rx stopping failed
   - TAPI_statusLLNotSupp Service is not supported by the low level driver

   \remarks
   The caller id status is updated after successful start at low level.
*/
IFX_int32_t TAPI_Phone_CidRx_Stop (TAPI_CHANNEL *pChannel,
                                   IFX_TAPI_CID_RX_CFG_t const *pCidRxCfg)
{
   IFX_TAPI_DRV_CTX_t     *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   IFX_int32_t             ret = TAPI_statusOk,
                           retLL = TAPI_statusOk;

#ifdef TAPI_VERSION3
   /* check if channel has the required receiver */
   if (pChannel->nChannel >= pChannel->pTapiDevice->nResource.FSKRCount)
   {
      /* errmsg: Service not supported on called channel context */
      RETURN_STATUS (TAPI_statusInvalidCh, 0);
   }
#endif /* TAPI_VERSION3 */

   if (IFX_TAPI_PtrChk (pDrvCtx->SIG.CID_RX_Stop))
   {
      retLL = pDrvCtx->SIG.CID_RX_Stop (pChannel->pLLChannel,
                                        pCidRxCfg);

      if (!TAPI_SUCCESS (retLL))
      {
         /* errmsg: CID Rx stopping failed */
         ret = TAPI_statusCIDStopRx;
      }
   }
   else
   {
      /* errmsg: Service is not supported by the low level driver */
      ret = TAPI_statusLLNotSupp;
   }

   if (TAPI_SUCCESS (ret))
   {
      pChannel->pTapiCidData->RxData.stat.nStatus =
                                                IFX_TAPI_CID_RX_STATE_INACTIVE;
   }

   RETURN_STATUS (ret, retLL);
}


/**
   Get the CID data already collected.

   \param  pChannel     Pointer to TAPI channel structure.
   \param  pCidRxData   Pointer to \ref IFX_TAPI_CID_RX_DATA_t structure.

   \return
   - TAPI_statusOk If successful
   - TAPI_statusCIDRXNoDataAvailable No data available in CID receiver buffer
   - TAPI_statusInvalidCh: Cannel does not have the required resource

   \remarks
   In case the data collection is ongoing, reading from actual buffer should be
   protected against concurrent access. The low level call guarantees this.
\remarks
   When the fifo is empty TAPI_statusCIDRXNoDataAvailable is returned. This is
   not really an error. It is just an indication that no data was returned.
*/
IFX_int32_t TAPI_Phone_Get_CidRxData (TAPI_CHANNEL *pChannel,
                                      IFX_TAPI_CID_RX_DATA_t *pCidRxData)
{
   IFX_TAPI_DRV_CTX_t *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   IFX_TAPI_LL_DEV_t  *pLLDev  = pChannel->pTapiDevice->pLLDev;
   TAPI_CIDRX_DATA_t      *pCidRx = &pChannel->pTapiCidData->RxData;
   IFX_TAPI_CID_RX_DATA_t *pFifo;
   IFX_int32_t             ret;

#ifdef TAPI_VERSION3
   /* check if channel has the required receiver */
   if (pChannel->nChannel >= pChannel->pTapiDevice->nResource.FSKRCount)
   {
      /* errmsg: Service not supported on called channel context */
      RETURN_STATUS (TAPI_statusInvalidCh, 0);
   }
#endif /* TAPI_VERSION3 */

   /* errmsg: No data available in CID receiver buffer */
   ret = TAPI_statusCIDRXNoDataAvailable;

   /* protect access to FIFO, lock interrupts */
   if (IFX_TAPI_PtrChk (pDrvCtx->IRQ.LockDevice))
      pDrvCtx->IRQ.LockDevice (pLLDev);

   switch (pCidRx->stat.nStatus)
   {
   case IFX_TAPI_CID_RX_STATE_ONGOING:
      if (IFX_Fifo_isEmpty(&(pCidRx->TapiCidRxFifo)))
      {
         /* read from actual buffer */
         ret = ifx_tapi_cid_RxDataCollectedGet (pChannel, pCidRxData);
         break;
      }
      /* If the fifo is not empty take the data from the fifo first. */
      /* deliberately fallthrough to default case */
      /*lint -fallthrough*/
   default:
      /* Allow readout of data in all other states not handled above.
         When there is no data in the fifo TAPI_statusErr is returned. */
      /* read from fifo */
      pFifo = (IFX_TAPI_CID_RX_DATA_t *)(IFX_void_t *)
              IFX_Fifo_readElement(&(pCidRx->TapiCidRxFifo));
      if (pFifo != IFX_NULL)
      {
         /* copy data into the buffer provided by the caller */
         *pCidRxData = *pFifo;
         /* did we fetch the last element? */
         if (pFifo == pCidRx->pData)
         {
            /* after we took the last element from the data fifo clear the
               data storage and update the status */
            pCidRx->pData = IFX_NULL;
            pCidRx->stat.nError = IFX_TAPI_CID_RX_ERROR_NONE;
            /* do not change the status if it is inactive or ongoing */
            if (pCidRx->stat.nStatus == IFX_TAPI_CID_RX_STATE_DATA_READY)
            {
               /* set only to active if status is data ready */
               pCidRx->stat.nStatus = IFX_TAPI_CID_RX_STATE_ACTIVE;
            }
         }
         return TAPI_statusOk;
      }
      break;
   }

   if (IFX_TAPI_PtrChk (pDrvCtx->IRQ.UnlockDevice))
      pDrvCtx->IRQ.UnlockDevice (pLLDev);

   RETURN_STATUS (ret, 0);
}


/**
   Retrieves a buffer for caller id receiver data.

   This function should be called each time memory is needed for Cid data
   buffering

   \param  pChannel     Pointer to TAPI channel structure.
   \param  nLen         Number of bytes needed in the buffer.

   \return
   Buffer pointer or IFX_NULL if no more memory in the pool.
*/
IFX_TAPI_CID_RX_DATA_t *TAPI_Phone_GetCidRxBuf (TAPI_CHANNEL *pChannel,
                                                IFX_uint32_t nLen)
{
   TAPI_CIDRX_DATA_t *pRxData = &pChannel->pTapiCidData->RxData;
   IFX_TAPI_CID_RX_DATA_t *pData = pRxData->pData;

   /* as soon as a buffer is requested set the status to ongoing */
   pRxData->stat.nStatus = IFX_TAPI_CID_RX_STATE_ONGOING;

   /* get a new buffer if there is currently no buffer open or if
      the currently open buffer is short about to overflow. */
   if ((pData == IFX_NULL) ||
       (pData->nSize + nLen > IFX_TAPI_CID_RX_SIZE_MAX))
   {
      /* allocate a buffer from FIFO */
      pData = (IFX_TAPI_CID_RX_DATA_t *)(IFX_void_t *)
              IFX_Fifo_writeElement (&pRxData->TapiCidRxFifo);
      if (pData != IFX_NULL)
      {
         /* prepare buffer for reception */
         memset (pData, 0, sizeof (IFX_TAPI_CID_RX_DATA_t));
         pRxData->pData = pData;
      }
      else
      {
         /* no buffer left in fifo: report error because we will loose data */

         /* Reporting is done only the first time the error occurs. When the
            error occured before no new reporting is done. The error is cleared
            by either restarting the receiver or reading all buffers from the
            fifo. */
         if (pRxData->stat.nError != IFX_TAPI_CID_RX_ERROR_READ)
         {
            IFX_TAPI_EVENT_t tapiEvent;

            /* set read error which means that we loose data */
            pRxData->stat.nError = IFX_TAPI_CID_RX_ERROR_READ;

            /* report event IFX_TAPI_EVENT_CID_RX_ERROR_READ */
            memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
            tapiEvent.id = IFX_TAPI_EVENT_CID_RX_ERROR_READ;
            /* no direction in event - always local due to coding */
            IFX_TAPI_Event_Dispatch(pChannel,&tapiEvent);
         }
      }
   }

   return pData;
}

/**
   Returns the CID receiver status

   \param  pChannel     Pointer to TAPI channel structure.
   \param  pCidRxStatus Pointer to IFX_TAPI_CID_RX_STATUS_t structure.

   \return Return value as follows:
   - TAPI_statusOk if successful
   - TAPI_statusInvalidCh: Cannel does not have the required resource

   \remarks
   The status variable must be protected because of possible mutual access.
   This is no low level function because the status is stored in TAPI.
*/
IFX_int32_t TAPI_Phone_CidRx_Status (TAPI_CHANNEL *pChannel,
                                     IFX_TAPI_CID_RX_STATUS_t *pCidRxStatus)
{
   IFX_TAPI_DRV_CTX_t *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   IFX_TAPI_LL_DEV_t  *pLLDev  = pChannel->pTapiDevice->pLLDev;

#ifdef TAPI_VERSION3
   /* check if channel has the required receiver */
   if (pChannel->nChannel >= pChannel->pTapiDevice->nResource.FSKRCount)
   {
      /* errmsg: Service not supported on called channel context */
      RETURN_STATUS (TAPI_statusInvalidCh, 0);
   }
#endif /* TAPI_VERSION3 */

   /* protect access to status structure , lock interrupts */
   if (IFX_TAPI_PtrChk (pDrvCtx->IRQ.LockDevice))
      pDrvCtx->IRQ.LockDevice (pLLDev);

   *pCidRxStatus = pChannel->pTapiCidData->RxData.stat;

   if (IFX_TAPI_PtrChk (pDrvCtx->IRQ.UnlockDevice))
      pDrvCtx->IRQ.UnlockDevice (pLLDev);

   return TAPI_statusOk;
}


/**
   Get the cid data already collected in an ongoing collection.

   \param  pChannel     Pointer to TAPI channel structure.
   \param  pCidRxData   Pointer to \ref IFX_TAPI_CID_RX_DATA_t structure.

   \return
   - TAPI_statusCIDRXNoDataAvailable No data available in CID receiver buffer
   - TAPI_statusOk if successful

   \remarks
   In CID ongoing state, this function protects the reading of actual data
   buffer from interrupts, because the data reading is done on interrupt level
*/
static IFX_int32_t ifx_tapi_cid_RxDataCollectedGet (TAPI_CHANNEL *pChannel,
                                                    IFX_TAPI_CID_RX_DATA_t *pCidRxData)
{
   IFX_TAPI_DRV_CTX_t *pDrvCtx = pChannel->pTapiDevice->pDevDrvCtx;
   IFX_TAPI_LL_DEV_t  *pDev    = pChannel->pTapiDevice->pLLDev;
   IFX_TAPI_CID_RX_DATA_t *pData = pChannel->pTapiCidData->RxData.pData;
   IFX_int32_t ret;

   /* errmsg: No data available in CID receiver buffer */
   ret = TAPI_statusCIDRXNoDataAvailable;

   /* protect buffer access from mutual access, lock interrupts */
   if (IFX_TAPI_PtrChk (pDrvCtx->IRQ.LockDevice))
   {
      pDrvCtx->IRQ.LockDevice (pDev);
   }

   if (pData != NULL)
   {
      *pCidRxData = *pData;
      /* after reading the data clear the buffer again */
      memset (pData, 0, sizeof (IFX_TAPI_CID_RX_DATA_t));
      ret = TAPI_statusOk;
   }

   if (IFX_TAPI_PtrChk (pDrvCtx->IRQ.UnlockDevice))
   {
        pDrvCtx->IRQ.UnlockDevice (pDev);
   }

   return ret;
}

#endif /* (TAPI_CFG_FEATURES & TAPI_FEAT_CID) */
