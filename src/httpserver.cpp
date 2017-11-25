//--------------------------------------------------------------------------------------

#include "httpserver.h"
#include "buzznet.h"

//--------------------------------------------------------------------------------------
//
HttpServer::HttpServer()
{
    
}

//--------------------------------------------------------------------------------------
//

HttpServer::~HttpServer()
{
    if(_peng)
    {
        _srvsock.destroy();
        Stop();
    }
}

//--------------------------------------------------------------------------------------
//
void HttpServer::Start(int port, Buz* peng)
{
    _peng = peng;
    _port = port;
    if(this->OsThread::Start())
    {
        _peng = 0; // not started
    }
    os::ge_sleep(1000);
    if(BStop())
        _peng = 0; // not started
}

//--------------------------------------------------------------------------------------
void HttpServer::ThreadFoo()
{
	Buz::Printf(_D,"HTTP Server Starting port %d\r\n",_port);
    tcp_cli_sock tc;
    if(_srvsock.create(_port))
    {
        IssueStop();
		Buz::b_BreaK = 1;
		PRINTF(_D|_E,"Cannot Create Server Socket on port %d. err:%d\r\n",_port,_srvsock.error());
        return;
    }
    char hdr[] = "HTTP/1.0 200 OK\r\n"
                 "Server: BuzzNet1.0\r\n"
                 "Content-Length: %d\r\n"
                 "Connection: Keep-Alive\r\n"
                 "Content-Type: text/html\r\n\r\n%s";
    char   red[512]="";
    char   req[256];
    char*   pred;
	if(0==_srvsock.listen())
	{
	
		while(!BStop())    
		{
            strcpy(red,"<br>");
			if(-1==_srvsock.accept(tc))
			{
				Buz::b_BreaK = 1;
                PRINTF(_D|_E,"\nAccept socket closed. %d Stopping crawler err: %d\r\n",_port, _srvsock.error());
				break;
			}

			int     servers = 0;
            int     rb = 0;
            char    server[128];
            int     acc = 0;
MORE_:
            if( (rb = tc.receive((BYTE*)req, 256))>0 )
			{
                req[rb] = 0;
				//PRINTF(_D,"<-:%s ",req);
           
	            if(req[0]=='*' || acc)
                {
					//   *ssss*ddddd*gggg*
                    for(int t=0;t < rb; t++)
                    {
                        if(req[t]=='*')
                        {
                            if(acc)
                            {
                                server[acc++] = 0;
                                //P_Buz->SaveInWorkingQueueFiles(server);
								++servers;
                                acc = 0;
                            }
                        }
                        else
                            server[acc++]=req[t];
                    }
					if (acc)
                    	goto MORE_;
					else
					{
						//PRINTF(_D,"Received %d URL Sites\r\n",servers);
						tc.send((BYTE*)&servers, 4);
						goto FINISH;
					}
                }


				if(pred=strstr(req,"stop="))
				{
					PRINTF(_I|_D,"Notified to stop\r\n");
					tc.destroy();
					Buz::b_BreaK = atoi(pred+5);
					break;
				}
				else if(pred=strstr(req,"pause="))
				{
					pred+=6;
					Buz::b_pause=::t_atoi(pred);
					
				}
				else if(pred=strstr(req,"delay="))
				{
					pred+=6;
					Buz::m_delaybeturls=::t_atoi(pred);
					if(Buz::m_delaybeturls > 5000)
						Buz::m_delaybeturls=5000;
					t_sprintf(red,"Delay: %d<br>",Buz::m_delaybeturls);
				}
				else if(pred=strstr(req,"external="))
				{
					pred+=9;
					Buz::b_externalsites=::t_atoi(pred);
					t_sprintf(red,"SiteMap: %d<br>",Buz::b_externalsites);
	
				}
				if(pred=strstr(req,"getget="))
				{
					strcpy(red,"Parameters: <br>stop=#&delay=#&external=#&forbid=$<br>");
				}
		
				// send http header
                char*  purls = new XCHAR[24000];
                purls[0]=0;
				int contLen = (int)_peng->GetStats(purls,24000);
                int aftrHdr = (int)str_len(red);
                t_sprintf(req, hdr, contLen+aftrHdr, red);
                
				tc.send(req, str_len(req));
                tc.send(purls,(int)contLen);

                red[0]=0;
                purls[0]=0;
                rb = 0;
                delete []purls;
			}
FINISH:
			tc.destroy();
            if(Buz::b_BreaK)
            {
                break;
            }
		}

    }
	else //listen
    {
		PRINTF(_D,"HTTP Cannot listen\r\n");
    }
    _srvsock.destroy();
    tc.destroy();
	Buz::b_BreaK=1;
    PRINTF(_D,"HTTP Server Finished. Killing Main Too!!!\r\n");
}

void HttpServer::Break()
{
        _srvsock.destroy();
}
