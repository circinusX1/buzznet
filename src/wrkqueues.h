#ifndef __WRK_QUEUE_H__
#define __WRK_QUEUE_H__

#include "hashvisit.h"
#include "url.h"


class Storage;
class WrkQueues
{
public:
    WrkQueues(void);
    ~WrkQueues(void);
    
    void Lint2Parent(Storage* pstorage){p_storage = pstorage;}
    void Save(XCHAR* fileName, int removeelems, BOOL final=0);
    void SaveExternalURLs(XCHAR* fileName);
    int  Load(XCHAR* fileName, BOOL firsttime);
    int  GetSize(){return (int)q_work.GetSize();}
    int  GetExtSize(){return (int)v_externs.size();}
    void EraseUrl(KSpUrl& url);
    void EraseAll();
    BOOL CheckExternalURLs();
    int  ForceInQ(KSpUrl& url);
    int  PushToExtQ(KSpUrl& url);
    int  PushToQ(KSpUrl& url);
    BOOL IsIn(KSpUrl& url){return h_vis.Find(url.get());};
    int  GetFifo(SpUrl* url);
    void UnVisit(KSpUrl& url){h_vis.Pop((Url*)url.get());}
    void Check();
private:
    Visited             h_vis;
    Dequeue<SpUrl >     q_work;
    vector<SpUrl >      v_externs;
    Storage*            p_storage;
};


#endif //__WRK_QUEUE_H__
