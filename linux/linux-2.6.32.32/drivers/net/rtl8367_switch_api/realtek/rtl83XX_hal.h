/* ============================================================
Follow by 
This API package supports multiple Realtek Switch contains ASIC  
drivers. ASIC drivers provide general APIs that based on user 
configuration to configure relative ASIC registers.

Programmer : Alvin Hsu, alvin_hsu@arcadyan.com.tw
=============================================================== */
#ifndef __RTL83XX_HAL__
#define __RTL83XX_HAL__

/*-------------------------------------------------------------
  Definitions
--------------------------------------------------------------- */
/* 
*		_RTK_RTL83XX_SWITCH_API				: Using RTK switch api version
*
*		SWITCH_REALTEK_RTL83XX_FAMILY :	Using RTK switch 
*		
*/

/*-------------------------------------------------------------
  Include files
--------------------------------------------------------------- */
#include "switch_vendor_project.h"		/* local definitions */
#include "switch_vendor_dbg.h"				/* local definitions */
#include "switch_vendor_common.h"			/* local definitions */

/*-------------------------------------------------------------
  RTL8367b DEFINITION
--------------------------------------------------------------- */
#if defined(CHIP_RTL8367RB)
/* MAC */
#define RTL8367B_MAC_DISABLE		-1 

#define RTL8367B_MAC_0					0		/* RJ-45 MAC 0 */
#define RTL8367B_MAC_1					1		/* RJ-45 MAC 1 */
#define RTL8367B_MAC_2					2		/* RJ-45 MAC 2 */
#define RTL8367B_MAC_3					3		/* RJ-45 MAC 3 */
#define RTL8367B_MAC_4					4		/* RJ-45 MAC 4 */
#if defined(_Vendor_TF_FTTH) || defined(PLATFORM_904DSL)
#define RTL8367B_MAC_LAN_END		4		/* Telefornica use 4 Lan port (0~3) */	
#else
#define RTL8367B_MAC_LAN_END		5   /* RTL8367B have 5 Lan port (0~4) */
#endif

#define RTL8367B_MAC_5_EXT			5		/* EXT ForceLink MAC 0 */
#define RTL8367B_MAC_6_EXT			6		/* EXT ForceLink MAC 1 */
#define RTL8367B_MAC_7_EXT			7		/* EXT ForceLink MAC 2 */
#define RTL8367B_MAC_END				8		/* RTL8367B have 8 MAC */

/* PHY */
#define RTL8367B_PHY_0					0		/* RJ-45 PHY 0 */
#define RTL8367B_PHY_1					1		/* RJ-45 PHY 1 */
#define RTL8367B_PHY_2					2		/* RJ-45 PHY 2 */
#define RTL8367B_PHY_3					3		/* RJ-45 PHY 3 */
#define RTL8367B_PHY_4					4		/* RJ-45 PHY 4 */
#if defined(_Vendor_TF_FTTH) || defined(PLATFORM_904DSL)
#define RTL8367B_PHY_END				4   /* Telefornica use 4 Lan port (0~3) */
#else
#define RTL8367B_PHY_END				5   /* RTL8367B have 5 Lan port */
#endif

#define RTL8367B_MAC_MAX				(RTL8367B_MAC_END-1)			/* RTL8367B have (0~7) port */
#define RTL8367B_MAC_NUM				RTL8367B_MAC_END					/* RTL8367B total 8 port */
#define RTL8367B_MAC_LAN_NUM		RTL8367B_MAC_LAN_END				
#define	RTL8367B_PHY_NUM        RTL8367B_PHY_END	

#define RTL8367B_MAC_EXT_NUM		2													/* RTL8367B use 2 ext port */
#endif

/*-------------------------------------------------------------
  REALTEK DEFINITION
--------------------------------------------------------------- */
/* Chip ID */
#define CHIP_ID_NONE								-1
#define CHIP_ID_RTL8363SB						0x1
#define CHIP_ID_RTL8365MB						0x2
#define CHIP_ID_RTL8367VB						0x3
#define CHIP_ID_RTL8367RB						0x4
#define CHIP_ID_RTL8367RVB					0x5
#define CHIP_ID_RTL8367MVB					0x6
#define CHIP_ID_RTL8367MB						0x7
#define CHIP_ID_RTL8368MB						0x8
#define CHIP_ID_RTL8305MB						0x9
#define CHIP_ID_RTL8307MVB					0xA


#if defined(RTK_X86_ASICDRV)
#define RTK_CHIP_NAME_STRING				"RTK_X86_ASICDRV"
#define RTK_CHIP_ID  								CHIP_ID_NONE

#elif defined(CHIP_RTL8363SB)
#define RTK_CHIP_NAME_STRING				"RTL8363SB"
#define RTK_CHIP_ID  								CHIP_ID_RTL8363SB

#elif defined(CHIP_RTL8365MB)
#define RTK_CHIP_NAME_STRING				"RTL8365MB"
#define RTK_CHIP_ID  								CHIP_ID_RTL8365MB

#elif defined(CHIP_RTL8367_VB)
#define RTK_CHIP_NAME_STRING				"RTL8367_VB"
#define RTK_CHIP_ID  								CHIP_ID_RTL8367VB

#elif defined(CHIP_RTL8367RB)
#define RTK_CHIP_NAME_STRING				"RTL8367RB"
#define RTK_CHIP_ID  								CHIP_ID_RTL8367RB

/* MAC, Translate Chip Mac into Common SWITCH MAC */
#define RTK_PORT_DISABLE						RTL8367B_MAC_DISABLE

#define RTK_PORT_0_MAC							RTL8367B_MAC_0
#define RTK_PORT_1_MAC							RTL8367B_MAC_1
#define RTK_PORT_2_MAC							RTL8367B_MAC_2
#define RTK_PORT_3_MAC							RTL8367B_MAC_3
#define RTK_PORT_4_MAC							RTL8367B_MAC_DISABLE 	/* Telefornica use 4 Lan port (0~3) */	
#define RTK_PORT_5_MAC							RTL8367B_MAC_DISABLE	/* Telefornica use 2 ext. port (1~2) */
#define RTK_PORT_6_MAC							RTL8367B_MAC_6_EXT
#define RTK_PORT_7_MAC							RTL8367B_MAC_7_EXT
#define RTK_PORT_END								RTL8367B_MAC_END			

#define RTK_PORT_MAX								RTL8367B_MAC_MAX			/* RTL8367B have (0~7) port */
#define RTK_PORT_NUM								RTL8367B_MAC_NUM			/* RTL8367B total 8 port */
#define RTK_PORT_LAN_NUM						RTL8367B_MAC_LAN_NUM		

#define RTK_EXTP_NUM								RTL8367B_MAC_EXT_NUM	/* RTL8367B use 2 ext port */

#elif defined(CHIP_RTL8367R_VB)
#define RTK_CHIP_NAME_STRING				"RTL8367R_VB"
#define RTK_CHIP_ID  								CHIP_ID_RTL8367RVB

#elif defined(CHIP_RTL8367M_VB)
#define RTK_CHIP_NAME_STRING				"RTL8367M_VB"
#define RTK_CHIP_ID  								CHIP_ID_RTL8367MVB

#elif defined(CHIP_RTL8367MB)
#define RTK_CHIP_NAME_STRING				"RTL8367MB"
#define RTK_CHIP_ID  								CHIP_ID_RTL8367MB

#elif defined(CHIP_RTL8368MB)
#define RTK_CHIP_NAME_STRING				"RTL8368MB"
#define RTK_CHIP_ID  								CHIP_ID_RTL8368MB

#elif defined(CHIP_RTL8305MB)
#define RTK_CHIP_NAME_STRING				"RTL8305MB"
#define RTK_CHIP_ID  								CHIP_ID_RTL8305MB

#elif defined(CHIP_RTL8307M_VB)
#define RTK_CHIP_NAME_STRING				"RTL8307M_VB"
#define RTK_CHIP_ID  								CHIP_ID_RTL8307MVB

#else

/* Not define CHIP, Error */
#error "NO defined CHIP_RTLXXXX for init-params!" 
#define RTK_CHIP_NAME_STRING				"No RTK Switch Chip"
#define RTK_CHIP_ID									CHIP_ID_NONE
#endif

#if(_RTK_RTL83XX_SWITCH_API == 1)
#define RTK_CHIP_API_VERSION 				"API version V1.2.9"
#elif(_RTK_RTL83XX_SWITCH_API == 2)
#define RTK_CHIP_API_VERSION 				"API version V1.2.10"
#endif

#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_LINUX)

#define RTK_PORT0_LOG_TO_PHY				RTK_PORT_3_MAC
#define RTK_PORT1_LOG_TO_PHY 				RTK_PORT_2_MAC
#define RTK_PORT2_LOG_TO_PHY 				RTK_PORT_1_MAC
#define RTK_PORT3_LOG_TO_PHY 				RTK_PORT_0_MAC
#define RTK_PORT4_LOG_TO_PHY  			RTK_PORT_6_MAC
#define RTK_PORT5_LOG_TO_PHY 				RTK_PORT_DISABLE
#define RTK_PORT6_LOG_TO_PHY 				RTK_PORT_DISABLE	
#define RTK_PORT7_LOG_TO_PHY  			RTK_PORT_DISABLE

#define RTK_PORT0_PHY_TO_LOG 				3	
#define RTK_PORT1_PHY_TO_LOG 				2
#define RTK_PORT2_PHY_TO_LOG 				1
#define RTK_PORT3_PHY_TO_LOG 				0
#define RTK_PORT4_PHY_TO_LOG 				RTK_PORT_DISABLE
#define RTK_PORT5_PHY_TO_LOG 				RTK_PORT_DISABLE
#define RTK_PORT6_PHY_TO_LOG 				4
#define RTK_PORT7_PHY_TO_LOG 				RTK_PORT_DISABLE

#define RTK_MAX_PORT								4 			// Lan ports
#define RTK_CPU_PORT 								4 			// CPU ports

#elif((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
/*
*		Logical Port : 
*		VR9-CPU-Port  WAN  WLAN5G  RTK-Ports
*		6          		5     4      3 2 1 0
*/
#ifdef _Vendor_TF_FTTH
#define RTK_PORT0_LOG_TO_PHY				RTK_PORT_3_MAC
#define RTK_PORT1_LOG_TO_PHY 				RTK_PORT_2_MAC
#define RTK_PORT2_LOG_TO_PHY 				RTK_PORT_1_MAC
#define RTK_PORT3_LOG_TO_PHY 				RTK_PORT_0_MAC
#define RTK_PORT4_LOG_TO_PHY  			RTK_PORT_6_MAC
#define RTK_PORT5_LOG_TO_PHY 				RTK_PORT_DISABLE
#define RTK_PORT6_LOG_TO_PHY 				RTK_PORT_DISABLE	
#define RTK_PORT7_LOG_TO_PHY  			RTK_PORT_DISABLE

#define RTK_PORT0_PHY_TO_LOG 				3	
#define RTK_PORT1_PHY_TO_LOG 				2
#define RTK_PORT2_PHY_TO_LOG 				1
#define RTK_PORT3_PHY_TO_LOG 				0
#define RTK_PORT4_PHY_TO_LOG 				RTK_PORT_DISABLE
#define RTK_PORT5_PHY_TO_LOG 				RTK_PORT_DISABLE
#define RTK_PORT6_PHY_TO_LOG 				4
#define RTK_PORT7_PHY_TO_LOG 				RTK_PORT_DISABLE

#define RTK_MAX_PORT								4 			// Lan ports
#define RTK_CPU_PORT 								4 			// CPU ports
#else
/* Other project */
#endif
#endif

/* CPU Port Tag */
#define RTK_CPUTAG_ETHTYPE					0x8899	
#define RTK_CPUTAG_PTL							0x4						
#define RTK_CPUTAG_PKTHDR_LEN				8


#define RTK_INIT_DELAY              1000
#define RTK_REST_DELAY              100


#define RTK_IGMP_REC_PAGE_NO				8
#define RTK_IGMP_REC_PAGE						32
#define RTK_MAX_IGMP_REC						256			// Maximum multicast addresses for HW IGMP snooping
#define RTK_MAX_SRC_NUM							32			// Maximum number for source IP addresses from each



#ifdef SWSetting
#undef SWSetting
#endif

#ifdef SWGetting
#undef SWGetting
#endif

#define SWSetting		1
#define SWGetting		0

#ifdef RTK_NULL
#undef RTK_NULL
#endif

#define RTK_NULL	((void*)0)

/*-------------------------------------------------------------
  REALTEK Register Field.
--------------------------------------------------------------- */
/* IGMP Action, from rtk_trap_igmp_action_t */
#define RTK_IGMP_ACTION_FORWARD								0			/* Forward */
#define RTK_IGMP_ACTION_TRAP2CPU							1			/* Send to CPU Port */
#define RTK_IGMP_ACTION_DROP									2			/* Drop */
#define RTK_IGMP_ACTION_FORWARD_EXCLUDE_CPU		3			/* Send to any port exclude CPU Port */
#define RTK_IGMP_ACTION_ASIC									4 		/* HW support */

/* Page 0: PCS Register (PHY 0~4)  */
#define RTK_PCS_CTL														PHY_CONTROL_REG				
#define RTK_PCS_STATUS												PHY_STATUS_REG			
#define RTK_PCS_PHYID_1												2				
#define RTK_PCS_PHYID_2												3
#define RTK_PCS_AN_ADVERTISEMENT							PHY_AN_ADVERTISEMENT_REG
#define	RTK_PCS_AN_LNKPAR_ABILITY							PHY_AN_LINKPARTNER_REG
#define RTK_PCS_AUTONEGO_EXPANSION						6	
#define RTK_PCS_AUTONEGO_PAGETX								7
#define RTK_PCS_AUTONEGO_LNKPAR_NEXTP					8
#define RTK_PCS_1000BaseT_CTL									PHY_1000_BASET_CONTROL_REG
#define RTK_PCS_1000BaseT_STATUS							PHY_1000_BASET_STATUS_REG
#define RTK_PCS_EXTENDED											15

/* RTL8367B_REG_DIGITAL_INTERFACE1_FORCE    0x1311, MASK 0xFFF */
#define RTK_MAC_FORCE_EN											1
#define RTK_MAC_AUTOPOLLING_EN								0

/* RTL8367B_REG_CHIP_RESET    0x1322 */
#define RTK_SWITCH_CHIPRST										0
#define RTK_SWITCH_SWRST											1


/*-------------------------------------------------------------
  REALTEK HAL structure
--------------------------------------------------------------- */ 
typedef struct _rtk_register {
   /** Register Address Offset for read or write access. */
   unsigned int u32RegAddr;
   /** Value to write or to read from 'u32Data'. */
   unsigned int u32Data;
}RTK_REGISTER;

typedef struct _rtk_portpower {
		unsigned int	u32PortNo;
		RTK_REGISTER	stRegAddrVal;
}RTK_PORTPOWER;

/* ethernet address type */
typedef struct _rtk_mac_type {
    unsigned char  octet[6];
}RTK_MAC_TYPE;

/* l2 address table - unicast data structure */
typedef struct _rtk_mactbl_entry {
    RTK_MAC_TYPE stMac;
    unsigned int u32Ivl;
    unsigned int u32CVid;
    unsigned int u32Fid;
    unsigned int u32EFid;
    unsigned int u32Port;
    unsigned int u32SABlock;
    unsigned int u32DABlock;
    unsigned int u32Auth;
    unsigned int u32Static;
}RTK_MACTBL_ENTRY;

typedef struct _rtk_mactbl_entry_get {
		unsigned int u32Method; 
		unsigned int u32PortNo; 
		unsigned int u32Idx; 
		RTK_MACTBL_ENTRY stEntry;
}RTK_MACTBL_ENTRY_GET;


typedef struct _rtk_mactbl_status {
		RTK_MAC_TYPE stMac;
		RTK_MACTBL_ENTRY stMacTblEntry;
}RTK_MACTBL_STATUS;

typedef struct _rtk_l2lut_status {
		unsigned int u32LookUpTblType;
		
		unsigned int u32DIP; 
		unsigned int u32SIP; 
		unsigned int u32PhyID;
		unsigned int u32PortMask;

		RTK_MACTBL_STATUS stMacTblStatus;
		unsigned int u32Idx; 
		unsigned char u8Method;
		
		unsigned char u8L2Init;
}RTK_L2LUT_STATUS;

typedef struct _rtk_port_maxpktlen {
		unsigned int u32PortNo; 
		unsigned int u32MaxPktLen; 
}RTK_PORT_MAXPKTLEN;

typedef struct _rtk_cpuporttag {
	unsigned int u32CpuPortNo;
	unsigned int u32CpuPortMode;
	unsigned int u32CpuPortMAsk;
}RTK_CPUPORT_TAG;

typedef struct _rtk_cpu_port_status {
		int s32CPUPortEnable;
		RTK_CPUPORT_TAG *pstCPUPortTag;
}RTK_CPU_PORT_STATUS;

/* Mapping from struct  rtk_port_mac_ability_s */
typedef struct _rtk_port_mac_status {
    unsigned int u32Forcemode;
    unsigned int u32Speed;
    unsigned int u32Duplex;
    unsigned int u32Link;
    unsigned int u32Nway;
    unsigned int u32Txpause;
    unsigned int u32Rxpause;
}RTK_PORT_MAC_STATUS;

typedef struct _rtk_port_link_status {
		unsigned int u32PortNo;
		RTK_PORT_MAC_STATUS stMacStatus;
}RTK_PORT_LINK_STATUS;

typedef struct _rtk_port_phy_status {
    unsigned int u32LinkStatus;
    unsigned int u32Speed;
    unsigned int u32Duplex;
}RTK_PORT_PHY_STATUS;

typedef struct _rtk_port_linkphy_status {
		unsigned int u32PortNo;
		RTK_PORT_PHY_STATUS stPhyStatus;
}RTK_PORT_LINKPHY_STATUS;

typedef struct _rtk_port_forcelinkext_status {
		unsigned int u32ExtPortNo;
		unsigned int u32Mode;
		RTK_PORT_MAC_STATUS stMacStatus;
}RTK_PORT_FORCELINKEXT_STATUS;

typedef struct _rtk_port_rgmiidelayext {
		unsigned int u32ExtPortNo;
		unsigned int u32TxDelay;
		unsigned int u32RxDelay;
}RTK_PORT_RGMIIDELAYEXT;

typedef struct _rtk_port_phytestmode {
		unsigned int u32PhyNo;
		unsigned int u32Mode;
}RTK_PORT_PHYTESTMODE;


typedef struct _rtk_igmp_record {
		unsigned long		u32GrpAddr;
		unsigned long		u32PortMap;
		unsigned int    u32EntryIdx;
		int							s32Used;
		unsigned long		u32Timer[RTK_MAX_PORT];						// aging timer
		unsigned long		u32SrcList[RTK_MAX_IGMP_REC];
		int							s32SrcCount;
		//#if _IGMP_SNOOP_EHT
		//struct igmp_source_record src_rec[SWITCH_MAX_VPORT];
		//#endif // _IGMP_SNOOP_EHT
}RTK_IGMP_RECORD; 

typedef struct _rtk_hw_igmp_status {
		unsigned char u8HWSnooping;
		unsigned char u8FastLeave;
		unsigned char u8StaticRouterPortNo;
		unsigned char u8FlgInit;
}RTK_HW_IGMP_STATUS;

typedef struct _rtk_cpu_egress_pkt_header
{
    unsigned int u32EthTypeHi					:8;		/* Realtek EtherType (16-bits) [0x8899] */
		unsigned int u32EthTypeLo					:8;		/* Realtek EtherType (16-bits) [0x8899] */
    unsigned int u32Protocol					:8;  	/* Protocol (8-bit) */
    unsigned int u32Reason						:8;		/* Reason */
    unsigned int u32EFID							:1; 	/* EFID */
    unsigned int u32EnhancedFID				:3; 	/* Enhanced FID */
		unsigned int u32PriSelect  				:1;		/* Priority Select */
    unsigned int u32Priority					:3; 	/* Priority */
    unsigned int u32Keep							:1; 	/* Keep */
    unsigned int u32VSEL							:1; 	/* VSEL */
    unsigned int u32DisableLearning		:1; 	/* Disable Learning */
    unsigned int u32VIDX							:5; 	/* VIDX */
		unsigned int u32Rev1							:8;		/* Reserved */
		unsigned int u32Rev2							:4;		/* Reserved */
		unsigned int u32PortNo            :4;		/* Port mask TX/SPA Rx(PortNo bits, LSB 4-bits) */
}RTK_CPU_EGRESS_PKT_HEADER;



/*-------------------------------------------------------------
  REALTEK HAL Macro.
--------------------------------------------------------------- */
#if defined(MDC_MDIO_OPERATION) 
extern unsigned short RTL83XX_SMI_READ(unsigned short u16Addr);
extern void RTL83XX_SMI_WRITE(unsigned short u16Addr, unsigned short u16Data);
#endif

#define RTK_MDIO_R16(addr)					RTL83XX_SMI_READ(addr)						
#define RTK_MDIO_W16(addr, value)		RTL83XX_SMI_WRITE(addr, value)
#define RTK_MDIO_16_ON(addr,value) 	RTK_MDIO_W16(addr, RTK_MDIO_R16(addr)|(value))
#define RTK_MDIO_16_OF(addr,value) 	RTK_MDIO_W16(addr, RTK_MDIO_R16(addr)&(~(value)))

/*-------------------------------------------------------------
  RTL8367RB HAL Macro.
--------------------------------------------------------------- */
#define RTL8367RB_MAXPKTLEN_SET(Len)			do {																																																																\
																									RTK_MDIO_W16(RTL8367B_REG_MAX_LENGTH_LIMINT_IPG, (Len << RTL8367B_MAX_LENTH_CTRL_OFFSET) & RTL8367B_MAX_LENTH_CTRL_MASK)	\
																					}while(0);																																																													\

#define	RTL8367RB_CPUPORTTAG_ENABLE(On)  do {																											\
																							if(On)																							\
																							{																										\
																									RTK_MDIO_16_ON(RTL8367B_REG_CPU_CTRL, WBIT00);	\
																							}																										\
																							else																								\
																							{																										\
																									RTK_MDIO_16_OF(RTL8367B_REG_CPU_CTRL, WBIT00);	\
																							}																										\
																					}while(0);																							\

#define RT8367RB_CPUPORTTAG_MASK(value)		do {																											\
																									RTK_MDIO_W16(RTL8367B_REG_CPU_PORT_MASK, value);	\
																					}while(0);																								\
																				
/*-------------------------------------------------------------
  Debug Message
--------------------------------------------------------------- */
#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_LINUX)
#ifdef __KERNEL__	
#define iprintf 	printk
#else
#define iprintf 	fprintf
#endif
#endif

#if(RTL8367B_HAL_DEBUG == SW_ENABLED)	
#define RTL8367B_HAL_MSG(msg)				msg;
#define RTL8367B_HAL_DBG(format, args...)		iprintf("[%s:%d]"format,__FUNCTION__, __LINE__,##args);
#define RTL8367B_DBG(format, args...)     	iprintf("[%s:%d]",__FUNCTION__, __LINE__);iprintf(format,##args);
//#define RTL83XX_DBG(format, args...)     	iprintf("[%s:%d]"format,__FILE__, __LINE__,##args);
#else
#define RTL8367B_HAL_MSG(msg)     			
#define RTL8367B_HAL_DBG(format, args...)		
#define RTL8367B_HAL_DMP(msg)     			
#endif

/*-------------------------------------------------------------
  Function Declaration
--------------------------------------------------------------- */
#define RTL83XX_HAL_FUN
#define RTL8367B_HAL_FUN

#ifdef RTL83XX_HAL_FUN							
#define RTL83XX_HAL_INTERFACE	
#else
#define RTL83XX_HAL_INTERFACE		extern
#endif

#ifdef RTL8367B_HAL_FUN							
#define RTL8367B_HAL_INTERFACE	
#else
#define RTL8367B_HAL_INTERFACE	extern
#endif

RTL83XX_HAL_INTERFACE void Dump_RTK83XX_CPUPortPktHdr(RTK_CPU_EGRESS_PKT_HEADER *pstRtkEgressHdr);

RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_Init(void);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_HwRst(unsigned int u32PluseWidth);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_Restart(void);

RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_PortPhyStatus_Get(unsigned short u16PhyNo, unsigned int *pu32LinkStatus, unsigned int *pu32Speed, unsigned int *pu32Duplex);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_PortMacStatus_Get(unsigned short u16PortNo, RTK_PORT_MAC_STATUS *pstPortMacStatus);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_PortTagEn_Cfg(unsigned int *pu32Enable, unsigned char u8Set);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_PortTag_Cfg(unsigned int *pu32PortNo, unsigned int *pu32Mode, unsigned char u8Set);

RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_ExtPortForceLink_Cfg(unsigned char u8ExtPortNo, unsigned char *pu8Mode, RTK_PORT_MAC_STATUS *pstPortMacStatus, unsigned char u8Set);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_ExtPortRGMIIDly_Cfg(unsigned char u8ExtPortNo, unsigned int *pu32TxDelay, unsigned int *pu32RxDelay, unsigned char u8Set);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_PortState_Get(unsigned int u32PortNo, unsigned int u32PktType, unsigned long long *pu64PktCnt);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_PhyEnableAll_Cfg(unsigned int *pu32PhyEnable, unsigned char u8Set);

RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_PhyTestMode_Cfg(unsigned char u8PhyNo, unsigned char u8TestMode);


RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_L2LUTInit(void);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_L2LUT_McastAddr_Cfg(unsigned char *pu8MacAddr, unsigned int u32Ivl, unsigned int u32CVidFid, unsigned int u32PortMask, unsigned char u8Set);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_L2LUT_McastCurrEntryMask_Get(unsigned char *pu8MacAddr, unsigned int u32Ivl, unsigned int u32CVidFid, unsigned int *pu32PortMask);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_L2LUT_McastCurrEntryMask_Get(unsigned char *pu8MacAddr, unsigned int u32Ivl, unsigned int u32CVidFid, unsigned int *pu32PortMask);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_L2LUT_IpMcastAddr_Cfg(unsigned int u32DIP, unsigned int u32SIP, unsigned int u32PortMask, unsigned char u8Set);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_L2LUT_IpMcastCurrEntryMask_Get(unsigned int u32DIP, unsigned int u32SIP, unsigned int *pu32PortMask);
RTL83XX_HAL_INTERFACE unsigned int RTL83XX_HalComm_L2LUT_IpMcastNextEntryIdx_Get(unsigned int *pu32AddrIdx, unsigned int *pu32DIP, unsigned int *pu32SIP, unsigned int *pu32PortMask);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_L2LUT_IpMcastTable_Dump(void);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_L2LUT_LookupTable_Dump(void);

RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_IGMPInit(void);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_IGMPStaticRouterPort_Cfg(unsigned int *pu32PortMask, unsigned char u8Set);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_IGMPProtocol_Cfg(unsigned int u32Port, unsigned char *pu8Proto, unsigned char *pu8Action, unsigned char u8Set);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_IGMPMaxGroup_Cfg(unsigned int u32Port, unsigned int *pu32Group, unsigned char u8Set);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_IGMPHWSnoop_Disable(void);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_IGMPPortAction_Get(unsigned int u32Port, unsigned int *pu32Action);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_IGMPPortCurGroupCnt_Get(unsigned int u32Port, unsigned int *pu32GroupCnt);
RTL83XX_HAL_INTERFACE int RTL83XX_HalComm_IGMPPortAction_Dump(void);

RTL83XX_HAL_INTERFACE void RTL83XX_HalComm_IGMPTble_Dump(void);

#endif
