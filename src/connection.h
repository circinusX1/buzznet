#ifndef __CONNECTION_H__
#define __CONNECTION_H__


#include "osg.h"
#include <deque>
#include <string>

extern "C" {
#ifdef WIN32
	#include "../openssl/include/ssl.h"
#else //LINUX
	#include "rand.h"
	#include "evp.h"
	#include "ssl.h"
#endif //!WIN32
};

#include "sock.h" 
#include "url.h"
#include "core.h"


//-----------------------------------------------------------------------------
#define     OVER_BUFF   8
#define     ROBOTS      0x1
#define     TARGET      0x2
#define     CONNECTING  0x4

//-----------------------------------------------------------------------------
#define CHAR_CHUNK  16384
//-----------------------------------------------------------------------------
class NetIO;
class Conection : public tcp_cli_sock
{
public:
    friend class NetIO;

    Conection(SpUrl& url, KXCHAR* proxiaddr, int proxiport);
    virtual ~Conection();
    DWORD     GetDelay(){return os::ge_gettick() - dw_starttime;}
	DWORD      GetStartTime(){return dw_starttime;};
    void      RefreshStartTime(){dw_starttime=os::ge_gettick();}
    void      KillTime(){dw_starttime=0;}
    BOOL      HasTimeouted();//{return os::ge_gettick() - dw_starttime > Buz::dw_timeout;}
    void      SetURL(SpUrl& purl){r_url = purl;}
    int       LastQuerry(){return w_asking;}
    void      QueryDone(int inorogress);
    int       Append(const BYTE* data, int length);
    BYTE *    GetContent(){return p_response;}
    BOOL      HasQuerry(){return s_request.length()>0;}
    void      ClearQuerry(){s_request.clear();};
    int      GetContentLength(){return n_accumulated;}
    KXCHAR* GetPageName(){return s_pagename.c_str();}
    SpUrl&    GetURL(){return r_url;}
    void     Kill(){
        if(p_response)
        {
            delete[] p_response;
        }
        p_response=0;
        b_killed=1; 
        this->destroy();
        }
    BOOL     IsDead(){return b_killed;}

    virtual void   FormatContentRequest();    
    virtual void   FormatRobotsRequest();
    virtual BOOL Connect(NetIO* pIo, KXCHAR *inetaddr, int port, int timeout);
    virtual int  Send();
    virtual int  Receive(BYTE* buff, int sizeb);

protected:
    SpUrl                   r_url;
    BOOL                    b_inuse;
    DWORD                   dw_starttime;
    string                  s_request;
    string                  s_pagename;
    BYTE *                  p_response;
    int                     n_chunksz;
    int                     n_accumulated;
    KXCHAR                  *s_proxiaddr;
    int                     n_proxiport;
    DWORD                   w_asking;
    int                     n_lasterror;
    BOOL                    b_killed;
};

class SSL_Conection : public Conection
{
public:
    SSL_Conection(SpUrl& url, KXCHAR* proxiaddr, int proxiport):
                  Conection(url,proxiaddr, proxiport)
                  {
                      p_ctx     = 0;
                      p_ssl     = 0;
                      p_meth    = 0;
                  }
    virtual ~SSL_Conection()
    {
        if(p_ssl)      SSL_free(p_ssl);
        if(p_ctx)      SSL_CTX_free(p_ctx);
    }
    virtual BOOL Connect(NetIO* pIo, KXCHAR *inetaddr, int port, int timeout);
    virtual int  Send();
    virtual int  Receive(BYTE* buff, int sizeb);
    virtual void FormatContentRequest();    
    virtual void FormatRobotsRequest(); 


private:
	SSL_CTX*                p_ctx;
	SSL*                    p_ssl;
	SSL_METHOD*             p_meth;
    string                  s_proxyconnect;
    
};


//===============================================================================
#define     CHUNK_SIZE      1024

#endif //
