// indexer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "osg.h"
#include "mymyysql.h"
#include "strops.h"
#include "bzipper.h"
#include "htmlparser.h"

#include <string>
#include <vector>
#include <algorithm>

int     nSites                  = 0; 
int     nCounter                = 0; 
int     n_grouprule             = 6;
UINT    n_linesperdoc           = 8;
XCHAR   s_tempFolder[260]       = {0};
XCHAR   s_destlocation[260]     = {0};
XCHAR   s_plugrepdbServer[64]   = {0};
XCHAR   s_plugrepdbDb[64]       = {0};
XCHAR   s_plugrepdbUser[64]     = {0};
XCHAR   s_plugrepdbPswd[64]     = {0};
UINT    n_maxsitetbls           = 16;
UINT    n_maxwordtables         = 16*6;
DWORD   n_curindexlevel         = 1;
int		n_minimweight           = 3;
int		n_maxdocspersite		= 64;
int		n_maxwordsperdoc        = 96;

vector<string>         _forbwords;

static void    GetCountry(const string& szIP, string& ciSo, MySQL& sql);
static void    _LoadCommonWords(KXCHAR* fileName);
static void    CreateTables(MySQL& sql);
static BOOL    Configure(KXCHAR* fname);
static void    Assign(KXCHAR* pred, KXCHAR* val);
static void    StartIndexing(MySQL& sql);
static BOOL    IndexFile(KXCHAR* sn, MySQL& sql, KXCHAR* filename, KXCHAR* ciso, KXCHAR* sumaryFile, int pageid,int dochash, int index);
static DWORD   _GetHash(KXCHAR* word);
static BOOL    IsFobiddenWord(XCHAR* pszob);
static void    GetReportFileName(KXCHAR* siteName, KXCHAR* fn, XCHAR* destfn);
static DWORD   _GetNextWordID(MySQL& sql,int inc);
static DWORD   _GetNextSearchID(MySQL& sql, int inc);
static void    _SortByValue(map< string, int>& weights, vector<map< string, int>::iterator>& vsorted);
static void    _ParsePdf(const XCHAR* fn, KXCHAR* sumaryFile, map<string,int>& weights);
static void    _ParseHtm(const XCHAR* sn, const XCHAR* fn, KXCHAR* sumaryFile, map<string,int>& weights, vector<string>&, int index);

struct  PNWord
{
    DWORD   srcId;
    DWORD   srcTbl;
    DWORD   wrdTbl;
    DWORD   wrdId;
}  gPnWords = {0} ;


//5A B3C 15C9F 2A37A6 51CBD6A 9E7AF027 30E31508 EB7F8C53
//66 CA7 18891 2F89EE 5C1B42D B274D1C5 9C25673B
//53 A5A 1413F 26E6F2 4B5F79E 9208FC73 AF16924F 33BBB7E4 43BB44E9 33AD5890 41FDB9C1 FDB97EAF B9765782 75549920

//---------------------------------------------------------------------------------------
int main(int argc,XCHAR* argv[])
{
    DWORD   dwr = _GetHash("abcdef");
    dwr = _GetHash("njoydeco"); printf("\r\n");
    dwr = _GetHash("zalsoft");printf("\r\n");
    dwr = _GetHash("gamedevgamedev");printf("\r\n");

	nCounter = 0;
    if(Configure("buzznet.conf"))
    {
        MySQL sql;
        if(sql.Connect(s_plugrepdbServer, s_plugrepdbUser, s_plugrepdbPswd))
        {
            sql.Qry(_F("USE %s;",s_plugrepdbDb));
            printf("press '~' to delete database;\n");
            int cha = getchar();

            if(cha=='~')
            {
                
                CreateTables(sql);
            } 
			
			while(1)
			{
            	StartIndexing(sql);
				os::ge_sleep(1000*7200);
			}
            
			
			sql.Disconnect();
        }
    }
    os::ge_sleep(12000);
	return 0;
}


//---------------------------------------------------------------------------------------
static BOOL Configure(KXCHAR* fname)
{
    os::H_FILE* pf = f_open(fname, "rb");
    if(pf)
    {
        XCHAR   pred[128];
        XCHAR   val[128];
        XCHAR   line[256];
        BOOL    inComment = FALSE;
        while(!feof(pf))
        {
            if(f_gets(line,255,pf))
            {
                so::trimall(line,' ');
				so::trimall(line,'\t');
                so::trimall(line,'\r');
                so::trimall(line,'\n');
                if(*line==0)
                    continue;
                if(!t_strncmp(line,"/*",2))
                {
                    inComment=1;
                    continue;
                }
                if(!t_strncmp(line,"*/",2))
                {
                    inComment=0;
                    continue;
                }
                if(inComment || *line=='#')
                    continue;
        
				KXCHAR* pnext = so::strccpy(pred, line,'=');

				if(pnext && *pnext)
				{
					so::strscpy(val, (XCHAR*)pnext+1, "#*");
					Assign(pred, val);
				}
            }
            else
                break;
            if(feof(pf))
                break;
        }
        f_close(pf);
    }
    return 1;
}

//---------------------------------------------------------------------------------------
static void Assign(KXCHAR* pred, KXCHAR* val)
{
    if(!strcmp(pred,"tempfolder"))
    {
        strcpy(s_tempFolder,val);
        so::trimright(s_tempFolder,'/');
    }
    if(!strcmp(pred,"writelocation"))
    {
        ::strcpy(s_destlocation,val);
        mk_dir(s_destlocation);
        return;
    }
    else if(!strcmp(pred,"indexlevel"))
	{
	    n_curindexlevel=::t_atoi(val);
	}

    else if(!strcmp(pred,"docspersite"))
	{
	    n_maxdocspersite=::t_atoi(val);
	}
    

    else if(!strcmp(pred,"sitetables"))
	{
	    //n_maxwordtables=::t_atoi(val);
        n_maxsitetbls   = ::t_atoi(val);
		n_maxwordtables = ::n_maxsitetbls*6;
	}
    else if(!strcmp(pred,"database"))
    {
        XCHAR loco[128];
        strcpy(loco, val);

        XCHAR* pTok = strtok(loco,",");
        strcpy(s_plugrepdbServer,pTok);
        
        if(pTok = strtok(0,","))
            strcpy(s_plugrepdbDb,pTok);
        if(pTok = strtok(0,","))
            strcpy(s_plugrepdbUser,pTok);
        if(pTok = strtok(0,","))
            strcpy(s_plugrepdbPswd,pTok);
        return;
    }
}
//StartToIndex();


static void CreateTables(MySQL& sql)
{
    //-----------------------------------------------------------------------
    // create a_kwords b_kwords ...z_kwords
    for(UINT i=0; i < 99; i++)
    {
        sql.Qry(_F("DROP TABLE IF EXISTS `words%d`;",i));
        sql.Qry(_F("DROP TABLE IF EXISTS `search%d`;",i));
    }

    for(UINT i=0; i < n_maxwordtables; i++)
    {
        

        sql.Qry(_F("CREATE TABLE `words%d` ("
                "`id` bigint(20) NOT NULL ,"
                "`word` char(32) NOT NULL default '',"
                "`devey` decimal(10,0) NOT NULL default '0',"
                "PRIMARY KEY  (`word`,`id`),"
                "UNIQUE KEY `unq` (`word`,`id`)"
                ") ENGINE=MyISAM; ",i));

        

        sql.Qry(_F("CREATE TABLE `search%d` ("
                    "`id` bigint(20) NOT NULL ,"
                    "`word_id` bigint(20) NOT NULL default '0',"   // kword id
                    "`weight` int(11) NOT NULL default '0',"       // occurences in doc
                    "`doc_tbl` int(11) NOT NULL default '0',"      // hash table index for doc and site
                    "`doc_id` bigint(20) NOT NULL default '0',"    // doc id ... leads to site id
                    "`pword_id` bigint(20) NOT NULL default '0'," // kword id
                    "`pword_tbl` bigint(20) NOT NULL default '0'," // kword id
                    "`nword_id` bigint(20) NOT NULL default '0'," // kword id
                    "`nword_tbl` bigint(20) NOT NULL default '0'," // kword id
                    " PRIMARY KEY  (`id`,`word_id`)"
                    ") ENGINE=MyISAM; ",i));
    }
    sql.Qry("UPDATE last_ids SET wordsid=1,srchid=1;");

}

// keep a file with txt in wich we index the offsets from database
static void  StartIndexing(MySQL& sql)
{
    DWORD  siteID;
    DWORD  docpageId;
    string siteName;
    string siteRemote;
    string ciSo;
    string szIP;
    string docpageName;
    TCHAR  sumFileName[260];

    for( UINT i=0;i < n_maxsitetbls; i++)    
    {
        //
        // index pages
        //
        while(sql.Qry(_F("SELECT id,name,ciso,ip,remote FROM sites%d WHERE indexed!=%d LIMIT 1;",i,n_curindexlevel)))
        {
            MYSQL_ROW row = sql.Fetch();
            siteID      = ::atol(row[0]);
            siteName    = row[1];
            ciSo        = row[2];
            szIP        = row[3];
            siteRemote  = row[4];
            ++nSites;
			
            if(ciSo.empty() || ciSo=="*" ||ciSo=="US")
                GetCountry(szIP, ciSo, sql);
			printf ("Queriyng site  '%s' \r\n", siteName.c_str());
			
            GetReportFileName(siteName.c_str(),siteName.c_str(),sumFileName);
            un_link(sumFileName); // short content text file

            vector<string>  pages;
			
            if(sql.Qry(_F("SELECT id,filename FROM docs%d WHERE siteid=%d AND indexed!=%d LIMIT %d;",
							i, siteID,n_curindexlevel,n_maxdocspersite)))
            {
                MYSQL_ROW row ;
                    
                while(row = sql.Fetch())
                {
                    pages.push_back(row[0]); //id
                    pages.push_back(row[1]); //document pagename
                }
            }
			
            if(pages.size())
            {
                _LoadCommonWords(ciSo.c_str());
                for(UINT pg=0; pg < pages.size();)
                {
                    docpageId   = ::atol(pages[pg].c_str());
                    docpageName = pages[pg+1];

                    // set country tooo
                    if(IndexFile(siteName.c_str(),sql, docpageName.c_str(), ciSo.c_str(), sumFileName, docpageId, i, pg))
                    {
                        sql.Qry(_F("UPDATE docs%d SET indexed=%d WHERE id=%d;",i,n_curindexlevel,docpageId));
                    }
                    pg += 2;
                }
            }
            sql.Qry(_F("UPDATE sites%d SET indexed=%d,ciso='%s',time=NOW() WHERE id=%d;",i,n_curindexlevel,ciSo.c_str(),siteID));
        }
    }
}


static void    DeleteLowRankLines(vector<map< string, int>::iterator>& vsorted, 
                           vector<string>& vsumanry)
{
    BOOL found = 0;
    vector<string>::iterator sb =  vsumanry.begin();
    for(;sb != vsumanry.end(); sb++)
    {
        found = 0;
        vector<map< string, int>::iterator>::iterator bw = vsorted.begin();
        for(; bw != vsorted.end(); ++bw) //for each word
        {
            int   wght          = (*bw)->second;
            const string&  itm  = (*bw)->first;

            if(wght > n_minimweight)
            {
                
                if(sb->find(itm)!=-1)
                {
                    found = 1;
                    break;
                }
            }
        }    
        if(!found && 
		   sb!= vsumanry.begin() && 
		   sb!=vsumanry.end() && vsumanry.size())
        {
            sb = vsumanry.erase(sb);
            if(sb!=vsumanry.begin() && vsumanry.size())
			{
                sb--;
			}
        }
    }
}


static BOOL  IndexFile(KXCHAR* sn, MySQL& sql, KXCHAR* filename, KXCHAR* ciso, 
                KXCHAR* sumaryFile, int docid, int dochash, int index)
{
    Bzipper b;
#ifdef _WIN32
    if(b.UnCompress(_F("%s.z",filename),_F("%s/temp.txt",s_tempFolder)))
#else
    if(b.UnCompress(_F("%s.z",filename),_F("%s/temp.txt",s_tempFolder)))
#endif //
    {
        return 0;
    }

	int indexedwords = 0;
    int                                     maxweight = 0;
    map<string,int>                         weights;
    vector<string>                          vsumanry ;

    printf("%d / %d  Indexing: %s ...",nCounter++,nSites ,filename);

    try{
   	    _ParseHtm(sn, filename, sumaryFile, weights,vsumanry, index);
    }catch(...)
    {
       printf(" error \r\n");
       return FALSE;
    }

    //
    // sort by weight
    //
    vector<map< string, int>::iterator> vsorted;
    _SortByValue(weights, vsorted);
    //
    // delete lines that weight <= 1
    //
    DeleteLowRankLines(vsorted, vsumanry);

    if(vsumanry.size())
    {
        // save sumary file per/site
        //
        FILE*   pfsc = f_open(sumaryFile,"ab"); // short content text file
        if(0==pfsc)
	        pfsc = f_open(sumaryFile,"wb"); 
    	    
        if(pfsc)
        {
            f_printf(pfsc,"\r\n:%s\r\n",filename);  // page name
            vector<string>::iterator b = vsumanry.begin();
            for(;b!=vsumanry.end();b++)
            {
                f_printf(pfsc,"%s\r\n",b->c_str());  // page name
            }
            f_close(pfsc);

        }

        //
        // place word with weight > 1 in database
        //
		
        vector<map< string, int>::iterator>::iterator bw = vsorted.begin();
        for(; bw != vsorted.end() && indexedwords < n_maxwordsperdoc; ++bw)
        {
            int      wght = (*bw)->second;
            const string&  itm  = (*bw)->first;

            if(wght > n_minimweight)
            {
                DWORD dwid,srcid;
                const XCHAR* pwrd = itm.c_str();
                DWORD   hash =_GetHash(pwrd);
                if(!sql.Qry(_F("SELECT id FROM words%d WHERE word='%s';",hash,pwrd)))
                {
                    dwid = _GetNextWordID(sql,1);
                    if(!sql.Qry(_F("INSERT INTO words%d VALUES(%d,'%s',0.0);",hash,dwid, pwrd)))
                    {
                        dwid = _GetNextWordID(sql,-1);
                    }
                }
                else
                {
                    dwid = sql.FetchLong(0);
                }
                srcid = _GetNextSearchID(sql,1);

                if(gPnWords.srcId)
                {
                    //update next word being this
                    sql.Qry(_F("UPDATE search%d SET nword_id=%d, nword_tbl=%d WHERE id=%d",
                               gPnWords.wrdTbl, dwid, hash, gPnWords.srcId));
                }
                
                
                sql.Qry(_F("INSERT INTO search%d VALUES "
                           "(%d,%d,%d,%d,%d,%d,%d,0,0);",
                            hash, srcid, dwid, wght, dochash, docid, gPnWords.wrdId,
                                                                     gPnWords.wrdTbl));

                // store a queue of last 3 words
                gPnWords.wrdId  = dwid;
                gPnWords.wrdTbl = hash;
                gPnWords.srcId  = srcid; // prev index
                gPnWords.srcTbl = hash;  // same as wrdTbl
				++indexedwords;
            }
        }
    }
	printf("%d words\n", indexedwords);
    return 1;
}

//-----------------------------------------------------------------------------
static BOOL    IsFobiddenWord(XCHAR*  pszob)
{
    so::trimcrlf(pszob);
    so::trimfront(pszob);
    so::trimright(pszob,' ');
    if(pszob[0]==0)  return 1;
    if(pszob[1]==0)  return 1;
    if(pszob[0]==' ')return 1;
    if(pszob[0]=='-')return 1;
    if(pszob[0]=='+')return 1;
    if(pszob[0]=='.')return 1;
    if(pszob[0]==',')return 1;
    if(pszob[0]=='=')return 1;
    if(pszob[0]=='/')return 1;
    if(strlen(pszob)<3)return 1;
    so::strlower(pszob);
    if(strchr(pszob,'=')) return 1;
    if(strchr(pszob,':')) return 1;
    if(strchr(pszob,'{')) return 1;
    if(strchr(pszob,'}')) return 1;
    if(strchr(pszob,'>')) return 1;
    if(strchr(pszob,'<')) return 1;
    if(strchr(pszob,';')) return 1;
    if(strchr(pszob,'(')) return 1;
    if(strchr(pszob,'\'')) return 1;
    if(strchr(pszob,'\"')) return 1;
    if(strchr(pszob,'/')) return 1;
    if(strchr(pszob,'\\')) return 1;
    if(strchr(pszob,')')) return 1;
	if(!strcmp(pszob,"www")) return 1;
	if(!strcmp(pszob,"com")) return 1;
	if(!strcmp(pszob,"net")) return 1;
    if(strstr(pszob,"content-"))return 1;
    if(strstr(pszob,"homepage"))return 1;
		

    int elems = (int)_forbwords.size();
    while(--elems > -1)
    {
        if(_forbwords[elems]==pszob)
            return 1;
    }
    return 0;
}


static DWORD   _GetHash(KXCHAR* word)
{
    DWORD     sumo = 0;
    KXCHAR*   pb   = word;
    int       wrd;
    while(*pb)
    {
        wrd = (*pb-20); if(wrd<0)wrd=0;
        sumo = (31 * sumo) + wrd;
        printf("%X ", sumo);
        ++pb;
    }
    return (sumo % n_maxwordtables);
}

static DWORD _GetNextWordID(MySQL& sql, int inc)
{
    if(inc)
        sql.Qry("UPDATE last_ids SET wordsid=wordsid+1;");
    else
        sql.Qry("UPDATE last_ids SET wordsid=wordsid-1;");
    sql.Qry("SELECT wordsid FROM last_ids;");
    return sql.FetchLong(0);
}

static DWORD _GetNextSearchID(MySQL& sql, int inc)
{
    if(inc)
        sql.Qry("UPDATE last_ids SET srchid=srchid+1;");
    else
        sql.Qry("UPDATE last_ids SET srchid=srchid-1;");
    sql.Qry("SELECT srchid FROM last_ids;");
    return sql.FetchLong(0);
}


static bool IsInDoc(vector<string>& vlines, string& line, KXCHAR* sword)
{
    if(line.find(sword) == -1)
    {
        vector<string>::iterator sb = vlines.begin();
        vector<string>::iterator se = vlines.end();
        for(;sb!=se;++sb)
        {
            if(sb->find(sword) != -1)
            {
                return true;
            }
        }
        return false;
    }
    return true;
}


static void   AddWord(XCHAR* temp, string& line, vector<string>& vlines, map<string,int>& weights, int weight)
{
    int linelen = 0;
    if(!*temp) return ;

    if(!strcmp(temp,"C++"))
        strcpy(temp,"c-plus-plus");
    
    char* token = 0;
    if(strchr(temp,' '))token = " ";
    if(strchr(temp,','))token = ",";
    if(strchr(temp,'.'))token = ".";
    if(strchr(temp,'_'))token = "_";
    if(strchr(temp,';'))token = ";";

    if(token)
    {
        TCHAR* pt = strtok(temp,token);
        while(pt)
        {
			if(*pt && !IsFobiddenWord(pt) && !IsInDoc(vlines,line,pt))
			{
               weights[pt] += weight;
               line+=_F("%s ", pt);
			}
            pt = strtok(0,token);
        }
    }
    else   if(!IsFobiddenWord(temp))
    {
        weights[temp] += weight;
        if(!IsInDoc(vlines,line,temp))
        {
            line+=_F("%s ", temp);
        }
    }
}



static void    _ParseHtm(KXCHAR* sn, const XCHAR* fname, KXCHAR* sumaryFile, 
                  map<string,int>& weights, vector<string>& vlines, int index)
{
    XCHAR          temp[1024]  = {0};
    XCHAR          tag[1024]   = {0};
    BOOL           finish      = 0;
    string         sline;      sline.reserve(256); sline = "";
    int            nweight = 0;

    str_istream si(_F("%s/temp.txt",s_tempFolder));
    scanner     sc(si);

	strcpy(temp,sn);
	if(index==0)
	{
		printf("<%s>", temp);
		AddWord(temp, sline,vlines, weights,1000);
	}
	
    while(!finish)
    {
        int t = sc.get_token();
        switch(t)
        {
            case scanner::TT_ERROR:
                tag[0] = 0;
                break;
            case scanner::TT_EOF:
                finish=1;
                break;
            case scanner::TT_TAG_START:
                strcpy(tag,sc.get_tag_name());
                so::strlower(tag);
                break;
            case scanner::TT_TAG_END:
                tag[0] = 0;
                //sc.get_tag_name());        
                break;
            case scanner::TT_ATTR:
                strcpy(temp, sc.get_attr_name());
                so::strlower(temp);
                if(0==strcmp(temp,"href"))
                {
                    strcpy(temp, sc.get_value());
                    AddWord(temp, sline, vlines, weights,50);
                }
                else if(0==strcmp(tag,"meta"))
                {
                    strcpy(temp, sc.get_value());
                    AddWord(temp, sline, vlines, weights,100);
                }
                break;
            case scanner::TT_WORD: 
            case scanner::TT_SPACE:
                  strcpy(temp, sc.get_value());
				  switch(tag[0])
				  {
					  case 't': //title or td or tr or ti
		 		  	      AddWord(temp, sline, vlines, weights, 1);
					      break;
					  case 'h':
						  if(*tag && so::isdigit( tag[1] ))
					      	 nweight = 50 - atoi(&tag[1]); //h1,h2,h3
						  else 
						      AddWord(temp, sline, vlines, weights, 1);
						  break;
					  
					  case 'a':
						  AddWord(temp, sline, vlines, weights,50);
						  break;
					  default:
						  AddWord(temp, sline, vlines, weights,1);
					  break;
							  
				  }
				  break;
        }
        if(sline.length() > 78 || finish) 
        {
            vlines.push_back(sline);
            sline.erase();
            if(vlines.size()>n_linesperdoc) //12 lines per doc
                break;
        }
    }
}


static void    _ParsePdf(KXCHAR* fname, KXCHAR* sumfname ,map<string,int>& weights, vector<string>& lines)
{
}

static void _LoadCommonWords(KXCHAR* fileName)
{
    _forbwords.clear();
    
    FILE* pf = f_open(_F("db_data_init/stopwords/%s",fileName),"rb");
    if(!pf)
        pf = f_open("db_data_init/stopwords/US","rb");
    if(pf)
    {
        XCHAR line[128] = {0};
        while(!feof(pf))
        {
            if(f_gets(line,120, pf))
            {
                if(line[0]!='#')
                {
                    so::trimcrlf(line);
                    if(line[0])
                        _forbwords.push_back(line);
                }
            }
            if(ferror(pf))   
                break;
        }
        f_close(pf);
    }
}


static void    GetReportFileName(KXCHAR* siteName, KXCHAR* fn, XCHAR* destfn)
{
	KXCHAR* pds = s_destlocation;
	XCHAR path[512] = {0};
	XCHAR temp[512] = {0};
	XCHAR temp2[512];

	strcpy(temp,siteName);
	
	if(!t_strncmp(temp,"www.",4))
		strcpy(temp2,temp+4);
	else
		strcpy(temp2,temp);
	
	if(temp2[0] == 0)
	{
		strcpy(temp2,"000");
	}
	else if(strlen(temp2) < 3)
	{
		strcat(temp2 ,"000");
	}
	t_sprintf(destfn, "%s/%c/%c/%c/%s/%s.sum", pds, temp2[0], temp2[1], temp2[2], siteName, fn);
    so::surefilename(destfn);
}

static bool comparex(const map< string, int>::iterator& a, const map< string, int>::iterator& b)
{
    return (a->second) > (b->second);
}

static void    _SortByValue(map< string, int>& weights, vector<map< string, int>::iterator>& vsorted)
{
    map< string, int>::iterator bw =  weights.begin();
    
	for(; bw!=weights.end(); ++bw)
        vsorted.push_back(bw);
	
    sort(vsorted.begin(), vsorted.end(),comparex);
}

static void    GetCountry(const string& szIP, string& ciSo, MySQL& sql)
{
    DWORD uip = ::inet_addr(szIP.c_str());
    sql.Qry(_F("SELECT iso_code FROM anp_ip2country WHERE %d >= ip_from "
                "AND %d <= ip_to LIMIT 1;",uip,uip));
    if(sql.GetRowCount())
        ciSo = sql.Fetch(0);
    else
        ciSo = "US";
}
