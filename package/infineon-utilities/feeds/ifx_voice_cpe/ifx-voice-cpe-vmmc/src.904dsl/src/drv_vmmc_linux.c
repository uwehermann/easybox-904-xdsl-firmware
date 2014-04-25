/******************************************************************************

                              Copyright (c) 2009
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

/**
   \file drv_vmmc_linux.c
   This file contains the implementation of the linux specific driver functions.
*/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_api.h"
#include "drv_vmmc_linux.h"

#ifdef LINUX
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/wait.h>
#include <linux/vmalloc.h>

#if 1 // charles, ctc merged
 #include <linux/sched.h>
 #include <linux/smp_lock.h>
#endif

#ifdef LINUX_2_6
#include <linux/version.h>
#ifndef UTS_RELEASE
#include <linux/utsrelease.h>
#endif /* UTC_RELEASE */
#undef CONFIG_DEVFS_FS
#endif /* LINUX_2_6 */

#include "drv_vmmc_init.h"
#ifdef EVALUATION
#include "drv_vmmc_eval.h"
#endif /* EVALUATION */
#ifdef PMC_SUPPORTED
#include "drv_vmmc_pmc.h"
#endif /* PMC_SUPPORTED */
#if (VMMC_CFG_FEATURES & VMMC_FEAT_HDLC)
#include "drv_vmmc_res.h"
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_HDLC) */

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0))

#ifndef wait_event_interruptible_timeout
#define __wait_event_interruptible_timeout(wq, condition, ret)                 \
do                                                                             \
{                                                                              \
   wait_queue_t __we;                                                          \
   init_waitqueue_entry(&__we, current);                                       \
                                                                               \
   add_wait_queue(&wq, &__we);                                                 \
   for (;;)                                                                    \
   {                                                                           \
      /* go halfway to sleep */                                                \
      set_current_state(TASK_INTERRUPTIBLE);                                   \
      /* check condition again */                                              \
      if (condition)                                                           \
         break;                                                                \
      ret = schedule_timeout(ret);                                             \
      /* timeout? then return */                                               \
      if (!ret)                                                                \
         break;                                                                \
   }                                                                           \
   /* restore the state in case we never fall asleep */                        \
   set_current_state(TASK_RUNNING);                                            \
   remove_wait_queue(&wq, &__we);                                              \
} while (0)

/**
   Linux 2.6 equivalent race condition free implmentation for
   IFXOS_WaitEvent_timeout.
*/
#define wait_event_interruptible_timeout(wq, condition, timeout)               \
({                                                                             \
   IFX_uint32_t __to = timeout;                                                \
   if (!(condition))                                                           \
      __wait_event_interruptible_timeout(wq, condition, __to);                 \
   __to;                                                                       \
})
#endif /* wait_event_interruptible_timeout */

#endif /* LINUX_2_6 */

/* ============================= */
/* Global variable declaration   */
/* ============================= */
IFX_uint16_t major      = VMMC_MAJOR;
IFX_uint16_t minorBase  = VMMC_MINOR_BASE;
IFX_char_t*  devName    = "vmmc";

#ifdef EVALUATION
extern IFX_sizedBuffer_t *VMMC_EvalSizedBuffer;
#endif /* #ifdef EVALUATION */

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */
/* operating system proc interface */
#ifdef VMMC_USE_PROC
static IFX_int32_t vmmc_proc_VersionGet (IFX_char_t *pBuf,
                                         IFX_int32_t nOffset,
                                         IFX_int32_t nLen);
static IFX_int32_t vmmc_proc_StatisticsGet (IFX_char_t *pBuf,
                                         IFX_int32_t nOffset,
                                         IFX_int32_t nLen);
#ifdef HAVE_CONFIG_H
static IFX_int32_t vmmc_proc_ConfigureGet (IFX_char_t *pBuf,
                                           IFX_int32_t nOffset,
                                           IFX_int32_t nLen);
#endif /* HAVE_CONFIG_H */
#if (VMMC_CFG_FEATURES & VMMC_FEAT_HDLC)
static IFX_int32_t vmmc_proc_HdlcInfoGet (IFX_char_t *pBuf,
                                          IFX_int32_t nOffset,
                                          IFX_int32_t nLen);
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_HDLC) */
#ifdef PMC_SUPPORTED
static int vmmc_proc_PmcGet (char *buf, char **start, off_t offset,
                             int count, int *eof, void *data);
#endif /* PMC_SUPPORTED */
static int vmmc_proc_Read (char *buf, char **start, off_t offset,
                           int len, int *eof, void *data);
static int vmmc_proc_EntriesInstall (void);
static void vmmc_proc_EntriesRemove(void);
#endif /* VMMC_USE_PROC */

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

/* ============================= */
/* Global function definition    */
/* ============================= */

/**
   Print a character string to a terminal for debugging.

   \param  pText        String to be logged.
*/
/*lint -esym(714, Drv_Log) -esym(765, Drv_Log) */
IFX_void_t Drv_Log(const IFX_char_t *pText)
{
   printk (pText);
}


/**
   This function allocates a temporary kernel buffer
      and copies the user buffer contents to it.

   \param  p_buffer     Pointer to the user buffer.
   \param  size         Size of the buffer.

   \return
   Pointer to kernel buffer or NULL.

   \remarks
   Buffer has to be released with \ref OS_UnmapBuffer.
*/
IFX_void_t* VMMC_OS_MapBuffer(IFX_void_t* p_buffer, IFX_uint32_t size)
{
   IFX_void_t* kernel_buffer;

   if (p_buffer == NULL)
   {
      return NULL;
   }
   kernel_buffer = vmalloc (size);
   if (kernel_buffer == NULL)
   {
      return NULL;
   }
   if (copy_from_user(kernel_buffer, p_buffer, size) > 0) /*lint !e506 !e774 */
   {
      vfree (kernel_buffer);
      return NULL;
   }

   return kernel_buffer;
}


/**
   This function releases a temporary kernel buffer.

   \param  p_buffer     Pointer to the kernel buffer.

   \remarks
   Counterpart of \ref OS_MapBuffer.
*/
IFX_void_t VMMC_OS_UnmapBuffer(IFX_void_t* p_buffer)
{
   if (p_buffer != NULL)
   {
      vfree (p_buffer);
      p_buffer = NULL;
   }
}


#ifdef VMMC_USE_PROC
/* ============================= */
/* /proc filesystem support      */
/* ============================= */


/**
   Read the statistics from the driver.

   \param  pBuf   Destination buffer.
   \param  nLen   Buffer length.

   \return
   Length of the string returned in buf.
*/
static IFX_int32_t vmmc_proc_StatisticsGet (IFX_char_t *pBuf,
                                         IFX_int32_t nOffset,
                                         IFX_int32_t nLen)
{
   IFX_int32_t  nOutLen = 0;
   IFX_uint16_t nDev = 0;

   if (nOffset != 0)
      return 0;

   nOutLen += snprintf (pBuf + nOutLen, (size_t)(nLen - nOutLen),
                        "%4s %4s\n",
                        "dev", "mol");

   for (nDev = 0; nDev < VMMC_MAX_DEVICES; nDev++)
   {
      VMMC_DEVICE *pDev = IFX_NULL;

      if (VMMC_GetDevice (nDev, &pDev) == IFX_SUCCESS)
      {
         nOutLen += snprintf (pBuf + nOutLen, (size_t)(nLen - nOutLen),
                              "%4d %4d\n",
                              pDev->nDevNr, pDev->nMipsOl);
      }
   }
   return nOutLen;
}

/**
   Read the version information from the driver.

   \param  pBuf   Destination buffer.
   \param  nLen   Buffer length.

   \return
   Length of the string returned in buf.
*/
static IFX_int32_t vmmc_proc_VersionGet (IFX_char_t *pBuf,
                                         IFX_int32_t nOffset,
                                         IFX_int32_t nLen)
{
   IFX_int32_t nOutLen = 0;

   if (nOffset != 0)
      return 0;

   nOutLen += snprintf (pBuf + nOutLen, (size_t)(nLen - nOutLen), "%s\n",
                        &DRV_VMMC_WHATVERSION[4]);
   nOutLen += snprintf (pBuf + nOutLen, (size_t)(nLen - nOutLen),
                        "Compiled on %s, %s for Linux kernel %s\n",
                        __DATE__, __TIME__, UTS_RELEASE);
   return nOutLen;
}


#ifdef HAVE_CONFIG_H
/**
   Read the configure parameters of the driver.

   \param  pBuf   Destination buffer.
   \param  nLen   Buffer length.

   \return
   Length of the string returned in buf.
*/
static IFX_int32_t vmmc_proc_ConfigureGet (IFX_char_t *pBuf,
                                           IFX_int32_t nOffset,
                                           IFX_int32_t nLen)
{
   IFX_int32_t nOutLen = 0;

   if (nOffset != 0)
      return 0;

   nOutLen += snprintf (pBuf + nOutLen, (size_t)(nLen - nOutLen),
                        "configure %s\n", &DRV_VMMC_WHICHCONFIG[0]);

   return nOutLen;
}
#endif /* HAVE_CONFIG_H */


#if (VMMC_CFG_FEATURES & VMMC_FEAT_HDLC)
/**
   Read the HDLC information of the driver.

   \param  pBuf   Destination buffer.
   \param  nLen   Buffer length.

   \return
   Length of the string returned in buf.
*/
static IFX_int32_t vmmc_proc_HdlcInfoGet (IFX_char_t *pBuf,
                                          IFX_int32_t nOffset,
                                          IFX_int32_t nLen)
{
   int nOutLen = 0;
   IFX_uint16_t nDev = 0;

   if (nOffset != 0)
      return 0;

   nOutLen += snprintf (pBuf + nOutLen, (size_t)(nLen - nOutLen),
                        "%3s %3s %3s "
                        "%5s "
                        "%6s "
                        "%5s "
                        "%s\n",
                        "Dev", "Ch", "Res",
                        "state",
                        "DD_MBX",
                        "fwbuf",
                        "fifo");

   for (nDev = 0; nDev < VMMC_MAX_DEVICES; nDev++)
   {
      VMMC_DEVICE *pDev = IFX_NULL;

      if (VMMC_GetDevice (nDev, &pDev) == IFX_SUCCESS)
      {
         IFX_uint16_t nResNr = 0;

         for (nResNr = 1; nResNr <= pDev->caps.nHDLC; nResNr++)
         {

            nOutLen += VMMC_RES_HDLC_InfoGet (pDev, nResNr,
                                              pBuf + nOutLen,
                                              nLen - nOutLen);
         }
      }
   }

   return nOutLen;
}
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_HDLC) */


#ifdef PMC_SUPPORTED
/**
   Read the Power Management Control status from the driver.

   \param   buf     Buffer to fill in the data.
   \param   start   Returns the increment to the offset for the next call.
   \param   offset  Offset where to begin output.
   \param   count   Number of characters in buffer.
   \param   eof     Return if eof was encountered.
   \param   data    Unused.
   \return  count   Number of returned characters.
*/
/*lint -e{715} yes - count and data are unused */
static int vmmc_proc_PmcGet (char *buf, char **start, off_t offset,
                             int count, int *eof, void *data)
{
   IFX_uint16_t       nDev, nCh;
   int len=0;

   /* Print status from all devices */
   for (nDev=0; nDev < VMMC_MAX_DEVICES; nDev++)
   {
      VMMC_DEVICE *pDev;

      if (VMMC_GetDevice (nDev, &pDev) == IFX_SUCCESS)
      {
         if (offset != 0)
         {
            offset--;
         }
         else
         {
            /* Headline */
            len += sprintf(buf+len, "-- Device: #%d %s --\n",
                           nDev, pDev->bFwActive ? "busy" : "idle");

            /* Loop over all channels */
            for (nCh=0; nCh < VMMC_MAX_CH_NR; nCh++)
            {
               if ((pDev->pPMC != IFX_NULL) &&
                   (pDev->pPMC[nCh].value != 0))
               {
                  len += sprintf(buf+len, "CH %d: ", nCh);

                  len += sprintf(buf+len, "%s%s%s%s%s%s%s%s%s%s%s%s",
                     pDev->pPMC[nCh].bits.pcm_ch ? "PCM CH, " : "",
                     pDev->pPMC[nCh].bits.pcm_lec ? "PCM LEC, " : "",
                     pDev->pPMC[nCh].bits.pcm_es ? "PCM ES, " : "",
                     pDev->pPMC[nCh].bits.pcm_lb ? "PCM LB, " : "",
                     pDev->pPMC[nCh].bits.hdlc_ch ? "HDLC CH, " : "",
                     pDev->pPMC[nCh].bits.alm_lec ? "ALM LEC, " : "",
                     pDev->pPMC[nCh].bits.alm_es ? "ALM ES, " : "",
                     pDev->pPMC[nCh].bits.cod_ch ? "COD CH, " : "",
                     pDev->pPMC[nCh].bits.cod_agc ? "COD AGC, " : "",
                     pDev->pPMC[nCh].bits.cod_fdp ? "COD FAX, " : "",
                     pDev->pPMC[nCh].bits.dect_ch ? "DECT CH, " : "",
                     pDev->pPMC[nCh].bits.dect_utg ? "DECT UTG, " : "");

                  len += sprintf(buf+len, "%s%s%s%s%s%s%s%s%s\n",
                     pDev->pPMC[nCh].bits.sig_ch ? "SIG CH, " : "",
                     pDev->pPMC[nCh].bits.sig_dtmfd ? "SIG DTMFd, " : "",
                     pDev->pPMC[nCh].bits.sig_dtmfg ? "SIG DTMFg, " : "",
                     pDev->pPMC[nCh].bits.sig_fskd ? "SIG FSKd, " : "",
                     pDev->pPMC[nCh].bits.sig_fskg ? "SIG FSKg, " : "",
                     pDev->pPMC[nCh].bits.sig_mftd ? "SIG MFTD, " : "",
                     pDev->pPMC[nCh].bits.sig_utg1 ? "SIG UTG1, " : "",
                     pDev->pPMC[nCh].bits.sig_utg2 ? "SIG UTG2, " : "",
                     pDev->pPMC[nCh].bits.sig_cptd ? "SIG CPTD, " : "");
                  /* fancy output: overwrite the last comma with a space */
                  //buf[len-3] = ' ';
                  buf[len-4] = 0x20;
               }
            }

            *start = (char *)1;
            *eof = 1;
            return len;
         }
      }
   }

   *eof = 0;

   return len;
}
#endif /* PMC_SUPPORTED */


/**
   The proc filesystem: function to read an entry.

   \param  page         Pointer to page buffer.
   \param  start        Pointer to start address pointer.
   \param  off          Offset.
   \param  count        Length to read.
   \param  eof          End of file.
   \param  data         Data (used as function pointer).

   \return
   Length of the string returned in buf.
*/
static int vmmc_proc_Read (char *page, char **start, off_t off,
   int count, int *eof, void *data)
{
   IFX_int32_t (*fn)(IFX_char_t *, IFX_int32_t, IFX_int32_t), len;

   if (data != NULL)
   {
      fn = data;
      len = fn (page, off, count);
   }
   else
   {
      *eof = 1;
      return 0;
   }

   *start = page;

   if (len < 0)
   {
      len = 0;
      *eof = 1;
   }
   else if (len < count)
   {
      *eof = 1;
   }
   else
   {
      len = count;
   }

   return len;
}


/**
   Initialize and install the /proc filesystem entries.

   \return
   IFX_SUCCESS or IFX_ERROR
*/
static int vmmc_proc_EntriesInstall (void)
{
   struct proc_dir_entry *driver_proc_node,
                         *proc_file;

   /* install the proc entry */
   TRACE(VMMC, DBG_LEVEL_NORMAL, ("vmmc: using proc fs\n"));

   driver_proc_node = proc_mkdir( "driver/" DEV_NAME, NULL);
   if (driver_proc_node != NULL)
   {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
      driver_proc_node->owner = THIS_MODULE;
#endif /* < Linux 2.6.30 */

      proc_file = create_proc_read_entry("version" , 0,
                     driver_proc_node, vmmc_proc_Read,
                     /*lint -e(611)*/ (void *)vmmc_proc_VersionGet);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
      if (proc_file != NULL)
         proc_file->owner = THIS_MODULE;
#endif /* < Linux 2.6.30 */

      proc_file = create_proc_read_entry("statistics" , 0,
                     driver_proc_node, vmmc_proc_Read,
                     /*lint -e(611)*/ (void *)vmmc_proc_StatisticsGet);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
      if (proc_file != NULL)
         proc_file->owner = THIS_MODULE;
#endif /* < Linux 2.6.30 */

#ifdef HAVE_CONFIG_H
      proc_file = create_proc_read_entry("configure", 0,
                     driver_proc_node, vmmc_proc_Read,
                     /*lint -e(611)*/ (void *)vmmc_proc_ConfigureGet);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
      if (proc_file != NULL)
         proc_file->owner = THIS_MODULE;
#endif /* < Linux 2.6.30 */
#endif /* HAVE_CONFIG_H */

#if (VMMC_CFG_FEATURES & VMMC_FEAT_HDLC)
      proc_file = create_proc_read_entry("res_hdlc", 0,
                     driver_proc_node, vmmc_proc_Read,
                     /*lint -e(611)*/ (void *)vmmc_proc_HdlcInfoGet);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
      if (proc_file != NULL)
         proc_file->owner = THIS_MODULE;
#endif /* < Linux 2.6.30 */
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_HDLC) */

#ifdef EVALUATION
      {
         struct proc_dir_entry *driver_proc_interrupt;

         /* initialize the buffer to IFX_NULL before first call */
         VMMC_EvalSizedBuffer = IFX_NULL;
         driver_proc_interrupt = create_proc_read_entry("interrupt", 0,
                                 driver_proc_node, VMMC_EvalReadProc,
                                 /*lint -e(611)*/ (void *)VMMC_EvalProcInterruptGet);
         if (driver_proc_interrupt != NULL)
         {
            driver_proc_interrupt->write_proc = VMMC_EvalProcInterruptPut;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
            driver_proc_interrupt->owner = THIS_MODULE;
#endif /* < Linux 2.6.30 */
         }
      }
#endif /* #ifdef EVALUATION */

#ifdef PMC_SUPPORTED
      proc_file = create_proc_read_entry("power", 0, driver_proc_node,
                                         vmmc_proc_PmcGet, IFX_NULL);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
      if (proc_file != NULL)
         proc_file->owner = THIS_MODULE;
#endif /* < Linux 2.6.30 */
#endif /* PMC_SUPPORTED */
   }
   else
   {
      TRACE(VMMC, DBG_LEVEL_HIGH,("vmmc: cannot create proc entry\n"));
      return IFX_ERROR;
   }

   return IFX_SUCCESS;
}


/**
   Remove proc filesystem entries.
*/
static void vmmc_proc_EntriesRemove(void)
{
#ifdef PMC_SUPPORTED
   remove_proc_entry("driver/" DEV_NAME "/power", 0);
#endif /* PMC_SUPPORTED */
#ifdef EVALUATION
   remove_proc_entry("driver/" DEV_NAME "/interrupt", 0);
#endif /* #ifdef EVALUATION */
   remove_proc_entry("driver/" DEV_NAME "/version", 0);
   remove_proc_entry("driver/" DEV_NAME "/statistics", 0);
#if (VMMC_CFG_FEATURES & VMMC_FEAT_HDLC)
   remove_proc_entry("driver/" DEV_NAME "/res_hdlc", 0);
#endif /* (VMMC_CFG_FEATURES & VMMC_FEAT_HDLC) */
#ifdef HAVE_CONFIG_H
   remove_proc_entry("driver/" DEV_NAME "/configure", 0);
#endif /* HAVE_CONFIG_H */
   remove_proc_entry("driver/" DEV_NAME , 0);

   return;
}

#endif /* VMMC_USE_PROC */


/* ============================= */
/* interrupt locking/unlocking   */
/* ============================= */

/**
   Disables the irq line if the driver is in interrupt mode and irq line is
   actually enabled according to device flag bIntEnabled.
   Disable the global interrupt if the device is not in polling mode and no
   interrupt line is connected.

   \param  pLLDev       Vmmc device handle.

   \remarks
   If the driver works in Polling mode, nothing is done.
   If the driver works in interrupt mode and the irq was already disabled
   (flag bIntEnabled is IFX_FALSE ), the OS disable function will
   not be called.

   Very important: It is assumed that disable and enable irq are done
   subsequently and that no routine calling disable/enable is executed
   inbetween as stated in following code example:

   \verbatim
   Allowed :

      Vmmc_IrqLockDevice;
      .. some instructions
      Vmmc_IrqUnlockDevice

   Not allowed:

      routineX (IFX_int32_t x)
      {
         Vmmc_IrqLockDevice;
         .. some instructions;
         Vmmc_IrqUnlockDevice
      }

      routineY (IFX_int32_t y)
      {
         Vmmc_IrqLockDevice;
         routine (x);    <---------------- routineX unlocks interrupts..
         be carefull!
         ... some more instructions;
         Vmmc_IrqUnlockDevice;
      }
   \endverbatim
*/
IFX_void_t Vmmc_IrqLockDevice (IFX_TAPI_LL_DEV_t *pLLDev)
{
   VMMC_DEVICE *pDev = (VMMC_DEVICE *) pLLDev;

   if (pDev->nIrqMask == 0)
   {
      VMMC_DISABLE_IRQGLOBAL(pDev->nIrqMask);
   }
}


/**
   Enables the irq line if the driver is in interrupt mode and irq line is
   actually disabled according to device flag bIntEnabled.
   Enable the global interrupt, if this was disabled by 'Vmmc_IrqDisable'

   \param  pLLDev       Vmmc device handle.
*/
IFX_void_t Vmmc_IrqEnable (IFX_TAPI_LL_DEV_t *pLLDev)
{
   VMMC_DEVICE *pDev = (VMMC_DEVICE *) pLLDev;
   vmmc_interrupt_t* pIrq = pDev->pIrq;

   /* invoke board or os routine to enable irq */
   VMMC_ENABLE_IRQLINE(pIrq->nIrq);
}


/**
   Enables the irq line if the driver is in interrupt mode and irq line is
   actually disabled according to device flag bIntEnabled.
   Enable the global interrupt, if this was disabled by 'Vmmc_IrqEnable'

   \param  pLLDev       Vmmc device handle.
*/
IFX_void_t Vmmc_IrqDisable (IFX_TAPI_LL_DEV_t *pLLDev)
{
   VMMC_DEVICE *pDev = (VMMC_DEVICE *) pLLDev;
   vmmc_interrupt_t* pIrq = pDev->pIrq;

   /* invoke board or os routine to disable irq */
   VMMC_DISABLE_IRQLINE(pIrq->nIrq);
}


/**
   Enables the irq line if the driver is in interrupt mode and irq line is
   actually disabled according to device flag bIntEnabled.
   Enable the global interrupt, if this was disabled by 'Vmmc_IrqLockDevice'

   \param  pLLDev       Vmmc device handle.

   Remarks
   cf Remarks of Vmmc_IrqLockDevice () in this file.
*/
IFX_void_t Vmmc_IrqUnlockDevice (IFX_TAPI_LL_DEV_t *pLLDev)
{
   VMMC_DEVICE * pDev = (VMMC_DEVICE *) pLLDev;

   if (pDev->nIrqMask != 0)
   {
      VMMC_OS_INTSTAT nMask = pDev->nIrqMask; /*lint --e{529} */

      pDev->nIrqMask = 0;
      VMMC_ENABLE_IRQGLOBAL(nMask);
   }
}


/**
   Enable the global interrupt if it was disabled by 'Vmmc_IrqLockDevice'.
   Do not enable the interrupt line if it is configured on the device.
   This function has to be called in error case inside the driver to unblock
   global interrupts but do not device interrupt. The interrupt line for that
   specific device is not enable in case of an error.
   \param  pDev         Vmmc device handle.

   Remarks
   cf Remarks of Vmmc_IrqLockDevice () in this file.
*/
IFX_void_t Vmmc_IrqUnlockGlobal (VMMC_DEVICE *pDev)
{
   if (pDev->nIrqMask != 0)
   {
      VMMC_OS_INTSTAT nMask = pDev->nIrqMask; /*lint --e{529} */
      pDev->nIrqMask = 0;
      VMMC_ENABLE_IRQGLOBAL(nMask);
   }

#ifdef DEBUG_INT
   pIrq->nCount--;
#endif /* DEBUG_INT */
}


/**
   Race condition free implmentation for IFXOS_WaitEvent_timeout
   \param  pDev         Reference to device context.

   \return  >0      wakeup occured, remaining timeout
            0       timeout

   \remarks
   Porting instructions

   Different OS implement the wait queue mechanism in different ways. The
   major difference is the way they handle a wakeup which occurs before
   the thread is completely sleeping. Linux is known not to handle this
   "early" wakeup properly, i.e. we'll sleep forever - or when we are lucky
   the same event occurs again. To prevent this case, we need to check a
   condition "half the way to sleep" - where we are still able to rollback
   our preparations to sleep.
   If your OS doesn't require this special handling, you can continue using
   IFXOS_WaitEvent_timeout - or your specific port of this macro - (and
   ignore the additional condition in pDev->bCmdOutBoxData).
*/
IFX_int32_t VMMC_WaitForCmdMbxData(VMMC_DEVICE *pDev)
{
   /*lint -esym(715, pDev) */
   return wait_event_interruptible_timeout(pDev->mpsCmdWakeUp.object,
                                           pDev->bCmdOutBoxData,
                                           (CMD_MBX_RD_TIMEOUT_MS * HZ / 1000));
}


/* ============================= */
/* Linux module support          */
/* ============================= */

/**
   Initialize the module.

   Initializes global mutex, sets trace level and registers the module.
   It calls also functions to initialize the hardware which is used.

   \return Always zero to indicate success.

   \remarks
   Called by the kernel.
*/
#ifdef MODULE
static int __init vmmc_module_init(void)
#else
int vmmc_module_init(void)
#endif
{
   VMMC_DeviceDriverStart();

#ifdef VMMC_USE_PROC
   vmmc_proc_EntriesInstall();
#endif /* VMMC_USE_PROC */

   return IFX_SUCCESS;
}


/**
   Clean up the module when unloaded.

   \remarks
   Called by the kernel.
*/
#ifdef MODULE
static void __exit vmmc_module_exit(void)
#else
void  vmmc_module_exit(void)
#endif
{
#ifdef VMMC_USE_PROC
   vmmc_proc_EntriesRemove();
#endif /* VMMC_USE_PROC */

   VMMC_DeviceDriverStop();

   TRACE (VMMC, DBG_LEVEL_LOW, ("cleaned up %s module.\n", DEV_NAME));
}


#if 1 // charles, ctc merged
/**
   stop a kernel thread. Called by the removing instance

   \param  pThrCntrl    Pointer to a thread control struct.
*/
IFX_void_t VMMC_OS_ThreadKill(VMMC_OS_ThreadCtrl_t *pThrCntrl)
{
   if ((pThrCntrl) &&
       (VMMC_OS_THREAD_INIT_VALID(pThrCntrl) == IFX_TRUE) &&
       (pThrCntrl->thrParams.bRunning == 1))
   {
      /* trigger user thread routine to shutdown */ \
      pThrCntrl->thrParams.bShutDown = IFX_TRUE;

      /* this function needs to be protected with the big
         kernel lock (lock_kernel()). The lock must be
         grabbed before changing the terminate
         flag and released after the down() call. */
      lock_kernel();
      mb();
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
      kill_proc(pThrCntrl->tid, SIGKILL, 1);
#else
      kill_pid(find_vpid(pThrCntrl->tid), SIGKILL, 1);
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28) */
      /* release the big kernel lock */
      unlock_kernel();
      wait_for_completion (&pThrCntrl->thrCompletion);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
      /* Now we are sure the thread is not in zombie state.
         We notify keventd to clean the process up. */
      kill_proc(2, SIGCHLD, 1);
#endif /* kernel 2.6.23 */
   }
}
#endif // charles, ctc merged

#ifdef MODULE
MODULE_DESCRIPTION("VMMC(VoiceMacroMipsCore) device driver - www.lantiq.com");
MODULE_AUTHOR("Lantiq Deutschland GmbH");
MODULE_SUPPORTED_DEVICE("DANUBE, TWINPASS, INCA-IP2, AR9, VR9");
MODULE_LICENSE("Dual BSD/GPL");

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17))
MODULE_PARM(major, "h" );
MODULE_PARM(minorBase, "h" );
MODULE_PARM(devName, "s" );
#else
module_param(major, ushort, 0);
module_param(minorBase, ushort, 0);
module_param(devName, charp, 0);
#endif /* < 2.6.17 */

MODULE_PARM_DESC(major ,"Device Major number");
MODULE_PARM_DESC(minorBase ,"Number of devices to be created");
MODULE_PARM_DESC(devName ,"Name of the device");

/*lint -save -e19  this is not as useless as lint thinks */
module_init(vmmc_module_init);
module_exit(vmmc_module_exit);
/*lint -restore */
#endif

#endif /* LINUX */
