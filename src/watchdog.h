#ifndef __WATCH_DOG_H__
#define __WATCH_DOG_H__

#include "osg.h"

//---------------------------------------------------------------------------------------
class Buz;
class WatchDog : public OsThread
{
public:
    WatchDog(Buz* peng):_peng(peng){};
    virtual ~WatchDog(void){};

    void ThreadFoo();

private:
    Buz*     _peng;
};



#endif //
