/*****************************************************************
*	Test Program
*****************************************************************/
/* Files */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#ifdef __KERNEL__
#include <linux/ioctl.h> 									/* needed for the _IOW etc stuff used later */
#endif
#include <linux/kernel.h> 								/* printk() 			*/ 
#include <linux/slab.h>										/* kmalloc() 			*/
#include <linux/fs.h>											/* everything... 	*/
#include <linux/cdev.h>
#include <linux/errno.h>									/* error codes 		*/
#include <linux/stat.h>

#include <asm/uaccess.h>

#include "switch_vendor_error.h"					/* local definitions */

#include "rtl83XX_hal.h"									/* local definitions */
#include "rtl8367_hal_linux_ioctl.h"			/* local definitions */

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

/* Function Declaraction */
int rtl8367_ioctl_test_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
ssize_t rtl8367_ioctl_test_read (struct file *filp, char *buf, size_t count, loff_t *offset);
ssize_t rtl8367_ioctl_test_write (struct file *file, const char *buf, size_t count, loff_t *offset);
static int rtl8367_ioctl_test_open(struct inode *inode, struct file *filp);
static int rtl8367_ioctl_test_release(struct inode *inode, struct file *filp);

extern int Util_ConvertIpAddr_CharStr(char *ip_adr_str, unsigned int *ip_adr_ptr);
extern int rtl8367_dev_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

/* Function Parameters */
unsigned int IOCmd=0;
module_param(IOCmd, int, S_IRUGO|S_IWUSR);

void *gpCtlParams=NULL;
RTL8367_IOCTL_PARAMS gunIOCTLParams;
rtk_l2_addr_table_t gstL2Entry;

unsigned long	gu64RtkIoctlCmdArg[8]={};     /* max. 8 command arguments (integer/long/string) */

#define DBG_IOC_MAJOR         	111
#define DBG_IOC_MINOR          	1
#define MAXDEVICENUM 						1

int gs32DbgIOCMajor = DBG_IOC_MAJOR;
int gs32DbgIOCMinor;

typedef struct
{
 	void **data;
 	struct cdev *cdev ;
  
}rtl8367_ioctl_test_dev_tag;

rtl8367_ioctl_test_dev_tag *rtl8367_ioctl_test_devices;

static struct file_operations rtl8367_ioctl_test_fops =
{
		.ioctl   = rtl8367_ioctl_test_ioctl,
		.read    = rtl8367_ioctl_test_read,
		.write   = rtl8367_ioctl_test_write,
		.open    = rtl8367_ioctl_test_open,
		.release = rtl8367_ioctl_test_release,
};


/* IOCTL Parameters */
#define test_ioc_switch_init	0



int rtl8367_ioctl_test_switch_init(void)
{
		int s32InitStatus;
		struct inode *inode; 
		struct file *filp;
		
		rtl8367_dev_ioctl(inode, filp, RTL8367_IOCTL_INIT, (unsigned long)&s32InitStatus);
		
		return SWHAL_ERR_SUCCESS;
}


int rtl8367_ioctl_test_help(void)
{
		printk(KERN_INFO "[%s]\n", __func__);

		printk(KERN_INFO " 0 : test_ioc_switch_init\n");


		
		return SWHAL_ERR_SUCCESS;
}

int rtl8367_ioctl_test_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    return 0;
}

ssize_t rtl8367_ioctl_test_read (struct file *filp, char *buf, size_t count, loff_t *offset)
{
 		return 0;
}

ssize_t rtl8367_ioctl_test_write (struct file *file, const char *buf, size_t count, loff_t *offset)
{
 		return 0;
}

static int rtl8367_ioctl_test_open(struct inode *inode, struct file *filp)
{
 		return 0;
}

static int rtl8367_ioctl_test_release(struct inode *inode, struct file *filp)
{
 		return 0;
}

static void rtl8367_ioctl_test_setup_cdev(rtl8367_ioctl_test_dev_tag *dev, int index)
{
    int err;
    int dev_id = MKDEV(gs32DbgIOCMajor,gs32DbgIOCMinor + index );
 
    cdev_init(dev->cdev,&rtl8367_ioctl_test_fops);
 
    dev->cdev->owner = THIS_MODULE;
 
    dev->cdev->ops = &rtl8367_ioctl_test_fops;
 
    err = cdev_add(dev->cdev,dev_id,1);
 
    if (err)
            printk("error %x adding dbg%x\n",err,index);
   
}

static int __init rtl8367_ioctl_test_module_init(void)
{
		int result;
		dev_t dev_id;

		printk(KERN_INFO "[%s] inser module\n", __func__);

		/* Register Char Devices */
		if(gs32DbgIOCMajor)
		{
		     dev_id = MKDEV(gs32DbgIOCMajor,gs32DbgIOCMinor);
		     result = register_chrdev_region(dev_id, 1, "rtl8367_ioctl_debug");
		}
		else
		{
		     result = alloc_chrdev_region(&dev_id, DBG_IOC_MINOR, 1, "rtl8367_ioctl_debug");
		     gs32DbgIOCMajor = MAJOR(dev_id);
		}

		if(result < 0)
		{
         printk("can't get major %d\n",gs32DbgIOCMajor);
         return result;
		}
  
		rtl8367_ioctl_test_devices->cdev = cdev_alloc();

		rtl8367_ioctl_test_setup_cdev(rtl8367_ioctl_test_devices, 1);


		/* Check IOCTL Command */
		switch(IOCmd)
		{
				case test_ioc_switch_init:
						rtl8367_ioctl_test_switch_init();
						break;


				default: 
						rtl8367_ioctl_test_help();  
						break;
		}

		return 0;
}

static void __exit rtl8367_ioctl_test_module_exit(void)
{
		printk(KERN_INFO "[%s] remove module\n", __func__);
		return;
}

module_init(rtl8367_ioctl_test_module_init);
module_exit(rtl8367_ioctl_test_module_exit);

MODULE_AUTHOR("RealTek");
MODULE_DESCRIPTION("RealTek switch api test module");
MODULE_LICENSE("GPL");

