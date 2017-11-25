
#include "buzznet.h"
#include "wrkqueues.h"
#include "storage.h"

WrkQueues::WrkQueues(void)
{
    q_work.SetSize(Buz::n_queuelength);
    v_externs.reserve(Buz::n_queuelength);
    h_vis.Clear();
}

WrkQueues::~WrkQueues(void)
{
}


void WrkQueues::Save(XCHAR* fileName, int removeelems, BOOL final)
{
    os::H_FILE* pf = f_open(fileName, "ab");
	if(pf==0)
		pf = f_open(fileName, "wb");
		
    if(pf != 0)
    {
        PRINTF(_D|_I,"Saving %s\r\n", fileName);
        int     remains = 0;
		DWORD   flength = 0;
        if(removeelems) // save a portion of it
        {
            remains = (int)q_work.GetSize()-(int)removeelems;
            if(remains<0)remains=0;
        }

        SpUrl   refUrl2;
        XCHAR   serialbuff[1400];
        while(q_work.GetFifo(&refUrl2) >= remains)
        {
            refUrl2->WriteTo(serialbuff);
            fputs(serialbuff,pf);
            flength = ftell(pf);
            if(final)
            {
                KXCHAR* file = p_storage->GetFileSto().GetFileName(refUrl2, 1, 1);
                p_storage->GetFileSto().DeleteFile(file);
                p_storage->GetDBSto().DeleteDocs(refUrl2, file);
            }
        }
        f_close(pf);
		
		if(flength > 256000)
		{
            P_Buz->RecicleFile(fileName);
			P_Buz->OnStateChanged();
		}
    }
    else
    {
		PRINTF(_D|_E,"Error saving %s\r\b",fileName);
    }
    os::ge_sleep(10);
}



//-----------------------------------------------------------------------------
int WrkQueues::Load(XCHAR* fileName, BOOL firsttime)
{
   
    int        ifread = 0;
    os::H_FILE* pf     = 0;
    //find higer file queueXXX from 256 down
    for(int t=0; fileName[t]; t++)
	{
		if(so::isdigit(fileName[t]))
		{
            int maxfiles = 256;
            while(--maxfiles>-1)
            {
                char sno[16]; 

                fileName[t] = 0;
                t_sprintf(sno,"%d",maxfiles);
                strcat(fileName,sno);
                if(pf = f_open(fileName,"rb"))
                {
                    break;
                }
            }
		}
        if(pf)
            break;
	}
    if(pf)
    {
        // load queue file
        XCHAR line[800] = {0};
        vector<string>    newfile;
        BOOL              queueFull = 0;  
        newfile.reserve(1024);
        v_externs.clear();
        PRINTF(_D,"Load(%s)\r\n",fileName);
        while(!feof(pf))
        {
            f_gets(line, 800, pf);
			
            if(line[0]!='#')
            {
                so::trimcrlf(line);
				if(!strchr(line,'|'))
				{
					continue;
				}
                if(line[0]!='#' && line[0] && line[1])
                {
                    if(0 == queueFull)
                    {
                        try{

                            SpUrl refUrl(new Url(line));
                            if(refUrl->Good())
                            {
                                if(0==this->PushToExtQ(refUrl))
								{
									queueFull = 1;
								}
								else
								{
                                    ++ifread;
								}
                            }
                        }catch(...)
                        {
                            PRINTF(_E,"Exception _CheckInputFile() %s \r\n");
                            break;
                        }
                    }
                    else
                    {
                        newfile.push_back(line);
                        if(newfile.size() > 4096)
                            break;
                    }
                }
            }
            if(feof(pf))
                break;
        }
        f_close(pf);
        os::ge_sleep(10);
        un_link(fileName);

        if(newfile.size())
        {
            os::H_FILE* pf = f_open(fileName, "wb");
            if(pf)
            {
                PRINTF(_D,"ReSaving %s\r\n", fileName);

                vector<string>::iterator b = newfile.begin();
                for(;b!=newfile.end();b++)
                {
                    f_printf(pf,"%s\r\n",b->c_str());
                }
                f_close(pf);
            }
        }
    }
	else
		os::ge_sleep(100);

    return ifread;        
}


void WrkQueues::EraseUrl(KSpUrl& url)
{
    h_vis.Pop((Url*)url.get());

    deque<SpUrl>& rq = q_work.GetInnerQ();

    deque<SpUrl>::iterator b;
    deque<SpUrl>::iterator e;
AGAIN:
    b = rq.begin();
    e = rq.end();
    for(;b!=e;++b)
    {
        if(!strcmp((*b)->SiteName(), url->SiteName()))
        {
            rq.erase(b);
            goto AGAIN;
        }
    }
}


BOOL    WrkQueues::CheckExternalURLs()
{
	SpUrl l;
	int   moved = 0;
	while(v_externs.size())
	{
		SpUrl& u = v_externs.back();
		h_vis.Pop((Url*)u.get());
		
		if(l.get() == 0)
			l = u;
		
		if(!l->SameSite(u.get())) 
		{
			break;
		}
			
		if(!p_storage->FilterIn(u, -1))
		{
			if(PushToQ(u) > Buz::n_queuelength /2)
			{
				v_externs.pop_back();
				break;
			}
			++moved;
		}
		v_externs.pop_back();
		
	}
	if(moved)
		PRINTF(_D,"%d urls moved from ExQ to Proc Q\n", moved);
    return 0;
}


int WrkQueues::ForceInQ(KSpUrl& url)
{
    q_work.PushBack((SpUrl&)url);
    h_vis.Push((Url*)url.get());
    return (int)q_work.GetSize();
}

int WrkQueues::PushToExtQ(KSpUrl& url)
{
    v_externs.push_back((SpUrl&)url);
    h_vis.Push((Url*)url.get());
	if((int)v_externs.size() < (Buz::n_queuelength/2))
    	return  (int)v_externs.size();
	return 0;
}

int WrkQueues::PushToQ(KSpUrl& url)
{
    q_work.PushBack((SpUrl&)url);
    h_vis.Push((Url*)url.get());
    return (int)q_work.GetSize();
}

void WrkQueues::EraseAll()
{
	h_vis.Clear();
	q_work.Clear();
}

int WrkQueues::GetFifo(SpUrl* url)
{
    return q_work.GetFifo(url);
}

void WrkQueues::Check()
{
    if(q_work.GetSize() > Buz::n_queuelength)
    {
        this->Save(Buz::s_queFileName, Buz::n_queuelength/2);
    }

    if(v_externs.size() > (size_t)Buz::n_queuelength)
	{
		this->SaveExternalURLs(Buz::s_queFileName);
	}
}

void WrkQueues::SaveExternalURLs(XCHAR* fileName)
{
	size_t flength;
    os::H_FILE* pf = f_open(fileName, "ab");
	if(pf==0)
    	pf = f_open(fileName, "wb");
    if(pf != 0)
    {
        SpUrl   refUrl2;
        XCHAR   serialbuff[1400];
        while(v_externs.size())
        {
            refUrl2  = v_externs.back();

            refUrl2->WriteTo(serialbuff);
            fputs(serialbuff,pf);
            flength = ftell(pf);
			
            v_externs.pop_back();
        }

	    if(flength > 256000)
	    {
            P_Buz->RecicleFile(fileName);
		    P_Buz->OnStateChanged();
	    }
    }
    else
    {
		PRINTF(_D|_E,"Error saving %s\r\b",fileName);
    }
    os::ge_sleep(10);
}
