#ifndef _MYSQL_H_
#define _MYSQL_H_

#include "core.h"
#ifdef _WIN32
    #include "../dbase/mysql/mysql.h"
#else
    #include <mysql/mysql.h>
#endif //


//---------------------------------------------------------------------------------------
class MySQL
{
public:
     MySQL()
     {
          queryResult_ptr = 0;
     }


    BOOL     Connect(const XCHAR* hostname, const XCHAR* username, const XCHAR* password)
    {
        mysql_init(&_mysql);

        mysql_options(&_mysql,MYSQL_READ_DEFAULT_GROUP,"WebPoof");
        mysql_options(&_mysql,MYSQL_OPT_COMPRESS,0);
        if (!mysql_real_connect(&_mysql,hostname,username,password,"",0,NULL,0))
        {
            Error();
            return 0;
        }
        return 1;

    }
    void    Disconnect()
    {
        FreeRez();
        mysql_close(&_mysql);
    }

    void Error()
    {
        printf("ERROR: (%s)\r\n",mysql_error(&_mysql));
#ifdef _WIN32
        OutputDebugString(_F("%s\r\n",mysql_error(&_mysql)));
#endif//
#ifdef _DEBUG
        printf("%s\r\n",_lastq.c_str());
#ifdef _WIN32
        OutputDebugString(_F("%s\r\n",_lastq.c_str()));
#endif//
#endif //_DEBUG

    }

    my_ulonglong Qry(const XCHAR* q)
    {
#ifdef _DEBUG
        _lastq = q;
#endif //_DEBUG
        FreeRez();
        int r =  mysql_query(&_mysql, q);
        if(0==r)
        {
             queryResult_ptr = mysql_store_result(&_mysql);
        }
        else
            Error();
        if(!strncmp(q,"SEL",3))
        {
            return GetRowCount();
        }
        return r==0;
    }

    DWORD GetRowCount()
    {
        if(queryResult_ptr)
            return (DWORD)mysql_num_rows(queryResult_ptr);
        return 0;
    }

    void Select (const XCHAR* s)
    {
#ifdef _DEBUG
        _lastq = s;
#endif //_DEBUG

        this->FreeRez();
        mysql_select_db(&_mysql,s);
    }

    void FreeRez(){
        if(queryResult_ptr)
            mysql_free_result(queryResult_ptr);
        queryResult_ptr = 0;
    }

    MYSQL_ROW Fetch()
    {
        if(queryResult_ptr)
            return mysql_fetch_row(queryResult_ptr);
        return 0;
    }
    
    INLN my_ulonglong LastID(){return mysql_insert_id(&_mysql);}


    XCHAR* Fetch(int index)
    {
        if(queryResult_ptr)
            return mysql_fetch_row(queryResult_ptr)[index];
        return 0;
    }

    long FetchLong(int index)
    {
        if(queryResult_ptr)
            return ::atol(mysql_fetch_row(queryResult_ptr)[index]);
        return -1;
    }

public:
    MYSQL       _mysql;
    MYSQL_RES   *queryResult_ptr;
#ifdef _DEBUG
    string      _lastq;
#endif 
    
};

#endif //
