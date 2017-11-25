
#ifndef __COREH_H__
#define __COREH_H__

#include "osg.h"
#include <string>
#include <map>
#include <list>
#include <deque>
#include <set>
#include <vector>
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
			return 1;
        }
		else
  		   printf("Error saving: %s\n",fname)	 ;;
    }
    return 0;
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

template <class X> class counted_ptr
{
public: 
    //explicit counted_ptr(X* p = 0) : _c(0) {if (p) _c = new cnt(p);}
    explicit counted_ptr(X* p = 0) : _c(0) {
        if (p) _c = new cnt(p);
    }
    ~counted_ptr(){dec();}
    counted_ptr(const counted_ptr& r) throw(){add(r._c);}
    counted_ptr& operator=(const counted_ptr& r){if (this != &r){dec(); add(r._c);} return *this;}
    INLN X& operator*()  const throw()   {return *_c->p;}
    INLN X* operator->() const throw()   {return _c->p;}
    INLN const X* get()const{return _c ? _c->p : 0;}
    INLN bool operator==(const counted_ptr<X>& o){return (*o.get() == *this->get());}
private:
    struct cnt {
        cnt(X* p = 0, UINT c = 1) : p(p), c(c) {}
        X*          p;
        UINT        c;
    }* _c;
    INLN void add(cnt* c) throw(){_c = c;if (c) ++c->c;}
    INLN void dec(){ if (_c) {
        if (--_c->c == 0) 
        {
            delete _c->p; 
            delete _c;
        }_c = 0;
    }
    }
};

#endif //__CORE_H__
