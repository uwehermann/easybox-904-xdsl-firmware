/******************************************************************************

                              Copyright (c) 2009
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

****************************************************************************
   Module      : drv_mps_vmmc_ar9.c
   Description : This file contains the implementation of the AR9/VR9 specific
                 driver functions.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_config.h"

#if defined(SYSTEM_AR9) || defined(SYSTEM_VR9)  /* defined in drv_config.h */

/* lib_ifxos headers */
#include "ifx_types.h"
#include "ifxos_linux_drv.h"
#include "ifxos_copy_user_space.h"
#include "ifxos_event.h"
#include "ifxos_lock.h"
#include "ifxos_select.h"
#include "ifxos_interrupt.h"

/* board specific headers */
#include <asm/ifx/ifx_regs.h>
#include <asm/ifx_vpe.h>
#include <asm/ifx/ifx_gpio.h>

/* device specific headers */
#include "drv_mps_vmmc.h"
#include "drv_mps_vmmc_dbg.h"
#include "drv_mps_vmmc_device.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
#define IFX_MPS_MODULE_ID       IFX_GPIO_MODULE_TAPI_VMMC
/* SmartSLIC reset - GPIO31: PORT1, PIN15 */
#if 0 // ctc merged
 #define SSLIC_RST_PORT    1
 #define SSLIC_RST_PIN     15
#else
 #define SSLIC_RST_PORT    2
 #define SSLIC_RST_PIN     5
#endif

/* Firmware watchdog timer counter address */
#define VPE1_WDOG_CTR_ADDR ((IFX_uint32_t)((IFX_uint8_t* )IFX_MPS_SRAM + 432))

/* Firmware watchdog timeout range, values in ms */
#define VPE1_WDOG_TMOUT_MIN 20
#define VPE1_WDOG_TMOUT_MAX 5000

/* ============================= */
/* Global variable definition    */
/* ============================= */
extern mps_comm_dev *pMPSDev;

/* ============================= */
/* Global function declaration   */
/* ============================= */
IFX_void_t ifx_mps_release (IFX_void_t);
extern IFX_uint32_t ifx_mps_reset_structures (mps_comm_dev * pMPSDev);
extern IFX_int32_t ifx_mps_bufman_close (IFX_void_t);
IFX_int32_t ifx_mps_wdog_callback (IFX_uint32_t wdog_cleared_ok_count);
extern IFXOS_event_t fw_ready_evt;
/* ============================= */
/* Local function declaration    */
/* ============================= */
static IFX_int32_t ifx_mps_fw_wdog_start_ar9(IFX_void_t);

/* ============================= */
/* Local variable definition     */
/* ============================= */
static IFX_int32_t vpe1_started = 0;
/* VMMC watchdog timer callback */
IFX_int32_t (*ifx_wdog_callback) (IFX_uint32_t flags) = IFX_NULL;

/* ============================= */
/* Local function definition     */
/* ============================= */

/******************************************************************************
 * AR9 Specific Routines
 ******************************************************************************/

/**
 * Start AR9 EDSP firmware watchdog mechanism.
 * Called after download and startup of VPE1.
 *
 * \param   none
 * \return  0         IFX_SUCCESS
 * \return  -1        IFX_ERROR
 * \ingroup Internal
 */
IFX_int32_t ifx_mps_fw_wdog_start_ar9()
{
  #if 0 // charles, ctc merged
	TRACE (MPS, DBG_LEVEL_HIGH,
	     (KERN_ERR "[%s %s %d]: ifx_mps_fw_wdog_start_ar9.\r\n",
	      __FILE__, __func__, __LINE__));
  #endif
   /* vpe1_wdog_ctr should be set up in u-boot as
      "vpe1_wdog_ctr_addr=0xBF2001B0"; protection from incorrect or missing
      setting */
   if (vpe1_wdog_ctr != VPE1_WDOG_CTR_ADDR)
   {
      vpe1_wdog_ctr = VPE1_WDOG_CTR_ADDR;
   }

   /* vpe1_wdog_timeout should be set up in u-boot as "vpe1_wdog_timeout =
      <value in ms>"; protection from insane setting */
   if (vpe1_wdog_timeout < VPE1_WDOG_TMOUT_MIN)
   {
      vpe1_wdog_timeout = VPE1_WDOG_TMOUT_MIN;
   }
   if (vpe1_wdog_timeout > VPE1_WDOG_TMOUT_MAX)
   {
      vpe1_wdog_timeout = VPE1_WDOG_TMOUT_MAX;
   }

   /* recalculate in jiffies */
   vpe1_wdog_timeout = vpe1_wdog_timeout * HZ / 1000;

   /* register BSP callback function */
   if (IFX_SUCCESS !=
       vpe1_sw_wdog_register_reset_handler (ifx_mps_wdog_callback))
   {
      TRACE (MPS, DBG_LEVEL_HIGH,
             (KERN_ERR "[%s %s %d]: Unable to register WDT callback.\r\n",
              __FILE__, __func__, __LINE__));
      return IFX_ERROR;;
   }

   /* start software watchdog timer */
   if (IFX_SUCCESS != vpe1_sw_wdog_start (0))
   {
      TRACE (MPS, DBG_LEVEL_HIGH,
             (KERN_ERR
              "[%s %s %d]: Error starting software watchdog timer.\r\n",
              __FILE__, __func__, __LINE__));
      return IFX_ERROR;
   }
   return IFX_SUCCESS;
}

/**
 * Firmware download to Voice CPU
 * This function performs a firmware download to the coprocessor.
 *
 * \param   pMBDev    Pointer to mailbox device structure
 * \param   pFWDwnld  Pointer to firmware structure
 * \return  0         IFX_SUCCESS, firmware ready
 * \return  -1        IFX_ERROR,   firmware not downloaded.
 * \ingroup Internal
 */
IFX_int32_t ifx_mps_download_firmware (mps_mbx_dev *pMBDev, mps_fw *pFWDwnld)
{
   IFX_uint32_t mem, cksum;
   IFX_uint8_t crc;
   IFX_boolean_t bMemReqNotPresent = IFX_FALSE;

   /* copy FW footer from user space */
   if (IFX_NULL == IFXOS_CpyFromUser(pFW_img_data,
                           pFWDwnld->data+pFWDwnld->length/4-sizeof(*pFW_img_data)/4,
                           sizeof(*pFW_img_data)))
   {
      TRACE (MPS, DBG_LEVEL_HIGH,
                  (KERN_ERR "[%s %s %d]: copy_from_user error\r\n",
                   __FILE__, __func__, __LINE__));
      return IFX_ERROR;
   }

   if(FW_FORMAT_NEW)
   {
      IFX_uint32_t plt = pFW_img_data->fw_vers >> 8 & 0xf;

      /* platform check */
      if (plt != FW_PLT_ARX && plt != FW_PLT_VRX)
      {
         TRACE (MPS, DBG_LEVEL_HIGH,("WRONG FIRMWARE PLATFORM!\n"));
         return IFX_ERROR;
      }
   }

   mem = pFW_img_data->mem;

   /* memory requirement sanity check */
   if ((crc = ~((mem >> 16) + (mem >> 8) + mem)) != (mem >> 24))
   {
      TRACE (MPS, DBG_LEVEL_HIGH,
          ("[%s %s %d]: warning, image does not contain size - assuming 1MB!\n",
           __FILE__, __func__, __LINE__));
      mem = 1 * 1024 * 1024;
      bMemReqNotPresent = IFX_TRUE;
   }
   else
   {
      mem &= 0x00FFFFFF;
   }

   /* check if FW image fits in available memory space */
   if (mem > vpe1_get_max_mem(0))
   {
      TRACE (MPS, DBG_LEVEL_HIGH,
      ("[%s %s %d]: error, firmware memory exceeds reserved space (%i > %i)!\n",
                 __FILE__, __func__, __LINE__, mem, vpe1_get_max_mem(0)));
      return IFX_ERROR;
   }

   /* reset the driver */
   ifx_mps_reset ();

   /* call BSP to get cpu1 base address */
   cpu1_base_addr = (IFX_uint32_t *)vpe1_get_load_addr(0);

   /* check if CPU1 base address is sane */
   if (cpu1_base_addr == IFX_NULL || !cpu1_base_addr)
   {
      TRACE (MPS, DBG_LEVEL_HIGH,
             (KERN_ERR "IFX_MPS: CPU1 base address is invalid!\r\n"));
      return IFX_ERROR;
   }
   else
   {
      /* check if CPU1 address is 1MB aligned */
      if ((IFX_uint32_t)cpu1_base_addr & 0xfffff)
      {
         TRACE (MPS, DBG_LEVEL_HIGH,
               (KERN_ERR "IFX_MPS: CPU1 base address is not 1MB aligned!\r\n"));
         return IFX_ERROR;
      }
   }

   /* further use uncached value */
   cpu1_base_addr = (IFX_uint32_t *)KSEG1ADDR(cpu1_base_addr);

   /* free all data buffers that might be currently used by FW */
   if (IFX_NULL != ifx_mps_bufman_freeall)
   {
      ifx_mps_bufman_freeall();
   }

   if(FW_FORMAT_NEW)
   {
      /* adjust download length */
      pFWDwnld->length -= (sizeof(*pFW_img_data)-sizeof(IFX_uint32_t));
   }
   else
   {
      pFWDwnld->length -= sizeof(IFX_uint32_t);

      /* handle unlikely case if FW image does not contain memory requirement -
         assumed for old format only */
      if (IFX_TRUE == bMemReqNotPresent)
         pFWDwnld->length += sizeof(IFX_uint32_t);

      /* in case of old FW format always assume that FW is encrypted;
         use compile switch USE_PLAIN_VOICE_FIRMWARE for plain FW */
#ifndef USE_PLAIN_VOICE_FIRMWARE
      pFW_img_data->enc = 1;
#else
#warning Using unencrypted firmware!
      pFW_img_data->enc = 0;
#endif /* USE_PLAIN_VOICE_FIRMWARE */
      /* initializations for the old format */
      pFW_img_data->st_addr_crc = 2*sizeof(IFX_uint32_t) +
                                  FW_AR9_OLD_FMT_XCPT_AREA_SZ;
      pFW_img_data->en_addr_crc = pFWDwnld->length;
      pFW_img_data->fw_vers = 0;
      pFW_img_data->magic = 0;
   }

   /* copy FW image to base address of CPU1 */
   if (IFX_NULL ==
       IFXOS_CpyFromUser ((IFX_void_t *)cpu1_base_addr,
                          (IFX_void_t *)pFWDwnld->data, pFWDwnld->length))
   {
      TRACE (MPS, DBG_LEVEL_HIGH,
             (KERN_ERR "[%s %s %d]: copy_from_user error\r\n", __FILE__,
              __func__, __LINE__));
      return IFX_ERROR;
   }

   /* process firmware decryption */
   if (pFW_img_data->enc == 1)
   {
      if(FW_FORMAT_NEW)
      {
         /* adjust decryption length (avoid decrypting CRC32 checksum) */
         pFWDwnld->length -= sizeof(IFX_uint32_t);
      }
      /* BootROM actually decrypts n+4 bytes if n bytes were passed for
         decryption. Subtract sizeof(u32) from length to avoid decryption
         of data beyond the FW image code */
      pFWDwnld->length -= sizeof(IFX_uint32_t);
      ifx_bsp_basic_mps_decrypt((IFX_uint32_t)cpu1_base_addr, pFWDwnld->length);
   }

   /* calculate CRC32 checksum over downloaded image */
   cksum = ifx_mps_fw_crc32(cpu1_base_addr, pFW_img_data);

   /* verify the checksum */
   if(FW_FORMAT_NEW)
   {
      if (cksum != pFW_img_data->crc32)
      {
         TRACE (MPS, DBG_LEVEL_HIGH,
                ("MPS: FW checksum error: img=0x%08x calc=0x%08x\r\n",
                pFW_img_data->crc32, cksum));
         return IFX_ERROR;
      }
   }
   else
   {
      /* just store self-calculated checksum */
      pFW_img_data->crc32 = cksum;
   }

   /* start VPE1 */
   ifx_mps_release ();

   /* start FW watchdog mechanism */
   ifx_mps_fw_wdog_start_ar9();

   /* get FW version */
   return ifx_mps_get_fw_version (0);
}


/**
 * Restart CPU1
 * This function restarts CPU1 by accessing the reset request register and
 * reinitializes the mailbox.
 *
 * \return  0        IFX_SUCCESS, successful restart
 * \return  -1       IFX_ERROR, if reset failed
 * \ingroup Internal
 */
IFX_int32_t ifx_mps_restart (IFX_void_t)
{
   /* raise reset request for CPU1 and reset driver structures */
   ifx_mps_reset ();
   /* Disable GPTC Interrupt to CPU1 */
   ifx_mps_shutdown_gpt ();
   /* re-configure GPTC */
   ifx_mps_init_gpt ();
   /* let CPU1 run */
   ifx_mps_release ();
   /* start FW watchdog mechanism */
   ifx_mps_fw_wdog_start_ar9();
   TRACE (MPS, DBG_LEVEL_HIGH, ("IFX_MPS: Restarting firmware..."));
   return ifx_mps_get_fw_version (0);
}

/**
 * Shutdown MPS - stop VPE1
 * This function stops VPE1
 *
 * \ingroup Internal
 */
IFX_void_t ifx_mps_shutdown (IFX_void_t)
{
   if (vpe1_started)
   {
      /* stop software watchdog timer */
      vpe1_sw_wdog_stop (0);
      /* clean up the BSP callback function */
      vpe1_sw_wdog_register_reset_handler (IFX_NULL);
      /* stop VPE1 */
      vpe1_sw_stop (0);
      vpe1_started = 0;
   }
   /* free GPTC */
   ifx_mps_shutdown_gpt ();
}

/**
 * Reset CPU1
 * This function causes a reset of CPU1 by clearing the CPU0 boot ready bit
 * in the reset request register RCU_RST_REQ.
 * It does not change the boot configuration registers for CPU0 or CPU1.
 *
 * \return  0        IFX_SUCCESS, cannot fail
 * \ingroup Internal
 */
IFX_void_t ifx_mps_reset (IFX_void_t)
{
   /* if VPE1 is already started, stop it */
   if (vpe1_started)
   {
      /* stop software watchdog timer first */
      vpe1_sw_wdog_stop (0);
      vpe1_sw_stop (0);
      vpe1_started = 0;
   }

   /* reset driver */
   ifx_mps_reset_structures (pMPSDev);
   ifx_mps_bufman_close ();
   return;
}

/**
 * Let CPU1 run
 * This function starts VPE1
 *
 * \return  none
 * \ingroup Internal
 */
IFX_void_t ifx_mps_release (IFX_void_t)
{
   IFX_int_t ret;
   IFX_int32_t RetCode = 0;

   /* Start VPE1 */
   if (IFX_SUCCESS !=
       vpe1_sw_start ((IFX_void_t *)cpu1_base_addr, 0, 0))
   {
      TRACE (MPS, DBG_LEVEL_HIGH, (KERN_ERR "Error starting VPE1\r\n"));
      return;
   }
   vpe1_started = 1;

   /* Sleep until FW is ready or a timeout after 3 seconds occured */
   ret = IFXOS_EventWait (&fw_ready_evt, 3000, &RetCode);
   if ((ret == IFX_ERROR) && (RetCode == 1))
   {
      /* timeout */
      TRACE (MPS, DBG_LEVEL_HIGH,
             (KERN_ERR "[%s %s %d]: Timeout waiting for firmware ready.\r\n",
              __FILE__, __func__, __LINE__));
      /* recalculate and compare the firmware checksum */
      ifx_mps_fw_crc_compare(cpu1_base_addr, pFW_img_data);
      /* dump exception area on a console */
      ifx_mps_dump_fw_xcpt(cpu1_base_addr, pFW_img_data);
   }
}

/**
 * WDT callback.
 * This function is called by BSP (module softdog_vpe) in case if software
 * watchdog timer expiration is detected by BSP.
 * This function needs to be registered at BSP as WDT callback using
 * vpe1_sw_wdog_register_reset_handler() API.
 *
 * \return  0        IFX_SUCCESS, cannot fail
 * \ingroup Internal
 */
IFX_int32_t ifx_mps_wdog_callback (IFX_uint32_t wdog_cleared_ok_count)
{
  #if 1 // charles, ctc merged
   IFXOS_INTSTAT flags;
  #else
   IFX_uint32_t flags;
  #endif
#ifdef DEBUG
   TRACE (MPS, DBG_LEVEL_HIGH,
          ("MPS: watchdog callback! arg=0x%08x\r\n", wdog_cleared_ok_count));
#endif /* DEBUG */

   /* reset SmartSLIC */
#if 1  /* Ignore reset SmartSLIC */
   printk("%s: Ignore SmartSLIC reset\n", __func__);
#else
   IFXOS_LOCKINT (flags);
   if (ifx_gpio_pin_reserve
       (IFX_GPIO_PIN_ID (SSLIC_RST_PORT, SSLIC_RST_PIN), IFX_MPS_MODULE_ID))
   {
      TRACE (MPS, DBG_LEVEL_HIGH,
             (KERN_ERR "[%s %s %d]: GPIO port/pin reservation error.\r\n",
              __FILE__, __func__, __LINE__));
   }
   /* P1_ALTSEL0.15 = 0 */
   if (ifx_gpio_altsel0_clear
       (IFX_GPIO_PIN_ID (SSLIC_RST_PORT, SSLIC_RST_PIN), IFX_MPS_MODULE_ID))
   {
      TRACE (MPS, DBG_LEVEL_HIGH,
             (KERN_ERR "[%s %s %d]: GPIO error clearing ALTSEL0.\r\n", __FILE__,
              __func__, __LINE__));
   }
   /* P1_ALTSEL1.15 = 0 */
   if (ifx_gpio_altsel1_clear
       (IFX_GPIO_PIN_ID (SSLIC_RST_PORT, SSLIC_RST_PIN), IFX_MPS_MODULE_ID))
   {
      TRACE (MPS, DBG_LEVEL_HIGH,
             (KERN_ERR "[%s %s %d]: GPIO error clearing ALTSEL1.\r\n", __FILE__,
              __func__, __LINE__));
   }
   /* P1_DIR.15 = 1 */
   if (ifx_gpio_dir_out_set
       (IFX_GPIO_PIN_ID (SSLIC_RST_PORT, SSLIC_RST_PIN), IFX_MPS_MODULE_ID))
   {
      TRACE (MPS, DBG_LEVEL_HIGH,
             (KERN_ERR "[%s %s %d]: GPIO error setting DIR.\r\n", __FILE__,
              __func__, __LINE__));
   }
   /* P1_OD.15 = 1 */
   if (ifx_gpio_open_drain_set
       (IFX_GPIO_PIN_ID (SSLIC_RST_PORT, SSLIC_RST_PIN), IFX_MPS_MODULE_ID))
   {
      TRACE (MPS, DBG_LEVEL_HIGH,
             (KERN_ERR "[%s %s %d]: GPIO error setting OD.\r\n", __FILE__,
              __func__, __LINE__));
   }
   /* P1_OUT.15 = 0 */
   if (ifx_gpio_output_clear
       (IFX_GPIO_PIN_ID (SSLIC_RST_PORT, SSLIC_RST_PIN), IFX_MPS_MODULE_ID))
   {
      TRACE (MPS, DBG_LEVEL_HIGH,
             (KERN_ERR "[%s %s %d]: GPIO error clearing OUT.\r\n", __FILE__,
              __func__, __LINE__));
   }
   if (ifx_gpio_pin_free
       (IFX_GPIO_PIN_ID (SSLIC_RST_PORT, SSLIC_RST_PIN), IFX_MPS_MODULE_ID))
   {
      TRACE (MPS, DBG_LEVEL_HIGH,
             (KERN_ERR "[%s %s %d]: GPIO port/pin freeing error.\r\n", __FILE__,
              __func__, __LINE__));
   }
   IFXOS_UNLOCKINT (flags);
#endif
   /* recalculate and compare the firmware checksum */
   ifx_mps_fw_crc_compare(cpu1_base_addr, pFW_img_data);

   /* dump exception area on a console */
   ifx_mps_dump_fw_xcpt(cpu1_base_addr, pFW_img_data);

   if (IFX_NULL != ifx_wdog_callback)
   {
      /* call VMMC driver */
      ifx_wdog_callback (wdog_cleared_ok_count);
   }
   else
   {
      TRACE (MPS, DBG_LEVEL_HIGH,
             (KERN_WARNING "MPS: VMMC watchdog timer callback is NULL.\r\n"));
   }
   return 0;
}

/**
 * Register WDT callback.
 * This function is called by VMMC driver to register its callback in
 * the MPS driver.
 *
 * \return  0        IFX_SUCCESS, cannot fail
 * \ingroup Internal
 */
IFX_int32_t
ifx_mps_register_wdog_callback (IFX_int32_t (*pfn) (IFX_uint32_t flags))
{
   ifx_wdog_callback = pfn;
   return 0;
}

#ifndef VMMC_WITH_MPS
EXPORT_SYMBOL (ifx_mps_register_wdog_callback);
#endif /* !VMMC_WITH_MPS */

#endif /* SYSTEM_AR9 || SYSTEM_VR9 */
