











#ifndef __SHIELD_9252_H
 #define __SHIELD_9252_H













/*! \class etherberry_9252 
 *  \brief Initialization of the chip and thread execution
 *	
 *
 * 
*/
class etherberry_9252 : public rpi_base 
{
  protected:
	
	

  public:
    etherberry_9252();
	virtual ~etherberry_9252();  
	
	int init();	           										///< General initialization function of the system 
	  
    unsigned int execute_fromthread();							///< Tasks that made by thread
	
  private:
	 int m_devhndl;												///< Stores device number
	
	 /*! \fn int waitforirq_or_timeout(unsigned int timoutms)
	   *   \brief calls ioctl with timeout
	   *
	   * 	Uses stored device number, timeout is currently fixed to the 100ms
	   * */
	int waitforirq_or_timeout(unsigned int timoutms);

	virtual int  is_resetneeded();								///< Checks if the Shield is ready, Returns (-) if needed and 0 if not 
	virtual int  do_resetshield();								///< Resets the shield, returns (-) if still fails
	virtual int  do_initirq();									///< IRQ and device init. returns (-) if fails and 0 if deviced opened
	
	
};


#endif