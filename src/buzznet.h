#ifndef __BUZZNET_H__
#define __BUZZNET_H__
 
#include <string>
#include <vector>
using namespace std;
#include "core.h"
#include "connection.h" 
#include "statistics.h" 
#include "storage.h"

//---------------------------------------------------------------------------------------
#define _D	0x8
#define _I	0x4
#define _W	0x2
#define _E	0x1

//---------------------------------------------------------------------------------------
class Buz
{
public:
                Buz();
    virtual    ~Buz();
    void        Run();
    BOOL        Configure(KXCHAR* , int);
    BOOL        OnTimeOut(SpUrl& url);
    void        OnConnectError( SpUrl& url);
    SITEEX      PushURL(SpUrl refUrl, const Url* referer=0, int again=0, int where=1);
    size_t      GetStats(XCHAR*, int maxlen);
    void        OnStateChanged();
    void        RecicleFile(XCHAR* server);
    static  void Printf(int what,KXCHAR*,...);
    void        OnRedirect(SpUrl& url);
    int         GetCounter(){return n_counter;}
    int         GetInSelect(){return _inselect;}
    void        CloseSockets();
    Storage*    GetStor(){ return _pRP;}
private:
    void        _HtmlReport();
    void        _Assign(KXCHAR* pred, KXCHAR* val);
    void        _TestPressedKey(BOOL& show);
    void        _Report();
    BOOL        _CheckInputFile();
    void        _DestroyRepository();
    BOOL        _CreateRepository();
    BOOL        _FlushToRemoteBznets();
    
public:
    Storage*                    _pRP;
    NetIO*                      _pIO;
	m_mutex     				o_mutex;
    DWORD                       _inselect;
    int                         n_counter;
    SpUrl                       _curentSite;


public: //--------------config--------------
	static BOOL				   b_BreaK;
    static Statistics           _stats;
    static int                  n_maxconn;
    static DWORD               n_queuelength;
    static int                  n_vissize;
    static XCHAR                s_proxi[128];
    static XCHAR                s_lo[1600];
    static int                  n_proxiport;
    static  BOOL                b_testing;
    static  BOOL                b_externalsites;
    static  int                 u_depth;
    static  int                 n_retries;
    static  int                 n_serverport;
    static  int                 n_linkperpage;
    static  DWORD               n_rechecksite;
    static  XCHAR               s_tempFolder[260];
    static  XCHAR               s_youremail[260];
    static  XCHAR               s_destlocation[260];

    static  XCHAR               s_repFileName[260];
    static  XCHAR               s_queFileName[260];
    static  XCHAR               s_errFileName[260];


    static  XCHAR               s_name[128];
    static  int                 n_grouprule;
    static  int                 m_delaybeturls;
    static  long                dw_timeout;
    static  BOOL                b_useparameters;
    static  BOOL                b_savepageparams;
    static  BOOL                b_checkexistDB;
    static  int                 b_httpport;
    static  DWORD               n_pagespersite;
    static  XCHAR               s_plugrepdbUser[64];
    static  XCHAR               s_plugrepdbPswd[64];
    static  XCHAR               s_plugrepdbDb[64];
    static  XCHAR               s_plugrepdbServer[64];
    static BOOL                 b_denyallow;
    static int                  n_printout;
    static int                  n_maxsitetebles;
    static int                  n_maxPageSize;
	static XCHAR				s_appFolder[260];//     = "app";
	static BOOL					b_pause;
	static int					n_dispersion;
    static BOOL                 b_compressfiles;
    static BOOL                 b_pushRootUrl;
};
 
#define PRINTF      Buz::Printf
extern Buz*         P_Buz;

#endif //__BUZZNET_H__
