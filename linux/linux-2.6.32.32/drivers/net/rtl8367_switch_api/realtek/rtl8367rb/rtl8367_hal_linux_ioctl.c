/* ============================================================
Follow by
This API package supports multiple Realtek Switch contains ASIC
drivers for LINUX. ASIC drivers provide general APIs that based
on user configuration to configure relative ASIC registers.

reference from rtl83XX_hal_linux.c/.h

Programmer : Alvin Hsu, alvin_hsu@arcadyan.com
=============================================================== */
#ifdef __KERNEL__
#include <linux/ioctl.h> 									/* needed for the _IOW etc stuff used later */
#include <linux/kernel.h> 								/* printk() 			*/
#endif
#include <linux/slab.h>										/* kmalloc() 			*/
#include <linux/fs.h>											/* everything... 	*/
#include <linux/errno.h>									/* error codes 		*/

#include <asm/uaccess.h>

#include "switch_vendor_error.h"					/* local definitions */

#include "rtl83XX_hal.h"									/* local definitions */
#include "rtl8367_hal_linux_ioctl.h"			/* local definitions */
#include <rtl8367_hal_linux_ioctl_extension.h>	/* common definitions */


#if(_RTK_RTL83XX_SWITCH_API == 1)
#error "No available switch api, version error!!"
#elif(_RTK_RTL83XX_SWITCH_API == 2)
#include "rtl8367rb/switch_api_v1_2_10/include/rtk_api_ext.h"
#include "rtl8367rb/switch_api_v1_2_10/include/rtk_api.h"
#include "rtl8367rb/switch_api_v1_2_10/include/rtk_types.h"
#include "rtl8367rb/switch_api_v1_2_10/include/rtk_error.h"

#if(RTK_CHIP_ID == CHIP_ID_RTL8367RB)
#include "rtl8367rb/switch_api_v1_2_10/include/rtl8367b_reg.h"
#endif
#endif

RTL8367_IOC_WRAPPER_CTX *gpstIocWrapperCtx = RTK_NULL;

rtl8367_ll_fkt gstRtl8367_FktPtrTbl[] =
{
   /* 0x00 */
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_switch_init,
   /* Index: 0x01 ; Command: RTL8367_IOCTL_MAXPKTLEN_SET  									*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_maxpktlength_set,
   /* Index: 0x02 ; Command: RTL8367_IOCTL_MAXPKTLEN_GET 										*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_maxpktlength_get,
   /* Index: 0x03 ; Command: RTL8367_IOCTL_PORTMAXPKTLEN_SET   							*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_portmaxpktlength_set,
   /* Index: 0x04 ; Command: RTL8367_IOCTL_PORTMAXPKTLEN_GET								*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_portmaxpktlength_get,
   /* Index: 0x05 ; Command: RTL8367_IOCTL_CPUPORTTAGEN_SET   							*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_CPUporttagenable_set,
   /* Index: 0x06 ; Command: RTL8367_IOCTL_CPUPORTTAGEN_GET   							*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_CPUporttagenable_get,
   /* Index: 0x07 ; Command: RTL8367_IOCTL_CPUPORTTAG_SET   								*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_CPUporttagcfg_set,
   /* Index: 0x08 ; Command: RTL8367_IOCTL_CPUPORTTAG_GET   								*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_CPUporttagcfg_get,
   /* Index: 0x09 ; Command:    																						*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x0A ; Command:    																						*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x0B ; Command:    																						*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x0C ; Command:    																						*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x0D ; Command:    																						*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x0E ; Command:    																						*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x0F ; Command:    																						*/
   (rtl8367_ll_fkt) RTK_NULL,


   /* Index: 0x10 ; Command: RTL8367_IOCTL_L2LUT_INIT   										*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_l2lut_init,
   /* Index: 0x11 ; Command: RTL8367_IOCTL_L2LUT_TYPE_SET   								*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_l2luttype_set,
   /* Index: 0x12 ; Command: RTL8367_IOCTL_L2LUT_TYPE_GET   								*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_l2luttype_get,
   /* Index: 0x13 ; Command: RTL8367_IOCTL_L2LUT_MAC_GET 		 								*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_l2mactableentry_get,
   /* Index: 0x14 ; Command: RTL8367_IOCTL_L2LUT_MAC_GETNEXT 	  						*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_l2mactableentry_getnext,
   /* Index: 0x15 ; Command: RTL8367_IOCTL_L2LUT_MAC_ADD 	  								*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x16 ; Command: RTL8367_IOCTL_L2LUT_MAC_DEL 	  								*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x17 ; Command: RTL8367_IOCTL_L2LUT_MAC_DUMP_BY_PORT   				*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x18 ; Command: RTL8367_IOCTL_L2LUT_MAC_DUMP   								*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x19 ; Command: RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_GET 	  		*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x1A ; Command: RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_GETNEXT 	  */
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_l2ipmulticasttableentry_getnext,
   /* Index: 0x1B ; Command: RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_ADD 	  		*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_l2ipmulticasttableentry_add,
   /* Index: 0x1C ; Command: RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_REMOVE   	*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_l2ipmulticasttableentry_remove,
   /* Index: 0x1D ; Command: RTL8367_IOCTL_L2LUT_UNICASTADDR_DUMP   				*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x1E ; Command:    																						*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x1F ; Command:    																						*/
   (rtl8367_ll_fkt) RTK_NULL,


   /* Index: 0x20 ; Command: RTL8367_IOCTL_PORTLINKCFG_GET   								*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_portlinkcfg_get,
   /* Index: 0x21 ; Command: RTL8367_IOCTL_PORTLINKPHYCFG_GET   						*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_portlinkphycfg_get,
   /* Index: 0x22 ; Command: RTL8367_IOCTL_PORTFORCELINKEXTCFG_SET   				*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_portforcelinkextcfg_set,
   /* Index: 0x23 ; Command: RTL8367_IOCTL_PORTFORCELINKEXTCFG_GET   				*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_portforcelinkextcfg_get,
   /* Index: 0x24 ; Command: RTL8367_IOCTL_PORTRGMIICLKEXT_SET   						*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_portrgmiiclkext_set,
   /* Index: 0x25 ; Command: RTL8367_IOCTL_PORTRGMIICLKEXT_GET   						*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_portrgmiiclkext_get,
   /* Index: 0x26 ; Command: RTL8367_IOCTL_PHYTESTMODE_SET   								*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_portphytestmode_set,
   /* Index: 0x27 ; Command: RTL8367_IOCTL_PHYTESTMODE_GET   								*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_portphytestmode_get,
   /* Index: 0x28 ; Command:    																						*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x29 ; Command:    																						*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x2A ; Command:    																						*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x2B ; Command:    																						*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x2C ; Command:    																						*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x2D ; Command:    																						*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x2E ; Command:    																						*/
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x2F ; Command: 														   									*/
   (rtl8367_ll_fkt) RTK_NULL,


   /* Index: 0x30 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x31 ;Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x32 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x33 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x34 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x35 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x36 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x37 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x38 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x39 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x3A ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x3B ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x3C ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x3D ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x3E ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x3F ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x40 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x41 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x42 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x43 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x44 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x45 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x46 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x47 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x48 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x49 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x4A ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x4B ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x4C ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x4D ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x4E ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x4F ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,


	 /* Index: 0x50 ; Command: RTL8367_IOCTL_REGISTER_SET ;  									*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_register_set,
   /* Index: 0x51 ; Command: RTL8367_IOCTL_REGISTER_GET ;  									*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_register_get,
   /* Index: 0x52 ; Command: RTL8367_IOCTL_PORTPOWER_ONOFF ;  							*/
   (rtl8367_ll_fkt) rtl8367_ioctl_ll_portpower_onoff,
   /* Index: 0x53 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x54 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x55 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x56 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x57 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x58 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x59 ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x5A ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x5B ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x5C ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x5D ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x5E ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
   /* Index: 0x5F ; Command: 																							  */
   (rtl8367_ll_fkt) RTK_NULL,
};

RTL8367_LOWLEVELFKT gstRtl8367_ll_FktTbl =
{
   RTK_NULL, 														/* pNext */
   (unsigned short)RTL8367_IOC_MAGIC, 	/* nType */
   RTL8367_IOC_MAXNR, 									/* nNumFkts */
   gstRtl8367_FktPtrTbl 								/* pFkts */
};


/*****************************************************************
*	Extern
*****************************************************************/

/*****************************************************************
*	Utility
*****************************************************************/

/*****************************************************************
*	Function
*****************************************************************/
/* This is the switch ioctl wrapper init function. */
void *rtl8367_ioctl_wrapper_init(void)
{
		RTL8367_IOC_WRAPPER_CTX *pstIocWrapCtx;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & DBG_SWITCH_IOCTL) == DBG_SWITCH_IOCTL)
		printk(KERN_INFO "\tinit ioctl wrapper\n");
		#endif

		pstIocWrapCtx = (RTL8367_IOC_WRAPPER_CTX*)kmalloc(sizeof(RTL8367_IOC_WRAPPER_CTX), GFP_KERNEL);
		if(!pstIocWrapCtx)
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & DBG_ERROR) == DBG_ERROR)
    		printk(KERN_INFO "[%s] ioctl wrapper memory allocation failed!!!\n", __func__);
				#endif

				kfree(pstIocWrapCtx);
				pstIocWrapCtx = RTK_NULL;
				
    		return RTK_NULL;
    }

		memset(pstIocWrapCtx, 0, sizeof(RTL8367_IOC_WRAPPER_CTX));

    pstIocWrapCtx->pstIoctlHandle = (RTL8367_IOC_HANDLE*)kmalloc(sizeof(RTL8367_IOC_HANDLE), GFP_KERNEL);
    if(!pstIocWrapCtx->pstIoctlHandle)
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & DBG_ERROR) == DBG_ERROR)
    		printk(KERN_INFO "[%s] ioctl handler memory allocation failed!!!\n", __func__);
				#endif

				if(pstIocWrapCtx)
					kfree(pstIocWrapCtx);

				pstIocWrapCtx = RTK_NULL;

				return RTK_NULL;
    }

		memset(pstIocWrapCtx->pstIoctlHandle, 0, sizeof(RTL8367_IOC_HANDLE));

		pstIocWrapCtx->pstIoctlHandle->pstLlTable = &gstRtl8367_ll_FktTbl;

		gpstIocWrapperCtx = pstIocWrapCtx;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & DBG_SWITCH_IOCTL) == DBG_SWITCH_IOCTL)
		printk(KERN_INFO "[%s] u16Type = 0x%x, u32NumFkts = %d\n", __func__, pstIocWrapCtx->pstIoctlHandle->pstLlTable->u16Type \
																																			 , pstIocWrapCtx->pstIoctlHandle->pstLlTable->u32NumFkts);
		#endif


		return pstIocWrapCtx;
}

int rtl8367_ioctl_wrapper_cleanup(void)
{
		RTL8367_IOC_WRAPPER_CTX *pstIocWrapCtx = gpstIocWrapperCtx;

		if(pstIocWrapCtx != RTK_NULL)
		{
				if(pstIocWrapCtx->pstIoctlHandle != RTK_NULL)
				{
						kfree(pstIocWrapCtx->pstIoctlHandle);
						pstIocWrapCtx->pstIoctlHandle = RTK_NULL;
				}

				kfree(pstIocWrapCtx);
				pstIocWrapCtx = RTK_NULL;
		}

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_lowlevelcommandsearch(void *pHandle, int s32Cmd, int s32Arg, RTL8367_API_TYPE Type)
{
		RTL8367_IOC_HANDLE *pstIocHandle = (RTL8367_IOC_HANDLE*)pHandle;
		RTL8367_LOWLEVELFKT *pstLlTable = pstIocHandle->pstLlTable;

		/* This table contains the low-level function for the IOCTL commands with the same MAGIC-Type numer. */
		rtl8367_ll_fkt  fkt;
		unsigned int   	u32Size;
		unsigned int   	u32RetVal;
		unsigned int   	u32CmdNr = _IOC_NR(s32Cmd);

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & DBG_SWITCH_IOCTL) == DBG_SWITCH_IOCTL)
		printk(KERN_INFO "\tioctl lowlevel command search\n");
		#endif

		#if 0
		if(down_interruptible(&swapi_sem))
		{
				/*  signal received, semaphore not acquired ... */
				return IFX_ERROR;
		}
		#endif

		do {
					if(_IOC_TYPE(s32Cmd) == pstLlTable->u16Type)
					{
							/* Number out of range. No function available for this command number. */
							if(u32CmdNr >= pstLlTable->u32NumFkts)
							{
									#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & DBG_ERROR) == DBG_ERROR)
            			printk("[%s] ioctl command number out of range. u32CmdNr = %d, u32NumFkts = %d\n", __func__, u32CmdNr, pstLlTable->u32NumFkts);
									#endif

									goto CmdSraechFailed;
							}

							fkt = pstLlTable->pstFkts[u32CmdNr];

							/* No low-level function given for this command. */
							if(fkt == RTK_NULL)
							{
									#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & DBG_ERROR) == DBG_ERROR)
            			printk("[%s] no low-level function given for this command. u32CmdNr = %d, u32NumFkts = %d\n", __func__, u32CmdNr, pstLlTable->u32NumFkts);
									#endif
									
									goto CmdSraechFailed;
							}

							/* Copy parameter from userspace. */
							u32Size = _IOC_SIZE(s32Cmd);

							/* Local temporary buffer to store the parameter is to small. */
							if(u32Size > 2048)
							{
									#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & DBG_ERROR) == DBG_ERROR)
            			printk("[%s] local temp. buffer to store the parameter is to small. u32CmdNr = %d, u32NumFkts = %d\n", __func__, u32CmdNr, pstLlTable->u32NumFkts);
									#endif
									
									goto CmdSraechFailed;
							}

							if(Type == RTL8367_API_USERAPP)
							{
									copy_from_user((void *)(pstIocHandle->s8ParamBuffer), (const void __user *)s32Arg, (unsigned long)u32Size);
									/* Now call the low-level function with the right low-level context \
									handle and the local copy of the parameter structure of 'arg'. */

            			//printk("pstIocHandle=%p, pstIocHandle->pLlHandle=%p\n", pstIocHandle, pstIocHandle->pLlHandle);

									u32RetVal = fkt(pstIocHandle->pLlHandle, (unsigned int)pstIocHandle->s8ParamBuffer);

									/* Copy parameter to userspace. */
									if(_IOC_DIR(s32Cmd) & _IOC_READ)
									{
											/* Only copy back to userspace if really required */
											copy_to_user((void __user *)s32Arg, (const void *)(pstIocHandle->s8ParamBuffer), (unsigned long)u32Size);
									}
							}
							else
							{
									memcpy((void *)(pstIocHandle->s8ParamBuffer), (const void *)s32Arg, (unsigned long)u32Size);
									u32RetVal = fkt(pstIocHandle->pLlHandle, (unsigned int)pstIocHandle->s8ParamBuffer);
									memcpy((void *)s32Arg, (const void *)(pstIocHandle->s8ParamBuffer), (unsigned long)u32Size);
							}

							#if 0
							up(&swapi_sem);
							#endif

							#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & DBG_SWITCH_IOCTL) == DBG_SWITCH_IOCTL)
							printk(KERN_INFO "\t_IOC_TYPE = %ld, _IOC_NR = %ld, _IOC_SIZE = %ld\n", __func__, _IOC_TYPE(s32Cmd), _IOC_NR(s32Cmd), _IOC_SIZE(s32Cmd));
							#endif

							return u32RetVal;
					}

					#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & DBG_SWITCH_IOCTL) == DBG_SWITCH_IOCTL)
					printk(KERN_INFO "[%s] _IOC_TYPE = %ld, _IOC_NR = %ld, _IOC_SIZE = %ld\n", __func__, _IOC_TYPE(s32Cmd), _IOC_NR(s32Cmd), _IOC_SIZE(s32Cmd));
					#endif
				
					/* 
					*	If command was not found in the current table index, look for the next \
					*	linked table. Search till it is found or we run out of tables. 
					*/
					pstLlTable = pstLlTable->pstNextFkt;

		}while (pstLlTable != RTK_NULL);

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & DBG_SWITCH_IOCTL) == DBG_SWITCH_IOCTL)
		printk("[%s] no low-level function given for this command. u32CmdNr = %d, u32NumFkts = %d \n", __func__, u32CmdNr, pstLlTable->u32NumFkts);
		#endif

		return SWHAL_ERR_FAILED;

CmdSraechFailed:

		/*  release the given semaphore */
		//up(&swapi_sem);

		/* No supported command low-level function found. */
   	return SWHAL_ERR_FAILED;
}

/* [0x00~0x0F] =========================================================================== */ 
int rtl8367_ioctl_ll_switch_init(void *pDevCtx, unsigned int u32Params)
{
		int s32RetVal;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN)) == (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN))	
		printk(KERN_INFO "\tInit rtl8367 switch\n");
		#endif

		if(RT_ERR_OK != (s32RetVal = rtk_switch_init()))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
				printk(KERN_INFO "\tInit rtl8367 switch failed, errno = 0x%08x\n", s32RetVal);
				#endif

				return s32RetVal;
		}

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_maxpktlength_set(void *pDevCtx, RTK_PORT_MAXPKTLEN *pstParams)
{
		int s32RetVal;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN)) == (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN))	
		printk(KERN_INFO "\tSet max. packet length = 0x%x\n", pstParams->u32MaxPktLen);
		#endif

		if(RT_ERR_OK != (s32RetVal = rtk_switch_maxPktLen_set((rtk_switch_maxPktLen_t)pstParams->u32MaxPktLen)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
				printk(KERN_INFO "\tSet max. packet length 0x%x failed, errno = 0x%08x\n", pstParams->u32MaxPktLen, s32RetVal);
				#endif

				return s32RetVal;
		}

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_maxpktlength_get(void *pDevCtx, RTK_PORT_MAXPKTLEN *pstParams)
{
		int s32RetVal;

		if(RT_ERR_OK != (s32RetVal = rtk_switch_maxPktLen_get((rtk_switch_maxPktLen_t *)&pstParams->u32MaxPktLen)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
				printk(KERN_INFO "\tGot max. packet length failed, errno = 0x%08x\n", s32RetVal);
				#endif

				pstParams->u32MaxPktLen = -1;
				return s32RetVal;
		}

		if(pstParams->u32MaxPktLen == MAXPKTLEN_1522B)
			pstParams->u32MaxPktLen = 1522;
		else if(pstParams->u32MaxPktLen == MAXPKTLEN_1536B)
			pstParams->u32MaxPktLen = 1536;
		else if(pstParams->u32MaxPktLen == MAXPKTLEN_1552B)
			pstParams->u32MaxPktLen = 1552;
		else if(pstParams->u32MaxPktLen == MAXPKTLEN_16000B)
			pstParams->u32MaxPktLen = 16000;
		else
		{
			pstParams->u32MaxPktLen = -1;
			return s32RetVal;
		}

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN)) == (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN))	
		printk(KERN_INFO "\tGot max. packet length = %d\n", pstParams->u32MaxPktLen);
		#endif

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_portmaxpktlength_set(void *pDevCtx, RTK_PORT_MAXPKTLEN *pstParams)
{
		int s32RetVal;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN)) == (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN))	
		printk(KERN_INFO "\tSet port id = %d, max packet length = %d \n", pstParams->u32PortNo, pstParams->u32MaxPktLen);
		#endif

		if(RT_ERR_OK != (s32RetVal = rtk_switch_portMaxPktLen_set((rtk_port_t)pstParams->u32PortNo,(rtk_uint32)pstParams->u32MaxPktLen)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
				printk(KERN_INFO "\tSet port %d, max. packet length failed, errno = 0x%08x\n", pstParams->u32PortNo, s32RetVal);
				#endif

				return s32RetVal;
		}

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_portmaxpktlength_get(void *pDevCtx, RTK_PORT_MAXPKTLEN *pstParams)
{
		int s32RetVal;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN)) == (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN))	
		printk(KERN_INFO "\tGet port id = %d\n", pstParams->u32PortNo);
		#endif

		if(RT_ERR_OK != (s32RetVal = rtk_switch_portMaxPktLen_get((rtk_port_t)pstParams->u32PortNo, (rtk_uint32 *)&pstParams->u32MaxPktLen)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tGot port %d, max. packet length failed, errno = 0x%08x\n", pstParams->u32PortNo, s32RetVal);
				#endif
				
				pstParams->u32MaxPktLen = -1;
				return s32RetVal;
		}

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN)) == (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN))	
		printk(KERN_INFO "\tGet max. packet length = %d\n", pstParams->u32MaxPktLen);
		#endif

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_CPUporttagenable_set(void *pDevCtx, unsigned int *pu32Params)
{
		int s32RetVal;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN)) == (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN))	
		printk(KERN_INFO "\tSet/Unset = %d\n", *pu32Params);
		#endif

		if(RT_ERR_OK != (s32RetVal = rtk_cpu_enable_set((rtk_enable_t)*pu32Params)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tSet CPU port tag function failed, errno = 0x%08x\n", s32RetVal);
				#endif
				
				return s32RetVal;
		}

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_CPUporttagenable_get(void *pDevCtx, unsigned int *pu32Params)
{
		int s32RetVal;

		if(RT_ERR_OK != (s32RetVal = rtk_cpu_enable_get((rtk_enable_t *)pu32Params)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tGot CPU port tag function status failed, errno = 0x%08x\n", s32RetVal);
				#endif
				
				return s32RetVal;
		}

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN)) == (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN))	
		printk(KERN_INFO "\tCPU port tag function status = %d\n", (unsigned int)*pu32Params);
		#endif

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_CPUporttagcfg_set(void *pDevCtx, RTK_CPUPORT_TAG	*pstParams)
{
		int s32RetVal;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN)) == (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN))	
		printk(KERN_INFO "\tSet CPU port %d, mode = %d, port mask = 0x%x\n", pstParams->u32CpuPortNo, pstParams->u32CpuPortMode, pstParams->u32CpuPortMAsk);
		#endif

		if(RT_ERR_OK != (s32RetVal = rtk_cpu_tagPort_set((rtk_port_t)pstParams->u32CpuPortNo, (rtk_cpu_insert_t)pstParams->u32CpuPortMode)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tSet CPU Port tag configuration failed, errno = 0x%08x\n", s32RetVal);
				#endif
				
				return s32RetVal;
		}

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_CPUporttagcfg_get(void *pDevCtx, RTK_CPUPORT_TAG *pstParams)
{
		int s32RetVal;

		if(RT_ERR_OK != (s32RetVal = rtk_cpu_tagPort_get((rtk_port_t *)&pstParams->u32CpuPortNo, (rtk_cpu_insert_t *)&pstParams->u32CpuPortMode)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tSet CPU Port tag configuration failed, errno = 0x%08x\n", s32RetVal);
				#endif
				
				return s32RetVal;
		}

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN)) == (DBG_KERNEL_SPACE|DBG_IOCTL_00_ADMIN))	
		printk(KERN_INFO "\tGot CPU port %d, mode = %d, port mask = 0x%x\n", pstParams->u32CpuPortNo, pstParams->u32CpuPortMode, pstParams->u32CpuPortMAsk);
		#endif
		
		return SWHAL_ERR_SUCCESS;
}

/* [0x10~0x1F] =========================================================================== */ 
int rtl8367_ioctl_ll_l2lut_init(void *pDevCtx, unsigned int u32Params)
{
		int s32RetVal;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))	
		printk(KERN_INFO "\tInit rtl8367 switch L2 lookup table!!!\n");
		#endif

		if(RT_ERR_OK != (s32RetVal = rtk_l2_init()))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tInit rtl8367 L2 lookup table failed, errno = 0x%08x\n", s32RetVal);
				#endif

				return s32RetVal;
		}

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_l2luttype_set(void *pDevCtx, unsigned int *pu32Params)
{
		int s32RetVal;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))	
		printk(KERN_INFO "\tSet L2 lookup table type = %d\n",	*pu32Params);
		#endif

		if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddrLookup_set((rtk_l2_lookup_type_t)*pu32Params)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tSet L2 lookup table type = %d failed, errno = 0x%08x\n", *pu32Params, s32RetVal);
				#endif
				
				return s32RetVal;
		}

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_l2luttype_get(void *pDevCtx, unsigned int *pu32Params)
{
		int s32RetVal;

		if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddrLookup_get((rtk_l2_lookup_type_t *)pu32Params)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tGot L2 lookup table type failed, errno = 0x%08x\n", s32RetVal);
				#endif
				
				return s32RetVal;
		}

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))	
		printk(KERN_INFO "\tGot L2 lookup table type = %d\n",	(unsigned int)*pu32Params);
		#endif

		return SWHAL_ERR_SUCCESS;
}


int rtl8367_ioctl_ll_l2mactableentry_get(void *pDevCtx, RTK_L2LUT_STATUS *pstParams)
{
		int s32RetVal;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))	
		printk(KERN_INFO "\tGet MAC adress = %02X:%02X:%02X:%02X:%02X:%02X\n", 
			pstParams->stMacTblStatus.stMac.octet[0],pstParams->stMacTblStatus.stMac.octet[1],pstParams->stMacTblStatus.stMac.octet[2],
			pstParams->stMacTblStatus.stMac.octet[3],pstParams->stMacTblStatus.stMac.octet[4],pstParams->stMacTblStatus.stMac.octet[5]);
		#endif
		
		if(RT_ERR_OK != (s32RetVal = rtk_l2_addr_get((rtk_mac_t *)&pstParams->stMacTblStatus.stMac, (rtk_l2_ucastAddr_t *)&pstParams->stMacTblStatus.stMacTblEntry)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				if(s32RetVal == RT_ERR_L2_ENTRY_NOTFOUND)
					printk(KERN_INFO "\tSpecified entry not found, errno = 0x%08x\n", s32RetVal);
				else
					printk(KERN_INFO "\tGot MAC entry from L2 lookup table failed, errno = 0x%x\n", s32RetVal);
				#endif

				return s32RetVal;
		}

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))	
		printk(KERN_INFO "\tGot MAC = %02X:%02X:%02X:%02X:%02X:%02X\n",
			pstParams->stMacTblStatus.stMacTblEntry.stMac.octet[0],pstParams->stMacTblStatus.stMacTblEntry.stMac.octet[1],pstParams->stMacTblStatus.stMacTblEntry.stMac.octet[2],
			pstParams->stMacTblStatus.stMacTblEntry.stMac.octet[3],pstParams->stMacTblStatus.stMacTblEntry.stMac.octet[4],pstParams->stMacTblStatus.stMacTblEntry.stMac.octet[5]);

		printk(KERN_INFO "\tGot ivl = %d, cvid = %d, fid = %d, efid = %d, port = %d, sa_block = %d, da_block = %d, static = %d,\n",
			pstParams->stMacTblStatus.stMacTblEntry.u32Ivl,pstParams->stMacTblStatus.stMacTblEntry.u32CVid,pstParams->stMacTblStatus.stMacTblEntry.u32Fid,
			pstParams->stMacTblStatus.stMacTblEntry.u32EFid,pstParams->stMacTblStatus.stMacTblEntry.u32Port,pstParams->stMacTblStatus.stMacTblEntry.u32SABlock,
			pstParams->stMacTblStatus.stMacTblEntry.u32DABlock,pstParams->stMacTblStatus.stMacTblEntry.u32Static);
		#endif

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_l2mactableentry_getnext(void *pDevCtx, RTK_L2LUT_STATUS *pstParams)
{
		int s32RetVal;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))	
		printk(KERN_INFO "\tGet next by method = %d, port id = %d, idx = 0x%08x(%d)\n", pstParams->u8Method, pstParams->u32PhyID, pstParams->u32Idx, pstParams->u32Idx);
		#endif

		while(1)
		{
		    if(RT_ERR_OK != (s32RetVal = rtk_l2_addr_next_get((rtk_l2_read_method_t)pstParams->u8Method, (rtk_port_t)pstParams->u32PhyID, (rtk_uint32 *)&pstParams->u32Idx, (rtk_l2_ucastAddr_t *)&pstParams->stMacTblStatus.stMacTblEntry)))
		    {
						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
						if(s32RetVal == RT_ERR_L2_ENTRY_NOTFOUND)
							printk(KERN_INFO "\tSpecified entry not found, errno = 0x%08x\n", s32RetVal);
						else
							printk(KERN_INFO "\tGot MAC entry from L2 lookup table failed, errno = 0x%x\n", s32RetVal);
						#endif

						return s32RetVal;
		    }
				else
				{
						/* Got one and break out!!! */
						break;
				}

		    pstParams->u32Idx++;
		}

		return SWHAL_ERR_SUCCESS;

}

int rtl8367_ioctl_ll_l2ipmulticasttableentry_getnext(void *pDevCtx, RTK_L2LUT_STATUS *pstParams)
{
		int s32RetVal;
		rtk_portmask_t stRtkPortMask;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))	
		printk(KERN_INFO "Current index = 0x%x(%d)\n", pstParams->u32Idx, pstParams->u32Idx);
		#endif

    if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_next_get((rtk_uint32 *)&pstParams->u32Idx, (ipaddr_t *)&pstParams->u32SIP, (ipaddr_t *)&pstParams->u32DIP, (rtk_portmask_t *)&stRtkPortMask)))
    {
				if(s32RetVal == RT_ERR_L2_ENTRY_NOTFOUND)
				{
						if(pstParams->u32Idx == 0)
						{
								/* No multicast group added!!! */
								#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))	
								printk(KERN_INFO "\tNo multicast group added!!! u32Idx = 0x%x(%d), errno = 0x%08x\n", pstParams->u32Idx, pstParams->u32Idx, s32RetVal);
								#endif
								pstParams->u32DIP = 0x0;
								pstParams->u32SIP = 0x0;
								pstParams->u32PortMask = 0x0;
								pstParams->u32Idx = 0x0;
								
								return s32RetVal;
						}
						else
						{
								/* Got last multicast group added!!! */
								#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))	
								printk(KERN_INFO "\tGot last multicast group added!!! u32Idx = 0x%x(%d)\n", pstParams->u32Idx, pstParams->u32Idx);
								#endif
								
								return SWHAL_ERR_L2IGMPTBL_LASTGRP;
						}
				}						
    }

		/* Got multicast group added!!! */
		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))	
		printk(KERN_INFO "[%s] pstParams->u32Idx = 0x%x(%d), u32SIP = 0x%x, u32DIP = 0x%x, PortMask = 0x%x\n", __func__, pstParams->u32Idx, pstParams->u32Idx, pstParams->u32SIP, pstParams->u32DIP, stRtkPortMask.bits[0]);
		#endif
		
		pstParams->u32PortMask = stRtkPortMask.bits[0];

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_l2ipmulticasttableentry_add(void *pDevCtx, RTK_L2LUT_STATUS *pstParams)
{
		int s32RetVal;
		//unsigned int u32AddrIdx = 0;

		rtk_portmask_t stRtkPortMask;
		unsigned int u32SIP, u32DIP;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))	
		printk(KERN_INFO "\tGDP = %d.%d.%d.%d, GSP = %d.%d.%d.%d, port Mmask = 0x%x\n",
			(pstParams->u32DIP >> 24)&0xFF, ((pstParams->u32DIP >> 16)&0xFF),
			(pstParams->u32DIP >> 8)&0xFF, (pstParams->u32DIP & 0xFF),
			(pstParams->u32SIP >> 24)&0xFF, ((pstParams->u32SIP >> 16)&0xFF),
			(pstParams->u32SIP >> 8)&0xFF, (pstParams->u32SIP & 0xFF),
			 pstParams->u32PortMask);
		#endif
		
		/* Find exiting entry */
		#if 0 // Mask for spent too much time, mplayer timeout issue
		while(1)
		{
				if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_next_get((rtk_uint32 *)&u32AddrIdx, (ipaddr_t *)&u32SIP, (ipaddr_t *)&u32DIP, (rtk_portmask_t *)&stRtkPortMask)))
				{
						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))	
						printk(KERN_INFO "u32AddrIdx = 0x%x, u32DIP = 0x%x, u32SIP = 0x%x, stRtkPortMask = 0x%x\n", u32AddrIdx, u32DIP, u32SIP, stRtkPortMask.bits[0]);
						#endif
						
						/* find last entry */
						if(u32AddrIdx == RTK_MAX_LUT_ADDR_ID)
						{
								stRtkPortMask.bits[0] = (unsigned int)(0x1<<pstParams->u32PhyID);
								#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))	
								printk(KERN_INFO "\thad not been added, u32PortMask = 0x%x\n", stRtkPortMask.bits[0]);
								#endif

								break;
						}
				}
				else
				{
						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
						printk(KERN_INFO "\tu32AddrIdx = 0x%x, u32DIP = 0x%x, u32SIP = 0x%x, u32PortMask = 0x%x\n", u32AddrIdx, u32DIP, u32SIP, stRtkPortMask.bits[0]);
						#endif
						
						/* GDP had been added! */
						if(pstParams->u32DIP == u32DIP)
						{
								#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
								printk(KERN_INFO "\tGDP had been added! new add mask = 0x%x, org = 0x%x\n", (unsigned int)(0x1<<pstParams->u32PhyID), stRtkPortMask.bits[0]);
								#endif
								
								if((unsigned int)(0x1<<pstParams->u32PhyID) & stRtkPortMask.bits[0])
								{
										#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
										printk(KERN_INFO "\tGDP had been added into port = %d, port mask = 0x%x!!!\n", pstParams->u32PhyID, stRtkPortMask.bits[0]);
										#endif

										return SWHAL_ERR_SUCCESS;
								}
								else
								{
										stRtkPortMask.bits[0] |= ((unsigned int)(0x1<<pstParams->u32PhyID));

										#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
										printk(KERN_INFO "\tNew mask = 0x%x\n", stRtkPortMask.bits[0]);
										#endif

										/* Find exting group and update port mask */
										break;
								}
						}
				}

				if(u32AddrIdx >= RTK_MAX_LUT_ADDR_ID)	break;

				u32AddrIdx++;
		}
		#else
		if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_get((ipaddr_t)pstParams->u32SIP, (ipaddr_t)pstParams->u32DIP, (rtk_portmask_t *)&stRtkPortMask)))
		{
				stRtkPortMask.bits[0] = (unsigned int)(0x1<<pstParams->u32PhyID);
				
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))	
				printk(KERN_INFO "\thad not been added, u32PortMask = 0x%x, u32PhyID = %d\n", stRtkPortMask.bits[0], pstParams->u32PhyID);
				#endif
				
		}
		else
		{
			  #if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
				printk(KERN_INFO "\tGDP had been added! new add mask = 0x%x, org = 0x%x\n", (unsigned int)(0x1<<pstParams->u32PhyID), stRtkPortMask.bits[0]);
				#endif
				
				if((unsigned int)(0x1<<pstParams->u32PhyID) & stRtkPortMask.bits[0])
				{
						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
						printk(KERN_INFO "\tGDP had been added into port = %d, port mask = 0x%x!!!\n", pstParams->u32PhyID, stRtkPortMask.bits[0]);
						#endif

						return SWHAL_ERR_SUCCESS;
				}
				else
				{
						stRtkPortMask.bits[0] |= ((unsigned int)(0x1<<pstParams->u32PhyID));

						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
						printk(KERN_INFO "\tNew mask = 0x%x\n", stRtkPortMask.bits[0]);
						#endif
				}			
		}
		#endif

		/* Add group in lookup table */
		if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_add((ipaddr_t)pstParams->u32SIP, (ipaddr_t)pstParams->u32DIP, (rtk_portmask_t)stRtkPortMask)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tAdd group 0x%x into lookup table failed, errno = 0x%08x\n", pstParams->u32DIP, s32RetVal);
				#endif
				
				return s32RetVal;
		}

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_l2ipmulticasttableentry_remove(void *pDevCtx, RTK_L2LUT_STATUS *pstParams)
{
		int s32RetVal;
		unsigned int u32AddrIdx = 0;

		rtk_portmask_t stRtkPortMask;
		unsigned int u32SIP, u32DIP;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
		printk(KERN_INFO "\tGDP = %d.%d.%d.%d, GSP = %d.%d.%d.%d, port mask = 0x%x\n",
			(pstParams->u32DIP >> 24)&0xFF, ((pstParams->u32DIP >> 16)&0xFF),
			(pstParams->u32DIP >> 8)&0xFF, (pstParams->u32DIP & 0xFF),
			(pstParams->u32SIP >> 24)&0xFF, ((pstParams->u32SIP >> 16)&0xFF),
			(pstParams->u32SIP >> 8)&0xFF, (pstParams->u32SIP & 0xFF),
			 pstParams->u32PortMask);
		#endif

		/* Find exiting entry */
		#if 0 // Mask for spent too much time, mplayer timeout issue
		while(1)
		{
				if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_next_get((rtk_uint32 *)&u32AddrIdx, (ipaddr_t *)&u32SIP, (ipaddr_t *)&u32DIP, (rtk_portmask_t *)&stRtkPortMask)))
				{
						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
						printk(KERN_INFO "u32AddrIdx = 0x%x, u32DIP = 0x%x, u32SIP = 0x%x, stRtkPortMask = 0x%x\n", u32AddrIdx, u32DIP, u32SIP, stRtkPortMask.bits[0]);
						#endif
						
						/* find last entry */
						if(u32AddrIdx == RTK_MAX_LUT_ADDR_ID)
						{
								#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
								printk(KERN_INFO "\tGroup had not been added!!!\n");
								#endif
								
								return s32RetVal;
						}
				}
				else
				{
						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
						printk(KERN_INFO "\tu32AddrIdx = 0x%x, u32DIP = 0x%x, u32SIP = 0x%x, u32PortMask = 0x%x\n", u32AddrIdx, u32DIP, u32SIP, stRtkPortMask.bits[0]);
						#endif

						/* GDP had been added! */
						if(pstParams->u32DIP == u32DIP)
						{
								#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
								printk(KERN_INFO "\tGDP had been added! new del mask = 0x%x, org = 0x%x\n", (unsigned int)(0x1<<pstParams->u32PhyID), stRtkPortMask.bits[0]);
								#endif

								if((unsigned int)(0x1<<pstParams->u32PhyID) & stRtkPortMask.bits[0])
								{
										stRtkPortMask.bits[0] &= ~((unsigned int)(0x1<<pstParams->u32PhyID));
										pstParams->u32PortMask = stRtkPortMask.bits[0];

										#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
										printk(KERN_INFO "\tNew port mask = 0x%x\n", pstParams->u32PortMask);
										#endif

										if(pstParams->u32PortMask == 0)
												break;
										else
										{
												/* change port mask, not del */
												#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
												printk(KERN_INFO "\tchange port mask, not del\n");
												#endif
												
												if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_add((ipaddr_t)pstParams->u32SIP, (ipaddr_t)pstParams->u32DIP, (rtk_portmask_t)stRtkPortMask)))
												{
														#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
														printk(KERN_INFO "\tAdd group 0x%x into lookup table failed, errno = 0x%08x\n", pstParams->u32DIP, s32RetVal);
														#endif
														
														return s32RetVal;
												}

												return SWHAL_ERR_SUCCESS;
										}
								}
								else
								{
										#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
										printk(KERN_INFO "\tGDP had not been added into port id = %d\n", pstParams->u32PhyID);
										#endif
										
										return SWHAL_ERR_SUCCESS;
								}
						}
				}

				if(u32AddrIdx >= RTK_MAX_LUT_ADDR_ID)	break;

				u32AddrIdx++;
		}
		#else
		if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_get((ipaddr_t)pstParams->u32SIP, (ipaddr_t)pstParams->u32DIP, (rtk_portmask_t *)&stRtkPortMask)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
				printk(KERN_INFO "\tGroup had not been added!!!\n");
				#endif
				
				return s32RetVal;
		}
		else
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
				printk(KERN_INFO "\tGDP had been added! new del mask = 0x%x, org = 0x%x\n", (unsigned int)(0x1<<pstParams->u32PhyID), stRtkPortMask.bits[0]);
				#endif

				if((unsigned int)(0x1<<pstParams->u32PhyID) & stRtkPortMask.bits[0])
				{
						stRtkPortMask.bits[0] &= ~((unsigned int)(0x1<<pstParams->u32PhyID));
						pstParams->u32PortMask = stRtkPortMask.bits[0];

						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
						printk(KERN_INFO "\tNew port mask = 0x%x\n", pstParams->u32PortMask);
						#endif

						if(pstParams->u32PortMask == 0)
						{
								#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
								printk(KERN_INFO "\tGDP delete! mask = 0x%x\n", stRtkPortMask.bits[0]);
								#endif
						}
						else
						{
								/* change port mask, not del */
								#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
								printk(KERN_INFO "\tchange port mask, not del\n");
								#endif
								
								if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_add((ipaddr_t)pstParams->u32SIP, (ipaddr_t)pstParams->u32DIP, (rtk_portmask_t)stRtkPortMask)))
								{
										#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
										printk(KERN_INFO "\tAdd group 0x%x into lookup table failed, errno = 0x%08x\n", pstParams->u32DIP, s32RetVal);
										#endif
										
										return s32RetVal;
								}

								return SWHAL_ERR_SUCCESS;
						}
				}
				else
				{
						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT)) == (DBG_KERNEL_SPACE|DBG_IOCTL_10_L2LUT))
						printk(KERN_INFO "\tGDP had not been added into port id = %d\n", pstParams->u32PhyID);
						#endif
						
						return SWHAL_ERR_SUCCESS;
				}			
		}
		#endif
		
		/* Delete group in lookup table */
		if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_del((ipaddr_t)pstParams->u32SIP, (ipaddr_t)pstParams->u32DIP)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tDelete group 0x%x into lookup table failed, errno = 0x%08x\n", pstParams->u32DIP, s32RetVal);
				#endif
				
				return s32RetVal;
		}

		return SWHAL_ERR_SUCCESS;
}

#if 0
int rtl8367_ioctl_ll_l2lutunicasttbl_dump(void *pDevCtx, unsigned int *pu32Params)
{
		int s32RetVal;
		unsigned int u32AddrIdx = 0;

		rtk_l2_ucastAddr_t l2_data;

		/* Get all unicast entry at whole system*/
		while(1)
		{
				//printk(KERN_INFO "\t[%s] u32AddrIdx = 0x%x(%d)\n", __func__, u32AddrIdx, u32AddrIdx);

		    if(RT_ERR_OK != (s32RetVal = rtk_l2_addr_next_get((rtk_l2_read_method_t)READMETHOD_NEXT_L2UCSPA, (rtk_port_t)*pu32Params, (rtk_uint32 *)&u32AddrIdx, &l2_data)))
		    {
							//printk(KERN_INFO "[%s] u32AddrIdx = 0x%x, u32SIP = 0x%x, u32DIP = 0x%x, PortMask = 0x%x\n", __func__, u32AddrIdx, u32SIP, u32DIP, stRtkPortMask.bits[0]);
							//printk(KERN_INFO "[%s] error = 0x%x\n", __func__, s32RetVal);
		          break;
		    }


				printk(KERN_INFO "\t[%s] u32AddrIdx = 0x%x, MAC = %02x:%02x:%02x:%02x:%02x:%02x\n", __func__, u32AddrIdx, \
					l2_data.mac.octet[0], l2_data.mac.octet[1], l2_data.mac.octet[2], l2_data.mac.octet[3], l2_data.mac.octet[4], l2_data.mac.octet[5]);

				u32AddrIdx++;

		}

		return SWHAL_ERR_SUCCESS;
}


int rtl8367_ioctl_ll_l2lutipmulticasttbl_dump(void *pDevCtx, unsigned int *pu32Params)
{
		int s32RetVal;
		rtk_l2_addr_table_t stL2Entry;
		unsigned int u32DIP=0, u32SIP=0;
		unsigned int stRtkPortMask=0;
		unsigned int u32AddrIdx=0;

		unsigned char *pu8EntrySIP,*pu8EntryDIP, SIP[16], DIP[16];

    memset(&stL2Entry,0,sizeof(rtk_l2_addr_table_t));

		printk(KERN_INFO "\tRTK L2LUT ========================================================\n");
    printk(KERN_INFO "\tIndex    Source_IP    Destination_IP    MemberPort    State\n\n");

		while(1)
		{
				if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_next_get((rtk_uint32 *)&u32AddrIdx, (ipaddr_t *)&u32SIP, (ipaddr_t *)&u32DIP, (rtk_portmask_t *)&stRtkPortMask)))
				{
						//printk(KERN_INFO "Get L2 LUT next entry, errno = 0x%x\n", s32RetVal);
						break;
				}

				stL2Entry.index = u32AddrIdx+1;
				//printk(KERN_INFO "[%s] index = 0x%x, u32AddrIdx = 0x%x\n", __FUNCTION__, stL2Entry.index, u32AddrIdx);

				if(stL2Entry.index > RTK_MAX_LUT_ADDR_ID)	break;

		    if(RT_ERR_OK == (s32RetVal = rtk_l2_entry_get(&stL2Entry)))
		    {
						//dprintf("[%s] index = %d\n", __FUNCTION__, stL2Entry.index);
						//dprintf("[%s] index = 0x%x, dip = 0x%x, sip = 0x%x\n", __FUNCTION__, stL2Entry.index,stL2Entry.dip, stL2Entry.sip);

						#if 0
						if(stL2Entry.index == 2112)
						{
								dprintf("[%s] index = %d\n", __FUNCTION__, stL2Entry.index);
								break;
						}
						#endif

						if(stL2Entry.is_ipmul)
		        {
								pu8EntrySIP = (unsigned char *)&stL2Entry.sip;
								pu8EntryDIP = (unsigned char *)&stL2Entry.dip;
								sprintf(SIP, "%d.%d.%d.%d", pu8EntrySIP[0], pu8EntrySIP[1], pu8EntrySIP[2], pu8EntrySIP[3]);
								sprintf(DIP, "%d.%d.%d.%d", pu8EntryDIP[0], pu8EntryDIP[1], pu8EntryDIP[2], pu8EntryDIP[3]);
#if 0
		            printk(KERN_INFO "%05d  ", stL2Entry.index);
								printk(KERN_INFO "%15s    ", SIP);
								printk(KERN_INFO "%15s    ", DIP);
								printk(KERN_INFO "%08X    ", stL2Entry.portmask);
								//dprintf("%03d.%03d.%03d.%03d     ", pu8EntrySIP[0], pu8EntrySIP[1], pu8EntrySIP[2], pu8EntrySIP[3]);
								//dprintf("%03d.%03d.%03d.%03d     ", pu8EntryDIP[0], pu8EntryDIP[1], pu8EntryDIP[2], pu8EntryDIP[3]);
								printk(KERN_INFO "  %s\n", (stL2Entry.is_static? "Static" : "Auto"));
#else
		            printk(KERN_INFO "\t%05d %15s %15s   %08X %s\n", stL2Entry.index, SIP, DIP, stL2Entry.portmask, (stL2Entry.is_static? "Static" : "Auto"));
#endif
						}
		        else if(stL2Entry.mac.octet[0]&0x01)
		        {
								#if 0
		            //_rangeOutput(strBuffer,1,l2_entry.portmask);
		            dprintf("%4d  %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X %-21s %-4d %-4s %-5s %s %d\n",
		            stL2Entry.index,
		            stL2Entry.mac.octet[0], stL2Entry.mac.octet[1], stL2Entry.mac.octet[2],\
								stL2Entry.mac.octet[3], stL2Entry.mac.octet[4],	stL2Entry.mac.octet[5],\
		            strBuffer, stL2Entry.fid, (stL2Entry.auth ? "Auth" : "x"),\
		            (stL2Entry.sa_block? "Block" : "x"), (stL2Entry.is_static? "Static" : "Auto"),\
		            stL2Entry.age);
								#endif
		        }
		        else if((stL2Entry.age!=0)||(stL2Entry.is_static==1))
		        {
								#if 0
		            _rangeOutput(strBuffer,1,stL2Entry.portmask);
		            dprintf("%4d  %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X %-21s %-4d %-4s %-5s %s %d\n",
		            stL2Entry.index,
								stL2Entry.mac.octet[0],stL2Entry.mac.octet[1],stL2Entry.mac.octet[2],\
								stL2Entry.mac.octet[3],stL2Entry.mac.octet[4],stL2Entry.mac.octet[5],\
		            strBuffer, stL2Entry.fid, (stL2Entry.auth ? "Auth" : "x"),\
		            (stL2Entry.sa_block? "Block" : "x"), (stL2Entry.is_static? "Static" : "Auto"),\
		            stL2Entry.age);
								#endif
		        }
		    }
				else
				{
						//dprintf("[%s] Dump Look-up table entry failed, error = 0x%x\n", __FUNCTION__, s32RetVal);
						break;
				}

				u32AddrIdx++;

		}

		printk(KERN_INFO "\tEND ==============================================================\n\n");

		return SWHAL_ERR_SUCCESS;

}
#endif

/* [0x20~0x2F] =========================================================================== */ 
int rtl8367_ioctl_ll_portlinkcfg_get(void *pDevCtx, RTK_PORT_LINK_STATUS *pstParams)
{
		int s32RetVal;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG)) == (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG))
		printk(KERN_INFO "\tGet port id = %d link configuration\n", pstParams->u32PortNo);
		#endif

		if(RT_ERR_OK !=	(s32RetVal = rtk_port_macStatus_get((rtk_port_t)pstParams->u32PortNo, (rtk_port_mac_ability_t *)&pstParams->stMacStatus)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tGot port %d link configuration failed, errno = 0x%08x\n", pstParams->u32PortNo, s32RetVal);
				#endif
				
				return s32RetVal;
		}

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG)) == (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG))
		printk(KERN_INFO "\tu32Forcemode = %d\n", pstParams->stMacStatus.u32Forcemode);
		printk(KERN_INFO "\tu32Speed     = %d\n", pstParams->stMacStatus.u32Speed);
		printk(KERN_INFO "\tu32Duplex    = %d\n", pstParams->stMacStatus.u32Duplex);
		printk(KERN_INFO "\tu32Link      = %d\n", pstParams->stMacStatus.u32Link);
		printk(KERN_INFO "\tu32Nway      = %d\n", pstParams->stMacStatus.u32Nway);
		printk(KERN_INFO "\tu32Txpause   = %d\n", pstParams->stMacStatus.u32Txpause);
		printk(KERN_INFO "\tu32Rxpause   = %d\n", pstParams->stMacStatus.u32Rxpause);
		#endif

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_portlinkphycfg_get(void *pDevCtx, RTK_PORT_LINKPHY_STATUS *pstParams)
{
		int s32RetVal;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG)) == (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG))
		printk(KERN_INFO "\tGet port id = %d PHY configuration\n", pstParams->u32PortNo);
		#endif

		if(RT_ERR_OK !=	(s32RetVal = rtk_port_phyStatus_get((rtk_port_t)pstParams->u32PortNo, (rtk_port_linkStatus_t *)&pstParams->stPhyStatus.u32LinkStatus, (rtk_data_t*)&pstParams->stPhyStatus.u32Speed, (rtk_data_t*)&pstParams->stPhyStatus.u32Duplex)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tGot port %d PHY configuration failed, errno = 0x%08x\n", pstParams->u32PortNo, s32RetVal);
				#endif

				return s32RetVal;
		}

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG)) == (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG))
		printk(KERN_INFO "\tu32LinkStatus = %d\n", pstParams->stPhyStatus.u32LinkStatus);
		printk(KERN_INFO "\tu32Speed      = %d\n", pstParams->stPhyStatus.u32Speed);
		printk(KERN_INFO "\tu32Duplex     = %d\n", pstParams->stPhyStatus.u32Duplex);
		#endif

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_portforcelinkextcfg_set(void *pDevCtx, RTK_PORT_FORCELINKEXT_STATUS *pstParams)
{
		int s32RetVal;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG)) == (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG))
		printk(KERN_INFO "\tSet extension port id = %d\n", pstParams->u32ExtPortNo);
		#endif

		if(RT_ERR_OK !=	(s32RetVal = rtk_port_macForceLinkExt_set((rtk_port_t)pstParams->u32ExtPortNo, (rtk_mode_ext_t)pstParams->u32Mode, (rtk_port_mac_ability_t *)&pstParams->stMacStatus)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tSet port id = %d force link configuration failed, errno = 0x%08x\n", pstParams->u32ExtPortNo, s32RetVal);
				#endif
				
				return s32RetVal;
		}

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG)) == (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG))
		printk(KERN_INFO "\tu32Mode      = %d\n", pstParams->u32Mode);
		printk(KERN_INFO "\tu32Forcemode = %d\n", pstParams->stMacStatus.u32Forcemode);
		printk(KERN_INFO "\tu32Speed     = %d\n", pstParams->stMacStatus.u32Speed);
		printk(KERN_INFO "\tu32Duplex    = %d\n", pstParams->stMacStatus.u32Duplex);
		printk(KERN_INFO "\tu32Link      = %d\n", pstParams->stMacStatus.u32Link);
		printk(KERN_INFO "\tu32Nway      = %d\n", pstParams->stMacStatus.u32Nway);
		printk(KERN_INFO "\tu32Txpause   = %d\n", pstParams->stMacStatus.u32Txpause);
		printk(KERN_INFO "\tu32Rxpause   = %d\n", pstParams->stMacStatus.u32Rxpause);
		#endif

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_portforcelinkextcfg_get(void *pDevCtx, RTK_PORT_FORCELINKEXT_STATUS *pstParams)
{
		int s32RetVal;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG)) == (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG))
		printk(KERN_INFO "\tGot extension port id = %d\n", pstParams->u32ExtPortNo);
		#endif
		
		if(RT_ERR_OK !=	(s32RetVal = rtk_port_macForceLinkExt_get((rtk_port_t)pstParams->u32ExtPortNo, (rtk_mode_ext_t *)&pstParams->u32Mode, (rtk_port_mac_ability_t *)&pstParams->stMacStatus)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tGot port id = %d force link configuration failed, errno = 0x%08x\n", pstParams->u32ExtPortNo, s32RetVal);
				#endif
				
				return s32RetVal;
		}

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG)) == (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG))
		printk(KERN_INFO "\tu32Mode      = %d\n", pstParams->u32Mode);
		printk(KERN_INFO "\tu32Forcemode = %d\n", pstParams->stMacStatus.u32Forcemode);
		printk(KERN_INFO "\tu32Speed     = %d\n", pstParams->stMacStatus.u32Speed);
		printk(KERN_INFO "\tu32Duplex    = %d\n", pstParams->stMacStatus.u32Duplex);
		printk(KERN_INFO "\tu32Link      = %d\n", pstParams->stMacStatus.u32Link);
		printk(KERN_INFO "\tu32Nway      = %d\n", pstParams->stMacStatus.u32Nway);
		printk(KERN_INFO "\tu32Txpause   = %d\n", pstParams->stMacStatus.u32Txpause);
		printk(KERN_INFO "\tu32Rxpause   = %d\n", pstParams->stMacStatus.u32Rxpause);
		#endif

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_portrgmiiclkext_set(void *pDevCtx, RTK_PORT_RGMIIDELAYEXT *pstParams)
{
		int s32RetVal;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG)) == (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG))
		printk(KERN_INFO "\tSet extension port id = %d, tx delay = %d, rx delay = %d\n", pstParams->u32ExtPortNo, pstParams->u32TxDelay, pstParams->u32RxDelay);
		#endif

		if(RT_ERR_OK != (s32RetVal = rtk_port_rgmiiDelayExt_set((rtk_ext_port_t)pstParams->u32ExtPortNo, (rtk_data_t)pstParams->u32TxDelay, (rtk_data_t)pstParams->u32RxDelay)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tSet port id = %d rgmii delay failed, errno = 0x%08x\n", pstParams->u32ExtPortNo, s32RetVal);
				#endif
			
				return s32RetVal;
		}

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_portrgmiiclkext_get(void *pDevCtx, RTK_PORT_RGMIIDELAYEXT *pstParams)
{
		int s32RetVal;

		if(RT_ERR_OK != (s32RetVal = rtk_port_rgmiiDelayExt_get((rtk_ext_port_t)pstParams->u32ExtPortNo, (rtk_data_t *)&pstParams->u32TxDelay, (rtk_data_t *)&pstParams->u32RxDelay)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tGot port id = %d rgmii delay failed, errno = 0x%08x\n", pstParams->u32ExtPortNo, s32RetVal);
				#endif
			
				return s32RetVal;
		}

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG)) == (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG))
		printk(KERN_INFO "\tGot extension port id = %d, tx delay = %d, rx delay = %d\n", pstParams->u32ExtPortNo, pstParams->u32TxDelay, pstParams->u32RxDelay);
		#endif

		return SWHAL_ERR_SUCCESS;
}


int rtl8367_ioctl_ll_portphytestmode_set(void *pDevCtx, RTK_PORT_PHYTESTMODE *pstParams)
{
		int s32RetVal;
		int i;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG)) == (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG))
		printk(KERN_INFO "\tSet port id = %d, test mode = %d\n", pstParams->u32PhyNo, pstParams->u32Mode);
		#endif

		/* Reset all port to normal mode */
		for(i=0;i<4;i++)
		{
				if(RT_ERR_OK != (s32RetVal = rtk_port_phyTestModeAll_set((rtk_port_t)i, (rtk_port_phy_test_mode_t)PHY_TEST_MODE_NORMAL)))
				{
						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
						printk(KERN_INFO "\tReset port id = %d, test mode = %d failed, errno = 0x%08x\n", pstParams->u32PhyNo, pstParams->u32Mode);
						#endif
						
						return s32RetVal;
				}
		}

		/* Set test mode */
		if(RT_ERR_OK != (s32RetVal = rtk_port_phyTestModeAll_set((rtk_port_t)pstParams->u32PhyNo, (rtk_port_phy_test_mode_t)pstParams->u32Mode)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tSet port id = %d, test mode = %d failed, errno = 0x%08x\n", pstParams->u32PhyNo, pstParams->u32Mode, s32RetVal);
				#endif

				return s32RetVal;
		}

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_portphytestmode_get(void *pDevCtx, RTK_PORT_PHYTESTMODE *pstParams)
{
		int s32RetVal;

		if(RT_ERR_OK != (s32RetVal = rtk_port_phyTestModeAll_get((rtk_port_t)pstParams->u32PhyNo, (rtk_port_phy_test_mode_t *)&pstParams->u32Mode)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))				
				printk(KERN_INFO "\tGot port id = %d, test mode = %d failed, errno = 0x%08x\n", pstParams->u32PhyNo, pstParams->u32Mode, s32RetVal);
				#endif
			
				return s32RetVal;
		}

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG)) == (DBG_KERNEL_SPACE|DBG_IOCTL_20_PORTCFG))
		printk(KERN_INFO "\tGot port id = %d, test mode = %d\n", pstParams->u32PhyNo, pstParams->u32Mode);
		#endif

		return SWHAL_ERR_SUCCESS;
}

/* [0x50~0x5F] =========================================================================== */ 
int rtl8367_ioctl_ll_register_set(void *pDevCtx, RTK_REGISTER *pstParams)
{
		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_50_MISC)) == (DBG_KERNEL_SPACE|DBG_IOCTL_50_MISC))			
		printk(KERN_INFO "\tRegAddr = 0x%04x, RegVal = 0x%04x\n", pstParams->u32RegAddr, pstParams->u32Data);
		#endif
		
		RTL83XX_SMI_WRITE((unsigned short)pstParams->u32RegAddr, (unsigned short)pstParams->u32Data);

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_register_get(void *pDevCtx, RTK_REGISTER *pstParams)
{
		pstParams->u32Data = (unsigned int)RTL83XX_SMI_READ((unsigned short)pstParams->u32RegAddr);

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_50_MISC)) == (DBG_KERNEL_SPACE|DBG_IOCTL_50_MISC))		
		printk(KERN_INFO "\tGot RegAddr[0x%04x] = 0x%04x\n", pstParams->u32RegAddr, pstParams->u32Data);
		#endif

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_ll_portpower_onoff(void *pDevCtx, RTK_PORTPOWER	*pstParams)
{
		int s32RetVal;
		unsigned short u16RegVal;
		
		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_50_MISC)) == (DBG_KERNEL_SPACE|DBG_IOCTL_50_MISC))	
		printk(KERN_INFO "\tPort Power OnOff. PortID = %d, RegNo. = 0x%04x, RegVal = 0x%04x\n", pstParams->u32PortNo, pstParams->stRegAddrVal.u32RegAddr, pstParams->stRegAddrVal.u32Data);
		#endif

		/* Power On/Off */
		u16RegVal = RTL83XX_SMI_READ((unsigned short)(0x2000 + (pstParams->u32PortNo << 5) + pstParams->stRegAddrVal.u32RegAddr));
		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_50_MISC)) == (DBG_KERNEL_SPACE|DBG_IOCTL_50_MISC))	
		printk(KERN_INFO "\tRegister[0x%04x] is 0x%04x\n", (unsigned short)(0x2000 + (pstParams->u32PortNo << 5) + pstParams->stRegAddrVal.u32RegAddr), u16RegVal);
		#endif

		if((pstParams->stRegAddrVal.u32Data&DWBIT11) == DWBIT11) 	// Power down
				u16RegVal |= WBIT11;
		else																											// Power on
				u16RegVal &= ~WBIT11;
		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_IOCTL_50_MISC)) == (DBG_KERNEL_SPACE|DBG_IOCTL_50_MISC))	
		printk(KERN_INFO "\tRegister[0x%04x] is 0x%04x\n", (unsigned short)(0x2000 + (pstParams->u32PortNo << 5) + pstParams->stRegAddrVal.u32RegAddr), u16RegVal);
		#endif

		/* Set register */
		if(RT_ERR_OK != (s32RetVal = rtk_port_phyReg_set((rtk_port_t)pstParams->u32PortNo, (rtk_port_phy_reg_t)pstParams->stRegAddrVal.u32RegAddr, (rtk_port_phy_data_t)u16RegVal)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
				if((pstParams->stRegAddrVal.u32Data&DWBIT11) == DWBIT11)
						printk(KERN_INFO "\tPort down failed, errno = 0x%08x\n", s32RetVal);
				else
						printk(KERN_INFO "\tPort on failed, errno = 0x%08x\n", s32RetVal);
				#endif
				
				return s32RetVal;
		}

		/* Re-direct */
		pstParams->stRegAddrVal.u32RegAddr 	= (unsigned short)(0x2000 + (pstParams->u32PortNo << 5) + pstParams->stRegAddrVal.u32RegAddr);
		pstParams->stRegAddrVal.u32Data			= RTL83XX_SMI_READ(pstParams->stRegAddrVal.u32RegAddr);
			
		return SWHAL_ERR_SUCCESS;
}

