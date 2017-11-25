#ifndef _IONET_H__
#define _IONET_H__

#include "core.h"
#include "connection.h"
//---------------------------------------------------------------------------------------

class Buz; 

#define MAX_DNSES   1400
//-----------------------------------------------------------------------------
class LocalDNS 
{
    struct DIpPair
    {
        DIpPair(KXCHAR* d, KXCHAR* s){
            strcpy(sdoma, d);
            strcpy(sip, s);
        }
        DIpPair(const DIpPair& r){
            strcpy(sdoma, r.sdoma);
            strcpy(sip, r.sip);
        }
        const DIpPair& operator=(const DIpPair& r){
            if(&r==this)return *this;
            strcpy(sdoma, r.sdoma);
            strcpy(sip, r.sip);
            return *this;
        }
        XCHAR   sdoma[96];
        XCHAR   sip[32];
    };
    deque<DIpPair>  _dns;

public:
    INLN KXCHAR*    FindDomain(KXCHAR* sip)
    {
        deque<DIpPair>::iterator b = _dns.begin();
        deque<DIpPair>::iterator e = _dns.end();
        for(;b!=e;b++)
        {
            if(!strcmp(b->sip,sip))
                return b->sip;
        }
        return 0;
    }

    INLN KXCHAR*    FindIP(KXCHAR* sdoma)
    {
        deque<DIpPair>::iterator b = _dns.begin();
        deque<DIpPair>::iterator e = _dns.end();
        for(;b!=e;b++)
        {
            if(!strcmp(b->sdoma,sdoma))
                return b->sip;
        }
        return 0;
    }
    void Insert(KXCHAR* d, KXCHAR* i);

    void Save();
    void Load();
};


//---------------------------------------------------------------------------------------
class NetIO
{  
public:

    enum IOERR {
        IO_INPROGRESS   = 0,
        IO_FULL         ,
        IO_DUPLICATE    ,
        IO_ERROR        ,
        IO_SSLERR       ,
        IO_CONNERR
    };

    friend class Conection;
    NetIO(XCHAR* proxy, int proxiport, int maxconn);
    ~NetIO();

    IOERR Open(SpUrl& refUrl);
    int   FdSet();
	void  Clear(SpUrl& refUrl);
    int   Check(Buz* peng, int counter);
    void  TestTimeouts(Buz* peng);
    void  ShutDown(Buz* pbe);
    void  DropConnections(KSpUrl& refUrl);
    int   GetConections();//{return _nsocks;}
    int   GetUrls(XCHAR* det, int length);
    KXCHAR* SzIP(SpUrl& refUrl);
    void    Flush(Buz* peng, int counter);
    void   CloseSockets();
private:
    BOOL  _IsHere(SpUrl& refUrl);
    void  _ProcRecData(Buz* peng, Conection* pst, int);
    void  _KillSocket(Conection* );

private:
    fd_set	    _r,_w;
    Conection*  _psocks[64];
    int         _nsocks;
    int         n_proxiport;
    XCHAR*      s_proxiaddr;
    int         n_maxconn;
    XCHAR*      ps_urls;
    m_mutex     o_mutex;
    LocalDNS    _dns;
    BOOL        _socketsChanged;
};


#endif //
