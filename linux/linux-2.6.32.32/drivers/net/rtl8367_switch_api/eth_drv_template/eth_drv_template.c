#include <linux/module.h>
//#include <linux/config.h> change to autoconf.h
#include <linux/autoconf.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <asm/io.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

typedef struct
{
 	void **data;
 	struct cdev *cdev ;
  
}DBG_Dev_Tag;

#define DBG_MAJOR         	111
#define DBG_MINOR          	1
#define MAXDEVICENUM 				1

#define DEBUG_IOC_MAGIC 		'D'
#define DEBUG_IOCSHOWDEB    _IOR(DEBUG_IOC_MAGIC,   1, int)

DBG_Dev_Tag *DBG_devices;

int DBG_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
ssize_t DBG_read (struct file *filp, char *buf, size_t count, loff_t *offset);
ssize_t DBG_write (struct file *file, const char *buf, size_t count, loff_t *offset);
static int DBG_open(struct inode *inode, struct file *filp);
static int DBG_release(struct inode *inode, struct file *filp);


int dbg_major = DBG_MAJOR;
int dbg_minor;

static struct file_operations DBG_fops =
{
		.ioctl   = DBG_ioctl,
		.read    = DBG_read,
		.write   = DBG_write,
		.open    = DBG_open,
		.release = DBG_release,
};

int DBG_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
#if 0
     struct net_device *dev = &_wpcm450_netdevice;
  
     switch (cmd)
     {
         case DEBUG_IOCSHOWDEB:
                 //show something
              
                     break;
                default:
                     break;
     }
#endif

    return 0;
}

ssize_t DBG_read (struct file *filp, char *buf, size_t count, loff_t *offset)
{
 		return 0;
}

ssize_t DBG_write (struct file *file, const char *buf, size_t count, loff_t *offset)
{
 		return 0;
}

static int DBG_open(struct inode *inode, struct file *filp)
{
#if 0
     DBG_Dev_Tag *dev;
  
     dev = container_of (inode->i_cdev,DBG_Dev_Tag,cdev);
  
     filp-> private_data = dev;
#endif

 		return 0;
}

static int DBG_release(struct inode *inode, struct file *filp)
{
 		return 0;
}



static void dbg_setup_cdev(DBG_Dev_Tag *dev,int index)
{
      int err;
      int dev_id = MKDEV(dbg_major,dbg_minor + index );
   
      cdev_init(dev->cdev,&DBG_fops);
   
      dev->cdev->owner = THIS_MODULE;
   
      dev->cdev->ops = &DBG_fops;
   
      err = cdev_add(dev->cdev,dev_id,1);
   
      if (err)
              printk ("error %x adding dbg%x\n",err,index);
   
}


static int __init DBG_init(void)
{
		int result;
		dev_t dev_id;


		printk("In %s\n", __func__); 

		DBG_devices = kmalloc (sizeof (DBG_Dev_Tag) * MAXDEVICENUM ,GFP_KERNEL);

		if (DBG_devices == NULL)
		{
		          result = -ENOMEM;
		          return result;
		}

		memset(DBG_devices, 0, MAXDEVICENUM * sizeof(DBG_Dev_Tag));

		if (dbg_major)
		{
		     dev_id = MKDEV(dbg_major,dbg_minor);
		     result = register_chrdev_region(dev_id, 1, "debug");
		}
		else
		{
		     result = alloc_chrdev_region(&dev_id, DBG_MINOR, 1, "dbg");
		     dbg_major = MAJOR(dev_id);
		}

		if(result < 0 )
		{
         printk ("can't get major %d\n",dbg_major);
         return result;
		}
  
		DBG_devices->cdev = cdev_alloc();

		dbg_setup_cdev(DBG_devices,1);

 		return 0;     
}

static void __exit DBG_exit(void)
{
		dev_t dev_id;

		kfree(DBG_devices);

		cdev_del(DBG_devices ->cdev);

		dev_id = MKDEV(dbg_major,dbg_minor);
		unregister_chrdev_region(dev_id,1);
		
		return;
}

MODULE_AUTHOR("XXXXXXXXXX");
MODULE_DESCRIPTION("Debug driver");
MODULE_LICENSE("GPL");

module_init(DBG_init);
module_exit(DBG_exit);
