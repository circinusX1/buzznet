
#ifndef __COREH_H__
#define __COREH_H__

#include "osg.h"
#include <string>
#include <map>
#include <list>
#include <deque>
#include <set>
#include <vector>
//#include "url.h"  
using namespace std;

//-----------------------------------------------------------------------------
#ifdef _WIN32
    #pragma warning (disable:4786)
#endif //
 

//---------------------------------------------------------------------------------------
class Cfm
{
public:
	Cfm(XCHAR* p,...);
    operator KXCHAR*();
    XCHAR _sc[1024];
};

#define	MKSTR	    (KXCHAR*)Cfm
#define	_F   	    (KXCHAR*)Cfm
#define RET(k_)     k_
//#define NILLA       0xFFFFFFFF
template <class T>T t_min(T a, T b)
{
	if(a<b)return a; 
	return b;
}

template <class T>T t_max(T& a, T& b)
{
	if(a<b)return a; 
	return b;
}

//---------------------------------------------------------------------------------------
INLN BOOL File_Save(const XCHAR* fname, const void* data, size_t length, const XCHAR* mode)   
{
    BOOL  ok=FALSE;
    if(length)
    {
        FILE* pf = f_open(fname,mode);
        if(pf){
            ok = (length==fwrite(data, 1, length, pf));
            fclose(pf);
        }
		else
		printf("Error saving: %s\n",fname)	 ;;
    }
    return ok;
}

INLN BOOL File_Save(const XCHAR* fname, const XCHAR* data, const XCHAR* mode)   
{
    if(data && *data)
    {
        FILE* pf = f_open(fname,mode);
        if(pf){
            fputs(data, pf);
            fclose(pf);
            return 1;
        }
		else
			printf("Error saving: %s\n",fname)	 ;;
    }
    return 0;
}



#endif //__CORE_H__
