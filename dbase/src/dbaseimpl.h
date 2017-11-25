#ifndef _DB_IMPL_H__
#define _DB_IMPL_H__
// dbase.cpp : Defines the entry point for the DLL application.
//
//#define _NO_INCLUDE_SOCK
//#define _WINSOCK2API_
#include <string>

#include "../../src/iurl.h"
#include "../../src/urlrepository.h"

#include "mysql.h"

#ifdef _WIN32
  #pragma comment(lib,"mysql/lib/libmySQL.lib")
#endif //

using namespace std;
#define OK 0
//------------------------------------------------------------ ---------------------------
class MySQL
{
public:
     MySQL()
     {
          queryResult_ptr = 0;
     }


    BOOL     Connect(const XCHAR* hostname, const XCHAR* username, const XCHAR* password)
    {
        mysql_init(&_mysql);

        mysql_options(&_mysql,MYSQL_READ_DEFAULT_GROUP,"WebPoof");
        mysql_options(&_mysql,MYSQL_OPT_COMPRESS,0);
        if (!mysql_real_connect(&_mysql,hostname,username,password,"",0,NULL,0))
        {
            Error();
            return 0;
        }
        return 1;

    }
    void    Disconnect()
    {
        Free();
        mysql_close(&_mysql);
    }

    void Error()
    {
        printf("ERROR: (%s)\r\n",mysql_error(&_mysql));
#ifdef _WIN32
        OutputDebugString(_F("%s\r\n",mysql_error(&_mysql)));
#endif//
    }

    my_ulonglong Qry(const XCHAR* q)
    {
        Free();
        int r =  mysql_query(&_mysql, q);
        if(0==r)
        {
             queryResult_ptr = mysql_store_result(&_mysql);
             
        }
        else
            Error();
        return queryResult_ptr ? mysql_num_rows(queryResult_ptr) : 0;
    }

    unsigned long GetRouwCount()
    {
        if(queryResult_ptr)
            return (unsigned long)mysql_num_rows(queryResult_ptr);
        return 0;
    }

    void Select (const XCHAR* s)
    {
        Free();
        mysql_select_db(&_mysql,s);
    }

    void Free(){
        if(queryResult_ptr)
            mysql_free_result(queryResult_ptr);
        queryResult_ptr = 0;
    }

    MYSQL_ROW Fetch()
    {
        if(queryResult_ptr)
            return mysql_fetch_row(queryResult_ptr);
        return 0;
    }

    XCHAR* Fetch(int index)
    {
        if(queryResult_ptr)
            return mysql_fetch_row(queryResult_ptr)[index];
        return 0;
    }
    
    long FetchLong(int index)
    {
        if(queryResult_ptr)
            return ::atol(mysql_fetch_row(queryResult_ptr)[index]);
        return -1;
    }

public:
    MYSQL       _mysql;
    MYSQL_RES   *queryResult_ptr;
    
};

//---------------------------------------------------------------------------------------
class DbRepository : public UrlRepository
{
public:
    DbRepository();
    virtual ~DbRepository(); 

    void    HitSite(KSpUrl& url);
    void    AddToCurentState(XCHAR *,const KXCHAR *);

    BOOL    Connect(const XCHAR* dl, PfnNewUrl pFn, const XCHAR* host, const XCHAR* dbase, const XCHAR* ussr, const XCHAR* password);
    BOOL    SavePageContent(KXCHAR* pageName, KXCHAR* page, size_t length, KSpUrl& url, KXCHAR* szMeta, KXCHAR* szWords);
    BOOL    PageExist(const SpUrl& url, XCHAR* page=0, size_t length=0);
    BOOL    SaveRobotsContent(XCHAR* page, size_t length, const SpUrl& url);
    BOOL    RobotsAllow(const SpUrl& refUrl2);
    void    OnPageTimeOut(SpUrl& refUrl2);
    void    OnHeaderError(const XCHAR* page, SpUrl& url);
    void    OnConnectError(SpUrl& url);
    void    SaveCurentState(XCHAR* filenme, int lefover=0,BOOL final=0);
    BOOL    RetrievePreviosState(XCHAR* ,BOOL firsttime=0);
    int     PutInWorkingQueue(const SpUrl& url, const Iurl* referer=0, BOOL again=0, int where=1);
    void    DeleteFromWorkingQueue(KSpUrl& url,BOOL search=0);
    int     GetFromWorkingQueue(SpUrl* url);
    BOOL    ReputInWorkingQueue(SpUrl& refUrl, const Iurl* referer=0, int where=1);
    BOOL    HasRobots(const SpUrl& url);
    void    UpdatePagesReportList(const XCHAR* urlName, XCHAR* fileName);
    int     GetWorkingQueueLoad();
    XCHAR*  GetRegionIP(KXCHAR *){return "US";}

private:
    int     _SetupDatabases();
    int     _CreateTables();
    void    _GetURLFileSaveName(KSpUrl& url, XCHAR* fn, XCHAR* destfn, int cread);

     
private:
    string      _qry;
    MySQL       _sql;
    PfnNewUrl   _New;
    string      _destlocation;
};



#endif //_DB_IMPL_H__