// 
//---------------------------------------------------------------------------------------
#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include "sock.h"

//---------------------------------------------------------------------------------------
class Buz;
class HttpServer : public OsThread
{
public:
    HttpServer(void);
    ~HttpServer(void);

    void Start(int port, Buz* peng);
    void ThreadFoo();
    void Break();

private:
    Buz*     _peng;
    int             _port;
    tcp_srv_sock    _srvsock;
};


#endif //__HTTP_SERVER_H__
