// dbase.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "dbaseimpl.h"


//--------------------------------------------------------------------------------------
#ifdef _WIN32
    #define EX_IN   __declspec(dllexport)
#else
    #define EX_IN 
#endif //

#ifdef _WIN32
//--------------------------------------------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
#endif //

//--------------------------------------------------------------------------------------
extern "C"
{
    EX_IN UrlRepository*	CreateInstance()
    {
	    return new DbRepository(); 
    }

    EX_IN  void	DestroyInstance(UrlRepository* pr)
    {
	    delete pr;
    }

    EX_IN const char* GetName(void)
    {
        return "DB windows";
    }

};



Cfm::Cfm(XCHAR* p,...)
{
    va_list args;
    va_start(args, p);
#ifdef _WIN32
    ::_vsnprintf(_sc, sizeof(_sc) / sizeof(XCHAR), p, args);
#else
    ::vsnprintf(_sc, sizeof(_sc) / sizeof(XCHAR), p, args);
#endif //
    va_end(args);
}

Cfm::operator const XCHAR*()
{
    return Cfm::_sc;
}
