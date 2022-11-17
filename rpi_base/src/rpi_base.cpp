/**********************************************************************************/
/*                                                                                */
/* cspitarget.cpp    ??                                                           */
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

// #include <conio.h>      //sb


#include "ethercatslave.h"



//
//
//
static unsigned int readfilebuf(const char* pathname, char * buffer, unsigned int maxbufsize)
{
  int file_fd;
  long ret;
  // read the File

  FILE* fh = fopen(pathname, "rb");
  if (fh == NULL)
  {
    buffer[0] = 0;
    return 0;
  }
  else
  {
    file_fd = fileno(fh);

    buffer[0] = 0;
    ret = read(file_fd, buffer, maxbufsize - 1);
    if (ret > 0)
    {
      buffer[ret] = 0;
    }
    else
    {
      buffer[0] = 0;
      ret = 0;
    }
  }
  fclose(fh);
  return ret;
}

const char* strip(char* path)
{
	unsigned int i;
	unsigned int len;

	len = (unsigned int)(strlen(path));

	for (i = 0; i < len; i++)
	{
		if (path[i] == '\r')
		{
			path[i] = 0;
		}
		if (path[i] == '\n')
		{
			path[i] = 0;
		}
	}
	return &path[0];
}



//
rpi_base::rpi_base()               
{
  isrealtime();
  m_curqidx=0;
  m_atomiccounter= 0;
  m_count = 0;
  m_laststart = 0;
	m_lastruntime = 0; 
  m_hwif = NULL;
  // m_hwif = new CLAN9252(SPI_Address);                                          
}

rpi_base::~rpi_base()
{
  if (m_hwif)
    {
      delete m_hwif;
      m_hwif=NULL;
    }
}



/*virtual */
int rpi_base::spi_init()
{   
    int err = 0;
    if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root ?\n");
      err = -10;
      return err;
    }
    //Setup SPI pins
    if (!bcm2835_spi_begin())
    {
      printf("bcm2835_spi_begin failed. Are you running as root ?\n");
      err = -9;
      return err;
    }

    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);     
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                 
    bcm2835_spi_setClockDivider(16);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
    bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(OUT, BCM2835_GPIO_FSEL_OUTP);         /// GPIO 12 adjusted as output to measure
                                                            /// frequency and stability

    return err;
}



int rpi_base::isrealtime()
{ 
   int isreal = 0;
   int err = -3;
   unsigned int len;
   char buffer[64];
   memset(&buffer[0], 0, sizeof(buffer));
   len = readfilebuf("/sys/kernel/realtime", &buffer[0], sizeof(buffer)-1);
   if (len > 0)
   {
    strip(&buffer[0]);
      isreal = (strcmp(&buffer[0], "1"));
     if (isreal != 0)
      {
        err = -5;
      }
      else{
        err = mlockall(MCL_CURRENT | MCL_FUTURE);
        }
    }
    
    return err;
}




void rpi_base::SPI_Write(unsigned char Data)
{
    bcm2835_spi_transfer(Data);
};


unsigned char rpi_base::SPI_Read(unsigned char Data)
{
    return bcm2835_spi_transfer(Data);
};


/// Output buffer queue
void rpi_base::send_output(const unsigned long long* source,size_t numlongs)
{
            
      for (unsigned long i=0; i<(sizeof(BufferIn)/sizeof(BufferIn.ul[0])); i++)
         {                                                // the input buffer
             fromSlaveBufferIn[m_curqidx].ul[i] = *source;   // atomic copy
             source++;
         }
          
      m_curqoutidx=m_curqidx;                             /// update index for spi output
      
      m_curqidx++;                                        /// next index for input from slave
      if (m_curqidx>=QINSize)
        {
          m_curqidx=0;
        }

}
    /// Input Buffer
   void rpi_base::receive_input(PROCBUFFER* pBufferOut)
   { 
    volatile unsigned int localcounter;
    
      do {
        localcounter=m_atomiccounter;
        *pBufferOut=m_BufferOutEthToSlaveBuffer;
        
      } while(localcounter != m_atomiccounter);

    
   }


//---- EtherCAT task ------------------------------------------------------------------------------


unsigned char rpi_base::MainTask()                           /// must be called cyclically by the application

{
  bool WatchDog = 0;
  bool Operational = 0; 
  unsigned char i;
  ULONG TempLong;
  unsigned char Status;
  

  TempLong.Long = Read_Reg_Wait (WDOG_STATUS, 1);           // read watchdog status
  if ((TempLong.Byte[0] & 0x01) == 0x01)                    //
    WatchDog = 0;                                           // set/reset the corresponding flag
  else                                                      //
    WatchDog = 1;                                           //
    
  TempLong.Long = Read_Reg_Wait (AL_STATUS_REG_0, 1);       // read the EtherCAT State Machine status
  Status = TempLong.Byte[0] & 0x0F;                         //
  if (Status == ESM_OP)                                     // to see if we are in operational state
    Operational = 1;                                        //
  else                                                      // set/reset the corrisponding flag
    Operational = 0;                                        //    

  

                                                            //--- process data transfert ----------
                                                            //                                                        
  if (WatchDog | !Operational)                              // if watchdog is active or we are 
  {                                                         // not in operational state, reset 
    for (i=0; i<4; i++)  {                                  // the output buffer
    BufferOut.ul[i] = 0;                                    //
    }   
  }
  else                                                      
  {                                                         
    SPIReadProcRamFifo();                                   // otherwise transfer process data from 
  }                                                         // the EtherCAT core to the output buffer  
  
  
  
  SPIWriteProcRamFifo();                                    // we always transfer process data from
                                                            // the input buffer to the EtherCAT core  

                 
  SPIReadProcRamFifo();                                    /// otherwise transfer process data from 
  
  

  if (WatchDog)                                             // return the status of the State Machine      
  {                                                         // and of the watchdog
    Status |= 0x80;                                         //
  } 
                                                            //
  return Status;                                            //  

}


//---- read a directly addressable registers  -----------------------------------------------------

unsigned long rpi_base::Read_Reg (unsigned short Address, unsigned char Len)

                                                            // Address = register to read
                                                            // Len = number of bytes to read (1,2,3,4)
                                                            //
                                                            // a long is returned but only the requested bytes
                                                            // are meaningful, starting from LsByte                                                 
{
  ULONG Result; 
  UWORD Addr;
  Addr.Word = Address; 
  unsigned char i; 
  
  SCS_Low_macro                                             // SPI chip select enable

  SPI_Write(COMM_SPI_READ);                                 // SPI read command
  SPI_Write(Addr.Byte[1]);                                  // address of the register
  SPI_Write(Addr.Byte[0]);                                  // to read, MsByte first
 
  for (i=0; i<Len; i++)                                     // read the requested number of bytes
  {                                                         // LsByte first 
    Result.Byte[i] = SPI_Read(DUMMY_BYTE);            
  }                                                         //    
  
  SCS_High_macro                                            // SPI chip select disable 
 
  return Result.Long;                                       // return the result
}


//---- write a directly addressable registers  ----------------------------------------------------


void rpi_base::Write_Reg (unsigned short Address, unsigned long DataOut)

                                                   // Address = register to write
                                                   // DataOut = data to write
{ 
  ULONG Data; 
  UWORD Addr;
  Addr.Word = Address;
  Data.Long = DataOut;    
  SCS_Low_macro                                             // SPI chip select enable  
  SPI_Write(COMM_SPI_WRITE);                           // SPI write command
  SPI_Write(Addr.Byte[1]);                             // address of the register
  SPI_Write(Addr.Byte[0]);                             // to write MsByte first
  SPI_Write(Data.Byte[0]);                             // data to write 
  SPI_Write(Data.Byte[1]);                             // LsByte first
  SPI_Write(Data.Byte[2]);                             //
  SPI_Write(Data.Byte[3]);                             //
  SCS_High_macro                                            // SPI chip select enable   
}



//---- read an undirectly addressable registers  --------------------------------------------------

unsigned long rpi_base::Read_Reg_Wait (unsigned short Address, unsigned char Len)                                                 
{
  ULONG TempLong;
  UWORD Addr;
  Addr.Word = Address;
                                                            // compose the command
                                                            //
  TempLong.Byte[0] = Addr.Byte[0];                          // address of the register
  TempLong.Byte[1] = Addr.Byte[1];                          // to read, LsByte first
  TempLong.Byte[2] = Len;                                   // number of bytes to read
  TempLong.Byte[3] = ESC_READ;                              // ESC read 

  Write_Reg (ECAT_CSR_CMD, TempLong.Long);     // write the command

  do
  {                                                         // wait for command execution
    TempLong.Long = Read_Reg(ECAT_CSR_CMD,4);  //
  }                                                         //
  while(TempLong.Byte[3] & ECAT_CSR_BUSY);                  //
                                                             
                                                              
  TempLong.Long = Read_Reg(ECAT_CSR_DATA,Len); // read the requested register
  return TempLong.Long;                                     //
}


//---- write an undirectly addressable registers  --------------------------------


void  rpi_base::Write_Reg_Wait (unsigned short Address, unsigned long DataOut)                                                    
{
  ULONG TempLong;
  UWORD Addr;
  Addr.Word = Address;


  Write_Reg (ECAT_CSR_DATA, DataOut);            // write the data

                                                              // compose the command
                                                              //                                
  TempLong.Byte[0] = Addr.Byte[0];                            // address of the register  
  TempLong.Byte[1] = Addr.Byte[1];                            // to write, LsByte first
  TempLong.Byte[2] = 4;                                       // we write always 4 bytes
  TempLong.Byte[3] = ESC_WRITE;                               // ESC write

  Write_Reg (ECAT_CSR_CMD, TempLong.Long);       // write the command

  do                                                          // wait for command execution
  {                                                           //
    TempLong.Long = Read_Reg (ECAT_CSR_CMD, 4);  //  
  }                                                           //  
  while (TempLong.Byte[3] & ECAT_CSR_BUSY);                   //
  
}


//---- read from process ram fifo ----------------------------------------------------------------


void rpi_base::SPIReadProcRamFifo()  
{
  ULONG TempLong;
  unsigned char i;
  
  m_atomiccounter++;
  

  Write_Reg (ECAT_PRAM_RD_ADDR_LEN, 0x00201000);                // we always read 32 bytes   0x0020----
                                                                // output process ram offset 0x----1000
                                                                
  Write_Reg (ECAT_PRAM_RD_CMD, 0x80000000);                     // start command                                                                  
                                                                                                                               
  do                                                            // wait for data to be transferred      
  {                                                             // from the output process ram 
    TempLong.Long = Read_Reg (ECAT_PRAM_RD_CMD,4);              // to the read fifo 
  }                                                             //    
  while (!(TempLong.Byte[0] & PRAM_READ_AVAIL) || (TempLong.Byte[1] != 8));     
  
 
  SCS_Low_macro                                                 // SPI chip select enable  
  
  SPI_Write(COMM_SPI_READ);                                     // SPI read command
  SPI_Write(0x00);                                              // address of the read  
  SPI_Write(0x00);                                              // fifo MsByte first
  

  for (i=0; i<32; i++)                                          // 32 bytes read loop 
  {                                                             // 
    BufferOut.Byte[i] = SPI_Read(DUMMY_BYTE);                   //
  }                                                             //
    
  m_BufferOutEthToSlaveBuffer =   BufferOut;
  SCS_High_macro                                                // SPI chip select disable    

  m_atomiccounter++;
  
}

//---- write to the process ram fifo --------------------------------------------------------------

void rpi_base::SPIWriteProcRamFifo()    // write 32 bytes to the input process ram, through the fifo
                                       //    
                                       // these are the bytes that we have read from the inputs of our                   
                                       // application and that will be sent to the EtherCAT master
{
  ULONG TempLong;
  unsigned char i;

  // get buffer from slave input
  BufferIn = fromSlaveBufferIn[m_curqoutidx];
             

  Write_Reg (ECAT_PRAM_WR_ADDR_LEN, 0x00201200);                // we always write 32 bytes 0x0020----
                                                                // input process ram offset 0x----1200
  Write_Reg (ECAT_PRAM_WR_CMD, 0x80000000);                     // start command  

  do                                                            // check fifo has available space     
  {                                                             // for data to be written
    TempLong.Long = Read_Reg (ECAT_PRAM_WR_CMD,4); //  
  }                                                             //    
  while (!(TempLong.Byte[0] & PRAM_WRITE_AVAIL) || (TempLong.Byte[1] < 8) );             
  
  
  SCS_Low_macro                                                 // enable SPI chip select
  SPI_Write(COMM_SPI_WRITE);                                    // SPI write command
  SPI_Write(0x00);                                              // address of the write fifo 
  SPI_Write(0x20);                                              // MsByte first 
  for (i=0; i<31; i++)                                          // 32 bytes write loop
  {                                                             //
    SPI_Write (BufferIn.Byte[i]);                          
  }                                                             //
                                                                //  
  SPI_Write (BufferIn.Byte[31]);                       
  SCS_High_macro                                                // disable SPI chip select      
} 


int rpi_base::milisleep(long long miliseconds)
{
   struct timespec rem;
   struct timespec req;
   
   req.tv_sec = (__time_t)(miliseconds*0.001);
   req.tv_nsec = (miliseconds * 1000000ll) -(req.tv_sec * 1000000000ll);

  
   return nanosleep(&req , &rem);
}

void rpi_base::bashoutput(){                  // replace it with vt100!!!
  outbuffer[sizeof(outbuffer)-1]=0;         
  numeric[sizeof(numeric)-1]=0;
  outbuffer[0]=0;
	
 /* 
  strncat(&outbuffer[0],"Out rpi:",sizeof(outbuffer)-1);
  for(int i=0; i<4; i++) {
	    snprintf(&numeric[0],sizeof(numeric)-1,"%016llx",BufferIn.ul[i]);		
			strncat(&outbuffer[0]," ",sizeof(outbuffer)-1);
      strncat(&outbuffer[0],numeric,sizeof(outbuffer)-1);

		}
  

	strncat(&outbuffer[0]," In rpi: ",sizeof(outbuffer)-1);
	for(int i=0; i<4; i++) {
	    snprintf(&numeric[0],sizeof(numeric)-1,"%016llx",BufferOut.ul[i]);
			strncat(&outbuffer[0],numeric,sizeof(outbuffer)-1);
		}


  snprintf(&numeric[0],sizeof(numeric)-1,"%lld",m_diffcycle);
  strncat(&outbuffer[0]," cycle:",sizeof(outbuffer)-1);
  strncat(&outbuffer[0],&numeric[0],sizeof(outbuffer)-1);
  
*/

/*
  strncat(&outbuffer[0]," In rpi: ",sizeof(outbuffer)-1);
	for(int i=0; i<2; i++) {
	    snprintf(&numeric[0],sizeof(numeric)-1,"%016llx",BufferOut.ul[i]);
			strncat(&outbuffer[0],numeric,sizeof(outbuffer)-1);
		}
*/  

  
  snprintf(&numeric[0],sizeof(numeric)-1,"%f",m_period.sbuf.vmean);
  strncat(&outbuffer[0],"period(ms):",sizeof(outbuffer)-1);
  strncat(&outbuffer[0],&numeric[0],sizeof(outbuffer)-1);
  
  snprintf(&numeric[0],sizeof(numeric)-1,"%f",m_period.sbuf.vrange);
  strncat(&outbuffer[0]," per_range:",sizeof(outbuffer)-1);
  strncat(&outbuffer[0],&numeric[0],sizeof(outbuffer)-1);
  
  snprintf(&numeric[0],sizeof(numeric)-1,"%f",m_period.sbuf.vstddev);
  strncat(&outbuffer[0]," per_stddev:",sizeof(outbuffer)-1);
  strncat(&outbuffer[0],&numeric[0],sizeof(outbuffer)-1);
  
  snprintf(&numeric[0],sizeof(numeric)-1,"%f",m_period.sbuf.vmax);
  strncat(&outbuffer[0]," per_max:",sizeof(outbuffer)-1);
  strncat(&outbuffer[0],&numeric[0],sizeof(outbuffer)-1);
  
  snprintf(&numeric[0],sizeof(numeric)-1,"%f",m_period.sbuf.vmin);
  strncat(&outbuffer[0]," per_min:",sizeof(outbuffer)-1);
  strncat(&outbuffer[0],&numeric[0],sizeof(outbuffer)-1);
  

  snprintf(&numeric[0],sizeof(numeric)-1,"%f",m_runtime.sbuf.vmean);
  strncat(&outbuffer[0]," runtime(us):",sizeof(outbuffer)-1);
  strncat(&outbuffer[0],&numeric[0],sizeof(outbuffer)-1);

  snprintf(&numeric[0],sizeof(numeric)-1,"%f",m_runtime.sbuf.vrange);
  strncat(&outbuffer[0]," runt_range:",sizeof(outbuffer)-1);
  strncat(&outbuffer[0],&numeric[0],sizeof(outbuffer)-1);
  
  snprintf(&numeric[0],sizeof(numeric)-1,"%f",m_runtime.sbuf.vstddev);
  strncat(&outbuffer[0]," runt_stddev:",sizeof(outbuffer)-1);
  strncat(&outbuffer[0],&numeric[0],sizeof(outbuffer)-1);
  
  snprintf(&numeric[0],sizeof(numeric)-1,"%f",m_runtime.sbuf.vmax);
  strncat(&outbuffer[0]," runt_max:",sizeof(outbuffer)-1);
  strncat(&outbuffer[0],&numeric[0],sizeof(outbuffer)-1);

  snprintf(&numeric[0],sizeof(numeric)-1,"%f",m_runtime.sbuf.vmin);
  strncat(&outbuffer[0]," runt_min:",sizeof(outbuffer)-1);
  strncat(&outbuffer[0],&numeric[0],sizeof(outbuffer)-1);

  printf ("\r %s",&outbuffer[0]);
  fflush(stdout);		

}





// ************************************************************
// * keyboad utilities
// ************************************************************

int rpi_base::_getch(bool echo=false){
      struct termios oldt, newt;
      int ch;
      tcgetattr( STDIN_FILENO, &oldt );
      newt = oldt;
      newt.c_lflag &= ~ICANON;
      if (echo)
		    newt.c_lflag &=  ECHO;
      else
		    newt.c_lflag &= ~ECHO;
      tcsetattr( STDIN_FILENO, TCSANOW, &newt );
      ch = getchar();
      tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
      return ch;
      }


int rpi_base::_kbhit(){
      struct termios oldt, newt;
      int ch;
      int oldf;    
      tcgetattr(STDIN_FILENO, &oldt);
      newt = oldt;
      newt.c_lflag &= ~(ICANON | ECHO);
      tcsetattr(STDIN_FILENO, TCSANOW, &newt);
      oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
      fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);    
      ch = getchar();    
      tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
      fcntl(STDIN_FILENO, F_SETFL, oldf);    
      if(ch != EOF){
	      ungetc(ch, stdin);
      	return 1;
	    }    
      return 0;
}

int rpi_base::utl_kbkey () {
   int rc;
   int key;

   key = _kbhit();

   if (key == 0) {
      rc = 0;
   } else {
      rc = _getch();
   }
  return rc;
}



