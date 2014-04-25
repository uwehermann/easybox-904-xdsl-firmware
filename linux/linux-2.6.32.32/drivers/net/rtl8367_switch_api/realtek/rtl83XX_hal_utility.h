#ifndef __RTL83XX_HAL_UTILITY__
#define __RTL83XX_HAL_UTILITY__

/*-------------------------------------------------------------
  Include files
--------------------------------------------------------------- */
#include "rtl83XX_hal.h"						/* local definitions */

/*-------------------------------------------------------------
  RTK HAL I/O parameters
--------------------------------------------------------------- */
#ifndef RTK_CONFIG_CPUTAG
#define RTK_CONFIG_CPUTAG 1
#endif

#ifndef RTK_CONFIG_L2LUT
#define RTK_CONFIG_L2LUT 	1
#endif

#ifndef RTK_CONFIG_8021X
#define RTK_CONFIG_8021X 	1
#endif

#ifndef RTK_CONFIG_MULTICAST
#define RTK_CONFIG_MULTICAST 1
#endif

#ifndef RTK_CONFIG_QOS
#define RTK_CONFIG_QOS 1
#endif

#ifndef RTK_CONFIG_STP
#define RTK_CONFIG_STP 1
#endif

#ifndef RTK_CONFIG_VLAN
#define RTK_CONFIG_VLAN 1
#endif

#ifndef RTK_CONFIG_WOL
#define RTK_CONFIG_WOL 1
#endif

#define RTK_MAX_COMMAND_ARGS		64
#define RTK_MAX_CMD_STR_LEN			100
#define RTK_SWITCH_UTILITY_COMMAND_BUFFER 70


typedef int (*RTK_LowLevel_FKT) (unsigned int);
#define RTK_UTILCMD_SW_FKT(x, y) (int)x? (RTK_LowLevel_FKT)y : (RTK_LowLevel_FKT)((void *)0)

/**
 * \brief Switch command structure that contains
 * - name:  command string that is expected to be entered from command
 *          line as argv[1]
 * - param: parameter description that describes the amount and type of
 *          parameters attached to the command
 * - ioctl_command: IOCTL command for device driver access
 *
 */
typedef struct _rtk_utility_command {
   const char         *s8Name;
   const char         *s8Param;          /* i=int, l=long, s=string */
   const unsigned int u32IOCtlCmd;
   const char         *s8ParamStr;
   const char         *s8DetailInfo;
}RTK_UTILITY_COMMAND;


typedef union _rtk_swio_params
{
	  /* Operation, Administration, and Management Functions */
		unsigned int										u32PktLength;
		RTK_PORT_MAXPKTLEN							stPortMaxPktLength;
		RTK_CPUPORT_TAG									stCpuPortTag;
		
		/* L2LUT Functions */
		unsigned int										u32LookUpTblType;
    RTK_L2LUT_STATUS								stL2LUTIpMcastTableAddrAdd;
		RTK_L2LUT_STATUS								stL2LUTIpMcastTableAddrRemove;
		//RTK_L2LUT_STATUS			L2LUT_IpMcastTableAddrDump;

		/* Port MAC/PHY Configuration */
		RTK_PORT_FORCELINKEXT_STATUS 		stPortForceLinkExt;
		RTK_PORT_RGMIIDELAYEXT 					stPortRGMIIDealyExt;

		/* Setting */
		unsigned int										u32Set;

		/* Return Address */ 
		unsigned int										u32PtrAddr;

}RTK_SWIO_PARAMS;

/*-------------------------------------------------------------
  RTK HAL I/O Command
--------------------------------------------------------------- */ 
/* IOCTL MAGIC */
//#define RTK_ETHSW_MAGIC ('R')//('E')
/*
I/O Command List

No. 						Class
[0x00~0x0F] 
0x00 : RTK_UTILCMD_INIT
0x01 : RTK_UTILCMD_MAXPKTLEN_SET
0x02 : RTK_UTILCMD_MAXPKTLEN_GET
0x03 : RTK_UTILCMD_PORTMAXPKTLEN_SET
0x04 : RTK_UTILCMD_PORTMAXPKTLEN_GET
0x05 : RTK_UTILCMD_CPUPORTTAGEN_SET
0x06 : RTK_UTILCMD_CPUPORTTAGEN_GET
0x07 : RTK_UTILCMD_CPUPORTTAG_SET
0x08 : RTK_UTILCMD_CPUPORTTAG_GET
0x09 : 
0x0A : 
0x04 : 

[0x10~0x1F]			L2 LookUpTbl 
0x10 : RTK_UTILCMD_L2LUT_INIT
0x11 : RTK_UTILCMD_L2LUT_TYPE_SET
0x12 : RTK_UTILCMD_L2LUT_TYPE_GET
0x13 :
0x14 :
0x15 : RTK_UTILCMD_L2LUT_IPMULTICASTADDR_ADD
0x16 : RTK_UTILCMD_L2LUT_IPMULTICASTADDR_DEL
0x17 : RTK_ETHSW_L2LUT_IPMULTICASTADDR_DUMP

[0x20~0x2F]     Port MAC/PHY Configuration
0x20 : RTK_UTILCMD_PORTLINKCFG_GET
0x21 : RTK_UTILCMD_PORTLINKPHYCFG_GET
0x22 : RTK_UTILCMD_PORTFORCELINKEXTCFG_SET
0x23 : RTK_UTILCMD_PORTFORCELINKEXTCFG_GET
0x24 : RTK_UTILCMD_PORTRGMIICLKEXT_SET
0x25 : RTK_UTILCMD_PORTRGMIICLKEXT_GET
0x26 : RTK_UTILCMD_PHYTESTMODE_SET
0x27 : RTK_UTILCMD_PHYTESTMODE_GET
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
#define RTK_UTILCMD_INIT														0x00

/* Set the max packet length for whole system setting, 1st configuration */
#define RTK_UTILCMD_MAXPKTLEN_SET										0x01

/* Get the max packet length for whole system setting, 1st configuration */
#define RTK_UTILCMD_MAXPKTLEN_GET										0x02

/* Set the max packet length for certain port setting, 2st configuration */
#define RTK_UTILCMD_PORTMAXPKTLEN_SET								0x03

/* Get the max packet length for certain port setting, 2st configuration */
#define RTK_UTILCMD_PORTMAXPKTLEN_GET								0x04

/* Set cpu tag function enable/disable, If CPU tag function is disabled,  
	 CPU tag will not be added to frame forwarded to CPU port, and all ports 
	 cannot parse CPU tag. */
#define	RTK_UTILCMD_CPUPORTTAGEN_SET								0x05

/* Get cpu tag function enable/disable */
#define	RTK_UTILCMD_CPUPORTTAGEN_GET								0x06

/* Set CPU port and inserting proprietary CPU tag mode to the frame that 
	 transmitting to CPU port. */
#define	RTK_UTILCMD_CPUPORTTAG_SET									0x07

/* Get cpu tag function enable/disable */
#define	RTK_UTILCMD_CPUPORTTAG_GET									0x08



/* Initialize l2 module of the specified device. */
#define RTK_UTILCMD_L2LUT_INIT											0x10

/* Set Lut IP multicast lookup function
	 If users set the lookup type to DIP, the group in exception table will be lookup by DIP+SIP
   If users set the lookup type to DIP+SIP, the group in exception table will be lookup by only DIP */
#define RTK_UTILCMD_L2LUT_TYPE_SET									0x11

/* Get Lut IP multicast lookup function */
#define RTK_UTILCMD_L2LUT_TYPE_GET									0x12

/* Adds a multicast group configuration to the multicast table. 
   No new entry is added in case this multicast group already exists in the table. 
   This commands adds a host member to the multicast group.*/
#define RTK_UTILCMD_L2LUT_IPMULTICASTADDR_ADD				0x15//_IOW(RTK_ETHSW_MAGIC, 0x3F, RTK_L2LUT_IPMCASTADDR_ADDEL)

/* Remove an host member from a multicast group. 
   The multicast group entry is completely removed from the multicast table in case it has no host member port left. */
#define RTK_UTILCMD_L2LUT_IPMULTICASTADDR_DEL				0x16//_IOWR(RTK_ETHSW_MAGIC, 0x40, RTK_L2LUT_IPMCASTADDR_ADDEL)

/* Read out the multicast membership table that is located inside the switch hardware. */
#define RTK_ETHSW_L2LUT_IPMULTICASTADDR_DUMP        0x17//_IOWR(IFX_ETHSW_MAGIC, 0x41, IFX_ETHSW_multicastTableRead_t)

/* Get port linking configuration. */
#define RTK_UTILCMD_PORTLINKCFG_GET									0x20

/* Get port PHY status configuration. */
#define RTK_UTILCMD_PORTLINKPHYCFG_GET							0x21

/* Set port force linking configuration. */
#define RTK_UTILCMD_PORTFORCELINKEXTCFG_SET					0x22

/* Get port force linking configuration. */
#define RTK_UTILCMD_PORTFORCELINKEXTCFG_GET					0x23

/* Set port force linking configuration. */
#define RTK_UTILCMD_PORTRGMIICLKEXT_SET							0x24

/* Get RGMII interface delay value for TX and RX. */
#define RTK_UTILCMD_PORTRGMIICLKEXT_GET							0x25

/* Set PHY in test mode and only one PHY can be in test mode at the same time. */
#define RTK_UTILCMD_PHYTESTMODE_SET									0x26
/* Get PHY in which test mode. */
#define RTK_UTILCMD_PHYTESTMODE_GET									0x27


/*-------------------------------------------------------------
  Function Declaration
--------------------------------------------------------------- */
#define RTL83XX_HAL_UTILITY_FUN

#ifdef RTL83XX_HAL_UTILITY_FUN							
#define RTL83XX_HAL_UTILITY_INTERFACE	
#else
#define RTL83XX_HAL_UTILITY_INTERFACE	extern
#endif

RTL83XX_HAL_UTILITY_INTERFACE void Util_PrintCmd(void);
RTL83XX_HAL_UTILITY_INTERFACE int Util_CheckCmd(char *ps8Cmd);
RTL83XX_HAL_UTILITY_INTERFACE int Util_ConvertParams(char **ps8Params, int s32NrParams);

RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_CmdBuildEx(unsigned int u32Cmd);
RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_Main(int argc, char **argv);
RTL83XX_HAL_UTILITY_INTERFACE int RTK_Switch_Utility(char *ps8Cmd);

RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_MaxPktLengthSet(unsigned int u32Params);
RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_MaxPktLengthGet(unsigned int u32Params);
RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_PortMaxPktLengthSet(RTK_PORT_MAXPKTLEN *pu32Params);
RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_PortMaxPktLengthGet(RTK_PORT_MAXPKTLEN *pu32Params);

RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_L2LUTTypeSet(unsigned int u32Params);
RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_L2LUTTypeGet(unsigned int u32Params);
RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_L2IpMulticastTableEntryAdd(RTK_L2LUT_STATUS *pstParams);
RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_L2IpMulticastTableEntryRemove(RTK_L2LUT_STATUS *pstParams);

RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_PortForceLinkExtCfgSet(RTK_PORT_FORCELINKEXT_STATUS *pstParams);
RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_PortForceLinkExtCfgGet(RTK_PORT_FORCELINKEXT_STATUS *pstParams);
RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_PortRGMIIDelayExtSet(RTK_PORT_RGMIIDELAYEXT *pstParams);
RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_PortRGMIIDelayExtGet(RTK_PORT_RGMIIDELAYEXT *pstParams);

RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_CPUPortTagEnSet(unsigned int u32Params);
RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_CPUPortTagEnGet(unsigned int u32Params);
RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_CPUPortTagCfgSet(RTK_CPUPORT_TAG	*pu32Params);
RTL83XX_HAL_UTILITY_INTERFACE int RTL83XX_Utility_CPUPortTagCfgGet(RTK_CPUPORT_TAG	*pu32Params);

RTL83XX_HAL_UTILITY_INTERFACE int Test_Switch_Utility_Cmd(void);

#endif // __RTL83XX_HAL_UTILITY__
