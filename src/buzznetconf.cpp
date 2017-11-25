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


Statistics  Buz::_stats;
int			Buz::n_dispersion     = -1;
BOOL        Buz::b_BreaK           = FALSE;
XCHAR       Buz::s_name[128]      = {0};
BOOL        Buz::b_pause          = 0;
BOOL        Buz::b_savepageparams = 1;
BOOL        Buz::b_compressfiles  = 1;
BOOL        Buz::b_useparameters  = 1;
int         Buz::b_httpport       = 8000; 
int         Buz::n_retries        = 3;
DWORD       Buz::n_rechecksite    = 20*3600*24; //days
int         Buz::m_delaybeturls   = 0;
DWORD       Buz::n_pagespersite   = 128;
int         Buz::n_maxconn        = 4;
DWORD       Buz::n_queuelength    = 2048;
XCHAR       Buz::s_proxi[128]     = {0};
int         Buz::n_proxiport      = 0;
XCHAR       Buz::s_youremail[260] = {0};
BOOL        Buz::b_externalsites  = 0;
int         Buz::u_depth          = 8;
int         Buz::n_vissize        = 4096;
XCHAR       Buz::s_destlocation[260]  = "_webs";
XCHAR       Buz::s_tempFolder[260]    = "logs";
XCHAR       Buz::s_appFolder[260]     = "app";
XCHAR       Buz::s_repFileName[260]   = {0};
XCHAR       Buz::s_queFileName[260]   = {0};
XCHAR       Buz::s_errFileName[260]   = {0};
int         Buz::n_maxsitetebles      = 2; 
BOOL        Buz::b_pushRootUrl        = 0;  
int         Buz::n_grouprule          = 6; 
long        Buz::dw_timeout           = 30000;
int         Buz::n_printout           = 7;  
int         Buz::n_linkperpage        =-1;
XCHAR       Buz::s_lo[1600]           = {0};
XCHAR       Buz::s_plugrepdbUser[64]  = {0};
XCHAR       Buz::s_plugrepdbPswd[64]  = {0};
XCHAR       Buz::s_plugrepdbDb[64]    = {0};
XCHAR       Buz::s_plugrepdbServer[64] = {0};
BOOL        Buz::b_checkexistDB        = 1;
BOOL        Buz::b_testing             = 0; 
int         Buz::n_maxPageSize         = (1024*256);
//---------------------------------------------------------------------------------------
BOOL Buz::Configure(KXCHAR* fname, BOOL initial)
{
    os::H_FILE* pf = f_open(fname, "rb");
    if(pf)
    {
		PRINTF(_D,"Reading file %s \r\n",fname);
        XCHAR   pred[128];
        XCHAR   val[128];
        XCHAR   line[256];
        BOOL    inComment = FALSE;
        while(!feof(pf))
        {
            if(fgets(line,255,pf))
            {
                so::trimall(line,' ');
				so::trimall(line,'\t');
                so::trimall(line,'\r');
                so::trimall(line,'\n');
                if(*line==0)
                    continue;
                if(!t_strncmp(line,"/*",2))
                {
                    inComment=1;
                    continue;
                }
                if(!t_strncmp(line,"*/",2))
                {
                    inComment=0;
                    continue;
                }
                if(inComment || *line=='#')
                    continue;
        
				KXCHAR* pnext = so::strccpy(pred, line,'=');

				if(pnext && *pnext)
				{
					so::strscpy(val, (XCHAR*)pnext+1, "#*");
					PRINTF(_D,"Reading %s =  %s \r\n",pred, val);
					_Assign(pred, val);
				}
            }
            else
                break;
            if(feof(pf))
                break;
        }
        f_close(pf);
    }

    if(Buz::s_plugrepdbDb[0]==0)
    {
        PRINTF(_I,"Database not set\r\n");
    }
    if(s_name[0]==0)
	{
		strcpy(s_name, "BuzzNet1.0");
	}

	if(Buz::s_youremail[0]==0)
	{
		PRINTF(_I,"Please provide your email in  buzznet.conf so people can complain if your crawler is too agressive\r\n");
		return 0;
	}

	if(s_proxi[0])
	{
        XCHAR  prx[128]={0};
		KXCHAR* pnext = so::strccpy(prx, s_proxi,':');

        strcpy(s_proxi, prx);

        if(pnext+1 && *(pnext+1))
		    n_proxiport = ::t_atoi(pnext+1);
        else
            n_proxiport=80;
	}
	if(initial && !_CreateRepository())
	{
		return 0;
	}
		
    if(initial)
    {
        mk_dir(Buz::s_tempFolder);
        sprintf(Buz::s_repFileName,"%s/report0",Buz::s_tempFolder);
        sprintf(Buz::s_queFileName,"%s/queue0",Buz::s_tempFolder);
        sprintf(Buz::s_errFileName,"%s/error0",Buz::s_tempFolder);
    }
    

    return 1;
}

//---------------------------------------------------------------------------------------
void Buz::_Assign(KXCHAR* pred, KXCHAR* val)
{
    if(!strcmp(pred,"httpserver"))
    {
        Buz::b_httpport=t_atoi(val);
        return;
    }

    if(!strcmp(pred,"parameters"))
    {
        Buz::b_useparameters=t_atoi(val);
        return;
    }

    if(!strcmp(pred,"saveparameters"))
    {
        Buz::b_savepageparams=t_atoi(val);
        return;
    }


    if(!strcmp(pred,"proxy"))
    {
        ::strcpy(s_proxi, (XCHAR*)val);
        return;
    }

    if(!strcmp(pred,"externalsites"))
    {
        b_externalsites = ::t_atoi(val);
        return;
    }

    if(!strcmp(pred,"depthsearch"))
    {
        u_depth = ::t_atoi(val);
        return;
    }

    if(!strcmp(pred,"bzip"))
    {
        Buz::b_compressfiles = ::t_atoi(val);
        return;
    }

    if(!strcmp(pred,"tempfolder"))
    {
        strcpy(s_tempFolder,val);
        so::trimright(Buz::s_tempFolder,'/');
        mk_dir(Buz::s_tempFolder);
        return;  
    }


    if(!strcmp(pred,"queuelength"))
    {
        n_queuelength = min(2046, ::t_atoi(val));
        if(n_queuelength < 128)
            n_queuelength = 128;
        return;
    }

    if(!strcmp(pred,"pagelife"))
    {
		int iv = t_atoi(val); if(iv==0)iv=1;
        Buz::n_rechecksite = (iv)*24*3600;
        return;
    }
    
    


    if(!strcmp(pred,"connections"))
    {
        n_maxconn = min(63, ::t_atoi(val));
        if(n_maxconn<=0)n_maxconn=1;
        return;
    }

    if(!strcmp(pred,"datainifolder"))
    {
        ::strcpy(s_appFolder,val);
        mk_dir(s_appFolder);
        return;
    }
	
	
	
    if(!strcmp(pred,"writelocation"))
    {
        ::strcpy(s_destlocation,val);
        mk_dir(s_destlocation);
        return;
    }

    if(!strcmp(pred,"name"))
    {
        ::strcpy(s_name,val);
        return;
    }


    if(!strcmp(pred,"delay"))
    {
        m_delaybeturls = ::t_atoi(val);
        if(m_delaybeturls > 5000)
            m_delaybeturls=5000;
        return;
    }
    if(!strcmp(pred,"pagespersite"))
	{
	    Buz::n_pagespersite=::t_atoi(val);
	}

    if(!strcmp(pred,"sitetables"))
	{
	    Buz::n_maxsitetebles=::t_atoi(val);
	}

	if(!strcmp(pred,"dispersion"))
	{
		Buz::n_dispersion  = atoi(val);
	}
	
    if(!strcmp(pred,"timeout"))
    {
        dw_timeout = max ( 10, ::t_atoi(val) );
        if(dw_timeout<=0)
			dw_timeout=10;
		else if(dw_timeout>120)
			dw_timeout=120;
		dw_timeout*=1000;
        return;
    }

    if(!strcmp(pred,"retryes"))
    {
        n_retries = min (6, ::t_atoi(val) );
        if(n_retries<=0)n_retries=1;
        return;
    }

    if(!strcmp(pred,"email"))
    {
        ::strcpy(s_youremail, val);
        return;
    }

    if(!strcmp(pred,"writerule"))
    {
        if(!strcmp(val,"deep"))
            n_grouprule=6;
        if(!strcmp(val,"asis"))
            n_grouprule=4;
        if(!strcmp(val,"bysite"))
            n_grouprule=1;
        else if(!strcmp(val,"byip"))
            n_grouprule=2;
        else if(!strcmp(val,"byip,bysite") || !strcmp(val,"byip,bysite"))
            n_grouprule=3;
        else if(!strcmp(val,"byip,asis") || !strcmp(val,"asis,byip"))
            n_grouprule=5;
        
        return;
    }

    if(!strcmp(pred,"printout"))
    {
		Buz::n_printout = 0;
		if(strchr(val,'E'))
			Buz::n_printout|=1;
		if(strchr(val,'W'))
			Buz::n_printout|=2;
		if(strchr(val,'I'))
			Buz::n_printout|=4;
		if(strchr(val,'D'))
			Buz::n_printout|=8;
		
		PRINTF(_D,"printout=%X\r\n",Buz::n_printout);
        return;
    }

    if(!strcmp(pred,"database"))
    {
        XCHAR loco[128];
        strcpy(loco, val);

        XCHAR* pTok = strtok(loco,",");
        strcpy(Buz::s_plugrepdbServer,pTok);
        
        if((pTok = strtok(0,",")))
            strcpy(Buz::s_plugrepdbDb,pTok);
        if((pTok = strtok(0,",")))
            strcpy(Buz::s_plugrepdbUser,pTok);
        if((pTok = strtok(0,",")))
            strcpy(Buz::s_plugrepdbPswd,pTok);
        return;
    }

    if(!strcmp(pred,"checkduplicates"))
    {
        if(!strcmp(val,"DB")||!strcmp(val,"db"))
            Buz::b_checkexistDB=1;
    }

    if(!strcmp(pred,"redirect"))
    {
        
    }
    if(!strcmp(pred,"testing"))
    {
        Buz::b_testing = 0;    
    }
    if(!strcmp(pred,"maximpage"))
    {
        Buz::n_maxPageSize = ::t_atoi(val);    
    }

    
}
