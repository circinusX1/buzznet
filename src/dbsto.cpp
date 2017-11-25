
#include "url.h"
#include "core.h"
#include "dbsto.h"
#include "buzznet.h"
#include "storage.h"



void DbSto::DeleteDocs(KSpUrl& url, KXCHAR* fileName)
{
    DWORD tbl = url->SiteHash();
    DWORD id  = url->DocID();
    if(id!=NILLA)
    {
        h_sql.Qry(_F("DELETE FROM docs%d where id=%d;", tbl, id));
    }
}

DWORD DbSto::GetSiteHits(SpUrl& url)
{
    MYSQL_ROW  r = _QrySite(url);   //id,robo,hits,time
    if(r)
    {
        url->Hits(atol(r[2]));
    }
	return url->Hits();
}

//-----------------------------------------------------------------------------
void    DbSto::AddSitedHit(KSpUrl& url)
{
    long  value  = 1;
	DWORD tblIdx = url->SiteHash();
    DWORD sid = url->SiteID();

	if(!h_sql.Qry(_F("UPDATE sites%d SET hits=hits+1 WHERE id=%d;", tblIdx, sid)))
	{
		printf("Cannot update 'hits' to [%d]%d:%s",tblIdx, sid,url->SiteName());
		Buz::b_BreaK = 1;
	}

    if(!h_sql.Qry(_F("SELECT hits FROM sites%d WHERE id=%d;", tblIdx, sid)))
	{
         this->SaveSite(url,tblIdx);
	}
	else
	{
		value = h_sql.FetchLong(0);
	}
	url->Hits((int)value);
}


//-----------------------------------------------------------------------------
void DbSto::SaveDocument(KSpUrl& url, KXCHAR* fileName)
{
	if(fileName[0]==0 || fileName[1]==' ')
		return;

    DWORD siteId = url->SiteID();
    DWORD tblIdx = url->SiteHash();

    if(NILLA == siteId)
    {
        siteId = SaveSite(url, tblIdx);
    }
    assert(siteId!=NILLA);

    DWORD   docID = NILLA;
    if(h_sql.Qry(_F("SELECT id FROM docs%d WHERE filename='%s';", tblIdx, fileName)))
    {
        docID = h_sql.FetchLong(0);

        if(!h_sql.Qry(_F("UPDATE docs%d SET time=NOW(),indexed=0 WHERE id=%d;",tblIdx,docID)))
        {
             PRINTF(_D|_E,"Error updating doc %s into tbl %d", fileName, tblIdx);
             Buz::b_BreaK = 1;
        }
    }
    else
    {
        docID    = _DB_NextDocID(1);

        if(!h_sql.Qry(_F("INSERT INTO docs%d VALUES "
                        "(%d,%d,%d,'%s',NOW(),0);", 
                         tblIdx,
                         docID,
                         url->SiteID(), 
                         url->RefererID(),
                         fileName)))
        {
            PRINTF(_D|_E,"Error inserting doc %s into tbl %d", fileName, tblIdx);
            Buz::b_BreaK = 1;
        }
        url->DocID((DWORD)docID);
    }
	AddSitedHit(url);
}


//-----------------------------------------------------------------------------
DWORD DbSto::SaveSite(KSpUrl& url, DWORD tblIdx, int timeup)
{
    DWORD siteID = GetSiteID(url);
    if(NILLA==tblIdx)
    {
        tblIdx = url->SiteHash();
    }
    if(NILLA == siteID)
    {
        KXCHAR* sname = url->FileSiteName();
        DWORD   dwIP  =  url->DwIP();

        siteID       = _DB_NextSiteID(1);

        if(!h_sql.Qry(_F("INSERT INTO sites%d VALUES( %d,'%s','%s','%s','%s',"
                      "%d, %d, NOW(),%d,%d,'*','*','*',0);",
                                                    tblIdx,
                                                    siteID,
                                                    sname,
                                                    url->Service(),
                                                    "*",
                                                    url->SzIP(),
                                                    url->SrvPort(),
                                                    0,
                                                    url->Hits(),
                                                    url->Robots())))
	    {
    	     PRINTF(_D|_E,"Error inserting site %s into tbl %d", sname, tblIdx);
             Buz::b_BreaK = 1;
	    }
    }else if(timeup)
    {
        h_sql.Qry(_F("UPDATE sites%d SET time=NOW(),indexed=0 WHERE id=%d;",tblIdx,siteID));
    }
    url->SiteID(siteID);
    return siteID;
}

BOOL	DbSto::HasTimeout(KSpUrl& url, DWORD hours)//++++
{
    XCHAR fullSite[800];
    url->FullName(fullSite);

    if(h_sql.Qry(_F("SELECT timediff FROM  scheduler WHERE docname='%s';", fullSite)))
    {
        return hours > (DWORD)h_sql.FetchLong(0);
    }
    return hours > Buz::n_rechecksite;
}


//-----------------------------------------------------------------------------
DWORD DbSto::_DB_NextDocID(int inc)
{
	if(inc)
    	h_sql.Qry("UPDATE last_ids SET docid=docid+1;");
	else
		h_sql.Qry("UPDATE last_ids SET docid=docid-1;");
    h_sql.Qry("SELECT docid FROM last_ids;");
    return h_sql.FetchLong(0);
}

//-----------------------------------------------------------------------------

DWORD DbSto::_DB_NextSiteID(int inc)
{
	if(inc)
    	h_sql.Qry("UPDATE last_ids SET siteid=siteid+1;");
	else
		h_sql.Qry("UPDATE last_ids SET siteid=siteid-1;");
    h_sql.Qry("SELECT siteid FROM last_ids;");
    return h_sql.FetchLong(0);
}


BOOL    DbSto::Robots(KSpUrl& url)
{
    if(url->Robots()) 
        return 1;

    MYSQL_ROW  r = _QrySite(url);   //id,robo,hits,time
    if(r)
    {
        //url->SiteID(atol(r[0]));
        url->Robots(atol(r[1]));
        //url->Hits(atol(r[2]));
    }
    return url->Robots();
}

//-----------------------------------------------------------------------------
DWORD   DbSto::GetSiteID(KSpUrl& url)
{
    if(url->SiteID()!=NILLA)
        return url->SiteID();

    DWORD tblIdx = url->SiteHash();

    XCHAR sname[128];
    strcpy(sname, url->SiteName());
    so::to_db_filename(sname);

    if(h_sql.Qry(_F("SELECT id FROM sites%d WHERE name = '%s';", tblIdx, sname)))
    {
        url->SiteID(h_sql.FetchLong(0));
        return url->SiteID();
    }
    return NILLA;
}

//-----------------------------------------------------------------------------
BOOL DbSto::HasDocument(KSpUrl& url, XCHAR* pout)
{
    long  sid    = url->SiteID();
    DWORD tblIdx = url->SiteHash();

    KXCHAR*  sfile = p_storage->GetFileSto().GetFileName(url, 1);
	if(pout)
		strcpy(pout, sfile);

    if(h_sql.Qry(_F("SELECT time FROM docs%d WHERE filename='%s' AND siteid=%d LIMIT 1;",
                        tblIdx, sfile, sid)))
    {
        //++++
        XCHAR* dTime = h_sql.Fetch(0);
        const  time_t recTime = os::str_time(dTime);
        const  time_t curTime   = time(0);
        DWORD  hours = (DWORD) ((difftime(curTime, recTime))/3600);
        if(HasTimeout(url,hours))
        {
            KXCHAR* file = p_storage->GetFileSto().GetFileName(url, 1);

            rename(_F("%s.z",file),_F("%s.%s.z",file,dTime));

            if(!strcmp(url->Document(),"/"))
            {
                if(!h_sql.Qry(_F("UPDATE sites%d SET hits=0,robo=0 WHERE id=%d;", tblIdx, sid)))
                {
                    PRINTF(_D|_E,"Cannot update hits=0 on revisited %s",url->FileSiteName());
                    Buz::b_BreaK = 1;
                }
            }
            return 0;
        }
        return 1;
    }
    return 0;
}


void DbSto::CatcheRejectedSites(vector<string>& stopSites)
{
    if(h_sql.Qry("SELECT name FROM forb_servers;"))
    {
        MYSQL_ROW  r;
        while(r = h_sql.Fetch())
        {
			PRINTF(_D,"%s +Fobidden\n",r[0]);
			if(r[0] && strlen(r[0])>3)
                stopSites.push_back(r[0]);
        }
    }
}

void DbSto::CatcheRejectedWords(vector<string>& stopW)
{
    if(h_sql.Qry("SELECT word FROM forb_words"))
    {
        MYSQL_ROW  r;
        while((r = h_sql.Fetch()))
        {
			if(r[0] && strlen(r[0])>3)
			{
				PRINTF(_D,"'%s' Word is Fobidden\n",r[0]);
                stopW.push_back(r[0]);
            }
		}
    }
}

MYSQL_ROW  DbSto::_QrySite(KSpUrl& url)
{
    long    bsq    = 0;//my_ulonglong
    DWORD   tblIdx = url->SiteHash();
    DWORD   sid    = url->SiteID();
    
    if(sid!=NILLA)
    {
        bsq = (long)h_sql.Qry(_F("SELECT id,robo,hits,time FROM sites%d WHERE id=%d;", tblIdx, sid));
    }
    else
    {
        bsq = (long)h_sql.Qry(_F("SELECT id,robo,hits,time FROM sites%d WHERE name='%s';", tblIdx, url->FileSiteName()));
    }
    if(bsq)
        return h_sql.Fetch();
    return 0;
}

void    DbSto::StayAlive()
{
   h_sql.Qry("SELECT * FROM sites1 LIMIT 1;");
}
