

/*! \file ethercatslave.h
    \brief generic include for the EtherCAT slave
*/



#ifndef __ETHERCATSLAVE_H
 #define __ETHERCATSLAVE_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>          //sb
#include <fcntl.h>
#include <bcm2835.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <termios.h>  //sb
#include <random>
#include <sys/ioctl.h>
#include <pthread.h>
#include <linux/gpio.h>
#include <sys/stat.h>   //sb
#include <semaphore.h>  
#include <sys/mman.h>
#include <sys/resource.h>

#ifdef __KERNEL__
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/interrupt.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#endif



#include <rt_utilities/rt_utilities.h>
#include <rpi_base/rpi_base.h>
#include <shield_9252/shield_9252.h>
#include <gpioirq/gpioirq.h>
#include <thread/thread.h>













#endif