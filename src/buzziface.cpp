// buzznet.cpp : Defines the entryls point for the console application.
//
#include "stdafx.h"
#include "core.h"
#include "connection.h"
#include "ionet.h"
#include "buzznet.h"
#include "huffman.h"




//---------------------------------------------------------------------------------------
BOOL    BuzzEngine::_CheckInputFile()
{
#ifdef _FREE
    return 0;
#endif// 
    if(s_inputurlsfile[0])
    {
        if(0!=BuzzEngine::Configure(s_inputurlsfile))
        {
            os::ge_sleep(1000);
            if(0!=BuzzEngine::Configure(s_inputurlsfile))
            {
                return 0;
            }
        }
        return _PushGlobalUrls();
    }
    return 0;
}


//---------------------------------------------------------------------------------------
void    BuzzEngine::_GetURLRecordLocation(const SpUrl& url, XCHAR* fn, XCHAR* destfn, int cread)
{
    if(n_grouprule==0) // bulk
    {
        so::format_ascii(fn);
	    sprintf(destfn,"%s/%s_%s",s_destlocation, url->GetServerName(), fn);
    }
    else if(n_grouprule==1)//by syte
    {
        so::format_ascii(fn);
        if(cread)
            mk_dir( MKSTR("%s/%s", s_destlocation, url->GetServerName()) );
	    sprintf(destfn,"%s/%s/%s",s_destlocation, url->GetServerName(), fn);
    }
    else if(n_grouprule==2)//by ip
    {
        so::format_ascii(fn);
        if(cread)
            mk_dir(MKSTR("%s/%s", s_destlocation, url->GetIP()));
	    sprintf(destfn,"%s/%s/%s",s_destlocation, url->GetIP(), fn);
    }
    else if(n_grouprule==3) //by abolute path
    {
        so::format_ascii(fn);
        if(cread)
            mk_dir(MKSTR("%s/%s/%s" ,s_destlocation, url->GetIP(),url->GetServerName()));
    	sprintf(destfn,"%s/%s/%s/%s",s_destlocation, url->GetIP(),url->GetServerName(), fn);
    }
    else
    {
        char path[512] = {0};

        if(n_grouprule==5)
        {
            sprintf(path, "%s/%s", s_destlocation, url->GetIP());
            if(cread)           
                mk_dir(path);
            sprintf(path, "%s/%s/%s", s_destlocation, url->GetIP(), url->GetServerName());
            if(cread)                
                mk_dir(path);

        }
        else
        {
            sprintf(path, "%s/%s", s_destlocation, url->GetServerName());
            if(cread)
                mk_dir(path);

        }

        size_t last = strlen(path);

        if(strcmp(fn,"robots.txt"))
        {
            const XCHAR* pobject = fn;
            while(*pobject)
            {
                XCHAR nc = *pobject++;
                path[last++] = nc;
                if(nc=='/' && cread)
                    mk_dir(path);
            }
            strcpy(destfn, path);
        }
        else
        {
            if(n_grouprule==5)
            {
                sprintf(destfn,"%s/%s/%s/%s", s_destlocation, url->GetIP(), url->GetServerName(), fn);
            }
            else
            {
                sprintf(destfn,"%s/%s/%s",s_destlocation, url->GetServerName(), fn);
            }
        }
        

    }

}

//---------------------------------------------------------------------------------------
BOOL BuzzEngine::HasRobots(const SpUrl& url)
{
    if(url->HasRobots())
        return 1;

    XCHAR filename[400];
    _GetURLRecordLocation(url,"robots.txt", filename);
    FILE* pf = fopen(filename,"rb");
    if(!pf)
        return FALSE; //no robots
    fclose(pf);
    return TRUE;
}


//---------------------------------------------------------------------------------------
BOOL  BuzzEngine::_SiteExist(const SpUrl& url)
{
    XCHAR   obj[400];
    XCHAR   file[512];

    url->GetObjectFilename(obj);
    this->_GetURLRecordLocation(url, obj, file);
    FILE* pf = fopen(file,"rb");
    if(pf)
    {
        fclose(pf);
        return 1;
    }
    return 0;
}


//---------------------------------------------------------------------------------------
BOOL BuzzEngine::_SaveRobots(XCHAR* page, size_t length, SpUrl& url)
{
    if(!_ParseHeader(page, url))
    {
        return 0;
    }
    XCHAR   filename[400];
    this->_GetURLRecordLocation(url, "robots.txt", filename,1);

    os::H_FILE* pf = f_open(filename, "wb");
    if(pf)
    {
        BOOL   start = 0;
        XCHAR* line  = strtok(page,"\n");
        while(line)
        {
            if(line[0]=='#')
            {
                line = strtok(0,"\n");
                continue;
            }
            so::trimall(line,' ');
            so::trimall(line,'\t');
            so::trimall(line,'\r');
            so::trimall(line,'\n');

            if(strstr(line,"User-agent:*") || strstr(line,"User-agent:BuzzNet") && ! start)
            {
                start = 1;
                line = strtok(0,"\n");
                continue;
            }
            else if(strstr(line,"User-agent:") && start)
            {
                start = 0;
                line = strtok(0,"\n");
                continue;
            }

            if(start)
            {
                if(!strcmp(line,"Disallow:/"))
                {
                    BuzzEngine::Printf(1, "FORBIDDEN *: %s %s\r\n",line,url->GetServerName());
                    return 0;                 //all fobidden
                }

                if(strstr(line,"Disallow:"))
                {
                    XCHAR* pd = strstr(line,"Disallow:");
                    if(pd)
                    {
                        XCHAR loco[128];
                        so::strscpy(loco, pd+9,"\r\n");
                        strcat(loco,"\r\n");
                        f_puts(loco, pf);
                    }
                }
            }
            line = strtok(0,"\n");
        }
        f_close(pf);
    }
    return 1;
}

//---------------------------------------------------------------------------------------
BOOL BuzzEngine::_SaveSite(const XCHAR* pageName, const XCHAR* page, size_t length, SpUrl& url)
{
    size_t  memlen = length;
    const   XCHAR* afterheader = ::strstr(page,"\r\n\r\n");

    if(!afterheader)
        afterheader = page;
    else
    {
        afterheader+=4;
        memlen-=(size_t)(afterheader-page);
    }

    XCHAR   obj[400];
    XCHAR   file[512];

    url->GetObjectFilename(obj);
    this->_GetURLRecordLocation(url, obj, file,1);
    so::surefilename(file);
    
    FILE* pf = fopen(file,"wb");
    if(pf)
    {
        //fprintf(pf,"<pre>%s %s %s</pre>\r\n",url->GetIP(), url->GetServerName(), url->GetFile());
        fwrite(afterheader, memlen, 1, pf);
        fclose(pf);
        BuzzEngine::Print(2, afterheader);
    }
    _UpdatereportFile(file);
    ++_stats.n_fetchedurls;
    return 1;         //by ip and syte
}

//---------------------------------------------------------------------------------------
BOOL BuzzEngine::_CheckSite(const XCHAR* pageName, XCHAR* page, size_t length, SpUrl& url)
{
    if(!_ParseHeader(page, url))
        return 0;

    size_t memlen = length;
    const XCHAR* afterheader = ::strstr(page,"\r\n\r\n");
    if(afterheader)afterheader+=4;
    if(!afterheader)
        afterheader = page;
    else
        memlen-=(size_t)(afterheader-page);
    if(memlen<=0)
    {
        printf (" empt");
        return 0;
    }

    if(!memcmp(p_previospage, afterheader, min((int)memlen, 4095)))
    {
        printf (" dup");
        return 0;
    }
    ::memcpy(p_previospage, afterheader, min((int)memlen, 4095));

     XCHAR   obj[400];
    XCHAR   file[512];

    url->GetObjectFilename(obj);
    this->_GetURLRecordLocation(url, obj, file);
    so::surefilename(file);
    FILE* pf = fopen(file,"rb");
    if(pf)
    {
        fclose(pf);
        return 0;
    }
    return 1;
}


//---------------------------------------------------------------------------------------
BOOL BuzzEngine::_RobotsAllow(const SpUrl& url)
{
    BOOL    allow = 1;
    FILE*   pf = 0;
    XCHAR   filename[400];


    _GetURLRecordLocation(url, "robots.txt", filename);
    pf = fopen(filename, "rb");
    if(!pf)
        return TRUE; //no robots

    XCHAR loco[256];
    while(!feof(pf))
    {
        memset(loco,0,4);
        fgets(loco, 255, pf);
        if(loco[2])
        {
            loco[strlen(loco)-2]=0;

            const XCHAR* psp = url->GetObject();
            if(so::strbegin(psp, loco))
            {
                BuzzEngine::Printf(1,"FORBIDDEN: %s %s\r\n",url->GetServerName(),loco);
                allow = 0;
                break;
            }
        }
        if(feof(pf))break;
    }
    f_close(pf);
    return allow;
}

//---------------------------------------------------------------------------------------
void BuzzEngine::_UpdatereportFile(const XCHAR* file)
{
    if(BuzzEngine::s_reportfile[0])
    {
        DWORD ct = os::ge_gettick();
        FILE* pf = 0;
        while(os::ge_gettick()-ct < 1000)
        {
            pf = fopen(BuzzEngine::s_reportfile, "ab");
            if(pf)
                break;
        }
        if(pf)
        {
            fclose(pf);
            fprintf(pf, "%s\r\n", file);
        }
    }
}

//---------------------------------------------------------------------------------------
void BuzzEngine::Printf(int what, const XCHAR* fmt, ...)
{
    if((BuzzEngine::n_printout & what))
    {
        if(BuzzEngine::n_printmode==-1)
        {
            va_list args;
            va_start(args, fmt);

            XCHAR szBuffer[1024];
            ::snprintf(szBuffer, sizeof(szBuffer) / sizeof(XCHAR), fmt, args);
            ::printf(szBuffer);
            va_end(args);    
        }
        else if(BuzzEngine::n_printmode>1)
        {
            ;
        }
    }
}

//---------------------------------------------------------------------------------------
void BuzzEngine::Print(int what, const XCHAR* ctnt)
{
    if((BuzzEngine::n_printout & what))
    {
        if(BuzzEngine::n_printmode==-1)
        {
            ::printf(ctnt);
        }
        else if(BuzzEngine::n_printmode>1)
        {
            ;
        }
    }
}
