#ifndef __DB_STO_H__
#define __DB_STO_H__

#include "mymyysql.h"

class Storage;
class DbSto
{
public:
    DbSto(void){};
    ~DbSto(void){h_sql.Disconnect();};

    void Lint2Parent(Storage* pstorage){p_storage = pstorage;}
    void    CreateTables();
    void    DeleteDocs(KSpUrl& url, KXCHAR* fileName);
    DWORD   GetSiteHits(SpUrl& url);
    void    AddSitedHit(KSpUrl& url);
    void    SaveDocument(KSpUrl& url, KXCHAR* fileName);
    DWORD   SaveSite(KSpUrl& url, DWORD tblIdx, int timeup=0);
    BOOL	HasTimeout(KSpUrl& url, DWORD hours);
    BOOL    Robots(KSpUrl& url);
    void    CreateTables(int bdelete);
    DWORD   GetSiteID(KSpUrl& url);
    BOOL    HasDocument(KSpUrl& url, XCHAR* pout );
    void    CatcheRejectedSites(vector<string>& stopSites);
    void    CatcheRejectedWords(vector<string>& stopW);
	void    StayAlive();
private:
    DWORD   _DB_NextDocID(int inc);
    DWORD   _DB_NextSiteID(int inc);
    MYSQL_ROW  _QrySite(KSpUrl& url);

private:
    string  s_sqlqry;
    MySQL   h_sql;
    Storage* p_storage;
};


#endif //
