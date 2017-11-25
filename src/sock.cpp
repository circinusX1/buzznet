// sock.cpp: implementation of the sock class.
//
//-----------------------------------------------------------------------------

#include "sock.h"



//-----------------------------------------------------------------------------
DWORD       sock::_tout = 2000;

char IP2String::_s[128];



//-----------------------------------------------------------------------------
void sock::Init()
{
#ifdef WIN32
    WSADATA     WSAData;
    WSAStartup (MAKEWORD(1,1), &WSAData);
#endif
}

//-----------------------------------------------------------------------------

void sock::Uninit()
{
#ifdef WIN32
    WSACleanup();
#endif
}

//-----------------------------------------------------------------------------
BOOL sock::CTime(void* , DWORD time)
{
     return time < sock::_tout;
}


//-----------------------------------------------------------------------------
sock::sock()
{
    _sock        = INVALID_SOCKET;
    _error       = 0;
    _blocking    = 0;
    _flags       = 0;
    _connecting  = 0;
};

//-----------------------------------------------------------------------------
sock::~sock()
{
    destroy();
};

//-----------------------------------------------------------------------------
int sock::setblocking(DWORD block)
{
    if(isopen())
    {
        DWORD dw = !block;
    #ifdef WIN32
        int           rv = ::ioctlsocket(_sock, FIONBIO ,&dw);
    #else
        int           rv = ::ioctl(_sock, FIONBIO ,&dw);
    #endif //
        _blocking = block;
        return rv;
    }
    return -1;
}

//-----------------------------------------------------------------------------
SOCKET sock::create(int )
{
   return (SOCKET)-1;
}

//-----------------------------------------------------------------------------
int  sock::GetLocalIP(DWORD* dw)
{
    sockaddr_in locSin    ;
    hostent*    pHe       = 0;
    char        szBuf[80] = {0};
    DWORD       dwSize    = 0;

    locSin.sin_family = AF_INET;
    dwSize = sizeof(szBuf);
    ::gethostname((char*)szBuf,(int)dwSize);
    pHe = ::gethostbyname(szBuf);
    if (pHe == NULL){
        return os::ge_error();
    }
    ::memcpy((char*)&(locSin.sin_addr), pHe->h_addr,pHe->h_length);
    *dw = locSin.sin_addr.s_addr;
    return 0;
}

//-----------------------------------------------------------------------------
// this can be long DNS access
int  sock::GetHostIp(const char* hostname, char* szip, DWORD* dwip)
{
    sockaddr_in      remsin  = {0};
    struct hostent  *hostent = ::gethostbyname(hostname);
    if(hostent)
    {
        ::memcpy((char*)&(remsin.sin_addr), hostent->h_addr, hostent->h_length);
        ::strcpy(szip, inet_ntoa((struct in_addr)remsin.sin_addr));
        if(*dwip)
        {
            *dwip = remsin.sin_addr.s_addr;
        }
        return 0;
    }
    return -1;//error
}


//-----------------------------------------------------------------------------
int sock::GetLocalIP(char* pRetLocalAddr)
{
    sockaddr_in locSin;
    hostent*    pHe;
    char        szBuf[80];
    DWORD       dwSize;

    locSin.sin_family = AF_INET;
    dwSize = sizeof(szBuf);
    gethostname((char*)szBuf,(int)dwSize);
    pHe = gethostbyname(szBuf);
    if (pHe == NULL)
    {
        return os::ge_error();
    }
    ::memcpy((char*)&(locSin.sin_addr), pHe->h_addr,pHe->h_length);

    ::t_sprintf(pRetLocalAddr,"%u.%u.%u.%u",
                (int)((locSin.sin_addr.s_addr    ) & 0xFF),
                (int)((locSin.sin_addr.s_addr>>8 ) & 0xFF),
                (int)((locSin.sin_addr.s_addr>>16) & 0xFF),
                (int)((locSin.sin_addr.s_addr>>24) & 0xFF));
    return 0;
}

//-----------------------------------------------------------------------------
int sock::getoption(int option)
{
    if(isopen())
    {
        int optionVal;
        socklen_t optLen = sizeof(optionVal);
        if(0 == getsockopt(_sock, SOL_SOCKET, option, (char*)&optionVal, &optLen))
            return optionVal;
    }
    return -1;
}


//-----------------------------------------------------------------------------
//err = ::setsockopt(_sock, SOL_SOCKET, SO_SNDBUF, (char *) &bfSz, sizeof(bfSz));
int sock::setoption(int option, int optionsize)
{
    if(isopen())
        return ::setsockopt(_sock, SOL_SOCKET, option, (char *) &optionsize, sizeof(optionsize));
    return -1;
}

//-----------------------------------------------------------------------------
int  sock::select_receive(char* buff, int length, struct sockaddr_in& rsin, int toutms)
{
    return select_receive((BYTE*) buff, length, rsin, toutms);
}

//-----------------------------------------------------------------------------
int  sock::select_receive(BYTE* buff, int length, struct sockaddr_in& rsin, int toutms)
{
    timeval  tv   = {toutms/1000, toutms%1000};
    fd_set   rdSet;

    _error = 0;
    FD_ZERO(&rdSet);
    FD_SET(_sock, &rdSet);
    int nfds = (int)_sock+1;
    if(::select(nfds, &rdSet, 0, 0, &tv) > 0)
    {
        return receive(buff, length);
    }
    _error = os::ge_error();
    return 0;
}


//-----------------------------------------------------------------------------
char* tcp_sock::getsocketaddr(char* pAddr)
{
    ::strcpy(pAddr, inet_ntoa((struct in_addr)_remote_sin.sin_addr));
    return pAddr;
}

//-----------------------------------------------------------------------------
int tcp_sock::send(const char* buff, const int length, int , const char* )
{
    return send((BYTE*)buff, length, 0 , 0 );
}
//-----------------------------------------------------------------------------
int tcp_sock::send(const BYTE* buff, const int length, int , const char* )
{
    _error = 0;
    int i = ::send(_sock,(char *)buff, length, 0);
    if(-1==i)
        _error = os::ge_error();
    return i;
}

//-----------------------------------------------------------------------------
int tcp_sock::receive(char* buff, int length, int , const char* )
{
    return receive((BYTE*) buff, length, 0, 0 );
}

//-----------------------------------------------------------------------------
int tcp_sock::receive(BYTE* buff, int length, int , const char* )
{
    _error = 0;
    int rb = ::recv(_sock,(char *)buff, length, 0);
    if(rb==-1)
        _error = os::ge_error();
    return rb;
}

//-----------------------------------------------------------------------------
void sock::destroy()
{
    _connecting  = 0;
    _flags       = 0;
    if((int)_sock > 0)
    {
        ::shutdown (_sock, 0x02);
        os::ge_sleep(10);
#ifdef WIN32
        ::closesocket(_sock);
#else
        ::close(_sock);
#endif
    }
    _sock=INVALID_SOCKET;
}

/*
//-----------------------------------------------------------------------------
const char*  tcp_cli_sock::tcp_cli_sock()
{
    ::t_sprintf(_sip,"%d.%d.%d.%d",
          (int)(_remote_sin.sin_addr.s_addr&0xFF),
          (int)(_remote_sin.sin_addr.s_addr&0xFF00)>>8,
          (int)(_remote_sin.sin_addr.s_addr&0xFF0000)>>16,
          (int)(_remote_sin.sin_addr.s_addr&0xFF000000)>>24);
    return _sip;
}
*/
//-----------------------------------------------------------------------------
tcp_sock::tcp_sock(){}

//-----------------------------------------------------------------------------
SOCKET tcp_sock::create(int opt)
{
    _error = 0;
    ::memset(&_local_sin,0,sizeof(_local_sin));
    ::memset(&_remote_sin,0,sizeof(_remote_sin));
    if((int)_sock != -1)
        return (SOCKET)-1;
    _sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if((int)_sock < 0)
		_error = os::ge_error();
    return _sock;
}

//-----------------------------------------------------------------------------
int tcp_sock::listen(int maxpending)
{
    _error = 0;
    int rv = ::listen(_sock, maxpending);
    if((int)INVALID_SOCKET == rv)
    {
        _error = os::ge_error();
    }
    return rv;
}

//-----------------------------------------------------------------------------
tcp_srv_sock::tcp_srv_sock(){}

//-----------------------------------------------------------------------------
SOCKET tcp_srv_sock::create(int port)
{

    _error = 0;
    ::memset(&_local_sin,0,sizeof(_local_sin));
    ::memset(&_remote_sin,0,sizeof(_remote_sin));
    _sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if(_sock <= 0)
    {
		_error = os::ge_error();
         return (SOCKET)-1;
    }


    _local_sin.sin_family		= AF_INET;
    _local_sin.sin_addr.s_addr	= htonl(INADDR_ANY);
	_local_sin.sin_port		    = htons(_port = port);
	if(::bind(_sock,(struct sockaddr*)&_local_sin,sizeof(_local_sin)) < 0)
	{
		_error =  os::ge_error();
		destroy();
        return (SOCKET)-1;
	}
    return 0;//OK
}


//-----------------------------------------------------------------------------
SOCKET tcp_srv_sock::accept(tcp_cli_sock& cliSock)
{
    _error = 0;
    socklen_t clilen = (socklen_t)sizeof(cliSock._remote_sin);
    cliSock._sock = ::accept(_sock,
                             (struct sockaddr*)&cliSock._remote_sin,
                             &clilen);
    if((int)cliSock._sock < 0)
    {
		_error = os::ge_error();
	}
    return cliSock._sock;
}

//-----------------------------------------------------------------------------
SOCKET tcp_cli_sock::create(int opt)
{
    _hostent = 0;
    return 0;
}


int tcp_cli_sock::try_connect(const char* sip, int port)
{
    _error = 0;
    sockaddr_in    locSin;
    _hostent = ::gethostbyname(sip);
    if(_hostent==0)
    {
        long dwbo = inet_addr(sip);
        _hostent = gethostbyaddr((char*)&dwbo, (int)sizeof(dwbo), AF_INET );
    }
    if(_hostent)
    {
        ::memcpy((char*)&(locSin.sin_addr), _hostent->h_addr, _hostent->h_length);
        if((int)_sock != (int)INVALID_SOCKET)
        {
            destroy();
        }
        _error = 0;
        _sock = ::socket(AF_INET, SOCK_STREAM, 0);
        if((int)_sock < 0)
	    {
		    _error = os::ge_error();
            return _error;
	    }
        setblocking(0);
        _remote_sin.sin_family		= AF_INET;
        _remote_sin.sin_addr.s_addr	= locSin.sin_addr.s_addr;
        _remote_sin.sin_port		= htons(port);

        _connecting = 1;
        if(SOCKET_ERROR == ::connect(_sock, (const struct sockaddr*)&_remote_sin, sizeof(_remote_sin)))
        {
		    _error = os::ge_error();
            return -1;
        }
        _connecting = 0;
        return 0;
    }
    return -1;
}

int tcp_cli_sock::raw_connect(const char* suip4, int port)
{
    return raw_connect(htonl(inet_addr(suip4)), port);
}

int tcp_cli_sock::raw_connect(DWORD uip4, int port)
{
    _error = 0;
    if((int)_sock != (int)INVALID_SOCKET)
        destroy();
    _error = 0;
    _sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if((int)_sock < 0)
	{
		_error = os::ge_error();
        return _error;
	}
    setblocking(0);
    _remote_sin.sin_family		= AF_INET;
    _remote_sin.sin_addr.s_addr	= htonl(uip4);
    _remote_sin.sin_port		= htons(port);

    _connecting = 1;
    if(SOCKET_ERROR == ::connect(_sock, (const struct sockaddr*)&_remote_sin, sizeof(_remote_sin)))
    {
		_error = os::ge_error();
        return -1;
    }
    _connecting = 0;
    return 0;
}


//-----------------------------------------------------------------------------
int tcp_cli_sock::i4connect(DWORD ip, int port, CancelCB cbCall, void* pUser)
{
    int             err;
    fd_set          fdWr;
    timeval         tv = {1, 0};

    if((int)_sock != (int)INVALID_SOCKET){
        destroy();
    }
    _error = 0;
    _sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if((int)_sock < 0)
	{
		_error = os::ge_error();
        return _error;
	}
    _remote_sin.sin_family		= AF_INET;
    _remote_sin.sin_addr.s_addr	= htonl(ip);
    _remote_sin.sin_port		= htons(port);
    if(_hostent==0)
    {
        _hostent = gethostbyaddr((char*)&ip, (int)sizeof(long), AF_INET );
    }

    // non blocking node couse we can cancel it by Cancel Call
    if(0==cbCall) cbCall = sock::DefCBCall;
    setblocking(0);
    DWORD         ti = os::ge_gettick();
    _connecting = 1;
    err = ::connect(_sock, (const struct sockaddr*)&_remote_sin, sizeof(_remote_sin));
    if(err==SOCKET_ERROR )
    {
        _error = os::ge_error();
    }
    if(_error==EINPROGRESS || _error == WOULDBLOCK)
	{
		while(cbCall(pUser, os::ge_gettick()-ti))
		{
			FD_ZERO(&fdWr);
			FD_SET((UINT)_sock, &fdWr);
            int nfds = (int)_sock+1;
			err = ::select(nfds,0,&fdWr,0,&tv);
			if(err > 0)
			{
				if(FD_ISSET(_sock, &fdWr))
				{
					setblocking(1);
                    _connecting = 0;
					return 0;   // no error
				}
			}
			os::ge_sleep(10);
		}
	}
	if(err==SOCKET_ERROR)
	{
    	destroy();
        return INVALID_SOCKET;
	}
    _connecting = 0;
	return 0;

}

//-----------------------------------------------------------------------------
int tcp_cli_sock::s4connect(const char* sip, int port, CancelCB cbCall, void* pUser)
{
    int             err;
    fd_set          fdWr;
    timeval         tv = {1, 0};

    if(0==cbCall) cbCall = sock::DefCBCall;

    if((int)_sock != (int)INVALID_SOCKET){
        destroy();
    }
    _error = 0;
    _sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if((int)_sock < 0)
	{
		_error = os::ge_error();
        return _error;
	}
    if(0==_hostent)
        _hostent = ::gethostbyname(sip);
	_remote_sin.sin_family		= AF_INET;
    _remote_sin.sin_port		= htons(port);
#ifdef _WIN32
    _remote_sin.sin_addr.s_addr	= inet_addr(sip);
#else
	inet_aton(sip, &_remote_sin.sin_addr);
#endif //
    setblocking(0);
    _connecting = 1;
    err = ::connect(_sock, (const struct sockaddr*)&_remote_sin, sizeof(_remote_sin));
    if(err==SOCKET_ERROR)
    {
        _error = os::ge_error();
    }


	if(_error == EINPROGRESS || _error == WOULDBLOCK)
	{
		int nfds = (int)_sock+1;
		DWORD       ti = os::ge_gettick();
		int tdiff = os::ge_gettick()-ti;
		while(cbCall(pUser, tdiff))
		{
			FD_ZERO(&fdWr);
			FD_SET((UINT)_sock, &fdWr);
			err = ::select(nfds,0,&fdWr,0,&tv);
			if(err > 0)
			{
				if(FD_ISSET(_sock, &fdWr))
				{
					setblocking(1);
                    _connecting = 0;
					return 0;   // no error
				}
			}
			os::ge_sleep(10);
			tdiff = os::ge_gettick()-ti;
		}
	}
	if(err==SOCKET_ERROR)
	{
    	destroy();
        return INVALID_SOCKET;
	}
    _connecting = 0;
	return 0;
}

//-----------------------------------------------------------------------------
int tcp_cli_sock::connect(const char* sip, int port, CancelCB cbCall, void* pUser)
{
    _error = 0;
    sockaddr_in    locSin;
    _hostent = ::gethostbyname(sip);
    if(_hostent==0)
		_hostent = gethostbyaddr(sip, (int)str_len(sip), AF_INET );
    if(_hostent)
    {
        ::memcpy((char*)&(locSin.sin_addr), _hostent->h_addr, _hostent->h_length);
        return this->s4connect(IP2String(locSin.sin_addr.s_addr),
							   port, cbCall, pUser);
    }
    _error = os::ge_error();
    return -1;
}


//-----------------------------------------------------------------------------
BOOL sock::DefCBCall(void*, DWORD dw)
{
    return dw < sock::_tout;
}

//-----------------------------------------------------------------------------
SOCKET udp_sock::create(int opt)
{
    _error = 0;
    _sock = ::socket (AF_INET, SOCK_DGRAM, 0);
    if ((int)_sock < 0)
    {
        _error = os::ge_error();
        destroy();
    }
    return _sock;
}

//-----------------------------------------------------------------------------
int  udp_sock::send(const char* buff, const int length, int port, const char* ip)
{
    return send((const BYTE*)buff, length, port, ip);
}

//-----------------------------------------------------------------------------
int  udp_sock::send(const BYTE* buff, const int length, int port, const char* ip)
{
    int snd ;
    _error = 0;
    if(_connected)
    {
        snd = ::send(_sock,(char *)buff, length, 0);
    }
    else
    {
        if(ip)
        {
            _remote_sin.sin_addr.s_addr = inet_addr(ip);
            _remote_sin.sin_port        = htons (port);
            _remote_sin.sin_family      = AF_INET;
        }
        snd = ::sendto(_sock, (char*)buff, length, 0,
                       (struct sockaddr  *) &_remote_sin,
                       sizeof (_remote_sin)) ;
    }
    if(-1 == snd){
        _error = os::ge_error();
    }
    return snd;
}

//-----------------------------------------------------------------------------
int  udp_sock::send(const char* buff, const int length, const struct sockaddr_in& rsin)
{
    return send((const BYTE*) buff, length, rsin);
}

//-----------------------------------------------------------------------------
int  udp_sock::send(const BYTE* buff, const int length, const struct sockaddr_in& rsin)
{
    int snd ;
    _error = 0;
    if(_connected)
    {
        snd = ::send(_sock,(char *)buff, length, 0);
    }
    else
    {
        snd = ::sendto(_sock, (char*)buff, length, 0,
                      (struct sockaddr  *) &rsin,
                      sizeof (rsin)) ;
    }
    if(-1 == snd){
        _error = os::ge_error();
    }
    return snd;
}

//-----------------------------------------------------------------------------
int  udp_sock::receive(char* buff, int length, struct sockaddr_in& rsin)
{
    return receive((BYTE*) buff, length, rsin);
}

//-----------------------------------------------------------------------------
int  udp_sock::receive(BYTE* buff, int length, struct sockaddr_in& rsin)
{
    int rcv;
    if(_connected)
    {
        rcv = ::recv(_sock,(char *)buff, length, 0);
    }
    else
    {
        socklen_t iRecvLen=(socklen_t)sizeof(rsin);
        rcv =  (int)::recvfrom (_sock,
                           (char*)buff,
		                   length,
                           0,
                           (struct sockaddr  *) &rsin,
                           &iRecvLen);
    }
    if(rcv==-1){
        _error = os::ge_error();
    }
    return rcv;
}

//-----------------------------------------------------------------------------
int  udp_sock::receive(char* buff, int length, int port, const char* ip)
{
    return receive((BYTE*)buff, length, port, ip);
}

//-----------------------------------------------------------------------------
int  udp_sock::receive(BYTE* buff, int length, int port, const char* ip)
{
    _error  = 0;
    int rcv ;
    if(_connected)
    {
        rcv = ::recv(_sock,(char *)buff, length, 0);
    }
    else
    {
        if(ip)
        {
            _remote_sin.sin_addr.s_addr = inet_addr(ip);
            _remote_sin.sin_port        = htons (port);
            _remote_sin.sin_family      = AF_INET;
            socklen_t iRecvLen                = sizeof(_remote_sin);
            rcv =  (int)recvfrom (_sock,
                               (char*)buff, length,
                               0,
                               (struct sockaddr  *) &_remote_sin,
                               &iRecvLen);

        }
        else
        {
            memset(&_remote_sin,0,sizeof(_remote_sin));
            socklen_t iRecvLen                = sizeof(_remote_sin);
            rcv =  (int)recvfrom (_sock, (char*)buff, length,
                                 0,
                                 (struct sockaddr  *) &_remote_sin,
                                 &iRecvLen);

        }
    }

    if(rcv==-1)
    {
        _error = os::ge_error();
    }
    return rcv;
}

//-----------------------------------------------------------------------------
SOCKET  udp_server_sock::create(int port)
{
    _error    = 0;
    _sock = ::socket(AF_INET, SOCK_DGRAM, 0);
    if((int)SOCKET_ERROR == (int)_sock)
        goto ERRX;
    ::memset(&_local_sin, 0, sizeof(_local_sin));
    _local_sin.sin_family        = AF_INET;
    _local_sin.sin_addr.s_addr   = htonl(INADDR_ANY);
    _local_sin.sin_port          = htons(port);
    ::bind(_sock,(struct sockaddr *)&_local_sin, sizeof(_local_sin));

    return 0;
ERRX:
    _error = os::ge_error();
    destroy();
    return (SOCKET)-1;
}

//-----------------------------------------------------------------------------
SOCKET  udp_client_sock::create(int opt)
{
    _error    = 0;
    _sock = ::socket(AF_INET, SOCK_DGRAM, 0);
    if(_sock>0)
        return 0;
    _error = os::ge_error();
    destroy();
    return (SOCKET)-1;
}


int  udp_sock::connect(const char* sip, int port, CancelCB cbCall, void* pUser)
{
    int             err;
    fd_set          fdWr;
    timeval         tv = {1, 0};
    _connected      = 0;

    if(_sock != INVALID_SOCKET)
    {
        this->destroy();
    }
    _error = 0;
    _sock = ::socket(AF_INET, SOCK_DGRAM, 0);
    if((int)_sock < 0)
	{
		_error = os::ge_error();
        return _error;
	}
	_remote_sin.sin_family		= AF_INET;
    _remote_sin.sin_addr.s_addr	= inet_addr(sip);
    _remote_sin.sin_port		= htons(port);
    memset(&_remote_sin.sin_zero, 0, 8);

    // non blocking node couse we can cancel it by Cancel Call
    setblocking(0);
    DWORD         ti = os::ge_gettick();
    err = ::connect(_sock, (const struct sockaddr*)&_remote_sin, sizeof(_remote_sin));
    if(0==cbCall)
        cbCall = sock::DefCBCall;

    while(cbCall(pUser, os::ge_gettick()-ti))
    {
        FD_ZERO(&fdWr);
        FD_SET((UINT)_sock, &fdWr);
        int nfds = (int)_sock+1;
        err = ::select(nfds,0,&fdWr,0,&tv);
        if(err > 0)
        {
            if(FD_ISSET(_sock, &fdWr))
            {
                setblocking(1);
                _connected = 1;
                return 0;   // no error
            }
        }
        os::ge_sleep(1);
    }
    destroy();
    return INVALID_SOCKET;
}



//-----------------------------------------------------------------------------
/*
// ip range 224.0.0.0 to 238.255.255.255 as
SOCKET udp_group_sock::create(int opt)
{
    _error = 0;
    ::memset(&_mcastGrp,0,sizeof(_mcastGrp));
	_groupmember = FALSE;

    // from server
    _local_sin.sin_family      = AF_INET;
    _local_sin.sin_port        = htons (0);
    _local_sin.sin_addr.s_addr = htonl (INADDR_ANY);

    if (::bind (_sock, (struct sockaddr*) &_local_sin,
              sizeof (_local_sin)) == SOCKET_ERROR)
    {
        _error =  ge_error();
		destroy();
		return -1;
    }
    int iOptVal = opt;  // time to live (in the net)
    if (setsockopt (_sock, IPPROTO_IP, IP_MULTICAST_TTL,
                    (char  *)&iOptVal, sizeof (int)) == SOCKET_ERROR)
    {
        _error =  ge_error();
		destroy();
		return -1;
    }
    return 0;
}

//-----------------------------------------------------------------------------
int udp_group_sock::send(const BYTE* buff, int length, int port, const char* ipGrp)
{
	int snd;

    _error = 0;
    _remote_sin.sin_family = AF_INET;
    _remote_sin.sin_port = htons (port);
    _remote_sin.sin_addr.s_addr = inet_addr(ipGrp);
    snd = ::sendto (_sock, (char*)buff, length, 0,
                    (struct sockaddr  *) &_remote_sin, sizeof (_remote_sin)) ;
    if(snd < 0)
    {
		_error = ge_error();
    }
    return snd;
}

//-----------------------------------------------------------------------------
int udp_group_sock::receive(BYTE* pbuff, int length, int port, const char* ipGrp)
{
    _error = 0;
    int iRecvLen = sizeof (_remote_sin);
    int i =  (int)recvfrom (_sock, (char*)pbuff, length, 0,
                            (struct sockaddr  *) &_remote_sin, &iRecvLen);
    if(i==-1)
    {
        _error = ge_error();
    }
    return i;
}

//-----------------------------------------------------------------------------
void    udp_group_sock::destroy()
{
    if(_groupmember)
    {
        _error = ::setsockopt (_sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                              (char  *)&_mcastGrp, sizeof (_mcastGrp)) ;
    }
    udp_sock::destroy();
}

//-----------------------------------------------------------------------------
int udp_group_sock::join(const char* ipGrp, int port)
{
    if(_sock != -1)
        return -1;

    _error = 0;
    _local_sin.sin_family = AF_INET;
    _local_sin.sin_port = htons (port);
    _local_sin.sin_addr.s_addr = htonl (INADDR_ANY);
    int one = 1;
    ::setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one));
    int bnd = ::bind (_sock, (struct sockaddr  *) &_local_sin, sizeof (_local_sin));
    if (bnd == -1)
    {
		_error =  ge_error();
		destroy();
		return bnd;
    }

    //join the multicast group
    _mcastGrp.imr_multiaddr.s_addr = inet_addr (ipGrp);
    _mcastGrp.imr_interface.s_addr = INADDR_ANY;
    bnd = ::setsockopt (_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char  *)&_mcastGrp, sizeof (_mcastGrp)) ;
    if (bnd == -1)
    {
		_error =  ge_error();
		destroy();
		return bnd;
    }
    _groupmember = TRUE;
    return 0;
}
*/
