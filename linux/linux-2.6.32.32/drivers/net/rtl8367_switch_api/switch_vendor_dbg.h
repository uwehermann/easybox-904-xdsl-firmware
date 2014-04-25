#ifndef __SWITCH_VENDOR_DEBUG__
#define __SWITCH_VENDOR_DEBUG__

#include "switch_vendor_common.h"
#include "switch_vendor_project.h"

#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_LINUX)
#if defined(__KERNEL__)
#include <linux/kernel.h> 			/* printk() 			*/ 
#endif
#endif

/*
	The length modifier for printf

	Here, "integer conversion" stands for d, i, o, u, x, or X conversion.

	hh     A following integer conversion corresponds to a signed  char  or
	      unsigned  char argument, or a following n conversion corresponds
	      to a pointer to a signed char argument.

	h      A following integer conversion corresponds to  a  short  int  or
	      unsigned  short int argument, or a following n conversion corre-
	      sponds to a pointer to a short int argument.

	l      (ell) A following integer conversion corresponds to a  long  int
	      or  unsigned long int argument, or a following n conversion cor-
	      responds to a pointer to a long int argument, or a  following  c
	      conversion  corresponds  to  a wint_t argument, or a following s
	      conversion corresponds to a pointer to wchar_t argument.

	ll     (ell-ell).  A following integer conversion corresponds to a long
	      long  int  or  unsigned long long int argument, or a following n
	      conversion corresponds to a pointer to a long long int argument.

	L      A following a, A, e, E, f, F, g, or G conversion corresponds  to
	      a long double argument.  (C99 allows %LF, but SUSv2 does not.)
	q      ("quad".  4.4BSD  and  Linux libc5 only.  Don't use.)  This is a
	      synonym for ll.

	j      A following integer conversion corresponds  to  an  intmax_t  or
	      uintmax_t argument.

	z      A  following  integer  conversion  corresponds  to  a  size_t or
	      ssize_t argument.  (Linux libc5 has Z with this meaning.   Don't
	      use it.)

	t      A  following integer conversion corresponds to a ptrdiff_t argu?
	      ment.
*/

/*-------------------------------------------------------------
  Global Debug Message
--------------------------------------------------------------- */
/* switch_vendor_hal.c */
#define SWITCH_VENDOR_HAL_DEBUG				SW_ENABLED		
#define SWITCH_VENDOR_HAL_DEBUG_DUMP	SW_ENABLED

/* rtl83XX_hal.c */
#define RTL8367B_HAL_DEBUG						SW_ENABLED
#define RTL8367B_HAL_DEBUG_DUMP				SW_ENABLED

/* rtl8367_hal_linux.c */
#ifdef PROC_DEBUG
#undef PROC_DEBUG
#endif
#define PROC_DEBUG										SW_ENABLED
#define RTL83XX_HAL_LINUX_DEBUG				SW_ENABLED	

#if defined(RTL83XX_HAL_LINUX_DEBUG) && (RTL83XX_HAL_LINUX_DEBUG == 1)
#define DBG_MODULE_INIT								DWBIT00				
#define DBG_SWITCH_INIT								DWBIT01				
#define DBG_SWITCH_OPEN								DWBIT02				
#define DBG_SWITCH_READ								DWBIT03				
#define DBG_SWITCH_WRITE							DWBIT04			
#define DBG_SWITCH_IOCTL							DWBIT05			
#define DBG_SWITCH_RELEASE						DWBIT06	 	
#if(PROC_DEBUG == 1)
#define DBG_PROCFS_CREATE							DWBIT07			
#define DBG_PROCFS_READ								DWBIT08				
#define DBG_PROCFS_WRITE							DWBIT09			
#define DBG_PROCFS_DESTORY						DWBIT10		
#define DBG_PROCFS										DWBIT11						
#endif
#define DBG_REGISTER_DUMP							DWBIT12			
#define DBG_PARAMETER_DUMP						DWBIT13	

#define DBG_ERROR											DWBIT14

#define DBG_IOCTL_00_ADMIN						DWBIT15
#define DBG_IOCTL_10_L2LUT						DWBIT16
#define DBG_IOCTL_20_PORTCFG					DWBIT17
#define DBG_IOCTL_30_									DWBIT18
#define DBG_IOCTL_40_									DWBIT19
#define DBG_IOCTL_50_MISC							DWBIT20	

#define DBG_KERNEL_SPACE							DWBIT31

#if 0
#define DBG_SWITCH_LEVEL							(	DBG_KERNEL_SPACE |\
																				DBG_MODULE_INIT  | DBG_SWITCH_INIT | DBG_PROCFS | DBG_ERROR |\
																		 		DBG_IOCTL_00_ADMIN | DBG_IOCTL_10_L2LUT | DBG_IOCTL_20_PORTCFG | DBG_IOCTL_50_MISC)	
#else
#define DBG_SWITCH_LEVEL							( DBG_ERROR |\
																		 		DBG_IOCTL_00_ADMIN | DBG_IOCTL_10_L2LUT | DBG_IOCTL_20_PORTCFG | DBG_IOCTL_50_MISC)	
#endif													

#if defined(DBG_SWITCH_LEVEL) && ((DBG_SWITCH_LEVEL & DBG_KERNEL_SPACE) == 0x0)
#define DBG_USER_SPACE								SW_ENABLED
#endif

#endif



/* rtl8367_hal_linux_ioctl.c */
#define RTL8367_HAL_IOCTL_DEBUG				SW_ENABLED

#define COLOR_DEBUG_MSG								SW_ENABLED


#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_LINUX)
#if defined(__KERNEL__)
/* This one if debugging is on, and kernel space */
#define iprintf		printk
#define dprintf		printk
#else
/* This one for user space */
#define iprintf		fprintf
#define dprintf		fprintf
#endif

#elif((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
//#define dprintf	iprintf
#endif

#if(COLOR_DEBUG_MSG == SW_ENABLED)
#define SWCLR0_RESET							"\33[0m"
#define SWCLR1_30_BLACK						"\33[1;30m"
#define SWCLR1_31_RED							"\33[1;31m"
#define SWCLR1_32_GREEN						"\33[1;32m"
#define SWCLR1_33_YELLOW					"\33[1;33m"
#define SWCLR1_34_BLUE						"\33[1;34m"
#define SWCLR1_35_MAGENTA					"\33[1;35m"
#define SWCLR1_36_CYAN						"\33[1;36m"
#define SWCLR1_37_WHITE						"\33[1;37m"
#else
#define SWCLR0_RESET							""
#define SWCLR1_30_BLACK						""
#define SWCLR1_31_RED							""
#define SWCLR1_32_GREEN						""
#define SWCLR1_33_YELLOW					""
#define SWCLR1_34_BLUE						""
#define SWCLR1_35_MAGENTA					""
#define SWCLR1_36_CYAN						""
#define SWCLR1_37_WHITE						""
#endif // USB_COLOR_DEBUG_MSG


#endif // __SWITCH_VENDOR_DEBUG__
