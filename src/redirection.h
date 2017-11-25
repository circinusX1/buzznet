#ifndef __REDIRETION_H__
#define __REDIRETION_H__


#include "osg.h"
#include "core.h"
//---------------------------------------------------------------------------------------
class Redirection
{
public:
    struct ServerName
    {
        ServerName(KXCHAR* p){strcpy(name,p);}
        XCHAR name[128];
    };

    struct RedRegion
    {
        RedRegion(){};
        XCHAR         region[128];
        XCHAR         ipbuzz[128];
        vector<ServerName>  server;
    };
 
    Redirection()
    {
        _maxsize=0;
    }
    ~Redirection()
    {
        vector<RedRegion*>::iterator b =  _regions.begin();
        vector<RedRegion*>::iterator e =  _regions.end();
        for(;b!=e;b++)
            delete *b;
    }


     void    Set( RedRegion* rr){
        _regions.push_back(rr);
    }

    size_t    Push(KXCHAR* servname, KXCHAR* region)
    {
        vector<RedRegion*>::iterator b =  _regions.begin();
        vector<RedRegion*>::iterator e =  _regions.end();
        for(;b!=e;b++)
        {
            if(!strstr(region, (*b)->region))
            {
                (*b)->server.push_back(ServerName(servname));
                size_t counter = (*b)->server.size();
                if(counter > _maxsize )
                    _maxsize = counter;
                return _maxsize;
            }
        }
        return 0;
    }
    
    size_t              Size()const{return _regions.size();}

    vector<RedRegion*>& GetForCleaning(){return _regions;}
    size_t              GetMaxSize(){return _maxsize;}

private:
    vector<RedRegion*>   _regions;      
    size_t               _maxsize;
};


#endif //__REDIRETION_H__
