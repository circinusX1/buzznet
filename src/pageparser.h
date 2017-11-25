#ifndef _PAGE_PARSER_H__
#define _PAGE_PARSER_H__

#include "storage.h"

class PageParser
{
public:
    typedef enum _MIMETYPE{
        eUNK         = -1,
        eAPP         = 0,
        eAUDIO,
        eIMAGE,
        eMESSAGE,
        eMULTIPART,
        eTEXT,
        eVIDEO,
    }MIMETYPE;

    PageParser(){
    }

    const    XCHAR*    ParseHeader(XCHAR* page, int &length, SpUrl& url , PageParser::MIMETYPE & mt);
    BOOL     ParsePage(KXCHAR* pageName, KXCHAR* page, size_t length, SpUrl& url, PageParser::MIMETYPE  mt);
    KXCHAR*  AfterHeaderContent()const{return p_afterHeader;}
    int      HdrLen(){return u_hdrlen;}
    DWORD    GetContentLength()const{return u_length;}
private:
    BOOL     _ExtractLinks(KXCHAR* pageName, KXCHAR* page, int length, SpUrl& url);
    SITEEX   _AddLinkString(XCHAR* web, set<string>&, SpUrl& );

private:
    KXCHAR*  p_afterHeader;
    int      u_hdrlen;
    DWORD    u_length;
};


#endif //_PAGE_PARSER_H__
