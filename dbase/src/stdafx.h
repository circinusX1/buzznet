// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _NO_INCLUDE_SOCK
#define _WINSOCK2API_

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include "../../src/iurl.h"
#include "../../src/urlrepository.h"


// TODO: reference additional headers your program requires here
//---------------------------------------------------------------------------------------
class Cfm
{
public:
	Cfm(XCHAR* p,...);
    operator const XCHAR*();
    XCHAR _sc[1024];
};
#define	_F	(const XCHAR*)Cfm
