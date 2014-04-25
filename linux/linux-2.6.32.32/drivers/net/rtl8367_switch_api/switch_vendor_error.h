#ifndef __SWITCH_VENDOR_ERROR__
#define __SWITCH_VENDOR_ERROR__

typedef enum _switch_hal_error_code
{
    SWHAL_ERR_FAILED 			= -1,                       /* General Error               */

    /* 0x0x for common error code */
    SWHAL_ERR_SUCCESS			= 0,                       	/* OK                          */
		SWHAL_ERR_DRVPLUG 		= 1,												/* Invalid driver plug         */		
		SWHAL_ERR_RESET 			= 2,												/* Invalid reset pluse         */

		SWHAL_ERR_SYSINIT			= 3,												/* Invalid chip init           */
		SWHAL_ERR_L2INIT			= 4,
		SWHAL_ERR_HWIGMPINIT	= 5,
		SWHAL_ERR_CPUTAGINIT 	= 6,
		SWHAL_ERR_IOCINIT			= 7,
		SWHAL_ERR_PARAMS 		 	= 8,												/* Invalid input parameter     */
		SWHAL_ERR_IOCMD 			= 9,												/* Invalid ioctl cmd           */
		SWHAL_ERR_NOMEM				= 10,														
		//SWHAL_ERR_PORTEXT	= 7,
		//SWHAL_ERR_PORTMODE	= 8,

		/* 0x3x for IGMP error code */
    SWHAL_ERR_L2IGMPTBL_LASTGRP			= 0x30,           /* Last of IpMulGrp in Table   */

		#if 0
    RT_ERR_INPUT,                                   /* 0x00000001, invalid input parameter                                              */
    RT_ERR_UNIT_ID,                                 /* 0x00000002, invalid unit id                                                      */
    RT_ERR_PORT_ID,                                 /* 0x00000003, invalid port id                                                      */
    RT_ERR_PORT_MASK,                               /* 0x00000004, invalid port mask                                                    */
    RT_ERR_PORT_LINKDOWN,                           /* 0x00000005, link down port status                                                */
    RT_ERR_ENTRY_INDEX,                             /* 0x00000006, invalid entry index                                                  */
    RT_ERR_NULL_POINTER,                            /* 0x00000007, input parameter is null pointer                                      */
    RT_ERR_QUEUE_ID,                                /* 0x00000008, invalid queue id                                                     */
    RT_ERR_QUEUE_NUM,                               /* 0x00000009, invalid queue number                                                 */
    RT_ERR_BUSYWAIT_TIMEOUT,                        /* 0x0000000a, busy watting time out                                                */
    RT_ERR_MAC,                                     /* 0x0000000b, invalid mac address                                                  */
    RT_ERR_OUT_OF_RANGE,                            /* 0x0000000c, input parameter out of range                                         */
    RT_ERR_CHIP_NOT_SUPPORTED,                      /* 0x0000000d, functions not supported by this chip model                           */
    RT_ERR_SMI,                                     /* 0x0000000e, SMI error                                                            */
    RT_ERR_NOT_INIT,                                /* 0x0000000f, The module is not initial                                            */
    RT_ERR_CHIP_NOT_FOUND,                          /* 0x00000010, The chip can not found                                               */
    RT_ERR_NOT_ALLOWED,                             /* 0x00000011, actions not allowed by the function                                  */
    RT_ERR_DRIVER_NOT_FOUND,                        /* 0x00000012, The driver can not found                                             */
    RT_ERR_SEM_LOCK_FAILED,                         /* 0x00000013, Failed to lock semaphore                                             */
    RT_ERR_SEM_UNLOCK_FAILED,                       /* 0x00000014, Failed to unlock semaphore                                           */
    RT_ERR_ENABLE,                                  /* 0x00000015, invalid enable parameter                                             */
		#endif

}SWITCH_HAL_ERROR_CODE;

#endif
