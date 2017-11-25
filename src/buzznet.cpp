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
#include "watchdog.h" 
//---------------------------------------------------------------------------------------



static BOOL Suspend = FALSE;
Buz*        P_Buz;

//---------------------------------------------------------------------------------------
HttpServer*  Phttp;

Buz::Buz()
{
    assert(P_Buz==0);
    P_Buz        = this;                               // one of these
    _pIO         = 0;
    _pRP         = 0; 
    _inselect    = 0;
    n_counter    = 0;
}

//---------------------------------------------------------------------------------------
Buz::~Buz()
{
    delete _pRP; _pRP=0;
    delete _pIO; _pIO=0;
}


//---------------------------------------------------------------------------------------
void Buz::Run()
{
    NetIO::IOERR rv;
    int          irv;
    BOOL         show=1;
    BOOL         showempty=1;
    WatchDog     w(this);

    w.Start();
    _pIO = new NetIO(s_proxi, n_proxiport, n_maxconn); 
   

    _stats.n_maxsockets = n_maxconn;
    _stats.n_starttime  = os::ge_gettick();
    _stats.w_second     = 0;
    
    OnStateChanged();
    _pRP->GetWQ().Load(Buz::s_queFileName,1);
    
    while(!Buz::b_BreaK && !w.IsStopped(0))
    {
        ++n_counter;
        _stats.n_connectingsocks = 0;
        _stats.n_inwait          = 0;
        _stats.n_inreceivesockes = 0;

        if(Suspend || Buz::b_pause)
        {
            if(show) 
                PRINTF(_D,"\r\nProcess susspended: \r\n"); show = 0;
            os::ge_sleep(200);
            continue;
        }
        if(Buz::b_BreaK) 
        {
            PRINTF(_D,"\r\nProcess interrupted: \r\n"); 
            break;
        }
        BOOL   qIsEmpty = 0;    
        SpUrl refUrl;
	    while(1)
		{
			qIsEmpty = 0;
            _stats.n_sockets     = _pIO->GetConections();
            if(_stats.n_sockets >= Buz::n_maxconn)
            {
                break;
            }

            _stats.n_queuelength = _pRP->GetWQ().GetFifo(&refUrl);
            if(_stats.n_queuelength < 0)
            {
                break;
            }
			//printf("%s<-\r\n",refUrl->SiteName());
			
			
			rv = _pIO->Open(refUrl);
            if(NetIO::IO_INPROGRESS == rv)
            {
                continue; //next one
            }
            else if(NetIO::IO_FULL == rv)
            {
                // shouldnt happen
                assert(0);
                PushURL(refUrl, 0, 1,-1); 
                break;
            }
            else if(NetIO::IO_CONNERR == rv)
            {
                OnConnectError(refUrl);
            
                _pIO->DropConnections(refUrl);
                _pRP->GetWQ().EraseUrl(refUrl);

                _pIO->Clear(refUrl);
            }
            else if(NetIO::IO_DUPLICATE == rv)
            {
            }
            else
            {
				PRINTF(_D, "NETIO UNEXPECTED. STOPING\r\n");
                Buz::b_BreaK = 1;  //stopping
            }
        }

		_stats.n_sockets = _pIO->GetConections();
		if(_stats.n_sockets > 0)
        {
            _inselect = 1;
            irv = _pIO->FdSet();
            if(-1 == irv)
            {
                int err = os::ge_error();
                if(err !=0)
                {
                    PRINTF(_E|_D,"SELECT SOCKET Error: %d. Process Exiting!\r\n", err);
                    break;
                }
            }
            if(irv>0)
            {
                _pIO->Check(this, n_counter);
            }
            _inselect = 0;
        } 
        else
        { 
            if(0==_pRP->ReadFromStorage())
            {
				qIsEmpty = 1;
                os::ge_sleep(1000);
                if(showempty)
                {
                    Configure("buzznet.conf", 0);
                    PRINTF(_I|_D,"Waiting buzznet.txt\r\n");
                    showempty = 0;
                }
            }
            else
            {
                showempty=1;
				qIsEmpty = 0;
            }
        }
        _pIO->Flush(this, n_counter);

        _Report();
        _pRP->CheckQueues();
        if((n_counter % 4096)==0)
        {
            _FlushToRemoteBznets();
			if(qIsEmpty)
			{
				_pRP->GetDBSto().StayAlive();
			}
        }
    }
    _FlushToRemoteBznets();
    _pIO->ShutDown(this);
    delete _pIO; _pIO=0;
    w.Stop();
    Phttp->Break();
    
    _pRP->GetWQ().Save(Buz::s_queFileName,0,1);
    _pRP->GetWQ().SaveExternalURLs(Buz::s_queFileName);
}


//---------------------------------------------------------------------------------------
void  Buz::OnConnectError(SpUrl& refUrl)
{
    return _pRP->OnConnectError(refUrl);
}

//---------------------------------------------------------------------------------------
BOOL Buz::OnTimeOut(SpUrl& url)
{
	PRINTF(_D,"%s rec: %d timeout\r\n",url->SiteName(),url->Recicle());
    return _pRP->PushAgainURL(url);
}


//---------------------------------------------------------------------------------------
void        Buz::_TestPressedKey(BOOL& show)
{
#ifdef _WIN32
    if(_kbhit())
    {
        if(toupper(_getch())=='Q')
        {
            Buz::b_BreaK=TRUE;
        }
        if(toupper(_getch())==' ')
        {
            Suspend = !Suspend;
            show=1;
        }
        if(toupper(_getch())=='Z')
        {
            PRINTF(_D,"\r\nqueue length=%d\r\n", _pRP->GetWQ().GetSize());
        }
    }
#endif //
}

//---------------------------------------------------------------------------------------
void Buz::_Report()
{
    _stats.n_currtime = os::ge_gettick();
    _stats.n_uptime   = (_stats.n_currtime - _stats.n_starttime)/1000;

    // each 4 seconds do a report
    if(_stats.n_currtime - _stats.w_second > 1000) 
    {
        _stats.w_second        = _stats.n_currtime;

        _stats.n_webspersec    = _stats.n_nurls;
        _stats.m_consspersec   = _stats.m_conss;
        _stats.m_dnsaccpersec  = _stats.m_dnsacc;
        _stats.n_bpspers       = _stats.n_bps; 

        _stats.m_conssTotal   += _stats.m_conss;
        _stats.m_dnsaccTotal  += _stats.m_dnsacc;
        _stats.n_fetchedurls  += _stats.n_nurls;
        _stats.m_totalkbytes += _stats.n_bps;

        _stats.n_nurls  = 0;
        _stats.m_conss  = 0;
        _stats.m_dnsacc = 0;
        _stats.n_bps    = 0; 

        //Report();
    }
    if(Buz::m_delaybeturls > 0)
    {
        os::ge_sleep(Buz::m_delaybeturls);
    }
}

//---------------------------------------------------------------------------------------
void Buz::_HtmlReport()
{
    t_sprintf(s_lo,"Spins=%d<br />\r\n"
	                "Url Objects..............%d\r\n"
	                "Fetched Pages............%d\r\n"
                    "Open Sockets.............%d\r\n"
                    "Connections/Sec..........%d\r\n"
                    "DNS Access/Sec...........%d\r\n"
	                "DNS Database.............%d\r\n"
                    "Queue....................%d\r\n"
					"Waiting List.............%d\r\n"
                    "Pages/Sec................%d\r\n"
                    "Kb/s.....................%d\r\n"
                    "Total KBytes.............%u\r\n",
		                                (int)n_counter,
	                                    (int)Url::l_urls,
                                        (int)_stats.n_fetchedurls+_stats.n_nurls,
                                        (int)_stats.n_sockets,
                                        (int)_stats.m_consspersec,
                                        (int)_stats.m_dnsaccpersec,
	                                    (int)_stats.m_dnsrecs,
                                        (int)_stats.n_queuelength,
                                        _pRP->GetWQ().GetExtSize(),
                                        (int)_stats.n_webspersec,
                                        (int)_stats.n_bpspers/1024, 
                                        (int)_stats.m_totalkbytes/1024
										);
}


//---------------------------------------------------------------------------------------
// first char is 
// 0 all
// 1 info1
// 2 warningd
// 4 exceptions and errors
void Buz::Printf(int what, KXCHAR* fmt, ...)
{
	if((Buz::n_printout & what))
	{
		XCHAR szBuffer[512];
		va_list args;
		va_start(args, fmt);
		::sn_printf(szBuffer, 500 / sizeof(XCHAR), fmt, args);
		so::replace_urlfmt(szBuffer);
		t_printf(szBuffer);
		va_end(args);    
    } 
}


//---------------------------------------------------------------------------------------

SITEEX  Buz::PushURL(SpUrl refUrl, const Url* referer, int again, int iPosi)
{
    SITEEX rv = _pRP->PushURL( refUrl, referer, again, iPosi);
    return rv;
}


//---------------------------------------------------------------------------------------
// returns current status into a html table
size_t Buz::GetStats(XCHAR* dest, int length)
{
    _HtmlReport();
	strcpy(dest,"<br>");
    length -= 5;
    if(_pIO) length -= _pIO->GetUrls(dest, length);
    int mlen = (int)str_len(s_lo);
    if(mlen > length-2) mlen = length;
    strncat(dest, s_lo, mlen);
	return str_len(dest);
}

//-----------------------------------------------------------------------------
BOOL Buz::_CreateRepository()
{
    _pRP = new Storage();
    _pRP->SetupDB();
    return 1;
}

//-----------------------------------------------------------------------------
void Buz::_DestroyRepository()
{
    delete _pRP;
}

//-----------------------------------------------------------------------------
void Buz::OnStateChanged()
{
    
}

//-----------------------------------------------------------------------------
void     Buz::CloseSockets()
{
    _pIO->TestTimeouts(this);
}

//-----------------------------------------------------------------------------
void Buz::RecicleFile(XCHAR* filename)
{
	for(int t=0; filename[t]; t++)
	{
		if(so::isdigit(filename[t]))
		{
			char sno[16];
			int ord = ::t_atoi(&filename[t])+1;
            if(ord > 256)
            {
                b_BreaK=1;
                PRINTF(0xFF,"To many log files Exiting\r\n ");
            }
			t_sprintf(sno,"%d",ord);
			filename[t] = 0;
			strcat(filename,sno);
			break;
		}
	}
}

//-----------------------------------------------------------------------------
BOOL Buz::_FlushToRemoteBznets()
{
    
    return 1;
}

//-----------------------------------------------------------------------------
void     Buz::OnRedirect(SpUrl& url)
{
    _pRP->OnRedirect(url);
}
