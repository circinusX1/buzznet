#ifndef __XURL_H__
#define __XURL_H__

#define HEADER_DES_LOCL_H
#include "osg.h"
#include "core.h" 
#include "sock.h"
#include "strops.h"
#include "url.h"


#define TARGET_MAX_LENGTH       196
#define TARGET_LIMIT_LENGTH     164
#define URL_MAX_KEYS            65536
//-----------------------------------------------------------------------------

class Url 
{
    XCHAR            s_temp[256];
    XCHAR            s_service[16];         // http:// ftp:// etc
    XCHAR            s_site[80];
    XCHAR            s_doc[TARGET_MAX_LENGTH];
    XCHAR            s_params[TARGET_MAX_LENGTH];
    XCHAR            s_ip[32];
    XCHAR            s_refererSite[80];
    int              n_port;
    DWORD            u_hash;
    DWORD            u_docHash;//
    DWORD            n_recicled;
    BOOL             b_robots;
    DWORD            u_depth;    
    DWORD            u_hits;
    BOOL             b_valid;
    DWORD            u_delayed;
    int              n_dispersion;
    DWORD            u_docID;
    DWORD            u_refDocID;
    DWORD            u_siteID;
    BOOL			 b_redirected; 

public:
    Url(KXCHAR* urlName, int depth, const Url* parent=0);
    Url(KXCHAR* fileLine);
    virtual     ~Url(void);

    void        Trace();
    DWORD       SiteID()const {return u_siteID;}
    void        SiteID(DWORD id){u_siteID=id;}
    BOOL        SameSite(KXCHAR* w1, KXCHAR* w2)const;
    DWORD       DocID()const {return u_docID;}
    void        DocID(DWORD id){u_docID=id; }
    void        WriteTo(XCHAR* serialbuff);
    const int   SrvPort()const{return n_port;}
    const int   Depth()const{return u_depth;}
    void        Depth(int d){u_depth=d;}
    const DWORD Hits()const{return u_hits;}
    void        Hits(int h){u_hits=h;}
	Url*		New_RootDocument();	
    void        AddCookie(KXCHAR* cookie);
    KXCHAR*     SzIP()const{if(s_ip[0]) return s_ip;  return "0"; };
    void        SzIP(KXCHAR* ip){ if(ip && *ip) strcpy(s_ip,ip);}
    DWORD       DwIP()const{return ::inet_addr(s_ip);};
    KXCHAR*     Service()const{return s_service;}
    KXCHAR*     Document()const {return *s_doc ? s_doc : "/";};
    void        Document(XCHAR* destBuff)const;
    KXCHAR*     Parameters()const;
    void        RecileUp(){++n_recicled;}
    int         Recicle()const {return n_recicled;}
    void        Recicle(int c){n_recicled=c;}
    const BOOL  SameSite(const Url* p)const ; 
    BOOL        Robots()const {return b_robots;}
    void        Robots(BOOL br){b_robots=br;};
    BOOL        Good();
    BOOL        IsSSL()const{return !strcmp(s_service,"https://");}
    DWORD       SiteHash();
    DWORD       UniqHash(size_t maxKeys);
    XCHAR*      UniqHash();
    KXCHAR*     FileSiteName();
	int 		DispersDec(){
		if(n_dispersion > 0) --n_dispersion;return n_dispersion;
	}
	void        Dispersion(int disp){this->n_dispersion=disp;}
	int			Dispersion(){return this->n_dispersion;};
    long        RefererID()const{return u_refDocID; }
    void        RefererID( long u){u_refDocID = u;}
    void        FullName(XCHAR* destBuff)const;//++++
	BOOL		Redirected(){return b_redirected; }
	void		Redirected(BOOL b){b_redirected=b; }
    KXCHAR*     SiteName()const{return s_site;}
private:    
    
    void        _CleanDocument(){strcpy(s_doc,"/");s_params[0]=0;} 
    BOOL        _NormalizeTarget(XCHAR* );
    void        _Parse(KXCHAR* urlName, const Url* pParent);
    void        _Goon(XCHAR *ploco, const Url* pParent);
    KXCHAR*     _GetPath()const;

private:
public:
    static DWORD     l_urls;
    static XCHAR     sz_work[2048];
};


typedef counted_ptr<Url>        SpUrl;
typedef const counted_ptr<Url>  KSpUrl;
//--------------------------------------------------------------------------------

#endif //
