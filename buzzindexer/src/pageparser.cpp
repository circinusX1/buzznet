
#include "stdafx.h"
#include "core.h"
//#include "url.h"
//#include "connection.h"
//#include "ionet.h"
//#include "buzznet.h"
//#include "huffman.h"
//#include "urlrepository.h"
#include "pageparser.h"



//---------------------------------------------------------------------------------------
KXCHAR*    PageParser::ParseHeader(XCHAR* page, int &length, SpUrl& url, PageParser::MIMETYPE& mt)
{
    KXCHAR* pafterHdr = so::strstr(page,"\r\n\r\n");
    if(pafterHdr==0 || str_len(pafterHdr) < 4) 
    {
        length = 0;
        return   0;
    }
    pafterHdr += 4;
    length -= (int)(pafterHdr - page);
    
    KXCHAR* perrcode = page + 9;
    int     errcode = ::t_atoi(perrcode);

    if(errcode != 200)
    {
		XCHAR   nw[512] = "";
        XCHAR* pLoc = strstr(page,"Location: ");
        if(pLoc)
        {
			if(str_len(pLoc)<9)
				return 0;
            pLoc += 9;
            while(*pLoc && *pLoc==' ')pLoc++;
            if(*pLoc==0)
            {
                PRINTF(_D,"Error: %d: %s%s\r\n", errcode, url->GetServerName(),url->GetObjectName());
                return 0;
            }

			try{
				so::strccpy(nw, pLoc, '\r');
				SpUrl refUrl(new Url(nw, Buz::n_depth, url.get()));
				if(refUrl->IsValid())
                {
                    P_Buz->OnRedirect(url);
					P_Buz->PutInWorkingQueue(refUrl, url.get());
                }
			}catch(...)
			{
				PRINTF(_E|_D,"Exception allocating new URL' %s'\r\n",nw);
				return 0;
			}

            PRINTF(_D,"Location: %d: %s->%s\r\n", errcode, url->GetServerName(), nw);
        }
        else
        {
            P_Buz->OnConnectError(url);
            PRINTF(_D,"HTTP-Error: %d: %s%s\r\n", errcode, url->GetServerName(),url->GetObjectName());
            pafterHdr = 0;
        }
    }

    KXCHAR* pCt = so::strstr(page,"Content-Type:");
    if(pCt)
    {
        char content[128] = {0};
        pCt+=13*sizeof(XCHAR);
        while(*pCt && *pCt==' ')pCt++;

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
    return pafterHdr;
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
BOOL PageParser::_AddLinkString( XCHAR* web, set<string>& luq , SpUrl& url)
{
    Url* pn = 0;
	try {
		pn = new Url(web, P_Buz->n_depth, url.get());
	}catch(...)
	{
        PRINTF(_E|_D,"Exception allocating new URL: %s\r\n");
		return 0;
	}
    SpUrl refUrl2(pn);
    if(!refUrl2->IsValid())
    {
        return 0;
    }

    string sk =  refUrl2->GetHashKey();
    if(luq.find(sk) != luq.end())
    {
        return 0;
    }
    luq.insert(sk);
    int 		isparent = refUrl2->IsParent(url.get());
	const Url*  ppUrl    = url.get();
	if(0 == Buz::b_externalsites)	
    {
		if(isparent)
		{
			
	        refUrl2->SetRobots(url->HasRobots());
			if(P_Buz->_pRP->RobotsAllow(refUrl2))
			{
				return (P_Buz->PutInWorkingQueue(refUrl2,ppUrl,0,1)!=
				                                 Storage::e_OVERLOAD);		 
			}
		}
		return 1;
	}
   	return (P_Buz->PutInWorkingQueue(refUrl2,ppUrl,0,!isparent) != Storage::e_OVERLOAD);
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


    string sk = url->GetHashKey();
    luq.insert(sk);

    P_Buz->_pRP->LoadRobots(url);


    while((ph = so::strstr(ph, "href=")) || 
          (ph2 = so::strstr(ph2, "src=")) )
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
                _AddLinkString((XCHAR*)ph, luq, url);
        }
        else
            _AddLinkString((XCHAR*)ph, luq, url);

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
