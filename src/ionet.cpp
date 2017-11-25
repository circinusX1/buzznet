#include "ionet.h"
#include "connection.h" 
#include "buzznet.h"
#include "statistics.h"


//---------------------------------------------------------------------------------------
NetIO::NetIO(XCHAR* proxy, int proxiport, int maxconn)
{
    n_maxconn = min(maxconn, 64);
    s_proxiaddr = 0;
    n_proxiport = 0;
    if(proxiport)
    {
        s_proxiaddr  = proxy;
        n_proxiport   = proxiport;
    }
    _nsocks = 0;
    _dns.Load();
};

//---------------------------------------------------------------------------------------
NetIO::~NetIO()
{
    _dns.Save();
    int ws = _nsocks;
    while(--ws>-1)
    {
        _KillSocket(_psocks[ws]);
        delete _psocks[ws];
    }
}

//---------------------------------------------------------------------------------------
// gets and setts into url
KXCHAR* NetIO::SzIP(SpUrl& refUrl)
{
    KXCHAR* pSrv = refUrl->SiteName();
    KXCHAR* pret = _dns.FindIP(pSrv);
    if(!pret)
    {
        XCHAR   szip[32]={0};
        if(0 == sock::GetHostIp(pSrv, szip))
        {
            this->_dns.Insert(pSrv, szip);
            refUrl->SzIP(szip);
        }
    }
    else
    {
        refUrl->SzIP(pret);
    }
    return refUrl->SzIP();
}


//---------------------------------------------------------------------------------------
NetIO::IOERR NetIO::Open(SpUrl& refUrl)
{
    AutoLock l(&o_mutex);
    if(_nsocks > n_maxconn)
    {
        return IO_FULL; //no more connections
    }
    if(_IsHere(refUrl))
    {
        return IO_DUPLICATE;
    }
    Conection *ps;
    if(refUrl->IsSSL())
    {
        try{
			ps = new SSL_Conection(refUrl, s_proxiaddr, n_proxiport);
		}catch(...)
		{
			PRINTF(_D|_E,"new SSL_Conection(%s::EXCEPTION) \r\n",refUrl->SiteName());
			return IO_SSLERR;
		}
    }
    else
    {	try{
            ps = new Conection(refUrl, s_proxiaddr, n_proxiport);
		}
		catch(...)
		{
			PRINTF(_D|_E,"new Conection(%s::EXCEPTION) \r\n",refUrl->SiteName());
			return IO_ERROR;				
		}
    }
    if(!ps->Connect(this, refUrl->SiteName(), refUrl->SrvPort(), Buz::dw_timeout))
    {
		PRINTF(_D,"Cannot Connect: %s%s error:%d\r\n",refUrl->SiteName(),refUrl->Document(), ps->error());
        delete ps;
        return IO_CONNERR; // cannot connect
    }
    _psocks[_nsocks++] = ps;
    ps->RefreshStartTime();
    return IO_INPROGRESS;
}

//---------------------------------------------------------------------------------------
int    NetIO::FdSet()
{
    int         nfds = 0;
    timeval     tv = {0, 100};
    Conection*  pst;

    int ws = _nsocks;
    FD_ZERO(&_r);
    FD_ZERO(&_w);
    while(--ws>-1)
    {   
        pst = _psocks[ws];
        if(pst->IsDead()) continue;
        if(pst->HasQuerry() != 0)
        {
            FD_SET((UINT)pst->socket(), &_w);
        }
        FD_SET((UINT)pst->socket(), &_r);
        nfds = max(nfds, (int)pst->socket()); 
    }
    if(_nsocks)
    {
        return ::select(nfds+1, &_r, &_w, 0, &tv);
    }
    return -1; // no sockets
}

int  NetIO::GetConections()
{
    int         ws = _nsocks;
    Conection*  pst;

    while(--ws>-1)
    {
        pst = _psocks[ws];
        if(pst->LastQuerry()==0)
            ++Buz::_stats.n_inreceivesockes;
        else
            ++Buz::_stats.n_connectingsocks;
    }
    return _nsocks;
}

//---------------------------------------------------------------------------------------
int NetIO::Check(Buz* peng, int counter)
{
    BYTE        loco[CHUNK_SIZE];
    int         ws = _nsocks;
    Conection*  pst;

    _socketsChanged = 0;

    while(--ws>-1)
    {
        pst = _psocks[ws];

        if(pst->IsDead()) continue;
        //can write
        if(FD_ISSET(pst->socket(), &_w))
        {
            if(pst->HasQuerry())
            {
                if(-1 == pst->Send())
                {
                    peng->OnConnectError(pst->GetURL());
                    _KillSocket(pst);
                }
                pst->ClearQuerry();
            }
        }
        else //????
        if(FD_ISSET(pst->socket(), &_r))
        {
            int eof = pst->Receive((BYTE*)loco, CHUNK_SIZE-1);
            if(eof <= 0)
            {
                _ProcRecData(peng, pst, ws);
                pst->QueryDone(0);
            }
            else
            {
                loco[eof] = 0;
                if(pst->Append(loco, eof)==0)
                {
                    _ProcRecData(peng, pst, ws);
                    pst->QueryDone(0);
                }
            }
        }
    }
    return 0;
}

//---------------------------------------------------------------------------------------
void NetIO::Flush(Buz* peng, int counter)
{
    if(_socketsChanged || counter)
    {
        TestTimeouts(peng);
    }
    _socketsChanged = 0;
}

//---------------------------------------------------------------------------------------
void NetIO::_ProcRecData(Buz* peng, Conection* pst, int ws)
{
    if(pst->GetContentLength())
    {
        if(pst->LastQuerry() == ROBOTS)
        {
            peng->GetStor()->OnReceivedRobots(pst->GetPageName(), pst->GetContent(), pst->GetContentLength(), pst->GetURL());
        }
        else //+ if(pst->LastQuerry() == TARGET)
        {
            peng->GetStor()->OnReceivedContent(pst->GetPageName(), pst->GetContent(), pst->GetContentLength(), pst->GetURL());
        }
    }
    _KillSocket(pst);
}

//-----------------------------------------------------------------------------------------
void NetIO::TestTimeouts(Buz* peng)
{
    AutoLock l(&o_mutex);
    int         ws = _nsocks;
    Conection*  pst;
    while(--ws>-1)
    {
        pst = _psocks[ws];
        if(pst->IsDead() || pst->HasTimeouted())
        {
            _KillSocket(pst);
            _psocks[ws] = _psocks[_nsocks-1];
            --_nsocks;
        }
    }
}

//-----------------------------------------------------------------------------------------
void NetIO::ShutDown(Buz* pbe)
{
    AutoLock l(&o_mutex);
    int         ws = _nsocks;
    Conection*  pst;
    while(--ws>-1)
    {
        pst = _psocks[ws];

        pbe->PushURL(pst->GetURL(), 0, 1);
        _KillSocket(pst);
        _psocks[ws] = _psocks[_nsocks-1];
        --_nsocks;
    }
}

//-----------------------------------------------------------------------------------------
int NetIO::GetUrls(XCHAR* dest, int length)
{
	AutoLock l(&o_mutex);
	int         ws = _nsocks;
	Conection*  pst;
	int         buflen = 0;
	
	strcat(dest, 
"Server................................................Page............Times\r\n");
	//dest[0] = 0;
	while(--ws>-1)
	{
		pst = _psocks[ws];
        if(pst->IsDead())
            continue;
 		KSpUrl& ru = pst->GetURL();
		strcat(dest, MKSTR("%s...",ru->SiteName()));
		strcat(dest, MKSTR("..%s...",pst->GetPageName()));
		strcat(dest, MKSTR("...%d...%d...%d\r\n", 
							  pst->GetDelay()/1000, 
							  Buz::dw_timeout/1000,
							  ru->Recicle()));
		buflen = str_len(dest);
		if(buflen > length-1024)
			break;
	}
	strcat(dest, "</table>");
	buflen = str_len(dest);
	return buflen;
}

//-----------------------------------------------------------------------------------------
void  NetIO::Clear(SpUrl& refUrl)
{
    AutoLock    l(&o_mutex);
    int         ws = _nsocks;
    Conection*  pst;

    while(--ws>-1)
    {
        pst = _psocks[ws];
        if(!strcmp(pst->GetURL()->UniqHash(), refUrl->UniqHash()))
        {
	        _KillSocket(pst);
        }
    }
}

//-----------------------------------------------------------------------------------------
void  NetIO::DropConnections(KSpUrl& refUrl)
{
    Clear((SpUrl&)refUrl);
}

//-----------------------------------------------------------------------------------------
BOOL  NetIO::_IsHere(SpUrl& refUrl)
{
    Conection*  pst = 0;
    int         ws  = _nsocks;
    DWORD       hk  = refUrl->UniqHash(URL_MAX_KEYS);
    KXCHAR* psn = refUrl->SiteName();

    while(--ws>-1)
    {
        pst = _psocks[ws];
        KSpUrl& r = pst->GetURL();
        if(refUrl->Robots())
        {
            if(r->UniqHash(URL_MAX_KEYS) == hk)
                return TRUE;
        }
        else
        {
            if(!strcmp(psn, r->SiteName()))
                return TRUE;
        }
    }
    return FALSE;
}

//-----------------------------------------------------------------------------------------
void  NetIO::_KillSocket(Conection* pst)
{
    pst->Kill();
    ++_socketsChanged;
}

//-----------------------------------------------------------------------------------------
void LocalDNS::Insert(KXCHAR* d, KXCHAR* i)
{
	_dns.push_back(DIpPair(d,i));
	if(_dns.size() > MAX_DNSES) 
	{
		int howmany = 128; //remove some front
		while(--howmany)
		{
			_dns.pop_front();
		}
	}
	P_Buz->_stats.m_dnsrecs=_dns.size();
}

//-----------------------------------------------------------------------------------------
void LocalDNS::Save()
{
    os::H_FILE* pf     = f_open(MKSTR("%s/dnsrecs",Buz::s_tempFolder),"wb");
    if(pf)
    {
        deque<DIpPair>::iterator b = _dns.begin();
        deque<DIpPair>::iterator e = _dns.end();
        for(;b!=e;b++)
        {
            fputs(MKSTR("%s\r\n",b->sdoma), pf);
            fputs(MKSTR("%s\r\n",b->sip), pf);
        }
        fclose(pf);
    }
    _dns.clear();
}

//-----------------------------------------------------------------------------------------
void LocalDNS::Load()
{
    _dns.clear();
    os::H_FILE* pf     = f_open(MKSTR("%s/dnsrecs",Buz::s_tempFolder),"rb");
    if(pf)
    {
        XCHAR name[96];
        XCHAR sip[32];
        while(!feof(pf))
        {

            if(f_gets(name, 96, pf) && f_gets(sip, 32, pf))
            {
                so::trimeol(name);
                so::trimeol(sip);
                _dns.push_back(DIpPair(name,sip));
            }
            if(feof(pf))
                break;
        }
        fclose(pf);
    }
}
