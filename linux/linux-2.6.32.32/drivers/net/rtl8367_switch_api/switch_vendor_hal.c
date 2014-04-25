/* ============================================================
Follow by 
This API package supports multiple Switches.

Programmer : Alvin Hsu, alvin_hsu@arcadyan.com
=============================================================== */
#include "switch_vendor_hal.h"

#if(_RTK_RTL83XX_SWITCH_API == 1)
#error "No available switch api, version error!!"
#elif(_RTK_RTL83XX_SWITCH_API == 2)
#include "realtek/rtl8367rb/switch_api_v1_2_10/include/rtk_api.h"
#endif


/*****************************************************************
*	Extern
*****************************************************************/
#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
extern int port_map[]; 					// logical to physical, 0-based
extern int logical_port_map[]; 	// physical to logical, 0-based

extern int vr9_GetPortLinkStatus(int port);

extern int ipnt_enable;
#elif((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_LINUX)
//extern int dlytsk(unsigned long slot, unsigned char dlytyp, unsigned int dlytme);
#endif

extern SWITCH_DEVICE_FILE_OPERATIONS gstRtl867RBFOps;
extern RTK_CPU_EGRESS_PKT_HEADER	gstRtkEgressHdr, gstCurrRtkEgressHdr; 


/*****************************************************************
*	Definition
*****************************************************************/
SWITCH_DEVICE_INFO gstSwDevInfo; 			// global switch information
SWITCH_DEVICE_STATE	gstSwDevState;		// global switch state

char *gps8SwDevChipName;							// global switch chip name string
char *gps8SwDevVersion;								// global switch version
char gs8SwDevChipId;									// global switch chip id

SWITCH_DEVICE_INFO gstSwDevInitInfo[] = {
	
#ifdef SWITCH_REALTEK_RTL83XX_FAMILY 
		#if(RTK_CHIP_ID == CHIP_ID_RTL8367RB)
    {RTK_CHIP_NAME_STRING, RTK_CHIP_API_VERSION, CHIP_ID_RTL8367RB, 0, &gstRtl867RBFOps}
		#endif
#else 
		{SWITCH_CHIP_NAME_NONE, SWITCH_CHIP_VERSION_NONE, SWITCH_CHIP_ID_NONE,0,((void *)0)}
#endif

};

#if defined(CONFIG_MAC0) && CONFIG_MAC0
void port_init(int port, int mode);
#endif /* CONFIG_MAC0*/


/*****************************************************************
*	Utility
*****************************************************************/
/* Switch driver plug */
int Util_SwitchDrvIdx(SWITCH_DEVICE_INFO *pstDevice, char *ps8ChipName)
{
    int i=0, idx=-1;

    while(pstDevice[i].ps8Name != NULL) 
		{
        if(strcmp(pstDevice[i].ps8Name, ps8ChipName) == 0) 
				{
            idx = i;	/* Protocol matches */
            break;
        }
				
        i++;
    }

    return idx;
}


#if(SWITCH_VENDOR_HAL_DEBUG_DUMP == SW_ENABLED)
void Dump_DevState(SWITCH_DEVICE_STATE *pstDevState)
{
		dprintf("%s\n// = = = = = = = = = = = = = = = //\n%s", SWCLR1_37_WHITE, SWCLR0_RESET);
		dprintf("%s  Switch Chip Name    = %s\n%s", SWCLR1_37_WHITE, pstDevState->stSwDevInfo.ps8Name, SWCLR0_RESET);
		dprintf("%s  Switch API version  = %s\n%s", SWCLR1_37_WHITE, pstDevState->stSwDevInfo.ps8Version, SWCLR0_RESET);
		dprintf("%s  Switch Chip ID      = %d\n%s", SWCLR1_37_WHITE, pstDevState->stSwDevInfo.s32ChipNo, SWCLR0_RESET);
		dprintf("%s  Switch Chip Initial = %d\n\n%s", SWCLR1_37_WHITE, pstDevState->stSwDevInfo.s8FlgInit, SWCLR0_RESET);

		dprintf("%s  RTK Main Port En    = %d\n%s", SWCLR1_37_WHITE, pstDevState->stRtkCPUPortStatus.s32CPUPortEnable, SWCLR0_RESET);
		//dprintf("%s  RTK Main Port       = %d\n%s", SWCLR1_37_WHITE, pstDevState->stRtkCPUPortStatus.s32CPUPortNo, SWCLR0_RESET);
		//dprintf("%s  RTK Main Port Mode  = %d\n\n%s", SWCLR1_37_WHITE, pstDevState->stRtkCPUPortStatus.s32CPUPortMode, SWCLR0_RESET);
#if 0
		dprintf("%s  RTK IGMP Initial     = %d\n%s", SWCLR1_37_WHITE, pstDevState->stSwIgmpInfo.u8FlgInit, SWCLR0_RESET);
		dprintf("%s  RTK IGMP HW Snooping = %d\n%s", SWCLR1_37_WHITE, pstDevState->stSwIgmpInfo.u8HWSnooping, SWCLR0_RESET);
		dprintf("%s  RTK IGMP FastLeave   = %d\n%s", SWCLR1_37_WHITE, pstDevState->stSwIgmpInfo.u8FastLeave, SWCLR0_RESET);
		dprintf("%s  RTK IGMP Router Port = 0x%x\n\n%s", SWCLR1_37_WHITE, pstDevState->stSwIgmpInfo.u8StaticRouterPortNo, SWCLR0_RESET);

		dprintf("%s  RTK L2LUT Initial    = %d\n%s", SWCLR1_37_WHITE, pstDevState->stSwL2LUTInfo.u8FlgInit, SWCLR0_RESET);
#endif

		dprintf("%s// = = = = = = = = = = = = = = = //\n\n%s", SWCLR1_37_WHITE, SWCLR0_RESET);

		return;
}

void Dump_Packet(unsigned char *pu8Data, unsigned int u32lLen)
{
		unsigned int i;

		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		BLOCK_PREEMPTION;
		#endif

		//dprintf("\n[%s] Len=%d\n", __FUNCTION__, u32lLen);

		for(i=0;i<u32lLen;i++)
		{
				//dprintf("%02x ",(unsigned char)(pu8Data[i]));

				if(i % 4 == 3)	dprintf(" ");
				if(i % 16 == 15)	dprintf("\n");
		}	

		if(i % 16 != 15)	
				dprintf("\n\n");
		else
				dprintf("\n");

		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		UNBLOCK_PREEMPTION;
		#endif

		return;
}
#endif

/*****************************************************************
*	Function
*****************************************************************/
#if(PROJECT_PLATFORM == PLATFORM_TELEFORNICA)
/*****************************************************************
*
*   PROCEDURE NAME:
*   VR9_BoardModified_GPIOCfg  			
*
*   DESCRIPTION:
*   board modified for PROJECT_PLATFORM = PLATFORM_TELEFORNICA
*		
*
*   NOTE:
*
*****************************************************************/
void VR9_BoardModified_GPIOCfg(void)
{
		// GPIO settings for board modified, merlin
		// clear P2_ALTSEL0_BIT8,9,14 / GPIO40,41,46
		*IFX_GPIO_P2_ALTSEL0 &= ~(WBIT08|WBIT09|WBIT14);
		*IFX_GPIO_P2_ALTSEL1 &= ~(WBIT08|WBIT09|WBIT14);

		// set P2_DIR_BIT8,9,14 as input / GPIO40,41,46
		*IFX_GPIO_P2_DIR &= ~(WBIT08|WBIT09|WBIT14);
		*IFX_GPIO_P2_OD &= ~(WBIT08|WBIT09|WBIT14);

		return;
}
#endif

/*****************************************************************
*
*   PROCEDURE NAME:
*   Switch_Hal_Init  			
*
*   DESCRIPTION:
*   This API support general driver plugging and pre-inited process.
*	
*	
* 	RETURN:
*   SWITCH_HAL_ERR_OK				- OK
*		SWITCH_HAL_ERR_RESET		- Hw reset pluse error
*		SWITCH_HAL_ERR_DRVPLUG	- Driver plug error
*		SWITCH_HAL_ERR_INIT			- Init failed
*
*   NOTE:
*
*****************************************************************/
int Switch_Hal_Init(void)
{
		unsigned int u32Idx;

		/* 0) Switch Chip Select */
		#ifdef SWITCH_REALTEK_RTL83XX_FAMILY
		gps8SwDevChipName = RTK_CHIP_NAME_STRING;
		gps8SwDevVersion	= RTK_CHIP_API_VERSION;
		gs8SwDevChipId		= RTK_CHIP_ID;
		#else
		gps8SwDevChipName = SWITCH_CHIP_NAME_NONE;		
		gps8SwDevVersion	= SWITCH_CHIP_VERSION_NONE;		
		gs8SwDevChipId		= SWITCH_CHIP_ID_NONE;						
		#endif

		dprintf("[%s] gps8SwDevChipName = %s, gps8SwDevVersion = %s, gs8SwDevChipId = %d\n", __FUNCTION__, gps8SwDevChipName, gps8SwDevVersion, gs8SwDevChipId);

		/* Switch driver plug */
		u32Idx = Util_SwitchDrvIdx(gstSwDevInitInfo, gps8SwDevChipName);
		if(u32Idx >= 0) 
		{
				/* 0) Switch driver plugging */
				/* Clear gstSwDevState info */
				memset(&gstSwDevState, 0, sizeof(SWITCH_DEVICE_STATE));

				gstSwDevState.stSwDevInfo = gstSwDevInitInfo[u32Idx];
				
				/*
				dprintf("Switch Chip Name    = %s\n", gstSwDevState.stSwDevInfo.ps8Name);
				dprintf("Switch API version  = %s\n", gstSwDevState.stSwDevInfo.ps8Version);
				dprintf("Switch Chip ID      = %d\n", gstSwDevState.stSwDevInfo.s32ChipNo);
				dprintf("Switch Chip Initial = %d\n\n", gstSwDevState.stSwDevInfo.s8FlgInit);
				*/
				
				#if(PROJECT_PLATFORM == PLATFORM_TELEFORNICA)
				/* 1) VR9 Configure */
				/* VR9_ETHSW_MDC_CFG_1 	: RES = 1, MCEN = 1, FREQ = 0x18
				 * PHY_ADDR_0 					: LNKST = 01, SPEED = 10, FDUP = 01.
				 * MII_PCDU_0_REG 			: RXDLY = 000, TXDLY = 011.
				 * VR9_ETHSW_MAC_CTRL_0 : GMII/RGMII interface mode at 1000Mbps
				*/
				#if defined(CONFIG_MAC0) && CONFIG_MAC0
				port_init(0, RGMII_MODE);
				#endif /* CONFIG_MAC0*/

				REG32(MII_CFG_0_REG)			 |=	 WBIT02;
				//REG32(VR9_ETHSW_MDC_CFG_1) 	= 0x00008118;
				REG32(PHY_ADDR_0) 					= (REG32(PHY_ADDR_0) & (~0x7E00)) | 0x3200;
				REG32(MII_PCDU_0_REG) 			= (REG32(MII_PCDU_0_REG) & (~0x0387)) | 0x0003; 	
				REG32(VR9_ETHSW_MAC_CTRL_0) = (REG32(VR9_ETHSW_MAC_CTRL_0) & (~0x0003)) | 0x0002;

				/*
				dprintf("\nREG[0x%08X] = 0x%08X\n",MII_CFG_0_REG,REG32(MII_CFG_0_REG)); 	
				dprintf("REG[0x%08X] = 0x%08X\n",VR9_ETHSW_MDC_CFG_1,REG32(VR9_ETHSW_MDC_CFG_1));
				dprintf("REG[0x%08X] = 0x%08X\n",PHY_ADDR_0,REG32(PHY_ADDR_0));
				dprintf("REG[0x%08X] = 0x%08X\n",MII_PCDU_0_REG,REG32(MII_PCDU_0_REG));
				dprintf("REG[0x%08X] = 0x%08X\n\n",VR9_ETHSW_MAC_CTRL_0,REG32(VR9_ETHSW_MAC_CTRL_0));
				*/
				
				/* ftth board modified for GPIO, merlin, 2012/03/29 */
				VR9_BoardModified_GPIOCfg();
				#endif

				/* 1) Switch init */
				if(SWHAL_ERR_SUCCESS == gstSwDevState.stSwDevInfo.pstSwDevFOps->init())
				{
						#if 0//def _SWITCH_REALTEK_RTL83XX_FAMILY
						gstSwDevState.stRtkCPUPortStatus.s32CPUPortEnable = gstRtkCPUPortStatus.s32CPUPortEnable;
						gstSwDevState.stRtkCPUPortStatus.s32CPUPortNo   	= gstRtkCPUPortStatus.s32CPUPortNo;
						gstSwDevState.stRtkCPUPortStatus.s32CPUPortMode 	= gstRtkCPUPortStatus.s32CPUPortMode;
						#endif
						
						gstSwDevState.stSwDevInfo.s8FlgInit 					  = 1;
						
						dprintf("%s[%s] Switch Initize Successful!%s", SWCLR1_33_YELLOW, __FUNCTION__, SWCLR0_RESET);
						//Dump_DevState(&gstSwDevState);
				}
				else
				{
						memset(&gstSwDevState, 0, sizeof(SWITCH_DEVICE_STATE));
						return SWHAL_ERR_SYSINIT;
				}
		}
	  else 
		{
				/* If driver plug failed, reset  gstSwDevState	*/
				memset(&gstSwDevState, 0, sizeof(SWITCH_DEVICE_STATE));
				return SWHAL_ERR_DRVPLUG;
	  }

		return SWHAL_ERR_SUCCESS;

}
#if 0
#define _LINKSTATUS_USE_MACSTATUS		1 	// use rtk_port_macStatus_get()
#define _LINKSTATUS_USE_PHYSTATUS		0		// use rtk_port_phyStatus_get()
/*****************************************************************
*
*   PROCEDURE NAME:
*   Switch_Hal_LinkUpStatus_Get  			
*
*   DESCRIPTION:
*   This API get Lan Link-Up Status
*
*   *pu32PhyNo : got link-up lan number
*
*								 0000 0000 0000 0000 : no port link-up
*                0000 0000 0000 0001 : port 0 link-up
*                0000 0000 0000 0010 : port 1 link-up
*                0000 0000 0000 0100 : port 2 link-up
*                0000 0000 0000 1000 : port 3 link-up
*
*                0000 0000 0001 0001 : port 4 link-up
*                0000 0000 0010 0010 : port 5 link-up
*                0000 0000 0100 0100 : port 6 link-up
*                0000 0000 1000 1000 : port 7 link-up
*
*		*pu8LinkUp : link-up status
*               
*		             0 : no link-up 
*	               1 : any link-up
*
*   u8ChipOpt : chip selection
*
*								 0 : Lantiq VR9
*								 1 : RTK, RTL8367RB
*
* 	RETURN:
*		SWITCH_HAL_ERR_OK(0)
*
*   NOTE:
*
*****************************************************************/
int Switch_Hal_LinkUpStatus_Get(unsigned int *pu32PhyNo, unsigned char *pu8LinkUp, unsigned char u8ChipOpt)
{
		int s32RetVal;
		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		int i;
		#endif
		unsigned short u16PhyNo, u16MaxPhyNo;
		#if(_LINKSTATUS_USE_PHYSTATUS == 1)
		unsigned int u32LinkStatus, u32Speed, u32Duplex;
		#endif
		#if(_LINKSTATUS_USE_MACSTATUS == 1)
		RTK_PORT_MAC_STATUS stPortMacStatus;
		#endif

		if(u8ChipOpt == 0)
		{
				#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
				for(i=0; i<SWITCH_MAX_PORT; i++)
				{
						if(1 == vr9_GetPortLinkStatus(port_map[i]))
						{
								*pu8LinkUp = 1;
								*pu32PhyNo = *pu32PhyNo | (0x1 << i);
						}
						else
						{
								*pu8LinkUp = 0;
								*pu32PhyNo = 0;
						}
				}

				//dprintf("[%s:%d] Show VR9 LinkUp Status, *pu32PhyNo = 0x%x, *pu8LinkUp = %d\n", __FUNCTION__, __LINE__, *pu32PhyNo, *pu8LinkUp);
				#elif((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_LINUX)
				printk(KERN_DEBUG "Not support now!\n");
				#endif
		}
		else if(u8ChipOpt == 1)
		{
				#ifdef SWITCH_REALTEK_RTL83XX_FAMILY // =============================== //
				u16MaxPhyNo = RTK_PORT_LAN_NUM;
				
				//dprintf("[%s] Show RTK LinkUpStatus\n", __FUNCTION__);
				//dprintf("[%s] u16MaxPhyNo = %d\n", __FUNCTION__, u16MaxPhyNo);
				*pu32PhyNo = 0;//((void *)0);
				*pu8LinkUp = 0;//((void *)0);
				
				for(u16PhyNo=0; u16PhyNo < u16MaxPhyNo; u16PhyNo++)
				{
						/* Get phy information */
						#if(_LINKSTATUS_USE_PHYSTATUS == 1)
						if(SWHAL_ERR_SUCCESS != (s32RetVal = RTL83XX_HalComm_PortPhyStatus_Get(u16PhyNo, &u32LinkStatus, &u32Speed, &u32Duplex)))
						{
								*pu8LinkUp = 0;
								*pu32PhyNo = 0;
								return SWHAL_ERR_FAILED;
						}
						#else
						if(SWHAL_ERR_SUCCESS != (s32RetVal = RTL83XX_HalComm_PortMacStatus_Get(u16PhyNo, (RTK_PORT_MAC_STATUS *)&stPortMacStatus)))
						{
								*pu8LinkUp = 0;
								*pu32PhyNo = 0;
								return SWHAL_ERR_FAILED;
						}
						#endif

						/* Check status */
						#if(_LINKSTATUS_USE_PHYSTATUS == 1)
						if(u32LinkStatus == PORT_LINKUP)
						#else
						if(stPortMacStatus.u32Link == PORT_LINKUP)
						#endif
						{
								*pu8LinkUp = 1;
								*pu32PhyNo |= (0x1<<u16PhyNo);
								//dprintf("[%s] Got one port link-up, port = %02d, 0x%x, *pu8LinkUp = %d\n", __FUNCTION__, u16PhyNo, *pu32PhyNo, *pu8LinkUp);
								//return SWITCH_HAL_ERR_OK;
						}

						/* No Lan link-up */
						if(u16PhyNo == (u16MaxPhyNo-1))
						{
								if(*pu8LinkUp == 1)
								{
										//dprintf("[%s] Got total link-up status, port = %02d, 0x%x, *pu8LinkUp = %d\n", __FUNCTION__, u16PhyNo, *pu32PhyNo, *pu8LinkUp);
								}
								else
								{
										*pu8LinkUp = 0;
										*pu32PhyNo = 0;

										//dprintf("[%s] Got no port link-up, port = 0x%x\n", __FUNCTION__, *pu32PhyNo);
										return SWHAL_ERR_FAILED;
								}
						}
				}
				#else
				dprint("Not RealTek chip! Please check _SWITCH_REALTEK_RTL83XX_FAMILY defined!\n");
				#endif // _SWITCH_REALTEK_RTL83XX_FAMILY ========================== //
		}

		return SWHAL_ERR_SUCCESS;
}
#endif

#ifdef SWITCH_REALTEK_RTL83XX_FAMILY
/*****************************************************************
*
*   PROCEDURE NAME:
*   Switch_Hal_CPUPortNo_Get  			
*
*   DESCRIPTION:
*   This API get port no. from cpu tag.
*	
*	
* 	RETURN:
*
*   NOTE:
*
*****************************************************************/
int Switch_Hal_CPUPortNo_Get(struct sk_buff *pstSkb, unsigned char *pu8PortNo)
{
		//int s32RetVal;

		unsigned char *pu8PktHdr;
		unsigned char *pu8EgressPktHdr = (unsigned char *)&gstCurrRtkEgressHdr;
		unsigned char u8Shift = 16;
		
		int i;
		
		pu8PktHdr = (unsigned char *)pstSkb->data+u8Shift;

		if(gstSwDevState.stRtkCPUPortStatus.s32CPUPortEnable)
		{
				/* Shift VLAN Tag */
				for(i=0; i<2; i++)
				{
						/* Check CPUPort Tag */
						if((pu8PktHdr[0] == 0x88) && (pu8PktHdr[1] == 0x99) && (pu8PktHdr[2] == 0x04))
						{
								//dprintf("[%s] Got RTK CPU-Port Tag!\n", __FUNCTION__);
								break;
						}
						//else
								//dprintf("[%s] wrong EthType[0x%02x][0x%02x], Protocol[0x%02x]\n", __FUNCTION__, pu8PktHdr[0], pu8PktHdr[1], pu8PktHdr[2]);


						u8Shift -= 4;
						pu8PktHdr -= 4;

						//dprintf("[%s] u8Shift = %02d\n", __FUNCTION__, u8Shift);

						/* Got no CPUPort Tag */
						if(u8Shift < 12)	return SWHAL_ERR_FAILED;
				}

				/* Capture CPUtag format into gstCurrRtkEgressHdr */
				for(i=0; i<=RTK_CPUTAG_PKTHDR_LEN; i++)
				{
						*pu8EgressPktHdr++ = pu8PktHdr[i];
				}

				//Dump_RTK83XX_CPUPortPktHdr(&gstCurrRtkEgressHsr);

				*pu8PortNo = (unsigned char)(gstCurrRtkEgressHdr.u32PortNo&0x0F);
				//dprintf("[%s:%d] gstCurrRtkEgressHdr.u32PortNo = %d, *pu8PortNo = %d\n", __FUNCTION__, __LINE__, gstCurrRtkEgressHdr.u32PortNo, *pu8PortNo);

				/* For project FTTH, get port no into BUF_BUFFER_FLAG2 */
				#if 0//def _Vendor_TF_FTTH
				BUF_BUFFER_FLAG2((char *)pstSkb) = RTK_LOG_PORT_MAP[gstCurrRtkEgressHdr.u32PortNo];
				dprintf("[%s:%d] gstCurrRtkEgressHdr.u32PortNo = %d, BUF_BUFFER_FLAG2 = %d\n", __FUNCTION__, __LINE__, gstCurrRtkEgressHdr.u32PortNo, BUF_BUFFER_FLAG2((char *)pstSkb));
				#endif		
		}
				
		return SWHAL_ERR_SUCCESS;
}
#endif


/*****************************************************************
*
*   PROCEDURE NAME:
*   Switch_Hal_Stripe_Tag  			
*
*   DESCRIPTION:
*   This API remove rtk switch tag
*	
*	
* 	RETURN:
*
*   NOTE:
*
*****************************************************************/
int Switch_Hal_StripeTag(struct sk_buff *pstSkb, unsigned int u32Shift, unsigned char u8TagLen)
{
		#ifdef SWITCH_REALTEK_RTL83XX_FAMILY
		unsigned char *pu8PktHdr; 

		//unsigned int u32TagEnable, u32PortNo, u32Mode;  //unused
		unsigned char *pu8EgressPktHdr = (unsigned char *)&gstCurrRtkEgressHdr;
		#endif
		
		unsigned char *pu8Dest, *pu8Src;
		int i;

		if((u32Shift <= 0) || (u8TagLen <= 0)) 
		{
				dprintf("[%s] wrong input, u32Shift = %d, u8TagLen = %02d\n", __FUNCTION__, u32Shift, u8TagLen);
				return SWHAL_ERR_PARAMS;
		}

		if(u32Shift <= u8TagLen)
		{
				dprintf("[%s] wrong input, u32Shift = %d, u8TagLen = %02d\n", __FUNCTION__, u32Shift, u8TagLen);
				return SWHAL_ERR_PARAMS;
		}

		if(pstSkb->len > u32Shift)
		{
				#ifdef SWITCH_REALTEK_RTL83XX_FAMILY
				if(gstSwDevState.stRtkCPUPortStatus.s32CPUPortEnable != SW_ENABLED)
				{
						dprintf("[%s] RTK CPU Port-Tag not Enable!, s32CPUPortEnable = %d\n", __FUNCTION__, gstSwDevState.stRtkCPUPortStatus.s32CPUPortEnable);	
						return SWHAL_ERR_CPUTAGINIT;
				}
				
				pu8PktHdr = pstSkb->data + u32Shift;

				if((pu8PktHdr[0] != 0x88) || (pu8PktHdr[1] != 0x99))
				{
						dprintf("[%s] wrong EthType[0x%02x][0x%02x]\n", __FUNCTION__, pu8PktHdr[0], pu8PktHdr[1]);
						return SWHAL_ERR_FAILED;
				}

				if(pu8PktHdr[2] != 0x04)
				{
						dprintf("[%s] wrong Protocol[0x%02x]\n", __FUNCTION__, pu8PktHdr[2]);
						return SWHAL_ERR_FAILED;
				}

				if(u8TagLen != RTK_CPUTAG_PKTHDR_LEN)
				{
						dprintf("[%s] wrong TagLen %d\n", __FUNCTION__, u8TagLen);
						return SWHAL_ERR_FAILED;
				}				

				dprintf("[%s] RTK EtherType [%02x][%02x], Protocol [%02d]\n", __FUNCTION__, pu8PktHdr[0], pu8PktHdr[1], pu8PktHdr[2]);
				
				if(gstSwDevState.stRtkCPUPortStatus.s32CPUPortEnable)
				{
						/* Capture CPUtag format into gstCurrRtkEgressHdr */
						for(i=0;i<=u8TagLen;i++)
						{
								*pu8EgressPktHdr++ = pu8PktHdr[i];
						}
						//Dump_RTK83XX_CPUPortPktHdr(&gstCurrRtkEgressHsr);

						/* For project FTTH, get port no into BUF_BUFFER_FLAG2 */
						#ifdef _Vendor_TF_FTTH
						//BUF_BUFFER_FLAG2((char *)pstSkb) = RTK_LOG_PORT_MAP[gstCurrRtkEgressHdr.u32PortNo];
						//dprintf("[%s:%d] gstCurrRtkEgressHdr.u32PortNo = %d, BUF_BUFFER_FLAG2 = %d\n", __FUNCTION__, __LINE__, gstCurrRtkEgressHdr.u32PortNo, BUF_BUFFER_FLAG2((char *)pstSkb));
						#endif

				#endif // SWITCH_REALTEK_RTL83XX_FAMILY		

						// General Code ============================================ // 
						pu8Src = (char *)pstSkb->data;
				    pu8Dest = (char *)(pstSkb->data + u8TagLen);
				    		
						/* Shift */
						for(i=(u32Shift-1); i>=0; i--)
						{
								pu8Dest[i] = pu8Src[i];
						}
						
						skb_pull(pstSkb,u8TagLen);

						return SWHAL_ERR_SUCCESS;
						// General Code ============================================ // 

				#ifdef SWITCH_REALTEK_RTL83XX_FAMILY
				}
				#endif
		}
		
		return SWHAL_ERR_FAILED;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   Switch_Hal_UtilTool_RMON_Get  			
*
*   DESCRIPTION:
*   VR9 API only
*	
*	
* 	RETURN:

*
*   NOTE:
*
*****************************************************************/
void Switch_Hal_UtilTool_RMON_Get(void)
{
#if 0
		char cmd_str[300/*MAX_CMD_STR_LEN*/];  // for MAX_CMD_STR_LEN = 300

		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		BLOCK_PREEMPTION;
		#endif

		dprintf("\r\n        Show RMON_Get 0 ========================================\r\n");
		sprintf(cmd_str, "RMON_Get 0");
		switch_utility(cmd_str);

		dprintf("\r\n        Show RMON_Get 1 ========================================\r\n");
		sprintf(cmd_str, "RMON_Get 1");
		switch_utility(cmd_str);
		//sysUDelay(1*1000*1000);		//sleep 1

		dprintf("\r\n        Show RMON_Get 2 ========================================\r\n");
		sprintf(cmd_str, "RMON_Get 2");
		switch_utility(cmd_str);

		dprintf("\r\n        Show RMON_Get 3 ========================================\r\n");
		sprintf(cmd_str, "RMON_Get 3");
		switch_utility(cmd_str);

		dprintf("\r\n        Show RMON_Get 4 ========================================\r\n");
		sprintf(cmd_str, "RMON_Get 4");
		switch_utility(cmd_str);

		dprintf("\r\n        Show RMON_Get 5 ========================================\r\n");
		sprintf(cmd_str, "RMON_Get 5");
		switch_utility(cmd_str);

		dprintf("\r\n        Show RMON_Get 6 ========================================\r\n");
		sprintf(cmd_str, "RMON_Get 6");
		switch_utility(cmd_str);

		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		UNBLOCK_PREEMPTION;
		#endif
#endif		
		return;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   Switch_Hal_UtilTool_MulticastTableEntry_Read  			
*
*   DESCRIPTION:
*   VR9 API only
*	
*	
* 	RETURN:

*
*   NOTE:
*
*****************************************************************/
void Switch_Hal_UtilTool_MulticastTableEntry_Read(void)
{
#if 0
		char cmd_str[300/*MAX_CMD_STR_LEN*/];
		int tmp_ipnt_enable = ipnt_enable;
		
		dprintf("[%s:%d] ipnt_enable = %d\n", __FUNCTION__, __LINE__, ipnt_enable);

		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		BLOCK_PREEMPTION;
		#endif
		
		if(tmp_ipnt_enable==0)
			tmp_iprintf_onoff(1);

		sprintf(cmd_str, "MulticastTableEntryRead");
		switch_utility(cmd_str);

		if(tmp_ipnt_enable==0)
			tmp_iprintf_onoff(0);

		#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
		UNBLOCK_PREEMPTION;
		#endif
#endif
		return;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*
*   DESCRIPTION:
*   
*	
*	
* 	RETURN:
*
*   NOTE:
*
*****************************************************************/
int Switch_Hal_(void)
{
		//int s32RetVal;

		return SWHAL_ERR_SUCCESS;
}


/*****************************************************************
*	Test Program
*****************************************************************/
#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_SUPERTASK)
int TestProg_LinkUpStatus_Get(void)
{
		//unsigned short u16PhyNo, i;  //unused

		unsigned int u32PhyNo=0; 
		unsigned char u8LinkUp=0;
		
		int s32RetVal;
		unsigned int u32Cnt=1;

		while(1)
		{
				dprintf("%s[%s] Get Link-Up status %d times\n\n%s", SWCLR1_32_GREEN, __FUNCTION__, u32Cnt, SWCLR0_RESET);

				if(SWHAL_ERR_SUCCESS != (s32RetVal = Switch_Hal_LinkUpStatus_Get(&u32PhyNo, &u8LinkUp, 1)))
				{	
						dprintf("Error Getting Link-Up status, errno=0x%08x\n", s32RetVal);
						return s32RetVal;
				}

				/* Dump table */
				dprintf("[%s] Dump Link-Up status Information\n", __FUNCTION__);
				dprintf("[%s] u32PhyNo = 0x%x, u8LinkUp = %d\n", __FUNCTION__, u32PhyNo, u8LinkUp);
				dprintf("\n\n");
				
				//mdelay(10);
				/* Wait 1 sec */
				dlytsk(0, SW_DLY_TICKS/*DLY_TICKS*/, SW_MSECTOTICKS(1000));
				//dlytsk(0, SW_DLY_SECS, (1/100));

				u32Cnt++;

				if(u32Cnt == 2)
				{
						dprintf("%sGet Phy status finished!\n%s", SWCLR1_32_GREEN, SWCLR0_RESET); 
						break;
				}
		}
	

		return 0;
}
#endif



