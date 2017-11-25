
#include "osg.h"
#include "url.h"
#include "strops.h"
#include "buzznet.h"

//=======================================================================================
DWORD    Url::l_urls;
XCHAR     Url::sz_work[2048];

//=======================================================================================

Url::Url(KXCHAR* urlName, int depth, const Url* pParent)
{
    ++Url::l_urls;
    u_refDocID    = NILLA; 
    u_docID    = NILLA; 
    u_siteID    = NILLA;
    u_hash     = NILLA;
    u_docHash = NILLA;
    n_port      = 0;
    s_site[0]  = 0;
    u_depth     = depth;
    n_recicled  = 0;
    b_robots    = 0;
    u_delayed  = 0;
    b_valid     = 1;
    s_doc[0] = 0;
    s_params[0] = 0;
    s_site[0]  = 0;
    s_service[0]= 0;
    s_ip[0]  = 0;
    u_hits      = 0;
    s_refererSite[0] = 0;
    b_redirected = 0;
	n_dispersion = Buz::n_dispersion;
    _Parse(urlName, pParent);
}

//---------------------------------------------------------------------------------------
Url::~Url(void)
{
    --Url::l_urls;
}

//---------------------------------------------------------------------------------------
void Url::FullName(XCHAR* destBuff)const//++++
{
    sprintf(destBuff,"%s%s%s",s_service, s_site, s_doc);
}


//---------------------------------------------------------------------------------------
void        Url::Trace()
{
    
}

//---------------------------------------------------------------------------------------
void        Url::AddCookie(KXCHAR* cookie)
{

}

//---------------------------------------------------------------------------------------
//
// inspired from larbin
//
BOOL Url::_NormalizeTarget(XCHAR* ptarget)
{
    int  i=0;

	//Buz::Printf(_RRQ,"%s\r\n",ptarget);
    while ( ptarget[i]  != 0 &&
            ptarget[i] != '#' )
    {
        if (ptarget[i] == '/' &&
            ptarget[i+1] != 0 &&
            ptarget[i+2] != 0)
        {
            if (ptarget[i+1] == '.' &&
                ptarget[i+2] == '/')
            {
                int j = i+3;
                while (ptarget[j] != 0)
                {
                    ptarget[j-2] = ptarget[j];
                    ++j;
                }
                ptarget[j-2] = 0;
            }
            else if (ptarget[i+1] == '/')
            {
                int j = i+2;
                while (ptarget[j] != 0)
                {
                    ptarget[j-1] = ptarget[j];
                    ++j;
                }
                ptarget[j-1] = 0;
            }
            else if (  ptarget[i+1] != 0 &&
                       ptarget[i+2] != 0 &&
                       ptarget[i+3] != 0 &&
                       ptarget[i+1] == '.' &&
                       ptarget[i+2] == '.' &&
                       ptarget[i+3] == '/')
            {
                if (i == 0)
                {
                    return 0;
                }
                else
                {
                    int j = i+4, dec;
                    --i;
                    while (ptarget[i] != '/') { --i; }
                    dec = i+1-j; // dec < 0
                    while (ptarget[j] != 0)
                    {
                        ptarget[j+dec] = ptarget[j];
                        ++j;
                    }
                    ptarget[j+dec] = 0;
                }
            }
            else if (ptarget[i+1] != 0 &&
                     ptarget[i+1] == '.' &&
                     ptarget[i+2] == 0)
            {
                ptarget[i+1] = 0;
                return 1;
            }
            else if (ptarget[i+1] != 0 &&
                     ptarget[i+2] != 0 &&
                     ptarget[i+1] == '.' &&
                     ptarget[i+2] == '.' &&
                     ptarget[i+3] == 0)
            {
                if (i == 0)
                {
                    return 0;
                }
                else
                {
                    i--;
                    while (ptarget[i] != '/')
                    {
                        i--;
                    }
                    ptarget[i+1] = 0;
                    return 1;
                }
            }
            else
            {
                i++;
            }
        }
        else if (ptarget[i] == '%')
        {
            int v1 = so::chartohex (ptarget[i+1]);
            int v2 = so::chartohex (ptarget[i+2]);
            if (v1 < 0 || v2 < 0) return 0;
            XCHAR c = 16 * v1 + v2;
            if ((unsigned char)c < 128 && (unsigned char)c > 9 && isgraph(c))
            {
                ptarget[i] = c;
                int j = i+3;
                while (ptarget[j] != 0)
                {
                    ptarget[j-2] = ptarget[j];
                    j++;
                }
                ptarget[j-2] = 0;
                i++;
            }
            else if (c == ' ' || c == '/')
            {
                i += 3;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            i++;
        }
    }
    ptarget[i] = 0;

	
    XCHAR* params = strchr(ptarget,'?');
    if(params)
    {
        memset(s_params,0,sizeof(s_params));
        int   j = 0;
        for(size_t i=0;i<str_len(params) && j<(TARGET_MAX_LENGTH-8); i++)
        {
            if(params[i]==' ')
            {
                s_params[j++]= '%';
                s_params[j++]= '2';
                s_params[j++]= '0';
                continue;
            }
            s_params[j++]= params[i];
        }
        //strncpy(s_params, params, min(sizeof(s_params)-2, str_len(params)));
        *params = 0;
    }
    return 1;
}

//---------------------------------------------------------------------------------------
KXCHAR* Url::_GetPath()const
{
    if(s_doc[0] && so::str_count(s_doc,'/') > 1)
    {
        strcpy(sz_work, s_doc);
        XCHAR* first = 0;
        if(so::strstr(sz_work,"http://"))
            first = so::strnchr(sz_work,'/',3);
        else
            first = so::strnchr(sz_work,'/',1);

        XCHAR* last  = so::strlastchr(first,'/');
        *(last+1)=0;
        return first;
    }
    return "/";
}

//---------------------------------------------------------------------------------------
/*
KXCHAR* Url::GetFile()const
{
    if(s_doc[0] && strchr(s_doc,'.'))
    {
        strcpy(sz_work, s_doc);

        XCHAR* pdot = so::strlastchr(sz_work,'.');
        XCHAR* last = so::str_chr(pdot,'?');
        XCHAR* first = so::strprevchr(sz_work, pdot,'/');
        *last = 0;
        return first+1;
    }
    return "/";
}
*/

//---------------------------------------------------------------------------------------
void Url::_Goon(XCHAR* ploco, const Url* pParent)
{
    so::strscpy(s_site, ploco, "?:/");
    //s_refererSite[0]=0;

    ploco += str_len(s_site);
    if(*ploco)
    {
        if(*ploco==':') //port
        {
            ++ploco;
            so::strccpy(sz_work, ploco, '/');
            n_port = ::t_atoi(sz_work);
            ploco+=str_len(sz_work);
        }
        if(*ploco=='/')//path
        {
            memset(s_doc,0,sizeof(s_doc));
            
			
			strncpy(s_doc, ploco, t_min((int)sizeof(s_doc)-2,(int)str_len(ploco)));
			
            if(!_NormalizeTarget(s_doc))
            {
                s_doc[0] = 0;
                b_valid     = 0;
            }
        }
        else
        {
            b_valid = 0;
        }
    }

    if(b_valid)
    {
        if(SameSite(s_site, s_refererSite))
        {
            if(u_depth>=0) 
                u_depth--;
            ++u_hits;   

			if(strlen(s_site)<strlen(s_refererSite))
            	strcpy(s_site, s_refererSite);
			           
            this->RefererID(pParent->DocID());
            this->SiteID(pParent->SiteID());

            this->SzIP(pParent->SzIP());
            if(0==n_port)
            n_port       = pParent->SrvPort();
            u_hits       = pParent->Hits()+1;  // ???

        }
        else
        {
            strcpy(s_refererSite, "*");
            this->RefererID(NILLA);
            this->SzIP(0);
           // n_port       = 80;
            u_hits       = 0;  // ???
            this->RefererID(NILLA);
            this->SiteID(NILLA);
        }
    }

}

//---------------------------------------------------------------------------------------
void   Url::_Parse(KXCHAR* urlName, const Url* pParent)
{
    if(pParent)
    {
        strcpy(s_refererSite, pParent->SiteName());
        this->RefererID(pParent->DocID());
        this->SiteID(pParent->SiteID());
        this->SzIP(pParent->SzIP());
        n_port       = pParent->SrvPort();
        u_hits       = pParent->Hits()+1;  // ???
    }

    XCHAR* ploco = (XCHAR*)urlName;

    if( so::strbegin(ploco,"ftp://")    || 
		so::strbegin(ploco,"mailto:")   || 
		so::strbegin(ploco,"about:")    ||
		so::strbegin(ploco,"irc://")    ||
		so::strbegin(ploco,"news://")  )
    {
        b_valid = 0;
        return;
    }

    if(so::strbegin(ploco,"http://"))
    {
        n_port = 80;
        strcpy(s_service,"http://");
        ploco += 7;
        _Goon(ploco, pParent);
    }
    else if(so::strbegin(ploco,"https://"))
    {
        n_port = 443;
        strcpy(s_service,"https://");
        ploco += 8;
        _Goon(ploco, pParent);
    }
    else  if(pParent)
    {

        if(so::strbegin(ploco,"http:"))
        {
            strcpy(s_service,"http://");
            ploco+=5;
        }
        else if(so::strbegin(ploco,"https:"))
        {
            strcpy(s_service,"https://");
            ploco+=6;
        }
        else
        {
            strcpy(s_service, pParent->Service());

            if(u_depth>=0) u_depth--;
            ++u_hits;   

            strcpy(s_refererSite, pParent->SiteName());
            this->RefererID(pParent->DocID());
            this->SiteID(pParent->SiteID());
            this->SzIP(pParent->SzIP());
            n_port       = pParent->SrvPort();
            u_hits       = pParent->Hits()+1;  // ???
        }

        XCHAR* pskp = so::strskipalnum(ploco);
        if(*pskp !=':')
        {
            while(*ploco && (*ploco=='\\' || *ploco=='\"'))
            {
                ploco++;
            }
            if(*ploco=='/')
            {
                memset(s_doc,0,sizeof(s_doc));
                strncpy(s_doc, ploco, t_min((int)sizeof(s_doc)-2,(int)str_len(ploco)));
            }
            else if(*ploco=='.')
            {
                strcpy(s_doc, pParent->_GetPath());

                if(*(ploco+1) && *(ploco+1)=='.' ) //backpedal
                {
                    XCHAR* plastsl = so::strlastchr(s_doc, '/');
                    XCHAR* prevsl  = so::strprevchr(s_doc, plastsl-1, '/');
                    *(prevsl) = 0;
                    ploco++;
                }
                if(!so::strcat(s_doc, ploco+1, sizeof(s_doc)-2))
                {
                    s_doc[0]=0;
                    b_valid = 0;
                    return;
                }
            }
            else
            {
                strcpy(s_doc, pParent->_GetPath());
                if(!so::strcat(s_doc, ploco, sizeof(s_doc)-2))
                {
                    s_doc[0]=0;
                    b_valid = 0;
                    return;
                }
            }
            if(!_NormalizeTarget(s_doc))
            {
                s_doc[0]=0;
                b_valid = 0;
                return;
            }
            strcpy(s_site, s_refererSite);

            if(u_depth > 0) --u_depth;
        }
        else
            b_valid = 0;
    }
}

//---------------------------------------------------------------------------------------
void Url::Document(XCHAR* destBuff)const
{
    if(s_doc[0])
    {
        strcpy(destBuff, s_doc);
        if(destBuff[str_len(destBuff)-1]=='/')
        {
            strcat(destBuff,"#0.htm");
        }
        else if  (!strchr(destBuff,'.'))
        {
            strcat(destBuff,"/#0.htm");
        }
        
        if(Buz::b_savepageparams && s_params[0])
        {
            strcat(destBuff, s_params);
        }
    }
    else
    {
        strcpy(destBuff,"/#0.htm");
    }
}


//-----------------------------------------------------------------------------
DWORD Url::UniqHash(size_t maxKeys)
{
	if(*s_site==0)
	    return NILLA;

    if(u_hash!=NILLA)
        return u_hash;

    DWORD hkey = max(0, n_port-80);
    int k=0;
    int digit;

    k=(int)str_len(UniqHash());
    while(--k>-1)
    {
        digit = sz_work[k]-0x20;
        if(digit<0)digit=0;
        hkey = 31 * hkey + digit;
    }
	hkey&=0x7FFFFFFF;
    u_hash = hkey % (DWORD)maxKeys;
	
    return u_hash;
}

//-----------------------------------------------------------------------------
KXCHAR* Url::Parameters()const 
{
    if(Buz::b_useparameters)
        return s_params;
    return "";
};

//-----------------------------------------------------------------------------
DWORD    Url::SiteHash()
{
	if(*s_site==0)
	    return NILLA;

    if(u_docHash != NILLA)
        return u_docHash;

    int   i = 0;
    if(so::strbegin(s_site,"www.")) i = 4;

    int     digit;
    DWORD   hkey=0;
    while (s_site[i] != 0) 
    {
        if(!t_strncmp(&s_site[i],".com", 4))
        {
            break;
        }
        if(s_site[i] != '.' && 
           s_site[i] != '/')
        {
            digit = s_site[i]-0x20;
            if(digit<0)        digit=0;
            hkey = 31 * hkey + digit;
        }
        ++i;
    }
    u_docHash = hkey % Buz::n_maxsitetebles;
    return u_docHash;
}

//-----------------------------------------------------------------------------
XCHAR*  Url::UniqHash()
{
	if(strlen(s_site)<2)
	    return "*****";

    int   i    = 0, k=0;
    if(so::strbegin(s_site,"www."))i=4;

    while (s_site[i] != 0) 
    {
        if(s_site[i] != '.' && 
           s_site[i] != '/')
            sz_work[k++]=s_site[i];

        if(!t_strncmp(&s_site[i],".com", 4))
        {
            break;
        }
        ++i;
    }

    i = 0;
    while (s_doc[i] != 0) 
    {
        if(s_doc[i]!='=' && 
           s_doc[i]!='&' && 
           s_doc[i]!=' ' && 
           s_doc[i]!='/' && 
           s_doc[i]!='?' && 
           s_doc[i]!='.' && 
           s_doc[i]!='-')
            sz_work[k++]=s_doc[i];
        ++i;
    }
    if(Buz::b_useparameters && s_params[0])
    {
        i = 0;
        while (s_params[i] != 0) 
        {
             if(s_params[i]!='=' && 
                s_params[i]!='&' && 
                s_params[i]!=' ' && 
                s_params[i]!='/' && 
                s_params[i]!='?' && 
                s_params[i]!='.' && 
                s_params[i]!='-')
                sz_work[k++]=s_params[i];
            ++i;
        }

    }
    sz_work[k]=0;
    return sz_work;
}

BOOL      Url::Good()
{
	if(Buz::n_pagespersite)
		return b_valid && u_depth!=0;
	return b_valid && u_depth!=0 && s_site[0];
	
};

BOOL    Url::SameSite(KXCHAR* w1, KXCHAR* w2)const
{
    if(w1[0] && w2[0])
    {
	    if(strlen(w1)>strlen(w2))
		    return strstr(w1,w2)!=(XCHAR*)0;
	    return strstr(w2,w1)!=(XCHAR*)0;
    }
    return 0;
}

//=============================================================================
const BOOL   Url::SameSite(const Url* p)const 
{
	if(this->DwIP()==p->DwIP())
	{
		const XCHAR* ps = p->SiteName();
    	return SameSite(ps, s_site);
	}
	return 0;
}


void Url::WriteTo(XCHAR* serialbuff)
{

	if(Parameters()[0])
	{
		if(this->SrvPort()!=80)
			sprintf(serialbuff, "%s%s:%u%s%s|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d\r\n",   
																this->Service(), 
																this->SiteName(), 
																this->SrvPort(),
																this->Document(),
																this->Parameters(),
																u_siteID,
																u_docID,
																u_refDocID,
																u_depth,
																u_hits,
																b_robots,
																n_recicled,
																u_hash,
																u_docHash,
																n_dispersion);
		else
		   sprintf(serialbuff, "%s%s%s%s|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d\r\n",     this->Service(), 
																this->SiteName(), 
																this->Document(),
																this->Parameters(),
																u_siteID,
																u_docID,
																u_refDocID,
																u_depth,
																u_hits,
																b_robots,
																n_recicled,
																u_hash,
																u_docHash,
																n_dispersion);
	}
	else
	{
		if(this->SrvPort()!=80)
			sprintf(serialbuff, "%s%s:%u%s|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d\r\n",   
																this->Service(), 
																this->SiteName(), 
																this->SrvPort(),
																this->Document(),
																u_siteID,
																u_docID,
																u_refDocID,
																u_depth,
																u_hits,
																b_robots,
																n_recicled,
																u_hash,
																u_docHash,
																n_dispersion);
		else
		   sprintf(serialbuff, "%s%s%s|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d\r\n",     this->Service(), 
																this->SiteName(), 
																this->Document(),
																u_siteID,
																u_docID,
																u_refDocID,
																u_depth,
																u_hits,
																b_robots,
																n_recicled,
																u_hash,
																u_docHash,
																n_dispersion);
	}
}

Url::Url(KXCHAR* fileLine)
{
    XCHAR line[1024];

    strcpy(line,fileLine);
	XCHAR *pt        =0;
	XCHAR urlName[400];
    DWORD siteId     =-1;
    DWORD pageId     =-1;
    DWORD parentPageId=-1;
    DWORD depth       =4;
    DWORD hits        =0;
    DWORD robots      =0;
    DWORD recicled    =0;
    DWORD hash        =-1;
    DWORD tblindex    =-1;
	int	dispersion    = 1;
	
	pt=::strtok(line,"|");if(pt) strcpy(urlName,pt);
	pt=::strtok(0,"|"); if(pt)siteId = ::atoi(pt);
	pt=::strtok(0,"|"); if(pt)pageId       = ::atoi(pt);
    pt=::strtok(0,"|"); if(pt)parentPageId = ::atoi(pt);
    pt=::strtok(0,"|"); if(pt)depth        = ::atoi(pt);
    pt=::strtok(0,"|"); if(pt)hits         = ::atoi(pt);
    pt=::strtok(0,"|"); if(pt)robots       = ::atoi(pt);
    pt=::strtok(0,"|"); if(pt)recicled     = ::atoi(pt);
    pt=::strtok(0,"|"); if(pt)hash         = ::atoi(pt);
    pt=::strtok(0,"|"); if(pt)tblindex     = ::atoi(pt);
	pt=::strtok(0,"|"); 
	if(pt)
		dispersion   = ::atoi(pt);
	else
	{
		//assert(0);
		b_valid = 0;
		return;
	}
	
	
	
	
    ++Url::l_urls;
    n_port       = 0;
    s_site[0]   = 0;
    b_valid      = 1;
    s_doc[0]  = 0;
    s_params[0]  = 0;
    s_site[0]   = 0;
    s_service[0] = 0;
    s_ip[0]       = 0;
    s_refererSite[0] = 0;
    _Parse(urlName, 0);


    u_siteID        = siteId        ;
    u_docID        = pageId        ;
    u_refDocID      = parentPageId  ;
    u_depth         = depth         ;
    u_hits          = hits          ;
    b_robots        = robots        ;
    n_recicled      = recicled      ;
    u_hash         = hash          ;
    u_docHash     = tblindex      ;
	n_dispersion    = dispersion    ;
}



Url*  Url::New_RootDocument()
{
	if(!strcmp(this->Document(),"/"))
		return 0;
	XCHAR loco[400];
	
	sprintf(loco, "%s%s",this->Service(),this->SiteName());
	Url* pnew = new Url(loco, Buz::u_depth, this);
	if(pnew)
	{
		if(pnew->Good())
		{
			pnew->RefererID(this->DocID());
			pnew->_CleanDocument();
			return pnew;
		}
		delete pnew;
	}
	return 0;
}

KXCHAR* Url::FileSiteName()
{
    strcpy(s_temp, this->SiteName());
    so::to_db_filename(s_temp);
    return s_temp;
}