/* ============================================================
Follow by 
This API package supports multiple Realtek Switch contains ASIC  
drivers for LINUX. ASIC drivers provide general APIs that based 
on user configuration to configure relative ASIC registers.


Programmer : Alvin Hsu, alvin_hsu@arcadyan.com
=============================================================== */
#ifndef __RTL8367_HAL_LINUX_IOCTL__
#define __RTL8367_HAL_LINUX_IOCTL__

/*-------------------------------------------------------------
  Include files
--------------------------------------------------------------- */
#include "switch_vendor_dbg.h"									/* local definitions */

#include "rtl83XX_hal.h"												/* local definitions */
#include <rtl8367_hal_linux_ioctl_extension.h>	/* common definitions */

/*-------------------------------------------------------------
  Definitions
--------------------------------------------------------------- */
#ifndef RTL8367_IOC_CONFIG_8021X
#define RTL8367_IOC_CONFIG_8021X 	1
#endif

#ifndef RTL8367_IOC_CONFIG_MULTICAST
#define RTL8367_IOC_CONFIG_MULTICAST 	1
#endif

#ifndef RTL8367_IOC_CONFIG_QOS
#define RTL8367_IOC_CONFIG_QOS 		1
#endif

#ifndef RTL8367_IOC_CONFIG_STP
#define RTL8367_IOC_CONFIG_STP 		1
#endif

#ifndef RTL8367_IOC_CONFIG_VLAN
#define RTL8367_IOC_CONFIG_VLAN 	1
#endif

#ifndef RTL8367_IOC_CONFIG_WOL
#define RTL8367_IOC_CONFIG_WOL 		1
#endif

#define  RTL8367_IOC_PORT			1
#define  RTL8367_IOC_VLAN			2
#define  RTL8367_IOC_SYS			3
#define  RTL8367_IOC_MISC			4

#if 0 // move to rtl8367_hal_linux_ioctl_extension.h
/* Check from Documentation/ioctl/ioctl-number.txt */
#define  RTL8367_IOC_MAGIC								0x21
#define  RTL8367_IO(reqType)							_IO(RTL8367_IOC_MAGIC, reqType)
#define  RTL8367_IOR(reqType, dataType)		_IOR(RTL8367_IOC_MAGIC, reqType, dataType)
#define  RTL8367_IOW(reqType, dataType)		_IOW(RTL8367_IOC_MAGIC, reqType, dataType)
#define  RTL8367_IOWR(reqType, dataType)	_IOWR(RTL8367_IOC_MAGIC, reqType, dataType)

#define  RTL8367_IOC_MAXNR 		99 

/*
I/O Command List

No. 						Class
[0x00~0x0F] 
0x00 : RTL8367_IOCTL_INIT
0x01 : RTL8367_IOCTL_MAXPKTLEN_SET
0x02 : RTL8367_IOCTL_MAXPKTLEN_GET
0x03 : RTL8367_IOCTL_PORTMAXPKTLEN_SET
0x04 : RTL8367_IOCTL_PORTMAXPKTLEN_GET
0x05 : RTL8367_IOCTL_CPUPORTTAGEN_SET
0x06 : RTL8367_IOCTL_CPUPORTTAGEN_GET
0x07 : RTL8367_IOCTL_CPUPORTTAG_SET
0x08 : RTL8367_IOCTL_CPUPORTTAG_GET
0x09 : 
0x0A : 
0x04 : 

[0x10~0x1F]			L2 LookUpTbl 
0x10 : RTL8367_IOCTL_L2LUT_INIT
0x11 : RTL8367_IOCTL_L2LUT_TYPE_SET
0x12 : RTL8367_IOCTL_L2LUT_TYPE_GET
0x13 :
0x14 :
0x15 : RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_ADD
0x16 : RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_DEL
0x17 : RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_DUMP

[0x20~0x2F]     Port MAC/PHY Configuration
0x20 : RTL8367_IOCTL_PORTLINKCFG_GET
0x21 : RTL8367_IOCTL_PORTLINKPHYCFG_GET
0x22 : RTL8367_IOCTL_PORTFORCELINKEXTCFG_SET
0x23 : RTL8367_IOCTL_PORTFORCELINKEXTCFG_GET
0x24 : RTL8367_IOCTL_PORTRGMIICLKEXT_SET
0x25 : RTL8367_IOCTL_PORTRGMIICLKEXT_GET
0x26 : RTL8367_IOCTL_PHYTESTMODE_SET
0x27 : RTL8367_IOCTL_PHYTESTMODE_GET
0x28 : 
0x29 : 
0x2A : 

[0x30~0x3F]			HW IGMP
0x30 :
0x31 :
0x32 : 
0x33 :
0x34 :
0x35 :
0x36 : 
0x37 :

[0x40~0x4F]			CPU Port 
0x40 : 
0x41 : 
0x42 : 
0x43 : 
0x44 : 
0x45 :
0x46 : 
0x47 :
0x48 : 
0x49 :

[0x50~0x5F]




*/




/* Set chip to default configuration enviroment */
#define RTL8367_IOCTL_INIT														RTL8367_IOR(0x00, unsigned int)

/* Set the max packet length for whole system setting, 1st configuration */
#define RTL8367_IOCTL_MAXPKTLEN_SET										0x01

/* Get the max packet length for whole system setting, 1st configuration */
#define RTL8367_IOCTL_MAXPKTLEN_GET										0x02

/* Set the max packet length for certain port setting, 2st configuration */
#define RTL8367_IOCTL_PORTMAXPKTLEN_SET								0x03

/* Get the max packet length for certain port setting, 2st configuration */
#define RTL8367_IOCTL_PORTMAXPKTLEN_GET								0x04

/* Set cpu tag function enable/disable, If CPU tag function is disabled,  
	 CPU tag will not be added to frame forwarded to CPU port, and all ports 
	 cannot parse CPU tag. */
#define	RTL8367_IOCTL_CPUPORTTAGEN_SET								0x05

/* Get cpu tag function enable/disable */
#define	RTL8367_IOCTL_CPUPORTTAGEN_GET								0x06

/* Set CPU port and inserting proprietary CPU tag mode to the frame that 
	 transmitting to CPU port. */
#define	RTL8367_IOCTL_CPUPORTTAG_SET									0x07

/* Get cpu tag function enable/disable */
#define	RTL8367_IOCTL_CPUPORTTAG_GET									0x08



/* Initialize l2 module of the specified device. */
#define RTL8367_IOCTL_L2LUT_INIT											0x10

/* Set Lut IP multicast lookup function
	 If users set the lookup type to DIP, the group in exception table will be lookup by DIP+SIP
   If users set the lookup type to DIP+SIP, the group in exception table will be lookup by only DIP */
#define RTL8367_IOCTL_L2LUT_TYPE_SET									0x11

/* Get Lut IP multicast lookup function */
#define RTL8367_IOCTL_L2LUT_TYPE_GET									0x12

/* Adds a multicast group configuration to the multicast table. 
   No new entry is added in case this multicast group already exists in the table. 
   This commands adds a host member to the multicast group.*/
#define RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_ADD				RTL8367_IOW(0x15, RTL8367_L2LUT_STATUS)

/* Remove an host member from a multicast group. 
   The multicast group entry is completely removed from the multicast table in case it has no host member port left. */
#define RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_DEL				RTL8367_IOWR(0x16, RTL8367_L2LUT_STATUS)

/* Read out the multicast membership table that is located inside the switch hardware. */
//#define RTL8367_IOCTL_L2LUT_IPMULTICASTADDR_DUMP      RTL8367_IOR(0x17, IFX_ETHSW_multicastTableRead_t)

/* Get port linking configuration. */
#define RTL8367_IOCTL_PORTLINKCFG_GET									0x20

/* Get port PHY status configuration. */
#define RTL8367_IOCTL_PORTLINKPHYCFG_GET							0x21

/* Set port force linking configuration. */
#define RTL8367_IOCTL_PORTFORCELINKEXTCFG_SET					0x22

/* Get port force linking configuration. */
#define RTL8367_IOCTL_PORTFORCELINKEXTCFG_GET					0x23

/* Set port force linking configuration. */
#define RTL8367_IOCTL_PORTRGMIICLKEXT_SET							0x24

/* Get RGMII interface delay value for TX and RX. */
#define RTL8367_IOCTL_PORTRGMIICLKEXT_GET							0x25

/* Set PHY in test mode and only one PHY can be in test mode at the same time. */
#define RTL8367_IOCTL_PHYTESTMODE_SET									0x26
/* Get PHY in which test mode. */
#define RTL8367_IOCTL_PHYTESTMODE_GET									0x27
#endif

/*-------------------------------------------------------------
  RTL8367RB HAL structure
--------------------------------------------------------------- */
#if 0 // move to rtl8367_hal_linux_ioctl_extension.h
typedef struct _rtl8367_l2lut_status {
		unsigned int u32LookUpTblType;
		
		unsigned int u32DIP; 
		unsigned int u32SIP; 
		unsigned int u32PortMask;

		unsigned char u8L2Init;
}RTL8367_L2LUT_STATUS;
#endif

typedef enum
{
    RTL8367_API_USERAPP = 0,
    RTL8367_API_KERNEL,
    RTL8367_API_END
}RTL8367_API_TYPE;

/* general declaration fits for all low-level functions. */
typedef int (*rtl8367_ll_fkt) (void *, unsigned int);
typedef struct _rtl8367_lowlevelfkt RTL8367_LOWLEVELFKT;

#undef RTL8367_ETHSW_FKT
#define RTL8367_ETHSW_FKT(x, y) x? (rtl8367_ll_fkt)y : RTK_NULL

/* Switch API low-level function tables to map all supported IOCTL commands */
struct _rtl8367_lowlevelfkt{
   /* Some device have multiple tables to split the generic switch API features
      and the device specific switch API features. Additional tables, if exist,
      can be found under this next pointer. Every table comes along with a
      different 'nType' parameter to differentiate. */
   RTL8367_LOWLEVELFKT         *pstNextFkt;
   /* IOCTL type of all commands listed in the table. */
   unsigned short               u16Type;
   /* Number of low-level functions listed in the table. */
   unsigned int                 u32NumFkts;
   /* Pointer to the first entry of the ioctl number table. This table is used
      to check if the given ioctl command fits the the found low-level function
      pointer under 'pFkts'. */
   // IFX_uint32_t                     *pIoctlCmds;
   /* Pointer to the first entry of the function table. Table size is given
      by the parameter 'nNumFkts'. */
   rtl8367_ll_fkt              *pstFkts;
};

typedef struct _rtl8367_ioc_handle {
   RTL8367_LOWLEVELFKT *pstLlTable;
   void *pLlHandle;
   char s8ParamBuffer[2048/*PARAM_BUFFER_SIZE*/];
}RTL8367_IOC_HANDLE;

typedef struct _rtl8367_ioc_wrapper_ctx
{
   RTL8367_IOC_HANDLE	*pstIoctlHandle;
   /** Array of pEthSWDev pointers associated with this driver context */
   void *pEthSWDev[1/*IFX_ETHSW_DEV_MAX*/];
}RTL8367_IOC_WRAPPER_CTX;

typedef struct _rtl8367_ioc_wrapperinit {
   RTL8367_LOWLEVELFKT 			*pstLlTable;
}RTL8367_IOC_WRAPPERINIT;


/*-------------------------------------------------------------
  Debug Message/Macros
--------------------------------------------------------------- */
#ifdef RTL8367_HAL_IOCTL_DEBUG
//#define IOMSG(string, args...)		printk(KERN_DEBUG "[%s:%d]rtl8367_ioc:" __FILE__ __LINE__ string, ##args)

#ifdef __KERNEL__	
/* This one if debugging is on, and kernel space */
#define IODEBUG(fmt, args...) 		printk(KERN_DEBUG "rtl8367_ioc: " fmt, ## args)
#else
/* This one for user space */
#define IODEBUG(fmt, args...) 		fprintf(stderr, fmt, ## args)
#endif

#else
#define IOMSG(string, args...)
#endif


/*-------------------------------------------------------------
  Function Declaration
--------------------------------------------------------------- */
#define RTL8367_HAL_IOCTL_FUN

#ifdef RTL8367_HAL_IOCTL_FUN							
#define RTL8367_HAL_IOCTL_INTERFACE	
#else
#define RTL8367_HAL_IOCTL_INTERFACE	extern
#endif

RTL8367_HAL_IOCTL_INTERFACE void *rtl8367_ioctl_wrapper_init(void);
RTL8367_HAL_IOCTL_INTERFACE int rtl8367_ioctl_wrapper_cleanup(void);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl(unsigned int nCmd, unsigned long nArg);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_lowlevelcommandsearch(void *pHandle, int s32Cmd, int s32Arg, RTL8367_API_TYPE Type);


RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_switch_init(void *pDevCtx, unsigned int u32Params);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_maxpktlength_set(void *pDevCtx, RTK_PORT_MAXPKTLEN *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_maxpktlength_get(void *pDevCtx, RTK_PORT_MAXPKTLEN *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_portmaxpktlength_set(void *pDevCtx, RTK_PORT_MAXPKTLEN *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_portmaxpktlength_get(void *pDevCtx, RTK_PORT_MAXPKTLEN *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_CPUporttagenable_set(void *pDevCtx, unsigned int *pu32Params);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_CPUporttagenable_get(void *pDevCtx, unsigned int *pu32Params);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_CPUporttagcfg_set(void *pDevCtx, RTK_CPUPORT_TAG	*pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_CPUporttagcfg_get(void *pDevCtx, RTK_CPUPORT_TAG *pstParams);


RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_l2lut_init(void *pDevCtx, unsigned int u32Params);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_l2luttype_set(void *pDevCtx, unsigned int *pu32Params);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_l2luttype_get(void *pDevCtx, unsigned int *pu32Params);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_l2mactableentry_get(void *pDevCtx, RTK_L2LUT_STATUS *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_l2mactableentry_getnext(void *pDevCtx, RTK_L2LUT_STATUS *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_l2ipmulticasttableentry_getnext(void *pDevCtx, RTK_L2LUT_STATUS *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_l2ipmulticasttableentry_add(void *pDevCtx, RTK_L2LUT_STATUS *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_l2ipmulticasttableentry_remove(void *pDevCtx, RTK_L2LUT_STATUS *pstParams);


RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_portlinkcfg_get(void *pDevCtx, RTK_PORT_LINK_STATUS *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_portlinkphycfg_get(void *pDevCtx, RTK_PORT_LINKPHY_STATUS *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_portforcelinkextcfg_set(void *pDevCtx, RTK_PORT_FORCELINKEXT_STATUS *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_portforcelinkextcfg_get(void *pDevCtx, RTK_PORT_FORCELINKEXT_STATUS *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_portrgmiiclkext_set(void *pDevCtx, RTK_PORT_RGMIIDELAYEXT *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_portrgmiiclkext_get(void *pDevCtx, RTK_PORT_RGMIIDELAYEXT *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_portphytestmode_set(void *pDevCtx, RTK_PORT_PHYTESTMODE *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_portphytestmode_get(void *pDevCtx, RTK_PORT_PHYTESTMODE *pstParams);


RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_register_set(void *pDevCtx, RTK_REGISTER *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_register_get(void *pDevCtx, RTK_REGISTER *pstParams);
RTL8367_HAL_IOCTL_INTERFACE	int rtl8367_ioctl_ll_portpower_onoff(void *pDevCtx, RTK_PORTPOWER	*pstParams);

#endif // __RTL83XX_HAL_IOCTL__
