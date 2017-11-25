//=======================================================================================FP
#include "url.h"
#include "buzznet.h"
#include "storage.h"
#include "pageparser.h"
#include "bzipper.h"

//<?php
/*
header("HTTP/1.1 301 Moved Permanently"); 
header("Location: http://www.webpoof.com/"); 
exit();
//?>
*/

//=======================================================================================
Storage::Storage()
{
    v_robotDocs.reserve(64);
    n_curcrcPos = 0;
    memset(u_crcs,0,sizeof(u_crcs));
    w_queue.Lint2Parent(this);
    f_storage.Lint2Parent(this);
    d_storage.Lint2Parent(this);
}

//=======================================================================================
Storage::~Storage()
{
    
}

BOOL    Storage::HasRobots(KSpUrl& url)
{
    return d_storage.Robots(url);
}

//-----------------------------------------------------------------------------
void    Storage::LoadRobots(KSpUrl& url)
{
    if(s_curSiteName!=url->FileSiteName())
    {
        PRINTF(_D|_I,"Loading Robots file for site: '%s'\r\n",url->FileSiteName());

        s_curSiteName=url->FileSiteName();
        v_robotDocs.clear();
        f_storage.LoadRobotsFile(url, v_robotDocs);
    }		
}


//-----------------------------------------------------------------------------
BOOL    Storage::PassRobots(KSpUrl& url)
{
    BOOL                        allow = 1;
    KXCHAR*                     psp = url->Document();
    vector<string>::iterator    b = v_robotDocs.begin();

    for(;b!=v_robotDocs.end();b++)
    {    
        if(so::strbegin(psp, (*b).c_str()))
        {
            allow = 0;
            break;
        }
    }
    return allow;
}


//=======================================================================================
// check if pageName and page[length] exist on repository. Return 0 if exist, 1 if !
BOOL    Storage::IsDocument(KSpUrl& url, XCHAR* page, int length)
{
	XCHAR fn[800];
    if( 0 == d_storage.HasDocument(url, fn))
	{
		FILE* pf = f_open(fn,"rb");
		if(pf)
		{
			fclose(pf);
			return 1;
		}
        return 0;
	}
	return 1;
}


//=======================================================================================
BOOL Storage::IsDocumentCRC(KSpUrl& url, XCHAR* page, int length)
{
    if(length && page)
    {
        int     memlen = length;
        KXCHAR* afterheader = ::strstr(page,"\r\n\r\n");

        if(afterheader && str_len(afterheader) > 4)
        {
            afterheader += 4;
            memlen -= (int)(afterheader-page);
        }
        else
        {
            afterheader = page;
            memlen = length;
        }
        
        DWORD crc = 0;
        for(int i=0; i < memlen;i+=4)
        {
            crc += afterheader[i] + (i*31);
        }

        //
        // ++++ crc ring for at least 16 pages content
        //
        int crcs = MAX_CRCS;
        while(--crcs>-1)
        {
            if(u_crcs[crcs] == crc)
            {
                return 1;
            }
        }
        u_crcs[n_curcrcPos] = crc;
        n_curcrcPos = (++n_curcrcPos) % MAX_CRCS;
    }
    return 0;
}


//-----------------------------------------------------------------------------
void    Storage::OnPageTimeOut( SpUrl& url)
{
    f_storage.LogErrorHosts(url, "Timeout:", Buz::s_errFileName);
    w_queue.UnVisit(url);
}

//-----------------------------------------------------------------------------
void    Storage::OnHeaderError(KXCHAR* page,  SpUrl& url)
{
    f_storage.LogErrorHosts(url, "Header:", Buz::s_errFileName);
    w_queue.UnVisit(url);
}

//-----------------------------------------------------------------------------
void    Storage::OnConnectError( SpUrl& url)
{
    f_storage.LogErrorHosts(url, "Connect:", Buz::s_errFileName);
    w_queue.UnVisit(url);
}


//-----------------------------------------------------------------------------
void    Storage::OnRedirect( SpUrl& url)
{
    //_LogErrorHosts(url, "Redirected:", Buz::s_errFileName);
    w_queue.UnVisit(url);
}

//-----------------------------------------------------------------------------
BOOL    Storage::RejectSiteName(KSpUrl& url)
{
	KXCHAR* ps = url->FileSiteName();
	
    vector<string>::iterator b = v_stopSites.begin();
    for(; b!= v_stopSites.end();b++)
    {
        const KXCHAR* pw = b->c_str();
		
	    if(strstr(pw, ps) || 
		   strstr(ps, pw))
        {
			PRINTF(_D,"'%s'=='%s' Is Fobidden\n",ps,pw);
            return 1;
        }
    }
    return 0;
}


 
//-----------------------------------------------------------------------------
BOOL    Storage::RejectDocument(KSpUrl& url)
{
    // url by language iso code
	XCHAR loco[1024];
	strcpy(loco,url->Document());
	strcat(loco,url->Parameters());
	
    
	if(loco[0]==0) return 0;
    if(so::stristr(loco,"login=") || 
       so::stristr(loco,"phpsess=") ||
	   so::stristr(loco,"sid=") ||
       so::stristr(loco,"sess="))
       return 1;
	if(so::strcount(loco,'/')>=8)
		return 1;
	
    vector<string>::iterator b = v_stopWords.begin();
    for(; b!= v_stopWords.end();b++)
    {
        KXCHAR* pw = b->c_str();
        switch(*pw)
        {
            case '$':    // anywhere in string
			case '@':    // at the begining of string
            case '^':    // at the end of string
                if(so::stristr(loco,pw+1))
				{
					//PRINTF(_D,"XRX:%s\r\n",loco);
                    return 1;
				}
                break;
            default:
                if(so::stristr(loco,pw))
				{
					//PRINTF(_D,"XRX:%s\r\n",loco);
                    return 1;
				}
                break;
        }
    }
    return 0;
}


//-----------------------------------------------------------------------------
BOOL    Storage::SetupDB()
{
    d_storage.CreateTables();
    d_storage.CatcheRejectedSites(v_stopSites);//vector<string>
    d_storage.CatcheRejectedWords(v_stopWords);
    return 1;
}

//-----------------------------------------------------------------------------
SITEEX     Storage::PushURL(KSpUrl& url, 
                            const Url* referer, 
                            BOOL again, 
                            int iwhere)
{
    if(again)
    {
  		w_queue.ForceInQ((SpUrl&)url);
        return e_SITEADDED;
    }
    
    if(!FilterIn(url, iwhere))
    {
        if(iwhere == -1) //working queue
        {
            w_queue.PushToQ(url);
        }
        else
        {
            w_queue.PushToExtQ(url);
        }
        return e_SITEADDED;
    }
    return e_REJECT;
}


BOOL Storage::FilterIn(KSpUrl& url, int iwhere)
{
    if(1==iwhere)
    {
        if(url->DispersDec()==0)
        {
            PRINTF(_D,"<<< Dispers: %s\r\n",url->FileSiteName());
            return 1;
        }
    }

    if(w_queue.IsIn(url))     
    {
        PRINTF(_D,"<<< Q Visited: %s\r\n",url->FileSiteName());
        return 1;
    }

	if(url->Recicle() > Buz::n_retries)
	{
        PRINTF(_D,"<<< Timeout: %s\r\n",url->FileSiteName());
		OnPageTimeOut((SpUrl&)url);
		return 1;
	} 
	if(url->Hits() >= Buz::n_pagespersite )
	{
        PRINTF(_D,"<<< Hits: %s\r\n",url->FileSiteName());
		w_queue.EraseAll();
		return 1;
	}

	if(RejectSiteName(url))  
    {
        PRINTF(_D,"<<< SiteName: %s\r\n",url->FileSiteName());
		return 1;
    }
	
    if(RejectDocument(url))     
    {
        PRINTF(_D,"<<< Document Name: %s\r\n",url->FileSiteName());
		return 1;
    }

    if(IsDocument(url)) 
    {
        PRINTF(_D,"<<< Document Name: %s\r\n",url->FileSiteName());
		return 1; 
    }

	return 0;
}


//-----------------------------------------------------------------------------
SITEEX    Storage::PushAgainURL(SpUrl& refUrl, const Url* referer,  int bwhere)
{
    if(refUrl->Recicle() < Buz::n_retries)
    {
        refUrl->RecileUp();
        return PushURL(refUrl, referer, 1, bwhere);
    }
    OnPageTimeOut(refUrl);
    return e_REJECT;
}


void    Storage::OnServerChanged() //NEW_NEW
{
    w_queue.EraseAll();
}

BOOL	Storage::PushExternalURL(KSpUrl& url)
{
    DWORD sz = w_queue.PushToExtQ(url);
	return sz;
}


void	Storage::CheckQueues()
{
    w_queue.Check();
}


//---------------------------------------------------------------------------------------
void Storage::OnReceivedContent(KXCHAR* pageName, BYTE* page, int length, SpUrl& url)
{
    ++Buz::_stats.n_nurls;                                           // stats update
    Buz::_stats.n_bps  += length;

    //
    // filter out
    //
    if( url->Hits()                >  Buz::n_pagespersite || 
        url->Dispersion()          == 0                   ||
        d_storage.GetSiteHits(url) >  Buz::n_pagespersite)
    {
        w_queue.UnVisit(url);
		//(_D,"X-X%s\r\n",pageName);
        return ;
    }

    PageParser::MIMETYPE mt  = PageParser::eUNK;
    PageParser           pp;


    KXCHAR* pPageStart = pp.ParseHeader((XCHAR*)page, length, url, RET(mt));
    if(pPageStart == 0 || length==0)
    {
        OnHeaderError((XCHAR*)page, url);
        w_queue.UnVisit(url);
		//PRINTF(_D,"X-H%s\r\n",pageName);
        return ;
    }
    if(PageParser::eTEXT==mt)
    {
		static int hits = -1;
		static int sid   = 0;
		static int count   = 0;

		if(++count % 3==0)
		{
			 if(url->Hits()== hits && sid == url->SiteID())
			 {
				 printf("Hits did not count up on %s\r\n",url->SiteName());
				 Buz::b_BreaK = 1;
			 }
		}

		
        XCHAR   docFName[800];

        f_storage.SaveDocument(pageName, pPageStart, length, url, pp, docFName);
        d_storage.SaveDocument(url, docFName);

        pp.ParsePage(pageName, pPageStart, length, url, mt);

		XCHAR out[800];
		sprintf(out,"%03d[%d]/%d:%s%s\t\t depth=%d hits=%d %s\r\n", 
                                        Buz::_stats.n_fetchedurls+
										Buz::_stats.n_nurls, 
		    							url->SiteHash(),
			    						url->Dispersion(),
				    					url->SiteName(), 
					    				url->Document(),
						    			url->Depth(),
	                                    url->Hits(), 
										url->Parameters());                                    
		out[80] = 0;
		PRINTF (_D|_I,out);
		hits = url->Hits();
    }
	w_queue.UnVisit(url);
}

//---------------------------------------------------------------------------------------
void Storage::OnReceivedRobots(KXCHAR* pagename, BYTE* page, int length, SpUrl& url)
{
    Buz::_stats.n_bps         += length;
    Buz::_stats.m_totalkbytes += length;

    PageParser::MIMETYPE mt  = PageParser::eUNK;		
    PageParser           pp;

    url->Robots(1);
    XCHAR* pAfterHdr = (XCHAR*)pp.ParseHeader((XCHAR*)page, length, url, mt);
    
    f_storage.SaveRobots(pAfterHdr, length, url);
    d_storage.SaveSite(url,url->SiteHash(),1);
    PushURL(url, 0, 1, -1);

    assert(url->SiteID()!=NILLA);
    PRINTF(_D,"R.[sid:%d:%s)\r\n",url->SiteID(), url->SiteName());
}


BOOL    Storage::ReadFromStorage()
{
  	OnServerChanged();
    if(w_queue.CheckExternalURLs())
        return 1;

    if(f_storage.CheckInputFile())
        return 1;

    if(w_queue.Load(Buz::s_queFileName,0))
        return 1;
    return 0;
}

//-----------------------------------------------------------------------------
// run indexer
#ifdef _WIN32
    #include "Tlhelp32.h"
    #include "Psapi.h"


static DWORD FindProcess(KXCHAR* pszExeFile)
{
    BOOL           bRet      = FALSE; 
    PROCESSENTRY32 pe32      = {0}; 
 
    //  Take a snapshot of all processes in the system. 
    HANDLE  hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
    if (hProcessSnap == INVALID_HANDLE_VALUE) 
        return (FALSE); 
 
    pe32.dwSize = sizeof(PROCESSENTRY32); 
 
    if (Process32First(hProcessSnap, &pe32)) 
    { 
        do 
        { 
            if(!stricmp(pszExeFile, pe32.szExeFile))
            {
                bRet= TRUE;
                break;
            }
        } while (Process32Next(hProcessSnap, &pe32)); 
    }
    CloseHandle (hProcessSnap); \
    if(bRet)
        return pe32.th32ProcessID;
    return 0;
}
#else
    #include <cstdlib>
    //linux Fork
#endif //
