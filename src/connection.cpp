#define HEADER_DES_LOCL_H



#include "stdafx.h"
#include "connection.h"
#include "buzznet.h"
#include "ionet.h"
//REQUEST SAMPLE
/*
//http----------------------------------------------------------------------------------

"GET http://www.nationaldoubledutchleague.com/DDDD.htm HTTP/1.GetI0
Host: www.nationaldoubledutchleague.com
User-Agent: Mozilla/5.0 (Windows)
Proxy-Connection: keep-alive

//https---------------------------------------------------------------------------------
CONNECT tdnam.com:443 HTTP/1.0
User-Agent: 
Proxy-Connection: keep-alive
Host: tdnam.com

CONNECT 74.112.23.48:8000 HTTP/1.0
User-Agent Mozilla
Proxy-Connection: keep-alive
Host: 74.112.23.48




*/


//---------------------------------------------------------------------------------------
Conection::Conection(SpUrl& url, KXCHAR* proxiaddr, int proxiport)
{
    assert((int)this->socket()==-1);
    s_proxiaddr = 0;
    n_proxiport = 0;
    r_url      = url;
    n_chunksz = 0;
    n_accumulated = 0;
    p_response = 0;
    w_asking   = 0;
    n_lasterror  = 0;
    b_killed     = 0;
    if(proxiport)
    {
        s_proxiaddr = proxiaddr;
        n_proxiport = proxiport;
    }
}

//---------------------------------------------------------------------------------------
Conection::~Conection()
{
    if(p_response) 
    {
		delete[] p_response;
    }
    this->destroy();
}

//---------------------------------------------------------------------------------------
BOOL    Conection::Connect(NetIO* pNetIo, KXCHAR *inetaddr, int port, int timeout)
{
    int     ret        = 0;
    XCHAR   ipaddr[32] = {0};

    ++P_Buz->_stats.m_conss;
    if(this->n_proxiport)
    {
        ret = this->try_connect(this->s_proxiaddr, this->n_proxiport);
    }
    else
    {
        KXCHAR* szIp = r_url->SzIP();
        if(szIp[0]==0 || szIp[0]=='0' )
            szIp = pNetIo->_dns.FindIP(inetaddr);
        if(szIp)
        {
            ret = this->raw_connect(szIp, port);
            strcpy(ipaddr, szIp);
        }
        else
        {
            ++P_Buz->_stats.m_dnsacc;
            ret = this->try_connect(inetaddr, port);
        }
    }
    dw_starttime = os::ge_gettick();
    if (ret <=0 && (this->error() == EINPROGRESS || 
		            this->error() == WOULDBLOCK) )
    {
        w_asking = CONNECTING;

        if(0 == ipaddr[0])
        {
            this->getsocketaddr(ipaddr);
            pNetIo->_dns.Insert(inetaddr, ipaddr);
        }
        r_url->SzIP(ipaddr);

        if(P_Buz->GetStor()->HasRobots(r_url))
        {
            r_url->Robots();
            FormatContentRequest();
        }
        else
        {
            FormatRobotsRequest();
        }
        return 1;
    }
    return 0;
}


//---------------------------------------------------------------------------------------
void      Conection::FormatRobotsRequest()
{
    if(this->n_proxiport)
    {
        s_request =   "GET http://";
        s_request +=  r_url->SiteName();
        if(r_url->SrvPort()!=80)
            s_request +=  (KXCHAR*)MKSTR(":%u", r_url->SrvPort());
        s_request +=  "/robots.txt HTTP/1.0\r\nHost: ";
    }
    else
    {
        s_request =  "GET /robots.txt HTTP/1.0\r\nHost: ";
    }
    s_request  +=  r_url->SiteName();
    s_request  +=  MKSTR("\r\nUser-Agent: %s (%s)",Buz::s_name, Buz::s_youremail);
    if(this->n_proxiport)
        s_request  +=  "\r\nProxy-Connection: Close";

    s_request  +=  "\r\n\r\n";
    s_pagename = "/robots.txt";
    w_asking   =  ROBOTS;
    n_chunksz = 0;
}

//---------------------------------------------------------------------------------------
void      Conection::FormatContentRequest()
{
	s_request.clear();
    if(this->n_proxiport)
    {
        s_request =   string("GET http://");
        s_request +=  r_url->SiteName();
        if(r_url->SrvPort()!=80)
            s_request +=  (KXCHAR*)MKSTR(":%u", r_url->SrvPort());
        if(Buz::b_useparameters)
        {
            if(r_url->Parameters()[0])
                s_request +=  MKSTR("%s%s HTTP/1.0\r\nHost: ", r_url->Document(),r_url->Parameters());
            else
                s_request +=  MKSTR("%s HTTP/1.0\r\nHost: ", r_url->Document());
        }
        else
            s_request +=  MKSTR("%s HTTP/1.0\r\nHost: ", r_url->Document());
    }
    else
    {
        s_request =   string("GET ");
        if(Buz::b_useparameters)
        {
            if(r_url->Parameters()[0])
                s_request +=  MKSTR("%s%s HTTP/1.0\r\nHost: ", r_url->Document(),r_url->Parameters());
            else
                s_request +=  MKSTR("%s HTTP/1.0\r\nHost: ", r_url->Document());
        }
        else
            s_request +=  MKSTR("%s HTTP/1.0\r\nHost: ", r_url->Document());

        //s_request =  loco;
    }
    
    s_request +=  r_url->SiteName();
    s_request +=  MKSTR("\r\nUser-Agent: %s (%s)",Buz::s_name, Buz::s_youremail);
    s_request +=  "\r\nAccept: text/html";
    if(this->n_proxiport)
        s_request  +=  "\r\nProxy-Connection: Close";
    s_request +=  "\r\n\r\n";
    
    s_pagename = r_url->Document();
    if(Buz::b_savepageparams)
        s_pagename += r_url->Parameters();


    if(s_pagename=="" || s_pagename=="/") 
        s_pagename="/#0.htm";
    else if(s_pagename[s_pagename.length()-1]=='/')
        s_pagename+="#0.htm";
    else if(s_pagename.find('.')==-1)
        s_pagename+="/#0.htm";

    w_asking   =  TARGET;
    n_chunksz = 0;
}

//---------------------------------------------------------------------------------------
void      Conection::QueryDone(int inorogress)
{
    w_asking     =  0;
}

//---------------------------------------------------------------------------------------
int Conection::Send()
{
    RefreshStartTime();
    return (int)this->send((BYTE*)s_request.c_str(), (int)s_request.length());
}

//---------------------------------------------------------------------------------------
int Conection::Receive(BYTE* buff, int sizeb)
{
    return this->receive((BYTE*)buff, sizeb);
}

//---------------------------------------------------------------------------------------
int  Conection::Append(const BYTE* data, int length)
{
    if(p_response==0)
    {
        n_chunksz     = CHAR_CHUNK;
        n_accumulated = 0;
        try{
			p_response    = new BYTE[CHAR_CHUNK];
		}catch(...)
		{
			return 0;
		}
        p_response[0] = 0;
    }
    
    int newlength = n_accumulated + length;
    if(newlength > Buz::n_maxPageSize) // page bigger than 250K is enough
         return FALSE;
    if(newlength >= n_chunksz)
    {
        if(w_asking == ROBOTS)
            return false;
        n_chunksz += CHAR_CHUNK;
        BYTE *loco = new BYTE[n_chunksz];
        memcpy(loco, p_response, (size_t)n_accumulated);
        loco[n_accumulated]=0;
        delete[] p_response;
        p_response = loco;
    } 
    memcpy(&p_response[n_accumulated], data, (size_t)length);
    n_accumulated+=length;
    p_response[n_accumulated]=0;
    assert(n_accumulated < n_chunksz);
    return TRUE;
}
 
//---------------------------------------------------------------------------------------
BOOL      Conection::HasTimeouted()
{
    return GetDelay() > (DWORD)Buz::dw_timeout;
}

//---------------------------------------------------------------------------------------
BOOL SSL_Conection::Connect(NetIO* pIo, KXCHAR *inetaddr, int port, int timeout)
{
    return Conection::Connect(pIo, inetaddr, port, timeout);
}

//---------------------------------------------------------------------------------------
int  SSL_Conection::Send()
{
    char loco[256]={0};

    n_lasterror  = 0;
    this->setblocking(1);

    if(this->n_proxiport)
    {
        this->send(s_proxyconnect.c_str(), (int)s_proxyconnect.size());
        if(this->receive(loco, 256)>=0)
        {
            const char* perrcode = loco + 9;
            int   errcode = ::t_atoi(perrcode);
            if(200 != errcode)
            {
                this->setblocking(0);
                return -1;
            }
        }
    }
    assert(p_ctx==0);

    SSLeay_add_ssl_algorithms();
	p_meth = SSLv2_client_method();
	SSL_load_error_strings();
	p_ctx = SSL_CTX_new(p_meth);
	if ((p_ctx = SSL_CTX_new(p_meth)) == NULL)
    {
        n_lasterror = SSL_get_error (p_ssl, -1);
        this->setblocking(0);
		return -1;
	}
	if ((p_ssl = SSL_new(p_ctx)) == NULL)
    {
        n_lasterror = SSL_get_error (p_ssl, -1);
        this->setblocking(0);
		return -1;
	}

    SSL_set_fd (p_ssl, (int)this->socket());

	if (SSL_connect(p_ssl) == -1)
    {
        n_lasterror = SSL_get_error (p_ssl, -1);
        this->setblocking(0);
		return -1;
	}

	// Send HTTP request
    if ((SSL_write(p_ssl, (char*)s_request.c_str(), (int)s_request.length())) == -1)
    {
        n_lasterror = SSL_get_error (p_ssl, -1);
        this->setblocking(0);
		return -1;
	}
    this->setblocking(0);
    return 1;
}

//---------------------------------------------------------------------------------------
int  SSL_Conection::Receive(BYTE* buff, int sizeb)
{
    int r = SSL_read(p_ssl, (XCHAR*)buff, sizeb);
    int n_lasterror = SSL_get_error(p_ssl,r);
    if(n_lasterror == SSL_ERROR_NONE)
    {
        return r;
    }
    if(n_lasterror == SSL_ERROR_WANT_READ)
        return CHUNK_SIZE+1;  // continue
    return -1;
}

//---------------------------------------------------------------------------------------
void SSL_Conection::FormatContentRequest()
{
    if(this->n_proxiport)
    {
        s_request =   "GET https://";
        s_request +=  r_url->SiteName();
        if(r_url->SrvPort()!=80)
            s_request +=  (KXCHAR*)MKSTR(":%u", r_url->SrvPort());

        //s_request +=  MKSTR("%s HTTP/1.0\r\nHost: ", r_url->Document());


        if(Buz::b_useparameters)
        {
            if(r_url->Parameters()[0])
                s_request +=  MKSTR("%s%s HTTP/1.0\r\nHost: ", r_url->Document(),r_url->Parameters());
            else
                s_request +=  MKSTR("%s HTTP/1.0\r\nHost: ", r_url->Document());
        }
            
        else
            s_request +=  MKSTR("%s HTTP/1.0\r\nHost: ", r_url->Document());

    }
    else
    {
		XCHAR loco[1024];
        if(Buz::b_useparameters)
            t_sprintf(loco, "GET %s HTTP/1.0\r\nHost: ", r_url->Document());
        else
        {
            if(r_url->Parameters()[0])
                t_sprintf(loco, "GET %s%s HTTP/1.0\r\nHost: ", r_url->Document(),r_url->Parameters());
            else
                t_sprintf(loco, "GET %s HTTP/1.0\r\nHost: ", r_url->Document());
        }

        s_request =  loco;
    }
    s_request +=  r_url->SiteName();
    s_request +=  MKSTR("\r\nUser-Agent: %s (%s)",Buz::s_name,Buz::s_youremail);
    s_request +=  "\r\nAccept: text/html";
    if(this->n_proxiport)
        s_request  +=  "\r\nProxy-Connection: Close";
    s_request +=  "\r\n\r\n";
    
    s_pagename = r_url->Document();
    if(Buz::b_savepageparams)
        s_pagename += r_url->Parameters();

    if(s_pagename=="" || s_pagename=="/") 
        s_pagename="/#0.htm";
    else if(s_pagename[s_pagename.length()-1]=='/')
        s_pagename+="#0.htm";
    else if(s_pagename.find('.')==-1)
        s_pagename+="/#0.htm";



    w_asking   =  TARGET;
    n_chunksz = 0;

    if(this->n_proxiport)
    {
        int port = r_url->SrvPort();
        
        s_proxyconnect =  MKSTR("CONNECT %s:%d HTTP/1.0\r\n", r_url->SiteName(), port);
        s_proxyconnect += MKSTR("User-Agent: %s (%s)\r\n",Buz::s_name, Buz::s_youremail);
        s_proxyconnect += MKSTR("%s\r\n","Proxy-Connection: Close");
        s_proxyconnect += MKSTR("Host: %s\r\n\r\n",r_url->SiteName());
    }
}


//---------------------------------------------------------------------------------------
void SSL_Conection::FormatRobotsRequest()
{
    if(this->n_proxiport)
    {
        s_request =   "GET httsp://";
        s_request +=  r_url->SiteName();
        if(r_url->SrvPort()!=80)
            s_request +=  (KXCHAR*)MKSTR(":%u", r_url->SrvPort());
        s_request +=  "/robots.txt HTTP/1.0\r\nHost: ";
    }
    else
    {
        s_request =  "GET /robots.txt HTTP/1.0\r\nHost: ";
    }
    s_request  +=  r_url->SiteName();
    s_request  +=  MKSTR("\r\nUser-Agent: %s (%s)",Buz::s_name, Buz::s_youremail);
    if(this->n_proxiport)
        s_request  +=  "\r\nProxy-Connection: Close";
    s_request  +=  "\r\n\r\n";
    s_pagename = "/robots.txt";
    w_asking   =  ROBOTS;
    n_chunksz = 0;

    if(this->n_proxiport)
    {
        int port = r_url->SrvPort();
        
        s_proxyconnect =  MKSTR("CONNECT %s:%d HTTP/1.0\r\n", r_url->SiteName(), port);
        s_proxyconnect += MKSTR("User-Agent: %s (%s)\r\n",Buz::s_name, Buz::s_youremail);
        s_proxyconnect += MKSTR("%s\r\n","Proxy-Connection: Close");
        s_proxyconnect += MKSTR("Host: %s\r\n\r\n",r_url->SiteName());
    }

}
