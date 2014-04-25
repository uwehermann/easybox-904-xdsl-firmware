/* ============================================================
Follow by 
This API package supports multiple Realtek Switch contains ASIC  
drivers for LINUX. ASIC drivers provide general APIs that based 
on user configuration to configure relative ASIC registers.


Programmer : Alvin Hsu, alvin_hsu@arcadyan.com
=============================================================== */
#ifndef __RTL8367_HAL_LINUX__
#define __RTL8367_HAL_LINUX__

/*-------------------------------------------------------------
  Definitions
--------------------------------------------------------------- */


/*-------------------------------------------------------------
  Include files
--------------------------------------------------------------- */
#include <linux/cdev.h>
#include "rtl8367_hal_linux_ioctl.h"

#if(_RTK_RTL83XX_SWITCH_API == 1)
#error "No available switch api, version error!!"
#elif(_RTK_RTL83XX_SWITCH_API == 2)
#include "rtl8367rb/switch_api_v1_2_10/include/rtk_api.h"
#endif

/*-------------------------------------------------------------
  RTL8367RB DEFINITION
--------------------------------------------------------------- */
#if defined(RTK_X86_ASICDRV)
#elif defined(CHIP_RTL8363SB)
#elif defined(CHIP_RTL8365MB)
#elif defined(CHIP_RTL8367_VB)
#elif defined(CHIP_RTL8367RB)
#define RTK_SWITCH_DRV_MODULE_VERSION		"1.0" 
#define RTK_MAJOR												177 
#define RTK_DEV_NAME										"rtl8367rb gigabit switch driver"
#define RTK_RELDATE											"2012/09/27"
#elif defined(CHIP_RTL8367R_VB)
#elif defined(CHIP_RTL8367M_VB)
#elif defined(CHIP_RTL8367MB)
#elif defined(CHIP_RTL8368MB)
#elif defined(CHIP_RTL8305MB)
#elif defined(CHIP_RTL8307M_VB)
#else
#error "NO defined CHIP_RTLXXXX for Linux init-params!" 
#endif




/*-------------------------------------------------------------
  RTL8367RB HAL structure
--------------------------------------------------------------- */ 
typedef struct _rtl8367_dev_state {
		struct cdev cdev;	  			/* Char device structure		*/
		
}RTL8367_DEV_STATE;

typedef struct _rtl8367_switch_state {

		RTK_PORT_FORCELINKEXT_STATUS stPortExtStatus[EXT_PORT_END];
		RTK_PORT_RGMIIDELAYEXT	stPortExtDelay[EXT_PORT_END];

		RTK_L2LUT_STATUS	stL2LUTStatus;
		
		unsigned char					u8CPUPort;
		unsigned char					u8SysInit;
}RTL8367_SWITCH_STATE;





/*-------------------------------------------------------------
  Debug Message/Macros
--------------------------------------------------------------- */
#ifdef RTL83XX_HAL_LINUX_DEBUG
#define RTKMSG(string, args...)		printk(KERN_DEBUG "[%s]", __func__, string, ##args)

#ifdef __KERNEL__	
/* This one if debugging is on, and kernel space */
#define RTKDEBUG(fmt, args...) 		printk(KERN_DEBUG "rtk: " fmt, ##args)
#else
/* This one for user space */
#define RTKDEBUG(fmt, args...) 		fprintf(stderr, fmt, ##args)
#endif

#else
#define RTKMSG(string, args...)
#endif

/*-------------------------------------------------------------
  Function Declaration
--------------------------------------------------------------- */
#define RTL83XX_HAL_LINUX_FUN

#ifdef RTL83XX_HAL_LINUX_FUN							
#define RTL83XX_HAL_LINUX_INTERFACE	
#else
#define RTL83XX_HAL_LINUX_INTERFACE	extern
#endif

//RTL83XX_HAL_LINUX_INTERFACE
//RTL83XX_HAL_LINUX_INTERFACE
//RTL83XX_HAL_LINUX_INTERFACE
//RTL83XX_HAL_LINUX_INTERFACE
//RTL83XX_HAL_LINUX_INTERFACE
//RTL83XX_HAL_LINUX_INTERFACE
//RTL83XX_HAL_LINUX_INTERFACE
//RTL83XX_HAL_LINUX_INTERFACE


#endif // __RTL83XX_HAL_LINUX__
