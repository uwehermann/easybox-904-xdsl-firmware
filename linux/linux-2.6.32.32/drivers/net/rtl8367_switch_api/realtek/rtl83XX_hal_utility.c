/* ============================================================
Follow by 
This API package supports multiple Realtek Switch contains  ASIC  
drivers.  ASIC  drivers  provide  general  APIs  that based on 
user configuration to configure relative ASIC registers.


Programmer : Alvin Hsu, alvin_hsu@arcadyan.com
=============================================================== */
#include "rtl83XX_hal_utility.h"			/* local definitions */

#include "switch_vendor_dbg.h"				/* local definitions */
#include "switch_vendor_error.h"			/* local definitions */

#if(_RTK_RTL83XX_SWITCH_API == 1)
#include "switch_api/include/rtk_api_ext.h"
#include "switch_api/include/rtk_api.h"

#elif(_RTK_RTL83XX_SWITCH_API == 2)
#include "switch_api_v1_2_10/include/rtk_api_ext.h"
#include "switch_api_v1_2_10/include/rtk_api.h"
#endif

#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_LINUX)
#include <asm/string.h> 
#endif

/*****************************************************************
*	Extern
*****************************************************************/
#if((PROJECT_PLATFORM&OS_TYPE_MASK) == OS_TYPE_LINUX)
//extern unsigned long strtoul(const char *nptr, char **endptr, int base);
extern size_t strlen(const char *s);
#endif


/*****************************************************************
*	Definition
*****************************************************************/
/* RTK I/O Command */
char gs8RtkUtilCmdStr[RTK_MAX_CMD_STR_LEN];										/* RTK switch utility string */

unsigned long	gu64RtkUtilCmdArg[RTK_MAX_COMMAND_ARGS]={};     /* max. 64 command arguments (integer/long/string) */
char	gs8RtkUtilCmdArgStr[RTK_MAX_CMD_STR_LEN]; 							/* max 100 characters as string parameters */

RTK_SWIO_PARAMS gunIOParams;

/**
 * \brief Switch commands available for configuration
 */
struct _rtk_utility_command gstRtkUtilCommands[] =
{
	/* [0x00~0x0F]			Operation, Administration, and Management ========== */
	{ "RTK_SysInit", "l", RTK_UTILCMD_INIT,
    "<Return Params Addr.>",
    "\tSet chip to default configuration enviroment\n\n"
    "\tParams Addr.:              Params address or NULL for status\n"},
	{ "RTK_MaxPktLengthSet", "l", RTK_UTILCMD_MAXPKTLEN_SET,
    "<Packet Length>",
    "\tSet the max packet length for whole system setting, 1st configuration\n\n"
		"\tPacket Length:             1522, 1536, 1552, 16000\n"},
	{ "RTK_MaxPktLengthGet", "l", RTK_UTILCMD_MAXPKTLEN_GET,
    "<Return Params Addr.>",
		"\tGet the max packet length for whole system setting, 1st configuration\n\n"
		"\tParams Addr.:              Params address for packet length\n"},
	{ "RTK_PortMaxPktLengthSet", "ll", RTK_UTILCMD_PORTMAXPKTLEN_SET,
    "<Port ID> <Packet Length>",
    "\tSet the max packet length for certain port setting, 2st configuration\n\n"
		"\tPort ID:                   0~Max Lan Port No.\n"
		"\tPacket Length:             1522, 1536, 1552, 16000\n"},
	{ "RTK_PortMaxPktLengthGet", "ll", RTK_UTILCMD_PORTMAXPKTLEN_GET,
    "<Port ID> <Params Addr.>",
		"\tGet the max packet length for certain port setting, 2st configuration\n"
		"\tPort ID:                   0~Max Lan Port No.\n"
		"\tParams Addr.:              Params address for packet length\n"},
  { "RTK_CPUPortTagEnSet", "l", RTK_UTILCMD_CPUPORTTAGEN_SET,
		"\tSet cpu tag function enable/disable\n\n"
    "<Enable/Disable>",
    "\tEnable/Disable:            0: Disable 1: Enable\n"},
  { "RTK_CPUPortTagEnGet", "l", RTK_UTILCMD_CPUPORTTAGEN_GET,
		"\tGet cpu tag function enable/disable\n\n"
    "<Return Params Addr.>",
		"\tParams Addr.:              Params address\n"},
  { "RTK_CPUPortTagCfgSet", "ll", RTK_UTILCMD_CPUPORTTAG_SET,
		"\tSet CPU port and inserting proprietary CPU tag mode\n\n"
    "<Port ID> <CPU Port Mode>",
    "\tPort ID:                   0~Max Port No.\n"
    "\tCPU Port Mode:             0: CPU_INSERT_TO_ALL\n"
    "\t                           1: CPU_INSERT_TO_TRAPPING\n"
    "\t                           2: CPU_INSERT_TO_NONE\n"},
  { "RTK_CPUPortTagCfgGet", "ll", RTK_UTILCMD_CPUPORTTAG_GET,
		"\tGet CPU port and inserting proprietary CPU tag mode\n\n"
    "<Return Params Addr. for CPU Port No.> <Return Params Addr. for CPU Port Mode>",
		"\tParams Addr.:              Params address\n"
		"\tParams Addr.:              Params address\n"},
		
	/* [0x10~0x1F]			L2 LookUpTbl ======================================= */
	{ "RTK_L2IpMulticastTableInit", "l", RTK_UTILCMD_L2LUT_INIT,
    "<Return Params Addr.>",
    "\tInitialize l2 module of the specified device\n\n"
    "\tParams Addr.:   Params address or NULL for status\n"},
	{ "RTK_L2IpMulticastTableTypeSet", "l", RTK_UTILCMD_L2LUT_TYPE_SET,
    "<L2LUT Type>",
    "\tSet Lut IP multicast lookup function\n\n"
    "\tL2LUT Type:     0:LOOKUP_MAC\n"
    "\t                1:LOOKUP_SIP_DIP\n"
    "\t                2:LOOKUP_DIP(default)\n"},
	{ "RTK_L2IpMulticastTableTypeGet", "l", RTK_UTILCMD_L2LUT_TYPE_GET,
    "<Return Params Addr.>",
    "\tGet Lut IP multicast lookup function\n"
    "\tParams Addr.:   Params address for type\n"},
  { "RTK_L2IpMulticastTableEntryAdd", "lss", RTK_UTILCMD_L2LUT_IPMULTICASTADDR_ADD,
    "<PHY ID> <GDP> <GSP>",
    "\tAdd Lut IP multicast entry\n\n"
    "\tPHY ID:         PHY ID\n"
    "\tGDP:            Group Destination IP address (xxx.xxx.xxx.xxx)\n"
    "\tGSP:            Group Source IP address (xxx.xxx.xxx.xxx)\n"},
  { "RTK_L2IpMulticastTableEntryRemove", "lss", RTK_UTILCMD_L2LUT_IPMULTICASTADDR_DEL,
    "<GDP> <GSP>",
    "\tDelete a ip multicast address entry from the specified device\n\n"
    "\tPHY ID:         PHY ID\n"
    "\tGDP:            Group Destination IP address (xxx.xxx.xxx.xxx)\n"
    "\tGSP:            Group Source IP address (xxx.xxx.xxx.xxx)\n"},
	{ "RTK_L2IpMulticastTableEntryDump", "", RTK_ETHSW_L2LUT_IPMULTICASTADDR_DUMP,
    "",
    "\t\n"},

	/* [0x20~0x2F]			Port MAC/PHY Configuration ========================= */
  { "RTK_PortLinkCfgGet", "ll", RTK_UTILCMD_PORTLINKCFG_GET,
    "<Port Id> <Return Params Addr. for configuration>",
    "\tGet port linking configuration.\n\n"
    "\tPort Id:                   0~7: Port No.\n"
		"\tParams Addr.:              Params address for configuration, struct SWITCH_PORTMAC_STATUS\n"},
  { "RTK_PortLinkPHYCfgGet", "llll", RTK_UTILCMD_PORTLINKPHYCFG_GET,
    "<Port Id> <Return Params Addr. for PHY link status> <Return Params Addr. for PHY link speed> <Return Params Addr. for PHY duplex mode>",
    "\tPort Id:                   0~4: Lan Port No.\n"
    "\tGet port PHY status configuration.\n\n"
		"\tParams Addr.:              Params address for PHY link status\n"
		"\tParams Addr.:              Params address for PHY link speed\n"
		"\tParams Addr.:              Params address for PHY duplex mode\n"},
	{ "RTK_PortForceLinkExtCfgSet", "lllllllll", RTK_UTILCMD_PORTFORCELINKEXTCFG_SET,
    "<Ext. Port Id> <MII Mode> <Force Duplex/Speed/Link> <Speed> <Duplex> <Link> <Nway> <TX pause> <RX pause>",
		"\tSet port force linking configuration.\n\n"
		"\tExt. Port Id:              0~2: Extension Port No.\n"
    "\tMII Mode:                  0: DISABLE 1: RGMII 2: MII_MAC 3: MII_PHY 4: TMII_MAC 5: TMII_PHY 6: GMII 7: RMII_MAC 8: RMII_PHY 9: RGMII_33V\n"
    "\tForce Duplex/Speed/Link:   0: DISABLE 1: Force\n"
    "\tSpeed:                     10: 10Mb 100:100Mb 1000:1GMb\n"
    "\tDuplex:                    0: Half duplex  1:Full duplex\n"
    "\tLink:                      0: DOWN 1: UP\n"
		"\tNway:                      0: DISABLE 1: ENABLE\n"
		"\tTXPause:                   0: DISABLE 1: ENABLE\n"
		"\tRXPause:                   0: DISABLE 1: ENABLE\n"},
  { "RTK_PortForceLinkExtCfgGet", "lll", RTK_UTILCMD_PORTFORCELINKEXTCFG_GET,
    "<Ext. Port Id> <Return Params Addr. for MII Mode> <Return Params Addr. for configuration>",
		"\tGet port force linking configuration.\n\n"
		"\tPort Id:                   0~2: Extension Port No.\n"
		"\tParams Addr.:              Params address for MII Mode\n"    
		"\tParams Addr.:              Params address for configuration\n"},
  { "RTK_PortRGMIIClkExtSet", "lll", RTK_UTILCMD_PORTRGMIICLKEXT_SET,
    "<Ext. Port Id> <Tx delay> <Rx delay>",
    "\tSet port force linking configuration.\n\n"
    "\tExt. Port Id:              0~2: Extension Port No.\n"
    "\tTx delay:                  0: no-delay, 1: delay 2ns and so on..\n"
    "\tRx delay:                  0~7: delay setup.\n"},
  { "RTK_PortRGMIIClkExtGet", "lll", RTK_UTILCMD_PORTRGMIICLKEXT_GET,
    "<Ext. Port Id> <Return Params Addr. for Tx> <Return Params Addr. for Rx>",
    "\tGet RGMII interface delay value for TX and RX.\n\n"
    "\tPort Id:                   0~2: Extension Port No.\n"
		"\tParams Addr.:              Params address for Tx\n"    
		"\tParams Addr.:              Params address for Rx\n"},

	/* [0x40~0x4F]			CPU Port =========================================== */

	/* ===================================================================== */
	
#if 0
  { "CfgGet", "", IFX_ETHSW_CFG_GET,"",
    "\tRETURN:      Configure DATA\n" },
    // take off eMulticastTableAgeTimer
  { "CfgSet", "lllls", IFX_ETHSW_CFG_SET,
    //"<Enable> <Mac Table Aging Timer> <VLAN Aware> <Packet Length> <Pause Mac Mode> <Pause Mac Address>",
    "<Mac Table Aging Timer> <VLAN Aware> <Packet Length> <Pause Mac Mode> <Pause Mac Address>",
    //"\tEnable:                          0: Disable 1:Enable\n"
    //"\t                                 Only support on WAIT_INIT=1\n"
    "\tMac Table Aging timer:           1: 1 second\n"
    "\t                                 2: 10 seconds\n"
    "\t                                 3: 300 seconds\n"
    "\t                                 4: 1 hour\n"
    "\t                                 5: 1 day\n"
    "\tVLAN Aware:                      0: Disable\n"
    "\t                                 1: Enable\n"
    "\tPacket Length:                   Input Number\n"
    "\tPause Mac Mode:                  0: Disable\n"
    "\t                                 1: Enable\n"
    "\tPause Mac Address:               xx:xx:xx:xx:xx:xx \n"},
  { "PortCfgGet", "l", IFX_ETHSW_PORT_CFG_GET,
    "<Port Id>",
    "\tPortId:          0..6\n" },
  { "PortCfgSet", "lllllllllll", IFX_ETHSW_PORT_CFG_SET,
    "<PortId> <Enable> <Unicast unknown drop> <Multicast unknown drop>\
     <Reserved packet drop> <Broadcast packet drop> <Aging> <Learning Mac Port Lock>  \
     <Learning Limit> <Port Moniter> <Flow Control>",
    "\tportid:                    0..6\n"
    "\tenable:                    0 - Disable this port\n"
    "\t                           1 - Enable this port\n"
    "\tUnicast Unknown drop:      Enable :1  Disable :0\n"
    "\tMulticast Unknown drop:    Enable :1  Disable :0\n"
    "\tReserved Packet drop:      Enable :1  Disable :0\n"
    "\tBroadcast Packet drop:     Enable :1  Disable :0\n"
    "\tAging:                     Enable :1  Disable :0\n"
    "\tLearning Mac PortLock:     Enable :1  Disable :0\n"
	#if  defined(AR9) || defined(DANUBE) || defined(AMAZON_SE)
    "\tLearning Limit:            0..31, 0:Doesn't limit the number of addresses to be learned\n"
	#else
    "\tLearning Limit:            0: Disable\n"
    "\t                           255:Default\n"
    "\t                           or specify the value\n"
	#endif
    "\tPort Monitor :             0: Normal port usage\n"
    "\t                           1: Port Ingress packets are mirrored to the monitor port.\n"
    "\t                           2: Port Egress packets are mirrored to the monitor port.\n"
    "\t                           3: Port Ingress and Egress packets are mirrored to the monitor port.\n"
    "\tFlow Control :             0: Auto 1: Flow RX 2: Flow TX 3:Flow RXTX 4: FLOW OFF\n"
    },
  { "PortRedirectGet", "l", IFX_ETHSW_PORT_REDIRECT_GET,
    "<Port Id>",
    "\tPort Id:                   \n"},
  { "PortRedirectSet", "lll", IFX_ETHSW_PORT_REDIRECT_SET,
    "<Port Id> <Redirect Egress> <Redirect Ingress>",
    "\tPort Id:                   0..2 (internal switch)\n"
    "\tRedirect Egress:             Enable :1  Disable :0\n"
    "\tRedirect Ingress:            Enable :1  Disable :0\n"},
  { "MAC_TableEntryRead", "", IFX_ETHSW_MAC_TABLE_ENTRY_READ,
    "",
    "\t\n"},
  { "MAC_TableEntryAdd", "lllls", IFX_ETHSW_MAC_TABLE_ENTRY_ADD,
    "<DataBase ID> <Port Id> <Age Timer> <Static Entry> <Mac Address>",
    "\tDataBase ID:                FID number\n"
    "\tPort Id:                    0..6\n"
    "\tAge Timer:                  Value (From 1s to 1,000,000s)\n"
    "\tStatic Entry:               Enable :1  Disable :0\n"
    "\tMac Address:                xx:xx:xx:xx:xx:xx\n"},
  { "MAC_TableEntryRemove", "ls", IFX_ETHSW_MAC_TABLE_ENTRY_REMOVE,
    "<DataBase ID> <Mac Address>",
    "\tDataBase ID:                FID number\n"
    "\tMac Address:                xx:xx:xx:xx:xx:xx\n"},
  { "MAC_TableClear", "", IFX_ETHSW_MAC_TABLE_CLEAR,
    "",
    "\tClear all MAC Table list\n" },
  { "VLAN_IdCreate", "ll", IFX_ETHSW_VLAN_ID_CREATE,
    "<VLAN ID> <FID>",
    "\tVLAN ID:             0..4095\n"
    "\tFID:                 FID number\n"},
  { "VLAN_IdDelete", "l", IFX_ETHSW_VLAN_ID_DELETE,
    "<VLAN ID>",
    "\tVLAN ID:             0..4095\n"},
  { "VLAN_IdGet", "l", IFX_ETHSW_VLAN_ID_GET,
    "<VLAN ID>",
    "\tVLAN ID:             0..4095\n"
    "\tReturn FID:                 FID number\n"},
  { "VLAN_PortMemberAdd", "lll", IFX_ETHSW_VLAN_PORT_MEMBER_ADD,
    "<VLAN ID> <Port ID> <Tag base Number Egress>",
      "\tVLAN ID:                    0..4095\n"
    "\tPort ID:                    0..6\n"
    "\tbVlanTagEgress:             1 - Enable 0 - Disable\n"},
    { "VLAN_PortMapTableRead", "", IFX_ETHSW_VLAN_PORT_MEMBER_READ,
    "",
    "\t\n"},
  { "VLAN_PortMemberRemove", "ll", IFX_ETHSW_VLAN_PORT_MEMBER_REMOVE,
    "<VLAN ID> <Port ID>",
    "\tVLAN ID:                    ID number\n"
    "\tPort ID:                    0..6\n"},
  { "VLAN_PortCfgGet", "l", IFX_ETHSW_VLAN_PORT_CFG_GET,
    "<Port ID>",
    "\tPort ID:                    0..6\n"},
  { "VLAN_PortCfgSet", "lllllll", IFX_ETHSW_VLAN_PORT_CFG_SET,
    "<Port ID> <Port VID> <VLAN Unknow Drop> <VLAN ReAssign> <Violation Mode> <Admit Mode> <TVM>",
    "\tPort ID:                    0..6\n"
    "\tPort VID:                   VID number\n"
    "\tVLAN Unknow Drop            Enable:1  Disable:0\n"
    "\tVLAN ReAssign:              Enable:1  Disable:0\n"
    "\tViolation mode:             NA:0 INGRESS: 1 EGRESS:2 BOTH(Ingress&Egress):3\n"
    "\tAdmit Mode:                 Admit All:0\n"
    "\t                            Admit UnTagged:1\n"
    "\t                            Admit Tagged:2\n"
    "\tTVM:                        Enable:1  Disable:0\n"},
  { "VLAN_ReservedAdd", "l", IFX_ETHSW_VLAN_RESERVED_ADD,
    "<nVId>",
	#if defined(AR9) || defined(DANUBE) || defined(AMAZON_SE)
    "\tnVId:                      0: VID=0\n"
    "\t                           1: VID=1\n"
    "\t                           0xFFF: VID=FFF\n"},
	#else
    "\tVLAN ID:             0..4095\n"},
	#endif
  { "VLAN_ReservedRemove", "l", IFX_ETHSW_VLAN_RESERVED_REMOVE,
	#if defined(AR9) || defined(DANUBE) || defined(AMAZON_SE)
    "<nVId>",
    "\tnVId:                      0: VID=0\n"
    "\t                           1: VID=1\n"
    "\t                           0xFFF: VID=FFF\n"},
	#else
    "\tVLAN ID:             0..4095\n"},
  #endif
  { "MulticastRouterPortAdd", "l", IFX_ETHSW_MULTICAST_ROUTER_PORT_ADD,
    "<Port ID>",
    "\tPort ID:                    Port number\n"},
  { "MulticastRouterPortRemove", "l", IFX_ETHSW_MULTICAST_ROUTER_PORT_REMOVE,
    "<Port ID>",
    "\tPort ID:                    Port number\n"},
  { "MulticastRouterPortRead", "", IFX_ETHSW_MULTICAST_ROUTER_PORT_READ,
    "",
    "\tReturn the Router Port Menber (HEX value).\n"},
  { "MulticastSnoopCfgGet", "", IFX_ETHSW_MULTICAST_SNOOP_CFG_GET,
    "",
    "\tReturn the info data        .\n"},
  { "MulticastSnoopCfgSet", "lllllllllll", IFX_ETHSW_MULTICAST_SNOOP_CFG_SET,
    "<IGMP Snooping> <IGMPv3> <cross-VLAN packets> <Port Forward> <Forward Port ID> <ClassOfService> <Robust>\
     <Query interval> <Report Suppression> <FastLeave> <LearningRouter>",
    "\tIGMP Snooping:              0:Disable 1:Auto Learning 2:Snoop Forward\n"
    "\tIGMPv3 Support:             0:Disable 1:Enable\n"
    "\tcross-VLAN packets:         0:Disable 1:Enable\n"
    "\tPort Forward:               0:Default 1:Discard 2:CPU port 3:Forward Port\n"
    "\tForwardPortId               Port ID \n"
    "\tClassOfService              0 - 254 s :Default(10s) \n"
    "\tRobust:                     0..3\n"
    "\tQuery interval:             Hex value from 100ms to 25.5s\n"
    "\tJoin&Report:                0:Both 1:Report Suppression only 2:Transparent Mode\n"
    "\tFastLeave:                  0:Disable 1:Enable\n"
    "\tLearningRouter              0:Disable 1:Enable\n"},
  { "RMON_Clear", "l",  IFX_ETHSW_RMON_CLEAR,
    "<Port Id>",
    "\tPort Id:                   0 ..6\n"},
  { "RMON_Get", "l",  IFX_ETHSW_RMON_GET,
    "<Port Id>",
    "\tPort Id:                   0 ..6\n"},
  { "MDIO_DataRead", "ll", IFX_ETHSW_MDIO_DATA_READ,
    "<PHY addr> <Register inside PHY>",
    "\tphy addr:           0..14\n"
    "\tregister:           0..24\n"},
  { "MDIO_DataWrite", "lll", IFX_ETHSW_MDIO_DATA_WRITE,
    "<PHY addr> <Register inside PHY> <DATA>",
    "\tPHY Addr:           0..4\n"
    "\tRegister:           0..24\n"
    "\tDATA:               value\n"},
	#if  defined(AR9) || defined(DANUBE) || defined(AMAZON_SE)
  { "MDIO_CfgGet", "", IFX_ETHSW_MDIO_CFG_GET,
    "",
    "<RETURN MDIO SPEED>\n"},
  { "MDIO_CfgSet", "ll",  IFX_ETHSW_MDIO_CFG_SET,
    "<bMDIO_Enable> <nMDIO_Speed>",
    "\tbMDIO_Enable:          0: N/A \n"
    "\teMDIO_Speed:           0:6.25 MHZ\n"
    "\t                       1:2.5 MHZ\n"
    "\t                       2:1.0 MHZ\n"},
	#endif
	#ifdef VR9
  { "MDIO_CfgGet", "", IFX_ETHSW_MDIO_CFG_GET,
    "<RETURN MDIO ENABLE PORT>\n"},
  { "MDIO_CfgSet", "ll",  IFX_ETHSW_MDIO_CFG_SET,
    "<bMDIO_Enable> <bEnable_port_rate>",
    "\tbMDIO_Enable:          0: DISABLe 1:ENABLE \n"
    "\t                       0:25 MHZ\n"
    "\t                       1:12.5 MHZ\n"
    "\t                       2:6.25 MHZ\n"
    "\t                       3:4.167 MHZ\n"
    "\t                       4:3.125 MHZ\n"
    "\t                       5:2.5 MHZ\n"
    "\t                       6:2.083 MHZ\n"
    "\t                       7:1.786 MHZ\n"
    "\t                       8:1.563 MHZ\n"
    "\t                       9:1.389 MHZ\n"
    "\t                       10:1.25 MHZ\n"
    "\t                       11:1.136 MHZ\n"
    "\t                       12:1.042 MHZ\n"
    "\t                       13:0.962 MHZ\n"
    "\t                       14:0.893 MHZ\n"
    "\t                       15:0.833 MHZ\n"
    "\t                       255:97.6 MHZ\n"},
  { "RegisterGet", "l",  IFX_FLOW_REGISTER_GET,
    "<nRegAddr>",
    "\tnRegAddr:          \n" },
  { "RegisterSet", "ll",  IFX_FLOW_REGISTER_SET,
    "<nRegAddr> <nData>",
    "\tnRegAddr:          \n"
    "\tnData:             \n" },
  { "CPU_PortExtendCfgGet", "",  IFX_ETHSW_CPU_PORT_EXTEND_CFG_GET,
    "Return the port extend config info\n"},
  { "CPU_PortExtendCfgSet", "llsslllllll",  IFX_ETHSW_CPU_PORT_EXTEND_CFG_SET,
    "<eHeaderAdd> <bHeaderRemove> <sHeader nMAC_Src> <sHeader nMAC_Dst> <sHeader nEthertype> <sHeader nVLAN_Prio> <sHeader nVLAN_CFI> <sHeader nVLAN_ID> <ePauseCtrl> <bFcsRemove> <nWAN_Ports>",
    "\teHeaderAdd:          0: No Header 1: Ethernet Header 2:Ethernet and VLAN Header\n"
    "\tbHeaderRemove:       0: NA 1: Remove\n"
    "\tsHeader:             Header Data (nMAC_Src, nMAC_Dst, nEthertype, nVLAN_Prio, nVLAN_CFI, nVLAN_ID)\n"
    "\tePauseCtrl:          0: Forward 1: Dispach\n"
    "\tbFcsRemove:          0: NA 1: Remove FCS\n"
    "\tnWAN_Ports:          WAN Port Number\n"},
	#endif
  { "CPU_PortCfgGet", "l",  IFX_ETHSW_CPU_PORT_CFG_GET,
    "<Port Id>",
    "\tPort Id:                   0 .. 6\n"},
  { "CPU_PortCfgSet", "llllll", IFX_ETHSW_CPU_PORT_CFG_SET,
    "<Port Id> <bFcsCheck> <bFcsGenerate> <bSpecialTagEgress> <bSpecialTagIngress> <bCPU_PortValid>",
    "\tnPortId:             0 .. 6\n"
    "\tbFcsCheck:           0: No Check 1: Check FCS\n"
    "\tbFcsGenerate:        0: Without FCS 1: With FCS\n"
    "\tbSpecialTagEgress:   0: Disable 1: Enable\n"
    "\tbSpecialTagIngress:  0: Disable 1: Enable\n"
    "\tbCPU_PortValid:      0: Not Define CPU Port 1: Define CPU Port\n"},
  { "MonitorPortGet", "l", IFX_ETHSW_MONITOR_PORT_CFG_GET,
    "<Port Id>",
    "\tPort Id:                   0 ..6\n"},
  { "MonitorPortSet", "ll", IFX_ETHSW_MONITOR_PORT_CFG_SET,
    "<Port Id> <bMonitorPort>",
    "\tPort Id:                   0 ..6\n"
    "\tbMonitorPort:              0: False, 1: True\n" },
  { "PHY_AddrGet", "l", IFX_ETHSW_PORT_PHY_ADDR_GET,
    "<Port Id>",
	#if  defined(AR9)
    "\tPort Id:                   0..6\n"},
	#elif defined(DANUBE)
    "\tPort Id:                   0..4\n"},
	#elif defined(AMAZON_SE)
    "\tPort Id:                   0..3\n"},
	#else
    "\tPort Id:                   0..6\n"},
	#endif
  { "QOS_PortCfgGet", "l",  IFX_ETHSW_QOS_PORT_CFG_GET,
    "<nPortId>",
    "\tnPortId:          0..6\n"
    "Return the port QOS config info\n"},
  { "QOS_PortCfgSet", "lll",  IFX_ETHSW_QOS_PORT_CFG_SET,
    "<nPortId> <eClassMode> <nTrafficClass>",
    "\tnPortId:          0..6\n"
    "\teClassMode:       \n"
    "\t                  0: No traffic class assignment based on DSCP or PCP\n"
    "\t                  1: Traffic class assignment based on DSCP\n"
    "\t                  2: Traffic class assignment based on PCP\n"
	#if  defined(AR9) || defined(DANUBE) || defined(AMAZON_SE)
    "\t                  3: Traffic class assignment based on DSCP and PCP,with DSCP has higher precedence then PCP\n"
    "\t                  4: Traffic class assignment based on PCP and DSCP,with PCP has higher precedence then DSCP\n"
	#else
    "\t                  3: Traffic class assignment based on DSCP and PCP\n"
    "\t                  4: PCP and DSCP Untag\n"
	#endif
	#if  defined(AR9) || defined(DANUBE) || defined(AMAZON_SE)
    "\tnTrafficClass:    Port Priority: 0 ~ 3\n"},
	#else
    "\tnTrafficClass:    Port Priority: 0 ~ F\n"},
	#endif
  { "QOS_DscpClassGet", "",  IFX_ETHSW_QOS_DSCP_CLASS_GET,
    "<RETURN QOS DSCP Traffic Class, Total:64 entry>\n"},
  { "QOS_DscpClassSet", "ll",  IFX_ETHSW_QOS_DSCP_CLASS_SET,
    "<Index> <nTrafficClass data> ",
    "\tIndex:                 0..63\n"
	#ifdef VR9
    "\tnTrafficClass:         Hex Data: 0..15\n"},
	#else
    "\tValue:                     0..3\n" },
	#endif
  { "QOS_PcpClassGet", "",  IFX_ETHSW_QOS_PCP_CLASS_GET,
   "<RETURN QOS PCP Traffic Class, Total:8 entry>\n"},
  { "QOS_PcpClassSet", "ll",  IFX_ETHSW_QOS_PCP_CLASS_SET,
    "<Index> <nTrafficClass data> ",
    "\tIndex:                 0..7\n"
	#ifdef VR9
    "\tnTrafficClass:         Hex Data : 0 .. 15\n"},
	#else
    "\tValue:                     0..3\n" },
	#endif
	#if defined(AR9) || defined(DANUBE) || defined(AMAZON_SE)
	#if 0
  { "QOS_PortShaperGet", "lll",  IFX_PSB6970_QOS_PORT_SHAPER_GET,
    "<nPort> <nTrafficClass> <eType>",
    "\tnPort:                     0..6\n"
    "\tnTrafficClass:             0..3\n"
    "\teType:                     0: Strict Priority\n"
    "\t                           1: Weighted Fair Queuing\n"},
  { "QOS_PortShaperSet", "llll",  IFX_PSB6970_QOS_PORT_SHAPER_SET,
    "<nPort> <nTrafficClass> <eType> <nRate>",
    "\tnPort:                     0..6\n"
    "\tnTrafficClass:             0..3\n"
    "\teType:                     0: Strict Priority\n"
    "\t                           1: Weighted Fair Queuing\n"
    "\tValue:                     N  MBit/s\n" },
	#else
  { "QOS_PortShaperCfgGet", "l",  IFX_PSB6970_QOS_PORT_SHAPER_CFG_GET,
    "<nPort> ",
    "\tnPort:                     0..6\n"},
  { "QOS_PortShaperCfgSet", "ll",  IFX_PSB6970_QOS_PORT_SHAPER_CFG_SET,
    "<nPort> <eWFQ_Type>",
    "\tnPort:                     0..6\n"
    "\teWFQ_Type:                 0: Weight\n"
    "\t                           1: Rate\n"},
  { "QOS_PortShaperStrictGet", "ll",  IFX_PSB6970_QOS_PORT_SHAPER_STRICT_GET,
    "<nPort> <nTrafficClass>",
    "\tnPort:                     0..6\n"
    "\tnTrafficClass:             0..3\n"},
  { "QOS_PortShaperStrictSet", "lll",  IFX_PSB6970_QOS_PORT_SHAPER_STRICT_SET,
    "<nPort> <nTrafficClass> <nRate>",
    "\tnPort:                     0..6\n"
    "\tnTrafficClass:             0..3\n"
    "\tnRate:                     N  MBit/s\n" },
  { "QOS_PortShaperWFQGet", "ll",  IFX_PSB6970_QOS_PORT_SHAPER_WFQ_GET,
    "<nPort> <nTrafficClass>",
    "\tnPort:                     0..6\n"
    "\tnTrafficClass:             0..3\n"},
  { "QOS_PortShaperWFQSet", "lll",  IFX_PSB6970_QOS_PORT_SHAPER_WFQ_SET,
    "<nPort> <nTrafficClass> <nRate>",
    "\tnPort:                     0..6\n"
    "\tnTrafficClass:             0..3\n"
    "\tnRate:                     N in ratio or in MBit/s\n" },
	#endif
  { "QOS_PortPolicerGet", "l",  IFX_PSB6970_QOS_PORT_POLICER_GET,
    "<nPort>",
    "\tnPort:                     0..6\n"},
  { "QOS_PortPolicerSet", "ll",  IFX_PSB6970_QOS_PORT_POLICER_SET,
    "<nPort> <nRate>",
    "\tnPort:                     0..6\n"
    "\tValue:                     N  MBit/s\n" },
  { "QOS_MFC_PortCfgGet", "l", IFX_PSB6970_QOS_MFC_PORT_CFG_GET,
    "<nPort>",
    "\tnPortId:                 0..6\n"},
  { "QOS_MFC_PortCfgSet", "lll", IFX_PSB6970_QOS_MFC_PORT_CFG_SET,
    "<nPort> <bPriorityPort> <bPriorityEtherType>",
    "\tnPortId:                 0..6\n"
    "\tbPriorityPort:           0: Disable\n"
    "\t                         1: Enable\n"
    "\tbPriorityEtherType:      0: Disable\n"
    "\t                         1: Enable\n"},
  { "QOS_MFC_Add", "lllllllll", IFX_PSB6970_QOS_MFC_ADD,
    "<eFieldSelection> <nPortSrc> <nPortDst> <nPortSrcRange> \n"
    "                                   <nPortDstRange> <nProtocol> <nEtherType> <nTrafficClass>\n"
    "                                   <ePortForward>",
    "\teFieldSelection:         1:UDP/TCP Source Port Filter\n"
    "\t                         2:UDP/TCP Destination Port Filter\n"
    "\t                         4:IP Protocol Filter\n"
    "\t                         8:Ethertype Filter\n"
    "\tnPortSrc                 Source port base\n"
    "\tnPortDst                 Destination port base\n"
    "\tnPortSrcRange            Check from nPortSrc till smaller nPortSrc + nPortSrcRange\n"
    "\tnPortDstRange            Check from nPortDst till smaller nPortDst + nPortDstRange.\n"
    "\tnProtocol                Protocol type\n"
    "\tnEtherType               Ether type\n"
    "\tnTrafficClass            Egress priority queue\n"
    "\tePortForward:            0:Default\n"
    "\t                         1:Discard\n"
    "\t                         2:Forward to the CPU port\n"},
    //"\t                         3:Forward to a port\n"},
  { "QOS_MFC_Del", "lllllll", IFX_PSB6970_QOS_MFC_DEL,
    "<eFieldSelection> <nPortSrc> <nPortDst> <nPortSrcRange>\n"
    "                                   <nPortDstRange> <nProtocol> <nEtherType>",
    "\teFieldSelection:         1:UDP/TCP Source Port Filter\n"
    "\t                         2:UDP/TCP Destination Port Filter\n"
    "\t                         4:IP Protocol Filter\n"
    "\t                         8:Ethertype Filter\n"
    "\tnPortSrc                 Source port base\n"
    "\tnPortDst                 Destination port base\n"
    "\tnPortSrcRange            Check from nPortSrc till smaller nPortSrc + nPortSrcRange\n"
    "\tnPortDstRange            Check from nPortDst till smaller nPortDst + nPortDstRange.\n"
    "\tnProtocol                Protocol type\n"
    "\tnEtherType               Ether type\n"},
  { "QOS_MFC_EntryRead", "", IFX_PSB6970_QOS_MFC_ENTRY_READ,
    "",
    "\t\n"},
  { "QOS_StormGet", "", IFX_PSB6970_QOS_STORM_GET ,
    "",
    "\t\n"},
  { "QOS_StormSet", "lllll", IFX_PSB6970_QOS_STORM_SET,
    "<bBroadcast> <bMulticast> <bUnicast> <nThreshold10M> <nThreshold100M>",
    "\tbStormBroadcast:         0: Disable\n"
    "\t                         1: Enable\n"
    "\tbStormMulticast:         0: Disable\n"
    "\t                         1: Enable\n"
    "\tbStormUnicast:           0: Disable\n"
    "\t                         1: Enable\n"
    "\tnThreshold10M:           10M Threshold (the number of the packets received during 50 ms)\n"
    "\tnThreshold100M:          100M Threshold (the number of the packets received during 50 ms)\n"},
  { "STP_PortCfgGet", "l", IFX_ETHSW_STP_PORT_CFG_GET ,
    "<nPortId>",
    "\tnPortId:                   0..6\n"},
  { "STP_PortCfgSet", "ll", IFX_ETHSW_STP_PORT_CFG_SET,
    "<nPortId> <ePortState>",
    "\tnPortId:                   0..6\n"
    "\tePortState:                0: Forwarding state\n"
    "\t                           1: Disabled/Discarding state\n"
    "\t                           2: Learning state\n"
    "\t                           3: Blocking/Listening state\n" },
  { "STP_BPDU_RULE_Get", "", IFX_ETHSW_STP_BPDU_RULE_GET ,
    "",
    "\t\n" },
  { "STP_BPDU_RULE_Set", "l", IFX_ETHSW_STP_BPDU_RULE_SET,
    "<eForwardPort>",
    "\teForwardPort:              0: Default portmap\n"
    "\t                           1: Discard\n"
    "\t                           2: Forward to cpu port\n"},
  { "8021X_PortCfgGet", "l", IFX_ETHSW_8021X_PORT_CFG_GET ,
    "<nPortId>",
    "\tnPortId:                 0..6\n"},
  { "8021X_PortCfgSet", "ll", IFX_ETHSW_8021X_PORT_CFG_SET,
    "<nPortId> <eState>",
    "\tnPortId:                 0..6\n"
    "\teState:                  0: Receive and transmit direction are authorized.\n"
    "\t                         1: Receive and transmit direction are unauthorized.\n"},
  { "RegisterGet", "l",  IFX_PSB6970_REGISTER_GET,
    "<nRegAddr>",
    "\tnRegAddr:          \n" },
  { "RegisterSet", "ll",  IFX_PSB6970_REGISTER_SET,
    "<nRegAddr> <nData>",
    "\tnRegAddr:          \n"
    "\tnData:             \n" },
  { "VersionGet", "l",  IFX_ETHSW_VERSION_GET,
    "<nId>",
    "\tnId:                      0:Switch API Version\n"
    "\t                            \n" },
  { "CapGet", "", IFX_ETHSW_CAP_GET,
    "<nCapType>",
    "\tnCapType                  0: physical Ethernet ports.\n"
    "\t                          1: virtual Ethernet ports\n"
    "\t                          2: internal packet memory [in Bytes]\n"
    "\t                          3: priority queues per device\n"
    "\t                          4: meter instances\n"
    "\t                          5: rate shaper instances\n"
    "\t                          6: VLAN groups\n"
    "\t                          7: Forwarding database IDs\n"
    "\t                          8: MAC table entries\n"
    "\t                          9: multicast level 3 hardware table entries\n"
    "\t                          10: PPPoE sessions\n" },
  { "PHY_Query", "l", IFX_ETHSW_PORT_PHY_QUERY,
    "<Port Id>",
    "\tPort Id:                   0 ..6\n"},
  { "RGMII_CfgGet", "l", IFX_ETHSW_PORT_RGMII_CLK_CFG_GET,
    "<Port Id>",
    "\tPort Id:                   0 ..6\n"},
  { "RGMII_CfgSet", "lll", IFX_ETHSW_PORT_RGMII_CLK_CFG_SET,
    "<Port Id> <nDelayRx> <nDelayTx>",
    "\tPort Id:                   0 ..1\n"
    "\tnDelayRx:                  0: 0ns, 6: 1.5ns, 7: 1.75ns 8: 2ns \n"
    "\tnDelayTx:                  0: 0ns, 6: 1.5ns, 7: 1.75ns 8: 2ns \n" },
  { "Reset", "l",  IFX_PSB6970_RESET,
    "<eReset>",
    "\teReset:                  0: Reset Phy\n"},
  { "HW_Init", "l",  IFX_ETHSW_HW_INIT,
    "<eInitMode>",
    "\teInitMode:               0: Basic hardware configuration\n"},
	#if 1
  { "PowerManagementGet", "", IFX_PSB6970_POWER_MANAGEMENT_GET,
    "\tRETURN:              Enable/Disable\n"},
  { "PowerManagementSet", "l", IFX_PSB6970_POWER_MANAGEMENT_SET,
      "<Enable/Disable>",
    "\tEnable/Disable:             Enable :1  Disable :0\n"},
	#endif
	#endif
/****************************************************************************/
/* API list which is only for VR9 */
/****************************************************************************/
	#ifdef VR9
  { "Reset", "",  IFX_FLOW_RESET,
    "\tRETURN:                  0: Switch Reset\n"},
  { "HW_Init", "",  IFX_ETHSW_HW_INIT,
    "\tRETURN:              Switch HW INIT\n"},
	{  "PCE_RuleRead", "l", IFX_FLOW_PCE_RULE_READ ,
		"<nIndex>",
		"\tnIndex:              Index \n" },
		{  "PCE_RuleWrite", "llllllllllllsllsllllllllllsllsllllllllllllllllllllllllllllllll", IFX_FLOW_PCE_RULE_WRITE ,
		"<nIndex> <Pattern parameters> <Action Parameters>",
		"\tnIndex :                  Index\n"
		"\t <Pattern Table Parameters>\n"
		"\tbEnable:                  Table Enable\n"
		"\tbPortIdEnable:            Port Enable\n"
		"\tnPortId:                  Port ID\n"
		"\tbDSCP_Enable:             Enable DSCP\n"
		"\tnDSCP:                    DSCP Value\n"
		"\tbPCP_Enable:              Enable PCP\n"
		"\tnPCP:                     PCP Value\n"
		"\tbPktLngEnable:            Enable Packet Length\n"
		"\tnPktLng                   Packet Length\n"
		"\tnPktLngRange:             Packet Length Range\n"
		"\tbMAC_DstEnable:           Enable Dst MAC Address\n"
		"\tnMAC_Dst[6]:              Dst MAC Address (xx:xx:xx:xx:xx:xx)\n"
		"\tnMAC_DstMask:             Dst MAC Address Mask\n"
		"\tbMAC_SrcEnable:           Enable Src MAC Address\n"
		"\tnMAC_Src[6]:              Src MAC Address (xx:xx:xx:xx:xx:xx)\n"
		"\tnMAC_SrcMask:             Src MAC Address Mask\n"
		"\tbAppDataMSB_Enable:       Enable Application Data MSB\n"
		"\tnAppDataMSB:              MSB Application Data\n"
		"\tbAppMaskRangeMSB_Select:  Enable MSB Application Data Mask\n"
		"\tnAppMaskRangeMSB:         MSB Application Data Mask\n"
		"\tbAppDataLSB_Enable:       Enable Application Data LSB\n"
		"\tnAppDataLSB:              LSB Application Data\n"
		"\tbAppMaskRangeLSB_Select:  Select LSB Application Data Mask\n"
		"\tnAppMaskRangeLSB:         LSB Application Data Mask\n"
		"\teDstIP_Select:            Select Dst IP Address\n"
		"\tnDstIP:                   Dst IP Address (xxx.xxx.xxx.xxx)\n"
		"\tnDstIP_MsbMask:           Dst IP Address Mask\n"
		"\teSrcIP_Select:            Select Src IP Address\n"
		"\tnSrcIP:                   Src IP Address (xxx.xxx.xxx.xxx)\n"
		"\tnSrcIP_MsbMask:           Src IP Address Mask\n"
		"\tbEtherTypeEnable:         Enable Ethernet Type\n"
		"\tnEtherType:               Ethernet Type Data\n"
		"\tnEtherTypeMask:           Ethernet Type Mask\n"
		"\tbProtocolEnable:          Enable Protocol\n"
		"\tnProtocol:                Protocol Data\n"
		"\tnProtocolMask:            Protocol Data Mask\n"
		"\tbSessionIdEnable:         Enable Session ID\n"
		"\tnSessionId:               Session ID Data\n"
		"\tbVid:                     Enable VLAN ID\n"
		"\tnVid:                     VLAN ID Data\n\n"
		"\t <Action Table Parameters>\n"
		"\teTrafficClassAction:      Action Traffic Class\n"
		"\tnTrafficClassAlternate:   Traffic Class Alternate\n"
		"\teSnoopingTypeAction:      Action Snooping Type\n"
		"\teLearningAction:          Action Learning\n"
		"\teIrqAction:               Action Irq\n"
		"\teCrossStateAction:        Action Cross State\n"
		"\teCritFrameAction:         Action Critical Frame\n"
		"\teTimestampAction:         Action Time Stamp\n"
		"\tePortMapAction:           Action Port Map\n"
		"\tnForwardPortMap:          Action Port Forwarding\n"
		"\tbRemarkAction:            Action Remarking\n"
		"\tbRemarkPCP:               Enable Remarking PCP\n"
		"\tbRemarkDSCP:              Enable Remarking DSCP\n"
		"\tbRemarkClass:             Enable Remarking Class\n"
		"\teMeterAction:             Action Metering\n"
		"\tnMeterId:                 Metering ID Data\n"
		"\tbRMON_Action:             Action RMON\n"
		"\tnRMON_Id:                 RMON ID Data\n"
		"\teVLAN_Action:             Action VLAN\n"
		"\tnVLAN_Id:                 VLAN ID Data\n"
		"\teVLAN_CrossAction:        Action Cross VLAN\n"},
		{  "PCE_RuleDelete", "l", IFX_FLOW_PCE_RULE_DELETE ,
		"<nIndex>",
		"\tnIndex:              Index \n" },
		{  "QOS_QueuePortGet", "ll", IFX_ETHSW_QOS_QUEUE_PORT_GET ,
		"<nPortId> <nTrafficClassId>",
		"\tnPortId:             Port ID\n"
		"\tnTrafficClassId:     Traffic Class index\n"},
		{  "QOS_QueuePortSet", "lll", IFX_ETHSW_QOS_QUEUE_PORT_SET ,
		"<nQueueId> <nPortId> <nTrafficClassId>",
		"\tnQueueId:            QoS queue index \n"
		"\tnPortId:             Port ID\n"
		"\tnTrafficClassId:     Traffic Class index\n"},
		{  "QOS_MeterCfgGet", "l", IFX_ETHSW_QOS_METER_CFG_GET ,
		"<nMeterId>",
		"\tnMeterId:            Meter index \n" },
		{  "QOS_MeterCfgSet", "lllll", IFX_ETHSW_QOS_METER_CFG_SET,
		"<bEnable> <nMeterId> <nCbs> <nEbs> <nRate>",
		"\tbEnable:             Enable/Disable the meter shaper\n"
		"\tnMeterId:            Meter index \n"
		"\tnCbs:                Committed Burst Size (CBS [bytes])\n"
		"\tnEbs:                Excess Burst Size (EBS [bytes])\n"
		"\tnRate:               Rate [Mbit/s]\n"},
		{  "QOS_MeterPortGet", "", IFX_ETHSW_QOS_METER_PORT_GET ,
		"",
		"\t\n" },
		{  "QOS_MeterPortAssign", "llll", IFX_ETHSW_QOS_METER_PORT_ASSIGN,
		"<nMeterId> <eDir> <nPortIngressId> <nPortEgressId>",
		"\tnMeterId:               Meter index\n"
		"\teDir:                   Port assignment 0:None 1:Ingress 2:Egress 3:Both\n"
		"\tnPortIngressId:         Ingress Port Id\n"
		"\tnPortEgressId:          Egress Port Id\n"},
		{  "QOS_MeterPortDeassign", "llll", IFX_ETHSW_QOS_METER_PORT_DEASSIGN,
		"<nMeterId> <eDir> <nPortIngressId> <nPortEgressId>",
		"\tnMeterId:               Meter index\n"
		"\teDir:                   Port assignment 0:None 1:Ingress 2:Egress 3:Both\n"
		"\tnPortIngressId:         Ingress Port Id\n"
		"\tnPortEgressId:          Egress Port Id\n"},
		{  "QOS_WredCfgGet", "", IFX_ETHSW_QOS_WRED_CFG_GET ,
		"",
		"\t\n" },
		{  "QOS_WredCfgSet", "lllllll", IFX_ETHSW_QOS_WRED_CFG_SET,
		"<eProfile> <nRed_Min> <nRed_Max> <nYellow_Min> <nYellow_Max> <nGreen_Min> <nGreen_Max>",
		"\teProfile:             Drop Probability Profile\n"
		"\t                        0:Pmin = 25%, Pmax = 75% (default)\n"
		"\t                        1:Pmin = 25%, Pmax = 50%\n"
		"\t                        2:Pmin = 50%, Pmax = 50%\n"
		"\t                        3:Pmin = 50%, Pmax = 75%\n"
		"\tnRed_Min:             WRED Red Threshold Min [8Byte segments / s]\n"
		"\tnRed_Max:             WRED Red Threshold Max [8Byte segments / s]\n"
		"\tnYellow_Min:          WRED Yellow Threshold Min [8Byte segments / s]\n"
		"\tnYellow_Max:          WRED Yellow Threshold Max [8Byte segments / s]\n"
		"\tnGreen_Min:           WRED Green Threshold Min [8Byte segments / s]\n"
		"\tnGreen_Max:           WRED Green Threshold Max [8Byte segments / s]\n"},
	 #if 1 // Should be remove later.
   {  "QOS_WredQueueCfgGet", "l", IFX_ETHSW_QOS_WRED_QUEUE_CFG_GET ,
      "<nQueueId>",
      " /* \tQoS queue index */ \n" },
   {  /*"QOS_WredQueueCfgSet", "llllllll", IFX_ETHSW_QOS_WRED_QUEUE_CFG_SET,*/
   	"QOS_WredQueueCfgSet", "lllllll", IFX_ETHSW_QOS_WRED_QUEUE_CFG_SET,
      "<nQueueId> <nRed_Min> <nRed_Max> <nYellow_Min> <nYellow_Max> <nGreen_Min> <nGreen_Max>",
      "\tnQueueId:             QoS queue index\n"
/*      "\teProfile:             Drop Probability Profile\n"
      "\t                        0:Pmin = 25%, Pmax = 75% (default)\n"
      "\t                        1:Pmin = 25%, Pmax = 50%\n"
      "\t                        2:Pmin = 50%, Pmax = 50%\n"
      "\t                        3:Pmin = 50%, Pmax = 75%\n" */
      "\tnRed_Min:             WRED Red Threshold Min [segments / Queue]\n"
      "\tnRed_Max:             WRED Red Threshold Max [segments / Queue]\n"
      "\tnYellow_Min:          WRED Yellow Threshold Min [segments / Queue]\n"
      "\tnYellow_Max:          WRED Yellow Threshold Max [segments / Queue]\n"
      "\tnGreen_Min:           WRED Green Threshold Min [segments / Queue]\n"
      "\tnGreen_Max:           WRED Green Threshold Max [segments / Queue]\n"},
	 #endif
   {  "QOS_SchedulerCfgGet", "l", IFX_ETHSW_QOS_SCHEDULER_CFG_GET ,
      "<nQueueId>",
      "\tQoS queue index \n" },
   {  "QOS_SchedulerCfgSet", "lll", IFX_ETHSW_QOS_SCHEDULER_CFG_SET,
      "<nQueueId> <eType> <nWeight>",
      "\tnQueueId:             QoS queue index \n"
      "\teType:                Scheduler Type  0:Strict Priority 1:Weighted Fair Queuing\n"
      "\tnWeight:              Ratio\n"},
   {  "QOS_StormAdd", "llll", IFX_ETHSW_QOS_STORM_CFG_SET ,
      "<nMeterId> <bBroadcast> <bMulticast> <bUnknownUnicast>",
      "\tnMeterId:                 Meter index\n"
      "\tbBroadcast:               1: Enable 0: Disable\n"
      "\tbMulticast:               1: Enable 0: Disable\n"
      "\tbUnknownUnicast:          1: Enable 0: Disable\n"},
   {  "QOS_StormGet", "", IFX_ETHSW_QOS_STORM_CFG_GET ,
      "",
      "\tReturn the STORM Configure\n"},
   {  "QOS_ShaperCfgGet", "l", IFX_ETHSW_QOS_SHAPER_CFG_GET ,
      "<nRateShaperId>",
      "\tRate Shapper ID\n" },
   {  "QOS_ShaperCfgSet", "llll", IFX_ETHSW_QOS_SHAPER_CFG_SET,
      "<nRateShaperId> <bEnable> <nCbs> <nRate> ",
      "\tnRateShaperId:             Rate Shapper ID\n"
      "\tbEnable:                   1: Enable  0:Discard\n"
      "\tnCbs:                      Committed Burst Size\n"
      "\tnRate:                     Rate [Mbit/s]\n"},
   {  "QOS_ShaperQueueAssign", "ll", IFX_ETHSW_QOS_SHAPER_QUEUE_ASSIGN ,
      "<nRateShaperId> <nQueueId>",
      "\tnRateShaperId:             Rate shaper index\n"
      "\tnQueueId:                  QoS queue index\n"},
   {  "QOS_ShaperQueueDeassign", "ll", IFX_ETHSW_QOS_SHAPER_QUEUE_DEASSIGN,
      "<nRateShaperId> <nQueueId>",
      "\tnRateShaperId:             Rate shaper index\n"
      "\tnQueueId:                  QoS queue index\n"},
   //  { "PCE_TablePrint", "l", 0xF9,
   //    "<TableIndex>",
   //    "\tTableIndex:                PCE Table Index\n"},
   {  "QOS_ClassPCPGet", "",  IFX_ETHSW_QOS_CLASS_PCP_GET,
      "<RETURN the Class to PCP mapping table, Total: 16 entry>\n"},
   {  "QOS_ClassPCPSet", "ll",  IFX_ETHSW_QOS_CLASS_PCP_SET,
      "<Index> <nTrafficClass data> ",
      "\tIndex:                 0..15\n"
      "\tnPCP:                  Hex Data : 0 .. 7\n"},
   {  "QOS_ClassDSCPGet", "",  IFX_ETHSW_QOS_CLASS_DSCP_GET,
      "<RETURN the Class to DSCP mapping table, Total: 16 entry>\n"},
   {  "QOS_ClassDSCPSet", "ll",  IFX_ETHSW_QOS_CLASS_DSCP_SET,
      "<Index> <nTrafficClass data> ",
      "\tIndex:                 0..15\n"
      "\tnDSCP:                  Hex Data : 0 .. 63\n"},
   {  "QOS_PortRemarkingCfgGet", "l",  IFX_ETHSW_QOS_PORT_REMARKING_CFG_GET,
      "<nPortId>",
      "\tnPortId:               0..6\n"
      "<RETURN the Remarking information>\n"},
   {  "QOS_PortRemarkingCfgSet", "lllll",  IFX_ETHSW_QOS_PORT_REMARKING_CFG_SET,
      "<nPortId> <eDSCP_IngressRemarkingEnable> <bDSCP_EgressRemarkingEnable> <bPCP_IngressRemarkingEnable> <bPCP_EgressRemarkingEnable>",
      "\tnPortId:               0..6\n"
      "\teDSCP_IngressRemarkingEnable:            0: Disable 1:TC6 2:TC3 3:DP3 4:DP3_TC3\n"
      "\tbDSCP_EgressRemarkingEnable:             0: Disable 1:Enable\n"
      "\tbPCP_IngressRemarkingEnable:             0: Disable 1:Enable\n"
      "\tbPCP_EgressRemarkingEnable:              0: Disable 1:Enable\n"},
   {  "STP_PortCfgGet", "l", IFX_ETHSW_STP_PORT_CFG_GET ,
      "<nPortId>",
      "\tnPortId:                   0..6\n"},
   {  "STP_PortCfgSet", "ll", IFX_ETHSW_STP_PORT_CFG_SET,
      "<nPortId> <ePortState>",
      "\tnPortId:                   0..6\n"
      "\tePortState:                0: Forwarding state\n"
      "\t                           1: Disabled/Discarding state\n"
      "\t                           2: Learning state\n"
      "\t                           3: Blocking/Listening state\n" },
   {  "STP_BPDU_RULE_Get", "", IFX_ETHSW_STP_BPDU_RULE_GET ,
      "",
      "\t\n" },
   {  "STP_BPDU_RULE_Set", "ll", IFX_ETHSW_STP_BPDU_RULE_SET,
      "<eForwardPort> <nForwardPortId>",
      "\teForwardPort:              0: Default portmap\n"
      "\t                           1: Discard\n"
      "\t                           2: Forward to cpu port\n"
      "\t                           3: Forward to a port\n"
      "\tnForwardPortId:            PortID\n"},
   {  "8021X_PortCfgGet", "l", IFX_ETHSW_8021X_PORT_CFG_GET ,
      "<nPortId>",
      "\tnPortId:                 0..6\n"},
   {  "8021X_PortCfgSet", "ll", IFX_ETHSW_8021X_PORT_CFG_SET,
      "<nPortId> <eState>",
      "\tnPortId:                 0..6\n"
      "\teState:                  0: port state authorized.\n"
      "\t                         1: port state unauthorized.\n"
      "\t                         2: RX authorized.\n"
      "\t                         3: TX authorized.\n"},
   {  "PHY_Query", "l", IFX_ETHSW_PORT_PHY_QUERY,
      "<Port Id>",
      "\tPort Id:                   0 ..6\n"},
	 #endif
   { "RMON_ExtendGet", "l",  IFX_FLOW_RMON_EXTEND_GET,
    "<Port Id>",
    "\tPort Id:                   0 ..6\n"},
   {  "8021X_EAPOL_RuleGet", "", IFX_ETHSW_8021X_EAPOL_RULE_GET,
      "",
      "\t\n"},
   {  "8021X_EAPOL_RuleSet", "ll", IFX_ETHSW_8021X_EAPOL_RULE_SET,
      "<eForwardPort> <nForwardPortId>",
      "\teForwardPort:            0: Default\n"
      "\t                         1: Discard\n"
      "\t                         2: Forward to the CPU port\n"
      "\t                         3: Forward to a port\n"
      "\tnForwardPortId:          port\n"},
   { "CapGet", "", IFX_ETHSW_CAP_GET,
      "<nCapType>",
      "\tnCapType                  0: physical Ethernet ports.\n"
      "\t                          1: virtual Ethernet ports\n"
      "\t                          2: internal packet memory [in Bytes]\n"
      "\t                          3: Buffer segment size\n"
      "\t                          4: priority queues per device\n"
      "\t                          5: meter instances\n"
      "\t                          6: rate shaper instances\n"
      "\t                          7: VLAN groups\n"
      "\t                          8: Forwarding database IDs\n"
      "\t                          9: MAC table entries\n"
      "\t                          10: multicast level 3 hardware table entries\n"
      "\t                          11: PPPoE sessions\n"},
   {  "Enable", "",  IFX_ETHSW_ENABLE,
      "\t Enable the SWITCH \n"},
   {  "Disable", "",  IFX_ETHSW_DISABLE,
      "\t Disable the SWITCH \n"},
#endif
};


RTK_LowLevel_FKT rtk_ethsw_flow_fkt_ptr_tbl [] =
{
		/* Command: RTK_UTILCMD_INIT ; Index: 0x00 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command: RTK_UTILCMD_MAXPKTLEN_SET ; Index: 0x01 */
		(RTK_LowLevel_FKT) RTL83XX_Utility_MaxPktLengthSet,
		/* Command: RTK_UTILCMD_MAXPKTLEN_GET ; Index: 0x02 */
		(RTK_LowLevel_FKT) RTL83XX_Utility_MaxPktLengthGet,
		/* Command: RTK_UTILCMD_PORTMAXPKTLEN_SET ; Index: 0x03 */
		(RTK_LowLevel_FKT) RTL83XX_Utility_PortMaxPktLengthSet,
		/* Command: RTK_UTILCMD_PORTMAXPKTLEN_GET ; Index: 0x04 */
		(RTK_LowLevel_FKT) RTL83XX_Utility_PortMaxPktLengthGet,
		/* Command: RTK_UTILCMD_CPUPORTTAGEN_SET ; Index: 0x05 */
		(RTK_LowLevel_FKT) RTK_UTILCMD_SW_FKT( RTK_CONFIG_CPUTAG, RTL83XX_Utility_CPUPortTagEnSet),
		/* Command: RTK_UTILCMD_CPUPORTTAGEN_GET ; Index: 0x06 */
		(RTK_LowLevel_FKT) RTK_UTILCMD_SW_FKT( RTK_CONFIG_CPUTAG, RTL83XX_Utility_CPUPortTagEnGet),
		/* Command: RTK_UTILCMD_CPUPORTTAGEN_SET ; Index: 0x07 */
		(RTK_LowLevel_FKT) RTK_UTILCMD_SW_FKT( RTK_CONFIG_CPUTAG, RTL83XX_Utility_CPUPortTagCfgSet),
		/* Command: RTK_UTILCMD_CPUPORTTAGEN_GET ; Index: 0x08 */
		(RTK_LowLevel_FKT) RTK_UTILCMD_SW_FKT( RTK_CONFIG_CPUTAG, RTL83XX_Utility_CPUPortTagCfgGet),
		/* Command:  ; Index: 0x09 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x0A */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x0B */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x0C */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x0D */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x0E */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x0F */
		(RTK_LowLevel_FKT) ((void *)0),

		/* Command: RTK_UTILCMD_L2LUT_INIT; Index: 0x10 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command: RTK_UTILCMD_L2LUT_TYPE_SET ; Index: 0x11 */
		(RTK_LowLevel_FKT) RTK_UTILCMD_SW_FKT( RTK_CONFIG_L2LUT , RTL83XX_Utility_L2LUTTypeSet),
		/* Command: RTK_UTILCMD_L2LUT_TYPE_GET ; Index: 0x12 */
		(RTK_LowLevel_FKT) RTK_UTILCMD_SW_FKT( RTK_CONFIG_L2LUT , RTL83XX_Utility_L2LUTTypeGet),
		/* Command:  ; Index: 0x13 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x14 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command: RTK_UTILCMD_L2LUT_IPMULTICASTADDR_ADD ; Index: 0x15 */
		(RTK_LowLevel_FKT) RTK_UTILCMD_SW_FKT( RTK_CONFIG_L2LUT , RTL83XX_Utility_L2IpMulticastTableEntryAdd),
		/* Command: RTK_UTILCMD_L2LUT_IPMULTICASTADDR_DEL ; Index: 0x16 */
		(RTK_LowLevel_FKT) RTK_UTILCMD_SW_FKT( RTK_CONFIG_L2LUT , RTL83XX_Utility_L2IpMulticastTableEntryRemove),
		/* Command: RTK_L2IpMulticastTableEntryDump ; Index: 0x17 */
		(RTK_LowLevel_FKT) ((void *)0),//(RTK_LowLevel_FKT) ((void *)0),//(RTK_LowLevel_FKT) RTK_UTILCMD_SW_FKT( RTK_CONFIG_MULTICAST , RTL83XX_HalComm_IOFLOW_L2IpMulticastTableEntryDump),
		/* Command:  ; Index: 0x18 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x19 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x1A */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x1B */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x1C */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x1D */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x1E */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x1F */
		(RTK_LowLevel_FKT) ((void *)0),

		/* Command: RTK_UTILCMD_PORTLINKCFG_GET ; Index: 0x20 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command: RTK_UTILCMD_PORTLINKPHYCFG_GET ; Index: 0x21 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command: RTK_UTILCMD_PORTFORCELINKEXTCFG_SET ; Index: 0x22 */
		(RTK_LowLevel_FKT) RTL83XX_Utility_PortForceLinkExtCfgSet,		
		/* Command: RTK_UTILCMD_PORTFORCELINKEXTCFG_GET ; Index: 0x23 */
		(RTK_LowLevel_FKT) RTL83XX_Utility_PortForceLinkExtCfgGet,
		/* Command: RTK_UTILCMD_PORTRGMIICLKEXT_SET ; Index: 0x24 */
		(RTK_LowLevel_FKT) RTL83XX_Utility_PortRGMIIDelayExtSet,
		/* Command: RTK_UTILCMD_PORTRGMIICLKEXT_GET ; Index: 0x25 */
		(RTK_LowLevel_FKT) RTL83XX_Utility_PortRGMIIDelayExtGet,
		/* Command:  ; Index: 0x26 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x27 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x28 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x29 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x2A */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x2B */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x2C */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x2D */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x2E */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x2F */
		(RTK_LowLevel_FKT) ((void *)0),

		/* Command:  ; Index: 0x30 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x31 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x32 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x33 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x34 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x35 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x36 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x37 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x38 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x39 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x3A */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x3B */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x3C */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x3D */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x3E */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x3F */
		(RTK_LowLevel_FKT) ((void *)0),
		
		/* Command:  ; Index: 0x40 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x41 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x42 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x43 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x44 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x45 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x46 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x47 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x48 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x49 */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x4A */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x4B */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x4C */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x4D */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x4E */
		(RTK_LowLevel_FKT) ((void *)0),
		/* Command:  ; Index: 0x4F */
		(RTK_LowLevel_FKT) ((void *)0),
};


int gs32CmdIndex = -1;

int gs32NrCmds = sizeof(gstRtkUtilCommands)/sizeof(gstRtkUtilCommands[0]);

/*****************************************************************
*	Utility
*****************************************************************/
#define ISSPACE(c)  ((c) == ' ' || ((c) >= '\t' && (c) <= '\r'))
#define ISASCII(c)  (((c) & ~0x7f) == 0)
#define ISUPPER(c)  ((c) >= 'A' && (c) <= 'Z')
#define ISLOWER(c)  ((c) >= 'a' && (c) <= 'z')
#define ISALPHA(c)  (ISUPPER(c) || ISLOWER(c))
#define ISDIGIT(c)  ((c) >= '0' && (c) <= '9')

unsigned long Util_Strtoul(char* nstr, char** endptr, int base)
{
#if !(defined(__KERNEL__))
    //return strtoul (nstr, endptr, base);    /* user mode */

#else
    char* s = nstr;
    unsigned long acc;
    unsigned char c;
    unsigned long cutoff;
    int neg = 0, any, cutlim;

    do
    {
        c = *s++;
    } while (ISSPACE(c));

    if (c == '-')
    {
        neg = 1;
        c = *s++;
    }
    else if (c == '+')
        c = *s++;

    if ((base == 0 || base == 16) &&
        c == '0' && (*s == 'x' || *s == 'X'))
    {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;

    cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
    cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
    for (acc = 0, any = 0; ; c = *s++)
    {
        if (!ISASCII(c))
            break;
        if (ISDIGIT(c))
            c -= '0';
        else if (ISALPHA(c))
            c -= ISUPPER(c) ? 'A' - 10 : 'a' - 10;
        else
            break;

        if (c >= base)
            break;
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
            any = -1;
        else
        {
            any = 1;
            acc *= base;
            acc += c;
        }
    }

    if (any < 0)
    {
        acc = INT_MAX;
    }
    else if (neg)
        acc = -acc;
    if (endptr != 0)
        *((const char **)endptr) = any ? s - 1 : nstr;
    return (acc);
#endif
}

void Util_PrintCmd(void)
{
   int c;

   dprintf("\nRTK Switch Utility Version : %s\n", RTK_CHIP_API_VERSION);
   dprintf("Available commands:\n");
	 
   for(c=0; c<gs32NrCmds; c++)
   {
      dprintf("\t%s\n", gstRtkUtilCommands[c].s8Name);
   }
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   Util_CheckCmd
*
*   DESCRIPTION:
*   This API checks the command line strings for correctness
*		argv[1] has to be a valid command that can be converted to an 
*		switch ioctl
*
*		Return:
*		Position of the given command in the command array if command is
*		found in the command array. Else -1 to indicate failure
*
*   NOTE:
*
*
*****************************************************************/
int Util_CheckCmd(char *ps8Cmd)
{
   int c;

   for(c=0; c<gs32NrCmds; c++)
   {
      if(strcmp(ps8Cmd, gstRtkUtilCommands[c].s8Name) == 0)
      {
         /* yeah, we found our command */
         break;
      }
   }
	 
   if(c >= gs32NrCmds)
   {
      dprintf("Command %s not found!\n", ps8Cmd);
      Util_PrintCmd();
      return SWHAL_ERR_FAILED;
   }

   return c;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   Util_ConvertParams
*
*   DESCRIPTION:
*   This API converts the input parameters in the command
*		to the specified format and update the command array
*
*
*   NOTE:
*
*
*****************************************************************/
int Util_ConvertParams(char **ps8Params, int s32NrParams)
{
   const char *parg_string;
   int  cmd_arg_str_offset = 0;
   int  argc = 0;
   const char *arg;
   int param_pos;
   int slen;
   char *endptr;

   if(s32NrParams == 0)
   {
      	/* Nothing to convert -> return OK */
      	return SWHAL_ERR_SUCCESS;
   }

   /*
    * transfer the parameter strings to the cmd_arg array by integer conversion
    */
   parg_string = gstRtkUtilCommands[gs32CmdIndex].s8Param;
   param_pos=0;
   while((*parg_string) && (argc < RTK_MAX_COMMAND_ARGS) && (param_pos < s32NrParams))
   {
	      arg = ps8Params[param_pos];
	      slen = strlen(arg);
	      switch (*parg_string)
	      {
			      case 'i':
			      case 'l':
								gu64RtkUtilCmdArg[argc++] = Util_Strtoul(arg, &endptr, 0);
	         			//gu64RtkUtilCmdArg[argc++] = strtoul(arg, &endptr, 0);
	         			break;
								
			      case 's':
								memcpy(&gs8RtkUtilCmdArgStr[cmd_arg_str_offset], arg, slen);
								gs8RtkUtilCmdArgStr[cmd_arg_str_offset + slen] = '\0';
								gu64RtkUtilCmdArg[argc++] = (long)&gs8RtkUtilCmdArgStr[cmd_arg_str_offset];
								cmd_arg_str_offset += slen+1;
								break;
							 
			      default:
								dprintf("Invalid format string in module, please fix and re-compile\n");
								return SWHAL_ERR_FAILED;
	      }
				
	      param_pos++;
	      parg_string++;
   }

   return SWHAL_ERR_SUCCESS;
}


/*****************************************************************
*
*   PROCEDURE NAME:
*   Util_ConvertIpAddr_CharStr
*
*   DESCRIPTION:
*   This API converts the passed ip address string (192.168.100.1)
*		to an array of values (0xC0, 0xA8, 0x64, 0x01)
*
*		ip_adr_str IP Address in String format
*		ip_adr_ptr IP Address in array format
*
*
*   NOTE:
*		copy from switch_utility.c
*
*
*****************************************************************/
int Util_ConvertIpAddr_CharStr( char *ip_adr_str, unsigned int *ip_adr_ptr )
{
		char *str_ptr=ip_adr_str;
		char *endptr;
		int i, val=0;
		unsigned int data = 0;

		if(strlen(ip_adr_str) > (12+3))
		{
			  dprintf("Invalid length of IP address string!\n");
			  return SWHAL_ERR_FAILED;
		}

		for(i=3; i>=0; i--)
		{
				val = Util_Strtoul(str_ptr, &endptr, 10);
				//val = strtoul(str_ptr, &endptr, 10);
			  if (*endptr && (*endptr != '.') && (*endptr != '-'))
			        return SWHAL_ERR_FAILED;

			  //printf("String Value = %03d\n",val);
			  data |= (val << (i * 8));
			  //printf("String Value = %08x\n",data);
			  str_ptr = endptr+1;
		}
		
   	*(ip_adr_ptr) = data;

	 	return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*	This for IOCTL only
*****************************************************************/
/*****************************************************************
*
*   PROCEDURE NAME:
*   
*
*   DESCRIPTION:
*   This API build the ioctl command based on the input command and 
*		parameters
*
*		fd:
*		Switch device file descriptor
*
*		ioctl_command: 
*		IOCTL command
*
*		return IFX_SUCCESS on successful execution of cmd -1 on failure
*
*   NOTE:
*
*
*
*****************************************************************/
int RTL83XX_Utility_CmdBuildEx(unsigned int u32Cmd)
{
		int s32RetVal=SWHAL_ERR_SUCCESS;

	 	void *pCtlParams=NULL;

		RTK_LowLevel_FKT RtkllFkt;

		RTK_PORT_MAC_STATUS stPortMacStatus;
		rtk_l2_addr_table_t stL2Entry;
		rtk_portmask_t stRtkPortMask;

		unsigned int u32SIP, u32DIP; 
		unsigned int u32AddrIdx=0;

		switch(u32Cmd) 
		{
				/* Command : 0x00 */
				case RTK_UTILCMD_INIT:
						memset(&gunIOParams.u32PtrAddr, 0x00, sizeof(gunIOParams.u32PtrAddr));

						gunIOParams.u32PtrAddr 		= (unsigned int)gu64RtkUtilCmdArg[0];

						if(RT_ERR_OK != (s32RetVal = rtk_switch_init()))	
						{ 
								dprintf("Error RTK Init, errno = 0x%x\n", s32RetVal); 

								if(gunIOParams.u32PtrAddr != 0)
										*(unsigned int *)gunIOParams.u32PtrAddr = SWHAL_ERR_SYSINIT;
						}						

						if(gunIOParams.u32PtrAddr != 0)
								*(unsigned int *)gunIOParams.u32PtrAddr = SWHAL_ERR_SUCCESS;

						return (gunIOParams.u32PtrAddr == 0)? SWHAL_ERR_SUCCESS :*(unsigned int *)gunIOParams.u32PtrAddr;
						
						break;

				/* Command : 0x01 */
				case RTK_UTILCMD_MAXPKTLEN_SET:
						memset(&gunIOParams.u32PktLength, 0x00, sizeof(gunIOParams.u32PktLength));

						if((unsigned int)gu64RtkUtilCmdArg[0] == 1522)
								gunIOParams.u32PktLength = MAXPKTLEN_1522B;
						else if((unsigned int)gu64RtkUtilCmdArg[0] == 1536)
								gunIOParams.u32PktLength = MAXPKTLEN_1536B;
						else if((unsigned int)gu64RtkUtilCmdArg[0] == 1552)
								gunIOParams.u32PktLength = MAXPKTLEN_1552B;
						else if((unsigned int)gu64RtkUtilCmdArg[0] == 16000)
								gunIOParams.u32PktLength = MAXPKTLEN_16000B;
						else
								return SWHAL_ERR_PARAMS;

						dprintf("[%s] Max Pkt Length = 0x%x\n", __FUNCTION__, gunIOParams.u32PktLength);

						pCtlParams = (void *)gunIOParams.u32PktLength;

						break;

				/* Command : 0x02 */
				case RTK_UTILCMD_MAXPKTLEN_GET:
						memset(&gunIOParams.u32PtrAddr, 0x00, sizeof(gunIOParams.u32PtrAddr));

						gunIOParams.u32PtrAddr = (unsigned int)gu64RtkUtilCmdArg[0];
						if(gunIOParams.u32PtrAddr == 0)
						{
								return SWHAL_ERR_PARAMS;
						}
						
						pCtlParams = (void *)gunIOParams.u32PtrAddr;

						break;

				/* Command : 0x03 */
				case RTK_UTILCMD_PORTMAXPKTLEN_SET:
						memset(&gunIOParams.stPortMaxPktLength, 0x00, sizeof(gunIOParams.stPortMaxPktLength));

						gunIOParams.stPortMaxPktLength.u32PortNo 		= (unsigned int)gu64RtkUtilCmdArg[0];
						gunIOParams.stPortMaxPktLength.u32MaxPktLen = (unsigned int)gu64RtkUtilCmdArg[1];
						dprintf("[%s] Port No. = %d, Max Pkt Length = 0x%x\n", __FUNCTION__, gunIOParams.stPortMaxPktLength.u32PortNo, gunIOParams.stPortMaxPktLength.u32MaxPktLen);

						pCtlParams = (void *)&gunIOParams.stPortMaxPktLength;

						break;

				/* Command : 0x04 */
				case RTK_UTILCMD_PORTMAXPKTLEN_GET:
						memset(&gunIOParams.stPortMaxPktLength, 0x00, sizeof(gunIOParams.stPortMaxPktLength));

						gunIOParams.stPortMaxPktLength.u32PortNo 		= (unsigned int)gu64RtkUtilCmdArg[0];
						gunIOParams.stPortMaxPktLength.u32MaxPktLen = (unsigned int)gu64RtkUtilCmdArg[1];
						dprintf("[%s] Port No. = %d, addr = 0x%x\n", __FUNCTION__, gunIOParams.stPortMaxPktLength.u32PortNo, gunIOParams.stPortMaxPktLength.u32MaxPktLen);
						if(gunIOParams.stPortMaxPktLength.u32MaxPktLen == 0)
						{
								return SWHAL_ERR_PARAMS;
						}
						
						pCtlParams = (void *)&gunIOParams.stPortMaxPktLength;

						break;

				/* Command : 0x05 */
				case RTK_UTILCMD_CPUPORTTAGEN_SET:
						memset(&gunIOParams.u32Set, 0x00, sizeof(gunIOParams.u32Set));

						gunIOParams.u32Set = (unsigned int)gu64RtkUtilCmdArg[0];

						pCtlParams = (void *)gunIOParams.u32Set;

						break;

				/* Command : 0x06 */
				case RTK_UTILCMD_CPUPORTTAGEN_GET:
						memset(&gunIOParams.u32PtrAddr, 0x00, sizeof(gunIOParams.u32PtrAddr));

						gunIOParams.u32PtrAddr = (unsigned int)gu64RtkUtilCmdArg[0];
						if(gunIOParams.u32PtrAddr == 0)
						{
								return SWHAL_ERR_PARAMS;
						}	

						pCtlParams = (void *)gunIOParams.u32Set;

						break;

				/* Command : 0x07 */
				case RTK_UTILCMD_CPUPORTTAG_SET:
						memset(&gunIOParams.stCpuPortTag, 0x00, sizeof(gunIOParams.stCpuPortTag));

						gunIOParams.stCpuPortTag.u32CpuPortNo 	= (unsigned int)gu64RtkUtilCmdArg[0];
						gunIOParams.stCpuPortTag.u32CpuPortMode = (unsigned int)gu64RtkUtilCmdArg[1];
						gunIOParams.stCpuPortTag.u32CpuPortMAsk	= (0x1 << (unsigned int)gu64RtkUtilCmdArg[0]);

						pCtlParams = (void *)&gunIOParams.stCpuPortTag;

						break;

				/* Command : 0x08 */
				case RTK_UTILCMD_CPUPORTTAG_GET:
						memset(&gunIOParams.stCpuPortTag, 0x00, sizeof(gunIOParams.stCpuPortTag));

						if((gu64RtkUtilCmdArg[0] != 0) && (gu64RtkUtilCmdArg[1] != 0))
						{	
								gunIOParams.stCpuPortTag.u32CpuPortNo 	= (unsigned int)gu64RtkUtilCmdArg[0];
								gunIOParams.stCpuPortTag.u32CpuPortMode = (unsigned int)gu64RtkUtilCmdArg[1];
						}
						else
						{
								return SWHAL_ERR_PARAMS;
						}

						pCtlParams = (void *)&gunIOParams.stCpuPortTag;

						break;


				/* Command : 0x10 */
				case RTK_UTILCMD_L2LUT_INIT:
						if(RTK_CONFIG_L2LUT)
						{
								memset(&gunIOParams.u32PtrAddr, 0x00, sizeof(gunIOParams.u32PtrAddr));

								gunIOParams.u32PtrAddr 		= (unsigned int)gu64RtkUtilCmdArg[0];

								if(RT_ERR_OK != (s32RetVal = rtk_l2_init()))	
								{ 
										dprintf("Error RTK L2LUT Init, errno = 0x%x\n", s32RetVal); 

										if(gunIOParams.u32PtrAddr != 0)
												*(unsigned int *)gunIOParams.u32PtrAddr = SWHAL_ERR_SYSINIT;

								}						

								if(gunIOParams.u32PtrAddr != 0)
										*(unsigned int *)gunIOParams.u32PtrAddr = SWHAL_ERR_SUCCESS;

								return (gunIOParams.u32PtrAddr == 0)? SWHAL_ERR_SUCCESS :*(unsigned int *)gunIOParams.u32PtrAddr;
						}
						else
						{
								return SWHAL_ERR_FAILED;
						}
						break;

				/* Command : 0x11 */
				case RTK_UTILCMD_L2LUT_TYPE_SET:
						memset(&gunIOParams.u32LookUpTblType, 0x00, sizeof(gunIOParams.u32LookUpTblType));

						gunIOParams.u32LookUpTblType = (unsigned int)gu64RtkUtilCmdArg[0];
						dprintf ("[%s] L2LUT Type = 0x%x\n", __FUNCTION__, gunIOParams.u32LookUpTblType);

						pCtlParams = (void *)gunIOParams.u32LookUpTblType;

						break;

				/* Command : 0x12 */
				case RTK_UTILCMD_L2LUT_TYPE_GET:
						memset(&gunIOParams.u32PtrAddr, 0x00, sizeof(gunIOParams.u32PtrAddr));

						gunIOParams.u32PtrAddr = (unsigned int)gu64RtkUtilCmdArg[0];
						if(gunIOParams.u32PtrAddr == 0)
						{
								return SWHAL_ERR_PARAMS;
						}	
						
						pCtlParams = (void *)gunIOParams.u32PtrAddr;
						
						break;

				/* Command : 0x15 */
			 	case RTK_UTILCMD_L2LUT_IPMULTICASTADDR_ADD:
						memset(&gunIOParams.stL2LUTIpMcastTableAddrAdd, 0x00, sizeof(gunIOParams.stL2LUTIpMcastTableAddrAdd));
						memset(&stL2Entry,0,sizeof(rtk_l2_addr_table_t)); 

            if(Util_ConvertIpAddr_CharStr((char*)gu64RtkUtilCmdArg[1], &gunIOParams.stL2LUTIpMcastTableAddrAdd.u32DIP) != SWHAL_ERR_SUCCESS)
            		return SWHAL_ERR_PARAMS;
            if(Util_ConvertIpAddr_CharStr((char*)gu64RtkUtilCmdArg[2], &gunIOParams.stL2LUTIpMcastTableAddrAdd.u32SIP) != SWHAL_ERR_SUCCESS)
            		return SWHAL_ERR_PARAMS;

						dprintf("[%s] GDP = 0x%x, GSP = 0x%x\n", __FUNCTION__, gunIOParams.stL2LUTIpMcastTableAddrAdd.u32DIP, gunIOParams.stL2LUTIpMcastTableAddrAdd.u32SIP);


						while(1)
						{
								if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_next_get((rtk_uint32 *)&u32AddrIdx, (ipaddr_t *)&u32SIP, (ipaddr_t *)&u32DIP, (rtk_portmask_t *)&stRtkPortMask)))
								{
										//dprintf("[%s] u32AddrIdx = 0x%x, u32DIP = 0x%x, u32SIP = 0x%x, stRtkPortMask = 0x%x\n", __FUNCTION__, u32AddrIdx, u32DIP, u32SIP, stRtkPortMask.bits[0]);
										//dprintf("Get L2 LUT next entry, errno = 0x%x\n", s32RetVal); 
										//break;

										/* find last entry */
										if(u32AddrIdx == RTK_MAX_LUT_ADDR_ID)
										{
												gunIOParams.stL2LUTIpMcastTableAddrAdd.u32PortMask = (unsigned int)(0x1<<gu64RtkUtilCmdArg[0]);
												dprintf("[%s] no had been added, u32PortMask = 0x%x\n", __FUNCTION__, gunIOParams.stL2LUTIpMcastTableAddrAdd.u32PortMask);
										}
								}
								else
								{
										dprintf("[%s] u32AddrIdx = 0x%x, u32DIP = 0x%x, u32SIP = 0x%x, stRtkPortMask = 0x%x\n", __FUNCTION__, u32AddrIdx, u32DIP, u32SIP, stRtkPortMask.bits[0]);

										/* GDP had been added! */
										if(gunIOParams.stL2LUTIpMcastTableAddrAdd.u32DIP == u32DIP)
										{
												dprintf("[%s] GDP had been added! new add mask = 0x%x, org = 0x%x\n", __FUNCTION__, (unsigned int)(0x1<<gu64RtkUtilCmdArg[0]), stRtkPortMask.bits[0]);
												if((unsigned int)(0x1<<gu64RtkUtilCmdArg[0]) & stRtkPortMask.bits[0])
												{
														dprintf("Port-mask had been added!\n");
														return SWHAL_ERR_SUCCESS;
												}
												else
												{
														gunIOParams.stL2LUTIpMcastTableAddrAdd.u32PortMask = (((unsigned int)(0x1<<gu64RtkUtilCmdArg[0])) | stRtkPortMask.bits[0]);
														dprintf("[%s] New mask = 0x%x\n", __FUNCTION__, gunIOParams.stL2LUTIpMcastTableAddrAdd.u32PortMask);
														break;
												}
										}
								}

								if(u32AddrIdx >= RTK_MAX_LUT_ADDR_ID)	break;

								u32AddrIdx++; 
						}

						#if 1
						dprintf ("[%s] GDP = %d.%d.%d.%d, GSP = %d.%d.%d.%d, u32PortMask = 0x%x\n", __FUNCTION__,
						#if 1
										(gunIOParams.stL2LUTIpMcastTableAddrAdd.u32DIP >> 24) & 0xFF,
          					(gunIOParams.stL2LUTIpMcastTableAddrAdd.u32DIP >> 16) & 0xFF,
          					(gunIOParams.stL2LUTIpMcastTableAddrAdd.u32DIP >> 8) & 0xFF,
          					 gunIOParams.stL2LUTIpMcastTableAddrAdd.u32DIP & 0xFF,
          					(gunIOParams.stL2LUTIpMcastTableAddrAdd.u32SIP >> 24) & 0xFF,
          					(gunIOParams.stL2LUTIpMcastTableAddrAdd.u32SIP >> 16) & 0xFF,
          					(gunIOParams.stL2LUTIpMcastTableAddrAdd.u32SIP >> 8) & 0xFF,
          					 gunIOParams.stL2LUTIpMcastTableAddrAdd.u32SIP & 0xFF,
						#elif 0
               			 x.stL2LUTIpMcastTableAddrAdd.u32DIP & 0xFF,
              			(x.stL2LUTIpMcastTableAddrAdd.u32DIP >> 8) & 0xFF,
              			(x.stL2LUTIpMcastTableAddrAdd.u32DIP >> 16) & 0xFF,
              			(x.stL2LUTIpMcastTableAddrAdd.u32DIP >> 24) & 0xFF,
               			 x.stL2LUTIpMcastTableAddrAdd.u32SIP & 0xFF,
              			(x.stL2LUTIpMcastTableAddrAdd.u32SIP >> 8) & 0xFF,
              			(x.stL2LUTIpMcastTableAddrAdd.u32SIP >> 16) & 0xFF,
              			(x.stL2LUTIpMcastTableAddrAdd.u32SIP >> 24) & 0xFF,
						#endif
										 gunIOParams.stL2LUTIpMcastTableAddrAdd.u32PortMask);
						#endif
						pCtlParams = (void *)&gunIOParams.stL2LUTIpMcastTableAddrAdd;

          	break;

				/* Command : 0x16 */		
       	case RTK_UTILCMD_L2LUT_IPMULTICASTADDR_DEL:
						memset(&gunIOParams.stL2LUTIpMcastTableAddrRemove, 0x00, sizeof(gunIOParams.stL2LUTIpMcastTableAddrRemove));
						memset(&stL2Entry,0,sizeof(rtk_l2_addr_table_t)); 

            if(Util_ConvertIpAddr_CharStr((char*)gu64RtkUtilCmdArg[1], &gunIOParams.stL2LUTIpMcastTableAddrRemove.u32DIP) != SWHAL_ERR_SUCCESS)
            		return SWHAL_ERR_PARAMS;
            if(Util_ConvertIpAddr_CharStr((char*)gu64RtkUtilCmdArg[2], &gunIOParams.stL2LUTIpMcastTableAddrRemove.u32SIP) != SWHAL_ERR_SUCCESS)
            		return SWHAL_ERR_PARAMS;

						while(1)
						{
								if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_next_get((rtk_uint32 *)&u32AddrIdx, (ipaddr_t *)&u32SIP, (ipaddr_t *)&u32DIP, (rtk_portmask_t *)&stRtkPortMask)))
								{
										//dprintf("[%s] u32AddrIdx = 0x%x, u32DIP = 0x%x, u32SIP = 0x%x, stRtkPortMask = 0x%x\n", __FUNCTION__, u32AddrIdx, u32DIP, u32SIP, stRtkPortMask.bits[0]);
										//dprintf("Get L2 LUT next entry, errno = 0x%x\n", s32RetVal); 

										/* find last entry */
										if(u32AddrIdx == RTK_MAX_LUT_ADDR_ID)
										{
												dprintf("[%s] no had been added\n", __FUNCTION__);
												return SWHAL_ERR_PARAMS;
										}
								}
								else
								{
										dprintf("[%s] u32AddrIdx = 0x%x, u32DIP = 0x%x, u32SIP = 0x%x, stRtkPortMask = 0x%x\n", __FUNCTION__, u32AddrIdx, u32DIP, u32SIP, stRtkPortMask.bits[0]);

										/* GDP had been added! */
										if(gunIOParams.stL2LUTIpMcastTableAddrRemove.u32DIP == u32DIP)
										{
												dprintf("[%s] GDP had been added! new del mask = 0x%x, org = 0x%x\n", __FUNCTION__, (unsigned int)(0x1<<gu64RtkUtilCmdArg[0]), stRtkPortMask.bits[0]);
												if((unsigned int)(0x1<<gu64RtkUtilCmdArg[0]) & stRtkPortMask.bits[0])
												{
														stRtkPortMask.bits[0] &= ~((unsigned int)(0x1<<gu64RtkUtilCmdArg[0]));
														gunIOParams.stL2LUTIpMcastTableAddrRemove.u32PortMask = stRtkPortMask.bits[0];
														dprintf("[%s] New mask = 0x%x\n", __FUNCTION__, gunIOParams.stL2LUTIpMcastTableAddrRemove.u32PortMask);

														if(gunIOParams.stL2LUTIpMcastTableAddrRemove.u32PortMask == 0)
																break;
														else
														{
																/* change port mask, not del */
																dprintf("[%s] change port mask, not del\n", __FUNCTION__);
																pCtlParams = (void *)&gunIOParams.stL2LUTIpMcastTableAddrRemove;
																RTL83XX_Utility_L2IpMulticastTableEntryAdd(pCtlParams);
														}
												}
												else
												{
														dprintf("Port-mask had been no added!\n");
														return SWHAL_ERR_PARAMS;
												}
										}
								}

								if(u32AddrIdx >= RTK_MAX_LUT_ADDR_ID)	break;

								u32AddrIdx++; 
						}


						dprintf ("[%s] Dest. IP: %d.%d.%d.%d, Src. IP: %d.%d.%d.%d, u32PortMask = 0x%x\n", __FUNCTION__,
						#if 1
										(gunIOParams.stL2LUTIpMcastTableAddrRemove.u32DIP >> 24) & 0xFF,
          					(gunIOParams.stL2LUTIpMcastTableAddrRemove.u32DIP >> 16) & 0xFF,
          					(gunIOParams.stL2LUTIpMcastTableAddrRemove.u32DIP >> 8) & 0xFF,
          					 gunIOParams.stL2LUTIpMcastTableAddrRemove.u32DIP & 0xFF,
          					(gunIOParams.stL2LUTIpMcastTableAddrRemove.u32SIP >> 24) & 0xFF,
          					(gunIOParams.stL2LUTIpMcastTableAddrRemove.u32SIP >> 16) & 0xFF,
          					(gunIOParams.stL2LUTIpMcastTableAddrRemove.u32SIP >> 8) & 0xFF,
          					 gunIOParams.stL2LUTIpMcastTableAddrRemove.u32SIP & 0xFF,
						#elif 0
               			 gunIOParams.stL2LUTIpMcastTableAddrRemove.u32DIP & 0xFF,
              			(gunIOParams.stL2LUTIpMcastTableAddrRemove.u32DIP >> 8) & 0xFF,
              			(gunIOParams.stL2LUTIpMcastTableAddrRemove.u32DIP >> 16) & 0xFF,
              			(gunIOParams.stL2LUTIpMcastTableAddrRemove.u32DIP >> 24) & 0xFF,
               			 gunIOParams.stL2LUTIpMcastTableAddrRemove.u32SIP & 0xFF,
              			(gunIOParams.stL2LUTIpMcastTableAddrRemove.u32SIP >> 8) & 0xFF,
              			(gunIOParams.stL2LUTIpMcastTableAddrRemove.u32SIP >> 16) & 0xFF,
              			(gunIOParams.stL2LUTIpMcastTableAddrRemove.u32SIP >> 24) & 0xFF,
						#endif						
										 gunIOParams.stL2LUTIpMcastTableAddrRemove.u32PortMask);

						pCtlParams = (void *)&gunIOParams.stL2LUTIpMcastTableAddrRemove;

          	break;

#if 0
				/* Command : 0x17 */
				case RTK_ETHSW_L2LUT_IPMULTICASTADDR_DUMP:
						//memset(&x.L2LUT_IpMcastTableAddrDump, 0x00, sizeof(x.L2LUT_IpMcastTableAddrDump));

						pCtlParams = 0;//(void *)&x.L2LUT_IpMcastTableAddrDump;

						
						return RTL83XX_HalComm_L2LUT_IpMcastTable_Dump();
						
						break;
#endif


				/* Command : 0x20 */
				case RTK_UTILCMD_PORTLINKCFG_GET:
						if((unsigned int)gu64RtkUtilCmdArg[0] > RTK_PORT_MAX)	return SWHAL_ERR_PARAMS;

						if(gu64RtkUtilCmdArg[1] != 0)
						{
								if(RT_ERR_OK !=	(s32RetVal = rtk_port_macStatus_get((rtk_port_t)gu64RtkUtilCmdArg[0], (rtk_port_mac_ability_t *)gu64RtkUtilCmdArg[1])))
								{
										dprintf("Get port link status failed! error=0x%08x\n", s32RetVal);
										if(s32RetVal == RT_ERR_PORT_ID)	return SWHAL_ERR_PARAMS;
										else return SWHAL_ERR_FAILED;
								}
						}
						else
						{
								return SWHAL_ERR_FAILED;
						}

						break;

				/* Command : 0x21 */
				case RTK_UTILCMD_PORTLINKPHYCFG_GET:
						if((unsigned int)gu64RtkUtilCmdArg[0] > RTK_PORT_LAN_NUM)	return SWHAL_ERR_PARAMS;

						if((gu64RtkUtilCmdArg[1] != 0) && (gu64RtkUtilCmdArg[2] != 0) && (gu64RtkUtilCmdArg[3] != 0))
						{
								if(RT_ERR_OK !=	(s32RetVal = rtk_port_phyStatus_get((rtk_port_t)gu64RtkUtilCmdArg[0], (rtk_port_linkStatus_t *)gu64RtkUtilCmdArg[1], (rtk_data_t*)gu64RtkUtilCmdArg[2], (rtk_data_t*)gu64RtkUtilCmdArg[3])))
								{
										dprintf("Get ethernet PHY linking status failed! error=0x%08x\n", s32RetVal);
										if(s32RetVal == RT_ERR_PORT_ID)	
												return SWHAL_ERR_PARAMS;
										else 
												return SWHAL_ERR_FAILED;
								}
						}
						else
						{
								return SWHAL_ERR_PARAMS;
						}


						break;
						
				/* Command : 0x22 */
				case RTK_UTILCMD_PORTFORCELINKEXTCFG_SET:
						memset(&gunIOParams.stPortForceLinkExt, 0x00, sizeof(gunIOParams.stPortForceLinkExt));
						
						gunIOParams.stPortForceLinkExt.pstMacStatus = &stPortMacStatus;
							
						gunIOParams.stPortForceLinkExt.u32ExtPortNo = gu64RtkUtilCmdArg[0];
						gunIOParams.stPortForceLinkExt.u32Mode			= gu64RtkUtilCmdArg[1];

						if(gunIOParams.stPortForceLinkExt.u32Mode == MODE_EXT_DISABLE)
						{
								/* Set all params to 0 */
								memset(&stPortMacStatus, 0x00, sizeof(rtk_port_mac_ability_t));
						}
						else if((gunIOParams.stPortForceLinkExt.u32Mode > MODE_EXT_DISABLE) &&\
							     	(gunIOParams.stPortForceLinkExt.u32Mode < MODE_EXT_END))
						{
								gunIOParams.stPortForceLinkExt.pstMacStatus->u32Forcemode = gu64RtkUtilCmdArg[2];
								if(gu64RtkUtilCmdArg[3] == 10)
									gunIOParams.stPortForceLinkExt.pstMacStatus->u32Speed = PORT_SPEED_10M;
								else if(gu64RtkUtilCmdArg[3] == 100)
									gunIOParams.stPortForceLinkExt.pstMacStatus->u32Speed = PORT_SPEED_100M;
								else if(gu64RtkUtilCmdArg[3] == 1000)
									gunIOParams.stPortForceLinkExt.pstMacStatus->u32Speed = PORT_SPEED_1000M;
								else 
									return SWHAL_ERR_PARAMS;
								gunIOParams.stPortForceLinkExt.pstMacStatus->u32Duplex 	= gu64RtkUtilCmdArg[4];					
								gunIOParams.stPortForceLinkExt.pstMacStatus->u32Link		= gu64RtkUtilCmdArg[5];
								gunIOParams.stPortForceLinkExt.pstMacStatus->u32Nway		= gu64RtkUtilCmdArg[6];
								gunIOParams.stPortForceLinkExt.pstMacStatus->u32Txpause	= gu64RtkUtilCmdArg[7];
								gunIOParams.stPortForceLinkExt.pstMacStatus->u32Rxpause	= gu64RtkUtilCmdArg[8];
						}
						else
						{
								dprintf("[%s] error mode = %d\n", __FUNCTION__, gunIOParams.stPortForceLinkExt.u32Mode);
								return SWHAL_ERR_PARAMS;
						}
						
						pCtlParams = (void *)&gunIOParams.stPortForceLinkExt;

						break;

				/* Command : 0x23 */
				case RTK_UTILCMD_PORTFORCELINKEXTCFG_GET:
						memset(&gunIOParams.stPortForceLinkExt, 0x00, sizeof(gunIOParams.stPortForceLinkExt));

						if((gu64RtkUtilCmdArg[1] != 0) && (gu64RtkUtilCmdArg[2] != 0))
						{
								gunIOParams.stPortForceLinkExt.u32ExtPortNo = gu64RtkUtilCmdArg[0];
								gunIOParams.stPortForceLinkExt.u32Mode			= gu64RtkUtilCmdArg[1];
								gunIOParams.stPortForceLinkExt.pstMacStatus	= gu64RtkUtilCmdArg[2];
						}
						else
						{
								dprintf("[%s] gu64RtkUtilCmdArg[1] = 0x%lx, gu64RtkUtilCmdArg[1] = 0x%lx, gu64RtkUtilCmdArg[1] = 0x%lx\n", __FUNCTION__, gu64RtkUtilCmdArg[0], gu64RtkUtilCmdArg[1], gu64RtkUtilCmdArg[2]);
								return SWHAL_ERR_PARAMS;
						}

						pCtlParams = (void *)&gunIOParams.stPortForceLinkExt;

						break;

				/* Command : 0x24 */
				case RTK_UTILCMD_PORTRGMIICLKEXT_SET:
						memset(&gunIOParams.stPortRGMIIDealyExt, 0x00, sizeof(gunIOParams.stPortRGMIIDealyExt));

						gunIOParams.stPortRGMIIDealyExt.u32ExtPortNo 	= (unsigned int)gu64RtkUtilCmdArg[0];
						gunIOParams.stPortRGMIIDealyExt.u32TxDelay 		= (unsigned int)gu64RtkUtilCmdArg[1];
						gunIOParams.stPortRGMIIDealyExt.u32RxDelay 		= (unsigned int)gu64RtkUtilCmdArg[2];

						pCtlParams = (void *)&gunIOParams.stPortRGMIIDealyExt;

						break;

				/* Command : 0x25 */
				case RTK_UTILCMD_PORTRGMIICLKEXT_GET:
						memset(&gunIOParams.stPortRGMIIDealyExt, 0x00, sizeof(gunIOParams.stPortRGMIIDealyExt));

						gunIOParams.stPortRGMIIDealyExt.u32ExtPortNo 	= (unsigned int)gu64RtkUtilCmdArg[0];
						gunIOParams.stPortRGMIIDealyExt.u32TxDelay 		= (unsigned int)gu64RtkUtilCmdArg[1];
						gunIOParams.stPortRGMIIDealyExt.u32RxDelay 		= (unsigned int)gu64RtkUtilCmdArg[2];

						pCtlParams = (void *)&gunIOParams.stPortRGMIIDealyExt;

						break;

       	default:
           break;
   	}


		/* pass data to device driver by means of ioctl command */
		#if 0
   	if(SWITCH_HAL_ERR_OK != (s32RetVal = RTL83XX_HalComm_IOCTL_CmdEx(u32Cmd, pContrlParams)))
   	{
      	dprintf("IOCTL failed for ioctl command 0x%x,	error = %0x%x\n", u32Cmd, s32RetVal);
      	return SWITCH_HAL_ERR_FAILED;
   	}
		#else
   	RtkllFkt = rtk_ethsw_flow_fkt_ptr_tbl[u32Cmd];
		RtkllFkt((unsigned int)pCtlParams);
   	#endif		

#if 0
		/*
		* For ioctl commands that read back anything, print the returned data
		*/
   	switch(u32Cmd) 
  	{

					
          default:
               break;

   	}
#endif

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_IOCTL_Utility, just like switch_U_main in Lantiq			
*
*   DESCRIPTION:
*   Main function of the switch IOCTL interface
*		This function first opens the character device, checks
*		the input command, converts the input parameters and then
* 	builds the ioctl command and executes it
*	
* 	RETURN:
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_Utility_Main(int argc, char **argv)
{
   	int s32RetVal=SWHAL_ERR_FAILED;

		dprintf("[%s] argc = %d, argv[1] = %s\n", __FUNCTION__, argc, argv[1]);

   	if((argc==1) ||
      ((argc==2) && ((strncmp(argv[1],"-h",2) == 0) || (strncmp(argv[1],"--help",6) == 0))))
   	{
	      dprintf("\nUsage: rtk switch_utility %s %s\n", gstRtkUtilCommands[gs32CmdIndex].s8Name, gstRtkUtilCommands[gs32CmdIndex].s8ParamStr);
  			dprintf("\n%s\n", gstRtkUtilCommands[gs32CmdIndex].s8DetailInfo);

				return SWHAL_ERR_SUCCESS;
   	}

		/* check command passed as argv[1] from command line */
		if((gs32CmdIndex = Util_CheckCmd(argv[1])) == -1)
		{
			  dprintf("[%s] Invalid command passed!\n", __FUNCTION__);
				
			  return SWHAL_ERR_IOCMD;
		}

		/* check number of parameters passed as command line parameters */
		if(strlen(gstRtkUtilCommands[gs32CmdIndex].s8Param) != (argc-2))
		{
			  dprintf("[%s] Invalid number of parameters: is %d, should be %d\n",
			     __FUNCTION__, (argc-2), strlen(gstRtkUtilCommands[gs32CmdIndex].s8Param));
				
   			dprintf("\nUsage: rtk_switch_utility %s %s\n", gstRtkUtilCommands[gs32CmdIndex].s8Name, gstRtkUtilCommands[gs32CmdIndex].s8ParamStr);
  			dprintf("\n%s\n", gstRtkUtilCommands[gs32CmdIndex].s8DetailInfo);
				
			  return SWHAL_ERR_FAILED;
		}

		/* convert parameter strings included in argv[2]... to cmd_arg array */
		if(Util_ConvertParams(&argv[2], argc-2) == -1)
		{
			  dprintf("[%s] Invalid command passed!\n", __FUNCTION__);

				//print_command(cmd_index);
   			dprintf("Usage: switch_utility %s %s\n", gstRtkUtilCommands[gs32CmdIndex].s8Name, gstRtkUtilCommands[gs32CmdIndex].s8ParamStr);
  			dprintf("\n%s\n", gstRtkUtilCommands[gs32CmdIndex].s8DetailInfo);
				
			  return SWHAL_ERR_FAILED;
		}

		/* finally build and send the ioctl command */
		//if(gstRtkUtilCommands[gs32CmdIndex].u32IOCtlCmd != 0xFFFFFFFF)
		//{
				s32RetVal = RTL83XX_Utility_CmdBuildEx(gstRtkUtilCommands[gs32CmdIndex].u32IOCtlCmd);
		//}
		//else
		//{
		  	//s32RetVal = execute_user_command(gstRtkUtilCommands[gs32CmdIndex].s8Name);
		//}
				
   	return s32RetVal;

}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_IOCTL or you can call rtk_switch_utility
*
*   DESCRIPTION:
*   This API is I/O Control for RTK switch ref. from switch_utility()
*
*
*
*   NOTE:
*
*
*
*****************************************************************/
int RTK_Switch_Utility(char *ps8Cmd)
{
		char *argv[RTK_SWITCH_UTILITY_COMMAND_BUFFER];
		int j, i=1;

		/* capture command & parameter */
		if(ps8Cmd && *ps8Cmd) 
		{
				//dprintf("[%s] ps8Cmd = %s\n", __FUNCTION__, ps8Cmd);

				argv[0] = "rtk_switch_utility";
				argv[i] = ps8Cmd;
				while(*ps8Cmd) 
				{
						if (*ps8Cmd == ' ') 
						{
								*ps8Cmd = '\0';
								argv[++i] = ps8Cmd+1;
						}
						
						if(i >= RTK_SWITCH_UTILITY_COMMAND_BUFFER)
						{
								dprintf("[switch_utility] command buffer overflow!\n");
								return SWHAL_ERR_IOCMD;
						}
						
						ps8Cmd++;
				}
		}

		//dprintf("[%s] i = %d\n", __FUNCTION__, i);
		#if 1 // just dump message
		{
				for (j=0; j<=i; j++)
				{
						if(j==0)
							dprintf("%s[%s] ", SWCLR1_32_GREEN, __FUNCTION__);

						dprintf("%s, ", argv[j]);
				}
				
				dprintf("\n%s",SWCLR0_RESET);
		}
		#endif

		RTL83XX_Utility_Main(i+1, argv);

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_Utility_MaxPktLengthSet
*
*   DESCRIPTION:
*   This API set the max packet length of the specific unit
*
*		Max packet length of the specific unit
*   - MAXPKTLEN_1522B
*   - MAXPKTLEN_1536B
*   - MAXPKTLEN_1552B
*   - MAXPKTLEN_16000B
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_Utility_MaxPktLengthSet(unsigned int u32Params)
{
		int s32RetVal;

		dprintf("[%s] Max pkt length = 0x%x\n", __FUNCTION__, u32Params);
		
		if(RT_ERR_OK != (s32RetVal = rtk_switch_maxPktLen_set((rtk_switch_maxPktLen_t)u32Params)))
		{
				dprintf("Error setting max pkt length, errno = 0x%x\n", s32RetVal); 

				return SWHAL_ERR_FAILED; 
		}
		
		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_Utility_MaxPktLengthGet
*
*   DESCRIPTION:
*   This API get the max packet length of the specific unit
*
*		Max packet length of the specific unit
*   - MAXPKTLEN_1522B
*   - MAXPKTLEN_1536B
*   - MAXPKTLEN_1552B
*   - MAXPKTLEN_16000B
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_Utility_MaxPktLengthGet(unsigned int u32Params)
{
		int s32RetVal;
		unsigned int u32MaxPktLen=0;

		dprintf("[%s] addr = 0x%x\n", __FUNCTION__, u32Params);

		if(u32Params != 0)
		{
				if(RT_ERR_OK != (s32RetVal = rtk_switch_maxPktLen_get((rtk_switch_maxPktLen_t *)u32Params)))
				{
						dprintf("Error getting max pkt length, errno = 0x%x\n", s32RetVal); 

						*(unsigned int *)u32Params = SWHAL_ERR_FAILED;
						return SWHAL_ERR_FAILED; 
				}

				dprintf("[%s] u32MaxPktLen = %d\n", __FUNCTION__, u32MaxPktLen);

				if(*(unsigned int *)u32Params == MAXPKTLEN_1522B)
					*(unsigned int *)u32Params = 1522;
				else if(*(unsigned int *)u32Params == MAXPKTLEN_1536B)
					*(unsigned int *)u32Params = 1536;
				else if(*(unsigned int *)u32Params == MAXPKTLEN_1536B)
					*(unsigned int *)u32Params = 1552;
				else if(*(unsigned int *)u32Params == MAXPKTLEN_16000B)
					*(unsigned int *)u32Params = 16000;
				else
					*(unsigned int *)u32Params = SWHAL_ERR_FAILED;
				
				return SWHAL_ERR_SUCCESS;
		}
		else
		{
				return SWHAL_ERR_PARAMS; 
		}
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_Utility_PortMaxPktLengthSet
*
*   DESCRIPTION:
*   This API set 2nd max packet length configuration 
*
*		Max packet length of the specific unit
*   - MAXPKTLEN_1522B
*   - MAXPKTLEN_1536B
*   - MAXPKTLEN_1552B
*   - MAXPKTLEN_16000B
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_Utility_PortMaxPktLengthSet(RTK_PORT_MAXPKTLEN *pu32Params)
{
		int s32RetVal;

		dprintf("[%s] port id = %d, max len. = 0x%x \n", __FUNCTION__, pu32Params->u32PortNo, pu32Params->u32MaxPktLen);

		if(RT_ERR_OK != (s32RetVal = rtk_switch_portMaxPktLen_set((rtk_port_t)pu32Params->u32PortNo,(rtk_uint32)pu32Params->u32MaxPktLen)))
		{
				dprintf("Error setting max pkt length, errno = 0x%x\n", s32RetVal); 

				return SWHAL_ERR_FAILED; 
		}
		
		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_Utility_PortMaxPktLengthGet
*
*   DESCRIPTION:
*   This API get the max packet length of the specific unit
*
*		Max packet length of the specific unit
*   - MAXPKTLEN_1522B
*   - MAXPKTLEN_1536B
*   - MAXPKTLEN_1552B
*   - MAXPKTLEN_16000B
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_Utility_PortMaxPktLengthGet(RTK_PORT_MAXPKTLEN *pu32Params)
{
		int s32RetVal;

		dprintf("[%s] port id = %d, addr = 0x%x \n", __FUNCTION__, pu32Params->u32PortNo, pu32Params->u32MaxPktLen);

		if(pu32Params != NULL)
		{
				if(RT_ERR_OK != (s32RetVal = rtk_switch_portMaxPktLen_get((rtk_port_t)pu32Params->u32PortNo, (rtk_uint32 *)pu32Params->u32MaxPktLen)))
				{
						dprintf("Error getting max pkt length, errno = 0x%x\n", s32RetVal); 

						*(rtk_uint32 *)pu32Params->u32MaxPktLen = SWHAL_ERR_FAILED;
						return SWHAL_ERR_FAILED; 
				}
				
				return SWHAL_ERR_SUCCESS;
		}
		else
		{
				return SWHAL_ERR_FAILED; 
		}

}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_Utility_L2LUTTypeSet
*
*   DESCRIPTION:
*   This API set Lut IP multicast lookup function
*
*
*   NOTE:
*
*   This API can work with rtk_l2_ipMcastAddrLookupException_add.
*   If users set the lookup type to DIP, the group in exception table
*   will be lookup by DIP+SIP
*      
*		If users set the lookup type to DIP+SIP, the group in exception table
*   will be lookup by only DIP
*
*****************************************************************/
int RTL83XX_Utility_L2LUTTypeSet(unsigned int u32Params)
{
		int s32RetVal;

		dprintf("[%s] L2LUT Type = %d\n",	__FUNCTION__, u32Params);

		if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddrLookup_set((rtk_l2_lookup_type_t)u32Params)))
		{
				dprintf("Error enable L2 LUT IP look-up function, errno = 0x%x\n", s32RetVal); 
				return SWHAL_ERR_FAILED; 
		}

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_Utility_L2LUTTypeGet
*
*   DESCRIPTION:
*   This API get Lut IP multicast lookup function
*
*   NOTE:
*
*
*****************************************************************/
int RTL83XX_Utility_L2LUTTypeGet(unsigned int u32Params)
{
		int s32RetVal;

		if(u32Params != 0)
		{
				if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddrLookup_get((rtk_l2_lookup_type_t *)u32Params)))
				{
						dprintf("Error enable L2 LUT IP look-up function, errno = 0x%x\n", s32RetVal); 
						return SWHAL_ERR_FAILED; 
				}

				dprintf("[%s] L2LUT Type = %d\n",	__FUNCTION__, *(unsigned int*)u32Params);

				return SWHAL_ERR_SUCCESS;
		}
		else
		{
				return SWHAL_ERR_FAILED; 
		}
}


/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_Utility_L2IpMulticastTableEntryAdd
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
int RTL83XX_Utility_L2IpMulticastTableEntryAdd(RTK_L2LUT_STATUS *pstParams)
{
		int s32RetVal;
		rtk_portmask_t stRtkPortMask;

		dprintf("[%s] GDP = %d.%d.%d.%d, GSP = %d.%d.%d.%d, Port Mask = 0x%x\n",	__FUNCTION__,
			(pstParams->u32DIP >> 24)&0xFF, ((pstParams->u32DIP >> 16)&0xFF),	
			(pstParams->u32DIP >> 8)&0xFF, (pstParams->u32DIP & 0xFF),
			(pstParams->u32SIP >> 24)&0xFF, ((pstParams->u32SIP >> 16)&0xFF),	
			(pstParams->u32SIP >> 8)&0xFF, (pstParams->u32SIP & 0xFF),
			 pstParams->u32PortMask);


		stRtkPortMask.bits[0] = pstParams->u32PortMask;
		dprintf("[%s] stRtkPortMask.bits = 0x%x\n", __FUNCTION__, stRtkPortMask.bits[0]);

		if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_add((ipaddr_t)pstParams->u32SIP, (ipaddr_t)pstParams->u32DIP, (rtk_portmask_t)stRtkPortMask)))
		{
				dprintf("Error L2 IpMcastAddr Add, errno = 0x%x\n", s32RetVal); 
				return SWHAL_ERR_FAILED; 
		}

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_Utility_L2IpMulticastTableEntryRemove
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
int RTL83XX_Utility_L2IpMulticastTableEntryRemove(RTK_L2LUT_STATUS *pstParams)
{
		int s32RetVal;

		dprintf("[%s] GDP = %d.%d.%d.%d, GSP = %d.%d.%d.%d, Port Mask = 0x%x\n",	__FUNCTION__,
			(pstParams->u32DIP >> 24)&0xFF, ((pstParams->u32DIP >> 16)&0xFF),	
			(pstParams->u32DIP >> 8)&0xFF, (pstParams->u32DIP & 0xFF),
			(pstParams->u32SIP >> 24)&0xFF, ((pstParams->u32SIP >> 16)&0xFF),	
			(pstParams->u32SIP >> 8)&0xFF, (pstParams->u32SIP & 0xFF),
			 pstParams->u32PortMask);

		if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddr_del((ipaddr_t)pstParams->u32SIP, (ipaddr_t)pstParams->u32DIP)))
		{
				dprintf("Error L2 IpMcastAddr Del, errno = 0x%x\n", s32RetVal); 
				return SWHAL_ERR_FAILED; 
		}

		return SWHAL_ERR_SUCCESS;
}



/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_Utility_PortForceLinkExtCfgSet
*
*   DESCRIPTION:
*   This API set external interface force linking configuration.
*
*   The external interface can be set to:
*      - MODE_EXT_DISABLE
*      - MODE_EXT_RGMII
*      - MODE_EXT_MII_MAC
*      - MODE_EXT_MII_PHY
*      - MODE_EXT_TMII_MAC
*      - MODE_EXT_TMII_PHY
*      - MODE_EXT_GMII
*      - MODE_EXT_RMII_MAC
*      - MODE_EXT_RMII_PHY
*
*   NOTE:
*
*
*
*****************************************************************/
int RTL83XX_Utility_PortForceLinkExtCfgSet(RTK_PORT_FORCELINKEXT_STATUS *pstParams)
{
		int s32RetVal;

		#if 0
		dprintf("[%s] Ext. Port Id = %d, Force Duplex/Speed/Link = %d, Duplex = %d, Speed = %d, Link = %d, MII Mode = %d, Nway = %d, TX pause = %d, RX pause = %d\n", __FUNCTION__\
									,pstParams->u32ExtPortNo, pstParams->stMacStatus.forcemode, pstParams->stMacStatus.duplex, pstParams->stMacStatus.speed, pstParams->stMacStatus.link\
									,pstParams->u32Mode, pstParams->stMacStatus.nway, pstParams->stMacStatus.txpause, pstParams->stMacStatus.rxpause);
		#endif
		dprintf("[%s] Ext. Port Id = %d, MII Mode = %d, Force Duplex/Speed/Link = %d, Speed = %d, Duplex = %d, Link = %d, Nway = %d, TX pause = %d, RX pause = %d\n", __FUNCTION__\
									,pstParams->u32ExtPortNo, pstParams->u32Mode, pstParams->pstMacStatus->u32Forcemode, pstParams->pstMacStatus->u32Speed, pstParams->pstMacStatus->u32Duplex\
									,pstParams->pstMacStatus->u32Link, pstParams->pstMacStatus->u32Nway, pstParams->pstMacStatus->u32Txpause, pstParams->pstMacStatus->u32Rxpause);

		if(RT_ERR_OK !=	(s32RetVal = rtk_port_macForceLinkExt_set((rtk_port_t)pstParams->u32ExtPortNo, (rtk_mode_ext_t)pstParams->u32Mode, (rtk_port_mac_ability_t *)pstParams->pstMacStatus)))
		{
				dprintf("Set port %d force linking configuration mode %d failed! error=0x%08x\n", pstParams->u32ExtPortNo, pstParams->u32Mode, s32RetVal);
				return SWHAL_ERR_FAILED;
		}

		return SWHAL_ERR_SUCCESS;
}


/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_Utility_PortForceLinkExtCfgGet
*
*   DESCRIPTION:
*   This API get external interface force mode properties.
*
*
*
*   NOTE:
*
*
*****************************************************************/
int RTL83XX_Utility_PortForceLinkExtCfgGet(RTK_PORT_FORCELINKEXT_STATUS *pstParams)
{
		int s32RetVal;

		dprintf("[%s] Ext. Port Id = %d, addr = 0x%x, addr = 0x%x\n", __FUNCTION__, pstParams->u32ExtPortNo, pstParams->u32Mode, (unsigned int)pstParams->pstMacStatus);

		if(RT_ERR_OK !=	(s32RetVal = rtk_port_macForceLinkExt_get((rtk_port_t)pstParams->u32ExtPortNo, (rtk_mode_ext_t *)pstParams->u32Mode, (rtk_port_mac_ability_t *)pstParams->pstMacStatus)))
		{
				dprintf("Get port force linking configuration failed! error=0x%08x\n", s32RetVal);
				return SWHAL_ERR_FAILED;
		}
		
		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_Utility_PortRGMIIDelayExtSet
*
*   DESCRIPTION:
*   This API set RGMII interface delay value for TX and RX.
*
* 	TX delay value, 1 for delay 2ns and 0 for no-delay
*   RX delay value, 0~7 for delay setup.
*
*   This API can set external interface 2 RGMII delay.
*   In TX delay, there are 2 selection: no-delay and 2ns delay.
*   In RX dekay, there are 8 steps for delay tunning. 0 for no-delay, and 7 for maximum delay.
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_Utility_PortRGMIIDelayExtSet(RTK_PORT_RGMIIDELAYEXT *pstParams)
{
		int s32RetVal;

		dprintf("[%s] Ext. Port Id = %d, u32TxDelay = 0x%x, u32RxDelay = 0x%x\n", __FUNCTION__, pstParams->u32ExtPortNo, pstParams->u32TxDelay, pstParams->u32RxDelay);
	
		if(RT_ERR_OK != (s32RetVal = rtk_port_rgmiiDelayExt_set((rtk_ext_port_t)pstParams->u32ExtPortNo, (rtk_data_t)pstParams->u32TxDelay, (rtk_data_t)pstParams->u32RxDelay)))
		{
				return SWHAL_ERR_FAILED;
		}
		
		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_Utility_PortRGMIIDelayExtGet
*
*   DESCRIPTION:
*   This API get RGMII interface delay value for TX and RX.
*
* 	TX delay value, 1 for delay 2ns and 0 for no-delay
*   RX delay value, 0~7 for delay setup.
*
*   This API can set external interface 2 RGMII delay.
*   In TX delay, there are 2 selection: no-delay and 2ns delay.
*   In RX dekay, there are 8 steps for delay tunning. 0 for no-delay, and 7 for maximum delay.
*
*   NOTE:
*
*****************************************************************/
int RTL83XX_Utility_PortRGMIIDelayExtGet(RTK_PORT_RGMIIDELAYEXT *pstParams)
{
		int s32RetVal;

		dprintf("[%s] Ext. Port Id = %d, addr = 0x%x, addr = 0x%x\n", __FUNCTION__, pstParams->u32ExtPortNo, pstParams->u32TxDelay, pstParams->u32RxDelay);

		if(RT_ERR_OK != (s32RetVal = rtk_port_rgmiiDelayExt_get((rtk_ext_port_t)pstParams->u32ExtPortNo, (rtk_data_t *)pstParams->u32TxDelay, (rtk_data_t *)pstParams->u32RxDelay)))
		{
				return SWHAL_ERR_FAILED;
		}
		
		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_Utility_CPUPortTagEnSet
*
*   DESCRIPTION:
*   This API 
*
*
*
*   NOTE:
*
*
*****************************************************************/
int RTL83XX_Utility_CPUPortTagEnSet(unsigned int u32Params)
{
		int s32RetVal;

		dprintf("[%s] u32Params = 0x%x\n", __FUNCTION__, u32Params);

		if(RT_ERR_OK != (s32RetVal = rtk_cpu_enable_set((rtk_enable_t)u32Params)))
		{
				dprintf("Error CPU tagPort, errno = %d\n", s32RetVal); 
				return SWHAL_ERR_FAILED; 
		}

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_Utility_CPUPortTagEnGet
*
*   DESCRIPTION:
*   This API 
*
*
*
*   NOTE:
*
*
*****************************************************************/
int RTL83XX_Utility_CPUPortTagEnGet(unsigned int u32Params)
{
		int s32RetVal;

		dprintf("[%s] addr = 0x%x\n", __FUNCTION__, u32Params);

		if(u32Params != 0)
		{
				if(RT_ERR_OK != (s32RetVal = rtk_cpu_enable_get((rtk_enable_t *)u32Params)))
				{
						dprintf("Error CPU tagPort, errno = %d\n", s32RetVal); 
						return SWHAL_ERR_FAILED; 
				}

				dprintf("[%s] CPU Port tag fun = %d\n", __FUNCTION__, *(unsigned int *)u32Params);
		}
		else
		{
				return SWHAL_ERR_PARAMS; 
		}

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_Utility_CPUPortTagCfgSet
*
*   DESCRIPTION:
*   This API 
*
*
*
*   NOTE:
*
*
*****************************************************************/
int RTL83XX_Utility_CPUPortTagCfgSet(RTK_CPUPORT_TAG	*pu32Params)
{
		int s32RetVal;

		dprintf("[%s] u32CpuPortNo = %d, u32CpuPortMode = %d, u32CpuPortMAsk = 0x%x\n", __FUNCTION__, pu32Params->u32CpuPortNo, pu32Params->u32CpuPortMode, pu32Params->u32CpuPortMAsk);

		if(RT_ERR_OK != (s32RetVal = rtk_cpu_tagPort_set((rtk_port_t)pu32Params->u32CpuPortNo, (rtk_cpu_insert_t)pu32Params->u32CpuPortMode)))
		{
				dprintf("Error CPU tagPort, errno = %d\n", s32RetVal); 
				return SWHAL_ERR_FAILED; 
		}

		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_Utility_CPUPortTagCfgGet
*
*   DESCRIPTION:
*   This API 
*
*
*
*   NOTE:
*
*
*****************************************************************/
int RTL83XX_Utility_CPUPortTagCfgGet(RTK_CPUPORT_TAG	*pu32Params)
{
		int s32RetVal;

		dprintf("[%s] addr = 0x%x, addr = 0x%x\n", __FUNCTION__, pu32Params->u32CpuPortNo, pu32Params->u32CpuPortMode);

		if(RT_ERR_OK != (s32RetVal = rtk_cpu_tagPort_get((rtk_port_t *)pu32Params->u32CpuPortNo, (rtk_cpu_insert_t *)pu32Params->u32CpuPortMode)))
		{
				dprintf("Error CPU tagPort, errno = %d\n", s32RetVal); 
				return SWHAL_ERR_FAILED; 
		}

		return SWHAL_ERR_SUCCESS;
}

#if 0
/*****************************************************************
*
*   PROCEDURE NAME:
*   RTL83XX_HalComm_IOFLOW_L2IpMulticastTableEntryDump
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
int RTL83XX_HalComm_IOFLOW_L2IpMulticastTableEntryDump(RTK_L2LUT_STATUS *pstParams)
{
		int s32RetVal;

		RTL83XX_HalComm_L2LUT_IpMcastTable_Dump();
		
		return SWITCH_HAL_ERR_OK;
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
int RTL83XX_Utility_(void)
{
		//int s32RetVal;


		
		return SWHAL_ERR_SUCCESS;
}

/*****************************************************************
*	Test Program
*****************************************************************/
int Test_Switch_Utility_Cmd(void)
{
		int s32RetVal;
		
		//int s32L2LUTInitStatus;
		//int s32InitStatus;
		int s32CPUPortStatus;
		int s32GotMaxPktLen; 
		//int s32GotPortMaxPktLen;
		//int s32MaxPktLen=1536;
		//int s32PhyPortNo=0;
		int s32GotL2LUTType;
		//int s32L2LUTType=2;
		int s32GotPortMode;
		rtk_port_mac_ability_t stGotPortStatus;
		int s32GotCPUPortNo, s32GotCPUPortMode;

		int s32GotTxDelay, s32GotRxDelay;

#if 0
		sprintf(gs8RtkUtilCmdStr, "RTK_SysInit 0x%x", &s32InitStatus);
		RTK_Switch_Utility(gs8RtkUtilCmdStr);
		dprintf("[%s] s32InitStatus = 0x%x, gunIOParams.u32PtrAddr = 0x%x\n", __FUNCTION__, s32InitStatus, gunIOParams.u32PtrAddr);

		sprintf(gs8RtkUtilCmdStr, "RTK_MaxPktLengthSet 0x%x", s32MaxPktLen);
		RTK_Switch_Utility(gs8RtkUtilCmdStr);
		dprintf("[%s] gunIOParams.u32PktLength = 0x%x\n", __FUNCTION__, gunIOParams.u32PktLength);

		sprintf(gs8RtkUtilCmdStr, "RTK_PortMaxPktLengthSet %d 0x%x", s32PhyPortNo, 1536);
		RTK_Switch_Utility(gs8RtkUtilCmdStr);

		sprintf(gs8RtkUtilCmdStr, "RTK_PortMaxPktLengthGet %d 0x%x", s32PhyPortNo, &s32GotPortMaxPktLen);
		RTK_Switch_Utility(gs8RtkUtilCmdStr);
		dprintf("[%s] s32GotPortMaxPktLen = 0x%x\n", __FUNCTION__, s32GotPortMaxPktLen);

		sprintf(gs8RtkUtilCmdStr, "RTK_L2IpMulticastTableInit 0x%x", &s32L2LUTInitStatus);
		RTK_Switch_Utility(gs8RtkUtilCmdStr);
		dprintf("[%s] s32L2LUTInitStatus = 0x%x, gunIOParams.u32PtrAddr = 0x%x\n", __FUNCTION__, s32L2LUTInitStatus, gunIOParams.u32PtrAddr);

		sprintf(gs8RtkUtilCmdStr, "RTK_L2IpMulticastTableTypeSet 0x%x", s32L2LUTType);
		RTK_Switch_Utility(gs8RtkUtilCmdStr);

    //"<Ext. Port Id> <MII Mode> <Force Duplex/Speed/Link> <Speed> <Duplex> <Link> <Nway> <TX pause> <RX pause>",
		sprintf(gs8RtkUtilCmdStr, "RTK_PortForceLinkExtCfgSet %d %d %d %d %d %d %d %d %d", 1, 1, 1, 1000, 1, 1, 0, 0, 0);
		RTK_Switch_Utility(gs8RtkUtilCmdStr);
		dump_rtl83xx_extport_ability(4, 1, 1);

		sprintf(gs8RtkUtilCmdStr, "RTK_PortRGMIIClkExtSet %d 0x%x 0x%x", 1, 1, 0);
		RTK_Switch_Utility(gs8RtkUtilCmdStr);

		sprintf(gs8RtkUtilCmdStr, "RTK_PortLinkCfgGet %d 0x%x", 1, &stGotPortStatus);
		RTK_Switch_Utility(gs8RtkUtilCmdStr);

#endif

		sprintf(gs8RtkUtilCmdStr, "RTK_PortForceLinkExtCfgGet %d 0x%hx 0x%hx", RTK_EXT_1, (int)&s32GotPortMode, (int)&stGotPortStatus);
		RTK_Switch_Utility(gs8RtkUtilCmdStr);
		dprintf("[%s] s32GotPortMode = 0x%x\n", __FUNCTION__, s32GotPortMode);
		dprintf("[%s] forcemode = %d\n", __FUNCTION__, stGotPortStatus.forcemode);
		dprintf("[%s] speed     = %d\n", __FUNCTION__, stGotPortStatus.speed);
		dprintf("[%s] duplex    = %d\n", __FUNCTION__, stGotPortStatus.duplex);
		dprintf("[%s] link      = %d\n", __FUNCTION__, stGotPortStatus.link);
		dprintf("[%s] nway      = %d\n", __FUNCTION__, stGotPortStatus.nway);
		dprintf("[%s] txpause   = %d\n", __FUNCTION__, stGotPortStatus.txpause);
		dprintf("[%s] rxpause   = %d\n\n", __FUNCTION__, stGotPortStatus.rxpause);

		sprintf(gs8RtkUtilCmdStr, "RTK_PortForceLinkExtCfgGet %d 0x%hx 0x%hx", RTK_EXT_2, (int)&s32GotPortMode, (int)&stGotPortStatus);
		RTK_Switch_Utility(gs8RtkUtilCmdStr);
		dprintf("[%s] s32GotPortMode = 0x%x\n", __FUNCTION__, s32GotPortMode);
		dprintf("[%s] forcemode = %d\n", __FUNCTION__, stGotPortStatus.forcemode);
		dprintf("[%s] speed     = %d\n", __FUNCTION__, stGotPortStatus.speed);
		dprintf("[%s] duplex    = %d\n", __FUNCTION__, stGotPortStatus.duplex);
		dprintf("[%s] link      = %d\n", __FUNCTION__, stGotPortStatus.link);
		dprintf("[%s] nway      = %d\n", __FUNCTION__, stGotPortStatus.nway);
		dprintf("[%s] txpause   = %d\n", __FUNCTION__, stGotPortStatus.txpause);
		dprintf("[%s] rxpause   = %d\n\n", __FUNCTION__, stGotPortStatus.rxpause);

		sprintf(gs8RtkUtilCmdStr, "RTK_PortRGMIIClkExtGet %d 0x%hx 0x%hx", RTK_EXT_1, (int)&s32GotTxDelay, (int)&s32GotRxDelay);
		RTK_Switch_Utility(gs8RtkUtilCmdStr);
		dprintf("[%s] s32GotPortMode = 0x%x, s32GotRxDelay = 0x%x\n", __FUNCTION__, s32GotTxDelay, s32GotRxDelay);

		sprintf(gs8RtkUtilCmdStr, "RTK_L2IpMulticastTableTypeGet 0x%hx", (int)&s32GotL2LUTType);
		RTK_Switch_Utility(gs8RtkUtilCmdStr);
		dprintf("[%s] s32GotL2LUTType = 0x%x\n", __FUNCTION__, s32GotL2LUTType);

		sprintf(gs8RtkUtilCmdStr, "RTK_CPUPortTagEnGet 0x%hx", (int)&s32CPUPortStatus);
		RTK_Switch_Utility(gs8RtkUtilCmdStr);
		dprintf("[%s] s32CPUPortStatus = %d\n", __FUNCTION__, s32CPUPortStatus);

		sprintf(gs8RtkUtilCmdStr, "RTK_MaxPktLengthGet 0x%hx", (int)&s32GotMaxPktLen);
		RTK_Switch_Utility(gs8RtkUtilCmdStr);
		dprintf("[%s] s32GotMaxPktLen = 0x%x, gunIOParams.u32PtrAddr = 0x%x\n", __FUNCTION__, s32GotMaxPktLen, gunIOParams.u32PtrAddr);

		sprintf(gs8RtkUtilCmdStr, "RTK_CPUPortTagCfgGet 0x%hx 0x%hx", (int)&s32GotCPUPortNo, (int)&s32GotCPUPortMode);
		if(SWHAL_ERR_SUCCESS != (s32RetVal = RTK_Switch_Utility(gs8RtkUtilCmdStr)))
		{
				dprintf("RTK_CPUPortTagGet error\n");
		}
		dprintf("[%s] s32GotCPUPortNo = %d, s32GotCPUPortMode = 0x%x\n", __FUNCTION__, s32GotCPUPortNo, s32GotCPUPortMode);

		return SWHAL_ERR_SUCCESS;
}



