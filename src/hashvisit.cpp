#include "hashvisit.h"



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Visited::Visited(size_t nkeys)
{
    n_keys  = (int)nkeys;
    n_size  = (int)(nkeys / 8);
    p_bytes = new BYTE[n_size + 64];
    memset(p_bytes,0,n_size+64);
}

//-----------------------------------------------------------------------------
Visited::~Visited()
{
    delete[] p_bytes;
}  

//-----------------------------------------------------------------------------
BOOL    Visited::Push(const Url* purl)  
{
    DWORD  uk       = ((Url*)purl)->UniqHash(n_keys);
	if(uk==NILLA)	return 0;
    size_t bytes    = uk>>3;
    BYTE bit        = 1<<(uk%8);
    assert(bytes<=n_size);
    BOOL exist      = p_bytes[bytes] & bit;
    p_bytes[bytes] |= bit;
    return exist;
}

//-----------------------------------------------------------------------------
BOOL    Visited::Pop(const Url* purl)
{
    DWORD  uk     = ((Url*)purl)->UniqHash(n_keys);
	if(uk==NILLA)	return 0;
    size_t bytes  = uk>>3;
    BYTE bit      = 1<<(uk%8);
    if(bytes<=n_size)
	{
    	BOOL exist    = p_bytes[bytes] & bit;
    	p_bytes[bytes] &= ~bit;
    	return exist;
	} 
	return 0;
}

//-----------------------------------------------------------------------------
BOOL  Visited::Find(const Url* purl)
{
    DWORD  uk    = ((Url*)purl)->UniqHash(n_keys);
	if(uk==NILLA)
		return 0;
    size_t bytes  = uk>>3;
    BYTE   bit   = 1 << (uk % 8);
    if(bytes<=n_size)
	{
    	return  p_bytes[bytes] & bit;
	}
	return 0;
}
