
#include "stdafx.h"
#include "core.h"
#include "url.h"
#include "connection.h"
#include "ionet.h"
#include "buzznet.h"
#include "storage.h"
#include "pageparser.h"



//---------------------------------------------------------------------------------------
KXCHAR*    PageParser::ParseHeader(XCHAR* page, int &length, 
                                   SpUrl& url, PageParser::MIMETYPE& mt)
{
    p_afterHeader = 0;
    u_hdrlen      = 0;
    u_length      = 0;

    p_afterHeader = so::strstr(page,"\r\n\r\n");

    if(p_afterHeader==0) 
    {
        length = 0;
        return   0;
    }
    u_length      = str_len(p_afterHeader);

    if(u_length < 4)
    {
        u_length = 0;
        length   = 0;
        return   0;
    }

    p_afterHeader += 4;
    u_length -=4;
    u_hdrlen = (int)(p_afterHeader - page);
    length   -= u_hdrlen;
    /*
	if(length <= 0)
	{
		length   = 0;
        u_length = 0;
        p_afterHeader = 0;
        return 0; 
	}	
    */
    KXCHAR* perrcode = page + 9;
    int     errcode = ::t_atoi(perrcode);

    if(errcode != 200)
    {
		XCHAR   nw[512] = "";
        if(errcode == 301)
        {
            XCHAR* pLoc = strstr(page,"Location: ");
            if(pLoc)
            {
			    if(str_len(pLoc)<9)
				    return 0;
                pLoc += 9;
                while(*pLoc && *pLoc==' ')pLoc++;
                if(*pLoc==0)
                {
                    PRINTF(_D,"Error:%d -> %s%s\r\n", errcode, url->FileSiteName(),url->Document());
                    return 0;
                }

				if(!url->Redirected())
				{
					try{
						so::strccpy(nw, pLoc, '\n');
						so::trimcrlf(nw);
						
						SpUrl refUrl(new Url(nw, Buz::u_depth, url.get()));
						if(refUrl->Good())
						{
							PRINTF(_D,"Location:[%d] %s -> %s\r\n", errcode, url->FileSiteName(), nw);
                            url->Redirected(1);
							P_Buz->OnRedirect(url);
							P_Buz->PushURL(refUrl, url.get(),-1);
						}
					}catch(...)
					{
						PRINTF(_E|_D,"Exception allocating new URL' %s'\r\n",nw);
						return 0;
					}
				}
                
            }
        }
        else
        {
            P_Buz->OnConnectError(url);
            PRINTF(_D,"HTTP-Error: %d: %s%s\r\n", errcode, url->FileSiteName(),url->Document());
            p_afterHeader = 0;
            length = 0;
            return 0;
        }
    }
    
    KXCHAR* pCt = so::strstr(page,"Content-Type:");
    if(pCt && strlen(pCt) > 15)
    {
        char content[256];
        pCt += 13*sizeof(XCHAR);
        while(*pCt && *pCt==' ')pCt++;

		if(strchr(pCt,'\r'))
		{
			so::strccpy(content,pCt,'\r');
	
			if(!t_strncmp(pCt,"application",sizeof(XCHAR)*11))
				mt=PageParser::eAPP;
			else if(!t_strncmp(pCt,"audio",sizeof(XCHAR)*5))
				mt=PageParser::eAUDIO;
			else if(!t_strncmp(pCt,"image",sizeof(XCHAR)*5))
				mt=PageParser::eIMAGE;
			else if(!t_strncmp(pCt,"message",sizeof(XCHAR)*7))
				mt=PageParser::eMESSAGE;
			else if(!t_strncmp(pCt,"multipart",sizeof(XCHAR)*9))
				mt=PageParser::eMULTIPART;
			else if(!t_strncmp(pCt,"text",sizeof(XCHAR)*4))
				mt=PageParser::eTEXT;
			else if(!t_strncmp(pCt,"video",sizeof(XCHAR)*5))
				mt=PageParser::eVIDEO;
		}
		else
			mt=PageParser::eAPP;
    }

    return p_afterHeader;
}


//---------------------------------------------------------------------------------------
BOOL PageParser::ParsePage(KXCHAR* pageName, KXCHAR* page, size_t length, SpUrl& url, PageParser::MIMETYPE mt)
{
    switch(mt)
    {
        case PageParser::eTEXT:
            return _ExtractLinks(pageName, page, (int)length, url);
            
    }
    return 0;
}


//---------------------------------------------------------------------------------------
SITEEX PageParser::_AddLinkString( XCHAR* web, set<string>& luq , 
											SpUrl& url)
{
	SITEEX rl;
    Url* 			pn = 0;
	
	try {
		pn = new Url(web, P_Buz->u_depth, url.get());
	}catch(...)
	{
        PRINTF(_E|_D,"Exception allocating new URL: %s\r\n");
		return e_REJECT;
	}
    SpUrl refUrl2(pn);
    if(!refUrl2->Good())
    {
        return e_REJECT;
    }

    string sk =  refUrl2->UniqHash();
    if(luq.find(sk) != luq.end())
    {
        return e_REJECT;;
    }
    luq.insert(sk);
	
	
    int 		isparent = refUrl2->SameSite(url.get());
	const Url*  ppUrl    = url.get();

    if(isparent)
    {
        refUrl2->Robots( url->Robots() );
        if(!P_Buz->_pRP->PassRobots(refUrl2))
        {
            return e_REJECT;;
        }
    }

    if(Buz::b_externalsites == 0)//stick to this site
    {
		rl = e_SITEADDED;
        if(isparent)
		{
			rl = P_Buz->PushURL(refUrl2,ppUrl,0,-1);
		}
		return rl;
    }

	if(isparent)
    {
   		return P_Buz->PushURL(refUrl2,ppUrl,0,-1);
    }

	Url* proot = refUrl2->New_RootDocument();
	if(proot)
	{
		SpUrl ru(proot);
		P_Buz->PushURL(ru,ppUrl,0,1);
	}
    P_Buz->PushURL(refUrl2,ppUrl,0,1);
	return e_SITEADDED;
}

//---------------------------------------------------------------------------------------
BOOL PageParser::_ExtractLinks(KXCHAR* pageName, KXCHAR* page, int length, SpUrl& url)
{
    set<string>   luq;
    const  XCHAR* ph  = page;
    const  XCHAR* ph2  = page;
    int    skiptag;
    bool   parsed = 0; 
    int    linkPerpage = 0;
	SITEEX rl;

  

    string sk = url->UniqHash();
    luq.insert(sk);

    P_Buz->_pRP->LoadRobots(url);


    while((ph = so::stristr(ph, "href=")) || 
          (ph2 = so::stristr(ph2, "src=")) )
    {
        skiptag = 5;
        if(ph == 0)
        {
           ph =  ph2;
           skiptag = 4;
        }
        parsed = 1;
        ph    += skiptag;
        while(*ph==' ' && *ph!=0)ph++;
        XCHAR gimir = *ph++;
        if(gimir!='\'' && gimir!='\"')
        {
            ph--;
            gimir='>';
        }
        XCHAR* end  = (XCHAR*)strchr(ph,gimir);
        if(!end)break;
        *end++ = 0;

		if(*ph==0)
			break;
		if(str_len(ph)>255)
			break;
        
        if(Buz::n_linkperpage > 0)
        {
            if(++linkPerpage < Buz::n_linkperpage)
            {
                rl = _AddLinkString((XCHAR*)ph, luq, url);
            }
        }
        else
		{
            rl = _AddLinkString((XCHAR*)ph, luq, url);
		}
		

        ph  = end;
        if(*ph!=0)
            ph++;
        else 
            break;
        if(*ph!=0)
            ph++;
        else
            break;
        ph2 = ph;
    }
    return 1;
}
