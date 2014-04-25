/* ============================================================
Follow by 
This API package supports multiple Realtek Switch contains ASIC  
drivers. ASIC drivers provide general APIs that based on user 
configuration to configure relative ASIC registers.

Programmer : Alvin Hsu, alvin_hsu@arcadyan.com.tw
=============================================================== */
#ifndef _SWITCH_VENDOR_HAL_
#define _SWITCH_VENDOR_HAL_

/*-------------------------------------------------------------
  Include files
--------------------------------------------------------------- */
#include "switch_vendor_common.h"
#include "switch_vendor_dbg.h"
#include "switch_vendor_project.h"
#include "switch_vendor_error.h"

#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_LINUX)
#include <linux/skbuff.h>								/* struct sk_buff */
#include <linux/delay.h>

#elif((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
#include "StdType.h"
#include "ifxmips_sw_reg.h"
#include "vr9_hw.h"
#include "vr9.h"
#include "ifxmips_eth.h"

#include "feature.h"

#include "buffer.h"
#include "mtmacro.h"
#include "convert.h"
#endif

#ifdef SWITCH_REALTEK_RTL83XX_FAMILY
#include "rtl83XX_hal.h"
#endif

/*-------------------------------------------------------------
  System Definition
--------------------------------------------------------------- */
/* Switch API Version */
#define SWITCH_CHIP_NAME_NONE 			"No Switch Chip!"
#define SWITCH_CHIP_VERSION_NONE 		"No Switch Chip Version!"

/* Switch Chip Number */
#define	SWITCH_CHIP_ID_NONE					-1
#define SWITCH_CHIP_ID_INTERNAL			0xF			

#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
#define sk_buff		buf_s

#ifndef udelay
#define udelay		sysUDelay
#endif

#ifdef mdelay
#undef mdelay
#endif

#ifndef mdelay
#define mdelay(m)	udelay(1000*m)
#endif
#endif 

#define SW_DLY_TICKS	1
#define SW_DLY_SECS		2

#define	SW_CLOCKHZ		1000		/* 1/100 second (10ms) clock */
#define SW_MSECTOTICKS(msecs) (unsigned long)(((unsigned long)msecs*SW_CLOCKHZ)/1000)

/*-------------------------------------------------------------
	Structure, Enum
--------------------------------------------------------------- */
typedef struct _switch_device_file_operations {
    int (*init)(void);														/* entry point for initializing */
		int (*reset)(unsigned int u32PluseWidth);			/* entry point for sw-reset */
		int (*restart)(void);													/* entry point for restart */
		int (*shutdown)(void);			            			/* entry point for shutdown */
    int (*send)(void);														/* entry point for sending  */
    int (*intr)(void);														/* entry point for receving */
    int (*ioctl)(unsigned int);										/* entry point for io control */
}SWITCH_DEVICE_FILE_OPERATIONS;

typedef struct _switch_device_info {
    char *ps8Name;																/* Switch chip name string */
		char *ps8Version;															/* Switch chip API version string */

		int		s32ChipNo;															/* Switch chip number */
		char 	s8FlgInit;															/* Switch chip init flag */

		SWITCH_DEVICE_FILE_OPERATIONS	*pstSwDevFOps;	/* Switch device file operations */	
}SWITCH_DEVICE_INFO;

typedef struct _switch_device_state {
		SWITCH_DEVICE_INFO stSwDevInfo;								/* Switch device state */	
		#ifdef SWITCH_REALTEK_RTL83XX_FAMILY
		RTK_CPU_PORT_STATUS stRtkCPUPortStatus;
		#endif
}SWITCH_DEVICE_STATE;

/*-------------------------------------------------------------
  Debug Message, See switch_vendor_dbg.h
--------------------------------------------------------------- */
#if(SWITCH_VENDOR_HAL_DEBUG == SW_ENABLED)			
#define SWITCH_VENDER_HAL_MSG(msg)							msg;
#define SWITCH_VENDER_HAL_DBG(format, args...)	iprintf("[%s:%d]"format,__FUNCTION__, __LINE__,##args);
#define SWITCH_VENDER_DBG(format, args...)     	iprintf("[%s:%d]",__FUNCTION__, __LINE__);iprintf(format,##args);
//#define SWITCH_VENDER_DBG(format, args...)     	iprintf("[%s:%d]"format,__FILE__, __LINE__,##args);
#else
#define SWITCH_VENDER_HAL_MSG(msg)     			
#define SWITCH_VENDER_HAL_DBG(format, args...)		
#define SWITCH_VENDER_HAL_DMP(msg)     			
#endif

/*-------------------------------------------------------------
  Function Declaration
--------------------------------------------------------------- */
#define SWITCH_VENDER_HAL_FUN

#ifdef SWITCH_VENDER_HAL_FUN							
#define SWITCH_VENDER_HAL_FUN_INTERFACE	
#else
#define SWITCH_VENDER_HAL_FUN_INTERFACE	extern
#endif

SWITCH_VENDER_HAL_FUN_INTERFACE int Util_SwitchDrvIdx(SWITCH_DEVICE_INFO *pstDevice, char *ps8ChipName);
#if(SWITCH_VENDOR_HAL_DEBUG_DUMP == SW_ENABLED)
SWITCH_VENDER_HAL_FUN_INTERFACE void Dump_DevState(SWITCH_DEVICE_STATE *pstDevState);
#endif

SWITCH_VENDER_HAL_FUN_INTERFACE int Switch_Hal_Init(void);
SWITCH_VENDER_HAL_FUN_INTERFACE int Switch_Hal_LinkUpStatus_Get(unsigned int *pu32PhyNo, unsigned char *pu8LinkUp, unsigned char u8ChipOpt);
#ifdef SWITCH_REALTEK_RTL83XX_FAMILY
SWITCH_VENDER_HAL_FUN_INTERFACE int Switch_Hal_CPUPortNo_Get(struct sk_buff *pstSkb, unsigned char *pu8PortNo);
#endif
SWITCH_VENDER_HAL_FUN_INTERFACE int Switch_Hal_StripeTag(struct sk_buff *pstSkb, unsigned int u32Shift, unsigned char u8TagLen);

#endif // _SWITCH_VENDER_HAL_

