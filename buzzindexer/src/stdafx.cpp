// stdafx.cpp : source file that includes just the standard includes
// indexer.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
#ifdef _WIN32
    #pragma comment(lib, "Gdi32.lib")  
    #pragma comment(lib, "Ws2_32.lib")  
    #pragma comment(lib, "Iphlpapi.lib")

    #pragma comment(lib,"../../mysql/lib/libmySQL.lib")
#ifdef _DEBUG
    #pragma comment(lib, "../../zlib/zlib-1.2.3/projects/visualc6/win32_lib_debug/zlibd.lib")
#else
    #pragma comment(lib, "../../zlib/zlib-1.2.3/projects\visualc6/win32_lib_release/zlib.lib")

#endif //

#endif //
