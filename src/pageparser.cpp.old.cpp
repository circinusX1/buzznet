
#include "stdafx.h"
#include "core.h"
#include "url.h"
#include "connection.h"
#include "ionet.h"
#include "buzznet.h"
#include "huffman.h"
#include "urllocalrep.h"
#include "httpserver.h"
#include "pageparser.h"



//---------------------------------------------------------------------------------------
BOOL    PageParser::ParseHeader(XCHAR* page, SpUrl& url)
{
    if(page==0)     
        return 0;
    if(*page==0)    
        return 0;
    
    const XCHAR* pafterHdr = so::strstr(page,"\r\n\r\n");
    if(pafterHdr==0) 
        return 0;

    int   rv = 1;
    const XCHAR* perrcode = page+9;
    int   errcode = ::atoi(perrcode);

    if(errcode != 200)
    {
		XCHAR   nw[512] = "";
        XCHAR* pLoc = strstr(page,"Location: ");
        if(pLoc)
        {
			if(strlen(pLoc)<9)
				return 0;
            pLoc += 9;
            while(*pLoc && *pLoc==' ')pLoc++;
            if(*pLoc==0)
            {
                PRINTF(_ERR,"Error: %d: %s%s\r\n", errcode, url->GetServerName(),url->GetObjectName());
                return 0;
            }

			try{
				so::strccpy(nw,pLoc,'\r');
				SpUrl refUrl(new Url(nw, BuzzEngine::n_depth, (Url*)url.get()));
				if(refUrl->IsValid())
					PBuzz->PutInWorkingQueue(refUrl, url.get());
			}catch(...)
			{
				PRINTF(_ERR,"Exception allocating new URL' %s'\r\n",nw);
				return 0;
			}

            PRINTF(_DEB,"Location: %d: %s->%s\r\n", errcode, url->GetServerName(), nw);
        }
        else
        {
            PRINTF(_WRN,"HTTP-Error: %d: %s%s\r\n", errcode, url->GetServerName(),url->GetObjectName());
            rv = 0;
        }
    }
    return rv;
}

void PageParser::ParsePage(const XCHAR* pageName, XCHAR* page, size_t length, SpUrl& url)
{
    set<string>   luq;
    const  XCHAR* ph  = page;
    const  XCHAR* ph2  = page;
    int    skiptag;
    bool   parsed = 0; 


    string sk = url->GetHashKey();
    luq.insert(sk);

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

        do{
			if(*ph==0)
				break;
			if(strlen(ph)>255)
				break;
            Iurl* pn = 0;
			try {
			    pn = new Url(ph, PBuzz->n_depth, (Url*)url.get());
			}catch(...)
			{
                PRINTF(_ERR,"Exception allocating new URL: %s\r\n");
				break;
			}
            SpUrl refUrl2(pn);
            if(!refUrl2->IsValid())
            {
				if(!strchr(ph,'%'))
                    PRINTF(_ERR,"%s not valid\r\n", ph);
                break;
            }

            sk =  refUrl2->GetHashKey();
            if(luq.find(sk) != luq.end())
            {
                break;
            }
            luq.insert(sk);

            if(refUrl2->IsParent(url.get())) // robots were queries already couse we are here
            {
                refUrl2->SetRobots(url->HasRobots());
                if(!PBuzz->_pRP->RobotsAllow(refUrl2))
                {
                    break;
                }
            }

            if(PBuzz->b_sitemap)
            {
                if(!strcmp(refUrl2->GetServerName(), url->GetServerName()))
                {
                    PBuzz->PutInWorkingQueue(refUrl2);
                }
            }
            else
            {
                PBuzz->PutInWorkingQueue(refUrl2);
            }
        }while(0);

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
}
/*
int main(int argc, char* argv[])
{
  str_istream si("C:\\_WEBS\\a\\d\\r\\www.adrhi.com\\#0.htm");

  basic_string<wchar_t> ttt;  

  markup::scanner sc(si);
  bool in_text = false;
  while(true)
  {
    int t = sc.get_token();
    switch(t)
    {
      case markup::scanner::TT_ERROR:
        printf("ERROR\n");
        break;
      case markup::scanner::TT_EOF:
        printf("EOF\n");
        goto FINISH;
      case markup::scanner::TT_TAG_START:
        printf("TAG START:%s---------------\n", sc.get_tag_name());
        ttt.clear();
        break;
      case markup::scanner::TT_TAG_END:
        printf("TAG END:%s-----------------\n", sc.get_tag_name());        
         printf("\r\n");
         printf("(%s)",ttt.c_str());
        printf("\r\n");
        ttt.clear();
        
        break;
      case markup::scanner::TT_ATTR:
        printf("\tATTR:%s=%S\n", sc.get_attr_name(), sc.get_value());
        break;
      case markup::scanner::TT_WORD: 
      case markup::scanner::TT_SPACE:
        //printf("{%S}\n", sc.get_value());
          ttt+=sc.get_value();
        break;
    }
  }
FINISH:
  printf("--------------------------\n");
  return 0;
}

*/

