#ifndef __PARSER_H__
#define __PARSER_H__

#include "osg.h"



namespace so
{
    BOOL strbegin(KXCHAR* beg, KXCHAR* str);
    BOOL strend(KXCHAR* hay, KXCHAR* str);
    KXCHAR* strstr(KXCHAR* beg, KXCHAR* str);
    XCHAR *strnew(KXCHAR *psz);
    BOOL isdigit(XCHAR c);
    void strlower(XCHAR* a);
    int chartohex (XCHAR c) ;
    BOOL is_alnum(KXCHAR c);
    void   trimrightsp(XCHAR* s);
    XCHAR* strlastchr(XCHAR* s, XCHAR p);
    XCHAR* strprevchr(XCHAR* s, XCHAR p);
    XCHAR* strprevchr(XCHAR* start, XCHAR* s, XCHAR p);
    XCHAR* str_chr(XCHAR* s, XCHAR p);
    int   str_count(KXCHAR* s, XCHAR p);
    void   trimcrlf(XCHAR* s);
    XCHAR* strnchr(XCHAR* s, XCHAR p, int pos);
    XCHAR* strcpy(XCHAR* d, XCHAR* s, int destsz=0);
    KXCHAR* strccpy(XCHAR* d, KXCHAR* s, XCHAR occ);
    XCHAR* strscpy(XCHAR* d, XCHAR* s, KXCHAR* occ);
    XCHAR* strskipalnum(XCHAR *s) ;
    BOOL  strcat(XCHAR* d, XCHAR* s, int szl);
    void  keep_alphanum(XCHAR* p);
    void  format_ascii(XCHAR* p);
    XCHAR* trimfront(XCHAR* p);
    void  trimall(XCHAR* p, XCHAR c);
    void  trimright(XCHAR* p, XCHAR c);
    void  trimnonsql(XCHAR* p);
    void  trimeol(XCHAR* p);
    void   surefilename(XCHAR* file);
    void strupper(XCHAR* a);
    int  str_findlast(KXCHAR* a, XCHAR chr);
	void replace_urlfmt(XCHAR*);
    
};


#endif //
