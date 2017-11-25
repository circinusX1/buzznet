// dbase.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include <string>
#include <vector>
#include "dbaseimpl.h"
using namespace std;

static XCHAR __wk_flds[]= " (wk_service,wk_ip,wk_pip,wk_port,wk_server,wk_page,wk_params,wk_robots,wk_recicled,wk_depth,wk_sparent,wk_hash,wk_timestamp,wk_delayed) ";
static int err;

#define MAX_SITES   2

void  Strimall(XCHAR* p, XCHAR c)
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
XCHAR* Strscpy(XCHAR* d, XCHAR* s, KXCHAR* occ)
{
    while(*s && !strchr(occ,*s))
        *d++=*s++;
    *d=0;
    return s;
}

BOOL Is_alnum(KXCHAR c)
{
    return  (c>='0' && c<='9') ||  (c<'z' && c>='A');
}



DbRepository::DbRepository()
{
   
}

DbRepository::~DbRepository()
{
    _sql.Disconnect();
}


BOOL    DbRepository::Connect(const XCHAR* dl, PfnNewUrl pFn, const XCHAR* host, const XCHAR* dbase, const XCHAR* user, const XCHAR* password)
{
    _destlocation = dl;
    _New = pFn;
    if(!_sql.Connect(host, user, password))
    {
        _sql.Disconnect();
        return 0;
    }
    _SetupDatabases();
    return 1;
}

BOOL    DbRepository::SavePageContent(KXCHAR* pageName, 
                                      const XCHAR* page, 
                                      size_t length, 
                                      const SpUrl& url, 
                                      KXCHAR* szMeta, 
                                      KXCHAR* szWords)
{

    if(page == 0)
        return FALSE;
    size_t  memlen = length;
    const   XCHAR* afterheader = ::strstr(page,"\r\n\r\n");

    if(!afterheader)
        afterheader = page;
    else
    {
        afterheader+=4;
        memlen-=(size_t)(afterheader-page);
    }
    if(memlen > 32)
    {
        XCHAR   obj[400];
        XCHAR   file[512];
    
        url->GetObjectFilename(obj);
        this->_GetURLFileSaveName(url, obj, file,1);
        BOOL saved = 0;
        FILE* pf = f_open(file,"wb");
        if(pf)
        {
            fwrite(afterheader, memlen, 1, pf);
            fclose(pf);
            saved = 1;
        }
        if(saved)
        {
            if(strstr(file,"index") || strstr(file,"#"))       
            {            
                this->_GetURLFileSaveName(url, "/#meta.html", file,1);
                pf = f_open(file,"ab");
                if(pf)
                {
                    fwrite(szMeta, strlen(szMeta), 1, pf);
                    fclose(pf);
                }
                this->_GetURLFileSaveName(url, "/#sumar.html", file,1);
                pf = f_open(file,"ab");
                if(pf)
                {
                    fwrite(szWords, strlen(szWords), 1, pf);
                    fclose(pf);
                }
            }
            long sid = url->GetParentID();
            //my_ulonglong siteId = _sql.Qry(_F("SELECT ",   url.GetServerName())
            

            


        }
    }
    return 1;         //by ip and syte    
}

BOOL    DbRepository::PageExist(const SpUrl& url, XCHAR* page, size_t length)
{
    if(_sql.Qry(_F("SELECT wk_id FROM wk_queue WHERE wk_hash='%s' LIMIT 1;",url->GetHashKey())))
    {
        if(_sql.FetchLong(0)!=-1)
            return 1;
    }

    for(int i=0;i<MAX_SITES;i++)
    {
        if(_sql.Qry(_F("SELECT id FROM sites%d WHERE name='%s' LIMIT 1;",i,url->GetServerName())))
        {
            if(_sql.Qry(_F("SELECT id FROM docs%d WHERE filename='%s' AND siteid=%d LIMIT 1;",
                           i,url->GetObjectName(), _sql.FetchLong(0))))
            {
                return 1;
            }
        }
    }

    return 0;
}

BOOL    DbRepository::SaveRobotsContent(XCHAR* page, size_t length, const SpUrl& url)
{
    BOOL    allow = 1;
    XCHAR   fileName[400];
    
    this->_GetURLFileSaveName(url, "robots.txt", fileName,1);
    os::H_FILE* pf = f_open(fileName, "wb");
    if(pf)
    {
        if(length)
        {
            BOOL   start = 0;
            XCHAR* line  = t_strtok(page,"\n");
            while(line)
            {
                if(line[0]=='#')
                {
                    line = t_strtok(0,"\n");
                    continue;
                }
                ::Strimall(line,' ');
                ::Strimall(line,'\t');
                ::Strimall(line,'\r');
                ::Strimall(line,'\n');

                if(strstr(line,"User-agent:*") || strstr(line,"User-agent:BuzzNet") && ! start)
                {
                    start = 1;
                    line = t_strtok(0,"\n");
                    continue;
                }
                else if(strstr(line,"User-agent:") && start)
                {
                    start = 0;
                    line = t_strtok(0,"\n");
                    continue;
                }

                if(start)
                {
                    if(!strcmp(line,"Disallow:/"))
                    {
                        //PRINTF(_D,"Disalow *: %s %s\r\n",line,url->GetServerName());
                        allow = 0;
                        break;                 //all fobidden
                    }

                    if(strstr(line,"Disallow:"))
                    {
                        XCHAR* pd = strstr(line,"Disallow:");
                        if(pd)
                        {
                            XCHAR loco[128];
                            Strscpy(loco, pd+9,"\r\n");
                            strcat(loco,"\r\n");
                            f_puts(loco, pf);
                        }
                    }
                }
                line = t_strtok(0,"\n");
            }
        }
        else
            fputs("#\r\n",pf);
        f_close(pf);


        XCHAR  loco[32];

        t_sprintf(loco,"/",url->GetIP());

        this->_GetURLFileSaveName(url, loco, fileName,1);
        os::H_FILE* pf = f_open(_F("%s%s",fileName,url->GetIP()), "wb");
        if(pf)
        {
            f_puts(url->GetIP(),pf);
            f_close(pf);
        }

        pf = f_open(_F("%s#hits.txt",fileName), "wb");
        if(pf)
        {
			unsigned long zero=0;
            f_write(&zero,1,sizeof(unsigned long),pf);
            f_close(pf);
        }

        long    lastSite=0;
        if(_sql.Qry("SELECT siteid FROM last_ids;"))
        {
            lastSite = _sql.FetchLong(0);
        }
        if(_sql.Qry(_F("SELECT tblname FROM site_disp WHERE min<=%d AND max>=%d ;", lastSite,lastSite)))
        {
            
            _sql.Qry(_F("INSERT INTO %s VALUES(NULL,'%s','%s','%s','%s',%d,%d,NOW(),0,0,%d,0,1);", 
                                                    _sql.Fetch(0),
                                                    url->GetIP(),
                                                    url->GetServerName(),
                                                    url->GetService(),
                                                    "**",
                                                    url->GetPort(),
                                                    0,
                                                    url->GetDepth()));
        }
    }
    return allow;
}

BOOL    DbRepository::RobotsAllow(const SpUrl& refUrl2)
{
    return 1;
}

void    DbRepository::OnPageTimeOut(SpUrl& refUrl2)
{
}

void    DbRepository::OnHeaderError(const XCHAR* page, SpUrl& url)
{
}

void    DbRepository::OnConnectError(SpUrl& url)
{
}

void    DbRepository::SaveCurentState( XCHAR* filename,  int removeelems, BOOL final)
{
}

BOOL    DbRepository::RetrievePreviosState( XCHAR* filename,BOOL firsttime)
{
    return 0;        
}

int     DbRepository::PutInWorkingQueue(const SpUrl& url, const Iurl* referer, BOOL again, int where)
{
    //check if we have it
    if(!PageExist(url))
    {
        Cfm f("INSERT INTO wk_queue VALUES (NULL, %d, '%s',%d, '%s','%s','%s','%s', %d, %d, %d,'%s', '%s', NOW(), %d);",
                referer ? referer->GetID() : -1,
                url->GetService(),
                url->GetPort(),
                url->GetIP(),
                url->GetServerName(),
                url->GetObjectName(), 
                url->GetParams(),
                url->HasRobots(),
                url->GetRecicleCunter(),
                url->GetDepth(),
                url->GetParentServerName(),
                url->GetHashKey(),
                0);
        if(_sql.Qry(f))
        {
            unsigned long id = (unsigned long)mysql_insert_id(&_sql._mysql);
            url->SetID(id);
        }
    }else if(again)
    {
        _sql.Qry(_F("UPDATE wk_queue SET wk_delayed=0, wk_timestamp=NOW(), wk_robots=%d WHERE wk_id=%d;", url->GetID(), url->HasRobots())) ;

    }
    return 1;
}


void    DbRepository::DeleteFromWorkingQueue(KSpUrl& url, BOOL fromq)
{
    Cfm f("DELETE FROM wk_queue WHERE wk_id=%d;",url->GetID());
    _sql.Qry(f);
}


int     DbRepository::GetFromWorkingQueue(SpUrl* url)
{
    long elems = 0;
    Cfm f("SELECT * FROM wk_queue WHERE wk_delayed=0 LIMIT 1;");
    if(elems=_sql.Qry(f))
    {
        SpUrl           spuParent;
        XCHAR           pageLoc[512];
        MYSQL_ROW       r = _sql.Fetch();

        int             depth = ::atoi(r[9]);
        long            pid   = ::atoi(r[1]);
        if(::atoi(r[3])!=80)
            sprintf(pageLoc,"%s%s:%s%s%s", r[2], r[5], r[3],r[6],r[7]);
        else
            sprintf(pageLoc,"%s%s%s%s", r[2], r[5],r[6],r[7]);

        int ivals[5];
        int id   = ::atoi(r[0]);
        ivals[0] = ::atoi(r[8]);
        ivals[1] = ::atoi(r[0]);
        ivals[2] = ::atoi(r[2]);
        ivals[3] = ::atoi(r[10]);
        ivals[4] = ::atoi(r[9]);

        Cfm f2("SELECT * FROM wk_queue WHERE wk_pid=%d;",id);        

        // see if it has a parent. select parent by pid
        if(_sql.Qry(f2))
        {
            XCHAR   pageLoc[512];

            if(::atoi(r[3])!=80)
                sprintf(pageLoc,"%s%s:%s%s%s", r[2], r[5], r[3],r[6],r[7]);
            else
                sprintf(pageLoc,"%s%s:%s%s", r[2], r[5],r[6],r[7]);

            _New(pageLoc, 0, 0, &spuParent); 
        }
        _New(pageLoc, depth, spuParent.get(), url);
        
        //(*url).addref();
        (*url)->SetRobots(ivals[0]);
        (*url)->SetID(ivals[1]);
        (*url)->SetParentID(ivals[2]);
        (*url)->SetDepth(ivals[3]);
        (*url)->SetRecicled(ivals[4]);


        Cfm f3("UPDATE wk_queue SET wk_delayed=1 WHERE wk_id=%d;",id);  
        _sql.Qry(f3);

    }
    return --elems;
}

void    DbRepository::HitSite(KSpUrl& url)
{
}

void    DbRepository::AddToCurentState(XCHAR *,const KXCHAR *)
{

}


BOOL    DbRepository::ReputInWorkingQueue(SpUrl& refUrl, const Iurl* referer, int where)
{
    return 1;
}


BOOL    DbRepository::HasRobots(const SpUrl& url)
{
    if(_sql.Qry(_F("SELECT wk_robots FROM wk_queue WHERE wk_hash='%s' AND wk_robots=1;",url->GetHashKey())))    
        return 1;

    for(int i=0;i<MAX_SITES;i++)
    {

        if(_sql.Qry(_F("SELECT robo FROM sites%d WHERE name='%s' LIMIT 1;",i,url->GetServerName())))
        {
            return 1;
        }
    }
    return 0;
}

void    DbRepository::UpdatePagesReportList(const XCHAR* urlName, XCHAR* fileName)
{

}

int     DbRepository::GetWorkingQueueLoad()
{
    return 1;
}

int     DbRepository::_SetupDatabases()
{
    _sql.Qry("USE buzznet;");
    //if(0==_sql.Qry("SELECT * FROM _countries;"))
    {
        _CreateTables();
    } 
    return 1;
}



int     DbRepository::_CreateTables()
{
    TCHAR ld[260];

    getcwd(ld,260);
    TCHAR* ps = ld;
    while(*ps){if(*ps=='\\')*ps='/';++ps;}
    
    //-------------------------------------------------------------------------
    // COUNTRIES
    _sql.Qry("DROP TABLE IF EXISTS `_countries`;");
    _sql.Qry( "CREATE TABLE `_countries` (`ISO_Code` varchar(2) NOT NULL default '',"
              "`Country` varchar(255) NOT NULL default '',"
              "`Region` varchar(255) NOT NULL default '',"
              "`RegionCode` varchar(4) NOT NULL default '',"
              "`Capital` varchar(255) NOT NULL default '',"
              "`Currency` varchar(255) NOT NULL default '',"
              "PRIMARY KEY  (`ISO_Code`)"
              ") TYPE=MyISAM;");
   
    string s = _F("LOAD DATA INFILE '%s/db_data_init/cntrys.dat' INTO TABLE _countries FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '''' LINES TERMINATED BY '\r\n'",ld);
    _sql.Qry(s.c_str());

    //-------------------------------------------------------------------------
    // COUNTRY IP
    _sql.Qry("DROP TABLE IF EXISTS anp_ip2country;");
    _sql.Qry("CREATE TABLE anp_ip2country ("
                "ip_from bigint(10) NOT NULL default '0',"
                "ip_to bigint(10) NOT NULL default '0',"
                "iso_code char(4) NOT NULL default '',"
                "PRIMARY KEY  (ip_from,ip_to),"
                "UNIQUE KEY k_ipfrom (ip_from,ip_to)"
                ") TYPE=MyISAM;" );

    _sql.Qry(_F("LOAD DATA INFILE '%s/db_data_init/cntry_ips.dat' INTO TABLE anp_ip2country FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '''' LINES TERMINATED BY '\r\n'",ld) );
        
    //-------------------------------------------------------------------------
    _sql.Qry("DROP TABLE IF EXISTS `wk_queue`;");
    _sql.Qry("CREATE TABLE `wk_queue` ("
                "`wk_id` bigint(11) NOT NULL auto_increment,"
                "`wk_pid` bigint(20) NOT NULL default '0',"
                "`wk_service` varchar(32) NOT NULL default '',"
                "`wk_port` int(11) NOT NULL default '0',"
                "`wk_ip` varchar(40) NOT NULL default '',"
                "`wk_server` varchar(128) NOT NULL default '',"
                "`wk_page` varchar(255) NOT NULL default '',"
                "`wk_params` varchar(255) NOT NULL default '',"
                "`wk_robots` int(11) NOT NULL default '0',"
                "`wk_recicled` int(11) NOT NULL default '0',"
                "`wk_depth` int(11) NOT NULL default '0',"
                "`wk_sparent` varchar(100) NOT NULL default '',"
                "`wk_hash` varchar(254) NOT NULL default '',"
                "`wk_timestamp` timestamp NOT NULL,"
                "`wk_delayed` int(11) NOT NULL default '0',"
                "PRIMARY KEY  (`wk_id`,`wk_hash`)"
                ") TYPE=MyISAM;");

    //--------------------------------------------------------------------------
    _sql.Qry("DROP TABLE IF EXISTS `search0`;");

    _sql.Qry("CREATE TABLE `search0` ("
            "`id` bigint(20) NOT NULL auto_increment,"
            "`page_id` bigint(20) NOT NULL default '0',"
            "`kword_id` bigint(20) NOT NULL default '0',"
            "`weight` int(11) NOT NULL default '0',"
            "PRIMARY KEY  (`id`)"
            ") ENGINE=MyISAM; ");

    //-----------------------------------------------------------------------
    _sql.Qry("DROP TABLE IF EXISTS `last_ids`;");
    _sql.Qry("CREATE TABLE `last_ids` ("
            "`siteid` bigint(20) NOT NULL default '0',"
            "`docid` bigint(20) NOT NULL default '0',"
            ") ENGINE=MyISAM; ");


    //-----------------------------------------------------------------------
    _sql.Qry("DROP TABLE IF EXISTS `site_hits`;");
    _sql.Qry("CREATE TABLE `site_hits` ("
             "`wk_hash` varchar(254) NOT NULL default '',"
             "`hits` int(11) NOT NULL default '0',"
             "`id` int(11) NOT NULL auto_increment,"
             "PRIMARY KEY  (`wk_hash`,`id`)"
             ") ENGINE=MyISAM; ");

    //-----------------------------------------------------------------------
    _sql.Qry("DROP TABLE IF EXISTS `doc_disp`;");
    _sql.Qry("CREATE TABLE `doc_disp` ("
            "`tblname` char(12) NOT NULL default '',"
            "`min` bigint(20) NOT NULL default '0',"
            "`max` bigint(20) NOT NULL default '0',"
            "PRIMARY KEY  (`min`,`max`)"
            ") ENGINE=MyISAM; ");

    _sql.Qry("INSERT INTO doc_disp VALUES('docs0',0,1000);");
    _sql.Qry("INSERT INTO doc_disp VALUES('docs1',1001,2000);");

    //-----------------------------------------------------------------------
    _sql.Qry("DROP TABLE IF EXISTS `site_disp`;");
    _sql.Qry("CREATE TABLE `site_disp` ("
            "`tblname` char(12) NOT NULL default '',"
            "`min` bigint(20) NOT NULL default '0',"
            "`max` bigint(20) NOT NULL default '0',"
            "PRIMARY KEY  (`min`,`max`)"
            ") ENGINE=MyISAM; ");

    _sql.Qry("INSERT INTO site_disp VALUES('sites0',0,100);");
    _sql.Qry("INSERT INTO site_disp VALUES('sites1',101,200);");

    //-----------------------------------------------------------------------
    // create a_kwords b_kwords ...z_kwords
    for(int i='a'; i <='a'; i++)
    {
        _sql.Qry(_F("DROP TABLE IF EXISTS `%c_kwords`;",i));
        _sql.Qry(_F("DROP TABLE IF EXISTS `%d_kwords`;",i));

        _sql.Qry(_F("CREATE TABLE `%c_kwords` ("
                "`id` bigint(20) NOT NULL auto_increment,"
                "`firstlet` char(2) NOT NULL default '',"
                "`word` char(32) NOT NULL default '',"
                "`devey` decimal(10,0) NOT NULL default '0',"
                "`unused` int(11) NOT NULL default '0',"
                "PRIMARY KEY  (`id`,`word`)"
                ") ENGINE=MyISAM; ",i));
    }

    //-----------------------------------------------------------------------
    // 
    for(int i=0;i<MAX_SITES;i++)
    {
        _sql.Qry(_F("DROP TABLE IF EXISTS `site%d`;",i));
        _sql.Qry(_F("DROP TABLE IF EXISTS `docs%d`;",i));

        _sql.Qry(_F("CREATE TABLE `sites%d` ("
                "`id` bigint(20) NOT NULL auto_increment,"
                "`ip` varchar(64) NOT NULL default '',"
                "`name` varchar(64) NOT NULL default '',"
                "`service` int(11) NOT NULL default '0',"
                "`ciso` varchar(4) NOT NULL default '', "
                "`port` int(11) NOT NULL default '0',"
                "`priority` int(11) NOT NULL default '0',"
                "`time` timestamp NOT NULL,"
                "`pages` int(11) NOT NULL default '0',"
                "`links` int(11) NOT NULL default '0',"
                "`depth` int(11) NOT NULL default '0',"
                "`hits`  int(11) NOT NULL default '0',"
                "`robo`  int(11) NOT NULL default '0',"
                "PRIMARY KEY  (`id`,`name`)"
                ") ENGINE=MyISAM; ",i));

        _sql.Qry(_F("CREATE TABLE `docs%d` ("
                "`id` bigint(20) NOT NULL auto_increment,"
                "`siteid` bigint(20) NOT NULL default '0',"
                "`parentid` bigint(20) NOT NULL default '0',"
                "`links` int(11) NOT NULL default '0',"
                "`filename` varchar(64) NOT NULL default '',"
                "`params`   varchar(128) NOT NULL default '',"
                "`words` varchar(200) binary NOT NULL default '',"
                "PRIMARY KEY  (`id`)"
                ") ENGINE=MyISAM; ",i));
    }
    

    return 1;
}


//-----------------------------------------------------------------------------
void    DbRepository::_GetURLFileSaveName(KSpUrl& url, XCHAR* fn, XCHAR* destfn, int cread)
{
    KXCHAR*     pds = _destlocation.c_str();
    XCHAR path[512] = {0};
    XCHAR temp[512] = {0};
    XCHAR temp2[512];

    strcpy(temp,url->GetServerName());
    
    if(!t_strncmp(temp,"www.",4))
        strcpy(temp2,temp+4);
    else
        strcpy(temp2,temp);
    
    if(temp2[0] == 0)
    {
        strcpy(temp2,"000");
    }
    else
    {
        for(size_t i=0;i<strlen(temp2);i++)
        {
            if(!Is_alnum(temp2[i]))
                temp2[i]='0';
        }
    }
    
    t_sprintf(path, "%s/%c", pds, temp2[0]);
    if(cread) 
        mk_dir(path);
        
    t_sprintf(path, "%s/%c/%c", pds, temp2[0], temp2[1]);
    if(cread)
        mk_dir(path);

    t_sprintf(path, "%s/%c/%c/%c", pds, temp2[0], temp2[1],temp2[2]);
    if(cread)
        mk_dir(path);


    t_sprintf(path, "%s/%c/%c/%c/%s", pds, temp2[0], temp2[1],temp2[2],url->GetServerName());
    if(cread)
        mk_dir(path);


    size_t last = t_strlen(path);

    if(strcmp(fn,"robots.txt"))
    {
        KXCHAR* pobject = fn;
        while(*pobject)
        {
            XCHAR nc = *pobject++;
            path[last++] = nc;
            if(nc=='/' && cread)
                mk_dir(path);
        }
        strcpy(destfn, path);
    }
    else
    {
        t_sprintf(destfn, "%s/%c/%c/%c/%s/%s", pds, temp2[0], temp2[1], temp2[2], url->GetServerName(), fn);
    }
    XCHAR* p = destfn;
    while(*p)
    {
        if(*p=='?' || *p=='*')
            *p='#';
        ++p;
    }
}
