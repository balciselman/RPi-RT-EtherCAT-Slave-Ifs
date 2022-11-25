/**********************************************************************************/
/*                                                                                */
/* gpioirq.c is a Kernel mode driver for GPIO interrupts in Raspberry Pi          */
/*                                                                                */
/**********************************************************************************/
/* This program is free software; you can redistribute it and/or modify it        */
/* under the terms of the GNU General Public License as published by the Free     */
/* Software Foundation; either version 2 of the License, or (at your option)      */
/* any later version.                                                             */
/*                                                                                */
/* This program is distributed in the hope that it will be useful, but WITHOUT    */
/* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or          */
/* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for      */
/* more details.                                                                  */
/*                                                                                */
/* You should have received a copy of the GNU General Public License along with   */
/* this program; if not, see <http://www.gnu.org/licenses/>.                      */
/**********************************************************************************/
/* Author:                                                                        */
/* S.Balci Moehwald GmbH - 2022                                                   */
/* Supervised by:                                                                 */
/* B.Benner Moehwald GmbH                                                         */
/* based on m_test.c by Igor <hardware.coder@gmail.com>                           */ 
/**********************************************************************************/


#define KERNEL_SPACE
#include "../include/gpioirq/gpioirq.h" 

/// Global Interrupt number initialization
short int irq_any_gpio    = 0;

/// context struct to use in the functions
typedef struct _MYCONTEXT {
    int wOpen;
    int rOpen;
    int debuglevel;
    int irq_number;
    char devicename[64];
    struct semaphore irq_lsigsema;             // linux semaphore array
} MYCONTEXT;


/// Interrupt handler which fired when interrupt is received
static irqreturn_t r_irq_handler (int irq,  void *data) {

   unsigned long flags;
   MYCONTEXT *md;

   // disable hard interrupts (remember them in flag 'flags')
   local_irq_save(flags);

   md = (MYCONTEXT *)(data);

   up(&md->irq_lsigsema);

   /// restore hard interrupts
   local_irq_restore(flags);

   return IRQ_HANDLED;
}


/// Function for interrupt configuration
static void r_int_config(MYCONTEXT* md) {

   if (gpio_request(GPIO_ANY_GPIO, GPIO_ANY_GPIO_DESC)) {
      printk("GPIO request failure: %s\n", GPIO_ANY_GPIO_DESC);
      return;
   }
   
   if ( (md->irq_number = gpio_to_irq(GPIO_ANY_GPIO)) < 0 ) {
      printk("GPIO to IRQ mapping failure %s\n", GPIO_ANY_GPIO_DESC);
      return;
   }

   printk("Mapped int %d\n", md->irq_number);

   return;
}


/// To release interrupts at the end
static void r_int_release(MYCONTEXT* md) {

   free_irq(md->irq_number, md);
   gpio_free(GPIO_ANY_GPIO);

   return;
}


/// Activates the interrupt with different orders that comes from user space
static int my_api(unsigned int cmd, MYCONTEXT* md,unsigned long arg)
{
  int err;
  int stat_xmode;
  

  stat_xmode = 0;
  err = -EFAULT;
  switch (_IOC_NR(cmd))
  {
  default:
  {
    printk("default: unsupported ioctl command=%u\n", cmd);
    return -ENOTTY;
  }
  break;


  case IRQMODDEV_WAIT_IRQ:
  {
    err = -EFAULT;
    
    if (!md->wOpen)
      {
         printk("irqctrl need open in write mode\n");
         return -EINVAL;
      }

    printk("IRQMOD_WAIT_IRQ: enter down_interruptible \n");
    err = down_interruptible(&md->irq_lsigsema);
    
    printk("IRQMOD_WAIT_IRQ: returns err=%d\n", err);
  }
  break;
  


  case IRQMODDEV_DOWN_TIMEOUT:
  {
    
    err = -EFAULT;
    
    if (!md->wOpen)
      {
         printk("irqctrl need open in write mode\n");
         return -EINVAL;
      }
    //printk("IRQMODDEV_DOWN_TIMEOUT: waiting %ld jiffies enter down_timeout \n",jiffies);
    err = down_timeoutms(&md->irq_lsigsema,arg);

    
    //printk("IRQMODDEV_DOWN_TIMEOUT: returns err=%d\n", err);
   
   
  }
  break;

  }
  
  return err;

}


#define DBG_OUT


//********************************************************************************
// int down_timeoutms(struct semaphore *sem,long timeout_ms)
//* wait for semaphore with timeout in ms, also for tickless kernel configs
//  waits minimum: timeout_ms, maximum: timeout_ms + 1 jiffie 
//********************************************************************************
static  __inline int down_timeoutms(struct semaphore *sem,long timeout_ms)
{
    // factor to get ns from ms
    static const unsigned long fac_ms2ns=1000000;
    // factor to get ns from us
    static const unsigned long fac_us2ns=1000;

      unsigned long long endtime;
      long long rest_ns;
      int err;
      register unsigned long localjiffies;
   #if defined(DBG_OUT)
      long long lastrest_ns;
   #endif

      // default return error  
      err     = -1; 
      // calc rest of time in ns
      rest_ns =  (long long)(fac_ms2ns * timeout_ms);
      // calc endtime
      endtime = ktime_get_mono_fast_ns() + (unsigned long long)(rest_ns); 
      // wait for semaphore or timeout
      while (rest_ns>0) {
        // err is alwaya !=0 at this position from init or failed call to down_tmeout(...)
            // wait for semaphore or timeout in jiffies
            // div_u64 instead of / is needed for 32 bit ARM7 gcc - force minimal 1 jiffie
            localjiffies = max((unsigned long)(1),usecs_to_jiffies(div_u64(rest_ns,fac_us2ns)));
            err=down_timeout(sem, localjiffies);
   #if defined(DBG_OUT)
            lastrest_ns = rest_ns;
   #endif
            if (err==0) {
                // return on success
                break;
            }
            // no success -> calc rest of time to wait
            rest_ns= endtime - ktime_get_mono_fast_ns();
   #if defined(DBG_OUT)
            printk("down_timeout: timeout_ms=%ld system measures jiffiess as=%lu restold_ns=%lld rest_ns=%lld err=%d\n",timeout_ms,localjiffies,lastrest_ns,rest_ns,err);
   #endif
        }
       
    return err;
}


/// Called the starting of the driver, activates irq. conf. and semaphore

static int driver_open (struct inode *device_file, struct file *instance)
{
  MYCONTEXT* md=(MYCONTEXT*)kzalloc(sizeof(MYCONTEXT), GFP_KERNEL);
  
  instance->private_data = md;

   
  strcpy(&md->devicename[0],DEVICE_NAME);

  r_int_config(md); 

   if (request_irq(md->irq_number,
                   &r_irq_handler,
                   IRQF_TRIGGER_FALLING,
                   &md->devicename[0],
                   md)) {
      printk("Irq Request failure\n");
      return -EFAULT;
   }

  md->wOpen = ((instance->f_mode & FMODE_WRITE)!=0);
  sema_init(&md->irq_lsigsema, 0);
  printk("Open was called! \n");
 return 0;  
}


///  Closing process which releases the interrupts
static int driver_close (struct inode *device_file, struct file *instance)
{

   MYCONTEXT* md;
   
   md=(MYCONTEXT*)(instance->private_data);        //??

   printk("Close was called! \n");

   r_int_release(md);

   printk("Irq number = %d",md->irq_number);

   kfree(instance->private_data);
   instance->private_data = NULL;
   return 0;  
}


/// Communication function between user and kernel space which calls appropriate functions
static long driver_unioctl (struct file *file, unsigned int cmd, unsigned long arg)
{
  MYCONTEXT *mycontext;                        


  int ret;
  int parameter_error;
  void __user* user_ptr;

  ret = 0;
  parameter_error = 0;
  user_ptr = (void __user*) arg;
  
  mycontext = file->private_data;
  
#if defined(DBG_OUT)
  printk (KERN_INFO "%s: IOCTL, command=%d(%08x), type=%d, nr=%d, dir=%d, size=%d, arg=%lx\n",
            DEVICE_NAME,
            cmd,
            cmd,
            _IOC_TYPE(cmd),
            _IOC_NR(cmd),
            _IOC_DIR(cmd),
            _IOC_SIZE(cmd),
            arg);
#endif


  if (_IOC_DIR(cmd) & _IOC_READ) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,0,0)
    parameter_error = !access_ok(user_ptr, _IOC_SIZE(cmd));
#else
    parameter_error = !access_ok(VERIFY_WRITE, user_ptr, _IOC_SIZE(cmd));
#endif
  }
  if (_IOC_DIR(cmd) & _IOC_WRITE) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,0,0)
    parameter_error = !access_ok(user_ptr, _IOC_SIZE(cmd));
#else
    parameter_error = !access_ok(VERIFY_READ, user_ptr, _IOC_SIZE(cmd));
#endif
  }
  if (parameter_error) {
    ret = -EFAULT;
  }
  else {
   //      printk("IOCTL called");
         ret=my_api(cmd,mycontext,arg);               
  }

  return ret;
}








/// File operations struct
static struct file_operations fops = {
   .owner = THIS_MODULE,
   .open = driver_open,
   .release = driver_close,
   .unlocked_ioctl = driver_unioctl
};


/// Initialization block which called in the beginning to adjust the 
/// connection between device and user space
int r_init(void) {
   int retval;
   printk(KERN_NOTICE "Hello !\n");
   // r_int_config();
   retval = register_chrdev(MYMAJOR, DEVICE_NAME, &fops);
   if(retval == 0){
      printk("%s - registered Device number, Major: %d\n", DEVICE_NAME,MYMAJOR);
   }
   else if (retval > 0){
      printk("%s - registered Device number, Major: %d err=%d\n", DEVICE_NAME,MYMAJOR,retval);
      }

   else {
      printk("Could noot register device number!\n");
      return -1;
   }
   return 0;
}


/// Cleanup function for the device
void r_cleanup(void) {
   unregister_chrdev(MYMAJOR, DEVICE_NAME);
   printk(KERN_NOTICE "Goodbye\n");
   
   return;
}


module_init(r_init);
module_exit(r_cleanup);


/****************************************************************************/
/* Module licensing/description block.                                      */
/****************************************************************************/
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
