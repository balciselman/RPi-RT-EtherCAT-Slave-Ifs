


#ifndef __GPIOIRQ_H
 #define __GPIOIRQ_H


#ifdef __KERNEL__


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h> 
#include <linux/timekeeping.h>


#endif

//#include <semaphore.h>
//#include <sys/stat.h>


#define DRIVER_AUTHOR                "Selman Balci"
#define DRIVER_DESC                  "GPIO Irq Driver"
#define DEVICE_NAME                  "gpioirq"

/// Interrupt number for the LAN9252 Microchip
#define GPIO_ANY_GPIO                25

/// text below will be seen in 'cat /proc/interrupts' command
#define GPIO_ANY_GPIO_DESC           "Sync Test Irq"
#define GPIO_ANY_GPIO_DEVICE_DESC    "Raspberry Pi 4B"


#define MYMAJOR                      90
#define IRQMOD_BASE                  40
#define	IRQMOD_IOC_MAGIC	           'B'

#define	IRQMODDEV_WAIT_IRQ       	   (IRQMOD_BASE+1)
#define	IRQMODDEV_DOWN_TIMEOUT       (IRQMOD_BASE+2)

#define	IRQMOD_WAIT_IRQ        	     _IO(IRQMOD_IOC_MAGIC,IRQMODDEV_WAIT_IRQ)
#define	IRQMOD_DOWN_TIMEOUT    	     _IOW(IRQMOD_IOC_MAGIC,IRQMODDEV_DOWN_TIMEOUT,int)         // IOW ---> Userland is writing, kernel is reading.


#ifdef __KERNEL__

static inline int get_arginput(void *arg,void *dest,size_t size)
{
    // copy input
    // native linux mode
    int err;
    err =  copy_from_user(dest,(const void __user *)arg, size);

    if  (  err !=0)
    {
      printk(KERN_INFO "GPIOIRQ: copy_from_user returns err=%d \n",err);
      return -EFAULT;
    }
    return 0;
}
#endif

#if defined(KERNEL_SPACE)

static long driver_unioctl (struct file *file, unsigned int cmd, unsigned long arg);
static  __inline int down_timeoutms(struct semaphore *sem,long timeout_ms);

#endif



#endif