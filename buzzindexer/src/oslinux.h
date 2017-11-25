#ifndef __OSLINUX_H__
#define __OSLINUX_H__

#ifndef _MT
    #define _MT
#endif // 

#define   SOCKET_ERROR   -1
#define   INVALID_SOCKET -1
#define    INFINITE      NILLA
//typedef int socklen_t;

#include <stdarg.h>
#include <errno.h>
#include <resolv.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <asm/ioctls.h>
#include <assert.h>
#include <string.h>
#include <netdb.h>
//#include <tcpd.h>
#include <curses.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>


#define INLN        inline
#define REG
#define NO_VT   
#define VIRT
#define NAKED_  
#define ASM_        asm
#define nvtclass    class
#define OUT_
#define OVERWRITE
#define OPBS  '/'
#define BS    '\\'
#define BSS   "\\"

//-----------------------------------------------------------------------------            

typedef unsigned short   WORD;
typedef unsigned long    DWORD;
typedef unsigned int     UINT;
typedef char             TCHAR;
typedef int              BOOL;
typedef unsigned char    BYTE;
#define TRUE             1
#define FALSE            0

#define  NILLA           0x80000000   
#define  LOWORD(_x)      (_x&0xFFFF)
#define  HIWORD(_x)      ((_x&0xFFFF0000)>>16)
#define  MAKELONG(a,b)   (a|(b<<16))
#define  ASSERT          assert

#define str_len     (int)strlen
#define f_open      fopen
#define f_read      fread
#define f_create    fopen 
#define f_close     fclose
#define f_write     fwrite 
#define f_puts      fputs 
#define f_printf    fprintf 
#define f_gets      fgets 
#define f_stat      fstat
#define t_sprintf   sprintf
#define stat_t      stat


#define un_link     unlink
#define mk_dir(a_)  mkdir(a_, 0777)
#define sn_printf   vsnprintf
#define t_printf    printf
#define t_atoi      atoi
#define t_strncmp   strncmp
#define t_strtok    strtok


namespace os
{
//public:
    typedef FILE     H_FILE;
//file

//char
    INLN void    ge_getch(){;};
	INLN void    ge_sleep(DWORD milisecs){usleep(milisecs * 1000);};
	INLN void    ge_idle(){usleep(8000);};
	INLN long    ge_error(){return errno;};
	INLN void    ge_error(long err){errno=err;};
	INLN DWORD   ge_gettick()
	{
        struct timeval     t;
        ::gettimeofday(&t, NULL);
        return (t.tv_sec * 1000) + (t.tv_usec / 1000);
    }

};

//-----------------------------------------------------------------------------            
class m_mutex
{
    mutable pthread_mutex_t _mut;
public:
    m_mutex()
    {
        pthread_mutexattr_t  attr;

		::memset(&attr,0,sizeof(attr));
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&_mut,&attr);
        pthread_mutexattr_destroy(&attr);
    }

    virtual ~m_mutex(){ 
        pthread_mutex_unlock(&_mut); 
        pthread_mutex_destroy(&_mut); }

    int Lock() const{ 
        return pthread_mutex_lock(&_mut); 
    }

    int TryLock() const { 
        return pthread_mutex_trylock(&_mut); 
    }

    int Unlock() const{ 
        return pthread_mutex_unlock(&_mut); 
    }
};

//-----------------------------------------------------------------------------            
class c_semaphore
{
    sem_t _sem;

public:
    c_semaphore( int init = 0 )
    {
        ::sem_init(&_sem,0,init); 
	}

    ~c_semaphore()
	{ 
        ::sem_destroy(&_sem); 
    }

    INLN int Wait(int to=-1) const 
	{ 
        ::sem_wait((sem_t *)&_sem);
        return 1;
    }

    INLN int TryWait(int to=-1) const
	{ 
        return (sem_trywait((sem_t *)&_sem)?errno:0); 
    }

    int Notify() const
	{ 
        return (sem_post((sem_t *)&_sem) ? errno : 0); 
    }

    int Value() const { 
        int val = -1; 
        sem_getvalue((sem_t *)&_sem,&val); 
        return val; 
    }
    void Reset( int init = 0 ){ 
        ::sem_destroy(&_sem); 
        sem_init(&_sem,0,init); 
    }
};

class h_event
{
public:
    h_event(BOOL mr=0, const char* name=0)
    {
        pthread_cond_init(&_he,0);
    }
    ~h_event(){   
        if(_waiting)
        {
            pthread_cond_broadcast(&_he); 
            os::ge_sleep(1000);
        }
        pthread_cond_destroy(&_he);
    }
    void signal(){
        pthread_cond_broadcast(&_he); 
    }
    void reset(){
        ;
    }
    void pulse(){
        pthread_cond_broadcast(&_he); 
    }
    int wait(int miis=NILLA)
	{
        _waiting=1;
        pthread_mutex_t t;
		
		pthread_cond_wait(&_he, &t ); 
		
        _waiting=0;
        return 0;
    }

private:
    pthread_cond_t _he;
    long           _waiting;
};


//-----------------------------------------------------------------------------            
class AutoLock
{
public:
    AutoLock(m_mutex* mutex):_mutex(mutex){if(_mutex)_mutex->Lock();}
    ~AutoLock(){if(_mutex)_mutex->Unlock();}
private:
    m_mutex* _mutex;
};

//-----------------------------------------------------------------------------    
class OsThread
{
public:
    typedef pthread_t THANDLE;
    OsThread(){
        _bstop   = 1;
        _hthread = 0;
    }

    virtual ~OsThread(){
        long stp=0;
        _mutex.Lock();
        stp = _bstop;
        _mutex.Unlock();
        if(!stp)
        {
            Stop();
            pthread_detach(_hthread);
            _hthread = 0;
        }
    }

    int     Start()
    {
        _mutex.Lock();
        _bstop   = 0;
        _mutex.Unlock();

        pthread_attr_t  attr;
        ::pthread_attr_init(&attr);
        ::pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        if (::pthread_create(&_hthread, &attr, SFoo, this) != 0)
        {
            ::pthread_attr_destroy(&attr);    
            return os::ge_error();
        }
        ::pthread_attr_destroy(&attr);
        os::ge_sleep(100);
        _start.Wait();
        return 0;
    }

    void IssueStop()
    {
        _mutex.Lock();
        _bstop = 1;
        _mutex.Unlock();
    }
    void    Stop()
    {
        _mutex.Lock();
        _bstop = 1;
        _mutex.Unlock();
        os::ge_sleep(100);
        Join();
    }

    BOOL  BStop()
    {
        return _bstop;
    }

    long  IsStopped(DWORD dws)
    {
        os::ge_sleep(dws);
        BOOL bret ;
        _mutex.Lock();
        bret = _bstop;
        _mutex.Unlock();
        return bret;
    }
    INLN int Join()
	{
        return ::pthread_join(_hthread, 0); 
    }

    int Kill(){ 
		return 0;
    }
    int Detach(){return 0;}    
    virtual void ThreadFoo()=0;

private:
    m_mutex     _mutex;
    c_semaphore   _start;
    long        _bstop;
    THANDLE     _hthread;
    long        _stopped;

    static void* SFoo(void* pData)
    {
		OsThread* pT = reinterpret_cast<OsThread*>(pData);
        pT->_mutex.Lock();
        pT->_stopped = 0;
        pT->_mutex.Unlock();
        pT->_start.Notify();
        pT->ThreadFoo();
        pT->_mutex.Lock();
        pT->_stopped = 1;
        pT->_mutex.Unlock();
		return 0;
    }
};

#define _PLATFORM_ "LINUX"

typedef  char TCHAR;
#define _T(c_) c_
#define _tcscpy strcpy
#define _tcscat strcat
#define _tcsicmp strcasecmp
#define _tcsncmp strncasecmp
#define _stprintf t_sprintf




#endif // !__OS_H__
