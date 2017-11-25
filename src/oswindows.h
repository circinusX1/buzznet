#ifndef __OSWINDOWS_H__
#define __OSWINDOWS_H__ 

#ifndef _MT 
    #define _MT
#endif //

#pragma warning (disable: 4996)
#define _WINSOCKAPI_
#define __WINCRYPT_H__
#include <windows.h>
#include <winsock2.h>
#include <process.h>
#include <direct.h>
#include <stdio.h>
#include <tchar.h>
#include <stdarg.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include "sys/stat.h"

#pragma comment(lib,"Wsock32.lib")

//-----------------------------------------------------------------------------
#define NILLA   0x80000000
#define INLN    __forceinline
#define REG
#define NO_VT   __declspec(novtable)
#define VIRT
#define NAKED_  __declspec( naked )
#define ASM_    __asm
#define nvtclass    class __declspec(novtable)
#define OUT_
#define OVERWRITE
#define OPBS  '/'
#define BS    '\\'
#define BSS   "\\"
//-----------------------------------------------------------------------------
#define     str_len     (int)strlen
#define     t_strtok    strtok
#define     t_atoi      atoi
#define     t_printf    printf   
#define     t_sprintf   sprintf  
#define     t_strncmp   strncmp

#define f_open      fopen
#define f_read      fread
#define f_create    fopen 
#define f_close     fclose
#define f_write     fwrite 
#define f_printf    fprintf
#define f_puts      fputs 
#define f_gets      fgets 
#define f_eof       feof
#define f_error     ferror
#define s_tat      _stat
#define f_stat     _fstat

#define un_link     _unlink
#define mk_dir      _mkdir
#define sn_printf   _vsnprintf



namespace os
{
//file    
    typedef FILE     H_FILE;
 //XCHAR
    INLN void    ge_getch(){getch();};
    INLN void    ge_sleep(DWORD milisecs){Sleep(milisecs);}
    INLN void    ge_idle(){Sleep(8);}
    INLN DWORD   ge_gettick(){return GetTickCount();}
    INLN long    ge_error(){return GetLastError();}
    INLN void    ge_error(long err){SetLastError(err);}
    INLN time_t       str_time(char* dTime){//++++
            struct tm   tt = {0};
            sscanf(dTime,"%d-%d-%d %d:%d:%d",&tt.tm_year, 
                                            &tt.tm_mon, 
                                            &tt.tm_mday, 
                                            &tt.tm_hour, 
                                            &tt.tm_min, 
                                            &tt.tm_sec);
            tt.tm_year -= 1900;
            tt.tm_mon  -= 1;
            return mktime(&tt);
    }
};

//-----------------------------------------------------------------------------
struct m_mutex
{
    mutable CRITICAL_SECTION _cs;
    m_mutex(){
        _inout = 0;
        InitializeCriticalSection(&_cs);
    }
    ~m_mutex(){
        if(_inout) LeaveCriticalSection(&_cs); 
        DeleteCriticalSection(&_cs);
    }
    int Lock() {
        if(_inout)
            return 1;
        EnterCriticalSection(&_cs);
        _inout=1;
        return 0;
    }
    int TryLock() 
    {
         if(_inout)
            return 1;
        #if(_WIN32_WINNT >= 0x0400)
            if(TryEnterCriticalSection(&_cs))
            {
                _inout=1;
                return 1;
            }
            return 0;
        #else
            EnterCriticalSection(&_cs);
            _inout=1;
            return 1;
        #endif
    }
    int Unlock() { 
        if(0==_inout)
            return 1;
        _inout=0;
        LeaveCriticalSection(&_cs); 
        return 1; 
    }
    long _inout;
};

//-----------------------------------------------------------------------------
struct semaphore
{
    HANDLE _sem;
    semaphore( int init = 0 ){
        _sem = CreateSemaphore(0,init,0x7fffffff,0);
    }
    ~semaphore(){ CloseHandle(_sem); }
    int Wait(int to = INFINITE) {
        return WaitForSingleObject((HANDLE)_sem,to)==WAIT_OBJECT_0;
    }
    int TryWait(int to = INFINITE) {
        return ((WaitForSingleObject((HANDLE)_sem,to)==WAIT_OBJECT_0) ? 1 : -1);
    }
    int  Notify() const{
        return (ReleaseSemaphore((HANDLE)_sem, 1, 0) ? 0 : -1);
    }

    int  Value() { long val = -1;
        ReleaseSemaphore((HANDLE)_sem,0,&val); return val;
    }
    void Reset( int init = 0 ){
        CloseHandle(_sem);
        _sem = CreateSemaphore(0,init,0x7fffffff,0);
    }
};

class h_event
{
public:
    h_event(int imr=0)
    {
        _he=CreateEvent(0,imr,0,0);
        _waiting=0;
    }
    ~h_event(){   
        if(_waiting)
        {
            SetEvent(_he); 
            wait(1000);
        }
        ::CloseHandle(_he);
    }
    void signal(){
        SetEvent(_he); 
    }
    void reset(){
        ResetEvent(_he); 
    }
    void pulse(){
        PulseEvent(_he); 
    }
    int wait(int miis = INFINITE){
        _waiting=1;
        int rv = WaitForSingleObject( _he,miis ); 
        _waiting=0;
        return rv;
    }

private:
    HANDLE _he;
    long   _waiting;
};


//-----------------------------------------------------------------------------
#pragma warning (disable:4312)
class OsThread
{
public:
    typedef DWORD THANDLE;
    OsThread(){
        _bstop   = 1;
        _hthread = 0;
        _stopped = 1;
    }

    ~OsThread(){
        if(!_stopped)
            Stop();
    }

    virtual int     Start()
    {
        o_mutex.Lock();
        _bstop   = 0;
        _hthread = (THANDLE)_beginthread(SFoo, 8912, (void*)this);
        o_mutex.Unlock();
        os::ge_sleep(100);
        _start.Wait();
        _stopped = 0;
        return 0;
    }

    virtual void IssueStop()
    {
        o_mutex.Lock();
        _bstop = 1;
        o_mutex.Unlock();
    }
    virtual void    Stop()
    {
        o_mutex.Lock();
        _bstop = 1;
        o_mutex.Unlock();
        os::ge_sleep(100);
        Join();
        _stopped = 1;
    }

    BOOL  BStop()
    {
        return _bstop;
    }

    long  IsStopped(DWORD dws)
    {
        return _stopped;
    }
    int Join(){
        return WaitForSingleObject((HANDLE)_hthread,INFINITE);
    }

    int Kill(){
       int rv = TerminateThread((HANDLE)_hthread, 0);
       _hthread = 0;
        _stopped = 1;
       return rv;
    }
    int Detach(){}
    virtual void ThreadFoo(){};

private:
    m_mutex       o_mutex;
    semaphore   _start;
    long        _bstop;
    long        _stopped;
    THANDLE     _hthread;
    static void SFoo(void* pData){
        OsThread* pT = reinterpret_cast<OsThread*>(pData);
        pT->_start.Notify();
        pT->ThreadFoo();
    }
};


//-----------------------------------------------------------------------------
struct AutoLock
{
    AutoLock(m_mutex* m_mutex):o_mutex(m_mutex){if(o_mutex)o_mutex->Lock();}
    ~AutoLock(){if(o_mutex)o_mutex->Unlock();}

    m_mutex* o_mutex;
};

#define _PLATFORM_ "WINDOWS MS"





#endif // !__OS_H__

