#ifndef __HASH_VISIT_H__
#define __HASH_VISIT_H__

#define STACK_SIZE  4096

#include "url.h"

//-----------------------------------------------------------------------------
class Visited 
{
public:
    typedef set<string> Ucell;
    Visited(size_t nkeys=URL_MAX_KEYS);
    ~Visited();
     
    BOOL    Push(const Url* purl);
    BOOL    Pop(const Url* purl);
    BOOL    Find(const Url* purl);
    void    Clear(){memset(p_bytes,0,n_size);}
private:
     int    n_size;
     BYTE*  p_bytes;
     int    n_keys;
};



//-----------------------------------------------------------------------------
template <class T>class Dequeue
{
public:
    Dequeue()
    {
    } 
	void SetSize(size_t lwn)
	{
		n_maxelems=lwn;
	}
	
    BOOL PushBack(T& rt, m_mutex* pm=0, h_event* pe=0)
    {
        if(pm) pm->Lock();
            _dq.push_back(rt);
        if(pm) pm->Unlock();
        if(pe) pe->signal();
        return _dq.size() < (n_maxelems-2);
    }

    BOOL PushFront(T& rt, m_mutex* pm=0, h_event* pe=0)
    {
        if(pm) pm->Lock();
         _dq.push_front(rt);
        if(pm) pm->Unlock();
        if(pe) pe->signal();
        return (_dq.size() < (n_maxelems-2));
    }

    long GetFifo(T* pt, m_mutex* pm=0, h_event* pe=0)
    {
		size_t size = _dq.size();
		if(size)
		{
			if(pe) pe->wait();
			if(pm) pm->Lock();
			*pt = _dq.front();
			_dq.pop_front();
			if(pm)pm->Unlock();
			if(pe)pe->signal();
		}
		return (long)size-1;
    }

    int GetLifo(T* pt, m_mutex* pm=0, h_event* pe=0)
    {
		int size = _dq.size();
		if(size)
		{
			if(pe) pe->wait();
			if(pm) pm->Lock();
			*pt = _dq.back();
			_dq.pop_backt();
			if(pm)pm->Unlock();
			if(pe)pe->signal();
		}
		return size-1;
    }

    deque<T>& GetInnerQ(){return _dq;}

    size_t GetSize()
    {
        return _dq.size();
    }

    size_t Percentage()
    {
        return (_dq.size()*100)/n_maxelems;
    }
	
	void Clear(){
		_dq.clear();
		}

private:

    deque<T>        _dq;
    size_t          n_maxelems;
};

#endif //
