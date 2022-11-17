#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include <time.h>
#include<sys/ioctl.h>
#include <pthread.h>
#include <linux/gpio.h>

#include "/projects/etherslave/etherberry_9252/include/gpioirq/gpioirq.h"





int main(){

    char devnamebuf[256];

    devnamebuf[sizeof(devnamebuf)-1]=0;
    snprintf(&devnamebuf[0],sizeof(devnamebuf)-1,"/%s/%s","dev",DEVICE_NAME);
    printf("try to open device %s ...\n",&devnamebuf[0]);

    int dev = open(&devnamebuf[0], O_RDWR);
    if (dev == -1)
    {
        printf("Opening was not possible!\n");
        return -1;
    }
    printf("Openning was successful!\n");
    // close(dev);
    // return 0;

    unsigned long long i = 0;
    
    int err;
    int timems;

    
    while(true)
    {   
        //err = ioctl(dev,IRQMODDEV_DOWN_TIMEOUT);
        timems = 100;
        printf("calling ioctl(%d,%ld,%d) ...\n",dev,IRQMOD_DOWN_TIMEOUT,timems);
        fflush(stdout);
        
        err = ioctl(dev,IRQMOD_DOWN_TIMEOUT,timems);

        if(err==0){
            printf("enter irq-wait ...\n");
            ioctl(dev,IRQMODDEV_WAIT_IRQ);
            i++;
            printf("return from irq-wait, interrupt number= %llu\n",i);
           
        }
        usleep(10000);
    }


    return 0;
}