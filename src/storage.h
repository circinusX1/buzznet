#ifndef __URL_REPOSIT_H__
#define __URL_REPOSIT_H__


#include "wrkqueues.h"
#include "dbsto.h"
#include "filesto.h"


typedef enum _SITEEX
{
    e_REJECT = 0,
	e_SITEADDED,
    e_MAX_HITS,
    e_OVERLOAD,
}SITEEX;


#define     MAX_CRCS    32
//-----------------------------------------------------------------------
class PageParser;
class Storage 
{
public:

            Storage();
            ~Storage();
    BOOL    SetupDB();
    BOOL    IsDocument(KSpUrl& url, XCHAR* page=0, int length=0);
    BOOL    PassRobots(KSpUrl& refUrl2);
    BOOL    HasRobots(KSpUrl& refUrl2);
    void    OnPageTimeOut(SpUrl& refUrl2);
    void    OnHeaderError(KXCHAR* page,  SpUrl& url);
    void    OnConnectError(SpUrl& url);
    void    OnRedirect( SpUrl& url);
    SITEEX  PushURL(KSpUrl& url, const Url* referer, BOOL again, int where);
    BOOL    PushExternalURL(KSpUrl& url);
    
    SITEEX  PushAgainURL(SpUrl& refUrl, const Url* referer=0 ,int where=1);
    
    BOOL    RejectSiteName(KSpUrl& url);
    BOOL    RejectDocument(KSpUrl& url);

    void    LoadRobots(KSpUrl& url);
    void    OnServerChanged(); //NEW_NEW

    void    CheckQueues();
    BOOL    IsDocumentCRC(KSpUrl& url, XCHAR* page, int length);
    void    OnReceivedContent(KXCHAR* pageName, BYTE* page, int length, SpUrl& url);
    void    OnReceivedRobots(KXCHAR* pagename, BYTE* page, int length, SpUrl& url);
    BOOL    ReadFromStorage();
	BOOL    FilterIn(KSpUrl& url, int iwhere);

    WrkQueues&   GetWQ()     {return   w_queue;}
    FileSto&     GetFileSto(){return   f_storage;}
    DbSto&       GetDBSto()  {return   d_storage;}

private:

    void    _CreateTables();
    
    
private:
    
    WrkQueues           w_queue;
    FileSto             f_storage;
    DbSto               d_storage;

    DWORD               u_crcs[MAX_CRCS];
    int                 n_curcrcPos;
    vector<string>      v_stopWords;
    vector<string>      v_stopSites;
    vector<string>      v_robotDocs;    
    string              s_curSiteName;
    m_mutex             o_mutex;
     
};

#endif //__URL_REPOSIT_H__
