// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifndef _OS_H__
#define _OS_H__ 


#ifdef _WIN32
    #ifndef WIN32
        #define WIN32
    #endif //
    #define WINDOWS
    #define _CFG_WIN_WIN
    #include "oswindows.h"
#else
    #include "oslinux.h"
#endif //
#ifndef _NO_INCLUDE_SOCK
//    #include "sock.h"
#endif //
#endif //

typedef char         XCHAR; 
typedef const char   KXCHAR;
//typedef wchar_t XCHAR;




// TODO: reference additional headers your program requires here
