/**********************************************************************************/
/*                                                                                */
/* shield_9252: Microchip LAN9252 integrated shield differentiation source        */
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
/**********************************************************************************/








#include "ethercatslave.h"




etherberry_9252::etherberry_9252()
{
  m_devhndl = -1;
}

etherberry_9252::~etherberry_9252()
{
  if (m_devhndl!=-1)
  {
    close(m_devhndl);
 } 
}

// calls ioctl function to communicate kernel mode
int etherberry_9252::waitforirq_or_timeout(unsigned int timeoutms)
{
  int err=-1;
  if (m_devhndl!=-1)
    {
      err = ioctl(m_devhndl,IRQMOD_DOWN_TIMEOUT,timeoutms);
    }
   return err;
  }

//
//
//

/*virtual */
int etherberry_9252::init()
{
    int err;

    err=spi_init();                                               ///spi initialized
      if (err!=0) 
        { 
          return err;
        }
       
    printf("Spi initialized!\n");
    

    if (is_resetneeded()!=0)                                         /// device is now able to receive requests
      {
        err=do_resetshield();
      }
    if (err!=0) 
    { 
      return err;
    } 

    err=do_initirq();                                             /// irq and device initialization
    if (err!=0) 
    { 
      return err;
    } 
    
    return err;
}  

/* virtual */
int etherberry_9252::is_resetneeded()        
{
  int err=0;
  ULONG TempLong;

  TempLong.Long = Read_Reg (HW_CFG, 4);              //
  if (TempLong.Long & READY) 
  {
      printf("Ready flag is also checked!\n");
  }
     else
  {
      return -11;
  }         
     
  /// check if the test register okey, when the chip resetted two times master resets the slaves
  
  TempLong.Long = Read_Reg (BYTE_TEST, 4);
  if(TempLong.Long != 0x87654321)
  {
    err=-15;
    printf("Test byte is not correct reset protocol initiated!\n");
  }
  return err;
}


int etherberry_9252::do_resetshield()
{
  ULONG TempLong;
  int err;
  
  Write_Reg (RESET_CTL, DIGITAL_RST ); // LAN9252 reset
  usleep (2000);  // wait 2mS
  Write_Reg (RESET_CTL, DIGITAL_RST ); // LAN9252 reset
  usleep (2000);  // wait 2mS

  TempLong.Long = Read_Reg (BYTE_TEST, 4); 

  if (TempLong.Long == 0x87654321)
  {
     err=0;
  }
  else
  { 
    printf("Test Register could not initialized correctly!\n");
    err=-9;
  }

  return err;
}


int  etherberry_9252::do_initirq()
  {
    
    Write_Reg_Wait (AL_EVENT_MASK_REG_0, 0x00000100);                 // enable interrupt from SM 0 event
      
    Write_Reg (IRQ_CFG, 0x00000111);                                  // set LAN9252 interrupt pin driver  
                                                                        // as push-pull active high
                                                                        // (On the EasyCAT shield board the IRQ pin
                                                                        // is inverted by a mosfet
                
    Write_Reg (INT_EN, 0x00000001);                                   // enable LAN9252 interrupt       
    printf("Interrupt for GPIO 25 has been initialized!\n");    

	
  int err=-1;
  char devnamebuf[256];

  devnamebuf[sizeof(devnamebuf)-1]=0;
  snprintf(&devnamebuf[0],sizeof(devnamebuf)-1,"/%s/%s","dev",DEVICE_NAME);
  printf("try to open device %s ...\n",&devnamebuf[0]);

  m_devhndl = open(&devnamebuf[0], O_RDWR);               
  if (m_devhndl == -1)                                    
  {
      fprintf(stderr,"Opening was not possible!\n");
      fprintf(stderr,"run:modprobe gpioirq\n");
      err=-27;
      return err;
  }
  err=0;
  printf("Openning was successful!\n");
  return err;
}


unsigned int etherberry_9252::execute_fromthread()
{
    int err = 0;
    int timeoutms=100;                      

    unsigned long long start_run;
	  unsigned long long period;
    bool statreset = false;
    

    while (1)
      {  
            err =waitforirq_or_timeout(timeoutms);
            if(err!=0)
            {
              //printf("irq timeout!err=%d\n",err);
            }
            
            start_run = Getns();
            /*
            if (statreset == false)
            {
              if(start_run>2000000000)
              {
                m_runtime.reset();
                statreset = true;
              }
             
            }
            */

            bcm2835_gpio_set(OUT);          // for oscilloscope masuring can be commented
            MainTask();     
            bcm2835_gpio_clr(OUT);          // for oscilloscope masuring can be commented
            
            m_lastruntime = Getns() - start_run;
            if (m_count > 1)
            {
              period = start_run - m_laststart;
              m_runtime.calc((double)(m_lastruntime) * 1E-3);
              m_period.calc((double)(period) * 1E-6);
            }
            m_count += 1;
            m_laststart = start_run;
            
    }
		 return 0;
 }
