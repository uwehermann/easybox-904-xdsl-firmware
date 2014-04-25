#include <linux/init.h>   
#include <linux/module.h>   
#include <linux/types.h>   
#include <linux/fs.h>   
  
#define SIMPLE_DEBUG 							1   
#define DEV_COUNT 								2   
#define SIMPLE_NAME 							"simple_char"   
  
static int simple_major = 100;   
static int simple_minor = 0;   
  
static __init int simple_init(void)   
{   
    dev_t dev;   
    int err;   
#if SIMPLE_DEBUG   
    printk(KERN_INFO "In %s\n", __func__);   
#endif   

    dev = MKDEV(simple_major, simple_minor);  
    if(dev > 0)
    {   

#if SIMPLE_DEBUG   
        //printk(KERN_INFO "try to register static char dev  %ld \n", dev);   
#endif   
        err = register_chrdev_region(dev,DEV_COUNT, SIMPLE_NAME);  
				if(err < 0)  
        {   
            printk(KERN_WARNING "register static char dev error\n");   
            err = alloc_chrdev_region(&dev, 0, DEV_COUNT, SIMPLE_NAME); 
 

            if(err < 0)   
            {   
                printk(KERN_ERR "register char dev error in line %d\n",__LINE__);   

                goto error;   
            }   
            else  
            {   
                simple_major = MAJOR(dev);
                simple_minor = MINOR(dev);
            }   
               
        }   
        else{   
           
        }   
    }   
    else 
    {    
#if SIMPLE_DEBUG   
        printk(KERN_INFO "try to register alloc char dev  \n");   
#endif   
        err = alloc_chrdev_region(&dev, 0, DEV_COUNT, SIMPLE_NAME);   
        if(err < 0)   
        {   
            printk(KERN_ERR "register char dev error in line %d\n\n",__LINE__);   

            goto error;   
        }   
        else  
        {   
            simple_major = MAJOR(dev);   
            simple_minor = MINOR(dev);   
        }   
  
    }   
  
#if SIMPLE_DEBUG   
    printk(KERN_INFO "register char dev success major = %d minor = %d \n", simple_major, simple_minor);   

#endif   
  
error:   
    return err;   
}   
  
static __exit void simple_exit(void)   
{   
    dev_t dev;   
#if SIMPLE_DEBUG   
    printk(KERN_INFO "In %s\n", __func__);   
#endif   
    dev = MKDEV(simple_major, simple_minor);   
    unregister_chrdev_region(dev, DEV_COUNT); 
}   
  
module_init(simple_init);   
module_exit(simple_exit);   
  
MODULE_LICENSE("GPL");   
MODULE_AUTHOR("kai_zhang(jsha.zk@163.com)");   
MODULE_DESCRIPTION("simple char driver!");  
