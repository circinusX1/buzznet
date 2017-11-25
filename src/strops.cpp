#include "strops.h"


namespace so{

    BOOL    strbegin(KXCHAR* content, KXCHAR* strset)
    {
        KXCHAR* pr = ::strstr(content, strset);
        if( pr == (XCHAR*)content)
            return 1;
        XCHAR locoset[128];
        ::strcpy(locoset, (XCHAR*)strset);
        strupper(locoset);
        return ::strstr(content, locoset) == content;
    }

    KXCHAR*   stristr(KXCHAR* content, KXCHAR* strset)
    {
        KXCHAR* ret = ::strstr(content, strset);
        if(ret)
            return ret;
        XCHAR locoset[1024];
        ::strcpy(locoset, (XCHAR*)strset);
        strupper(locoset);
        return ::strstr(content, locoset);
    }

	
    KXCHAR*   strstr(KXCHAR* content, KXCHAR* strset)
    {
        KXCHAR* ret = ::strstr(content, strset);
        if(ret)
            return ret;
        XCHAR locoset[1024];
        ::strcpy(locoset, (XCHAR*)strset);
        strupper(locoset);
        return ::strstr(content, locoset);
    }
    void   trimcrlf(XCHAR* p)
    {
        int   d  = 0;
        XCHAR* pd = p;
        while(*p)
        {
            if(*p!='\r' && *p!='\n')
            {
                pd[d++]=*p;
            }
            ++p;
        }
        pd[d++]=0;
    }

    void strlower(XCHAR* a)
    {
	    XCHAR* pc = a;
        int    diff = 'a'-'A';
		while(*pc)
		{ 
			
			if(*pc>='A'&& *pc<='Z')
				*pc += diff;
			++pc;
		}
    }
	
	void strupper(XCHAR* a)
	{
	    XCHAR* pc = a;
        int diff = 'a'-'A';
		while(*pc)
		{ 
			if(*pc>='a'&& *pc<='z')
				*pc -= diff;
			++pc;
		}
		
	}

    BOOL isdigit(XCHAR c)
    {
        return (c >= '0' && c <= '9');
    }

    XCHAR *strnew(KXCHAR *psz)
    {
        XCHAR *res = new XCHAR[str_len(psz) + sizeof(XCHAR)];
        ::strcpy(res, (XCHAR*)psz);
        return res;
    }
    
    int chartohex (XCHAR c)
    {
        if (c >= '0' && c <= '9')
            return (c - '0');
        else if (c >= 'a' && c <= 'f')
            return (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F')
            return (c - 'A' + 10);
        else
            return -1;
    }


    XCHAR* strlastchr(XCHAR* s, XCHAR p)
    {
        XCHAR* pdot = s;
        XCHAR* pdp = pdot;
        while(1){
            pdot = pdp;
            pdp = strchr(pdot,p);
            if(pdp)
                ++pdp;
            else 
                break;
        }
        return pdot-1;
    }

    XCHAR* strprevchr(XCHAR* start, XCHAR* s, XCHAR p)
    {
        XCHAR* cp = s;
        while(cp!=start && *cp != p)--cp;
        return cp;
    }

    XCHAR* str_chr(XCHAR* s, XCHAR p)
    {
        while(*s && *s!=p)++s;
        return s;
    }

    int   str_count(KXCHAR* s, XCHAR cc)
    {
        int c = 0;
        KXCHAR* p = s;
        while(*p)if(*p++==cc)++c;
        return c;
    }

    XCHAR* strnchr(XCHAR* s, XCHAR p, int pos)
    {
        XCHAR* pdot = s;
        XCHAR* pdp = pdot;
        while(1){
            pdot = pdp;
            pdp = strchr(pdot,p);
            if(pdp && --pos)
                ++pdp;
            else
                break;
        }
        return pdp ? pdp : pdot-1;
    }

    KXCHAR* strccpy(XCHAR* d, KXCHAR* s, XCHAR occ)
    {
        if(s==0)return 0;
        while(*s && *s!=occ)
            *d++=*s++;
        *d = 0;
        return s;
    }

    XCHAR* strscpy(XCHAR* d, XCHAR* s, KXCHAR* occ)
    {
        if(s==0)return 0;
        while(*s && !strchr(occ,*s))
            *d++=*s++;
        *d=0;
        return s;
    }

    BOOL is_alnum(KXCHAR c)
    {
        return  (c>='0' && c<='9') ||  (c<='z' && c>='A');
    }

    XCHAR* strskipalnum(XCHAR *s)
    {
        while(*s && *s>0 && is_alnum(*s))++s;
        return s;
    }

    BOOL  strcat(XCHAR* d, XCHAR*s, int szl)
    {
        if((str_len(d) + str_len(s)) < (size_t)(szl-1))
        {
            ::strcat(d,s);
            return 1;
        }
        return 0;
    }

    void  keep_alphanum(XCHAR* p)
    {
        int   d  = 0;
        XCHAR* pd = p;
        while(*p)
        {
            if(is_alnum(*p))
            {
                pd[d++]=*p;
            }
            ++p;
        }
        pd[d++]=0;
    }
	
	void replace_urlfmt(XCHAR* p)
	{
	    while(*p)
        {
            if(*p=='%')
				*p='_';
            ++p;
        }
        *p=0;	
	}

    void  format_ascii(XCHAR* p){
        while(*p)
        {
            if(*p=='/')
                *p='(';
			else if(*p=='\\')
				*p=')';
            else if(*p=='?')
                *p='#';
            else if(*p=='*')
                *p='^';
            else if(*p=='&')
                *p=',';
            else if(*p=='\"')
                *p='@';
            ++p;
        }
        *p=0;
    }

    XCHAR* trimfront(XCHAR* p)
    {
        while(*p && (*p==' ' || *p=='\t'))
            ++p;
        return p;
    }

    void  trimright(XCHAR* p, XCHAR c)
    {
        XCHAR* plast = p+str_len(p)-1;
        while(plast!=p && *plast==c)
            --plast;
        *(plast+1)=0;
    }

    void  trimeol(XCHAR* p)
    {
        XCHAR* plast = p+str_len(p)-1;
        while(plast!=p && (*plast=='\r' || *plast=='\n'))
            --plast;
        *(plast+1)=0;
    }


    void  trimnonsql(XCHAR* p)
    {
        XCHAR *d=p;

        while(*p)
        {
            if(*p==',')
                *d++=' ';
            else if((*p >= 'a' && *p <= 'z') || (*p >='A' && *p<= 'Z'))
            {
                *d++=*p;
            }
            ++p;
        }
        *d=0;
    }

    void  trimall(XCHAR* p, XCHAR c)
    {
        XCHAR *d=p;
        while(*p)
        {
            if(*p!=c)
            {
                *d++=*p;
            }
            ++p;
        }
        *d=0;
    }


    BOOL strend(KXCHAR* hay, KXCHAR* ned)
    {
        size_t hl = str_len(hay);
        size_t ed = str_len(ned);
        if(hl > ed)
        {
            hay += (hl-ed);
            return !::strcmp(hay, ned);
        }
        return 0;
    }

    int  str_findlast(KXCHAR* a, XCHAR chr)
    {
        int pla = (int)str_len(a);
        while(--pla>-1)
        {
            if(a[pla]==chr)
                return (int)pla;
        }
        return 0;
    }
	
	
	int strcount(const XCHAR* s, XCHAR p)
	{
		int cnt=0;
		const XCHAR* ps =s;
		while(*ps)
		{  
			if(*ps==p) ++cnt;
			++ps;
		}
		return cnt;
	}

    
    
    void to_db_filename(XCHAR* psz)
    {
        XCHAR      loco[800] = {0};
        XCHAR* s = psz;
        XCHAR* d = loco;
        while(*s)
        {
            //~!@#$%^&()_+=[]-
            switch(*s)
            {
                case '\r':
                case '\n':
                    ++s;
                    continue;
                    break;
                case '*' :
                    *d++='!';
                    *s++;
                    continue;
                case '?' :
                    *d++='#';
                    *s++;
                    break;
                case '\'':
                    *d++='$';
                    *s++;
                    break;
                case '`' :
                    *d++='@';
                    *s++;
                    break;
                case '\"':
                    *d++='@';
                    *s++;
                    break;
                case '%' :
                    *s++;
                    if(*s=='2')
                        *s++;
                    if(*s=='0')
                        *s++;
                    *d++=' ';
                    break;
                default:
                    *d++=*s++;
            }
        }
        *d = 0;
        ::strcpy(psz, loco);
    }

    void from_db_filename(XCHAR* psz)
    {
      
    }
};
