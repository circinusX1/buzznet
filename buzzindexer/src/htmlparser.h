#ifndef __MARKUP
#define __MARKUP

//| 
//| simple and fast XML/HTML scanner/tokenizer
//|
//| (C) Andrew Fedoniouk @ terrainformatica.com
//|
#include "../../src/core.h"

struct str_istream
{
  XCHAR* iniP;
  XCHAR* p;
  const XCHAR* end;
  size_t   _length;

  str_istream(const XCHAR* src)
  {
      FILE* pf = fopen(src,"rb");
      if(pf)
      {
          fseek(pf,0, SEEK_END);
          unsigned long chars = ftell(pf);
          p = new XCHAR[chars+32];
          fseek(pf,0, SEEK_SET);
          size_t rd = fread((void*)p,1,chars,pf);
          p[rd] = 0;
          _length = rd;
          iniP  = p;
          end   = p + strlen(p);
          fclose(pf);
      }
      else
      {
          p   = 0;
          iniP = 0;
          end = 0;
      }
  }

  str_istream(XCHAR* src, int length)
  {
      p   = src;
      end = src + length;
  }

  ~str_istream(){

      delete []iniP;
  }
  //str_istream(const XCHAR* src): p(src), end(src + strlen(src)) {}

  virtual XCHAR get_char() 
  { 
      return p < end? *p++: 0; 
  }
};

class scanner
{
    public:
    enum token_type 
    {
        TT_ERROR = -1,
        TT_EOF = 0,

        TT_TAG_START,   // <tag ...
                        //     ^-- happens here
        TT_TAG_END,     // </tag>
                        //       ^-- happens here 
                        // <tag ... />
                        //            ^-- or here 
        TT_ATTR,        // <tag attr="value" >      
                        //                  ^-- happens here   
        TT_WORD,
        TT_SPACE,

        TT_DATA,        // content of followings:
        TT_COMMENT_START, TT_COMMENT_END, // after "<!--" and "-->"
        TT_CDATA_START, TT_CDATA_END,     // after "<![CDATA[" and "]]>"
        TT_PI_START, TT_PI_END,           // after "<?" and "?>"
        TT_ENTITY_START, TT_ENTITY_END,   // after "<!ENTITY" and ">"
        
    };

    enum $ { MAX_TOKEN_SIZE = 1024, MAX_NAME_SIZE = 1024 };

    public:

    scanner(str_istream& is): 
        input(is), 
        input_char(0), 
        value_length(0), 
        tag_name_length(0), 
        attr_name_length(0),
	    got_tail(false) 
        { 
            c_scan = &scanner::scan_body; 
        }

    // get next token
    token_type      get_token() { return (this->*c_scan)(); } 

    // get value of TT_WORD, TT_SPACE, TT_ATTR and TT_DATA
    KXCHAR*    get_value();
        
    // get attribute name
    KXCHAR*     get_attr_name();

    // get tag name
    KXCHAR*     get_tag_name();

    // should be overrided to resolve entities, e.g. &nbsp;
    virtual XCHAR   resolve_entity(KXCHAR* buf, int buf_size) { return 0; }
        
    private: /* methods */

    typedef token_type (scanner::*scan)();
    scan        c_scan; // current 'reader'

    // content 'readers'
    token_type  scan_body();
    token_type  scan_head();
    token_type  scan_comment();
    token_type  scan_cdata();
    token_type  scan_pi();
    token_type  scan_tag();
    token_type  scan_entity_decl();

    XCHAR       skip_whitespace();
    void        push_back(XCHAR c);

    XCHAR       get_char();
    XCHAR       scan_entity();

    bool        is_whitespace(XCHAR c);
        
    void        append_value(XCHAR c);
    void        append_attr_name(XCHAR c);
    void        append_tag_name(XCHAR c);

    private: /* data */

    //enum state { TEXT = 0, MARKUP = 1, COMMENT = 2, CDATA = 3, PI = 4 };
    //state       where;
    str_istream&   input;
    token_type  token;

    XCHAR       value[MAX_TOKEN_SIZE];
    int         value_length;

    XCHAR        tag_name[MAX_NAME_SIZE];
    int         tag_name_length;

    XCHAR        attr_name[MAX_NAME_SIZE];
    int         attr_name_length;


    XCHAR       input_char; 

    bool        got_tail; // aux flag used in scan_comment, etc. 

};

 
#endif
