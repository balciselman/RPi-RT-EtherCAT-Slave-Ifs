/**********************************************************************************/
/*                                                                                */
/* thread.cpp: Thread creation and priority adjustments                           */
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




static unsigned int s_threadunique=0;


static inline unsigned int InterlockedIncrement(volatile unsigned int* xaddr)
{
#if __GNUC__ >  7
	return __atomic_add_fetch(xaddr, 1, 0);
#else
	return __sync_add_and_fetch(xaddr, 1);
#endif
}


const char* fix_threadname(const char* cname, char* buffer, size_t maxsize)
{
	if (strlen(cname) > 12)
	{
		char temp[10];
		strncpy(&buffer[0], cname, maxsize - 1);
		buffer[11] = '@';
		buffer[12] = 0;
		temp[sizeof(temp) - 1] = 0;
		snprintf(&temp[0], sizeof(temp) - 1, "%d", InterlockedIncrement(&s_threadunique) - 1);
		strncat(&buffer[0], &temp[0], maxsize - 1);
		cname = &buffer[0];
	}
	return cname;
}


/*
 * Raise the soft priority limit up to prio, if that is less than or equal
 * to the hard limit
 * if a call fails, return the error
 * if successful return 0
 * if fails, return -1
*/
static int raise_soft_prio(int policy, const struct sched_param* param)
{
    int err;
    int policy_max;	/* max for scheduling policy such as SCHED_FIFO */
    int soft_max;
    int hard_max;
    int prio;
    struct rlimit rlim;

    prio = param->sched_priority;

    policy_max = sched_get_priority_max(policy);
    if (policy_max == -1) {
        err = errno;

        printf("WARN: no such policy\n");
        return err;
    }

    err = getrlimit(RLIMIT_RTPRIO, &rlim);
    if (err) {
        err = errno;
        printf("WARN: getrlimit failed err=%d !", err);
        return err;
    }

    soft_max = (rlim.rlim_cur == RLIM_INFINITY) ? policy_max : rlim.rlim_cur;
    hard_max = (rlim.rlim_max == RLIM_INFINITY) ? policy_max : rlim.rlim_max;

    if (prio > soft_max && prio <= hard_max) {
        rlim.rlim_cur = prio;
        err = setrlimit(RLIMIT_RTPRIO, &rlim);
        if (err) {
            err = errno;
            printf("WARN: setrlimit failed status=%d !", err);
            /* return err; */
        }
    }
    else {
        err = -1;
    }

    return err;
}



/*
 * Check the error status of sched_setscheduler
 * If an error can be corrected by raising the soft limit priority to
 * a priority less than or equal to the hard limit, then do so.
 */
static int setscheduler(pid_t pid, int policy, const struct sched_param* param)
{
    int err = 0;

try_again:
    err = sched_setscheduler(pid, policy, param);
    if (err) {
        err = errno;
        if (err == EPERM) {
            int err1;
            err1 = raise_soft_prio(policy, param);
            if (!err1) goto try_again;
        }
    }

    return err;
}









threadbase::threadbase(const char* name, int priority, void* ref, int cpu, bool crrunning, unsigned int stacksize) : genericthread()
{
    m_isrt = 0;
    m_priority = priority;
    m_ref = ref;
    m_activ = false;
    m_cpu = m_rcpu = cpu;
    m_crrunning = crrunning;
    imp_buf_size = 0;
    imp_buf = NULL;
}

threadbase::~threadbase()
{

}

unsigned int threadbase::ExecThread(void* ref)
{
    unsigned int stat;
    struct sched_param schedp;
    
    memset(&schedp, 0, sizeof(schedp));
        
    if (m_isrt)
    {
        
        schedp.sched_priority = m_priority;
        if (setscheduler(0, SCHED_FIFO, &schedp))
            printf("thread: failed to set priority to %d\n", m_priority);
    }
    if (m_rcpu != -1)
    {
        cpu_set_t mask;

        // reset affinity
        CPU_ZERO(&mask);

        CPU_SET(m_rcpu, &mask);
        if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) == -1)
            printf("Could not set CPU affinity to CPU #%d\n", m_rcpu);
    }
    //register_this_thread();
    stat = Execute(ref);
    return stat;
}


typedef struct _HALTHREADID 
{
  pthread_mutex_t m_startmutex;
  pthread_t    m_thread;
} HALTHREADID;


static void* __Start_thread(void* obj)
{
  thread* tobj=NULL;
  unsigned long stat;
  HALTHREADID* pthread;

  tobj=(thread*)obj;

  pthread = (HALTHREADID*)tobj->imp_buf;

  pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, NULL);


  if (tobj)
  {
    // wait for start mutex
    pthread_mutex_lock(&pthread->m_startmutex);



    tobj->m_activ=true;
    stat=tobj->ExecThread(tobj->m_ref);
    tobj->m_activ=false;
    return (void*)stat;
  }
  return NULL;
}






thread::thread(const char* name, int priority, void* ref, int cpu, bool crrunning, unsigned int stacksize) : threadbase(name, priority, ref, cpu, crrunning, stacksize)
{
  HALTHREADID* pthread;

  m_priority = priority;
  m_ref = ref;
  m_activ = false;
  m_cpu = cpu;
  
  m_crrunning = crrunning;

  

//  sys.registerthread(this);
  if (stacksize == 0)
  {
      stacksize = 128 * 1024;
  }

  imp_buf_size = sizeof(HALTHREADID);
  imp_buf = (unsigned char*)malloc(imp_buf_size);
  if (imp_buf)
  {
    memset(imp_buf, 0, imp_buf_size);
    pthread = (HALTHREADID*)imp_buf;


  #if defined(__GNUC__)
    cpu_set_t cpus;
    int nproc;
    pthread_attr_t attr;

    pthread_mutex_init(&pthread->m_startmutex, NULL);
    pthread_mutex_lock(&pthread->m_startmutex);

    pthread_attr_init(&attr);
    if (stacksize!=0)
    {  
      pthread_attr_setstacksize(&attr,(size_t)stacksize);
    }

    nproc = sysconf(_SC_NPROCESSORS_ONLN);
    pthread_create (&pthread->m_thread,&attr,&__Start_thread, (void *)this);
    if ((cpu < nproc) && (cpu !=-1))
    {
        CPU_ZERO(&cpus);
        CPU_SET(cpu, &cpus);
        sched_setaffinity(pthread->m_thread, sizeof(cpu_set_t), &cpus);
    }
    pthread_attr_destroy(&attr);
    if (crrunning)
    {
      pthread_mutex_unlock(&pthread->m_startmutex);
    }
  #endif
  }
}

void thread::Resume()
{
  HALTHREADID* pthread;

  pthread = (HALTHREADID*)imp_buf;

#if defined(__GNUC__)

  pthread_mutex_unlock(&pthread->m_startmutex);
#endif
}

thread::~thread()
{

  if ((imp_buf != NULL) && (imp_buf_size != 0))
  {
    Terminate();

    imp_buf_size = 0;
    free(imp_buf);
    imp_buf = NULL;
  }
  m_activ = false;
 // sys.unregisterthread(this);
}

bool thread::Terminate()
{
  HALTHREADID* pthread;
  pthread = (HALTHREADID*)imp_buf;
  bool term = false;

  if ((imp_buf != NULL) && (imp_buf_size != 0))
  {

#if defined(__GNUC__)
    if (pthread->m_thread)
    {
      pthread_mutex_destroy(&pthread->m_startmutex);

      pthread_cancel (pthread->m_thread);
      pthread_join (pthread->m_thread, NULL);
      memset(&pthread->m_thread,0,sizeof(pthread->m_thread));
      term=true;
    }	
#endif
  }
  if (term)
  {
    m_activ = false;
  }
  return term;
}



/* static */
void* genericthread::sget_threadid()
{
    void* id = NULL;
#if defined(__GNUC_LINUX__)
    pthread_t tid;
    
    tid=pthread_self();
    id = (void*)(tid);
#endif
    return id;
}

#if defined(USE_THIS)

/*static*/
genericthread* genericthread::get_curthread()
{
    void* id;

    pthread_t tid;
    
    tid=pthread_self();
    id = (void*)(tid);

    return id;
}

#endif




//static HashOwnerNumericList<genericthread> ms_threadlist; 

void genericthread::register_this_thread()
{
    void* id;
    char namebuf[64];
#if defined(__GNUC__)
    const char* cname;
#endif    

    id = genericthread::sget_threadid();

    namebuf[sizeof(namebuf) - 1] = 0;
    snprintf(&namebuf[0], sizeof(namebuf) - 1, "%llu", (unsigned long long)(id));
 //   ms_threadlist.add(this, (unsigned long long)(id));                             // sb:!!!!
#if defined(__GNUC__)
    // name the thread
    cname=fix_threadname(Name(), &namebuf[0], sizeof(namebuf) - 1);
    pthread_setname_np((pthread_t)(id), cname);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





rtthread::rtthread(const char* name,int priority,void* ref,int cpu,bool crrunning,unsigned int stacksize) : threadbase(name,priority,ref,cpu,crrunning,stacksize)
{
	
   HALTHREADID* pthread;
   m_priority = priority;
   m_ref = ref;
   m_activ = false;
   m_crrunning = crrunning;
   m_isrt = 1;
   imp_buf_size = sizeof(HALTHREADID);
   imp_buf      = (unsigned char*)malloc(imp_buf_size);
   memset(imp_buf,0,imp_buf_size);
   pthread = (HALTHREADID*)imp_buf;

   if (stacksize == 0)
   {
	   stacksize = 128 * 1024;
   }


  pthread_mutex_init(&pthread->m_startmutex,NULL);
  pthread_mutex_lock(&pthread->m_startmutex);


  pthread_attr_t attr;
  sched_param    schparam;

  pthread_attr_init(&attr);

  if (stacksize != 0)
  {
	  pthread_attr_setstacksize(&attr, (size_t)(stacksize));
  }

  // for preempt_rt
  pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
  pthread_attr_setschedparam(&attr,&schparam);
  cpu_set_t cpus;
  int nproc;
  nproc = sysconf(_SC_NPROCESSORS_ONLN);
  pthread_create(&pthread->m_thread, &attr, &__Start_thread, (void*)this);
  if (cpu == -1)
  {
	  m_rcpu = cpu = get_nextcpu();
  }
  if (cpu < nproc)
  {
	  CPU_ZERO(&cpus);
	  CPU_SET(cpu, &cpus);
	  sched_setaffinity(pthread->m_thread, sizeof(cpu_set_t), &cpus);
  }
  {
	  char namebuf[64];
	  namebuf[sizeof(namebuf) - 1] = 0;

	  const char* cname = fix_threadname(Name(), &namebuf[0], sizeof(namebuf) - 1);
	  pthread_setname_np(pthread->m_thread, cname);
  }
  pthread_attr_destroy(&attr);
  if (crrunning)
  {
	  pthread_mutex_unlock(&pthread->m_startmutex);
  }
}


void rtthread::Resume()
{
	HALTHREADID* pthread;

	pthread = (HALTHREADID*)imp_buf;

	pthread_mutex_unlock(&pthread->m_startmutex);
  
}

void rtthread::Suspend()
{
    HALTHREADID* pthread;

    pthread = (HALTHREADID*)imp_buf;

    pthread_cancel(pthread->m_thread);
}

rtthread::~rtthread()
{
	HALTHREADID* pthread;

	pthread = (HALTHREADID*)imp_buf;


	if ((imp_buf!=NULL) && (imp_buf_size!=0))
	{
		Terminate();
		imp_buf_size = 0;
		free(imp_buf);
		imp_buf=NULL;
	}
	m_activ=false;	
	//sys.unregisterrtthread(this);
}


bool rtthread::Terminate()
{
    HALTHREADID* pthread;
	pthread = (HALTHREADID*)imp_buf;
    bool term=false;
	
	if ((imp_buf!=NULL) && (imp_buf_size!=0))
	{
	  if (pthread->m_thread)
	   {
		 pthread_mutex_destroy(&pthread->m_startmutex);

		 pthread_cancel (pthread->m_thread);
		 pthread_join (pthread->m_thread, NULL);
		 //pthread->m_thread = NULL;
		 term=true;
	    }	
	}
	if (term)
	  {
		m_activ=false;	
	  }	
	return term;
}

static unsigned int sc_cpu = 0;

int rtthread::get_nextcpu()
{
    static int nproc=-1;
    int cpu;

    if (nproc == -1)
    {
        nproc = sysconf(_SC_NPROCESSORS_ONLN);
        sc_cpu = (nproc - 1);
    }
    InterlockedIncrement(&sc_cpu);   //
    cpu = (sc_cpu % nproc);
    return cpu;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

basemutex::basemutex(const char* name) //: CRTCLASS_sysbaseobject(name, objtyp)
{
   m_name=new char[strlen(name)+1];
   strcpy(&m_name[0],name);

}


basemutex::~basemutex()
{
  if (m_name)
    {

      delete [] m_name;
      m_name = NULL;

    }
}


mutex::mutex(const char* name) : basemutex(name)
{
#if defined(__GNUC__)
  sem_t** p_sema;
#endif

  imp_buf_size = 0;
  imp_buf = NULL;

#if defined(__GNUC__)
  imp_buf_size =sizeof(sem_t);
#endif

  imp_buf = (unsigned char*)malloc(imp_buf_size);
  if (imp_buf)
  {
    memset(imp_buf, 0, imp_buf_size);


#if defined(__GNUC__)
    p_sema = (sem_t**)imp_buf;
    /* Open a named semaphore NAME with open flags OFLAG.  */
    *p_sema = sem_open(&m_name[0], O_CREAT, ALLPERMS, 1); /* create with initial free */
    if (*p_sema == SEM_FAILED)
    {
        *p_sema = NULL;
//        fprintf(stderr,"Cannot open semaphore %s", &m_name[0]);
    }
#endif
  }
}


mutex::~mutex()
{
#if defined(__GNUC__)
    sem_t** p_sema;
#endif
    if ((imp_buf != NULL) && (imp_buf_size != 0))
    {
#if defined(__GNUC__)
        p_sema = (sem_t**)imp_buf;
        sem_close(*p_sema);
#endif
        free(imp_buf);
        imp_buf = NULL;
        imp_buf_size = 0;
    }
 //   sys.unregistermutex(this);
}


void mutex::lock()
{
#if defined(__GNUC__)
    sem_t** p_sema;
#endif
#if defined(__GNUC__)
    p_sema = (sem_t**)imp_buf;
    sem_wait(*p_sema);
#endif
}


void mutex::unlock()
{
#if defined(__GNUC__)
    sem_t** p_sema;
#endif
#if defined(__GNUC__)
    p_sema = (sem_t**)imp_buf;
    sem_post(*p_sema);
#endif
}

void mutex::reset()
{
#if defined(__GNUC__)
    sem_t** p_sema;
    int v = 1;

    p_sema = (sem_t**)imp_buf;
    sem_getvalue(*p_sema, &v);
    if (v == 0)
    {
        sem_post(*p_sema);
    }
#endif
}

// Thread

// Priority set to 98!
	rttestthread::rttestthread(etherberry_9252* device) : rtthread("rttestthread",88,NULL,-1,false,0)
	{
      m_device=device;
     
  }

  rttestthread::~rttestthread()
	{
	}
  
	unsigned int rttestthread::Execute(void * ref )
	{ 
    if (m_device)
      {
        return m_device->execute_fromthread();
    }
    return 0xFFFFFFF;
  }
