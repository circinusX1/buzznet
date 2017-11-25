

#include "htmlparser.h"


// case sensitive string equality test
// s_lowcase shall be lowercase string
INLN bool equal(KXCHAR* s, KXCHAR* s1, size_t length)
{
    switch(length)
    {
    case 8: if(s1[7] != s[7]) return false;
    case 7: if(s1[6] != s[6]) return false;
    case 6: if(s1[5] != s[5]) return false;
    case 5: if(s1[4] != s[4]) return false;
    case 4: if(s1[3] != s[3]) return false;
    case 3: if(s1[2] != s[2]) return false;
    case 2: if(s1[1] != s[1]) return false;
    case 1: if(s1[0] != s[0]) return false;
    case 0: return true;
    default: return strncmp(s,s1,length) == 0;
    }
}

KXCHAR* scanner::get_value() 
{
    value[value_length] = 0;
    return value;
}

KXCHAR* scanner::get_attr_name() 
{
    attr_name[attr_name_length] = 0;
    return attr_name;
}

KXCHAR* scanner::get_tag_name() 
{
    tag_name[tag_name_length] = 0;
    return tag_name;
}

scanner::token_type scanner::scan_body() 
{
    XCHAR c = get_char();

    value_length = 0;

    bool ws = false;

    if(c == 0) return TT_EOF;
    else if(c == '<') return scan_tag();
    else if(c == '&')
        c = scan_entity();
    else
        ws = is_whitespace(c);

    while(true) 
    {
        append_value(c);
        c = input.get_char();
        if(c == 0)  { push_back(c); break; }
        if(c == '<') { push_back(c); break; }
        if(c == '&') { push_back(c); break; }

        if(is_whitespace(c) != ws) 
        {
            push_back(c);
            break;
        }

    }
    return ws? TT_SPACE:TT_WORD;
}

scanner::token_type scanner::scan_head()
{
    XCHAR c = skip_whitespace();

    if(c == '>') { c_scan = &scanner::scan_body; return scan_body(); }
    if(c == '/')
    {
        XCHAR t = get_char();
        if(t == '>')   { c_scan = &scanner::scan_body; return TT_TAG_END; }
        else { push_back(t); return TT_ERROR; } // erroneous situtation - standalone '/'
    }

    attr_name_length = 0;
    value_length = 0;

    // attribute name...
    while(c != '=') 
    {
        if( c == 0) return TT_EOF;
        if( c == '>' ) { push_back(c); return TT_ATTR; } // attribute without value (HTML style)
        if( is_whitespace(c) )
        {
            c = skip_whitespace();
            if(c != '=') { push_back(c); return TT_ATTR; } // attribute without value (HTML style)
            else break;
        }
        if( c == '<') return TT_ERROR;
        append_attr_name(c);
        c = get_char();
    }

    c = skip_whitespace();
    // attribute value...

    if(c == '\"')
        while(c = get_char())
        {
            if(c == '\"') return TT_ATTR;
            if(c == '&') c = scan_entity();
            append_value(c);
        }
    else if(c == '\'') // allowed in html
        while(c = get_char())
        {
            if(c == '\'') return TT_ATTR;
            if(c == '&') c = scan_entity();
            append_value(c);
        }
    else  // scan token, allowed in html: e.g. align=center
        do
        {
            if( is_whitespace(c) ) return TT_ATTR;
            if( c == '/' || c == '>' ) { push_back(c); return TT_ATTR; }
            if( c == '&' ) c = scan_entity();
            append_value(c);
        } while(c = get_char());

        return TT_ERROR;
}

// caller already consumed '<'
// scan header start or tag tail
scanner::token_type scanner::scan_tag() 
{
    tag_name_length = 0;

    XCHAR c = get_char();

    bool is_tail = c == '/';
    if(is_tail) c = get_char();

    while(c) 
    {
        if(is_whitespace(c)) { c = skip_whitespace(); break; }
        if(c == '/' || c == '>') break;
        append_tag_name(c);

        switch(tag_name_length)
        {
        case 3: 
            if(equal(tag_name,"!--",3))  { c_scan = &scanner::scan_comment; return TT_COMMENT_START; }
            break;
        case 8:
            if( equal(tag_name,"![CDATA[",8) ) { c_scan = &scanner::scan_cdata; return TT_CDATA_START; }
            break;
        case 7:
            if( equal(tag_name,"!ENTITY",8) ) { c_scan = &scanner::scan_entity_decl; return TT_ENTITY_START; }
            break;
        }

        c = get_char();
    }

    if(c == 0) return TT_ERROR;    

    if(is_tail)
    {
        if(c == '>') return TT_TAG_END;
        return TT_ERROR;
    }
    else 
        push_back(c);

    c_scan = &scanner::scan_head;
    return TT_TAG_START;
}

// skip whitespaces.
// returns first non-whitespace XCHAR
XCHAR scanner::skip_whitespace() 
{
    while(XCHAR c = get_char()) 
    {
        if(!is_whitespace(c)) return c;
    }
    return 0;
}

void    scanner::push_back(XCHAR c) { input_char = c; }

XCHAR scanner::get_char() 
{ 
    if(input_char) { XCHAR t(input_char); input_char = 0; return t; }
    return input.get_char();
}


// caller consumed '&'
XCHAR scanner::scan_entity() 
{
    XCHAR buf[32];
    int i = 0;
    XCHAR t;
    for(; i < 31 ; ++i )
    {
        t = get_char();
        if(t == 0) return TT_EOF;
        buf[i] = XCHAR(t); 
        if(t == ';')
            break;
    }
    buf[i] = 0;
    if(i == 2)  
    {
        if(equal(buf,"gt",2)) return '>';
        if(equal(buf,"lt",2)) return '<';
    }
    else if(i == 3 && equal(buf,"amp",3)) 
        return '&';
    else if(i == 4) 
    {
        if(equal(buf,"apos",4)) return '\'';
        if(equal(buf,"quot",4)) return '\"';
    }
    t = resolve_entity(buf,i);
    if(t) return t;
    // no luck ...
    append_value('&');
    for(int n = 0; n < i; ++n)
        append_value(buf[n]);
    return ';';
}

bool scanner::is_whitespace(XCHAR c)
{
    return c <= ' ' 
        && (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f');
}

void scanner::append_value(XCHAR c) 
{ 
    if(value_length < (MAX_TOKEN_SIZE - 1)) 
        value[value_length++] = c;
}

void scanner::append_attr_name(XCHAR c)
{
    if(attr_name_length < (MAX_NAME_SIZE - 1)) 
        attr_name[attr_name_length++] = XCHAR(c);
}

void scanner::append_tag_name(XCHAR c)
{
    if(tag_name_length < (MAX_NAME_SIZE - 1)) 
        tag_name[tag_name_length++] = XCHAR(c);
}

scanner::token_type scanner::scan_comment()
{
    if(got_tail)
    {
        c_scan = &scanner::scan_body;
        got_tail = false;
        return TT_COMMENT_END;
    }
    for(value_length = 0; value_length < (MAX_TOKEN_SIZE - 1); ++value_length)
    {
        XCHAR c = get_char();
        if( c == 0) return TT_EOF;
        value[value_length] = c;

        if(value_length >= 2 
            && value[value_length] == '>' 
            && value[value_length - 1] == '-' 
            && value[value_length - 2] == '-')
        {
            got_tail = true;
            value_length -= 2;
            break;
        }
    }
    return TT_DATA;
}

scanner::token_type scanner::scan_cdata()
{
    if(got_tail)
    {
        c_scan = &scanner::scan_body;
        got_tail = false;
        return TT_CDATA_END;
    }
    for(value_length = 0; value_length < (MAX_TOKEN_SIZE - 1); ++value_length)
    {
        XCHAR c = get_char();
        if( c == 0) return TT_EOF;
        value[value_length] = c;

        if(value_length >= 2 
            && value[value_length] == '>' 
            && value[value_length - 1] == ']' 
            && value[value_length - 2] == ']')
        {
            got_tail = true;
            value_length -= 2;
            break;
        }
    }
    return TT_DATA;
}

scanner::token_type scanner::scan_pi()
{
    if(got_tail)
    {
        c_scan = &scanner::scan_body;
        got_tail = false;
        return TT_PI_END;
    }
    for(value_length = 0; value_length < (MAX_TOKEN_SIZE - 1); ++value_length)
    {
        XCHAR c = get_char();
        if( c == 0) return TT_EOF;
        value[value_length] = c;

        if(value_length >= 1 
            && value[value_length] == '>' 
            && value[value_length - 1] == '?')
        {
            got_tail = true;
            value_length -= 1;
            break;
        }
    }
    return TT_DATA;
}

scanner::token_type scanner::scan_entity_decl()
{
    if(got_tail)
    {
        c_scan = &scanner::scan_body;
        got_tail = false;
        return TT_ENTITY_END;
    }
    XCHAR t;
    unsigned int tc = 0;
    for(value_length = 0; value_length < (MAX_TOKEN_SIZE - 1); ++value_length)
    {
        t = get_char();
        if( t == 0 ) return TT_EOF;
        value[value_length] = t;
        if(t == '\"') tc++;
        else if( t == '>' && (tc & 1) == 0 )
        {
            got_tail = true;
            break;
        }
    }
    return TT_DATA;
}

