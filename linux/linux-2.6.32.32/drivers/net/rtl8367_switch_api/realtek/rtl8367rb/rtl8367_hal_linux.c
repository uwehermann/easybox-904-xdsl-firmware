/* ============================================================
rtl83XX_hal_linux.c: RealTek 8367 series chipset ethernet driver.

Follow by 
This API package supports multiple Realtek Switch contains ASIC  
drivers for LINUX. ASIC drivers provide general APIs that based 
on user configuration to configure relative ASIC registers.


Programmer : Alvin Hsu, alvin_hsu@arcadyan.com
=============================================================== */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h> 				/* printk() 			*/ 
#include <linux/slab.h>						/* kmalloc() 			*/
#include <linux/types.h>  				/* size_t 				*/ 
#include <linux/fs.h>							/* everything... 	*/
#include <linux/proc_fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/string.h>					/* strtok() 			*/
#include <linux/stat.h>
#include <linux/utsname.h>      	/* struct utsname */
#include <linux/version.h>
#include <linux/delay.h> 
#include <linux/errno.h>					/* error codes 		*/

#include "switch_vendor_error.h"	/* local definitions */
#include "switch_vendor_common.h"	/* local definitions */
#include "switch_vendor_dbg.h"		/* local definitions */
#include "switch_vendor_project.h"	/* local definitions */

#include "rtl83XX_hal.h"					/* local definitions */
#include "rtl8367_hal_linux.h"		/* local definitions */
#include "rtl8367_hal_linux_ioctl.h"

#if(_RTK_RTL83XX_SWITCH_API == 1)
#error "No available switch api, version error!!"
#elif(_RTK_RTL83XX_SWITCH_API == 2)
#include "rtl8367rb/switch_api_v1_2_10/include/rtk_api_ext.h"
#include "rtl8367rb/switch_api_v1_2_10/include/rtk_api.h"
#include "rtl8367rb/switch_api_v1_2_10/include/rtk_types.h"
#include "rtl8367rb/switch_api_v1_2_10/include/rtk_error.h"

#if(RTK_CHIP_ID == CHIP_ID_RTL8367RB)
#include "rtl8367rb/switch_api_v1_2_10/include/rtl8367b_reg.h"
#endif
#endif

#include <switch_api/ifxmips_sw_reg.h>
#include <asm-mips/gic.h>

/*****************************************************************
*	Definition
*****************************************************************/
static unsigned int RTK_MAJOR_DYANMIC=RTK_MAJOR;
static unsigned int RTK_MINOR_DYANMIC=0;
static char version[] = 
RTK_DEV_NAME ": " RTK_SWITCH_DRV_MODULE_VERSION " (" RTK_RELDATE ")\n";

RTL8367_DEV_STATE *gpstRtl8367DevState;
RTL8367_SWITCH_STATE *gpstRtl8367SwState;
unsigned char gu8Rtl8367IocAPI=RTL8367_API_KERNEL;

RTK_PORT_MAC_STATUS rtl8367_ext_port_cfg[EXT_PORT_END] =
{
		/* EXT_PORT_0 */
		{0,0,0,0,0,0,0},
		/* EXT_PORT_1 */
		{1,2,1,1,0,0,0},
		/* EXT_PORT_2 */
		{0,0,0,0,0,0,0}
};

static struct file_operations rtl8367_fops;						/* variable declaration */ 
#if defined(PROC_DEBUG)
//static struct file_operations rtl8367_proc_fops;			/* variable declaration */
#endif

/* function declaration */
int rtl8367_switch_hwrst(unsigned int u32PluseWidth);
int rtl8367_switch_restart(void);
int rtl8367_switch_coreinit(void);
int rtl8367_switch_init(void);

int rtl8367_dev_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg);
static int rtl8367_dev_open(struct inode *inode, struct file *filp);
static int rtl8367_dev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static int rtl8367_dev_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
static int rtl8367_dev_release(struct inode *inode, struct file *filp);
static int rtl8367_dev_cleanup(void);
static int rtl8367_dev_init(void);


#if defined(PROC_DEBUG)
static void rtl8367_proc_write_help(void);
static int rtl8367_proc_write(struct file *file, const char *buf, unsigned long count, void *data);
static int rtl8367_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data);
static int rtl8367_proc_create(void);
static int rtl8367_proc_destroy(void);

char gs8ProcPath[] = "driver/rtk";
struct proc_dir_entry *proc_root_rtk_driver;

/* 
* 	Create a set of file operations for our proc file. 
*/
#if 0
static struct file_operations rtl8367_proc_fops = {	
			.owner   				= THIS_MODULE,	
			//.read    				= rtl8367_proc_read,	
			//.write					= rtl8367_proc_write,
};
#endif
#endif

static struct file_operations rtl8367_fops = {    
			.owner     			= THIS_MODULE,
			.open						= rtl8367_dev_open,
			.read						= rtl8367_dev_read,
			.write					= rtl8367_dev_write,
			.ioctl     			= rtl8367_dev_ioctl,
			.release				= rtl8367_dev_release,
};


/*****************************************************************
*	Extern
*****************************************************************/
#if defined(MDC_MDIO_OPERATION) 
extern unsigned short RTL83XX_SMI_READ(unsigned short u16Addr);
extern void RTL83XX_SMI_WRITE(unsigned short u16Addr, unsigned short u16Data);
#endif

extern RTL8367_IOC_WRAPPER_CTX *gpstIocWrapperCtx;


/*****************************************************************
*	Utility
*****************************************************************/
#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & DBG_PARAMETER_DUMP) == DBG_PARAMETER_DUMP)
void util_dump_port_ext_status(RTK_PORT_FORCELINKEXT_STATUS *pstPortExtStatus, RTK_PORT_RGMIIDELAYEXT *pstPortExtDelay)
{
		if(pstPortExtStatus != RTK_NULL)
		{
				printk(KERN_INFO "[%s] RTK_PORT_FORCELINKEXT_STATUS ================= \n", __func__);

				printk(KERN_INFO "u32ExtPortNo = %d\n", pstPortExtStatus->u32ExtPortNo);
				printk(KERN_INFO "u32Mode      = %d\n\n", pstPortExtStatus->u32Mode);
				
				printk(KERN_INFO "[MacStatus]u32Forcemode   = %d\n", pstPortExtStatus->stMacStatus.u32Forcemode);
				printk(KERN_INFO "[MacStatus]u32Speed       = %d\n", pstPortExtStatus->stMacStatus.u32Speed);
				printk(KERN_INFO "[MacStatus]u32Duplex      = %d\n", pstPortExtStatus->stMacStatus.u32Duplex);
				printk(KERN_INFO "[MacStatus]u32Link        = %d\n", pstPortExtStatus->stMacStatus.u32Link);
				printk(KERN_INFO "[MacStatus]u32Nway        = %d\n", pstPortExtStatus->stMacStatus.u32Nway);
				printk(KERN_INFO "[MacStatus]u32Txpause     = %d\n", pstPortExtStatus->stMacStatus.u32Txpause);
				printk(KERN_INFO "[MacStatus]u32Rxpause     = %d\n\n", pstPortExtStatus->stMacStatus.u32Rxpause);
		}

		if(pstPortExtDelay != RTK_NULL)
		{
				printk(KERN_INFO "[%s] RTK_PORT_RGMIIDELAYEXT ================= \n", __func__);

				printk(KERN_INFO "u32ExtPortNo = %d\n", pstPortExtDelay->u32ExtPortNo);
				printk(KERN_INFO "u32TxDelay   = %d\n", pstPortExtDelay->u32TxDelay);
				printk(KERN_INFO "u32RxDelay   = %d\n", pstPortExtDelay->u32RxDelay);
		}

		return;
}
#endif

void rtk_msleep(unsigned int msec)
{
		unsigned int	i;

		for(i=0; i<msec; i++) 
		{
				udelay(1000);
		}
}


void rtl8367_definition_print(void)
{
		#if defined(SWITCH_REALTEK_RTL83XX_FAMILY)
		printk(KERN_INFO "SWITCH_REALTEK_RTL83XX_FAMILY defined!");
		#endif

		#if defined(BIG_ENDIAN)
		printk(KERN_INFO "BIG_ENDIAN defined!");
		#endif

		#if defined(MDC_MDIO_OPERATION)
		printk(KERN_INFO "MDC_MDIO_OPERATION defined!");
		#endif

		#if defined(MDIO_USED)
		printk(KERN_INFO "MDIO_USED defined!");
		#endif

		#if defined(CHIP_RTL8367RB)
		printk(KERN_INFO "CHIP_RTL8367RB defined!");
		#endif

		#if defined(_RTK_RTL83XX_SWITCH_API)
		printk(KERN_INFO "_RTK_RTL83XX_SWITCH_API=%d defined!", _RTK_RTL83XX_SWITCH_API);
		#endif


		return;
}


/*****************************************************************
*	Function
*****************************************************************/
int rtl8367_switch_hwrst(unsigned int u32PluseWidth)
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
		rtk_msleep(u32PluseWidth);

		asm("sync");

		// pull-high
		*IFX_GPIO_P2_OUT |= u32PinNo;
		
		return SWHAL_ERR_SUCCESS;
}

int rtl8367_switch_restart(void)
{
		rtl8367_switch_hwrst(RTK_SWITCH_SWRST);
		
		/* need to wait 1 sec */
		rtk_msleep(RTK_INIT_DELAY);

		rtl8367_switch_init();
		
		return SWHAL_ERR_SUCCESS;
}

int rtl8367_switch_portpower(unsigned short u16PortNo, unsigned char u8On)
{
#if 0
		unsigned short u16Addr;
		unsigned short u16Data; 

		u16Addr = 0x2000 + (u16PortNo<<5) + 0/*reg0*/;
	
		u16Data = RTL83XX_SMI_READ(u16Addr);
		//printk("KERN_INFO [%s:%d]reg[0x%x]=0x%x, ",__func__,__LINE__,u16Addr,u16Data);

		if(u8On) // Normal operation
				RTL83XX_SMI_WRITE(u16Addr,(u16Data & ~WBIT11));
		else // Power down
				RTL83XX_SMI_WRITE(u16Addr,(u16Data | WBIT11));

		//printk("write 0x%x into\n", bOn ? data & ~WBIT11 : data | WBIT11);
#else
		int s32RetVal;

		unsigned short u16Addr;
		unsigned short u16Data; 

		u16Addr = 0x2000 + (u16PortNo<<5) + 0/*reg0*/;

		u16Data = RTL83XX_SMI_READ(u16Addr);

		if(u8On == 1)
		{
				u16Data &= ~WBIT11;
		}
		else
		{
				u16Data |= WBIT11;
		}

		if(RT_ERR_OK != (s32RetVal = rtk_port_phyReg_set((rtk_port_t)u16PortNo, (rtk_port_phy_reg_t)0, (rtk_port_phy_data_t)u16Data)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))					
				if(u8On == 1)
						printk(KERN_INFO "\tPort on failed!!!, errno = 0x%08x\n", s32RetVal);
				else
						printk(KERN_INFO "\tPort down failed!!!, errno = 0x%08x\n", s32RetVal);
				#endif
				
				return s32RetVal;
		}

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_SWITCH_INIT)) == (DBG_KERNEL_SPACE|DBG_SWITCH_INIT))
		if(u8On == 1)
				printk(KERN_INFO "\tPort power on, set Register[0x%04x] = 0x%04x\n", u16Addr, u16Data);
		else
				printk(KERN_INFO "\tPort power down, set Register[0x%04x] = 0x%04x\n", u16Addr, u16Data);
		#endif
		
#endif

		return SWHAL_ERR_SUCCESS;
}


int rtl8367_switch_corecleanup(void)
{
		RTL8367_SWITCH_STATE *gpstSwState = gpstRtl8367SwState;

		if(gpstSwState != RTK_NULL)
		{
				kfree(gpstSwState);
				gpstSwState = RTK_NULL;
		}

		return SWHAL_ERR_SUCCESS;
}

int rtl8367_switch_coreinit(void)
{
		unsigned int u32TimeOut=0;
		rtk_portmask_t stStaticRouterPortMask;
		RTL8367_SWITCH_STATE *pstSwState;

		unsigned int i;
		unsigned char u8PortMode, u8TxDelay, u8RxDelay;
		int s32RetVal;

		/* ================================================================== */
		/* 0) HW Reset 																												*/
		/* ================================================================== */
		if(SWHAL_ERR_SUCCESS != rtl8367_switch_hwrst(RTK_REST_DELAY))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
				printk(KERN_ERR "[%s] rtl8367 HW reset pluse finished!!!\n", __func__);
				#endif
				
				return SWHAL_ERR_RESET;
		}
		
		/* delay 1 sec */
		rtk_msleep(RTK_INIT_DELAY);

		do{

				rtk_msleep(100);
				
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_SWITCH_INIT)) == (DBG_KERNEL_SPACE|DBG_SWITCH_INIT))
				printk(KERN_INFO "Register[0x1202]=0x%04X, timespent=%d\n", RTL83XX_SMI_READ(0x1202), u32TimeOut+=100);
				#endif
				
				if(u32TimeOut > 500)
				{
						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
						printk(KERN_ERR "%s %s HW reset failed!!! while(1) here!!!\n%s", SWCLR1_31_RED, RTK_DEV_NAME, SWCLR0_RESET);	
						#endif
						
						while(1);	
				}
			
		}while(0x88A8 != RTL83XX_SMI_READ(0x1202));
		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_SWITCH_INIT)) == (DBG_KERNEL_SPACE|DBG_SWITCH_INIT))
		printk(KERN_INFO "rtl8367 HW reset finished!, timespent=%d\n\n", u32TimeOut);
		#endif

		pstSwState = (RTL8367_SWITCH_STATE *)kmalloc(sizeof(RTL8367_SWITCH_STATE),GFP_KERNEL);
		if (!pstSwState) 
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
	    	printk(KERN_INFO "rtl8367 switch memory allocation failed!!!\n");
				#endif		
				
				kfree(pstSwState);
				pstSwState = RTK_NULL;
				
				return SWHAL_ERR_NOMEM;
    }

		memset(pstSwState, 0, sizeof(RTL8367_SWITCH_STATE));

		/* ================================================================== */
		/* 1) Switch Init Process																							*/
		/* ================================================================== */
		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_SWITCH_INIT)) == (DBG_KERNEL_SPACE|DBG_SWITCH_INIT))
		printk(KERN_DEBUG "1) rtl8367 switch init\n");	
		#endif
		
		if(SWHAL_ERR_SUCCESS != (s32RetVal = rtk_switch_init()))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
				printk(KERN_ERR "[%s] rtl8367 switch init failed!!!, errno = 0x%x\n", __func__, s32RetVal);
				#endif
				
				kfree(pstSwState);
				pstSwState = RTK_NULL;

				/* Power down PHY */
				rtl8367_switch_portpower(RTK_PORT_0_MAC,0);
				rtl8367_switch_portpower(RTK_PORT_1_MAC,0);
				rtl8367_switch_portpower(RTK_PORT_2_MAC,0);
				rtl8367_switch_portpower(RTK_PORT_3_MAC,0);
				
				return SWHAL_ERR_SYSINIT;
		}

		/* Power down PHY */
		rtl8367_switch_portpower(RTK_PORT_0_MAC,0);
		rtl8367_switch_portpower(RTK_PORT_1_MAC,0);
		rtl8367_switch_portpower(RTK_PORT_2_MAC,0);
		rtl8367_switch_portpower(RTK_PORT_3_MAC,0);

		
		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_REGISTER_DUMP)) == (DBG_KERNEL_SPACE|DBG_REGISTER_DUMP))
		printk(KERN_INFO "Register[0x1305]=0x%04X\n",RTL83XX_SMI_READ(RTL8367B_REG_DIGITAL_INTERFACE_SELECT));
		printk(KERN_INFO "Register[0x1310]=0x%04X\n",RTL83XX_SMI_READ(RTL8367B_REG_DIGITAL_INTERFACE0_FORCE));
		printk(KERN_INFO "Register[0x1311]=0x%04X\n",RTL83XX_SMI_READ(RTL8367B_REG_DIGITAL_INTERFACE1_FORCE));
		printk(KERN_INFO "Register[0x1306]=0x%04X\n",RTL83XX_SMI_READ(RTL8367B_REG_EXT0_RGMXF));
		printk(KERN_INFO "Register[0x1307]=0x%04X\n",RTL83XX_SMI_READ(RTL8367B_REG_EXT1_RGMXF));
		printk(KERN_INFO "Register[0x13c5]=0x%04X\n",RTL83XX_SMI_READ(RTL8367B_REG_EXT2_RGMXF));
		#endif

		/* ================================================================== */
		/* 2) MAC Extension Interface Set																			*/
		/* ================================================================== */
		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_SWITCH_INIT)) == (DBG_KERNEL_SPACE|DBG_SWITCH_INIT))
		printk(KERN_DEBUG "2) MAC extension interface set\n");	
		#endif		
		
		for(i=EXT_PORT_1; i<EXT_PORT_END; i++)
		{
				if (i==EXT_PORT_1)	
				{	
						u8PortMode 	= 1;
						u8TxDelay 	= 1;
						u8RxDelay 	= 0;
				}
				else	
				{
						u8PortMode 	= 0;
						u8TxDelay 	= 0;
						u8RxDelay 	= 0;
				}
				
				if(RT_ERR_OK !=	(s32RetVal = rtk_port_macForceLinkExt_set((rtk_port_t)i, (rtk_mode_ext_t)u8PortMode, (rtk_port_mac_ability_t *)&rtl8367_ext_port_cfg[i])))
				{
							#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
							printk(KERN_INFO "[%s] Set extension port %d force linking configuration mode %d failed!!!, errno = 0x%x\n", __func__, i, u8PortMode, s32RetVal);
							#endif

							goto ERR_HANDLE;
				}

				if(RT_ERR_OK != (s32RetVal = rtk_port_rgmiiDelayExt_set((rtk_ext_port_t)i, (rtk_data_t)u8TxDelay, (rtk_data_t)u8RxDelay)))
				{
							#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
							printk(KERN_INFO "[%s] Set extension port %d RGMII failed!!!, errno = 0x%x\n", __func__, i, s32RetVal);
							#endif

							goto ERR_HANDLE;
				}

				pstSwState->stPortExtStatus[i].u32ExtPortNo = i;
				pstSwState->stPortExtStatus[i].u32Mode			= u8PortMode;
				pstSwState->stPortExtStatus[i].stMacStatus  = rtl8367_ext_port_cfg[i];

				pstSwState->stPortExtDelay[i].u32ExtPortNo	= i;
				pstSwState->stPortExtDelay[i].u32TxDelay		= u8TxDelay;
				pstSwState->stPortExtDelay[i].u32RxDelay		= u8RxDelay;

				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_PARAMETER_DUMP)) == (DBG_KERNEL_SPACE|DBG_PARAMETER_DUMP))
				util_dump_port_ext_status(&pstSwState->stPortExtStatus[i],&pstSwState->stPortExtDelay[i]);
				#endif
		}

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_REGISTER_DUMP)) == (DBG_KERNEL_SPACE|DBG_REGISTER_DUMP))
		printk(KERN_INFO "Register[0x1305]=0x%04X\n",RTL83XX_SMI_READ(RTL8367B_REG_DIGITAL_INTERFACE_SELECT));
		printk(KERN_INFO "Register[0x1310]=0x%04X\n",RTL83XX_SMI_READ(RTL8367B_REG_DIGITAL_INTERFACE0_FORCE));
		printk(KERN_INFO "Register[0x1311]=0x%04X\n",RTL83XX_SMI_READ(RTL8367B_REG_DIGITAL_INTERFACE1_FORCE));
		printk(KERN_INFO "Register[0x1306]=0x%04X\n",RTL83XX_SMI_READ(RTL8367B_REG_EXT0_RGMXF));
		printk(KERN_INFO "Register[0x1307]=0x%04X\n",RTL83XX_SMI_READ(RTL8367B_REG_EXT1_RGMXF));
		printk(KERN_INFO "Register[0x13c5]=0x%04X\n",RTL83XX_SMI_READ(RTL8367B_REG_EXT2_RGMXF));
		#endif

		/* ================================================================== */
		/* 3) Static Router Port Set    																			*/
		/* ================================================================== */		
		if(RT_ERR_OK != (s32RetVal = rtk_cpu_enable_set(SW_ENABLED)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
				printk(KERN_INFO "[%s] Enable cpu port failed!!!, errno = 0x%x\n", __func__, s32RetVal);
				#endif

				goto ERR_HANDLE;
		}

		if(RT_ERR_OK != (s32RetVal = rtk_cpu_tagPort_set(RTK_PORT_6_MAC, CPU_INSERT_TO_NONE)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
				printk(KERN_INFO "[%s] Set cpu port %d for func %d failed!!!, errno = 0x%x\n", __func__, RTK_PORT_6_MAC, CPU_INSERT_TO_ALL,s32RetVal);
				#endif

				goto ERR_HANDLE;
		}

		for(i=0;i<RTL8367B_MAC_LAN_END;i++)
		{

				if(RT_ERR_OK != (s32RetVal = rtk_igmp_protocol_set(i, PROTOCOL_IGMPv1, IGMP_ACTION_TRAP2CPU)))
				{
						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
						printk(KERN_INFO "[%s] Set igmp protocol port %d , protocol %d, action %d, failed!!!, errno = 0x%x\n", __func__, i, PROTOCOL_IGMPv1, IGMP_ACTION_TRAP2CPU, s32RetVal);
						#endif

						goto ERR_HANDLE;
				}

				if(RT_ERR_OK != (s32RetVal = rtk_igmp_protocol_set(i, PROTOCOL_IGMPv2, IGMP_ACTION_TRAP2CPU)))
				{
						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
						printk(KERN_INFO "[%s] Set igmp protocol port %d , protocol %d, action %d, failed!!!, errno = 0x%x\n", __func__, i, PROTOCOL_IGMPv1, IGMP_ACTION_TRAP2CPU, s32RetVal);
						#endif

						goto ERR_HANDLE;
				}

				if(RT_ERR_OK != (s32RetVal = rtk_igmp_protocol_set(i, PROTOCOL_IGMPv3, IGMP_ACTION_TRAP2CPU)))
				{
						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
						printk(KERN_INFO "[%s] Set igmp protocol port %d , protocol %d, action %d, failed!!!, errno = 0x%x\n", __func__, i, PROTOCOL_IGMPv1, IGMP_ACTION_TRAP2CPU, s32RetVal);
						#endif

						goto ERR_HANDLE;
				}
				/* Terry 20121226, MLDv1 and MLDv2 also trap to CPU */
#if 1
				if(RT_ERR_OK != (s32RetVal = rtk_igmp_protocol_set(i, PROTOCOL_MLDv1, IGMP_ACTION_TRAP2CPU)))
				{
						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
						printk(KERN_INFO "[%s] Set igmp protocol port %d , protocol %d, action %d, failed!!!, errno = 0x%x\n", __func__, i, PROTOCOL_IGMPv1, IGMP_ACTION_TRAP2CPU, s32RetVal);
						#endif

						goto ERR_HANDLE;
				}

				if(RT_ERR_OK != (s32RetVal = rtk_igmp_protocol_set(i, PROTOCOL_MLDv2, IGMP_ACTION_TRAP2CPU)))
				{
						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
						printk(KERN_INFO "[%s] Set igmp protocol port %d , protocol %d, action %d, failed!!!, errno = 0x%x\n", __func__, i, PROTOCOL_IGMPv1, IGMP_ACTION_TRAP2CPU, s32RetVal);
						#endif

						goto ERR_HANDLE;
				}
#endif				
		}

#if 0		
		stStaticRouterPortMask.bits[0] = (0x1<<RTK_PORT_6_MAC);
		if(RT_ERR_OK != (s32RetVal = rtk_igmp_static_router_port_set(stStaticRouterPortMask)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
				printk(KERN_INFO "[%s] Set static router port %d failed!!!, errno = 0x%x\n", __func__, RTK_PORT_6_MAC, s32RetVal);
				#endif

				goto ERR_HANDLE;
		}
#endif		

		pstSwState->u8CPUPort = RTK_PORT_6_MAC;

		/* ================================================================== */
		/* 4) LookUp Table Init			    																			*/
		/* ================================================================== */
		if(RT_ERR_OK != (s32RetVal = rtk_l2_init()))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
				printk(KERN_ERR "[%s] Init L2 LUT failed!!!, errno = 0x%x\n", __func__, s32RetVal);
				#endif
				
				kfree(pstSwState);
				pstSwState = RTK_NULL;

				return SWHAL_ERR_L2INIT;
		}

		if(RT_ERR_OK != (s32RetVal = rtk_l2_ipMcastAddrLookup_set(LOOKUP_DIP)))
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
				printk(KERN_INFO "[%s] Set L2 LUT type failed!!!, errno = 0x%x\n", __func__, s32RetVal);
				#endif
				
				goto ERR_HANDLE;
		}

		pstSwState->stL2LUTStatus.u32LookUpTblType = LOOKUP_DIP;
		pstSwState->stL2LUTStatus.u8L2Init = SW_ENABLED;

		pstSwState->u8SysInit = SW_ENABLED;

		gpstRtl8367SwState = pstSwState;
		
		return SWHAL_ERR_SUCCESS;

ERR_HANDLE:

		kfree(pstSwState);
		pstSwState = RTK_NULL;

		return SWHAL_ERR_FAILED;
}


int rtl8367_switch_init(void)
{
		void *pRetPtr;
		int s32RetVal;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_SWITCH_INIT)) == (DBG_KERNEL_SPACE|DBG_SWITCH_INIT))
		printk(KERN_INFO "Do rtl8367 switch init!!!\n");
		#endif

		/* ================================================================== */
		/* rtl8367 switch core init																						*/
		/* ================================================================== */
    if(SWHAL_ERR_SUCCESS != (s32RetVal=rtl8367_switch_coreinit())) 
    {
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
				printk(KERN_ERR "[%s] rtl8367 init-process failed!!!, errno = 0x%x\n", __func__, s32RetVal);
				#endif
				
				return SWHAL_ERR_SYSINIT;
		}

		/* ================================================================== */
		/* rtl8367 switch ioctl init																					*/
		/* ================================================================== */		
    if(RTK_NULL == (pRetPtr=rtl8367_ioctl_wrapper_init())) 
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
				printk(KERN_ERR "[%s] rtl8367 ioctl init-process failed!!!, errno = 0x%x\n", __func__, s32RetVal);
				#endif

				return SWHAL_ERR_IOCINIT;
    }

		gpstIocWrapperCtx->pEthSWDev[0] = gpstRtl8367SwState;

		return SWHAL_ERR_SUCCESS;		
}

int rtl8367_dev_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
		RTL8367_IOC_WRAPPER_CTX *pstIocWrapCtx = gpstIocWrapperCtx;

		/* process /dev/rtl_switch */
		//char *path = "/dev/rtl_switch";
		//char *needle = "/";
    //char *buf = strstr(path, needle);

		//printk(KERN_INFO "[%s] gu8Rtl8367IocAPI = %d\n", __func__, gu8Rtl8367IocAPI);

		if(gu8Rtl8367IocAPI == RTL8367_API_USERAPP)
		{
				if(!iminor(inode))
				{
						pstIocWrapCtx->pstIoctlHandle->pLlHandle = pstIocWrapCtx->pEthSWDev[0];
				}
				else
				{
						/* error handler */
				}
		}
		else
		{
    		//path = buf+strlen(needle); 		/* pointer to dev */
    		//buf  = strstr(path, needle);
    		//path = buf+strlen(needle); 		/* pointer to rtl_switch */

				pstIocWrapCtx->pstIoctlHandle->pLlHandle = pstIocWrapCtx->pEthSWDev[0];
		}
		
		//printk(KERN_INFO "[%s:%d]: pEthSWDev[0] = 0x%x, 0x%x\n", __func__, __LINE__, pstIocWrapCtx->pstIoctlHandle->pLlHandle, pstIocWrapCtx->pEthSWDev[0]);

		return rtl8367_ioctl_lowlevelcommandsearch(pstIocWrapCtx->pstIoctlHandle, cmd, arg, gu8Rtl8367IocAPI);
}

static int rtl8367_dev_open(struct inode *inode, struct file *filp)
{
		RTL8367_DEV_STATE *pstDevState = gpstRtl8367DevState;
		
		pstDevState = container_of(inode->i_cdev, RTL8367_DEV_STATE, cdev);
		filp->private_data = pstDevState; /* for other methods */

		/* Do others 2ed-init process */
		
		return 0;
}


static int rtl8367_dev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
		return 0;
}

static int rtl8367_dev_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
		return 0;
}


static int rtl8367_dev_release(struct inode *inode, struct file *filp)
{	
		return 0;
}

static int rtl8367_dev_init(void)
{
		int s32RetVal;
		dev_t DevNo = 0;

		RTL8367_DEV_STATE *pstDevState;

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_MODULE_INIT)) == (DBG_KERNEL_SPACE|DBG_MODULE_INIT))
		printk(KERN_INFO "create device notes for %s\n", RTK_DEV_NAME);
		#endif
		
		/* 
		*  Get a range of minor numbers to work with, asking for a dynamic 
		*  major unless directed otherwise at load time. 
		*/	
		if(RTK_MAJOR_DYANMIC)
		{		
				DevNo = MKDEV(RTK_MAJOR_DYANMIC, RTK_MINOR_DYANMIC);		
				if(DevNo > 0)
				{
						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_MODULE_INIT)) == (DBG_KERNEL_SPACE|DBG_MODULE_INIT))
						printk(KERN_INFO "try to register rtl8367 char dev using major no. = %d, minor no. = %d, notes = 0x%x\n", RTK_MAJOR_DYANMIC, RTK_MINOR_DYANMIC, DevNo); 
						#endif

						/* register useing static major no. */
						s32RetVal = register_chrdev_region(DevNo, 1, RTK_DEV_NAME);	
						if(s32RetVal < 0)
						{
								#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
								printk(KERN_INFO "[%s] register rtl8367 char dev with major no. %d failed!!! continue trying with dynamic register\n", __func__, RTK_MAJOR_DYANMIC); 
								#endif

								/* register useing dynamic major no. */
								s32RetVal = alloc_chrdev_region(&DevNo, RTK_MINOR_DYANMIC, 1, RTK_DEV_NAME);
								if(s32RetVal < 0)  
								{
										#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
										printk(KERN_INFO "[%s] register dynamic rtl8367 char dev failed!!!\n", __func__); 
										#endif
										
										return s32RetVal;
								}
								else
								{
										RTK_MAJOR_DYANMIC = MAJOR(DevNo);
										RTK_MINOR_DYANMIC = MINOR(DevNo);

										#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_MODULE_INIT)) == (DBG_KERNEL_SPACE|DBG_MODULE_INIT))
										printk(KERN_INFO "dynamic rtl8367 char dev major : %d, minor : %d\n", RTK_MAJOR_DYANMIC, RTK_MINOR_DYANMIC); 
										#endif
								}
						}
				}
				else
				{
						/* register useing dynamic major no. */
						s32RetVal = alloc_chrdev_region(&DevNo, RTK_MINOR_DYANMIC, 1, RTK_DEV_NAME);		
						if(s32RetVal < 0)  
						{
								#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
								printk(KERN_INFO "[%s] register dynamic rtl8367 char dev failed!!!\n", __func__); 
								#endif
								
								return s32RetVal;
						}
						else
						{
								RTK_MAJOR_DYANMIC = MAJOR(DevNo);
								RTK_MINOR_DYANMIC = MINOR(DevNo);
								
								#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_MODULE_INIT)) == (DBG_KERNEL_SPACE|DBG_MODULE_INIT))
								printk(KERN_INFO "dynamic rtl8367 char dev major : %d, minor : %d\n", RTK_MAJOR_DYANMIC, RTK_MINOR_DYANMIC); 
								#endif
						}
				}
		} 
		else 
		{		
				s32RetVal = alloc_chrdev_region(&DevNo, RTK_MINOR_DYANMIC, 1, RTK_DEV_NAME);		
				if(s32RetVal < 0)  
				{
						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
						printk(KERN_INFO "[%s] register dynamic rtl8367 char dev failed!!!\n", __func__); 
						#endif
						
						return s32RetVal;
				}
				else
				{
						RTK_MAJOR_DYANMIC = MAJOR(DevNo);
						RTK_MINOR_DYANMIC = MINOR(DevNo);

						#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_MODULE_INIT)) == (DBG_KERNEL_SPACE|DBG_MODULE_INIT))
						printk(KERN_INFO "dynamic rtl8367 char dev major : %d, minor : %d\n", RTK_MAJOR_DYANMIC, RTK_MINOR_DYANMIC); 
						#endif
				}	
		}

		pstDevState = (RTL8367_DEV_STATE *)kmalloc(sizeof(RTL8367_DEV_STATE),GFP_KERNEL);
		if(!pstDevState) 
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
    		printk(KERN_INFO "[%s] memory allocation failed!!!\n", __func__);
				#endif

				kfree(pstDevState);

				return -ENOMEM;
    }

		memset(pstDevState, 0, sizeof(RTL8367_DEV_STATE));

		gpstRtl8367DevState = pstDevState;
		
		/* Set up the char_dev structure for this device. */
		cdev_init(&gpstRtl8367DevState->cdev, &rtl8367_fops);	
		gpstRtl8367DevState->cdev.owner 	= THIS_MODULE;	
		gpstRtl8367DevState->cdev.ops 		= &rtl8367_fops;	
		s32RetVal = cdev_add(&gpstRtl8367DevState->cdev, DevNo, 1);	
		if(s32RetVal)	
		{
				#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & (DBG_KERNEL_SPACE|DBG_ERROR)) == (DBG_KERNEL_SPACE|DBG_ERROR))	
				printk(KERN_INFO "Add %s failed!!!, errno = 0x%x\n", RTK_DEV_NAME, s32RetVal);
				#endif
				
				rtl8367_dev_cleanup();
				return -1;	
		}


		#if defined(PROC_DEBUG)
		rtl8367_proc_create();
		#endif

		//printk(KERN_INFO "register char dev success major = %d minor = %d \n", RTK_MAJOR_DYANMIC, RTK_MINOR_DYANMIC); 
		//printk(KERN_INFO "%sRTK_SWAPI: Registered character device [%s] with major no [%d]%s\n", SWCLR1_33_YELLOW ,RTK_DEV_NAME, RTK_MAJOR_DYANMIC, SWCLR0_RESET);
		printk(KERN_INFO "RTK_SWAPI: Registered character device [%s] with major no [%d]\n", RTK_DEV_NAME, RTK_MAJOR_DYANMIC);

		return s32RetVal;	

}

static int rtl8367_dev_cleanup(void)
{
		RTL8367_DEV_STATE *gpstDevState = gpstRtl8367DevState;
		dev_t DevNo = MKDEV(RTK_MAJOR_DYANMIC, RTK_MINOR_DYANMIC);	

		#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & DBG_MODULE_INIT) == DBG_MODULE_INIT)
		//printk(KERN_INFO "%srtl8367 switch driver register failed!!!%s\n", SWCLR1_31_RED, SWCLR0_RESET); 
		#endif
		
		#if(PROC_DEBUG == 1)
		rtl8367_proc_destroy();
		#endif

		cdev_del(&gpstDevState->cdev);

		/* dev_cleanup is never called if registering failed */
		unregister_chrdev_region(DevNo, 1);

		if(gpstDevState != RTK_NULL)
		{
				kfree(gpstDevState);
				gpstDevState = RTK_NULL;
		}
		
		return 0;
}

#if defined(PROC_DEBUG)
static int rtl8367_proc_read(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
		return 0;
}

static void rtl8367_proc_write_help( void )
{
	printk(	"\nRTL8367RB Giga-Ethernet switch control:\n"
			"        help                           - show this help message\n"
			"        restart                        - reset and re-initialize switch\n"
			"        smi-read <from-addr> [to-addr] - read  switch register data via SMI/MDIO bus\n"
			"        smi-write <addr> <val>         - write switch register data\n"
			"  <addr> : SMI register address\n"
			"  <val>  : SMI register value to write\n"
		  );
}

static int rtl8367_proc_write(struct file *file, const char *buf, unsigned long count, void *data)
{
		char*	pPtr;
		unsigned int iTmp;
		unsigned short iCnt, iFrom, iTo;
		int	iCnt2;
		char sBuf[128];

		int nPortId;
		int nPhyTestMode;

		sBuf[0] = sBuf[sizeof(sBuf)-1] = '\0';

		/* trim the tailing space, tab and LF/CR*/
		if(count > 0)
		{
				if(count >= sizeof(sBuf))
					count = sizeof(sBuf) - 1;

				if(copy_from_user(sBuf, buf, count))
					return count;

				pPtr = (char*)sBuf + count - 1;

				for (; *pPtr==' ' || *pPtr=='\t' || *pPtr=='\n' || *pPtr=='\r'; pPtr++)
				{
					*pPtr = '\0';
				}
		}

		/* restart */
		if(strnicmp(sBuf, "restart", sizeof("restart")-1) == 0)
		{
				rtl8367_switch_restart();
				return count;
		}

		/* smi-read */
		if(strnicmp(sBuf, "smi-read", sizeof("smi-read")-1) == 0 )
		{
				for (pPtr=sBuf+sizeof("smi-read"); *pPtr==' ' || *pPtr=='\t'; pPtr++) { }

				/* <from-Addr> */
				for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }

				if(*pPtr == '\0') 
				{
						rtl8367_proc_write_help();
						return count;
				}
				
		    iTmp = (int)simple_strtol(pPtr, &pPtr, 0);
				
				if(iTmp < 0 || iTmp > 0xFFFF) 
				{
						rtl8367_proc_write_help();
						return count;
				}

				iFrom = (u16)iTmp;

				/* [to-Addr] */
				for(; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
				if(*pPtr != '\0') 
				{
			    	iTmp = (int)simple_strtol(pPtr, &pPtr, 0);

						if(iTmp < 0 || iTmp > 0xFFFF) 
						{
								rtl8367_proc_write_help();
								return count;
						}

						iTo = (u16)iTmp;

						if(iTo < iFrom) 
						{
								iTo = iFrom;
								iFrom = (u16)iTmp;
						}
				}
				else 
				{
						iTo = iFrom;
				}
				
				/* print */
				if(iTo == iFrom) 
				{
						iTmp = RTL83XX_SMI_READ(iFrom);
						printk( "0x%04x (", iTmp );

						for( iCnt2 = 15; iCnt2 >= 0; iCnt2-- ) 
						{
								printk( "%s", ( iTmp & (0x1<<iCnt2) ) ? "1" : "0" );

								if((iCnt2 % 4 ) == 0 && iCnt2 != 0)
								printk( "." );
						}

						printk( ")\n" );
				} 
				else 
				{
						printk( "  addr   value          binary\n" );
						printk( " ======  ======  ===================\n" );
						
						for(iCnt = iFrom; iCnt <= iTo; iCnt++) 
						{
								iTmp = RTL83XX_SMI_READ( iCnt );
								printk( " 0x%04x  0x%04x  ", iCnt, iTmp );

								for(iCnt2 = 15; iCnt2 >= 0; iCnt2--) 
								{
										printk( "%s", ( iTmp & (0x1<<iCnt2) ) ? "1" : "0" );

										if((iCnt2 % 4 ) == 0 && iCnt2 != 0)
											printk( "." );
								}

								printk( "\n" );
						}
				}

				return count;
		}

		/* smi-write */
		if(strnicmp( sBuf, "smi-write", sizeof("smi-write")-1 ) == 0)
		{
				for(pPtr=sBuf+sizeof("smi-write"); *pPtr==' ' || *pPtr=='\t'; pPtr++) { }

				/* <Addr> */
				for(; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }

				if(*pPtr == '\0') 
				{
						rtl8367_proc_write_help();
						return count;
				}

				iTmp = (int)simple_strtol(pPtr, &pPtr, 0);

				if(iTmp < 0 || iTmp > 0xFFFF) 
				{
						rtl8367_proc_write_help();
						return count;
				}
				
				iFrom = (u16)iTmp;

				/* <val> */
				for(; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }

				if(*pPtr == '\0') 
				{
						rtl8367_proc_write_help();
						return count;
				}

				iTmp = (int)simple_strtol(pPtr, &pPtr, 0);

				if(iTmp < 0 || iTmp > 0xFFFF) 
				{
						rtl8367_proc_write_help();
						return count;
				}
				
				/* write */
				RTL83XX_SMI_WRITE( iFrom, (u16)iTmp );

				return count;
		}

		/* PhyTest */
		if(strnicmp( sBuf, "PhyTest", sizeof("PhyTest")-1 ) == 0)
		{
				for (pPtr=sBuf+sizeof("PhyTest"); *pPtr==' ' || *pPtr=='\t'; pPtr++) { }

				/* <Port Id> */
				for (; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }

				if(*pPtr == '\0') 
				{
						rtl8367_proc_write_help();
						return count;
				}
				
	    	iTmp = (int)simple_strtol(pPtr, &pPtr, 0);

				if(iTmp < 0 || iTmp > 7) 
				{
						rtl8367_proc_write_help();
						return count;
				}
				
				nPortId = (u16)iTmp;

				/* <PHY test mode> */
				for(; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
				if(*pPtr == '\0') 
				{
						rtl8367_proc_write_help();
						return count;
				}
				
	    	iTmp = (int)simple_strtol(pPtr, &pPtr, 0);

				if(iTmp < 0 || iTmp > 0xFFFF) 
				{
						rtl8367_proc_write_help();
						return count;
				}

				nPhyTestMode = iTmp;

				printk("[PhyTest] Set PHY %d, Mode %d\n", nPortId, nPhyTestMode);
				//RTL83XX_HalComm_PhyTestMode_Cfg(nPortId, nPhyTestMode);

				return count;
		}

		/* PhyNormal */
		if(strnicmp(sBuf, "PhyNormal", sizeof("PhyNormal")-1 ) == 0)
		{
				for (pPtr=sBuf+sizeof("PhyNormal"); *pPtr==' ' || *pPtr=='\t'; pPtr++) { }

				/* <Port Id> */
				for(; *pPtr==' ' || *pPtr=='\t'; pPtr++) { }
				if(*pPtr == '\0') 
				{
						rtl8367_proc_write_help();
						return count;
				}
				
	    	iTmp = (int)simple_strtol(pPtr, &pPtr, 0);

				if(iTmp < 0 || iTmp > 7) 
				{
						rtl8367_proc_write_help();
						return count;
				}
				
				nPortId = (u16)iTmp;

				printk("[PhyNormal] Set PHY %d to normal Mode\n", nPortId);
				//RTL83XX_HalComm_PhyTestMode_Cfg(nPortId,0);

				return count;
		}

		/* unknown */
		rtl8367_proc_write_help();

		return count;

}

static int rtl8367_proc_create(void)
{
		static struct proc_dir_entry*	pstProcEntry;	
		#if 0
		unsigned int u32ProcMode;
		
		u32ProcMode = ((S_IRUSR|S_IWUSR|S_IXUSR) |\
									 (S_IRGRP|S_IWGRP|S_IXGRP) |\
									 (S_IROTH|S_IWOTH|S_IXOTH));

		proc_root_rtk_driver = proc_mkdir(gs8ProcPath, 0);
		proc_create("driver/rtk/", u32ProcMode, NULL, &rtl8367_proc_fops);
		pstProcEntry = create_proc_entry(strcat(gs8ProcPath,RTK_CHIP_NAME_STRING), u32ProcMode, NULL);	
		pstProcEntry = create_proc_entry("driver/rtk/rtl8367rb", u32ProcMode, NULL);	
		#endif
		
		pstProcEntry = create_proc_entry("driver/rtl8367", 0666, NULL);
		if(pstProcEntry == NULL) 	
		{		
				#if((DBG_SWITCH_LEVEL & DBG_PROCFS_CREATE) == DBG_PROCFS_CREATE) 
				printk(KERN_INFO "create rtl8367 switch proc file failed!!!\n");		
				#endif
				
				return -1;	
		}
		#if 0
		else
		{
				pstProcEntry->proc_fops = &rtl8367_proc_fops;
		}
		#endif
		
		pstProcEntry->write_proc = rtl8367_proc_write;	
		pstProcEntry->read_proc  = rtl8367_proc_read;	

		return 0;
}

static int rtl8367_proc_destroy(void)
{
		/* no problem if it was not registered */
    remove_proc_entry("driver/rtl8367", NULL);

		return 0;
}
#endif // PROC_DEBUG


static int __init rtl8367_switch_module_init(void)
{
		rtl8367_switch_init();
		rtl8367_dev_init();

		//#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & DBG_MODULE_INIT) == DBG_MODULE_INIT)
		printk(KERN_INFO "%s V%s init success!!! release at %s\n", RTK_DEV_NAME, RTK_SWITCH_DRV_MODULE_VERSION, RTK_RELDATE);
		//#endif

		return 0;
}

static void __exit rtl8367_switch_module_exit(void)
{
		//#if defined(RTL83XX_HAL_LINUX_DEBUG) && ((DBG_SWITCH_LEVEL & DBG_MODULE_INIT) == DBG_MODULE_INIT)	
		printk("%s %s exit\n", RTK_DEV_NAME, RTK_SWITCH_DRV_MODULE_VERSION);
		//#endif

		rtl8367_switch_corecleanup();
		rtl8367_ioctl_wrapper_cleanup();
		rtl8367_dev_cleanup();
}

EXPORT_SYMBOL(rtl8367_dev_ioctl);

module_init(rtl8367_switch_module_init);
module_exit(rtl8367_switch_module_exit);

MODULE_AUTHOR("Alvin Hsu <alvin_hsu@arcadyan.com.tw>");
MODULE_DESCRIPTION("RealTek RTL8367RB Gigabit-Switch chipset driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(RTK_SWITCH_DRV_MODULE_VERSION);

