/* ============================================================
Follow by 
This API package supports multiple Realtek Switch contains  ASIC  
drivers.  ASIC  drivers  provide  general  APIs  that based on 
user configuration to configure relative ASIC registers.


Programmer : Alvin Hsu, alvin_hsu@arcadyan.com
=============================================================== */
#include "switch_vendor_hal.h"			/* local definitions */
#include "rtl83XX_hal.h"						/* local definitions */
#include "rtl83XX_hal_utility.h"		/* local definitions */

#include "switch_vendor_project.h"	/* local definitions */
#include "switch_vendor_error.h"		/* local definitions */

#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
#include "vr9.h"

#elif((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_LINUX)
#include <linux/byteorder/generic.h>					/* htonl() */
#include <linux/delay.h>											/* mdelay() */
#include <asm/ifx/vr9/vr9.h>
#endif

#if(_RTK_RTL83XX_SWITCH_API == 1)
#error "No available switch api, version error!!"
#elif(_RTK_RTL83XX_SWITCH_API == 2)
#include "rtl8367rb/switch_api_v1_2_10/include/rtk_api_ext.h"
#include "rtl8367rb/switch_api_v1_2_10/include/rtk_api.h"
#include "rtl8367rb/switch_api_v1_2_10/include/rtk_types.h"

#if(RTK_CHIP_ID == CHIP_ID_RTL8367RB)
#include "rtl8367rb/switch_api_v1_2_10/include/rtl8367b_reg.h"
#endif
#endif

/*****************************************************************
*	Extern
*****************************************************************/
/*
extern SWITCH_DEVICE_STATE	gstSwDevState;
extern char *gps8SwDevChipName;							// global switch chip name string
extern char *gps8SwDevVersion;							// global switch version
extern char gs8SwDevChipId;									// global switch chip id
*/

extern char gs8RtkUtilCmdStr[RTK_MAX_CMD_STR_LEN];	// RTK switch utility string
extern RTK_SWIO_PARAMS gunIOParams;


#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_LINUX)
/**
 * msleep - sleep safely even with waitqueue interruptions
 * @msecs: Time in milliseconds to sleep for
 */
extern void msleep(unsigned int msecs);
#endif

/*****************************************************************
*	Definition
*****************************************************************/
#if(RTK_CHIP_ID == CHIP_ID_RTL8367RB)
SWITCH_DEVICE_FILE_OPERATIONS gstRtl867RBFOps = {
		RTL83XX_HalComm_Init,					/* entry point for initializing */
		RTL83XX_HalComm_HwRst,				/* entry point for sw-reset */
		RTL83XX_HalComm_Restart,			/* entry point for restart */
		((void *)0),			   					/* entry point for shutdown */
    ((void *)0),									/* entry point for sending  */
    ((void *)0),									/* entry point for receving */
    ((void *)0)										/* entry point for io control */
};
#endif

RTK_CPU_EGRESS_PKT_HEADER	gstRtkEgressHdr, gstCurrRtkEgressHdr; 
//RTK_CPU_PORT_STATUS gstRtkCPUPortStatus;

int RTK_PORT_MAP[RTK_PORT_END] 		= {RTK_PORT0_LOG_TO_PHY, RTK_PORT1_LOG_TO_PHY, RTK_PORT2_LOG_TO_PHY, RTK_PORT3_LOG_TO_PHY, RTK_PORT4_LOG_TO_PHY, RTK_PORT5_LOG_TO_PHY, RTK_PORT6_LOG_TO_PHY, RTK_PORT7_LOG_TO_PHY}; // logical to physical, 0-based
int RTK_LOG_PORT_MAP[RTK_PORT_END] = {RTK_PORT0_PHY_TO_LOG, RTK_PORT1_PHY_TO_LOG, RTK_PORT2_PHY_TO_LOG, RTK_PORT3_PHY_TO_LOG, RTK_PORT4_PHY_TO_LOG, RTK_PORT5_PHY_TO_LOG, RTK_PORT6_PHY_TO_LOG, RTK_PORT7_PHY_TO_LOG}; // physical to logical, 0-based

RTK_IGMP_RECORD gRtkIGMPTbl[RTK_IGMP_REC_PAGE_NO][RTK_IGMP_REC_PAGE];



/*****************************************************************
*	Utility
*****************************************************************/
unsigned int Util_ConvertToRTKPortMask(unsigned int u32PortMask)
{
		unsigned char bits;
		unsigned int u32RtkPortMask=0;
		
		if(u32PortMask > 0x0000000F)
		{
				dprintf("[%s] u32PortMap = 0x%x overflow!\n", __FUNCTION__, u32PortMask);
				return SWHAL_ERR_FAILED;
		}

		for(bits=0;bits<RTK_PORT_LAN_NUM;bits++)
		{
				if(u32PortMask&(0x1<<bits))
				{
						u32RtkPortMask |= (0x1<<RTK_PORT_MAP[bits]);
				}
		}

		dprintf("[%s] u32RtkPortMask = 0x%x to 0x%x\n", __FUNCTION__, u32PortMask, u32RtkPortMask);
		
		return u32RtkPortMask;
}

int Util_IGMPTblEntrySearch(unsigned long u32GrpAddr, int *ps32FreeIdx)
{
		int	i, j, idx=-1;
		//unsigned int u32PageNo;
		unsigned long mask = htonl(0x007fffff);

		dprintf("[%s] u32GrpAddr = 0x%lx\n", __FUNCTION__, u32GrpAddr);

		for(j=0; j<RTK_IGMP_REC_PAGE_NO; j++)
		{
				for(i=0; i<RTK_IGMP_REC_PAGE; i++) 
				{
						if (gRtkIGMPTbl[j][i].s32Used) 
						{
								if ((gRtkIGMPTbl[j][i].u32GrpAddr & mask) == (u32GrpAddr & mask)) 
								{
										dprintf("[%s] got entry [%d][%d], address mask = 0x%lx\n", __FUNCTION__, j, i, (gRtkIGMPTbl[j][i].u32GrpAddr & mask));
										return i;
								}
						}
						else 
						{
								if (idx == -1)  idx = i;

								dprintf("[%s]got no entry, idx = %d\n", __FUNCTION__, idx);
						}
				}
		}
	
		if(ps32FreeIdx) *ps32FreeIdx = idx;

		return -1;
}

void Dump_RTK83XX_CPUPortPktHdr(RTK_CPU_EGRESS_PKT_HEADER *pstRtkEgressHdr/*, unsigned char u8PktHdrOpt*/)
{
		#if 0
		if(u8PktHdrOpt == 0) // Dump Global PktHdr
		{}
		else if(u8PktHdrOpt == 1) // Dump Global Current PktHdr
		{
				
		}
		else if(u8PktHdrOpt == 2)
		{}
		else
		#endif		
		dprintf("%s\n// = = = = = = = = = = = = = = = //\n%s", SWCLR1_37_WHITE, SWCLR0_RESET);
		dprintf("%s Realtek EtherTypeHi   0x%x\n%s", SWCLR1_37_WHITE, pstRtkEgressHdr->u32EthTypeHi, SWCLR0_RESET);
		dprintf("%s Realtek EtherTypeLo   0x%x\n%s", SWCLR1_37_WHITE, pstRtkEgressHdr->u32EthTypeLo, SWCLR0_RESET);
		dprintf("%s Realtek Protocol      0x%x\n%s", SWCLR1_37_WHITE, pstRtkEgressHdr->u32Protocol, SWCLR0_RESET);
		dprintf("%s Realtek Reason        0x%x\n%s", SWCLR1_37_WHITE, pstRtkEgressHdr->u32Reason, SWCLR0_RESET);

		dprintf("%s Realtek EFID          0x%x\n%s", SWCLR1_37_WHITE, (pstRtkEgressHdr->u32EFID&DWBIT31)>>31, SWCLR0_RESET);
		dprintf("%s Realtek Enhanced FID  0x%x\n%s", SWCLR1_37_WHITE, (pstRtkEgressHdr->u32EnhancedFID&0x07000000)>>24, SWCLR0_RESET);
		dprintf("%s Realtek Priority Sel  0x%x\n%s", SWCLR1_37_WHITE, (pstRtkEgressHdr->u32PriSelect&DWBIT27)>>27, SWCLR0_RESET);
		dprintf("%s Realtek Priority      0x%x\n%s", SWCLR1_37_WHITE, (pstRtkEgressHdr->u32Priority&0x00070000)>>16, SWCLR0_RESET);
		dprintf("%s Realtek Keep          0x%x\n%s", SWCLR1_37_WHITE, (pstRtkEgressHdr->u32Keep&DWBIT23)>>23, SWCLR0_RESET);
		dprintf("%s Realtek VSEL          0x%x\n%s", SWCLR1_37_WHITE, (pstRtkEgressHdr->u32VSEL&DWBIT22)>>22, SWCLR0_RESET);
		dprintf("%s Realtek Dis-Learning  0x%x\n%s", SWCLR1_37_WHITE, (pstRtkEgressHdr->u32DisableLearning&DWBIT21)>>21, SWCLR0_RESET);
		dprintf("%s Realtek VIDX          0x%x\n%s", SWCLR1_37_WHITE, (pstRtkEgressHdr->u32VIDX&0x1F0000)>>16, SWCLR0_RESET);

		dprintf("%s Realtek PortNo        0x%x\n%s", SWCLR1_37_WHITE, pstRtkEgressHdr->u32PortNo&0x7, SWCLR0_RESET);
		dprintf("%s// = = = = = = = = = = = = = = = //\n\n%s", SWCLR1_37_WHITE, SWCLR0_RESET);

		return;
}

void Dump_RTL83XX_ExtPort(unsigned char u8Port, unsigned char u8Mode)
{
		rtk_port_mac_ability_t stPortXability;
		rtk_api_ret_t RetVal;

		RetVal = rtk_port_macForceLinkExt_get(u8Port, (rtk_mode_ext_t*)&u8Mode, &stPortXability);
		if(RetVal == RT_ERR_OK)
		{	
				if(stPortXability.forcemode == 1)
					dprintf("Port[%d] in force mode!\n",u8Port);
				else
					dprintf("Port[%d] in auto-polling mode!\n",u8Port);

				if(stPortXability.speed == 0)
					dprintf("Port[%d]'s speed is 10M!\n",u8Port);
				else if(stPortXability.speed == 1)
					dprintf("Port[%d]'s speed is 100M!\n",u8Port);
				else if(stPortXability.speed == 2)
					dprintf("Port[%d]'s speed is 1G!\n",u8Port);

				if(stPortXability.duplex == 1)
					dprintf("Port[%d] in Duplex mode!\n",u8Port);
				else
					dprintf("Port[%d] in Half-Duplex mode!\n",u8Port);

				if(stPortXability.link == 1)
					dprintf("Port[%d] in Link-Up mode!\n",u8Port);
				else
					dprintf("Port[%d] in Link-Down mode!\n",u8Port);

				if(stPortXability.nway == 1)
					dprintf("Port[%d] in Auto-Nego mode!\n",u8Port);
				else
					dprintf("Port[%d] disable Auto-Nego mode!\n",u8Port);

				if(stPortXability.txpause == 1)
					dprintf("Port[%d] enable tx pause frame!\n",u8Port);
				else
					dprintf("Port[%d] disable tx pause frame!\n",u8Port);

				if(stPortXability.rxpause == 1)
					dprintf("Port[%d] enable rx pause frame!\n\n",u8Port);
				else
					dprintf("Port[%d] disable rx pause frame!\n\n",u8Port);
		}
		else
		{
				if(RetVal == RT_ERR_FAILED)
					dprintf("rtk_port_macForceLinkExt_get Failed!\n");
				else if(RetVal == RT_ERR_SMI)
					dprintf("rtk_port_macForceLinkExt_get SMI access error!\n");
				else if(RetVal == RT_ERR_INPUT)
					dprintf("rtk_port_macForceLinkExt_get Invalid input parameters!\n");
		}
		
		return;
}

void Dump_RTL83XX_ExtPortAbility(unsigned char u8RtlChipId, unsigned char u8Port, unsigned char u8Mode)
{
		//dprintf("[%s] Dump %s ext. port ability.\n", __FUNCTION__, gps8SwDevChipName);

		if(u8Port >= EXT_PORT_END)
		{
				dprintf("Wrong port number!\n");
				return;
		}

		if(u8Mode == MODE_EXT_DISABLE)
		{
				dprintf("Mode disable!\n");
				return;
		}

		switch(u8RtlChipId)
		{
				case CHIP_ID_RTL8363SB:
				case CHIP_ID_RTL8367MB:
				case CHIP_ID_RTL8367MVB:
				case CHIP_ID_RTL8307MVB:
						Dump_RTL83XX_ExtPort(u8Port, u8Mode);			
						break;
				case CHIP_ID_RTL8365MB:
				case CHIP_ID_RTL8367RVB:
				case CHIP_ID_RTL8305MB:
						if(u8Port != EXT_PORT_1)
						{
								dprintf("Wrong port number!\n"); 
								return;
						}
						
						Dump_RTL83XX_ExtPort(EXT_PORT_1, u8Mode);
						break;
				case CHIP_ID_RTL8367VB:
						dprintf("No Extension port!\n");
						break;
				case CHIP_ID_RTL8367RB:
						Dump_RTL83XX_ExtPort(u8Port, u8Mode);			
						break;
				case CHIP_ID_RTL8368MB:
						Dump_RTL83XX_ExtPort(u8Port, u8Mode);			
						break;
				default :
						dprintf("Wrong RTK chip type (0x%02x)!\n", u8RtlChipId);
						break;
		}

		return;
}

void Dump_RTL83XX_ExtPortAbilityAll(unsigned int u32RtlChipNum, unsigned char u8Mode)
{
		unsigned char u8PortNum;
		//dprintf("[%s] Dump %s ext. port ability.\n", __FUNCTION__, gps8SwDevChipName);

		switch(u32RtlChipNum)
		{
				case CHIP_ID_RTL8363SB:
				case CHIP_ID_RTL8367MB:
				case CHIP_ID_RTL8367MVB:
				case CHIP_ID_RTL8307MVB:
							for(u8PortNum = EXT_PORT_0; u8PortNum <= EXT_PORT_1; u8PortNum++)
							{
									if(u8PortNum == EXT_PORT_2)	return;
									Dump_RTL83XX_ExtPort(u8PortNum, u8Mode);
							}
							
							break;
							
				case CHIP_ID_RTL8365MB:
				case CHIP_ID_RTL8367RVB:
				case CHIP_ID_RTL8305MB:	
							Dump_RTL83XX_ExtPort(EXT_PORT_1, u8Mode);

							break;
							
				case CHIP_ID_RTL8367VB:
							dprintf("No Extension port!\n");
					
							break;
							
				case CHIP_ID_RTL8367RB:
							for(u8PortNum = EXT_PORT_1; u8PortNum <= EXT_PORT_2; u8PortNum++)
							{
									if(u8PortNum >= EXT_PORT_END)	return;
									Dump_RTL83XX_ExtPort(u8PortNum, u8Mode);
							}
							
							break;
							
				case CHIP_ID_RTL8368MB:
							for(u8PortNum = EXT_PORT_0; u8PortNum <= EXT_PORT_2; u8PortNum++)
							{
									if(u8PortNum == EXT_PORT_END)	return;
									Dump_RTL83XX_ExtPort(u8PortNum, u8Mode);
							}
							
							break;
							
				default :
							dprintf("wrong rtl chip type (0x%02x)!\n", u32RtlChipNum);
							
							break;
		}

		return;
}

/*****************************************************************
*	Function
*****************************************************************/
/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_Init  			
*
*   DESCRIPTION:
*
*		1. rtk_switch_init()
*		2. RTL8367RB 
*	   	 Set EXT_PORT_1 as MODE_EXT_RGMII
*	     		 EXT_PORT_2 as MODE_EXT_DISABLE
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_Init(void)
{
		int s32RetVal;
		int s32L2LUTInitStatus, s32InitStatus=0;

		unsigned int u32TimeOut=0;

		dprintf("%s[%s] %s initize ...\n%s", SWCLR1_33_YELLOW, __FUNCTION__, RTK_CHIP_NAME_STRING, SWCLR0_RESET);

		/* 0) HW reset */
		if(SWHAL_ERR_SUCCESS != RTL83XX_HalComm_HwRst(RTK_REST_DELAY))
			return SWHAL_ERR_RESET;

		mdelay(RTK_INIT_DELAY);

		do {

				mdelay(RTK_INIT_DELAY);
				//dprintf("reg[0x1202]=0x%04X, time=%d\n",RTL83XX_SMI_READ(0x1202),u32TimeOut+=100);

				if(u32TimeOut > 20*RTK_INIT_DELAY)
				{
						dprintf("%sMDIO Interface Error! Time Out!\n%s", SWCLR1_31_RED, SWCLR0_RESET);	return SWHAL_ERR_FAILED;
				}
			
		}while(0x88A8 != RTL83XX_SMI_READ(0x1202));
		//dprintf("RTL HW-RST Finished!, time=%d\n\n",u32TimeOut);

		/* 1) Init RTK Switch Chip */		
		sprintf(gs8RtkUtilCmdStr, "RTK_SysInit 0x%hx", (int)&s32RetVal);
		RTK_Switch_Utility(gs8RtkUtilCmdStr);
		if(s32RetVal != SWHAL_ERR_SUCCESS)
		{
				dprintf("[%s] s32InitStatus = 0x%x, gunIOParams.u32PtrAddr = 0x%x\n", __FUNCTION__, s32InitStatus, gunIOParams.u32PtrAddr);
				dprintf("[%s] Error RTK SysInit, errno = 0x%x\n", __FUNCTION__, s32RetVal); 
				return SWHAL_ERR_SYSINIT;
		}

		/* Dump Register 
		RTL83XX_SMI_READ(RTL8367B_REG_DIGITAL_INTERFACE_SELECT);
		RTL83XX_SMI_READ(RTL8367B_REG_DIGITAL_INTERFACE0_FORCE);
		RTL83XX_SMI_READ(RTL8367B_REG_DIGITAL_INTERFACE1_FORCE);
		*/

		/* 2) Init Interface */
		/* 2-1) Set Force External Interface */
		sprintf(gs8RtkUtilCmdStr, "RTK_PortForceLinkExtCfgSet %d %d %d %d %d %d %d %d %d", RTK_EXT_1, MODE_EXT_RGMII, 1, 1000, PORT_FULL_DUPLEX, PORT_LINKUP, 0, 0, 0);
		if(SWHAL_ERR_SUCCESS != (s32RetVal = RTK_Switch_Utility(gs8RtkUtilCmdStr)))
				return s32RetVal;
		//Dump_RTL83XX_ExtPortAbility(gs8SwDevChipId, RTK_EXT_1, MODE_EXT_RGMII);

		sprintf(gs8RtkUtilCmdStr, "RTK_PortForceLinkExtCfgSet %d %d %d %d %d %d %d %d %d", RTK_EXT_2, MODE_EXT_DISABLE, 0, 0, 0, 0, 0, 0, 0);
		if(SWHAL_ERR_SUCCESS != (s32RetVal = RTK_Switch_Utility(gs8RtkUtilCmdStr)))
				return s32RetVal;
		//Dump_RTL83XX_ExtPortAbility(gs8SwDevChipId, RTK_EXT_2, MODE_EXT_DISABLE);

		sprintf(gs8RtkUtilCmdStr, "RTK_PortRGMIIClkExtSet %d 0x%x 0x%x", 1, 1, 0);
		if(SWHAL_ERR_SUCCESS != (s32RetVal = RTK_Switch_Utility(gs8RtkUtilCmdStr)))
				return s32RetVal;

		/* 3) Init functions */
		/* 3-1) Set L2 LUT */
		sprintf(gs8RtkUtilCmdStr, "RTK_L2IpMulticastTableInit 0x%hx", (int)&s32L2LUTInitStatus);
		if(SWHAL_ERR_SUCCESS != (s32RetVal = RTK_Switch_Utility(gs8RtkUtilCmdStr)))
				return s32RetVal;

		sprintf(gs8RtkUtilCmdStr, "RTK_L2IpMulticastTableTypeSet 0x%x", LOOKUP_DIP);
		if(SWHAL_ERR_SUCCESS != (s32RetVal = RTK_Switch_Utility(gs8RtkUtilCmdStr)))
				return s32RetVal;

		/* 4) Init Others */
		/* 4-1) Set Max packet length */
		sprintf(gs8RtkUtilCmdStr, "RTK_MaxPktLengthSet 0x%x", 1536);
		if(SWHAL_ERR_SUCCESS != (s32RetVal = RTK_Switch_Utility(gs8RtkUtilCmdStr)))
				return s32RetVal;

		/* 4-2) Set CPU port tag */
		sprintf(gs8RtkUtilCmdStr, "RTK_CPUPortTagEnSet 0x%x", SW_ENABLED);
		if(SWHAL_ERR_SUCCESS != (s32RetVal = RTK_Switch_Utility(gs8RtkUtilCmdStr)))
				return s32RetVal;
		else
		{
				sprintf(gs8RtkUtilCmdStr, "RTK_CPUPortTagCfgSet 0x%x 0x%x", RTK_PORT_6_MAC, CPU_INSERT_TO_ALL);
				if(SWHAL_ERR_SUCCESS != (s32RetVal = RTK_Switch_Utility(gs8RtkUtilCmdStr)))
						return s32RetVal;

				/* Add CPU tagPort for CPU Port */
				//gstSwDevState.stRtkCPUPortStatus.s32CPUPortEnable = SW_ENABLED;
				//#ifdef _Vendor_TF_FTTH
				//gstSwDevState.stRtkCPUPortStatus.s32CPUPortNo			= RTK_PORT_6_MAC;
				//#endif
				//gstSwDevState.stRtkCPUPortStatus.s32CPUPortMode 	= CPU_INSERT_TO_ALL; 

		}

		//Test_Switch_Utility_Cmd();
				
		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_Hal_HwRst
*
*   DESCRIPTION:
*		RTL internal process
*
*   1. Determine various default settings via the hardware strap pins at the end of the nRESET signal 
*		2. Autoload the configuration from EEPROM if EEPROM is detected 
*		3. Complete the embedded SRAM BIST process 
*		4. Initialize the packet buffer descriptor allocation 
*		5. Initialize the internal registers and prepare them to be accessed by the external CPU 
*
*		Send reset pulse by VR9-288 GPIO 37
*	
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_HwRst(unsigned int u32PluseWidth)
{
		// GPIO settings
		unsigned int u32PinNo;

		#if((PROJECT_PLATFORM&PLATFORM_MASK) == PLATFORM_TELEFORNICA)
		u32PinNo = DWBIT05;    	// GPIO 37
		#elif((PROJECT_PLATFORM&PLATFORM_MASK) == PLATFORM_904DSL)
		u32PinNo = DWBIT09;			// GPIO 41
		#endif
		
		// clear P2_ALTSEL0_BITX
		*IFX_GPIO_P2_ALTSEL0 &= ~u32PinNo;
		*IFX_GPIO_P2_ALTSEL1 &= ~u32PinNo;

		// set P2_DIR_BITX as output 
		*IFX_GPIO_P2_DIR |= u32PinNo;
		*IFX_GPIO_P2_OD |= u32PinNo;

		// P2_OUT_BITX, pull-high first
		*IFX_GPIO_P2_OUT |= u32PinNo;

		// pull-low
		*IFX_GPIO_P2_OUT &= ~u32PinNo;
							
		asm("sync");

		// wait
		mdelay(u32PluseWidth);

		asm("sync");

		// pull-high
		*IFX_GPIO_P2_OUT |= u32PinNo;
		
		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_Restart
*
*   DESCRIPTION:
*   
*		1. RTL8367RB_Hal_SwRst()
*		2. RTL83XX_HalComm_HwRst()
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_Restart(void)
{
		RTL83XX_HalComm_HwRst(RTK_SWITCH_SWRST);
		
		/* need to wait 1 sec */
		mdelay(RTK_INIT_DELAY);

		RTL83XX_HalComm_Init();

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_PhyStatus_Get
*
*   DESCRIPTION:
*		The API get ethernet PHY linking status and will return auto 
*		negotiation status of phy.
*
*   u16PhyNo = port 0 ~ port 4
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_PortPhyStatus_Get(unsigned short u16PhyNo, unsigned int *pu32LinkStatus, unsigned int *pu32Speed, unsigned int *pu32Duplex)
{
		int s32RetVal;

 		//dprintf("u16PhyNo=0x%02X, RTK_PORT_ID_MAX=0x%02X\n",u16PhyNo,RTK_PORT_ID_MAX);
		if(u16PhyNo >= RTK_PORT_LAN_NUM)	return SWHAL_ERR_PARAMS;
		
		if(RT_ERR_OK !=	(s32RetVal = rtk_port_phyStatus_get((rtk_port_t)u16PhyNo, (rtk_port_linkStatus_t*)pu32LinkStatus, (rtk_data_t*)pu32Speed, (rtk_data_t*)pu32Duplex)))
		{
				RTL8367B_HAL_MSG(dprintf("Get ethernet PHY linking status failed! error = 0x%08x\n", s32RetVal));
				if(s32RetVal == RT_ERR_PORT_ID)	
						return SWHAL_ERR_PARAMS;
				else 
						return SWHAL_ERR_FAILED;
		}
		
		#if 0
		dprintf("Got PhyNo=%d Status\n",u16PhyNo);
		dprintf("PHY LinkStatus = 0x%02x\n", *pu32LinkStatus);
		dprintf("PHY Speed      = 0x%02x\n", *pu32Speed);
		dprintf("PHY Duplex     = 0x%02x\n\n", *pu32Duplex);
		#endif

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_PortStatus_Get
*
*   DESCRIPTION:
*
*   u16PortNo = port 0 ~ port 7
*
*		port 0 = port 0
*		port 1 = port 1
*		port 2 = port 2
*		port 3 = port 3
*		port 4 = port 4
*
*	  port 5 = EXT_PORT_0   
*		port 6 = EXT_PORT_1  
*		port 7 = EXT_PORT_2
*		
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_PortMacStatus_Get(unsigned short u16PortNo, RTK_PORT_MAC_STATUS *pstPortMacStatus)
{
		int s32RetVal;

		if(u16PortNo >= RTK_PORT_NUM)	return SWHAL_ERR_PARAMS;

		if(RTK_CHIP_ID == CHIP_ID_RTL8363SB)
		{
				if((u16PortNo == 0) || (u16PortNo == 3) || (u16PortNo == 4) || (u16PortNo == 7))
				{	return SWHAL_ERR_PARAMS;	}
		}
		else if((RTK_CHIP_ID == CHIP_ID_RTL8365MB) || \
			      (RTK_CHIP_ID == CHIP_ID_RTL8305MB))
		{
				if((u16PortNo == 4) || (u16PortNo == 5) || (u16PortNo == 7))
				{	return SWHAL_ERR_PARAMS;	}
		}
		else if(RTK_CHIP_ID == CHIP_ID_RTL8367VB)
		{
				if((u16PortNo == 5) || (u16PortNo == 6) || (u16PortNo == 7))
				{	return SWHAL_ERR_PARAMS;	}
		}
		else if(RTK_CHIP_ID == CHIP_ID_RTL8367RB)
		{
				if(u16PortNo == 5)
				{	return SWHAL_ERR_PARAMS;	}
		}
		else if(RTK_CHIP_ID == CHIP_ID_RTL8367RVB)
		{
				if((u16PortNo == 5) || (u16PortNo == 7))
				{	return SWHAL_ERR_PARAMS;	}
		}
		else if((RTK_CHIP_ID == CHIP_ID_RTL8367MB) || \
						(RTK_CHIP_ID == CHIP_ID_RTL8367MVB) || \
						(RTK_CHIP_ID == CHIP_ID_RTL8307MVB))
		{
				if(u16PortNo == 7)	{	return SWHAL_ERR_PARAMS;	}
		}
		else 
		{
			  if(RTK_CHIP_ID != CHIP_ID_RTL8368MB)
					return SWHAL_ERR_FAILED;
		}
		
		if(RT_ERR_OK !=	(s32RetVal = rtk_port_macStatus_get((rtk_port_t)u16PortNo, (rtk_port_mac_ability_t *)pstPortMacStatus)))
		{
				RTL8367B_HAL_MSG(dprintf("Get port link status failed! error=0x%08X\n", s32RetVal));
				if(s32RetVal == RT_ERR_PORT_ID)	return SWHAL_ERR_PARAMS;
				else return SWHAL_ERR_FAILED;
		}
		
		#if 0
		dprintf("Got Port=%d MacStatus\n",u16PortNo);
		dprintf("forcemode = 0x%02x\n",pstPortMacStatus->forcemode);
		dprintf("speed     = 0x%02x\n",pstPortMacStatus->speed);
		dprintf("duplex    = 0x%02x\n",pstPortMacStatus->duplex);
		dprintf("link      = 0x%02x\n",pstPortMacStatus->link);
		dprintf("nway      = 0x%02x\n",pstPortMacStatus->nway);
		dprintf("txpause   = 0x%02x\n",pstPortMacStatus->txpause);
		dprintf("rxpause   = 0x%02x\n\n",pstPortMacStatus->rxpause);
		#endif
		
		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_PortTag_Cfg
*
*   DESCRIPTION:
*   This API set CPU port function enable/disable.
*		
*	
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_PortTagEn_Cfg(unsigned int *pu32Enable, unsigned char u8Set)
{
		int s32RetVal;

		if(u8Set)
		{
				//dprintf("[%s] u8Set = %d\n", __FUNCTION__, u8Set);
				if(RT_ERR_OK != (s32RetVal = rtk_cpu_enable_set((rtk_enable_t)SW_ENABLED)))
				{
						dprintf("Error CPU tagPort, errno = %d\n", s32RetVal); 
						return SWHAL_ERR_FAILED; 
				}
		}
		else
		{
				if(RT_ERR_OK != (s32RetVal = rtk_cpu_enable_get((rtk_enable_t *)pu32Enable)))
				{
						dprintf("Error CPU tagPort, errno = %d\n", s32RetVal); 
						return SWHAL_ERR_FAILED; 
				}

				//dprintf("[%s] *pu32Enable = %d, u8Set = %d\n", __FUNCTION__, *pu32Enable, u8Set);

		}

		return SWHAL_ERR_SUCCESS;

}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_PortTag_Cfg
*
*   DESCRIPTION:
*   
*		
*	
*		
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_PortTag_Cfg(unsigned int *pu32PortNo, unsigned int *pu32Mode, unsigned char u8Set)
{
		int s32RetVal;

		if(u8Set)
		{
				//dprintf("[%s] *pu32PortNo = %d, *pu32Mode = %d, u8Set = %d\n", __FUNCTION__, *pu32PortNo, *pu32Mode, u8Set);

				if(RT_ERR_OK != (s32RetVal = rtk_cpu_tagPort_set((rtk_port_t)*pu32PortNo, (rtk_cpu_insert_t)*pu32Mode)))
				{
						dprintf("Error CPU tagPort, errno = %d\n", s32RetVal); 
						return SWHAL_ERR_FAILED; 
				}
		}
		else
		{
				if(RT_ERR_OK != (s32RetVal = rtk_cpu_tagPort_get((rtk_port_t *)pu32PortNo, (rtk_cpu_insert_t *)pu32Mode)))
				{
						dprintf("Error CPU tagPort, errno = %d\n", s32RetVal); 
						return SWHAL_ERR_FAILED; 
				}

				//dprintf("[%s] *pu32PortNo = %d, *pu32Mode = %d, u8Set = %d\n", __FUNCTION__, *pu32PortNo, *pu32Mode, u8Set);

		}

		return SWHAL_ERR_SUCCESS;

}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_ExtPortForceLink_Cfg (extension port)
*
*   DESCRIPTION:
*		This API can set/get external interface force mode properties.
*
*   u8ExtPortNo : 
*		EXT_PORT_0 ~ EXT_PORT_2
*
*		u8Mode : 
*		The external interface can be set to:
*   - MODE_EXT_DISABLE,
*   - MODE_EXT_RGMII,
*   - MODE_EXT_MII_MAC,
*   - MODE_EXT_MII_PHY,
*   - MODE_EXT_TMII_MAC,
*   - MODE_EXT_TMII_PHY,
*   - MODE_EXT_GMII,
*   - MODE_EXT_RMII_MAC,
*   - MODE_EXT_RMII_PHY,
*
*		pstPortMacStatus :
*
*		u8Set : 
*		Set(1) or Get(0)
*		
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_ExtPortForceLink_Cfg(unsigned char u8ExtPortNo, unsigned char *pu8Mode, RTK_PORT_MAC_STATUS *pstPortMacStatus, unsigned char u8Set)
{
		int s32RetVal;

		//dprintf("[%s] u8ExtPortNo = %d, pu8Mode = %d, u8Set = %d\n", __FUNCTION__, u8ExtPortNo, *pu8Mode, u8Set);
		
		/* Check EXT_PORT_X */
		if((RTK_CHIP_ID  == CHIP_ID_RTL8363SB) || \
			 (RTK_CHIP_ID == CHIP_ID_RTL8367MB)	|| \
			 (RTK_CHIP_ID == CHIP_ID_RTL8367MVB)|| \
			 (RTK_CHIP_ID == CHIP_ID_RTL8307MVB))
		{
				if((u8ExtPortNo != EXT_PORT_0) && (u8ExtPortNo != EXT_PORT_1))
					return SWHAL_ERR_PARAMS;
		}
		else if((RTK_CHIP_ID == CHIP_ID_RTL8365MB) || \
						(RTK_CHIP_ID == CHIP_ID_RTL8367RVB)|| \
						(RTK_CHIP_ID == CHIP_ID_RTL8305MB))
		{
				if(u8ExtPortNo != EXT_PORT_1)
					return SWHAL_ERR_PARAMS;
		}
		else if(RTK_CHIP_ID == CHIP_ID_RTL8367VB) 
		{
				return SWHAL_ERR_FAILED;
		}
		else if(RTK_CHIP_ID == CHIP_ID_RTL8367RB)
		{
				if((u8ExtPortNo != EXT_PORT_1) && (u8ExtPortNo != EXT_PORT_2))
					return SWHAL_ERR_PARAMS;
		}
		else
				return SWHAL_ERR_FAILED;

		/* Check Mode */
		if(*pu8Mode >= MODE_EXT_END) return SWHAL_ERR_PARAMS;

		/* Check Cfg */
		if(u8Set)
		{
				if(RT_ERR_OK !=	(s32RetVal = rtk_port_macForceLinkExt_set((rtk_port_t)u8ExtPortNo, (rtk_mode_ext_t)*pu8Mode, (rtk_port_mac_ability_t*)pstPortMacStatus)))
				{
						dprintf("Set port %d force linking configuration mode %d failed! error=0x%08x\n", u8ExtPortNo, *pu8Mode, s32RetVal);
						return SWHAL_ERR_FAILED;
				}

				//dprintf("\nSet ExtPort = %d MAC Status\n",u8ExtPortNo);
				//Dump_RTL83XX_ExtPortAbility(RTK_CHIP_ID, u8ExtPortNo, *pu8Mode);

		}
		else
		{
				if(RT_ERR_OK !=	(s32RetVal = rtk_port_macForceLinkExt_get((rtk_port_t)u8ExtPortNo, (rtk_mode_ext_t*)pu8Mode, (rtk_port_mac_ability_t*)pstPortMacStatus)))
				{
						dprintf("Get port force linking configuration failed! error=0x%08X\n", s32RetVal);
						return SWHAL_ERR_FAILED;
				}

				//dprintf("\nGot ExtPort = %d MAC Status\n",u8ExtPortNo);
				#if 0
				dprintf("forcemode = 0x%02x\n",pstPortMacStatus->forcemode);
				dprintf("speed     = 0x%02x\n",pstPortMacStatus->speed);
				dprintf("duplex    = 0x%02x\n",pstPortMacStatus->duplex);
				dprintf("link      = 0x%02x\n",pstPortMacStatus->link);
				dprintf("nway      = 0x%02x\n",pstPortMacStatus->nway);
				dprintf("txpause   = 0x%02x\n",pstPortMacStatus->txpause);
				dprintf("rxpause   = 0x%02x\n\n",pstPortMacStatus->rxpause);
				#else
				//Dump_RTL83XX_ExtPortAbility(RTK_CHIP_ID, u8ExtPortNo, *pu8Mode);
				#endif
		}

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_RGMIIDly_Set
*
*   DESCRIPTION:
*   This API can set external interface 2 RGMII delay.
*
*		u8ExtPortNo : 
*		EXT_PORT_0 ~ EXT_PORT_2
*
*   u32TxDelay :  
*		TX delay value, 1 for delay 2ns and 0 for no-delay
*      
*		u32RxDelay :
*		RX delay value, 0~7 for delay setup.
*
*		u8Set : 
*		Set(1) or Get(0)
*
*   NOTE:
*   In TX delay, there are 2 selection: no-delay and 2ns delay.
*   In RX dekay, there are 8 steps for delay tunning. 0 for no-delay, and 7 for maximum delay.
*
*****************************************************************/
int RTL83XX_HalComm_ExtPortRGMIIDly_Cfg(unsigned char u8ExtPortNo, unsigned int *pu32TxDelay, unsigned int *pu32RxDelay, unsigned char u8Set)
{
		int s32RetVal;

		/* Check EXT_PORT_X */		
		if((RTK_CHIP_ID == CHIP_ID_RTL8363SB) || \
			 (RTK_CHIP_ID == CHIP_ID_RTL8367MB)	|| \
			 (RTK_CHIP_ID == CHIP_ID_RTL8367MVB)|| \
			 (RTK_CHIP_ID == CHIP_ID_RTL8307MVB))
		{
				if((u8ExtPortNo != EXT_PORT_0) && (u8ExtPortNo != EXT_PORT_1))
					return SWHAL_ERR_PARAMS;
		}
		else if((RTK_CHIP_ID == CHIP_ID_RTL8365MB) || \
						(RTK_CHIP_ID == CHIP_ID_RTL8367RVB)|| \
						(RTK_CHIP_ID == CHIP_ID_RTL8305MB))
		{
				if(u8ExtPortNo != EXT_PORT_1)
					return SWHAL_ERR_PARAMS;
		}
		else if(RTK_CHIP_ID == CHIP_ID_RTL8367VB) 
		{
				return SWHAL_ERR_PARAMS;
		}
		else if(RTK_CHIP_ID == CHIP_ID_RTL8367RB)
		{
				if((u8ExtPortNo != EXT_PORT_1) && (u8ExtPortNo != EXT_PORT_2))
					return SWHAL_ERR_PARAMS;
		}
		else
				return SWHAL_ERR_FAILED;

		if(u8Set)
		{
				if(RT_ERR_OK != (s32RetVal = rtk_port_rgmiiDelayExt_set((rtk_ext_port_t)u8ExtPortNo, (rtk_data_t)*pu32TxDelay, (rtk_data_t)*pu32RxDelay)))
				{
					dprintf("RTL ERRNO = 0x%08X\n", s32RetVal);
					return SWHAL_ERR_FAILED;
				}
		}
		else
		{
				if(RT_ERR_OK != (s32RetVal = rtk_port_rgmiiDelayExt_get((rtk_ext_port_t)u8ExtPortNo, (rtk_data_t*)pu32TxDelay, (rtk_data_t*)pu32RxDelay)))
				{
						dprintf("RTL ERRNO = 0x%08X\n", s32RetVal);
						return SWHAL_ERR_FAILED;
				}
		}
		
		return SWHAL_ERR_SUCCESS;
}


/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_PortState_Get
*
*   DESCRIPTION:
*   
*		
*	
*		
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_PortState_Get(unsigned int u32PortNo, unsigned int u32PktType, unsigned long long *pu64PktCnt)
{
		int s32RetVal;


		if(RT_ERR_OK != (s32RetVal = rtk_stat_port_get((rtk_port_t)u32PortNo, (rtk_stat_port_type_t)u32PktType, (rtk_stat_counter_t*)pu64PktCnt)))
		{
				dprintf("Error Getting Port %d  Type 0x%08X State, errno = 0x%08x\n", u32PortNo, u32PktType, s32RetVal);
				return SWHAL_ERR_FAILED;
		}

		dprintf("Got u32PortNo=%d, u32PktType=0x%08x, pu64PktCnt=0x%llx\n", u32PortNo, u32PktType, *pu64PktCnt);

		
		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_PhyEnableAll_Cfg
*
*   DESCRIPTION:
*   All PHYs are disabled by hardware pin, enable all PHYs
*		
*	
*		u8Set : 
*		Set(1) or Get(0)
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_PhyEnableAll_Cfg(unsigned int *pu32PhyEnable, unsigned char u8Set)
{
		int s32RetVal;

		if(u8Set)
		{
				if(RT_ERR_OK != (s32RetVal = rtk_port_phyEnableAll_set((rtk_enable_t)ENABLE)))
				{
						dprintf("Error Setting all PHY enable status, errno = %d\n", s32RetVal);
						return SWHAL_ERR_FAILED;
				}
		}
		else
		{
				if(RT_ERR_OK != (s32RetVal = rtk_port_phyEnableAll_get((rtk_enable_t *)pu32PhyEnable)))
				{
						dprintf("Error Getting all PHY enable status, errno = %d\n", s32RetVal);
						return SWHAL_ERR_FAILED;
				}
				else
						dprintf("u32PhyEnable = %d\n", *pu32PhyEnable);
		}

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_PhyTestMode_Cfg
*
*   DESCRIPTION:
*		Set PHY in test mode.
*
*   u8PhyNo : 
*		RTL8367B_PHY_0 ~ RTL8367B_PHY_4
*
*		u8TestMode : 
*		PHY_TEST_MODE_NORMAL= 0,
*   PHY_TEST_MODE_1,
*   PHY_TEST_MODE_2,
*   PHY_TEST_MODE_3,
*   PHY_TEST_MODE_4,
*
*   NOTE:
*   Set PHY in test mode and only one PHY can be in test mode at 
*		the same time.
*
*****************************************************************/
int RTL83XX_HalComm_PhyTestMode_Cfg(unsigned char u8PhyNo, unsigned char u8TestMode)
{
		int s32RetVal;

		dprintf("%s[%s]u8PhyNo=%d, u8TestMode=%d\n%s", SWCLR1_33_YELLOW, __FUNCTION__, u8PhyNo, u8TestMode, SWCLR0_RESET);
		if(u8PhyNo >= RTK_PORT_LAN_NUM)	return SWHAL_ERR_PARAMS;

		#if 0
		if(RTK_CHIP_ID == CHIP_ID_RTL8363SB)
		{
				if((u8PhyNo == 0) || (u8PhyNo == 3) || (u8PhyNo == 4))
				{	return SWHAL_ERR_PARAMS;	}
		}
		else if((RTK_CHIP_ID == CHIP_ID_RTL8365MB) || \
			      (RTK_CHIP_ID == CHIP_ID_RTL8305MB))
		{
				if(u16PortNo == 4)
				{	return SWHAL_ERR_PARAMS;	}
		}
		else 
		{		
				if((RTK_CHIP_ID != CHIP_ID_RTL8367MB)   && \
						(RTK_CHIP_ID != CHIP_ID_RTL8367MVB) && \
						(RTK_CHIP_ID != CHIP_ID_RTL8307MVB) && \
						(RTK_CHIP_ID != CHIP_ID_RTL8367VB)  && \
						(RTK_CHIP_ID != CHIP_ID_RTL8367RB)  && \
						(RTK_CHIP_ID != CHIP_ID_RTL8367RVB) && \
						(RTK_CHIP_ID != CHIP_ID_RTL8368MB))
					return SWHAL_ERR_FAILED;
		}
		#endif

		if(RT_ERR_OK != (s32RetVal = rtk_port_phyTestMode_set((rtk_port_t)u8PhyNo, (rtk_port_phy_test_mode_t)u8TestMode)))
		{
				dprintf("Error Setting Phy %d TestMode %d, errno=%0x08x\n", u8PhyNo, u8TestMode, s32RetVal);
				return SWHAL_ERR_FAILED;
		}
		
		
		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_L2LUTInit
*
*   DESCRIPTION:
*   Initialize l2 module before calling any l2 APIs
*		
*	
*		
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_L2LUTInit(void)
{
		int s32RetVal;
		//unsigned int u32Port, u32PortMax;
		unsigned int u32LookUpType;

		//dprintf("[%s] L2LUTInit\n", __FUNCTION__);

		/* Init L2 LUT */ 
		if(RT_ERR_OK != (s32RetVal = rtk_l2_init()))
		{
				dprintf("Error enable L2 LUT, errno = 0x%x\n", s32RetVal); 
				return SWHAL_ERR_L2INIT; 
		}

		/* Enable look-up function and look-up for DIP+SIP */
		//#if _IGMP_SNOOP_EHT
		//u32LookUpType = LOOKUP_SIP_DIP;
		//#else
		u32LookUpType = LOOKUP_DIP;
		//#endif
		
		if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddrLookup_set(u32LookUpType)))
		{
				dprintf("Error enable L2 LUT IP look-up function, errno = 0x%x\n", s32RetVal); 
				return SWHAL_ERR_FAILED; 
		}

		/* Flush LUT */
		#if 0
		if(RTK_CHIP_ID == CHIP_ID_RTL8367RB)
		{
				u32PortMax = RTL8367B_MAC_LAN_END;
		}

		for(u32Port=0; u32Port<u32PortMax; u32Port++)
		{
				if(RT_ERR_OK !=	(s32RetVal = rtk_l2_flushType_set(FLUSH_TYPE_BY_PORT, 0, u32Port)))
				{
						dprintf("Error flush LUT for port %d, error = 0x%x\n", u32Port, s32RetVal);
						return SWHAL_ERR_FAILED;
				}
		}
		#endif
		
		//gstSwDevState.stSwL2LUTInfo.u8FlgInit 				= SW_ENABLED;

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_L2LUT_McastAddr_Cfg
*
*   DESCRIPTION:
*   Initialize l2 module before calling any l2 APIs
*		
*   pu8MacAddr : 
*		6 bytes multicast(I/G bit is 1) mac address to be written into LUT.
*
*   u32Ivl :
*		IVL/SVL setting, 1:IVL, 0:SVL
*
*  	u32CVidFid :
*		Filtering database
*
*   NOTE:
*   If the multicast mac address already existed in the LUT, it will udpate the
*   port mask of the entry. Otherwise, it will find an empty or asic auto learned
*   entry to write. If all the entries with the same hash value can't be replaced,
*   ASIC will return a RT_ERR_L2_INDEXTBL_FULL error.
*
* 	If the mac has existed in the LUT, it will be deleted. Otherwise, it will return 
*		RT_ERR_L2_ENTRY_NOTFOUND.
*
*****************************************************************/
int RTL83XX_HalComm_L2LUT_McastAddr_Cfg(unsigned char *pu8MacAddr, unsigned int u32Ivl, unsigned int u32CVidFid, unsigned int u32PortMask, unsigned char u8Set)
{
		int s32RetVal;
		rtk_portmask_t stRtkPortMask;
			
		dprintf("[%s:%d] group address = %02x:%02x:%02x:%02x:%02x:%02x, u32Ivl = 0x%x, u32CVidFid = 0x%x, pu32PortMask = 0x%08x, u8Set = %d, 1(add) or 0(del)\n", 
			__FUNCTION__, __LINE__, pu8MacAddr[0], pu8MacAddr[1], pu8MacAddr[2], pu8MacAddr[3], pu8MacAddr[4], pu8MacAddr[5],
			u32Ivl, u32CVidFid, u32PortMask, u8Set);


		if(u8Set)
		{
				stRtkPortMask.bits[0] = u32PortMask;
				//dprintf("[%s] stPortMask.bits = 0x%x, u8Set = %d\n", __FUNCTION__, stRtkPortMask.bits[RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST], u8Set);

				if(RT_ERR_L2_INDEXTBL_FULL == (s32RetVal = rtk_l2_mcastAddr_add((rtk_mac_t *)pu8MacAddr, (rtk_data_t)u32Ivl, (rtk_data_t)u32CVidFid, stRtkPortMask)))
				{
						dprintf("Error L2 McastAddr Add by mac addr, errno = %d\n", s32RetVal); 
						return SWHAL_ERR_FAILED; 
				}
		}
		else
		{ 
				if(RT_ERR_L2_INDEXTBL_FULL == (s32RetVal = rtk_l2_mcastAddr_del((rtk_mac_t *)pu8MacAddr, (rtk_data_t)u32Ivl, (rtk_data_t)u32CVidFid)))
				{
						dprintf("Error L2 McastAddr Del by mac addr, errno = %d\n", s32RetVal); 
						return SWHAL_ERR_FAILED; 
				}

				dprintf("[%s] u8Set = %d\n", __FUNCTION__, u8Set);

		}

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_L2LUT_McastCurrEntry_Get
*
*   DESCRIPTION:
*   Get LUT multicast entry port mask.
*		
*   pu8MacAddr : 
*		6 bytes multicast(I/G bit is 1) mac address to be written into LUT.
*
*   u32Ivl :
*		IVL/SVL setting, 1:IVL, 0:SVL
*
*  	u32CVidFid :
*		Filtering database
*
*		pu32PortMask :
*		Port mask
*
*   NOTE:
*   If the multicast mac address existed in the LUT, it will return 
*		the port where the mac is learned. Otherwise, it will return a 
*		RT_ERR_L2_ENTRY_NOTFOUND error.
*
*****************************************************************/
int RTL83XX_HalComm_L2LUT_McastCurrEntryMask_Get(unsigned char *pu8MacAddr, unsigned int u32Ivl, unsigned int u32CVidFid, unsigned int *pu32PortMask)
{
		int s32RetVal;
		rtk_mac_t *pstRtkMacAddr;
		rtk_portmask_t stRtkPortMask;
		
		dprintf("[%s:%d] group address = %02x:%02x:%02x:%02x:%02x:%02x, u32Ivl = 0x%x, u32CVidFid = 0x%x\n", 
			__FUNCTION__, __LINE__, pu8MacAddr[0], pu8MacAddr[1], pu8MacAddr[2], pu8MacAddr[3], pu8MacAddr[4], pu8MacAddr[5],
			u32Ivl, u32CVidFid);

		pstRtkMacAddr = (rtk_mac_t *)pu8MacAddr;
			
		if(RT_ERR_OK != (s32RetVal = rtk_l2_mcastAddr_get((rtk_mac_t *)pstRtkMacAddr, (rtk_data_t)u32Ivl, (rtk_data_t)u32CVidFid, (rtk_portmask_t *)&stRtkPortMask)))
		{
				dprintf("Get L2 LUT current entry by mac addr, errno = %d\n", s32RetVal); 
				return SWHAL_ERR_FAILED; 
		}

 		*pu32PortMask = stRtkPortMask.bits[0];
		dprintf("[%s:%d] *pu32PortMask = 0x%x\n",__FUNCTION__, __LINE__, stRtkPortMask.bits[RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST]);

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_L2LUT_McastNextEntry_Get
*
*   DESCRIPTION:
*   Get next LUT multicast entry.
*		
*   pu32AddrIdx : 
*		The Address ID
*
*   pu8MacAddr : 
*		6 bytes multicast(I/G bit is 1) mac address to be written into LUT.
*
*   u32Ivl :
*		IVL/SVL setting, 1:IVL, 0:SVL
*
*  	u32CVidFid :
*		Filtering database
*
*		
*   NOTE:
*   Get the next L2 multicast entry after the current entry pointed by pu32CurrMacAddr.
*   The address of next entry is returned by pAddress. User can use (address + 1)
*   as pAddress to call this API again for dumping all multicast entries in LUT.
*
*****************************************************************/
unsigned int RTL83XX_HalComm_L2LUT_McastNextEntryIdx_Get(unsigned char *pu8MacAddr, unsigned int u32Ivl, unsigned int u32CVidFid, unsigned int u32PortMask)
{
		int s32RetVal;
		rtk_portmask_t stRtkPortMask;
		unsigned int u32AddrIdx; 

		dprintf("[%s:%d] group address = %02x:%02x:%02x:%02x:%02x:%02x, u32Ivl = 0x%x, u32CVidFid = 0x%x, u32PortMask = 0x%08x\n", 
			__FUNCTION__, __LINE__, pu8MacAddr[0], pu8MacAddr[1], pu8MacAddr[2], pu8MacAddr[3], pu8MacAddr[4], pu8MacAddr[5], u32Ivl, u32CVidFid, u32PortMask);

		stRtkPortMask.bits[0] = u32PortMask;
		if(RT_ERR_OK != (s32RetVal = rtk_l2_mcastAddr_next_get((rtk_uint32 *)&u32AddrIdx, (rtk_mac_t *)pu8MacAddr, (rtk_data_t *)&u32Ivl, (rtk_data_t *)&u32CVidFid, (rtk_portmask_t *)&stRtkPortMask)))
		{
				dprintf("Get L2 LUT next entry by mac addr, errno = %d\n", s32RetVal); 
				return SWHAL_ERR_FAILED; 
		}
 
		return u32AddrIdx;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_L2LUT_IpMcastAddr_Cfg
*
*   DESCRIPTION:
*   Add/Del Lut IP multicast entry by IP.
*		
*   u32SIP : 
*		Source IP Address.
*
*   u32DIP :
*		Destination IP Address.
*
*   portmask : 
*	 	Destination port mask.
*
*   NOTE:
*   System supports L2 entry with IP multicast DIP/SIP to forward IP multicasting frame as user
*   desired. If this function is enabled, then system will be looked up L2 IP multicast entry to
*   forward IP multicast frame directly without flooding.
*
*   The API can delete a IP multicast address entry from the specified device.
*
*****************************************************************/
int RTL83XX_HalComm_L2LUT_IpMcastAddr_Cfg(unsigned int u32DIP, unsigned int u32SIP, unsigned int u32PortMask, unsigned char u8Set)
{
		int s32RetVal;
		rtk_portmask_t stRtkPortMask;
		unsigned char *pu8DIP, *pu8SIP;

		//if(gstSwDevState.stSwL2LUTInfo.u8FlgInit == SW_DISABLED)
		//{
				//dprintf("[%s] L2 LUT not inited!\n", __FUNCTION__);
				//return SWHAL_ERR_FAILED;
		//}

		pu8DIP = (unsigned char *)&u32DIP;
		pu8SIP = (unsigned char *)&u32SIP;

		
		dprintf("[%s:%d] group address = %d.%d.%d.%d, source address = %d.%d.%d.%d, u32PortMask = 0x%x, u8Set = %d\n", 
			__FUNCTION__, __LINE__, pu8DIP[0], pu8DIP[1], pu8DIP[2], pu8DIP[3],	
			pu8SIP[0], pu8SIP[1], pu8SIP[2], pu8SIP[3], u32PortMask, u8Set);

		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		//BLOCK_PREEMPTION;
		#endif
		
		if(u8Set)
		{
				#if 0
				if(u32PortMask > 0xF)
				{
						dprintf("[%s] u32PortMask overflow!\n", __FUNCTION__, u32PortMask);
						return SWHAL_ERR_FAILED;
				}
				#endif
	
				stRtkPortMask.bits[0] = Util_ConvertToRTKPortMask(u32PortMask);
				dprintf("[%s] stRtkPortMask.bits = 0x%x, u8Set = %d\n", __FUNCTION__, stRtkPortMask.bits[0], u8Set);

				if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_add((ipaddr_t)u32SIP, (ipaddr_t)u32DIP, (rtk_portmask_t)stRtkPortMask)))
				{
						dprintf("Error L2 IpMcastAddr Add, errno = 0x%x\n", s32RetVal); 
						return SWHAL_ERR_FAILED; 
				}
		}
		else
		{ 
				if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_del((ipaddr_t)u32SIP, (ipaddr_t)u32DIP)))
				{
						dprintf("Error L2 IpMcastAddr Del, errno = 0x%x\n", s32RetVal); 
						return SWHAL_ERR_FAILED; 
				}

				//dprintf("[%s] u8Set = %d\n", __FUNCTION__, u8Set);
		}

		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		//UNBLOCK_PREEMPTION;
		#endif
		
		#if 0
		dprintf("[%s] reg[0x0510] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0510));
		dprintf("[%s] reg[0x0511] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0511));
		dprintf("[%s] reg[0x0512] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0512));
		dprintf("[%s] reg[0x0513] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0513));
		dprintf("[%s] reg[0x0514] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0514));
		dprintf("[%s] reg[0x0515] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0515));
		#endif

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_L2LUT_IpMcastCurrEntry_Get
*
*   DESCRIPTION:
*   Get LUT multicast entry by IP.
*		
*   u32SIP : 
*		Source IP Address.
*
*   u32DIP :
*		Destination IP Address.
*
*   portmask : 
*	 	Destination port mask.
*
*   NOTE:
*   If the multicast mac address existed in the LUT, it will return 
*		the port where the mac is learned. Otherwise, it will return a 
*		RT_ERR_L2_ENTRY_NOTFOUND error.
*
*****************************************************************/
int RTL83XX_HalComm_L2LUT_IpMcastCurrEntryMask_Get(unsigned int u32DIP, unsigned int u32SIP, unsigned int *pu32PortMask)
{
		int s32RetVal;
		rtk_portmask_t stRtkPortMask;
		unsigned char *pu8DIP, *pu8SIP;

		pu8DIP = (unsigned char *)&u32DIP;
		pu8SIP = (unsigned char *)&u32SIP;

		dprintf("[%s:%d] group address = %d.%d.%d.%d, source address = %d.%d.%d.%d\n", 
			__FUNCTION__, __LINE__, pu8DIP[0], pu8DIP[1], pu8DIP[2], pu8DIP[3], pu8SIP[0], pu8SIP[1], pu8SIP[2], pu8SIP[3]);

		if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_get((ipaddr_t)u32SIP, (ipaddr_t)u32DIP, (rtk_portmask_t *)&stRtkPortMask)))
		{
				dprintf("Get L2 LUT current entry by IP, errno = 0x%x\n", s32RetVal); 
				return SWHAL_ERR_FAILED; 
		}

		*pu32PortMask = stRtkPortMask.bits[0];
		dprintf("[%s:%d] *pu32PortMask = 0x%x\n",__FUNCTION__, __LINE__, stRtkPortMask.bits[RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST]);

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_L2LUT_IpMcastNextEntry_Get
*
*   DESCRIPTION:
*   Get next LUT multicast entry by IP.
*		
*   pu32AddrIdx : 
*		The Address ID
*
*   pu8MacAddr : 
*		6 bytes multicast(I/G bit is 1) mac address to be written into LUT.
*
*   u32Ivl :
*		IVL/SVL setting, 1:IVL, 0:SVL
*
*  	u32CVidFid :
*		Filtering database
*
*		
*   NOTE:
*   Get the next L2 multicast entry after the current entry pointed by pu32CurrMacAddr.
*   The address of next entry is returned by pAddress. User can use (address + 1)
*   as pAddress to call this API again for dumping all multicast entries in LUT.
*
*****************************************************************/
unsigned int RTL83XX_HalComm_L2LUT_IpMcastNextEntryIdx_Get(unsigned int *pu32AddrIdx, unsigned int *pu32DIP, unsigned int *pu32SIP, unsigned int *pu32PortMask)
{
		int s32RetVal;
		rtk_portmask_t stRtkPortMask;
		unsigned int u32AddrIdx=1; 
		unsigned char *pu8DIP, *pu8SIP;

		pu8DIP = (unsigned char *)pu32DIP;
		pu8SIP = (unsigned char *)pu32SIP;

		stRtkPortMask.bits[0] = *pu32PortMask;

		if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_next_get((rtk_uint32 *)pu32AddrIdx, (ipaddr_t *)pu32SIP, (ipaddr_t *)pu32DIP, (rtk_portmask_t *)&stRtkPortMask)))
		{
				dprintf("Get L2 LUT next entry, errno = 0x%x\n", s32RetVal); 
				return SWHAL_ERR_FAILED; 
		}

		dprintf("[%s] group address = %d.%d.%d.%d, source address = %d.%d.%d.%d\n", 
			__FUNCTION__, pu8DIP[0], pu8DIP[1], pu8DIP[2], pu8DIP[3], pu8SIP[0], pu8SIP[1], pu8SIP[2], pu8SIP[3]);
 
		return u32AddrIdx;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_L2LUT_IpMcastTable_Dump
*
*   DESCRIPTION:
*   This API dump L2 multicast entry at whole system
*
*
*
*
*
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_L2LUT_IpMcastTable_Dump(void)
{
		int s32RetVal;
		unsigned int u32AddrIdx = 0; 
		unsigned int u32SIP, u32DIP; 
		//ipaddr_t u32SIP, u32DIP; 
		rtk_portmask_t stRtkPortMask;

		#if 0
		if(gstSwDevState.stSwL2LUTInfo.u8FlgInit == SW_DISABLED)
		{
				dprintf("[%s] L2 LUT not inited!\n", __FUNCTION__);
				return SWHAL_ERR_FAILED;
		}
		#endif

		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		BLOCK_PREEMPTION;
		#endif
		
		while(1) 
		{      
				//dprintf("[%s] u32AddrIdx = 0x%x(%d)\n", __FUNCTION__, u32AddrIdx, u32AddrIdx);
				
		    if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_next_get(&u32AddrIdx, &u32SIP, &u32DIP, &stRtkPortMask))) 
		    { 
							//dprintf("[%s] u32AddrIdx = 0x%x, u32SIP = 0x%x, u32DIP = 0x%x, PortMask = 0x%x\n", __FUNCTION__, u32AddrIdx, u32SIP, u32DIP, stRtkPortMask.bits[0]);
							//dprintf("[%s] error = 0x%x\n", __FUNCTION__, s32RetVal);
		          break; 
		    } 

				dprintf("[%s] u32AddrIdx = 0x%x, u32SIP = 0x%x, u32DIP = 0x%x, PortMask = 0x%x\n", __FUNCTION__, u32AddrIdx, u32SIP, u32DIP, stRtkPortMask.bits[0]);
		    u32AddrIdx++; 

				//dlytsk(cur_task, SW_DLY_TICKS/*DLY_TICKS*/, SW_MSECTOTICKS(100));
		}
		
		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		UNBLOCK_PREEMPTION;
		#endif

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_L2LUT_LookupTable_Dump
*
*   DESCRIPTION:
*   This API dump all entries from lookup table by index from 1~2112
*
*
*
*
*
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_L2LUT_LookupTable_Dump(void)
{
		int s32RetVal;
		rtk_l2_addr_table_t stL2Entry;
		unsigned int u32DIP=0, u32SIP=0;
		unsigned int stRtkPortMask=0;
		unsigned int u32AddrIdx=0;

		unsigned char *pu8EntrySIP,*pu8EntryDIP, SIP[16], DIP[16];

		//unsigned int i=1;

    memset(&stL2Entry,0,sizeof(rtk_l2_addr_table_t)); 

		dprintf("RTK L2LUT ========================================================\n");
    dprintf("\rIndex        Source_IP     Destination_IP    MemberPort    State\n\n");         

		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		BLOCK_PREEMPTION;
		#endif
		
		while(1)
		{  
				#if 0
		    stL2Entry.index = (int)RTL83XX_HalComm_L2LUT_IpMcastNextEntryIdx_Get(&u32AddrIdx, &u32DIP, &u32SIP, &stRtkPortMask); 
				if(stL2Entry.index == SWHAL_ERR_FAILED)	break;
				#else
				if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_next_get((rtk_uint32 *)&u32AddrIdx, (ipaddr_t *)&u32SIP, (ipaddr_t *)&u32DIP, (rtk_portmask_t *)&stRtkPortMask)))
				{
						//dprintf("Get L2 LUT next entry, errno = 0x%x\n", s32RetVal); 
						//return SWHAL_ERR_FAILED;
						break;
				}
				#endif

				stL2Entry.index = u32AddrIdx+1;
				//dprintf("[%s] index = 0x%x, u32AddrIdx = 0x%x\n", __FUNCTION__, stL2Entry.index, u32AddrIdx);

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

		            dprintf("%05d  ", stL2Entry.index);  
								dprintf("%15s    ", SIP);
								dprintf("%15s    ", DIP);
								dprintf("%08X    ", stL2Entry.portmask);
								//dprintf("%03d.%03d.%03d.%03d     ", pu8EntrySIP[0], pu8EntrySIP[1], pu8EntrySIP[2], pu8EntrySIP[3]);
								//dprintf("%03d.%03d.%03d.%03d     ", pu8EntryDIP[0], pu8EntryDIP[1], pu8EntryDIP[2], pu8EntryDIP[3]);
								dprintf("  %s\n", (stL2Entry.is_static? "Static" : "Auto"));
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
						//return SWHAL_ERR_FAILED;
						break;
				}

				//dlytsk(cur_task, SW_DLY_TICKS/*DLY_TICKS*/, SW_MSECTOTICKS(100));

				u32AddrIdx++;
					
		}
		
		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		UNBLOCK_PREEMPTION;
		#endif
		
		dprintf("END ==============================================================\n\n");

		return SWHAL_ERR_SUCCESS;
}


/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_IGMPInit
*
*   DESCRIPTION:
*   This API enables H/W IGMP and set a default initial configuration.
*
*
*		Be careful of that some reserved range IP addresses will always 
*		be flooded to all ports.
*
*		IPv4: 224.0.0.0 ~ 224.0.0.255 
*		IPv6: 33:33:00:00:00:00 ~ 33:33:00:00:00:FF 
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_IGMPInit(void)
{
		int s32RetVal;
		unsigned int u32Port;
		
		/* Check IGMP function is ENABLE or not */
		//if(gstSwDevState.stSwIgmpInfo.u8FlgInit == SW_ENABLED)
			//return SWHAL_ERR_SUCCESS;
		
		/*
		*	 Initialize H/W IGMP function
		*
		*  The API initialize IGMP/MLD snooping module and 
		*	 configure IGMP/MLD protocol action as following
		*
		*	 IGMPv1: H/W process 
		*  IGMPv2: H/W process 
		*	 IGMPv3: Flooding without process
		*
		*  MLDv1: H/W process 
		*  MLDv2: Flooding without process
		*
		*/
		/* 1) call init() to enable H/W IGMP and set a default initial configuration */
		if(RT_ERR_OK != (s32RetVal = rtk_igmp_init()))	
		{ 
				dprintf("Error init IGMP configuration, errno = %d\n", s32RetVal);
				//gstSwDevState.stSwIgmpInfo.u8FlgInit = SW_DISABLED;
				
				return SWHAL_ERR_HWIGMPINIT; 
		}

		/* 2) set others initial configuration */
		for(u32Port=0; u32Port<RTK_PORT_LAN_NUM; u32Port++)
		{
				//dprintf("u32Port = %d\n",u32Port);
				#if((PROJECT_PLATFORM&PLATFORM_MASK) == PLATFORM_TELEFORNICA)
				/* PROTOCOL_IGMPv3 deal with CPU */
				if(RT_ERR_OK !=	(s32RetVal = rtk_igmp_protocol_set((rtk_port_t)u32Port, (rtk_igmp_protocol_t)PROTOCOL_IGMPv3, (rtk_trap_igmp_action_t)IGMP_ACTION_TRAP2CPU)))
				{
						dprintf("Error setting IGMP protocol/action for port %d, error = 0x%x\n", u32Port, s32RetVal);
						if(s32RetVal == RT_ERR_PORT_ID)	return SWHAL_ERR_PARAMS;
						else if(s32RetVal == RT_ERR_INPUT) return SWHAL_ERR_PARAMS;
						else return SWHAL_ERR_FAILED;
				}

				/* MLDv2 drop */
				if(RT_ERR_OK !=	(s32RetVal = rtk_igmp_protocol_set((rtk_port_t)u32Port, (rtk_igmp_protocol_t)PROTOCOL_MLDv2, (rtk_trap_igmp_action_t)IGMP_ACTION_TRAP2CPU)))
				{
						dprintf("Error setting IGMP protocol/action for port %d, error = 0x%x\n", u32Port, s32RetVal);
						if(s32RetVal == RT_ERR_PORT_ID)	return SWHAL_ERR_PARAMS;
						else if(s32RetVal == RT_ERR_INPUT) return SWHAL_ERR_PARAMS;
						else return SWHAL_ERR_FAILED;
				}
				#endif

				//gstSwDevState.stSwIgmpInfo.u8HWSnooping    	= SW_ENABLED;
		}

		#if 0
		/* 3) Enable IGMP fastleave function */
		if(RT_ERR_OK != (s32RetVal = rtk_igmp_fastLeave_set(SW_ENABLED)))	
		{ 
				dprintf("Error enable IGMP fastleave, errno = 0x%x\n", s32RetVal);
				//gstSwDevState.stSwIgmpInfo.u8FastLeave = SW_DISABLED;
				
				return SWHAL_ERR_FAILED; 
		}

		//gstSwDevState.stSwIgmpInfo.u8FastLeave 			= SW_ENABLED;
		#endif

		//gstSwDevState.stSwIgmpInfo.u8FlgInit 				= SW_ENABLED;


		#if 0 // Dump IGMP Port Action
		{
				unsigned int u32Action;
				unsigned int u32HWSnooping, u32FastLeave;
				
				RTL83XX_HalComm_IGMPPortAction_Dump();
				
				if(RT_ERR_OK != (s32RetVal = rtk_igmp_state_get((rtk_enable_t *)&u32HWSnooping)))	
				{ 
						dprintf("Error gotting IGMP H/W state, errno = 0x%x\n", s32RetVal); 
				}
				else
						dprintf("IGMP H/W state = 0x%x\n", u32HWSnooping);	

				if(RT_ERR_OK != (s32RetVal = rtk_igmp_fastLeave_get((rtk_enable_t *)&u32FastLeave)))	
				{ 
						dprintf("Error gotting IGMP fast-leave state, errno = 0x%x\n", s32RetVal); 
				}
				else
						dprintf("IGMP fast-leave state = 0x%x\n", u32FastLeave);	

		}
		#endif

		return SWHAL_ERR_SUCCESS;
}


/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_IGMPStaticRouterPort_Cfg
*
*   DESCRIPTION:
*   This API set/get static router port
*
*
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_IGMPStaticRouterPort_Cfg(unsigned int *pu32PortMask, unsigned char u8Set)
{
		int s32RetVal;
		rtk_portmask_t stRtkPortMask;

		if(u8Set)
		{
				//dprintf("[%s] *pu32PortMask = 0x%x, u8Set = %d\n", __FUNCTION__, *pu32PortMask, u8Set);

				if(*pu32PortMask > RTK_MAX_PORT_MASK)
				{
						dprintf("[%s] u32PortMask overflow\n", __FUNCTION__);
						return SWHAL_ERR_PARAMS;
				}

				stRtkPortMask.bits[0] = *pu32PortMask;

				if(RT_ERR_OK != (s32RetVal = rtk_igmp_static_router_port_set(stRtkPortMask)))
				{
						dprintf("Error setting IGMP static router port, errno = 0x%x\n", s32RetVal); 
						return SWHAL_ERR_FAILED; 
				}

				//gstSwDevState.stSwIgmpInfo.u8StaticRouterPortNo = stRtkPortMask.bits[0];
		}
		else
		{
				if(RT_ERR_OK != (s32RetVal = rtk_igmp_static_router_port_get((rtk_portmask_t *)&stRtkPortMask)))
				{
						dprintf("Error getting IGMP static router port, errno = 0x%x\n", s32RetVal); 
						return SWHAL_ERR_FAILED; 
				}

	 			*pu32PortMask = stRtkPortMask.bits[0];
				//dprintf("[%s] *pu32PortMask = 0x%x, u8Set = %d\n", __FUNCTION__, *pu32PortMask, u8Set);
		}

		return SWHAL_ERR_SUCCESS;
		
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_IGMPProtocol_Cfg
*
*   DESCRIPTION:
*   This API get IGMP/MLD protocol action
* 	Input:
*   u32Port     - Port ID
*   pu8Proto    - IGMP/MLD protocol
*
*		PROTOCOL_IGMPv1 = 0
*   PROTOCOL_IGMPv2 = 1
*   PROTOCOL_IGMPv3 = 2
*   PROTOCOL_MLDv1  = 3
*   PROTOCOL_MLDv2  = 4
*
*   pu8Action   - Per-port and per-protocol IGMP action setting
*
*   IGMP_ACTION_FORWARD = 0,
*   IGMP_ACTION_TRAP2CPU,
*   IGMP_ACTION_DROP,
*   IGMP_ACTION_FORWARD_EXCLUDE_CPU,
*   IGMP_ACTION_ASIC,
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_IGMPProtocol_Cfg(unsigned int u32Port, unsigned char *pu8Proto, unsigned char *pu8Action, unsigned char u8Set)
{
		int s32RetVal;

		if(u8Set)
		{
				dprintf("[%s] u32Port = %d, *pu8Proto = %d, *pu8Action = %d\n", __FUNCTION__, u32Port, *pu8Proto, *pu8Action);
				if(RT_ERR_OK !=	(s32RetVal = rtk_igmp_protocol_set((rtk_port_t)u32Port, (rtk_igmp_protocol_t)*pu8Proto, (rtk_trap_igmp_action_t)*pu8Action)))
				{
						RTL8367B_HAL_MSG(dprintf("Set IGMP/MLD protocol action failed! error=0x%08X\n", s32RetVal));
						if(s32RetVal == RT_ERR_PORT_ID)	return SWHAL_ERR_PARAMS;
						else return SWHAL_ERR_FAILED;
				}
		}
		else
		{
				if(RT_ERR_OK !=	(s32RetVal = rtk_igmp_protocol_get((rtk_port_t)u32Port, (rtk_igmp_protocol_t)*pu8Proto, (rtk_trap_igmp_action_t*)&pu8Action)))
				{
						RTL8367B_HAL_MSG(dprintf("Get IGMP/MLD protocol action failed! error=0x%08X\n", s32RetVal));
						if(s32RetVal == RT_ERR_PORT_ID)	return SWHAL_ERR_PARAMS;
						else return SWHAL_ERR_FAILED;
				}

				dprintf("[%s] u32Port = %d, *pu8Proto = %d, *pu8Action = %d\n", __FUNCTION__, u32Port, *pu8Proto, *pu8Action);
		}

		return SWHAL_ERR_SUCCESS;
}


/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_IGMPMaxGroup_Cfg
*
*   DESCRIPTION:
*   This API set per port multicast group learning limit.
*
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_IGMPMaxGroup_Cfg(unsigned int u32Port, unsigned int *pu32Group, unsigned char u8Set)
{
		int s32RetVal;

		if(u8Set)
		{
				if(RT_ERR_OK !=	(s32RetVal = rtk_igmp_maxGroup_set((rtk_port_t)u32Port, (rtk_uint32)*pu32Group)))
				{
					RTL8367B_HAL_MSG(dprintf("Set per port multicast group learning limit failed! error=0x%08X\n", s32RetVal));
					if(s32RetVal == RT_ERR_OUT_OF_RANGE)	return SWHAL_ERR_PARAMS;
					else return SWHAL_ERR_FAILED;
				}
		}
		else
		{
				if(RT_ERR_OK !=	(s32RetVal = rtk_igmp_maxGroup_get((rtk_port_t)u32Port, (rtk_uint32*)&pu32Group)))
				{
					RTL8367B_HAL_MSG(dprintf("Get per port multicast group learning limit failed! error=0x%08X\n", s32RetVal));
					if(s32RetVal == RT_ERR_PORT_ID)	return SWHAL_ERR_PARAMS;
					else return SWHAL_ERR_FAILED;
				}
		}

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL8367RB_Hal_IGMPHWSnoop_Disable
*
*   DESCRIPTION:
*   This API Disable H/W function, All Protocol will trap to CPU
*
*
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_IGMPHWSnoop_Disable(void)
{
		int s32RetVal;
		unsigned int u32Port;
		unsigned char u8Proto;

		//dprintf("[%s] Disable IGMP H/W\n", __FUNCTION__);
		
		/* 1) Disable HW Snooping, Action will be HW process */
		if(RT_ERR_OK != (s32RetVal = rtk_igmp_state_set(SW_DISABLED)))	
		{ 
				dprintf("Error enable IGMP configuration, errno = 0x%x\n", s32RetVal);										
				return SWHAL_ERR_FAILED; 
		}

		/* 2) Set all action will be trap to CPU */
		for(u32Port=0; u32Port<RTK_PORT_LAN_NUM; u32Port++)
		{
				for(u8Proto=0; u8Proto<PROTOCOL_END; u8Proto++)
				{
						if(RT_ERR_OK !=	(s32RetVal = rtk_igmp_protocol_set((rtk_port_t)u32Port, (rtk_igmp_protocol_t)u8Proto, (rtk_trap_igmp_action_t)IGMP_ACTION_TRAP2CPU)))
						{
								dprintf("Error setting IGMP protocol/action for port %d, error=0x%x\n", u32Port, s32RetVal);
								if(s32RetVal == RT_ERR_PORT_ID)	return SWHAL_ERR_PARAMS;
								else if(s32RetVal == RT_ERR_INPUT) return SWHAL_ERR_PARAMS;
								else return SWHAL_ERR_FAILED;
						}
				}
		}

		//RTL83XX_HalComm_IGMPPortAction_Dump();
		
		//gstSwDevState.stSwIgmpInfo.u8HWSnooping = SW_DISABLED;

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_IGMPPortAction_Get
*
*   DESCRIPTION:
*   This API get IGMP/MLD protocol action
*
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_IGMPPortAction_Get(unsigned int u32Port, unsigned int *pu32Action)
{
		int s32RetVal;
		unsigned char u8Proto;

		if(u32Port>RTK_PORT_LAN_NUM) return SWHAL_ERR_PARAMS;

		/*
		*  u8Proto : IGMP/MLD protocol
    *	 PROTOCOL_IGMPv1 = 0
    *	 PROTOCOL_IGMPv2 = 1
    *	 PROTOCOL_IGMPv3 = 2
    *	 PROTOCOL_MLDv1  = 3
    *	 PROTOCOL_MLDv2  = 4
    *	 PROTOCOL_END    = 5
    *
    *  u8Action : Per-port and per-protocol IGMP action seeting
    *  IGMP_ACTION_FORWARD 							= 0
    *  IGMP_ACTION_TRAP2CPU							= 1
    *  IGMP_ACTION_DROP 								= 2
    *  IGMP_ACTION_FORWARD_EXCLUDE_CPU 	= 3
    *  IGMP_ACTION_ASIC                	= 4
    *  IGMP_ACTION_END                 	= 5
		*/
		for(u8Proto=0; u8Proto<PROTOCOL_END; u8Proto++)
		{
				if(RT_ERR_OK !=	(s32RetVal = rtk_igmp_protocol_get((rtk_port_t)u32Port, (rtk_igmp_protocol_t)u8Proto, (rtk_trap_igmp_action_t*)pu32Action)))
				{
						RTL8367B_HAL_MSG(dprintf("Get IGMP/MLD protocol %d, action failed! error=0x%08X\n", u8Proto, s32RetVal));
						if(s32RetVal == RT_ERR_PORT_ID)	return SWHAL_ERR_PARAMS;
						else return SWHAL_ERR_FAILED;
				}

				//dprintf("[%s] RTL8367RB Port %d, Protocol %d, Action = %d\n", __FUNCTION__, u32Port, u8Proto, *pu32Action);
		}

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_IGMPPortCurGroupCnt_Get
*
*   DESCRIPTION:
*   This API get per port multicast group learning count.
*
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_HalComm_IGMPPortCurGroupCnt_Get(unsigned int u32Port, unsigned int *pu32GroupCnt)
{
		int s32RetVal;

		if(u32Port>RTK_PORT_LAN_NUM) return SWHAL_ERR_PARAMS;
		
		if(RT_ERR_OK !=	(s32RetVal = rtk_igmp_currentGroup_get((rtk_port_t)u32Port, (rtk_uint32*)pu32GroupCnt)))
		{
				RTL8367B_HAL_MSG(dprintf("Get per port multicast group learning count failed! error=0x%08X\n", s32RetVal));
				if((s32RetVal == RT_ERR_PORT_ID) || (s32RetVal == RT_ERR_NULL_POINTER))	return SWHAL_ERR_PARAMS;
				else return SWHAL_ERR_FAILED;
		}

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_IGMPPortAction_Dump
*
*   DESCRIPTION:
*   This API 
*
*
*
*
*
*
*   NOTE:
*
*
*
*****************************************************************/
int RTL83XX_HalComm_IGMPPortAction_Dump(void)
{
		//int s32RetVal;

		unsigned int u32Port=0, u32Action;

		if(u32Port>RTK_PORT_LAN_NUM) return SWHAL_ERR_PARAMS;

		for(u32Port=0; u32Port<RTK_PORT_LAN_NUM; u32Port++)
		{
				RTL83XX_HalComm_IGMPPortAction_Get(u32Port, &u32Action);
		}
		
		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_IGMPTbl_Dump
*
*   DESCRIPTION:
*   This API 
*
*
*
*
*
*
*   NOTE:
*
*
*
*****************************************************************/
int RTL83XX_HalComm_IGMPTbl_PageDump(unsigned int u32PageNo)
{
		//int s32RetVal;

		unsigned int i, j;
		unsigned char *u8ptr, ip[16];

		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		BLOCK_PREEMPTION;
		#endif
		
		dprintf("========== RTK IGMP snooping table ==========\n");
		for (i=0; i<RTK_IGMP_REC_PAGE; i++) 
		{
				u8ptr = (unsigned char *)&gRtkIGMPTbl[u32PageNo][i].u32GrpAddr;
				sprintf(ip, "%d.%d.%d.%d", u8ptr[0], u8ptr[1], u8ptr[2], u8ptr[3]);
				dprintf("%03d %1d %04d %15s %08X ( ", i, gRtkIGMPTbl[u32PageNo][i].s32Used, gRtkIGMPTbl[u32PageNo][i].u32EntryIdx, ip, (unsigned int)gRtkIGMPTbl[u32PageNo][i].u32PortMap);

				for (j=0; j<RTK_MAX_PORT; j++) 
				{
						dprintf("%ld ", gRtkIGMPTbl[u32PageNo][i].u32Timer[j]);
				}

				dprintf(")\n");
		}
		dprintf("=================== END ====================\n");

		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		UNBLOCK_PREEMPTION;
		#endif
		
		return SWHAL_ERR_SUCCESS;
}


#if 0

#endif

/*****************************************************************
*
*   PROCEDURE NAME:
*		RTL83XX_HalComm_IGMPTble_Dump
*
*   DESCRIPTION:
*   
*	
*	
*
*   NOTE:
*
*
*****************************************************************/
void RTL83XX_HalComm_IGMPTble_Dump(void)
{
		int i, j, k;
		unsigned char *u8ptr, ip[16];

		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		BLOCK_PREEMPTION;
		#endif
		
		dprintf("========== RTK IGMP snooping table ==========\n");
		for(i=0; i<RTK_IGMP_REC_PAGE_NO; i++) 
		{
				for(j=0; j<RTK_IGMP_REC_PAGE; j++)
				{
						u8ptr = (unsigned char *)&gRtkIGMPTbl[i][j].u32GrpAddr;
						sprintf(ip, "%d.%d.%d.%d", u8ptr[0], u8ptr[1], u8ptr[2], u8ptr[3]);
						dprintf("%03d %1d %15s %08X ( ", i, gRtkIGMPTbl[i][j].s32Used, ip, (unsigned int)gRtkIGMPTbl[i][j].u32PortMap);

						for(k=0; k<RTK_MAX_PORT; k++) 
						{
								dprintf("%ld ", gRtkIGMPTbl[i][j].u32Timer[k]);
						}

						dprintf(")\n");
				}
		}
		dprintf("=================== END ===================\n");

		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		UNBLOCK_PREEMPTION;
		#endif
}



#if(RTK_CHIP_ID == CHIP_ID_RTL8367RB)
/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL8367RB_Hal_SwRst  			
*
*   DESCRIPTION:
*		RTL internal process
*	
*		u8RstType =  RTL_SWITCH_SWRST
*		1. Clear the FIFO and re-start the packet buffer link list 
*		2. Restart the auto-negotiation process 
*
*		u8RstType =  RTL_SWITCH_CHIPRST
*		1. Download configuration from strap pin and EEPROM 
*		2. Start embedded SRAM BIST (Built-In Self Test) 
*		3. Clear all the Lookup and VLAN tables 
*		4. Reset all registers to default values 
*		5. Restart the auto-negotiation process
*
*   NOTE:
*
*****************************************************************/
int RTL8367RB_Hal_SwRst(unsigned char u8RstType)
{
		unsigned short u16Offset=0;

		//dprintf("Reg_ChipRst[0x%04X] = 0x%04X, Ori\n", RTL8367B_REG_CHIP_RESET, RTL83XX_SMI_READ(RTL8367B_REG_CHIP_RESET));

		if(u8RstType == RTK_SWITCH_CHIPRST)
				u16Offset = RTL8367B_CHIP_RST_OFFSET;
		else if(u8RstType == RTK_SWITCH_SWRST)
				u16Offset = RTL8367B_SW_RST_OFFSET;

		/* write 1 clear */
		RTL83XX_SMI_WRITE(RTL8367B_REG_CHIP_RESET, (RTL83XX_SMI_READ(RTL8367B_REG_CHIP_RESET) | (0x1 << u16Offset)));

		//dprintf("Reg_ChipRst[0x%04X] = 0x%04X, After\n", RTL8367B_REG_CHIP_RESET, RTL83XX_SMI_READ(RTL8367B_REG_CHIP_RESET));

		return SWHAL_ERR_SUCCESS;
}
#endif


/*****************************************************************
*
*   PROCEDURE NAME:
*   
*
*   DESCRIPTION:
*   This API 
*
*
*
*
*
*
*   NOTE:
*
*
*
*****************************************************************/
int RTL83XX_HalComm_(void)
{
		//int s32RetVal;


		
		return SWHAL_ERR_SUCCESS;
}



/*****************************************************************
*	Test Program
*****************************************************************/
#if 0
int TestProg_PortMacStatus_Get(void)
{
		unsigned short u16PortNo, i;
		rtk_port_mac_ability_t stPortMacStatus[RTL8367B_MAC_MAX];

		int s32RetVal;
		unsigned int u32Cnt=1;
		
		while(1)
		{
				dprintf("%sGet Port Mac Status %d times\n\n%s", CLR1_32_GREEN, u32Cnt, SWCLR0_RESET);

				for(u16PortNo=0; u16PortNo<RTK_PORT_NUM; u16PortNo++)
				{
						if(SWHAL_ERR_SUCCESS != (s32RetVal = RTL83XX_HalComm_PortMacStatus_Get(u16PortNo, &stPortMacStatus[u16PortNo])))
						{	
								if(s32RetVal == SWITCH_HAL_ERR_PORT)
								{
										dprintf("Port %d not support!\n\n",u16PortNo);
								}
								else
								{
										dprintf("Error Getting MAC status, errno=0x%08x\n",s32RetVal);
										return s32RetVal;
								}
						}
				}
				
				/* Dump table */
				dprintf("Port MAC Information\n");
				for(i=0; i<9; i++)
				{
						for(u16PortNo=0; u16PortNo<RTK_PORT_NUM; u16PortNo++)
						{
								if(i == 1)
								{
										if(u16PortNo == 0)
												dprintf("forcemode = %02d,  ", stPortMacStatus[u16PortNo].forcemode);
										else
										{
												if(u16PortNo == 5)	dprintf("||  ");
												dprintf("%02d,  ", stPortMacStatus[u16PortNo].forcemode);
										}
										
										if(u16PortNo == (RTL8367B_MAC_MAX-1))
												dprintf("\n");
								}
								else if(i == 2)
								{
										if(u16PortNo == 0)
												dprintf("speed     = %02d,  ", stPortMacStatus[u16PortNo].speed);
										else
										{
												if(u16PortNo == 5)	dprintf("||  ");
												dprintf("%02d,  ", stPortMacStatus[u16PortNo].speed);
										}

										if(u16PortNo == (RTL8367B_MAC_MAX-1))
												dprintf("\n");
								}
								else if(i == 3)
								{
										if(u16PortNo == 0)
												dprintf("duplex    = %02d,  ", stPortMacStatus[u16PortNo].duplex);
										else
										{
												if(u16PortNo == 5)	dprintf("||  ");
												dprintf("%02d,  ", stPortMacStatus[u16PortNo].duplex);
										}
										
										if(u16PortNo == (RTL8367B_MAC_MAX-1))
												dprintf("\n");
								}
								else if(i == 4)
								{
										if(u16PortNo == 0)
												dprintf("link      = %02d,  ", stPortMacStatus[u16PortNo].link);
										else
										{
												if(u16PortNo == 5)	dprintf("||  ");
												dprintf("%02d,  ", stPortMacStatus[u16PortNo].link);
										}

										if(u16PortNo == (RTL8367B_MAC_MAX-1))
												dprintf("\n");
								}
								else if(i == 5)
								{
										if(u16PortNo == 0)
												dprintf("duplex    = %02d,  ", stPortMacStatus[u16PortNo].duplex);
										else
										{
												if(u16PortNo == 5)	dprintf("||  ");
												dprintf("%02d,  ", stPortMacStatus[u16PortNo].duplex);
										}

										if(u16PortNo == (RTL8367B_MAC_MAX-1))
												dprintf("\n");
								}
								else if(i == 6)
								{
										if(u16PortNo == 0)
												dprintf("nway      = %02d,  ", stPortMacStatus[u16PortNo].nway);
										else
										{
												if(u16PortNo == 5)	dprintf("||  ");
												dprintf("%02d,  ", stPortMacStatus[u16PortNo].nway);
										}

										if(u16PortNo == (RTL8367B_MAC_MAX-1))
												dprintf("\n");
								}
								else if(i == 7)
								{
										if(u16PortNo == 0)
												dprintf("txpause   = %02d,  ", stPortMacStatus[u16PortNo].txpause);
										else
										{
												if(u16PortNo == 5)	dprintf("||  ");
												dprintf("%02d,  ", stPortMacStatus[u16PortNo].txpause);
										}

										if(u16PortNo == (RTL8367B_MAC_MAX-1))
												dprintf("\n");
								}
								else if(i == 8)
								{
										if(u16PortNo == 0)
												dprintf("rxpause   = %02d,  ", stPortMacStatus[u16PortNo].rxpause);
										else
										{
												if(u16PortNo == 5)	dprintf("||  ");
												dprintf("%02d,  ", stPortMacStatus[u16PortNo].rxpause);
										}

										if(u16PortNo == (RTL8367B_MAC_MAX-1))
												dprintf("\n");
								}
								else if(i == 0)
								{
										
								}
								else
								{
										return -1;
								}
						}
				}
				dprintf("\n");
				
				//mdelay(10);
				/* Wait 1 sec */
				dlytsk(0, SW_DLY_TICKS/*DLY_TICKS*/, SW_MSECTOTICKS(100));
				//dlytsk(0, SW_DLY_SECS, (1/100));

				u32Cnt++;

				if(u32Cnt == 2)
				{
						dprintf("%sGet Port MAC status finished!\n%s",CLR1_32_GREEN, SWCLR0_RESET); 
						break;
				}
				
		}

		return 0;

}


int TestProg_PortPhyStatus_Get(void)
{
		unsigned short u16PhyNo, i;

		#if 0
		unsigned char u8LinkStatus[RTL8367B_MAC_LAN_END];
		unsigned int u32Speed[RTL8367B_MAC_LAN_END];
		unsigned int u32Duplex[RTL8367B_MAC_LAN_END];
		#else
		SWITCH_PORTPHY_STATUS stPortPhyStatus[RTL8367B_MAC_LAN_END];
		#endif
		
		int s32RetVal;
		unsigned int u32Cnt=1;

		while(1)
		{
				dprintf("%sGet Port PHY status %d times\n\n%s", CLR1_32_GREEN, u32Cnt, SWCLR0_RESET);

				for(u16PhyNo=0; u16PhyNo<RTK_PORT_LAN_NUM; u16PhyNo++)
				{
						if(0 != (s32RetVal = RTL83XX_HalComm_PortPhyStatus_Get(u16PhyNo, &stPortPhyStatus[u16PhyNo].u32LinkStatus, &stPortPhyStatus[u16PhyNo].u32Speed, &stPortPhyStatus[u16PhyNo].u32Duplex)))
						{	
								dprintf("Error Getting Phy status, errno=0x%08x\n",s32RetVal);
								return s32RetVal;
						}
				}

				/* Dump table */
				dprintf("Port PHY Information\n");
				for(i=0; i<4; i++)
				{
						for(u16PhyNo=0; u16PhyNo<RTK_PORT_LAN_NUM; u16PhyNo++)
						{
								if(i == 1)
								{
										if(u16PhyNo == 0)
												dprintf("linkstatus = %02d,  ", stPortPhyStatus[u16PhyNo].u32LinkStatus);
										else
										{
												dprintf("%02d,  ", stPortPhyStatus[u16PhyNo].u32LinkStatus);
										}
										
										if(u16PhyNo == (RTL8367B_MAC_LAN_END-1))
												dprintf("\n");
								}
								else if(i == 2)
								{
										if(u16PhyNo == 0)
												dprintf("speed      = %02d,  ", stPortPhyStatus[u16PhyNo].u32Speed);
										else
										{
												dprintf("%02d,  ", stPortPhyStatus[u16PhyNo].u32Speed);
										}

										if(u16PhyNo == (RTL8367B_MAC_LAN_END-1))
												dprintf("\n");
								}
								else if(i == 3)
								{
										if(u16PhyNo == 0)
												dprintf("duplex     = %02d,  ", stPortPhyStatus[u16PhyNo].u32Duplex);
										else
										{
												dprintf("%02d,  ", stPortPhyStatus[u16PhyNo].u32Duplex);
										}
										
										if(u16PhyNo == (RTL8367B_MAC_MAX-1))
												dprintf("\n");
								}
								else if(i == 0)
								{
										
								}
								else
								{
										return -1;
								}
						}
				}
				dprintf("\n\n");
				
				//mdelay(10);
				/* Wait 1 sec */
				dlytsk(0, SW_DLY_TICKS/*DLY_TICKS*/, SW_MSECTOTICKS(1000));
				//dlytsk(0, SW_DLY_SECS, (1/100));

				u32Cnt++;

				if(u32Cnt == 2)
				{
						dprintf("%sGet Phy status finished!\n%s", CLR1_32_GREEN, SWCLR0_RESET); 
						break;
				}
		}

		return 0;
}



/*
// port statistic counter index 
typedef enum rtk_stat_port_type_e
{
    STAT_IfInOctets = 0,
    STAT_Dot3StatsFCSErrors,
    STAT_Dot3StatsSymbolErrors,
    STAT_Dot3InPauseFrames,
    STAT_Dot3ControlInUnknownOpcodes,
    STAT_EtherStatsFragments,
    STAT_EtherStatsJabbers,
    STAT_IfInUcastPkts,
    STAT_EtherStatsDropEvents,
    STAT_EtherStatsOctets,
    STAT_EtherStatsUnderSizePkts,
    STAT_EtherOversizeStats,
    STAT_EtherStatsPkts64Octets,
    STAT_EtherStatsPkts65to127Octets,
    STAT_EtherStatsPkts128to255Octets,
    STAT_EtherStatsPkts256to511Octets,
    STAT_EtherStatsPkts512to1023Octets,
    STAT_EtherStatsPkts1024to1518Octets,
    STAT_EtherStatsMulticastPkts,
    STAT_EtherStatsBroadcastPkts,
    STAT_IfOutOctets,
    STAT_Dot3StatsSingleCollisionFrames,
    STAT_Dot3StatsMultipleCollisionFrames,
    STAT_Dot3StatsDeferredTransmissions,
    STAT_Dot3StatsLateCollisions,
    STAT_EtherStatsCollisions,
    STAT_Dot3StatsExcessiveCollisions,
    STAT_Dot3OutPauseFrames,
    STAT_Dot1dBasePortDelayExceededDiscards,
    STAT_Dot1dTpPortInDiscards,
    STAT_IfOutUcastPkts,
    STAT_IfOutMulticastPkts,
    STAT_IfOutBroadcastPkts,
    STAT_OutOampduPkts,
    STAT_InOampduPkts,
    STAT_PktgenPkts,
    STAT_InMldChecksumError,
    STAT_InIgmpChecksumError,
    STAT_InMldSpecificQuery,
    STAT_InMldGeneralQuery,
    STAT_InIgmpSpecificQuery,
    STAT_InIgmpGeneralQuery,
    STAT_InMldLeaves,
    STAT_InIgmpInterfaceLeaves,
    STAT_InIgmpJoinsSuccess,
    STAT_InIgmpJoinsFail,
    STAT_InMldJoinsSuccess,
    STAT_InMldJoinsFail,
    STAT_InReportSuppressionDrop,
    STAT_InLeaveSuppressionDrop,
    STAT_OutIgmpReports,
    STAT_OutIgmpLeaves,
    STAT_OutIgmpGeneralQuery,
    STAT_OutIgmpSpecificQuery,
    STAT_OutMldReports,
    STAT_OutMldLeaves,
    STAT_OutMldGeneralQuery,
    STAT_OutMldSpecificQuery,
    STAT_InKnownMulticastPkts,
    STAT_IfInMulticastPkts,
    STAT_IfInBroadcastPkts,
    STAT_PORT_CNTR_END
}rtk_stat_port_type_t;
*/

int TestProg_PortState_Get(void)
{
		unsigned int u32PortNo;
		unsigned int u32PktType=STAT_IfInOctets;
		//unsigned long long u64PktCnt;
		unsigned long u64PktCnt;

		int s32RetVal;
		unsigned int u32Cnt=1;

		while(1)
		{
				dprintf("get status %d times\n",u32Cnt);

				for(u32PortNo=0; u32PortNo<RTL8367B_MAC_LAN_END; u32PortNo++)
				{
						#if 0
						if(0 != (s32RetVal = RTL83XX_HalComm_PortState_Get(u32PortNo, u32PktType, &u64PktCnt)))
						{	
								dprintf("Error Getting port state, errno=0x%08x\n",s32RetVal);
								return s32RetVal;
						}
						#else
						if(0 != (s32RetVal = rtk_stat_port_get(u32PortNo, u32PktType, (rtk_stat_counter_t*)&u64PktCnt)))
						{	
								dprintf("Error Getting port state, errno=0x%08x\n",s32RetVal);
								return s32RetVal;
						}

						dprintf("Got u32PortNo=%d, u32PktType=0x%x, u64PktCnt=0x%x\n", u32PortNo, u32PktType, u64PktCnt);

						//if(0 != (s32RetVal = rtk_stat_port_getAll(u32PortNo, &u64PktCnt)))
						//{	
								//dprintf("Error Getting port state, errno=0x%08x\n",s32RetVal);
								//return s32RetVal;
						//}

						//dprintf("2 Got u32PortNo=%d, u64PktCnt=0x%08x\n", u32PortNo, u64PktCnt);

						#endif
						
				}

				//mdelay(10);
				/* Wait 1 sec */
				dlytsk(0, SW_DLY_TICKS/*DLY_TICKS*/, SW_MSECTOTICKS(100));
				//dlytsk(0, SW_DLY_SECS, (1/100));

				u32Cnt++;

				if(u32Cnt == 2)
				{
						dprintf("%sGet port state finished!\n%s", CLR1_32_GREEN, SWCLR0_RESET); 
						break;
				}
		}

		return 0;
}

/* 
for bug in struct fdb_ipmulticast_st,
struct fdb_ipmulticast_st
{
#ifdef _LITTLE_ENDIAN
rtk_uint16 sip0:8;
rtk_uint16 sip1:8;

rtk_uint16 sip2:8;
rtk_uint16 sip3:8;

rtk_uint16 dip0:8;
rtk_uint16 dip1:8;

rtk_uint16 dip2:8;
rtk_uint16 dip3:4;
rtk_uint16 l3lookup:1;
rtk_uint16 reserved:3;

rtk_uint16 mbr:8;
rtk_uint16 igmpidx:8;

rtk_uint16 igmp_asic:1;
rtk_uint16 lut_pri:3;
rtk_uint16 fwd_en:1;
rtk_uint16 nosalearn:1;
rtk_uint16 valid:1;
rtk_uint16 reserved2:9;
#else
rtk_uint16 sip1:8;
rtk_uint16 sip0:8;

rtk_uint16 sip3:8;
rtk_uint16 sip2:8;

rtk_uint16 dip1:8;
rtk_uint16 dip0:8;

rtk_uint16 reserved:3;		<<= here  
rtk_uint16 l3lookup:1;
rtk_uint16 dip3:4;
rtk_uint16 dip2:8;

rtk_uint16 igmpidx:8;
rtk_uint16 mbr:8;

rtk_uint16 reserved2:9;
rtk_uint16 valid:1;
rtk_uint16 nosalearn:1;
rtk_uint16 fwd_en:1;
rtk_uint16 lut_pri:3;
rtk_uint16 igmp_asic:1;
#endif
};

please fix API version v1.2.9, v1.2.10
*/
int TestProg_IpMcastAddr_Add(void)
{
		int s32RetVal;
		unsigned int dip, sip;
		rtk_portmask_t port_mask;

		#if 1 // Big Endian
		dip = 0xE0010203; //224.1.2.3
    sip = 0xC0A86401; //192.168.100.1
		#else  // Little  Endian
		dip = 0x030201E0; //224.1.2.3
    sip = 0x0164A8C0; //192.168.100.1 
		#endif

		dprintf("[%s] dip = 0x%x, sip = 0x%x\n", __FUNCTION__, dip, sip);

		port_mask.bits[0] = 0x5; //Port0& Port2 ¬°member port
		if ((s32RetVal = rtk_l2_ipMcastAddr_add(sip,dip,port_mask)) != RT_ERR_OK)
    {
				dprintf("[%s] add error \n", __FUNCTION__);
				//return s32RetVal;
		}

		dprintf("[%s] reg[0x0510] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0510));
		dprintf("[%s] reg[0x0511] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0511));
		dprintf("[%s] reg[0x0512] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0512));
		dprintf("[%s] reg[0x0513] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0513));
		dprintf("[%s] reg[0x0514] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0514));
		dprintf("[%s] reg[0x0515] = 0x%x\n\n", __FUNCTION__, RTL83XX_SMI_READ(0x0515));

		dprintf("[%s] reg[0x0520] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0520));
		dprintf("[%s] reg[0x0521] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0521));
		dprintf("[%s] reg[0x0522] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0522));
		dprintf("[%s] reg[0x0523] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0523));
		dprintf("[%s] reg[0x0524] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0524));
		dprintf("[%s] reg[0x0525] = 0x%x\n\n", __FUNCTION__, RTL83XX_SMI_READ(0x0525));

		dprintf("[%s] reg[0x0A30] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0A30));

		RTL83XX_HalComm_L2LUT_IpMcastTable_Dump();
		RTL83XX_HalComm_L2LUT_LookupTable_Dump();

		return 0;
}

int TestProg_Reg1_Dump(void)
{
		BLOCK_PREEMPTION;
		dprintf("[%s] reg[0x0510] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0510));
		dprintf("[%s] reg[0x0511] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0511));
		dprintf("[%s] reg[0x0512] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0512));
		dprintf("[%s] reg[0x0513] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0513));
		dprintf("[%s] reg[0x0514] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0514));
		dprintf("[%s] reg[0x0515] = 0x%x\n\n", __FUNCTION__, RTL83XX_SMI_READ(0x0515));

		dprintf("[%s] reg[0x0520] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0520));
		dprintf("[%s] reg[0x0521] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0521));
		dprintf("[%s] reg[0x0522] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0522));
		dprintf("[%s] reg[0x0523] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0523));
		dprintf("[%s] reg[0x0524] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0524));
		dprintf("[%s] reg[0x0525] = 0x%x\n\n", __FUNCTION__, RTL83XX_SMI_READ(0x0525));

		dprintf("[%s] reg[0x0A30] = 0x%x\n", __FUNCTION__, RTL83XX_SMI_READ(0x0A30));
		UNBLOCK_PREEMPTION;
		
		return 0;
}

int TestProg_DataType(void)
{


		dprintf("sizeof(unsigned int) = %d, sizeof(unsigned long) = %d\n",sizeof(unsigned int), sizeof(unsigned long));
		dprintf("sizeof(rtk_uint32) = %d, sizeof(ipaddr_t) = %d\n",sizeof(rtk_uint32), sizeof(ipaddr_t));

		return 0;
}

int TestProg_RTL83XX_HalComm_Init(void)
{
		int s32RetVal;
		unsigned long u64InitTimeSpent, u64LastTimeSnap;
		unsigned int i;

		u64LastTimeSnap = GetTime();//get_sys_time();
		//dprintf("[%s] Before call rtk_switch_init, u64LastTimeSnap = %ld\n", __FUNCTION__, u64LastTimeSnap);

		#if 0
		if(RT_ERR_OK != (s32RetVal = rtk_switch_init()))	
		{ 
				iprintf("Error RTK Init, errno = %d\n", s32RetVal); 
				return SWITCH_HAL_ERR_INIT; 
		}
		#else
		for(i=0;i<10;i++)
		{
				smi_write(0x1202, 0x88A8);
		}
		#endif
		dprintf("[%s] After call rtk_switch_init, time spent = %ld\n", __FUNCTION__, GetTime()-u64LastTimeSnap);

		//dprintf("[%s] After call rtk_switch_init, u64LastTimeSnap = %ld\n", __FUNCTION__, u64LastTimeSnap);
		//dprintf("[%s] After call rtk_switch_init, time spent = %ld\n", __FUNCTION__, GetTime()-u64LastTimeSnap);

		return 0;
}
#endif

