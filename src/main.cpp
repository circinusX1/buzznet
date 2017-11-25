// buzznet.cpp : Defines the entryls point for the console application.
//
#include "stdafx.h"
#include "core.h"
#include "url.h"
#include "connection.h"
#include "ionet.h"
#include "buzznet.h"
#include "storage.h"
#include "httpserver.h"
#include "pageparser.h"
//---------------------------------------------------------------------------------------

static BOOL Suspend = FALSE;


extern HttpServer*  Phttp;
static void ControlC (int i)
{
    PRINTF(_D,"Ctrl+C Breaking process\r\n");
    if(Phttp)
    {
        Phttp->Break();
        Phttp->Stop();
    }
    Buz::b_BreaK = TRUE;
}

//---------------------------------------------------------------------------------------
static void SuspendZ(int i)
{
    Suspend = !Suspend;
}

//---------------------------------------------------------------------------------------
#ifdef _CONSOLE
int main(int argc, TCHAR* argv[])
{ 
  
    WsaInit i; 

#ifndef _WIN32
    signal(SIGINT,  ControlC);
    signal(SIGABRT, ControlC);
    signal(SIGTSTP, SuspendZ);
#endif 

    

    do{
        Buz  be;
        HttpServer  srv;
        Phttp = &srv;

        if(!be.Configure("buzznet.conf",1))
        {
            return -1;
        }
        if(Buz::b_httpport)
        {
            srv.Start(Buz::b_httpport,&be);
        }
        be.Run();
    }while(0);
	
    printf("\r\n\r\n*****The Program exits in 30 seconds********************");
#ifdef _WIN32
    getch();
#else
    os::ge_sleep(6000);
#endif //
    return 0;
}
#endif //
