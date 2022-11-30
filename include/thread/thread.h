

#ifndef __THREAD_H
 #define __THREAD_H




const char* fix_threadname(const char* cname, char* buffer, size_t maxsize); 


/*! expand forbid copy of class instance in class definition  */
#define FORBID_COPY(CLASSNAME) private: CLASSNAME(const CLASSNAME&); CLASSNAME& operator=(const CLASSNAME&)



/*! \class baseobj 
 *  \brief This is the base of all thread classes 
 *
 *  In shared mode operators new and delete are implemented to
 *  put all objects in shared memory
*/
class baseobj
{
private:
	FORBID_COPY(baseobj);

public:
	baseobj() {};            /*!<  default constructor (inline empty) */ 
	virtual ~baseobj() {};   /*!<  virtual destructor (inline empty) */

	virtual const char* Name()
	{
		return "rttest";
	}
};



/*! \class genericthread
 *  \brief This class implements a generic thread, also created externel \see HAL */
class genericthread : public baseobj
{

	FORBID_COPY(genericthread);

	public:
		/*! A constructor.	*/
		genericthread()
		{}
		/*! A destructor. */
		virtual ~genericthread()
		{}
#if defined(USE_THIS)		
		/*! get current running thread */
		static genericthread* get_curthread();
#endif		
		/*! get threadid */
		static void* sget_threadid();
		/*! register this thread */
		void register_this_thread();
};

 
/*! \class threadbase
 *  \brief This class implements a thread \see HAL */

class threadbase : public genericthread 
{
	FORBID_COPY(threadbase);

public:
	/*! A constructor.
		 \param name		- name of the thread
		 \param priority   - a value [1..99]
		 \param ref        - value will be pased to virtual Execute methode
		 \param cpu        - cpu affinity
		 \param crrunning  - if true thread will run after creation or hold if false
		 \param stacksize  - stacksize of the thread ( 0= default stacksize for plattform )
		 */
	threadbase(const char* name, int priority, void* ref, int cpu = -1, bool crrunning = true, unsigned int stacksize = 0);
	/*! A destructor. */
	virtual ~threadbase();

	int          m_priority;	//!< priority member
	void*		 m_ref;		//!< ref for Execute
	int          m_cpu;		//!< cpu affinity
	int          m_isrt;    //!< isrt
	int          m_rcpu;    // real cpu
	volatile unsigned int m_activ;		//!< rtthread is active
	volatile unsigned int m_crrunning;	//!< rtthread marker run after create

	/*! Resume() resumes a thread created with crrunnung=false */
	virtual void Resume() {};
	/*! Terminate() terminates a running rtthread */
	virtual bool Terminate() { return false; };

	/*! helper methode for threadstart */
	unsigned int ExecThread(void* ref);


	/*! pure virtual Execute() methode */
	virtual unsigned int Execute(void* ref) { return 0; }
	
protected:
	size_t imp_buf_size;	//!<  size of os object bufferspace
public:
	unsigned char* imp_buf;		//!<  buffer space for os object
};


/*! \class thread  
 *  \brief This class implements a thread \see HAL */
class thread  :  public threadbase
{
	FORBID_COPY(thread);

public:
	 /*! constructor. */
     thread(const char* name,			//!< name of the thread
					 int priority,				//!< priority 0..99
					 void* ref,					//!< value will be pased to virtual Execute methode
					 int cpu=-1,				//!< cpu affinity
					 bool crrunning=true,		//!< if true thread will run after creation or hold if false	
					 unsigned int stacksize=0	//!< stacksize of the thread ( 0= default stacksize for plattform
					 );	
	 /*! destructor. */ 
	 virtual ~thread();
	 
	 /*! Resume() resumes a thread created with crrunnung=false */ 
	 void Resume();
	 /*! Terminate() terminates a running thread */ 
	 virtual bool Terminate();		  

	 /*! pure virtual Execute() methode */
	 virtual unsigned int Execute(void * ref /*!<  ref */
		 )  {   return 0;  }
     
	 
};

/*! \class basemutex  
 *  \brief This class is a base locking mutex ( binary semaphore ) \see HAL */
class basemutex // :  public CRTCLASS_sysbaseobject				sb:??
{
	FORBID_COPY(basemutex);
	
  protected:
     char * m_name;	
  public:
	 /*!
	  A constructor. */ 
	 basemutex(const char* name);
	 /*! 
	  A destructor. */ 
	 virtual ~basemutex();
    
	 /*! lock() waits for the mutex
	     */
     virtual void lock()=0;
     /*! unlock() releases the mutex
	     */
	 virtual void unlock()=0;
};



/*! \class mutex  
 *  \brief This class implements a mutex ( binary semaphore ) \see HAL */
class mutex  :  public  basemutex
{
	FORBID_COPY(mutex);
	
 public:
	 /*!
	  A constructor. */ 
	 mutex(const char* name);
	 /*! 
	  A destructor. */ 
	 virtual ~mutex();
    
	 /*! lock() waits for the mutex
	     */
     virtual void lock();
     /*! unlock() releases the mutex */
	 virtual void unlock();

	 /*! reset() resets the mutex */
	 virtual void reset();


protected:
	

	size_t imp_buf_size;	//!<  size of os object bufferspace
	unsigned char * imp_buf;		//!<  buffer space for os object
};



/*! \class rtthread  
 *  \brief This class implements a thread \see HAL */
class rtthread  :  public threadbase
{

	FORBID_COPY(rtthread);

  public:
	 /*! A constructor. 
	      \param name		- name of the thread
		  \param priority   - a value [1..99] 
		  \param ref        - value will be pased to virtual Execute methode
		  \param cpu        - cpu affinity
		  \param crrunning  - if true thread will run after creation or hold if false 
		  \param stacksize  - stacksize of the thread ( 0= default stacksize for plattform )
		  */ 
     rtthread(const char* name,int priority,void* ref,int cpu=-1,bool crrunning=true,unsigned int stacksize=0);
	 /*! A destructor. */ 
	 virtual ~rtthread();
	 
	 void Suspend();
	 /*! Resume() resumes a thread created with crrunnung=false */ 
	 void Resume();
	 /*! Terminate() terminates a running rtthread */ 
	 virtual bool Terminate();		  

	 /*! pure virtual Execute() methode */
	 virtual unsigned int Execute(void * ref)  {   return 0;  }
	 int get_nextcpu();

}; 
//////////////!!!!!!!!! make the implementation according to new system.
/*! \class rttestthread 
 *  \brief Real Time Test Thread
 *
 * 
*/
class rttestthread : public rtthread
{
	protected:
	etherberry_9252* m_device;
	
	public:
	 rttestthread(etherberry_9252* device);	
	 /*! destructor. */ 
	 virtual ~rttestthread();
	 
	
	 /*! pure virtual Execute() methode */
	 virtual unsigned int Execute(void * ref /*!<  ref */);
	 void statistics(){};//!!
	 
};



#endif