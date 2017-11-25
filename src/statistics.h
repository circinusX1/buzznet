#ifndef __STATISTICS_H__
#define __STATISTICS_H__ 
#include "osg.h"

class Statistics
{
public:
    Statistics(){
        n_maxsockets    = 0;
        n_inwait   = 0;
        n_connectingsocks= 0;
        n_inreceivesockes= 0;
        n_queuelength   = 0;
        n_fetchedurls   = 0;
        n_timeouturls   = 0;
        n_forbidenurls  = 0;
        n_bps           = 0;
        m_totalkbytes   = 0;
        n_starttime     = 0;
        n_currtime      = 0;
        n_webspersec    = 0;
        m_conss         = 0;
        m_dnsacc        = 0;
        m_consspersec   = 0;
        m_dnsaccpersec  = 0;
        n_bpspers       = 0; 
        n_uptime        = 0; 
        w_second        = 0; 
        n_nurls         = 0;
        m_conssTotal    = 0;
        m_dnsaccTotal   = 0;
        m_dnsrecs       = 0;
        n_sockets       = 0;
    };
    ~Statistics(){};

    DWORD n_currtime;
    DWORD n_starttime;
    int n_maxsockets;
    int n_inwait;
    int n_connectingsocks;
    int n_inreceivesockes;
    int n_queuelength;
    int n_fetchedurls;
    int n_timeouturls;
    int n_forbidenurls;
    int n_bps;
    int n_bpspers;
    DWORD m_totalkbytes;
    unsigned int n_uptime;
    unsigned int n_webspersec;
    unsigned int m_conssTotal    ;
    unsigned int m_dnsaccTotal   ;
    unsigned int m_conss         ;
	size_t       m_dnsrecs;
    unsigned int m_dnsacc        ;
    unsigned int m_consspersec  ;
    unsigned int m_dnsaccpersec ;
    DWORD w_second;
    unsigned int n_nurls;
    int          n_sockets;
};


#endif //
