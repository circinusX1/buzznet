#ifndef _PAGE_PARSER_H__
#define _PAGE_PARSER_H__


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

    const   XCHAR*    ParseHeader(XCHAR* page, int &length, SpUrl& url , PageParser::MIMETYPE & mt);
    BOOL    ParsePage(KXCHAR* pageName, KXCHAR* page, size_t length, SpUrl& url, PageParser::MIMETYPE  mt);
  
    
private:
    BOOL    _ExtractLinks(KXCHAR* pageName, KXCHAR* page, int length, SpUrl& url);
    BOOL    _AddLinkString(XCHAR* web, set<string>&, SpUrl& );
   
};


#endif //_PAGE_PARSER_H__
