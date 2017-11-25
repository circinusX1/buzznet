
#include "core.h" 
#include "url.h"
 

//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
Cfm::operator KXCHAR*()
{
    return Cfm::_sc;
}




