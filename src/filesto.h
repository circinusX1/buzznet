#ifndef __FILE_STO_H__
#define __FILE_STO_H__


#include "core.h"
#include "url.h"

class PageParser;
class Storage;
class FileSto
{
public:
    FileSto(void){};
    ~FileSto(void){};

    KXCHAR* GetFileName(KSpUrl& url, BOOL target, int cread=0);
    void  Lint2Parent(Storage* pstorage){p_storage = pstorage;}
    void  DeleteFile(KXCHAR* filename);
    BOOL  SaveDocument(KXCHAR* pName, KXCHAR* page, int length, KSpUrl& url, const   PageParser& pp, XCHAR* retBuff=0);
    BOOL  SaveRobots(XCHAR* pageIn, size_t length, KSpUrl& url);
    void  LoadRobotsFile(KSpUrl& url, vector<string>&   vrobotDocs);
    BOOL  HasDoc(KSpUrl& url);
    void  LogErrorHosts(SpUrl& url, KXCHAR* errStr, XCHAR* errFile);
    BOOL  CheckInputFile();
	BOOL  EnsureDir(KSpUrl& url);
    void  Reset();
private:
    BOOL _IsDir(KXCHAR *szpath);

    Storage*        p_storage;
    set<string>     _dirs;
    XCHAR           s_temp[800];
};


#endif //
