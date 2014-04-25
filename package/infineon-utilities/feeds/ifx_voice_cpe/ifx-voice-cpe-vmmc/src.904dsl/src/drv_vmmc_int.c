/******************************************************************************

                              Copyright (c) 2009
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

 ******************************************************************************
   Module      : drv_vmmc_int.c
   Description : This file contains the implementation of the interrupt handler
   Remarks     : The implementation assumes that multiple instances of this
                 interrupt handler cannot preempt each other, i.e. if there
                 is more than one VMMC in your design all INCAIP2 are
                 expected to raise interrupts at the same priority level.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_api.h"

/* the following include is against the general concept of private data
   structures and should be considered as a temporary only. It's required to
   access pCh->pCOD->fw_cod_ch_speech.ENC in VMMC_Upstream_Callback */
#include "drv_vmmc_cod_priv.h"
/* the following include is against the general concept of private data
   structures and should be considered as a temporary only. It's required to
   access pCh->pSIG->fw_sig_dtmfrcv.IS in VMMC_VC_Callback */
/**\todo Remove also include of "drv_vmmc_api.h" in this file. External function
   should be put somewhere else */
#include "drv_vmmc_sig_priv.h"
#include "drv_vmmc_alm_priv.h"
#include "drv_mps_vmmc.h"
#include "drv_vmmc_api.h"
#include "drv_vmmc_alm.h"
#include "drv_vmmc_cod.h"
#include "drv_vmmc_sig.h"
#include "drv_vmmc_sig_cid.h"
#include "drv_vmmc_pcm.h"
#include "drv_vmmc_int.h"
#include "drv_vmmc_int_evt.h"
#include "drv_vmmc_res.h"
#include "drv_vmmc_init.h"
#include "drv_vmmc_announcements.h"

/* check for correct BSP version */
#ifndef MAX_EVENT_MSG_LENGTH
#error ATTENTION **************************************************************
#error this version requires an updated BSP/MPS driver supporting the evtMbx
#error in case of Danube this is version 3.0.5.4.2 or newer, 2007-06-27 ow
#error ATTENTION **************************************************************
#endif /* MAX_EVENT_MSG_LENGTH */

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

#ifndef MFTD_BSP
/* temporary used, till the BSP supports MFTD */
typedef struct   /**< Register structure for Voice Channel Status registers
                      MPS_RVCxSR, MPS_SVCxSR, MPS_CVCxSR and MPS_VCxENR   */
{
    u32 dtmfr_dt   : 1;
    u32 dtmfr_pdt  : 1;
    u32 dtmfr_dtc  : 4;
    u32 utgs2      : 1;
    u32 utgs1      : 1;
    u32 cptd       : 1;
    u32 rcv_ov     : 1;
    u32 dtmfg_buf  : 1;
    u32 dtmfg_req  : 1;
    u32 tg_inact   : 1;
    u32 cis_buf    : 1;
    u32 cis_req    : 1;
    u32 cis_inact  : 1;
    u32 mftd2      : 5;
    u32 mftd1      : 5;
    u32 lin_req    : 1;
    u32 epq_st     : 1;
    u32 vpou_st    : 1;
    u32 vpou_jbl   : 1;
    u32 vpou_jbh   : 1;
    u32 dec_chg    : 1;
} MPS_RVC0SR_bits_t;

typedef union
{
    u32             val;
    MPS_RVC0SR_bits_t fld;
} MPS_RVC0SR_t;
#endif /* MFTD_BSP */

/* ============================= */
/* Global function declarations  */
/* ============================= */

/* ============================= */
/* Local function declarations   */
/* ============================= */
static IFX_void_t VMMC_Ad_Callback(MbxEventRegs_s *pEvent);
static IFX_void_t VMMC_Ad0_Callback(VMMC_DEVICE *pDev, MbxEventRegs_s *pEvent);
extern IFX_void_t VMMC_Ad1_Callback(VMMC_DEVICE *pDev, MbxEventRegs_s *pEvent);
static IFX_void_t VMMC_VC_Callback(MbxEventRegs_s *pEvent);
static IFX_void_t VMMC_EvtMbx_Callback(u32 cookie,
                                       mps_event_msg *pEvtMsg);
static IFX_void_t VMMC_Upstream_Callback(mps_devices mpsChannel);

/* ============================= */
/* Local variable definitions    */
/* ============================= */

/* ============================= */
/* Global function definitions   */
/* ============================= */

/*******************************************************************************
Description :
   VMMC callback routine registration
Arguments   :
   mpsCh
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
*******************************************************************************/
IFX_return_t VMMC_Register_Callback(VMMC_DEVICE *pDev, IFX_int32_t mpsCh)
{
   IFX_return_t ret  = IFX_SUCCESS;
   MbxEventRegs_s mask;
   IFX_void_t (*event_callback)(MbxEventRegs_s *events);
   static int enabled = 0;

   /* register callback function. (command or voice stream) */
   /* num: 1 for command 2,3,4 for voice.
      dir: 1 for upstream and 2 for downstream.
      but, for downstream, it seems not to be used.*/

   ret = ifx_mps_register_data_callback((mps_devices)mpsCh, 1,
                                        VMMC_Upstream_Callback);

   if (ret != IFX_SUCCESS)
   {
      TRACE(VMMC, DBG_LEVEL_HIGH,
           ("INFO: [%d] Register data callback failed %d.\n",
            (mpsCh -1), ret));
   }

   if (mpsCh == command)
      event_callback = VMMC_Ad_Callback;
   else
      event_callback = VMMC_VC_Callback;

   ifx_mps_register_event_mbx_callback((IFX_uint32_t) pDev,
                                       VMMC_EvtMbx_Callback);
   /* prepare interrupt mask, these are mainly events related to mailbox handling */
   mask.MPS_Ad0Reg.val       = 0x0000CF68;
   mask.MPS_Ad1Reg.val       = 0x00000000;
   mask.MPS_VCStatReg[0].val = 0x00000000;
   mask.MPS_VCStatReg[1].val = 0x00000000;
   mask.MPS_VCStatReg[2].val = 0x00000000;
   mask.MPS_VCStatReg[3].val = 0x00000000;

   ret = ifx_mps_register_event_callback((mps_devices) mpsCh,
                                         (MbxEventRegs_s *) &mask,
                                          event_callback);
   if (ret != IFX_SUCCESS)
   {
      TRACE(VMMC, DBG_LEVEL_HIGH,
           ("INFO: [%d] Register event callback failed %d.\n", (mpsCh -1), ret));
   }
   if ((enabled == 0) && (ret == IFX_SUCCESS))
   {
      /* disable DD_MBX interrupt by default */
      mask.MPS_Ad0Reg.fld.dd_mbx = 0;

      /* enable interrupts according to the configured mask */
      ret = ifx_mps_event_activation((mps_devices) mpsCh, (MbxEventRegs_s *) &mask);
      if (ret != IFX_SUCCESS)
         TRACE (VMMC, DBG_LEVEL_HIGH,("INFO: [%d] Activating events failed %d.\n", (mpsCh -1), ret));

      enabled = 1;
   }

   return ret;
}

/*******************************************************************************
Description :
   VMMC callback routine de-registration
Arguments   :
   mpsCh
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
*******************************************************************************/
IFX_return_t VMMC_UnRegister_Callback(IFX_int32_t mpsCh)
{
   IFX_return_t ret =IFX_SUCCESS;

   ret = ifx_mps_unregister_data_callback((mps_devices) mpsCh, 1);
   if (ret != IFX_SUCCESS)
   {
      TRACE (VMMC, DBG_LEVEL_HIGH,
            ("INFO: [%d] Un-Register data callback failed %d.\n", (mpsCh -1), ret));
   }

   ret = ifx_mps_unregister_event_callback((mps_devices) mpsCh);
   if (ret != IFX_SUCCESS)
   {
      TRACE (VMMC, DBG_LEVEL_HIGH,
            ("INFO: [%d] Un-Register event callback failed %d.\n", (mpsCh -1), ret));
   }

   ret = ifx_mps_unregister_event_mbx_callback();
   if (ret != IFX_SUCCESS)
   {
      TRACE (VMMC, DBG_LEVEL_HIGH,
            ("INFO: [%d] Un-Register event mailbox callback failed %d.\n", (mpsCh -1), ret));
   }

   return ret;
}


/*******************************************************************************
Description:
   serves ALM and EDSP events
Arguments:
   *pDev    - pointer to the device structure
Return   :
   none
Remarks  :
*******************************************************************************/
static IFX_void_t VMMC_Ad_Callback(MbxEventRegs_s *pEvent)
{
   VMMC_DEVICE *pDev;

   VMMC_GetDevice (0, &pDev);

   if (pEvent->MPS_Ad0Reg.val)
      VMMC_Ad0_Callback(pDev, pEvent);
   if (pEvent->MPS_Ad1Reg.val)
      VMMC_Ad1_Callback(pDev, pEvent);
}

static IFX_void_t VMMC_Ad0_Callback(VMMC_DEVICE *pDev, MbxEventRegs_s *pEvent)
{
   MPS_Ad0Reg_u *pAd0reg = &(pEvent->MPS_Ad0Reg);
   IFX_TAPI_EVENT_t tapiEvent;

   if(pAd0reg->fld.mips_ol)
   {
      SYS_OLOAD_ACK_t mipsOvld;
      pDev->nMipsOl++;

      SET_DEV_ERROR(VMMC_ERR_MIPS_OL);
/*lint -e{525} */
#if (VMMC_CFG_FEATURES & VMMC_FEAT_FAX_T38)
{
      TAPI_CHANNEL *pChannel;
      IFX_uint32_t nCh;
      VMMC_CHANNEL *pCh;

      for (nCh = 0; nCh < pDev->caps.nALI; nCh++)
      {
         pCh = &pDev->pChannel[nCh];
         pChannel = (TAPI_CHANNEL *) pCh->pTapiCh;

         /* check status and do some actions */
         if (pCh->TapiFaxStatus.nStatus &
            (IFX_TAPI_FAX_T38_DP_ON | IFX_TAPI_FAX_T38_TX_ON))
         {
            /* transmission stopped, so set error and raise exception */
            /* Fill event structure. */
            memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
            tapiEvent.id = IFX_TAPI_EVENT_T38_ERROR_OVLD;
            IFX_TAPI_Event_Dispatch(pChannel,&tapiEvent);
            TRACE(VMMC,DBG_LEVEL_NORMAL,
                  ("VIN%d, IRQ: MIPS overload, Ch%d: FAX stopped\n",
                  pDev->nDevNr, nCh));
         }
      }
}
#endif
      memset(&mipsOvld, 0, sizeof(SYS_OLOAD_ACK_t));
      mipsOvld.RW = CMD_WRITE;
      mipsOvld.CMD = CMD_EOP;
      mipsOvld.MOD = MOD_SYSTEM;
      mipsOvld.ECMD = ECMD_MIPSOLACKN_CMD;
      mipsOvld.LENGTH = 0x4;
      mipsOvld.MIPS_OL = 1;

      CmdWriteIsr (pDev, (IFX_uint32_t*)&mipsOvld, mipsOvld.LENGTH);
      VMMC_EVAL_MOL(pDev);
   }

   if(pAd0reg->fld.cmd_err)
   {
      SYS_CERR_ACK_t cmdErrAck;
      memset(&cmdErrAck, 0, sizeof(SYS_CERR_ACK_t));
      cmdErrAck.MOD  = MOD_SYSTEM;
      cmdErrAck.CMD  = CMD_EOP;
      /* all other fields are 0, i.e. above memset is sufficient */
      /* cmdErrAck.ECMD = 0; */
      CmdWriteIsr(pDev, (IFX_uint32_t *) &cmdErrAck, 0);

      SET_DEV_ERROR(VMMC_ERR_CERR);
      pDev->bCmdReadError = IFX_TRUE;
      /* wake up sleeping process for further processing of received command */
      pDev->bCmdOutBoxData = IFX_TRUE;
      VMMC_OS_EventWakeUp (&pDev->mpsCmdWakeUp);

      /* call event dispatcher */
      memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
      tapiEvent.id = IFX_TAPI_EVENT_DEBUG_CERR;
      IFX_TAPI_Event_Dispatch(pDev->pChannel[0].pTapiCh, &tapiEvent);
   }
   if(pAd0reg->fld.data_err)
   {
      SYS_DERR_ACK_t dataErrAck;
      TRACE (VMMC, DBG_LEVEL_HIGH,("INT: Data error!\n"));
      /* Improve: Reset the data downstream mailbox pointers */

      /* Send the Data Error Acknowledge */
      memset(&dataErrAck, 0, sizeof(SYS_DERR_ACK_t));
      dataErrAck.RW = CMD_WRITE;
      dataErrAck.CMD = CMD_EOP;
      dataErrAck.MOD = MOD_SYSTEM;
      dataErrAck.ECMD = ECMD_DATAERR_ACK;
      dataErrAck.LENGTH = 0;

      CmdWriteIsr (pDev, (IFX_uint32_t*)&dataErrAck, dataErrAck.LENGTH);
   }
   if(pAd0reg->fld.wd_fail)
   {
      TRACE(VMMC, DBG_LEVEL_HIGH,
            ("VMMC%d: IRQ: EDSP watchdog failure\n", pDev->nDevNr));
      memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
      tapiEvent.id = IFX_TAPI_EVENT_FAULT_FW_WATCHDOG;
      IFX_TAPI_Event_Dispatch(pDev->pChannel[0].pTapiCh, &tapiEvent);
   }
   if(pAd0reg->fld.pcm_crash)
   {
      TRACE (VMMC, DBG_LEVEL_HIGH,("INT: PCM crash!\n"));
   }

#if (VMMC_CFG_FEATURES & VMMC_FEAT_HDLC)
   if(pAd0reg->fld.dd_mbx)
   {
      irq_VMMC_RES_HDLC_DD_MBX_Handler (pDev);
   }
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_HDLC) */

}

/*******************************************************************************
Description :
   handles ALM interrupts reported through register LINEx_INT
Arguments   :
   pCh         - pointer to the channel structure
   nLineX_Int  - value of LINEx_INT register
Return      :
   none
Remarks     :
   This function serves phone events interrupts. Therefore, in case of tapi,
   an instance to data channel structure must be used for operation on
   data channel variables.
*******************************************************************************/
IFX_void_t VMMC_Ad1_Callback(VMMC_DEVICE *pDev, MbxEventRegs_s *pEvent)
{
   VMMC_CHANNEL *pCh;
   IFX_TAPI_EVENT_t tapiEvent;
   /* Tapi (HL) channel context. */
   TAPI_CHANNEL *pChannel;

   MPS_Ad1Reg_u *pAd1reg = &(pEvent->MPS_Ad1Reg);

   if(pAd1reg->fld.hs_con)
   {
      TRACE (VMMC, DBG_LEVEL_HIGH,("Headset connected at interface 0\n"));
   }
   if(pAd1reg->fld.hs_dis)
   {
      TRACE (VMMC, DBG_LEVEL_HIGH,("Headset disconnected at interface 0\n"));
   }
   if(pAd1reg->fld.ltest0)
   {
      TRACE (VMMC, DBG_LEVEL_HIGH,("Line testing finished at interface 0\n"));
   }
   if(pAd1reg->fld.ltest1)
   {
      TRACE (VMMC, DBG_LEVEL_HIGH,("Line testing finished at interface 1\n"));
   }
   if((pAd1reg->fld.offhook0) ||
      (pAd1reg->fld.onhook0)  ||
      (pAd1reg->fld.offhook1) ||
      (pAd1reg->fld.onhook1)  ||
      (pAd1reg->fld.otemp0)   ||
      (pAd1reg->fld.otemp1)
      )
   {
      /* Alm channel 0 */
      if ((pAd1reg->fld.offhook0) || (pAd1reg->fld.onhook0) ||
          (pAd1reg->fld.otemp0) )
      {
         pCh = &pDev->pChannel[0];
      }
      else /* Alm channel 1 */
      {
         pCh = &pDev->pChannel[1];
      }
      /* Get tapi channel context. */
      pChannel = (TAPI_CHANNEL *) pCh->pTapiCh;


      if((pAd1reg->fld.offhook0) ||
         (pAd1reg->fld.onhook0)  ||
         (pAd1reg->fld.offhook1) ||
         (pAd1reg->fld.onhook1))
      {
         /* Fill event structure. */
         memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
         if ((pAd1reg->fld.offhook0)||(pAd1reg->fld.offhook1))
            tapiEvent.id = IFX_TAPI_EVENT_FXS_OFFHOOK_INT;
         else
            tapiEvent.id = IFX_TAPI_EVENT_FXS_ONHOOK_INT;

         IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
      }

      if(pAd1reg->fld.otemp0)
      {
         irq_VMMC_ALM_LineDisable (&pDev->pChannel[0]);

         /* Fill event structure. */
         memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
         tapiEvent.id = IFX_TAPI_EVENT_FAULT_LINE_OVERTEMP;
         tapiEvent.ch = 0;
         IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);

         TRACE(VMMC, DBG_LEVEL_HIGH,
              ("Vmmc%d, Ch%d: IRQ: WARNING: overtemperature,"
               " setting line to power down\n",
               pDev->nDevNr, (pCh->nChannel -1) ));
      }
      if(pAd1reg->fld.otemp1)
      {
         irq_VMMC_ALM_LineDisable (&pDev->pChannel[1]);

         /* Fill event structure. */
         memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
         tapiEvent.id = IFX_TAPI_EVENT_FAULT_LINE_OVERTEMP;
         tapiEvent.ch = 1;
         IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);

         TRACE(VMMC, DBG_LEVEL_HIGH,
              ("Vmmc%d, Ch%d: IRQ: WARNING: overtemperature,"
               " setting line to power down\n",
               pDev->nDevNr, (pCh->nChannel -1) ));
      }
   }
}

/*******************************************************************************
Description :
   handles EDSP interrupts reported through register EDSPx_INT1

Arguments   :
   pCh            - pointer to the channel structure
   nEDSPx_Int1    - value of EDSPx_INT1 register
   nEDSPx_Stat1   - value of EDSPx_STAT1 register

Return      :
   none
Remarks     :
*******************************************************************************/
static IFX_void_t VMMC_VC_Callback(MbxEventRegs_s *pEvent)
{
   IFX_TAPI_EVENT_t tapiEvent;
   /* Tapi (HL) channel context. */
   TAPI_CHANNEL *pChannel;
   IFX_uint8_t nKey = 0;
   VMMC_CHANNEL *pCh;
   IFX_uint32_t nCh = 99;
#ifdef MFTD_BSP
   MPS_VCStatReg_u *pVc_reg = IFX_NULL;
#else
   MPS_RVC0SR_t *pVc_reg = IFX_NULL;
#endif /* MFTD_BSP */
   VMMC_DEVICE *pDev;

   VMMC_GetDevice (0, &pDev);

   if (pEvent->MPS_VCStatReg[0].val)
   {
      nCh = 0;
#ifdef MFTD_BSP
      pVc_reg = &(pEvent->MPS_VCStatReg[0]);
#else
      pVc_reg = (MPS_RVC0SR_t *)&(pEvent->MPS_VCStatReg[0]);
#endif /* MFTD_BSP */
   }
   else if (pEvent->MPS_VCStatReg[1].val)
   {
      nCh = 1;
#ifdef MFTD_BSP
      pVc_reg = &(pEvent->MPS_VCStatReg[1]);
#else
      pVc_reg = (MPS_RVC0SR_t *)&(pEvent->MPS_VCStatReg[1]);
#endif /* MFTD_BSP */
   }
   else if (pEvent->MPS_VCStatReg[2].val)
   {
      nCh = 2;
#ifdef MFTD_BSP
      pVc_reg = &(pEvent->MPS_VCStatReg[2]);
#else
      pVc_reg = (MPS_RVC0SR_t *)&(pEvent->MPS_VCStatReg[2]);
#endif /* MFTD_BSP */
   }
   else if (pEvent->MPS_VCStatReg[3].val)
   {
      nCh = 3;
#ifdef MFTD_BSP
      pVc_reg = &(pEvent->MPS_VCStatReg[3]);
#else
      pVc_reg = (MPS_RVC0SR_t *)&(pEvent->MPS_VCStatReg[3]);
#endif /* MFTD_BSP */
   }
   else
   {
      TRACE(VMMC, DBG_LEVEL_HIGH,("MPS_VCStatReg[0]: 0x%08x\n", pEvent->MPS_VCStatReg[0].val));
      TRACE(VMMC, DBG_LEVEL_HIGH,("MPS_VCStatReg[1]: 0x%08x\n", pEvent->MPS_VCStatReg[1].val));
      TRACE(VMMC, DBG_LEVEL_HIGH,("MPS_VCStatReg[2]: 0x%08x\n", pEvent->MPS_VCStatReg[2].val));
      TRACE(VMMC, DBG_LEVEL_HIGH,("MPS_VCStatReg[3]: 0x%08x\n", pEvent->MPS_VCStatReg[3].val));
      TRACE(VMMC, DBG_LEVEL_HIGH,("Any other channel??\n"));
      return;
   }

   /* Get low level channel context. */
   pCh = &pDev->pChannel[nCh];
   /* Get tapi channel context. */
   pChannel = (TAPI_CHANNEL *) pCh->pTapiCh;

#ifdef TAPI_CID
   if(pVc_reg->fld.cis_buf)
   {
      TRACE (VMMC, DBG_LEVEL_HIGH,("Caller ID generator input buffer underflow\n"));
      CIDTX_SET_STATE(CID_TRANSMIT_ERR);
   }

   if(pVc_reg->fld.cis_inact)
   {
      /* Transition 1 -> 0 : Cid end of transmission and CIS disabled
       via autodeactivation if not offhook. */
      if ((pCh->pSIG != NULL) &&
          (pCh->pSIG->cidSend.nState != CID_OFFHOOK) &&
          (pCh->pSIG->cidSend.nState != CID_SETUP))
      {
         CIDTX_SET_STATE (CID_TRANSMIT_END);
         /* send event to inform that the data transmission has ended */
         memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
         tapiEvent.id = IFX_TAPI_EVENT_CID_TX_END;
         IFX_TAPI_Event_Dispatch(pChannel,&tapiEvent);
      }
      if ((pCh->pSIG != NULL) &&
          (pCh->pSIG->cidSend.nState != CID_SETUP))
      {
         VMMC_CidFskMachine(pCh);
      }
   }

   if(pVc_reg->fld.cis_req)
   {
      /* Execute CID FSK state machine */
      if ((pCh->pSIG != NULL) &&
          (pCh->pSIG->cidSend.nState != CID_SETUP))
      {
         VMMC_CidFskMachine(pCh);
      }
   }
#endif /* TAPI_CID */

   if(pVc_reg->fld.cptd)
   {
      memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
      tapiEvent.id = IFX_TAPI_EVENT_TONE_DET_CPT;
      IFX_TAPI_Event_Dispatch(pChannel,&tapiEvent);
   }
   if(pVc_reg->fld.dec_chg)
   {
      /* Note: This irq bit is overlayed with two functions depending on
               the operating mode of the coder channel (Fax/Voice) */
#if (VMMC_CFG_FEATURES & VMMC_FEAT_FAX_T38)
      /*  Fax datapump request */
      if (pCh->TapiFaxStatus.nStatus & IFX_TAPI_FAX_T38_DP_ON)
      {
         pCh->pTapiCh->bFaxDataRequest = IFX_TRUE;
         VMMC_OS_DrvSelectQueueWakeUp (&pChannel->wqWrite,
                                       VMMC_OS_DRV_SEL_WAKEUP_TYPE_WR);
         pCh->nFdpReq++;
      }
      else
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_FAX_T38) */
      {
         /* Fill event structure; acknowledge of this interrupt will be
            triggered by the event dispatcher. */
         memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
         tapiEvent.id = IFX_TAPI_EVENT_COD_DEC_CHG;
         IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
      }
   }
   if(pVc_reg->fld.dtmfg_buf)
   {
      irq_VMMC_SIG_DtmfOnUnderrun(pCh);
   }
   if(pVc_reg->fld.dtmfg_req)
   {
      irq_VMMC_SIG_DtmfOnRequest(pCh);
   }
   /* Valid DTMF key detected */
   if (pVc_reg->fld.dtmfr_dt)
   {
      /* Get DTMF KEY */
      nKey = (pVc_reg->fld.dtmfr_dtc) & 0xf;

      if (pCh->pSIG != IFX_NULL)
      {
         SIG_DTMFR_CTRL_t *pDtmfRcv = &pCh->pSIG->fw_sig_dtmfrcv;

         /* Fill event structure. */
         memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
         tapiEvent.id = IFX_TAPI_EVENT_DTMF_DIGIT;

         /* translate fw-coding to tapi enum coding */
         tapiEvent.data.dtmf.digit = irq_VMMC_SIG_DTMF_encode_fw2tapi(nKey);
         /* translate fw-coding to ascii charater */
         tapiEvent.data.dtmf.ascii =
            (IFX_uint8_t)irq_VMMC_SIG_DTMF_encode_fw2ascii(nKey);

         if (pDtmfRcv->IS == 0)
         {
            /* SIG1 of signalling channel is input to the signalling channel,
               which is by convention the output of the respective ALM channel */
            tapiEvent.data.dtmf.local = 1;
         }
         else
         {
            /* SIG2 of signalling channel is input to the signalling channel
               which is by convention the output of the respective COD channel */
            tapiEvent.data.dtmf.network = 1;
         }
         IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
      }
   }

   /* lin_req is overlaid with epq_trig */
   if(pVc_reg->fld.lin_req)
   {
      /*TRACE (VMMC, DBG_LEVEL_HIGH,("Trigger of the event play out unit ch=%d\n", pCh->nChannel-1));*/
   }
   if(pVc_reg->fld.epq_st)
   {
      /*TRACE (VMMC, DBG_LEVEL_HIGH,("Statistics of the event play out unit\n"));*/
   }

   if(pVc_reg->fld.rcv_ov)
   {
      /* Send the Status Error Acknowledge command to the Firmware */
      SYS_SERR_ACK_t statErrAck;

      memset (&statErrAck, 0, sizeof(SYS_SERR_ACK_t));

      statErrAck.RW = CMD_WRITE;
      statErrAck.CMD = CMD_EOP;
      statErrAck.MOD = MOD_SYSTEM;
      statErrAck.ECMD = ECMD_STAT_ERR_ACK;
      statErrAck.CHAN = nCh;
      statErrAck.LENGTH = 4;
      statErrAck.RCV_OV = 1;

      CmdWriteIsr (pDev, (IFX_uint32_t *)((IFX_void_t *) &statErrAck), statErrAck.LENGTH);

#if (VMMC_CFG_FEATURES & VMMC_FEAT_FAX_T38)
      if (pCh->TapiFaxStatus.nStatus & (IFX_TAPI_FAX_T38_DP_ON | IFX_TAPI_FAX_T38_TX_ON))
      {
         memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
         tapiEvent.id = IFX_TAPI_EVENT_T38_ERROR_DATA;

         IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);

         TRACE(VMMC, DBG_LEVEL_HIGH,
               ("Fax datapump Error (FDP_ERR), Channel %d\n", nCh));
      }
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_FAX_T38) */
   }
   if(pVc_reg->fld.tg_inact)
   {
      /* Nothing to do*/
   }
   if(pVc_reg->fld.utgs2)
   {
      if (TAPI_ToneState (pCh->pTapiCh, 1) == TAPI_CT_ACTIVE)
      {
         /* UTG2, resource is #1 */
         memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
         tapiEvent.id = IFX_TAPI_EVENT_TONE_GEN_END_RAW;
         /* value stores the utg resource number */
         tapiEvent.data.value = 1;

         IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
      }
   }
   if(pVc_reg->fld.utgs1)
   {
      /* UTG1, resource is #0 */
      if (TAPI_ToneState (pCh->pTapiCh, 0) == TAPI_CT_ACTIVE)
      {
         /* Fill event structure. */
         memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
         tapiEvent.id = IFX_TAPI_EVENT_TONE_GEN_END_RAW;
         /* value stores the utg resource number */
         tapiEvent.data.value = 0;

         IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
      }
   }
   if(pVc_reg->fld.vpou_jbh)
   {
      TRACE (VMMC, DBG_LEVEL_LOW,("Voice play out unit jitter buffer high limit\n"));
   }
   if(pVc_reg->fld.vpou_jbl)
   {
      TRACE (VMMC, DBG_LEVEL_LOW,("Voice play out unit jitter buffer low limit\n"));
   }
   if(pVc_reg->fld.vpou_st)
   {
      TRACE (VMMC, DBG_LEVEL_LOW,("Voice play out unit statistics\n"));
   }
   if (pVc_reg->fld.mftd2)
   {
      irq_VMMC_SIG_MFTD_Event (pCh, pVc_reg->fld.mftd2, IFX_TRUE);
   }
   if (pVc_reg->fld.mftd1)
   {
      irq_VMMC_SIG_MFTD_Event (pCh, pVc_reg->fld.mftd1, IFX_FALSE);
   }
}


/*******************************************************************************
Description :
   upstream packet handler (voice, fax and CID data)
Arguments   :
   mps_devices
Return      :
   IFX_SUCCESS or IFX_ERROR
Remarks     :
*******************************************************************************/
static IFX_void_t VMMC_Upstream_Callback(mps_devices mpsChannel)
{
   IFX_return_t ret = IFX_ERROR;
   IFX_uint16_t *pPacket = NULL;
#if (VMMC_CFG_FEATURES & VMMC_FEAT_KPI) || (defined TAPI_CID)
   IFX_boolean_t dropPacket = IFX_FALSE,
                 processed = IFX_FALSE;
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_KPI) || TAPI_CID */
   static IFX_uint16_t nFailCnt = 0;
   VMMC_DEVICE *pDev;

   VMMC_GetDevice (0, &pDev);

   if (mpsChannel == command)
   {
      /* wake up sleeping process for further processing of received command */
      pDev->bCmdOutBoxData = IFX_TRUE;
      VMMC_OS_EventWakeUp (&pDev->mpsCmdWakeUp);
   }
#if (VMMC_CFG_FEATURES & VMMC_FEAT_PACKET)
   else
   {
      VMMC_CHANNEL *pCh;
      IFX_uint8_t nCh = 0;
      mps_message rw;

      ret = ifx_mps_read_mailbox(mpsChannel, &rw);
      pPacket = (IFX_uint16_t *)KSEG0ADDR((IFX_int32_t)rw.pData);

      /* For mps driver, voice0 = 2
                         voice1 = 3
                         voice2 = 4
                         voice3 = 5
                         voice4 = 6.
                         voice5 = 7.
         It must be changed to TAPI channel number.
         (ch0 = 0, ch1 = 1 and so on.)
      */
      nCh = (IFX_uint8_t) mpsChannel - 2;

      /*if (rw.cmd_type == DAT_PAYL_PTR_MSG_HDLC_PACKET)*/
      LOG_RD_PKT((pDev->nDevNr), nCh, pPacket, rw.nDataBytes,
                 !ret ? ret : pDev->err);

      pCh = &pDev->pChannel[nCh];
      if ((pCh->pCOD == IFX_NULL || pCh->pCOD->fw_cod_ch_speech.ENC == 0) &&
          rw.cmd_type <= DAT_PAYL_PTR_MSG_EVENT_PACKET)
      {
         /* discard voice and event packets if coder is not running */
#ifdef USE_BUFFERPOOL
         IFX_TAPI_VoiceBufferPut((void *)pPacket);
#else
         kfree((void *)pPacket);
#endif
         IFX_TAPI_Stat_Add(pCh->pTapiCh, IFX_TAPI_STREAM_COD,
                           TAPI_STAT_COUNTER_EGRESS_DISCARDED, 1);
         return;
      }

#ifdef TAPI_CID
      /* handle CID packet */
      if (rw.cmd_type == DAT_PAYL_PTR_MSG_CID_DATA_PACKET)
      {
         /* process cid packet */
         irq_VMMC_SIG_CID_RX_Data_Collect (pCh->pTapiCh, pPacket, rw.nDataBytes);
         /* discard cid packet now that we processed it */
         dropPacket = IFX_TRUE;
         processed = IFX_TRUE;
      }
#endif /* TAPI_CID */

#if (VMMC_CFG_FEATURES & VMMC_FEAT_KPI)
      /* check if voice packets needs redirection to KPI */
      if ((rw.cmd_type == DAT_PAYL_PTR_MSG_VOICE_PACKET) ||
          (rw.cmd_type == DAT_PAYL_PTR_MSG_EVENT_PACKET) ||
          (rw.cmd_type == DAT_PAYL_PTR_MSG_FAX_T38_PACKET) )
      {
         /* if the KPI group and channel number is 0 packets will take the
            normal route to the application through the fifo below - the
            processed flag is not set in this case
            if the KPI channel number is set packets will be put into the
            KPI group fifo stored in the stream switch */
         if (IFX_TAPI_KPI_ChGet(pCh->pTapiCh, IFX_TAPI_KPI_STREAM_COD))
         {
            if ( irq_IFX_TAPI_KPI_PutToEgress(pCh->pTapiCh,
                                              IFX_TAPI_KPI_STREAM_COD,
                                              pPacket, rw.nDataBytes)
                 != IFX_SUCCESS )
            {
               dropPacket = IFX_TRUE;
               /* update statistic */
               IFX_TAPI_Stat_Add(pCh->pTapiCh, IFX_TAPI_STREAM_COD,
                                 TAPI_STAT_COUNTER_EGRESS_DISCARDED, 1);
            }
            processed = IFX_TRUE;
         }
      }
      else
      /* check if DECT packets need redirection to KPI */
      if (rw.cmd_type == DAT_PAYL_PTR_MSG_DECT_PACKET)
      {
         if (IFX_TAPI_KPI_ChGet(pCh->pTapiCh, IFX_TAPI_KPI_STREAM_DECT))
         {
            if (irq_IFX_TAPI_KPI_PutToEgress(
                   pCh->pTapiCh, IFX_TAPI_KPI_STREAM_DECT,
                   pPacket, rw.nDataBytes) != IFX_SUCCESS)
            {
               dropPacket = IFX_TRUE;
               /* update statistic */
               IFX_TAPI_Stat_Add(pCh->pTapiCh, IFX_TAPI_STREAM_DECT,
                                 TAPI_STAT_COUNTER_EGRESS_DISCARDED, 1);
            }
         }
         else
         {
            /* if the KPI group and channel number is 0 discard DECT packets */
            dropPacket = IFX_TRUE;
            /* update statistic */
            IFX_TAPI_Stat_Add(pCh->pTapiCh, IFX_TAPI_STREAM_DECT,
                              TAPI_STAT_COUNTER_EGRESS_DISCARDED, 1);
         }
         processed = IFX_TRUE;
      }
#if (VMMC_CFG_FEATURES & VMMC_FEAT_HDLC)
      else
      /* check if HDLC packets need redirection to KPI */
      if (rw.cmd_type == DAT_PAYL_PTR_MSG_HDLC_PACKET)
      {
         /* if the KPI group and channel number is 0 packets will take the
            normal route to the application through the fifo below - the
            processed flag is not set in this case
            if the KPI channel number is set packets will be put into the
            KPI group fifo stored in the stream switch */
         if (IFX_TAPI_KPI_ChGet(pCh->pTapiCh, IFX_TAPI_KPI_STREAM_HDLC))
         {
            if (irq_IFX_TAPI_KPI_PutToEgress (pCh->pTapiCh,
               IFX_TAPI_KPI_STREAM_HDLC, pPacket, rw.nDataBytes) != IFX_SUCCESS)
            {
               dropPacket = IFX_TRUE;
               /* update statistic */
               IFX_TAPI_Stat_Add(pCh->pTapiCh, IFX_TAPI_STREAM_HDLC,
                                 TAPI_STAT_COUNTER_EGRESS_DISCARDED, 1);
            }
            pCh->nNoKpiPathError = 0;
         }
         else
         {
            if (0 == (pCh->nNoKpiPathError % 100))
            {
               IFX_TAPI_EVENT_t tapiEvent;

               memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
               tapiEvent.ch = pCh->nChannel - 1;
               tapiEvent.id = IFX_TAPI_EVENT_FAULT_HDLC_NO_KPI_PATH;

               IFX_TAPI_Event_Dispatch(pCh->pTapiCh, &tapiEvent);
            }

            pCh->nNoKpiPathError++;

            dropPacket = IFX_TRUE;
            /* update statistic */
            IFX_TAPI_Stat_Add(pCh->pTapiCh, IFX_TAPI_STREAM_HDLC,
                              TAPI_STAT_COUNTER_EGRESS_DISCARDED, 1);
         }
         processed = IFX_TRUE;
      }
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_HDLC) */
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_KPI) */

#if (VMMC_CFG_FEATURES & VMMC_FEAT_KPI) || (defined TAPI_CID)
      /* if flag is set drop the data packet and exit from the processing */
      if (dropPacket == IFX_TRUE)
      {
#ifdef USE_BUFFERPOOL
         IFX_TAPI_VoiceBufferPut((void *)pPacket);
#else
         kfree((void *)pPacket);
#endif
         return;
      }
      if (processed == IFX_TRUE)
      {
         return;
      }
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_KPI) || (defined TAPI_CID) */

      /* sort the packets into the fifos towards application */

      if ((rw.cmd_type == DAT_PAYL_PTR_MSG_VOICE_PACKET) ||
          (rw.cmd_type == DAT_PAYL_PTR_MSG_EVENT_PACKET) ||
          (rw.cmd_type == DAT_PAYL_PTR_MSG_FAX_DATA_PACKET) ||
          (rw.cmd_type == DAT_PAYL_PTR_MSG_FAX_STATUS_PACKET) )
      {
         ret = IFX_TAPI_UpStreamFifo_Put(pCh->pTapiCh, IFX_TAPI_STREAM_COD,
                                        (IFX_void_t *)pPacket, rw.nDataBytes, 0);

         if (!(pCh->pTapiCh->nFlags & CF_NONBLOCK))
         {
            /* data available, wake up waiting upstream function */
            VMMC_OS_EventWakeUp (&pCh->pTapiCh->semReadBlock);
         }
         /* if a non-blocking read should be performed just wake up once */
         if (pCh->pTapiCh->nFlags & CF_NEED_WAKEUP)
         {
            pCh->pTapiCh->nFlags |= CF_WAKEUPSRC_STREAM;
            /* don't wake up any more */
            pCh->pTapiCh->nFlags &= ~CF_NEED_WAKEUP;
            VMMC_OS_DrvSelectQueueWakeUp (&pCh->pTapiCh->wqRead,
                                          VMMC_OS_DRV_SEL_WAKEUP_TYPE_RD);
         }

         if (ret != IFX_SUCCESS)
         {
            /* update statistic */
            IFX_TAPI_Stat_Add(pCh->pTapiCh, IFX_TAPI_STREAM_COD,
                              TAPI_STAT_COUNTER_EGRESS_CONGESTED, 1);
         }
      }

      if (IFX_ERROR == ret)
      {
         if (nFailCnt == 0)
         {
            TRACE (VMMC, DBG_LEVEL_HIGH,
                  ("INFO: Upstream packet fifo full[ch(%d)]!\n",
                  (pCh->nChannel-1)));
         }
         nFailCnt += 1;
         nFailCnt %= 500;
#ifdef USE_BUFFERPOOL
         IFX_TAPI_VoiceBufferPut((void *)pPacket);
#else
         kfree((void *)pPacket);
#endif
      }
      else
      {
         nFailCnt = 0;
      }
   }
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_PACKET) */
}


/**
   Event Mailbox Callback routine and Event Handler

   \param  cookie       32bit value that is a reference to the device struct.
   \param  pEvtMsg      Pointer to a single event message. The length can be
                        retrieved from the event message header.
*/
static IFX_void_t VMMC_EvtMbx_Callback(u32 cookie, mps_event_msg *pEvtMsg)
{
   VMMC_DEVICE     *pDev = (VMMC_DEVICE *) cookie;
   VMMC_EvtMsg_t   *pEvt = /*lint --e(826)*/ (VMMC_EvtMsg_t *) pEvtMsg;
   VMMC_CHANNEL    *pCh  = &pDev->pChannel[pEvt->evt_ali_ovt.CHAN];
   TAPI_CHANNEL    *pChannel = pCh->pTapiCh;
   IFX_TAPI_EVENT_t tapiEvent;

#if 0
   /* Modify for HDLC ready event */
   if (((pCh->pALM == IFX_NULL) || (pCh->pSIG == IFX_NULL) ||
       (pCh->pCOD == IFX_NULL) || (pCh->pCON == IFX_NULL) ||
       (pCh->pPCM == IFX_NULL)) && ((pCh->pPCM == IFX_NULL) || ((pEvt->val[0] & VMMC_EVT_ID_MASK) != VMMC_EVT_ID_PCM_HDLC_RDY)))
   {
      TRACE(VMMC, DBG_LEVEL_HIGH,
           ("VMMC channel uninitialised while receiving voice FW event 0x%08X\n"
            "  Channel=%d pALM=%p pSIG=%p pCOD=%p pPCM=%p pCON=%p\n",
            pEvt->val[0], pEvt->evt_ali_ovt.CHAN,
            pCh->pALM, pCh->pSIG, pCh->pCOD, pCh->pPCM, pCh->pCON));
   }
#endif

   /* prepare tapi event, channel information is the same for all events */
   memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
   tapiEvent.ch = pEvt->evt_ali_ovt.CHAN;

   LOG_RD_EVENT_MBX(pDev->nDevNr, pDev->nChannel,
                    &pEvt->val[0],
                    /* 0 - only CMD header (32 bit), 4 - 32 bits are following */
                    pEvt->evt_ali_ovt.LENGTH);

   switch (pEvt->val[0] & VMMC_EVT_ID_MASK)
   {
      /* ALI Overtemperature ************************************************ */
      case VMMC_EVT_ID_ALI_OVT:
         if (pCh->pALM != IFX_NULL)
         {
            irq_VMMC_ALM_LineDisable (&pDev->pChannel[pEvt->evt_ali_ovt.CHAN]);
         }

         tapiEvent.id = IFX_TAPI_EVENT_FAULT_LINE_OVERTEMP;
         IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);

         TRACE(VMMC, DBG_LEVEL_HIGH,
              ("Vmmc%d, Ch%d: IRQ: WARNING: overtemperature,"
               " setting line to power down\n",
               pDev->nDevNr, (pCh->nChannel -1) ));
         VMMC_EVAL_EVENT(pCh, tapiEvent);
         break;

      /* ALI RAW Hook event ************************************************* */
      case VMMC_EVT_ID_ALI_RAW_HOOK:
         if (pEvt->evt_ali_raw_hook.RON == EVT_ALI_RAW_HOOK_RON_ONHOOK)
            tapiEvent.id = IFX_TAPI_EVENT_FXS_ONHOOK_INT;
         else
            tapiEvent.id = IFX_TAPI_EVENT_FXS_OFFHOOK_INT;
         IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
         VMMC_EVAL_EVENT(pCh, tapiEvent);
         break;

      /* ALI LT END event *************************************************** */
      case VMMC_EVT_ID_ALI_LT_END:
         if (pCh->pALM != IFX_NULL)
         {
#if (VMMC_CFG_FEATURES & VMMC_FEAT_PHONE_DETECTION)
            if (pCh->pALM->eCapMeasState == VMMC_CapMeasState_InProgress)
            {
               tapiEvent.id = IFX_TAPI_EVENT_LINE_MEASURE_CAPACITANCE_RDY_INT;
            }
            else
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_PHONE_DETECTION) */
            {
               tapiEvent.id = IFX_TAPI_EVENT_LT_GR909_RDY;
            }
            IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);

            /* workaround to ensure we are in linemode disabled at the end
               of the measurement */
            irq_VMMC_ALM_LineDisable(&pDev->pChannel[pEvt->evt_ali_ovt.CHAN]);
            /* end of workaround */
#if 0
            /* correct the cached linemode inside the low level ch context */
            VMMC_ALM_CorrectLinemodeCache(pCh, VMMC_OPMOD_PDH);
#endif
         }
         VMMC_EVAL_EVENT(pCh, tapiEvent);
         break;

      /* ALI AR9DCC event *************************************************** */
      case VMMC_EVT_ID_ALI_AR9DCC:
         /* Ground fault is most important so we handle it first */
         if (pEvt->evt_ali_ar9dcc.EVT == EVT_ALI_AR9DCC_EVT_GF)
         {
            if (pCh->pALM != IFX_NULL)
            {
               /* Set the linemode to disabled. If the DCCtrl already did it
                  this disabled is the confirmation required by the DCCtrl.
                  If the DCCtrl did nothing this will switch off the line.
                  In both cases the line is then disabled and may be set to
                  other line states by the application. */
               irq_VMMC_ALM_LineDisable(&pDev->pChannel[pEvt->evt_ali_ar9dcc.CHAN]);
            }
            /* Inform the application through a TAPI event. */
            tapiEvent.id = IFX_TAPI_EVENT_FAULT_LINE_GK_HIGH;
            IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
         }
         else
         /* Ground key is second in importance */
         if (pEvt->evt_ali_ar9dcc.EVT == EVT_ALI_AR9DCC_EVT_GK)
         {
            /* Upon ground key we do nothing. Just a TAPI event is sent to
               the application. */
            tapiEvent.id = IFX_TAPI_EVENT_FAULT_LINE_GK_LOW;
            IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
         }
         else
         /* Ground fault end */
         if (pEvt->evt_ali_ar9dcc.EVT == EVT_ALI_AR9DCC_EVT_GF_FIN)
         {
            tapiEvent.id = IFX_TAPI_EVENT_FAULT_LINE_GK_HIGH_END;
            IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
         }
         else
         /* Ground key end */
         if (pEvt->evt_ali_ar9dcc.EVT == EVT_ALI_AR9DCC_EVT_GK_FIN)
         {
            tapiEvent.id = IFX_TAPI_EVENT_FAULT_LINE_GK_LOW_END;
            IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
         }
         else
         /* Overtemperature end */
         if (pEvt->evt_ali_ar9dcc.EVT == EVT_ALI_AR9DCC_EVT_OTEMP_FIN)
         {
            tapiEvent.id = IFX_TAPI_EVENT_FAULT_LINE_OVERTEMP_END;
            IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
         }
#if ((VMMC_CFG_FEATURES & VMMC_FEAT_CALIBRATION) || (VMMC_CFG_FEATURES & VMMC_FEAT_PHONE_DETECTION))
         else
         /* OPMODE changes */
         if ((pEvt->evt_ali_ar9dcc.EVT == EVT_ALI_AR9DCC_EVT_OPC) &&
             (pCh->pALM != IFX_NULL))
         {
#if (VMMC_CFG_FEATURES & VMMC_FEAT_CALIBRATION)
            /* Currently opmode changes are only enabled while calibration
               is activated. They are used to indicate the end of the FW
               calibration sequence. */
            if (pEvt->evt_ali_ar9dcc.OPMODE == VMMC_SDD_OPMODE_CALIBRATE)
            {
               /* Set flag so that below we can detect the end of calibration */
               pCh->pALM->bCalibrationRunning = IFX_TRUE;
            }

            /* Calibration ends with an opmode change */
            if ((pCh->pALM->bCalibrationRunning == IFX_TRUE) &&
                (pEvt->evt_ali_ar9dcc.OPMODE != VMMC_SDD_OPMODE_CALIBRATE))
            {
               pCh->pALM->bCalibrationRunning = IFX_FALSE;
               tapiEvent.id = IFX_TAPI_EVENT_CALIBRATION_END_INT;
               IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
            }
#endif /* VMMC_CFG_FEATURES & VMMC_FEAT_CALIBRATION */

#if (VMMC_CFG_FEATURES & VMMC_FEAT_PHONE_DETECTION)
            switch (pEvt->evt_ali_ar9dcc.OPMODE)
            {
               case VMMC_SDD_OPMODE_DISABLED:
                  if (pCh->pALM->eCapMeasState == VMMC_CapMeasState_Started)
                  {
                     /* C-measurement can be started only if line mode is PDH.
                        Here the TAPI HL is informed by an internal event that
                        the line is ready for C-measurement start. */
                     tapiEvent.id =
                        IFX_TAPI_EVENT_LINE_MEASURE_CAPACITANCE_START_INT;
                     IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
                  }
                  VMMC_ALM_CorrectLinemodeCache(pCh, VMMC_OPMOD_PDH);
                  break;
               /* for the following opmode changes just update command cache */
               case VMMC_SDD_OPMODE_GR909:
                  VMMC_ALM_CorrectLinemodeCache(pCh, VMMC_OPMOD_GR909);
                  break;
               case VMMC_SDD_OPMODE_ACTIVE:
                  VMMC_ALM_CorrectLinemodeCache(pCh, VMMC_OPMOD_ACT);
                  break;
               case VMMC_SDD_OPMODE_RING_BURST:
                  VMMC_ALM_CorrectLinemodeCache(pCh, VMMC_OPMOD_RING);
                  break;
               case VMMC_SDD_OPMODE_STANDBY:
                  VMMC_ALM_CorrectLinemodeCache(pCh, VMMC_OPMOD_ONHOOK);
                  break;
               case VMMC_SDD_OPMODE_FXO:
                  VMMC_ALM_CorrectLinemodeCache(pCh, VMMC_OPMOD_FXO);
                  break;
               case VMMC_SDD_OPMODE_CALIBRATE:
                  VMMC_ALM_CorrectLinemodeCache(pCh, VMMC_OPMOD_CALIBRATE);
                  break;
               default:
                  TRACE(VMMC, DBG_LEVEL_LOW,
                       ("VMMC unsupported opmode change dev%d ch%d mode%d\n",
                          pDev->nDevNr, (pCh->nChannel -1),
                          pEvt->evt_ali_ar9dcc.OPMODE));
                  break;
            } /* switch */
#endif /* VMMC_CFG_FEATURES & VMMC_FEAT_PHONE_DETECTION */
         }
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_CALIBRATION) */
         else
         /* SSI crash is unlikely to occur and interrupts the service.
            No fast response is possible or needed so handle it last. */
         if (pEvt->evt_ali_ar9dcc.EVT == EVT_ALI_AR9DCC_EVT_SSI_CRASH)
         {
            /* Here we do nothing.
               Just a TAPI event is sent to the application. */
            tapiEvent.id = IFX_TAPI_EVENT_FAULT_HW_SSI_ERR;
            IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
         }
#if (VMMC_CFG_FEATURES & VMMC_FEAT_METERING)
         else
         /* Metering pulse finished */
         if (pEvt->evt_ali_ar9dcc.EVT == EVT_ALI_AR9DCC_EVT_TTXF)
         {
            tapiEvent.id = IFX_TAPI_EVENT_METERING_END;
            IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
         }
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_METERING) */
         /* Note: other events reported with this message are not handled
            at the moment. */
         break;

      /* COD Decoder Change event ******************************************* */
      case VMMC_EVT_ID_COD_DEC_CHANGE:
         /* Fill event structure; acknowledge of this interrupt will be
            triggered by the event dispatcher. */
         if (pEvt->evt_cod_dec_change.DC)
         {
            tapiEvent.id = IFX_TAPI_EVENT_COD_DEC_CHG;
            tapiEvent.data.dec_chg.dec_type = VMMC_COD_trans_cod_fw2tapi(
                                              pEvt->evt_cod_dec_change.DEC);
            IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
         }
         break;

      /* COD Fax Data Pump event ******************************************** */
      case VMMC_EVT_ID_COD_FAX_REQ:
#if (VMMC_CFG_FEATURES & VMMC_FEAT_FAX_T38)
         if (pCh->TapiFaxStatus.nStatus & IFX_TAPI_FAX_T38_DP_ON)
         {
            IFX_boolean_t bReportEvt = IFX_TRUE;
            switch (pEvt->evt_cod_fax_req.FDP_EVT)
            {
               case EVT_COD_FAX_REQ_FDP_EVT_FDP_REQ:
                  pCh->pTapiCh->bFaxDataRequest = IFX_TRUE;
                  VMMC_OS_DrvSelectQueueWakeUp (&pChannel->wqWrite,
                                                VMMC_OS_DRV_SEL_WAKEUP_TYPE_WR);
                  pCh->nFdpReq++;
                  bReportEvt = IFX_FALSE;
                  break;
               case EVT_COD_FAX_REQ_FDP_EVT_MBSU:
                  tapiEvent.id = IFX_TAPI_EVENT_T38_ERROR_DATA;
                  tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_DATA_MBSU;
                  break;
               case EVT_COD_FAX_REQ_FDP_EVT_DBSO:
                  tapiEvent.id = IFX_TAPI_EVENT_T38_ERROR_DATA;
                  tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_DATA_DBSO;
                  break;
               case EVT_COD_FAX_REQ_FDP_EVT_MBDO:
                  tapiEvent.id = IFX_TAPI_EVENT_T38_ERROR_DATA;
                  tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_DATA_MBDO;
                  break;
               case EVT_COD_FAX_REQ_FDP_EVT_MBDU:
                  tapiEvent.id = IFX_TAPI_EVENT_T38_ERROR_DATA;
                  tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_DATA_MBDU;
                  break;
               case EVT_COD_FAX_REQ_FDP_EVT_DBDO:
                  tapiEvent.id = IFX_TAPI_EVENT_T38_ERROR_DATA;
                  tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_DATA_DBDO;
                  break;
               default:
                  /* unknown event type */
                  break;
            }
            if (bReportEvt)
            {
               IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
            }
         }
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_FAX_T38) */
         break;

      /* SIG UTG1 event ***************************************************** */
      case VMMC_EVT_ID_SIG_UTG1:
         if (TAPI_ToneState (pCh->pTapiCh, 0) == TAPI_CT_ACTIVE)
         {
            tapiEvent.id = IFX_TAPI_EVENT_TONE_GEN_END_RAW;
            /* value stores the utg resource number */
            tapiEvent.data.value = 0;
            IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
         }
         break;

      /* SIG UTG2 event ***************************************************** */
      case VMMC_EVT_ID_SIG_UTG2:
         if (TAPI_ToneState (pCh->pTapiCh, 1) == TAPI_CT_ACTIVE)
         {
            tapiEvent.id = IFX_TAPI_EVENT_TONE_GEN_END_RAW;
            /* value stores the utg resource number */
            tapiEvent.data.value = 1;
            IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
         }
         break;

      /* DECT UTG event ***************************************************** */
      case VMMC_EVT_ID_DECT_UTG:
         tapiEvent.id = IFX_TAPI_EVENT_TONE_GEN_END_RAW;
         /* value stores the utg resource number */
         tapiEvent.data.value = 2;
         IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
         break;

      /* SIG DTMF Generator event ******************************************* */
      case VMMC_EVT_ID_SIG_DTMFG:
         if (pCh->pSIG != IFX_NULL)
         {
            switch (pEvt->evt_sig_dtmfg.EVENT)
            {
               case EVT_SIG_DTMFG_EVENT_READY:
                  irq_VMMC_SIG_DtmfOnReady(pCh);
                  break;
               case EVT_SIG_DTMFG_EVENT_BUF_REQUEST:
                  irq_VMMC_SIG_DtmfOnRequest(pCh);
                  break;
               case EVT_SIG_DTMFG_EVENT_BUF_UNDERFLOW:
                  irq_VMMC_SIG_DtmfOnUnderrun(pCh);
                  break;
               default:
                  break;
            }
         }
         break;

      /* SIG CID sender event *********************************************** */
      case VMMC_EVT_ID_SIG_CIDS:
#ifdef TAPI_CID
         switch (pEvt->evt_sig_cids.EVENT)
         {
            case EVT_SIG_CIDS_EVENT_READY:
               /* CID end of transmission and CIS disabled
                  via autodeactivation if not offhook. */
               if ((pCh->pSIG != NULL) &&
                   (pCh->pSIG->cidSend.nState != CID_OFFHOOK) &&
                   (pCh->pSIG->cidSend.nState != CID_SETUP))
               {
                  CIDTX_SET_STATE (CID_TRANSMIT_END);
                  /* send event to inform that the data transmission has ended */
                  memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
                  tapiEvent.id = IFX_TAPI_EVENT_CID_TX_END;
                  IFX_TAPI_Event_Dispatch(pChannel,&tapiEvent);
               }
               if ((pCh->pSIG != NULL) &&
                   (pCh->pSIG->cidSend.nState != CID_SETUP))
               {
                  VMMC_CidFskMachine(pCh);
               }
               break;

            case EVT_SIG_CIDS_EVENT_BUF_REQUEST:
               /* trigger CID FSK state machine */
               if ((pCh->pSIG != NULL) &&
                   (pCh->pSIG->cidSend.nState != CID_SETUP))
               {
                  VMMC_CidFskMachine(pCh);
               }
               break;

            case EVT_SIG_CIDS_EVENT_BUF_UNDERFLOW:
               if (pCh->pSIG != NULL)
               {
                  CIDTX_SET_STATE(CID_TRANSMIT_ERR);
               }
               tapiEvent.id = IFX_TAPI_EVENT_CID_TX_UNDERRUN_ERR;
               IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
               break;

            default:
               break;
         }
#endif /* TAPI_CID */
         break;

      /* SIG DTMF detector event ******************************************** */
      case VMMC_EVT_ID_SIG_DTMFD:
         if ((IFX_uint32_t)(pCh->nChannel - 1) < pDev->caps.nDTMFD)
         {
            /* separate DTMF_START events from DTMF_END events */
            if (pEvt->evt_sig_dtmfd.EVT == EVT_SIG_DTMFD_EVT_DTMF_START)
            {
               tapiEvent.id = IFX_TAPI_EVENT_DTMF_DIGIT;
               /* translate fw-coding to tapi enum coding */
               tapiEvent.data.dtmf.digit =
                  irq_VMMC_SIG_DTMF_encode_fw2tapi(pEvt->evt_sig_dtmfd.DTMF);
               /* translate fw-coding to ascii charater */
               tapiEvent.data.dtmf.ascii =
                  (IFX_uint8_t)irq_VMMC_SIG_DTMF_encode_fw2ascii(
                     pEvt->evt_sig_dtmfd.DTMF);
               /* Remember the DTMF sign for DTMF end reporting below */
               if (pCh->pSIG != NULL)
               {
                  pCh->pSIG->nLastDtmfSign = pEvt->evt_sig_dtmfd.DTMF;
               }
            }
            else
            {
               tapiEvent.id = IFX_TAPI_EVENT_DTMF_END;
               /* The DTMF end event carries no DTMF sign so repeat the last
                  sign that was detected. If the variable does not exist the
                  event reports 0x00 which means "no digit". Because of this
                  keep the assignment to the event within this if-statement. */
               if (pCh->pSIG != NULL)
               {
                  pEvt->evt_sig_dtmfd.DTMF = pCh->pSIG->nLastDtmfSign;
                  /* translate fw-coding to tapi enum coding */
                  tapiEvent.data.dtmf.digit =
                     irq_VMMC_SIG_DTMF_encode_fw2tapi(pEvt->evt_sig_dtmfd.DTMF);
                  /* translate fw-coding to ascii charater */
                  tapiEvent.data.dtmf.ascii =
                     (IFX_uint8_t)irq_VMMC_SIG_DTMF_encode_fw2ascii(
                        pEvt->evt_sig_dtmfd.DTMF);
               }
            }

            if (pEvt->evt_sig_dtmfd.I == EVT_SIG_DTMFD_I_I1)
            {
               /* SIG1 of signalling channel is input to the signalling
                  channel, which is by convention the output of the
                  respective ALM channel */
               tapiEvent.data.dtmf.local = 1;
            }
            else
            {
               /* SIG2 of signalling channel is input to the signalling
                  channel which is by convention the output of the
                  respective COD channel */
               tapiEvent.data.dtmf.network = 1;
            }
            IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
            VMMC_EVAL_EVENT(pCh, tapiEvent);
         }
         break;

      /* SIG CPTD events **************************************************** */
      case VMMC_EVT_ID_SIG_CPTD:
         tapiEvent.id = IFX_TAPI_EVENT_TONE_DET_CPT;
         IFX_TAPI_Event_Dispatch(pChannel,&tapiEvent);
         break;

      /* SIG MFTD events **************************************************** */
      case VMMC_EVT_ID_SIG_MFTD:
         if ((pEvt->evt_sig_mftd.I1) && (pCh->pSIG != NULL))
            irq_VMMC_SIG_MFTD_Event (pCh, pEvt->evt_sig_mftd.MFTD1, IFX_FALSE);
         if ((pEvt->evt_sig_mftd.I2) && (pCh->pSIG != NULL))
            irq_VMMC_SIG_MFTD_Event (pCh, pEvt->evt_sig_mftd.MFTD2, IFX_TRUE);
         break;

      /* SIG RFC2833 event ************************************************** */
      case VMMC_EVT_ID_SIG_RFC2833DET:
         tapiEvent.id = IFX_TAPI_EVENT_RFC2833_EVENT;
         tapiEvent.data.rfc2833.event = pEvt->evt_sig_rfcdet.EVT;
         IFX_TAPI_Event_Dispatch(pChannel,&tapiEvent);
         break;

#if (VMMC_CFG_FEATURES & VMMC_FEAT_FAX_T38_FW)
      /* FAX event ********************************************************** */
      case VMMC_EVT_ID_COD_FAX_STATE:
         tapiEvent.id = IFX_TAPI_EVENT_T38_STATE_CHANGE;
         switch (pEvt->evt_cod_fax_state.T38_FAX_STATE)
         {
            case EVT_COD_FAX_STATE_ST_NEG:
               tapiEvent.data.t38 = IFX_TAPI_EVENT_T38_NEG;
               break;
            case EVT_COD_FAX_STATE_ST_MOD:
               tapiEvent.data.t38 = IFX_TAPI_EVENT_MOD;
               break;
            case EVT_COD_FAX_STATE_ST_DEM:
               tapiEvent.data.t38 = IFX_TAPI_EVENT_DEM;
               break;
            case EVT_COD_FAX_STATE_ST_TRANS:
               tapiEvent.data.t38 = IFX_TAPI_EVENT_TRANS;
               break;
            case EVT_COD_FAX_STATE_ST_PP:
               tapiEvent.data.t38 = IFX_TAPI_EVENT_PP;
               break;
            case EVT_COD_FAX_STATE_ST_INT:
               tapiEvent.data.t38 = IFX_TAPI_EVENT_INT;
               break;
            case EVT_COD_FAX_STATE_ST_DCN:
               tapiEvent.data.t38 = IFX_TAPI_EVENT_DCN;
               break;
            default:
               /* TODO */
               break;
         }
         IFX_TAPI_Event_Dispatch(pChannel,&tapiEvent);
    break;

      /* FAX channel event ************************************************** */
      case VMMC_EVT_ID_COD_FAX_ERR_EVT:
         tapiEvent.id = IFX_TAPI_EVENT_T38_ERROR_DATA;
         switch (pEvt->evt_cod_fax_err.FOIP_ERR)
         {
            case EVT_COD_FAX_ERR_MBSU:
               tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_DATA_MBSU;
               break;
            case EVT_COD_FAX_ERR_DBSO:
               tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_DATA_DBSO;
               break;
            case EVT_COD_FAX_ERR_MBDO:
               tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_DATA_MBDO;
               break;
            case EVT_COD_FAX_ERR_MBDU:
               tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_DATA_MBDU;
               break;
            case EVT_COD_FAX_ERR_DBDO:
               tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_DATA_DBDO;
               break;
            case EVT_COD_FAX_ERR_WPIP:
               tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_IP_PACKET;
               break;
            case EVT_COD_FAX_ERR_CBO:
               tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_CB_OVERFLOW;
               break;
            case EVT_COD_FAX_ERR_DBO:
               tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_DB_OVERFLOW;
               break;
            case EVT_COD_FAX_ERR_WRC:
               tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_BAD_CMD;
               break;
            case EVT_COD_FAX_ERR_AF1:
               tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_SESS_START;
               break;
            case EVT_COD_FAX_ERR_AF2:
               tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_SESS_STOP;
               break;
            case EVT_COD_FAX_ERR_TFF:
               tapiEvent.data.value = IFX_TAPI_EVENT_T38_ERROR_FLUSH_FIN;
               break;
            default:
               /* unknown event type */
               break;
         }
         IFX_TAPI_Event_Dispatch(pChannel,&tapiEvent);
         break;
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_FAX_T38_FW) */

      case VMMC_EVT_ID_COD_ANN_END:
#if (VMMC_CFG_FEATURES & VMMC_FEAT_ANNOUNCEMENTS)
         VMMC_AnnEndEventServe (pCh);

         tapiEvent.id = IFX_TAPI_EVENT_COD_ANNOUNCE_END;
         tapiEvent.data.announcement.nAnnIdx = pEvt->evt_cod_ann_end.ANNID;
         IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_ANNOUNCEMENTS) */
         break;

      case VMMC_EVT_ID_CPD_STAT_MOS:
         if (pCh->pCOD != NULL)
         {
            tapiEvent.id = IFX_TAPI_EVENT_COD_MOS;
            VMMC_OS_MutexGet(&pCh->chAcc);
            tapiEvent.data.mos.nR = pCh->pCOD->fw_cod_cfg_stat_mos.R_DEF;
            tapiEvent.data.mos.nCTI = pCh->pCOD->fw_cod_cfg_stat_mos.CTI;
            tapiEvent.data.mos.nAdvantage = pCh->pCOD->fw_cod_cfg_stat_mos.A_FACT;
            VMMC_OS_MutexRelease (&pCh->chAcc);
            tapiEvent.data.mos.nMOS = pEvt->evt_cpd_stat_mos.MOS_CQE;
            IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
         }
         break;

#if (VMMC_CFG_FEATURES & VMMC_FEAT_HDLC)
      /* HDLC events ******************************************************** */
      case VMMC_EVT_ID_PCM_HDLC_RDY:
         if (pEvt->evt_pcm_hdlc.TE)
         {
            irq_VMMC_PCM_HDLC_BufferReadySet(pCh);
         }
         if (pEvt->evt_pcm_hdlc.TO)
         {
            tapiEvent.id = IFX_TAPI_EVENT_FAULT_HDLC_TX_OVERFLOW;
            IFX_TAPI_Event_Dispatch(pChannel,&tapiEvent);
         }
         break;
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_HDLC) */

      /* FXO events ********************************************************* */
      case VMMC_EVT_ID_FXO:
         if (pCh->pALM != NULL)
         {
            switch (pEvt->evt_fxo.EVENT)
            {
               case EVT_FXO_RING_ON:
                  pCh->pALM->fxo_flags |= (1 << FXO_RING);
                  tapiEvent.id = IFX_TAPI_EVENT_FXO_RING_START;
                  break;
               case EVT_FXO_RING_OFF:
                  pCh->pALM->fxo_flags &= ~(1 << FXO_RING);
                  tapiEvent.id = IFX_TAPI_EVENT_FXO_RING_STOP;
                  break;
               case EVT_FXO_BATT_ON:
                  pCh->pALM->fxo_flags |= (1 << FXO_BATTERY);
                  tapiEvent.id = IFX_TAPI_EVENT_FXO_BAT_FEEDED;
                  break;
               case EVT_FXO_BATT_OFF:
                  pCh->pALM->fxo_flags &= ~(1 << FXO_BATTERY);
                  tapiEvent.id = IFX_TAPI_EVENT_FXO_BAT_DROPPED;
                  break;
               case EVT_FXO_OSI_END:
                  tapiEvent.id = IFX_TAPI_EVENT_FXO_OSI;
                  break;
               case EVT_FXO_APOH_ON:
                  pCh->pALM->fxo_flags |= (1 << FXO_APOH);
                  tapiEvent.id = IFX_TAPI_EVENT_FXO_APOH;
                  break;
               case EVT_FXO_APOH_OFF:
                  pCh->pALM->fxo_flags &= ~(1 << FXO_APOH);
                  tapiEvent.id = IFX_TAPI_EVENT_FXO_NOPOH;
                  break;
               case EVT_FXO_POLARITY_REVERSED:
                  pCh->pALM->fxo_flags &= ~(1 << FXO_POLARITY);
                  tapiEvent.id = IFX_TAPI_EVENT_FXO_POLARITY;
                  tapiEvent.data.fxo_polarity =
                     IFX_TAPI_EVENT_DATA_FXO_POLARITY_REVERSED;
                  break;
               case EVT_FXO_POLARITY_NORMAL:
                  pCh->pALM->fxo_flags |= (1 << FXO_POLARITY);
                  tapiEvent.id = IFX_TAPI_EVENT_FXO_POLARITY;
                  tapiEvent.data.fxo_polarity =
                     IFX_TAPI_EVENT_DATA_FXO_POLARITY_NORMAL;
                  break;
               default:
                  tapiEvent.id = IFX_TAPI_EVENT_FXO_NONE;
                  break;
            }
            IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);
         }
         break;

      /* the following events are _intentionally_ not handled at the moment */
      case VMMC_EVT_ID_SIG_RFC2833STAT:
         VMMC_EVAL_UNHANDLED_EVENT(pCh, pEvt->val[0] & VMMC_EVT_ID_MASK);
         break;
      case VMMC_EVT_ID_COD_VPOU_LIMIT:
         VMMC_EVAL_UNHANDLED_EVENT(pCh, pEvt->val[0] & VMMC_EVT_ID_MASK);
         break;
      case VMMC_EVT_ID_COD_VPOU_STAT:
         VMMC_EVAL_UNHANDLED_EVENT(pCh, pEvt->val[0] & VMMC_EVT_ID_MASK);
         break;
      case VMMC_EVT_ID_SYS_INT_ERR:
         /* VMMC_EVAL_UNHANDLED_EVENT(pCh, pEvt->val[0] & VMMC_EVT_ID_MASK); */
         break;
      case VMMC_EVT_ID_COD_LIN_REQ:
         break;
      case VMMC_EVT_ID_COD_LIN_UNDERFLOW:
         break;
      default:
         TRACE(VMMC, DBG_LEVEL_HIGH,
              ("VMMC unknown event id 0x%08X\n", pEvt->val[0]));
   }
}


#if defined(SYSTEM_AR9) || defined(SYSTEM_VR9)
/**
   Watchdog Timer callback routine. Called by MPS driver.
   \param flags    Zero for the moment. Reserved for future use.

   \return         success, cannot fail.
*/
IFX_int32_t VMMC_WDT_Callback (IFX_uint32_t flags)
{
   VMMC_DEVICE     *pDev;
   VMMC_CHANNEL    *pCh;
   TAPI_CHANNEL    *pChannel;
   IFX_TAPI_EVENT_t tapiEvent;

   VMMC_GetDevice (0, &pDev);

   /* this specific event is being reported on channel 0 */
   pCh = &pDev->pChannel[0];
   pChannel = pCh->pTapiCh;

   memset(&tapiEvent, 0, sizeof(IFX_TAPI_EVENT_t));
   tapiEvent.id = IFX_TAPI_EVENT_FAULT_FW_WATCHDOG;
   IFX_TAPI_Event_Dispatch(pChannel, &tapiEvent);

   return 0;
}
#endif /*SYSTEM_AR9 || SYSTEM_VR9*/

/*
   - improvement: implement cascaded event structure in VMMC_EvtMbx_Callback

   - mbx events are currently intentionally not enabled for
      - Voice Playout Unit event
      - Voice Playout Unit statistics change
      - RFC2833 statistics change
      - Linear Channel Data request

   - DTMFG READY evt, DTMF generation has finished, report? hl tapi trigger??
     currently disabled and skipped in interrupt routine
*/
