
#include "core.h"
#include "filesto.h"
#include "bzipper.h"
#include "pageparser.h"
#include "buzznet.h"
#include "storage.h"



//-----------------------------------------------------------------------------
// _NEW_NEW
KXCHAR*    FileSto::GetFileName(KSpUrl& url, BOOL target, int cread)
{
    KXCHAR* pds = Buz::s_destlocation;
    XCHAR   temp[256]   = {0};
    XCHAR   temp2[256]  = {0};
    XCHAR   szfname[512] = {0};

    if(target)url->Document(szfname);
    else       strcpy(szfname,"robots.txt");
        
    strcpy(temp, url->FileSiteName());
    
    if(!t_strncmp(temp,"www.",4))
        strcpy(temp2, temp+4);
    else
        strcpy(temp2, temp);
    
    if(temp2[0] == 0 || strlen(temp2)<3)
    {
        strcpy(temp2,"000");
    }
    for(int i=0;i<3;i++)
    {
        if(temp2[i]=='.')
            temp2[i]='0';
    }
    //NEW_NEW
    t_sprintf(s_temp, "%s/%c/%c/%c/%s/%s", pds, temp2[0], temp2[1],temp2[2],url->FileSiteName(),szfname);
    so::to_db_filename(s_temp);
    if(cread)
    {
        XCHAR* pchr = s_temp;
        while(*pchr != 0)
        {
            if(*pchr=='.')
                break;
            if(*pchr=='/'||*pchr=='\\')
            {
                *pchr = 0;
                if(!this->_IsDir(s_temp))
                {
                    mk_dir(s_temp);
                    _dirs.insert(s_temp); //cache to reduce IO
                }
                *pchr = '/';
            }
            ++pchr;
        }
    }
    return s_temp;
}

//-----------------------------------------------------------------------------
void  FileSto::DeleteFile(KXCHAR* filename)
{
    un_link(filename);
}

//=======================================================================================
// sves on disk pagename file with page content of max length 8k for url url
BOOL    FileSto::SaveDocument(KXCHAR* pName,
                              KXCHAR* page, 
                              int     length, 
                              KSpUrl& url, 
                              const   PageParser& pp,
                              XCHAR* destBuff)
{
    if(page == 0 || *page==0)
        return FALSE;

    KXCHAR* pcontent = pp.AfterHeaderContent();
    DWORD   memlen   = pp.GetContentLength();
    if(pcontent && *pcontent)
    {
        KXCHAR* sfile = GetFileName(url, 1, 1);
        if(destBuff)
            strcpy(destBuff, sfile); //return filename to caller
        
        if(Buz::b_compressfiles)
        {
		    Bzipper b;
            b.Compress(sfile, (BYTE*)pcontent, memlen);
        }
        else
        {
            if(!File_Save(sfile, pcontent, memlen, "wb"))
			{
				Buz::b_BreaK = 1;
			}
        }
    }
    return 1; 
}

//-----------------------------------------------------------------------------
BOOL    FileSto::SaveRobots(XCHAR* pageIn, size_t length, KSpUrl& url)
{
    

    if(pageIn==0 || length==0)
    {
         pageIn=0; length=0;
    }
    KXCHAR* fileName =GetFileName(url, 0, 1);
    os::H_FILE* pf = f_open(fileName, "wb");
    if(pf)
    {
        if(length)
        {
            XCHAR*   page = new XCHAR[length+8];

            strcpy(page, pageIn);

            BOOL   start = 0;
            XCHAR* line  = t_strtok(page,"\n");
            while(line)
            {
                if(line[0]=='#')
                {
                    line = t_strtok(0,"\n");
                    continue;
                }
                so::trimall(line,' ');
                so::trimall(line,'\t');
                so::trimall(line,'\r');
                so::trimall(line,'\n');

                if(strstr(line,"User-agent:*") ||strstr(line,"user-agent:*") || strstr(line,"User-agent:BuzzNet") && ! start)
                {
                    start = 1;
                    line = t_strtok(0,"\n");
                    continue;
                }
                else if((strstr(line,"User-agent:") || strstr(line,"user-agent:"))&& start)
                {
                    start = 0;
                    line = t_strtok(0,"\n");
                    continue;
                }

                if(start)
                {
                    if(!strcmp(line,"Disallow:/") || !strcmp(line,"disallow:/"))
                    {
                        f_puts("/\r\n",pf);
                        break;
                    }

                    if(strstr(line,"Disallow:")||strstr(line,"disallow:"))
                    {
                        XCHAR* pd = strstr(line,"Disallow:");
                        if(0==pd)
                            pd = strstr(line,"disallow:");
                        if(pd)
                        {
                            XCHAR loco[128];
                            so::strscpy(loco, pd+9,"\r\n");
                            strcat(loco,"\r\n");
                            f_puts(loco, pf);
                        }
                    }
                }
                line = t_strtok(0,"\n");
            }
            delete[] page;
        }
        else
            fputs("#\r\n",pf);
        f_close(pf);
    }
    
    return 1;
}

void FileSto::LoadRobotsFile(KSpUrl& url, vector<string>&   vrobotDocs)
{
    KXCHAR*   fileName = GetFileName(url, 0);
    FILE* pf = f_open(fileName, "rb");
    if(!pf) 
        return;                    // no robots file

    XCHAR loco[256];
    while(!feof(pf))
    {
        ::memset(loco,0,4);
        if(fgets(loco, 255, pf))
        {
            so::trimcrlf(loco);
            if(loco[0])
            {
                vrobotDocs.push_back(loco);
            }
        }
        if(feof(pf))
            break;
    }
    f_close(pf);
}


//=======================================================================================
BOOL FileSto::HasDoc(KSpUrl& url)
{
    double      seconds=0;
    KXCHAR*     file = GetFileName(url, 1);
	
    FILE* pf = f_open(file,"rb");
	if(!pf)
		pf = f_open(_F("%s.z",file),"rb");
    if(pf)
    {
#ifdef WIN32		
        struct _stat  st = {0};
#else
		struct stat  st = {0};
#endif //

        if(0 == f_stat(fileno(pf), &st))
        {
            const time_t curTime   = time(0);
            
#ifdef _DEBUG
            const  XCHAR* ct = ctime(&curTime);
            const  XCHAR* ft = ctime(&st.st_ctime );
#endif //
            seconds = difftime(curTime, st.st_ctime);
        }
        fclose(pf);

        double  oldTime = Buz::n_rechecksite;
        if(seconds > oldTime)
        {
            un_link(file);
            return 0;
        }
        return 1;
    }
    return 0;
}


void FileSto::LogErrorHosts(SpUrl& url, KXCHAR* errStr, XCHAR* errFile)
{
    os::H_FILE* pf = f_open(errFile, "ab");
    if(pf)
    {
        fprintf(pf,"%s %s%s%s\r\n",errStr, url->Service(), url->FileSiteName(), url->Document());
        size_t length = ftell(pf);
        fclose(pf);

        if(length>2000000)
        {
            P_Buz->RecicleFile(errFile);
        }
    }
}


//---------------------------------------------------------------------------------------
BOOL    FileSto::CheckInputFile()
{
    XCHAR       loco[260];

    sprintf(loco, "%s/buzznet.txt",Buz::s_tempFolder);
    os::H_FILE* pf = f_open(loco,"rb");
    if(pf)
    {
        char line[514];
        while(!feof(pf))
        {
            f_gets(line, 512, pf);
            if(line[0]!='#')
            {
                while(line[str_len(line)-1]=='\r' || line[str_len(line)-1]=='\n')
                    line[str_len(line)-1]=0;
                if(line[0]!='#' && line[0] && line[1])
                {
                    try{
                        SpUrl refUrl(new Url(line, Buz::u_depth, 0));
                        if(refUrl->Good())
                        {
                            if(Buz::b_pushRootUrl)    
                            {
							    Url* prd = refUrl->New_RootDocument();
							    if(prd)
							    {
								    SpUrl su(prd);
                                    p_storage->GetWQ().PushToExtQ(su);
							    }
                            }

                            p_storage->GetWQ().PushToExtQ(refUrl);
                        }
                    }catch(...)
                    {
                        PRINTF(_E,"Exception _CheckInputFile() %s \r\n");
                    }
                }
            }
            if(feof(pf))
                break;
        }
        f_close(pf);
        os::ge_sleep(32);
        un_link(loco);
        return 1;
    }
    return 0;
}


BOOL FileSto::_IsDir(KXCHAR *szpath)
{
    if(_dirs.find(szpath) != _dirs.end())
        return 1;

#ifdef WIN32		
    struct _stat  st = {0};
#else
    struct stat  st = {0};
#endif //
   if(0 == s_tat(szpath, &st))
   {
#ifdef WIN32		
       return (_S_IFDIR & st.st_mode);
#else
       return S_ISDIR (st.st_mode);
#endif //
   }
   return 0;
}

void  FileSto::Reset()
{
    _dirs.clear();
}