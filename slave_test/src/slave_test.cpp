/**********************************************************************************/
/*                                                                                */
/* slave_test.cpp: main for the slave interface                                   */
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

////////////////////////////////////////// For long long random numbers
  /* Seed */
  std::random_device rd;
  /* Random number generator */
  std::default_random_engine generator(rd());
  /* Distribution on which to apply the generator */
  std::uniform_int_distribution<long long unsigned> distribution(0,0xFFFFFFFFFFFFFFFF);
///////////////////////////////////////////

etherberry_9252  ETHB;			// EtherC instantiation



rttestthread mytestthread(&ETHB);



char buf[]={0x00, 0x20, 0x30, 0x08, 0x06, 0x14, 0x04, 0x17};
const char *str[]  ={"SUN", "MON", "TUES", "WED", "THUR", "FRI", "SAT"};   


PROCBUFFER  BufferfromEthCAT;


int main(int argc, char* argv[])
{
  int status;
  unsigned long long loopstartms, loopstartns;
//unsigned long long before, beforems;
  unsigned long long counter = 0;
  unsigned long long inputbuffer[4];
//long long rest;
//const unsigned long long onems_inns=1000000;

  //---- initialize the EtherC board -----

  memset(&inputbuffer[0],0,sizeof(inputbuffer));

  status=ETHB.init();
    if (status!=0)
      {
        fprintf(stderr,"Error Initialization failed!\r\n");
        bcm2835_spi_end();
        bcm2835_close();
        return status;
      }

  
  //#define USE_THIS
	/*
  #if defined(USE_THIS)
  struct timespec tim;
  tim.tv_sec = 0;
  tim.tv_nsec = 200000;
  while (1)
	{   
     start = Getns();
     bcm2835_gpio_set(OUT);
     nanosleep(&tim, NULL);        					        // delay of 10mS
     bcm2835_gpio_clr(OUT);
     nanosleep(&tim, NULL);	         					        // delay of 10mS
     period= (Getns() - start) * 1E-3;
     printf("period(us): %lld\n",period);
  }
  #endif
*/
  mytestthread.Resume();                   /// Activating the thread!
	

	while (1)
	{
    
    loopstartns = Getns();	
    loopstartms = loopstartns / 1000000;
    
    ETHB.receive_input(&BufferfromEthCAT); 

    inputbuffer[0] = loopstartms;
    inputbuffer[1] = counter;                         // counter at the second position
    //inputbuffer[2] = distribution(generator);
    //inputbuffer[3] = distribution(generator);
    inputbuffer[2] = 1;
    inputbuffer[3] = 2;
    counter++;
     
    ETHB.send_output(&inputbuffer[0]);

    ETHB.bashoutput(); 
/*
		before = Getns(); 
    beforems = before / 1000000;  

    ETHB.m_diffcycle = (beforems & 0xFFFFFFFF) - ETHB.BufferOut.ul[0];

    rest = ((loopstartns+10*(onems_inns))-before)/onems_inns;
      
    if (rest<1)
        {
          rest=1;
        }
      if (rest>100)
       {
         rest=100;
       }  
      ETHB.m_delay=(Getns()-loopstartns)/1000000;
*/
      ETHB.milisleep(100);
  }

	bcm2835_i2c_end();
	bcm2835_close();
 
}