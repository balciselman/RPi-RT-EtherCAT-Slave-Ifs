/*! \file cspitarget.h
    \brief Header file of the system.
*/

#ifndef __RPI_BASE_H
 #define __RPI_BASE_H


extern FILE* stream;         // why is this here??

//!@{
//! LAN9252 Datasheet TABLE 5-1: SYSTEM CONTROL AND STATUS REGISTERS 
//!---- access to EtherCAT registers -------------------

#define RESET_CTL               0x01F8        

#define ECAT_CSR_DATA           0x0300     
#define ECAT_CSR_CMD            0x0304     
#define ECAT_PRAM_RD_ADDR_LEN   0x0308      
#define ECAT_PRAM_RD_CMD        0x030C      
#define ECAT_PRAM_WR_ADDR_LEN   0x0310     
#define ECAT_PRAM_WR_CMD        0x0314     

#define ECAT_PRAM_RD_DATA       0x0000     
#define ECAT_PRAM_WR_DATA       0x0020     

#define ID_REV                  0x0050      
#define IRQ_CFG                 0x0054
#define INT_STS                 0x0058
#define INT_EN                  0x005C
#define BYTE_TEST               0x0064
#define HW_CFG                 	0x0074
#define PMT_CTRL                0x0084
#define GPT_CFG                 0x008C
#define GPT_CNT                 0x0090
#define FREE_RUN                0x009C
//!@}


//!@{
//! DEFINE AS OUTPUT PIN G23,G24,G12,G16,G20,G21 HEADER JP2 ETHERBERRY BOARD
#define OUT_G23 RPI_BPLUS_GPIO_J8_16   ///< OUTPUT 1
#define OUT_G24 RPI_BPLUS_GPIO_J8_18   ///< OUTPUT 2
#define OUT_G12 RPI_BPLUS_GPIO_J8_32   ///< OUTPUT 3
#define OUT_G16 RPI_BPLUS_GPIO_J8_36   ///< OUTPUT 4
#define OUT_G20 RPI_BPLUS_GPIO_J8_38   ///< OUTPUT 5
#define OUT_G21 RPI_BPLUS_GPIO_J8_40   ///< OUTPUT 6
//!@}


//!@{
//! DEFINE AS INPUT PIN G22,G05,G06,G13,G19,G26 HEADER JP2 ETHERBERRY BOARD
#define IN_G22 RPI_BPLUS_GPIO_J8_15   ///< INPUT 1
#define IN_G05 RPI_BPLUS_GPIO_J8_29   ///< INPUT 2
#define IN_G06 RPI_BPLUS_GPIO_J8_31   ///< INPUT 3
#define IN_G13 RPI_BPLUS_GPIO_J8_33   ///< INPUT 4
#define IN_G19 RPI_BPLUS_GPIO_J8_35   ///< INPUT 5
#define IN_G26 RPI_BPLUS_GPIO_J8_37   ///< INPUT 6
//!@}


//!@{  
//! LAN9252 Datasheet TABLE 12-15: ETHERCAT CORE CSR REGISTERS
//!
#define TYPE_REG 				0x0000
#define REV_REG  				0x0001
#define BUILD_REG_1 			0x0002
#define BUILD_REG_2 			0x0003
#define FMMU_REG	 			0x0004
#define SYNCMANAGER_REG	 		0x0005
#define RAM_SIZE_REG 			0x0006
#define PORT_DESCR_REG	 		0x0007
#define ESC_FEATUR_REG_1 		0x0008
#define ESC_FEATUR_REG_2		0x0009
#define CONF_STATION_REG_1 		0x0010
#define CONF_STATION_REG_2		0x0011
#define CONF_STATION_ALI_REG_1 	0x0012
#define CONF_STATION_ALI_REG_2	0x0013
//!@}


//!@{
//! Write Protection Register
#define WD_REG_EN				0x0020
#define WD_REG_PR				0x0021
#define ESC_WD_REG_EN			0x0030
#define ESC_WD_REG_PR			0x0031
//!@}


//!@{
//! Data Link Layer
#define ESC_RST_REG				0x0040
#define ESC_RST_PDI_REG			0x0041
#define ECL_DL_CTRL_REG_0		0x0100 
#define ECL_DL_CTRL_REG_1		0x0101 
#define ECL_DL_CTRL_REG_2		0x0102 
#define ECL_DL_CTRL_REG_3		0x0103 
#define PHY_R_W_OFF_1			0x0108
#define PHY_R_W_OFF_2			0x0109
#define ECL_DL_STATUS_REG_0		0x0110 
#define ECL_DL_STATUS_REG_1		0x0111
//!@}


//!@{
//! Application Layer
#define AL_CTRL_REG_0			0x0120 
#define AL_CTRL_REG_1			0x0121
#define AL_STATUS_REG_0			0x0130 
#define AL_STATUS_REG_1			0x0131
#define AL_STATUS_COD_REG_0		0x0134 
#define AL_STATUS_COD_REG_1		0x0135
#define RUN_LED_OVERRIDE_REG 	0x0138
//!@}


//!@{
//! PDI (Process Data Interface)
#define PID_CTRL_REG 			0x0140
#define ESC_CONF_REG 			0x0141
#define ASIC_CONF_REG_0			0x0142
#define ASIC_CONF_REG_1			0x0143
#define PID_CONF_REG 			0x0150
#define SYNC_PID_CONF_REG 		0x0151
#define EXT_PID_CONF_REG_0		0x0152
#define EXT_PID_CONF_REG_1		0x0153
//!@}


//!@{
//! Interrupts
#define ECAT_EVENT_MASK_REG_0	0x0200
#define ECAT_EVENT_MASK_REG_1	0x0201
#define AL_EVENT_MASK_REG_0		0x0204
#define AL_EVENT_MASK_REG_1		0x0205
#define AL_EVENT_MASK_REG_2		0x0206
#define AL_EVENT_MASK_REG_3		0x0207
#define ECAT_EVENT_REQ_REG_0 	0x0210
#define ECAT_EVENT_REQ_REG_1 	0x0211
#define AL_EVENT_REQ_REG_0 		0x0220
#define AL_EVENT_REQ_REG_1 		0x0221
#define AL_EVENT_REQ_REG_2 		0x0222
#define AL_EVENT_REQ_REG_3 		0x0223
//!@}


//!@{
//! Error Counters
#define RX_ERROR_CNT_REG_0 		0x0300
// ....
#define RX_ERROR_CNT_REG_7 		0x0307
#define FWD_RX_ERROR_CNT_REG_0	0x0308
// ....
#define FWD_RX_ERROR_CNT_REG_B	0x030B
#define ECAT_PRO_UNIT_CNT_ERROR	0x030C
#define PID_CNT_ERROR			0x030D
#define PID_CODE_ERROR			0x030E
#define LOST_LINK_CNT_REG_0 	0x0310
// ....
#define LOST_LINK_CNT_REG_3 	0x0313
//!@}


//!@{
//! EEPROM Interface
#define EEPROM_CONF_REG 		0x0500
#define EEPROM_PDI_STATE_REG 	0x0501
#define EEPROM_CTRL_REG_0 		0x0502
#define EEPROM_CTRL_REG_1 		0x0503
#define EEPROM_ADDR_REG_0 		0x0504
// ....
#define EEPROM_ADDR_REG_4 		0x0507
#define EEPROM_DATA_REG_0 		0x0508
// ....
#define EEPROM_DATA_REG_4 		0x050B
//!@}

//!@{
//! MII Management Interface
#define MII_MANAGE_CTRL_REG_0	0x0510
#define MII_MANAGE_CTRL_REG_1	0x0511
#define PHY_ADDR_REG			0x0512
#define PHY_REGISTER_ADDR_REG	0x0513
#define PHY_DATA_REG_0			0x0514
#define PHY_DATA_REG_1			0x0515
#define MII_MANAGE_ECAT_REG		0x0516
#define MII_MANAGE_PDI_REG		0x0517
//!@}

#define AL_STATUS               0x0130      			///< Application Layer Status
#define WDOG_STATUS             0x0440    				///< Watch dog status

//!@{
//! LAN9252 flags
#define ECAT_CSR_BUSY     0x80
#define PRAM_READ_BUSY    0x80
#define PRAM_READ_AVAIL   0x01
#define PRAM_WRITE_AVAIL  0x01
#define READY             0x08000000
#define DIGITAL_RST       0x00000001
#define ETHERCAT_RST      0x00000040
//!@}


//!@{
//! EtherCAT flags 
#define ESM_INIT                0x01                  ///< init
#define ESM_PREOP               0x02                  ///< pre-operational
#define ESM_BOOT                0x03                  ///< bootstrap
#define ESM_SAFEOP              0x04                  ///< safe-operational
#define ESM_OP                  0x08                  ///< operational
//!@}

//!@{
//! ESC commands 
#define ESC_WRITE 		   0x80
#define ESC_READ 	       0xC0
//!@}

//!@{
//! SPI 
#define COMM_SPI_READ    0x03
#define COMM_SPI_WRITE   0x02
#define DUMMY_BYTE       0xFF
//!@}


#define QINSize 10   									///< size of input queue

#define PIN RPI_GPIO_P1_24								///< Chip Select pin
#define OUT RPI_V2_GPIO_P1_32							///< GPIO 12 adjusted as output to measure frequency and stability



/*! \typedef UWORD
*   \brief union for two byte char for Adresses
*/
typedef union
{
    unsigned short  Word;
    unsigned char   Byte[2];

} UWORD;

/*! \typedef ULONG
*   \brief union for 4 byte
*/
typedef union
{
    unsigned int       Long;
    unsigned short  Word[2];
    unsigned char   Byte[4];

} ULONG;

/*! \typedef PROCBUFFER
*   \brief Union for 32 Bytes used for Buffers
*/
typedef union
{
   unsigned char Byte [32];    
   unsigned int ui[8];        // 8 x 32 bit
   unsigned long long ul[4];  // 4 x 64 bit
        
} PROCBUFFER;


/*! \class CLAN9252 
 *  \brief Raspberry Pi EtherCAT shield differentiator
 *
 * This class differentiates EtherCAT chips for future implementations
*/
class CLAN9252;


/// Inline function to get current monotonic time
///
static inline unsigned long long Getns()
{ 
  struct timespec ts;

  clock_gettime(CLOCK_MONOTONIC,&ts);
  //clock_gettime(CLOCK_MONOTONIC, &ts);
  return ((ts.tv_sec * 1000000000) + ts.tv_nsec);
}






/*! \class rpi_base 
 *  \brief This is the base class for the Pi and SPI communication.
 *
 * This class includes; initialization of the Raspberry Pi, 
 * SPI communication functions,
 * bash keybord utilities (not activated in the test program)
*/
class rpi_base
{
  public:
    rpi_base();  
	virtual ~rpi_base();  
	
	virtual int init()=0;	           								///< Pure virtual initialization function defined in the derived class
    int milisleep(long long miliseconds);							///< Sleep function in miliseconds based on nanosleep

	static int _getch(bool echo);									///< Keyboard utilities
	static int _kbhit();											///< Keyboard utilities
	static int utl_kbkey ();										///< Keyboard utilities
	
	void send_output(const unsigned long long* source,size_t numlongs); ///< Sends the output buffer
  	void receive_input(PROCBUFFER* pBufferOut); 					///< Pulls the data

	//void inputbuffer();			delete it									///< Input buffer called cyclically in the main
	void bashoutput();												///< Bash output Line
	
	long long m_delay;												///< Stores the delay between cycles
	long long m_diffcycle;											///< Problem with calculation!!


	char outbuffer[256];											///< Buffer for bash output
	char numeric[64];												///< For bash

	unsigned int m_curqidx;											///< next index for input from slave
	unsigned int m_curqoutidx;										///< update index for spi output


	PROCBUFFER m_BufferOutEthToSlaveBuffer;
    PROCBUFFER BufferOut;             

    PROCBUFFER fromSlaveBufferIn[QINSize];
	PROCBUFFER BufferIn;

  	
	
   protected:

   	int spi_init();													///< BCM2835 and SPI pins initialitation
    virtual int  is_resetneeded()=0;								///< Checks if the Shield is ready
	virtual int  do_resetshield()=0;								///< Resets the shield
	virtual int  do_initirq() {return 0;}							///< IRQ initialization which implemented in the derived class
	

	unsigned char MainTask();										///< Maintask that calls read and write functios cyclically   
	unsigned long Read_Reg    (unsigned short Address, unsigned char Len);///< Read the directly addressable registers
    unsigned long Read_Reg_Wait  (unsigned short Address, unsigned char Len);///< Read the undirectly addressable registers
	void          Write_Reg  (unsigned short Address, unsigned long DataOut);///< Write to the directly addressable registers
	void          Write_Reg_Wait (unsigned short Address, unsigned long  DataOut);///< Write to the undirectly addressable registers
    
	stattool m_runtime;
 	stattool m_period;

	unsigned long long m_lastruntime;
  	unsigned long long m_laststart;
  	unsigned long long m_count;
	volatile unsigned int m_atomiccounter;


   private:
    CLAN9252*     m_hwif;                 // !!
    
    void          SPIReadProcRamFifo();    							///< Read from process ram fifo
    void          SPIWriteProcRamFifo();   							///< Write to the process ram fifo

    #define SCS_Low_macro     bcm2835_gpio_write(PIN, LOW);			///< Standard SPI chip select management
    #define SCS_High_macro    bcm2835_gpio_write(PIN, HIGH);		///< Standard SPI chip select management

    // standard SPI transfert 
    void SPI_Write(unsigned char Data);								///< Standard SPI transfer 
    unsigned char SPI_Read(unsigned char Data);						///< Standard SPI transfer

	int isrealtime();
   
};






#endif