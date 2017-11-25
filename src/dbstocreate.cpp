
#include "url.h"
#include "core.h"
#include "dbsto.h"
#include "buzznet.h"


void     DbSto::CreateTables()
{
    if(!h_sql.Connect(Buz::s_plugrepdbServer, Buz::s_plugrepdbUser, Buz::s_plugrepdbPswd))
    {
        h_sql.Disconnect();
        return ;
    }
	printf("Press ~ to wipe out database\r\n");
	int ch = getchar();
    BOOL bdelete = (ch=='~');
    
    h_sql.Qry(_F("USE %s;",Buz::s_plugrepdbDb));

    //-------------------------------------------------------------------------
    // COUNTRIES
    if(bdelete)
        h_sql.Qry("DROP TABLE IF EXISTS `_countries`;");
    h_sql.Qry( "CREATE TABLE `_countries` (`ISO_Code` varchar(2) NOT NULL default '',"
              "`Country` varchar(255) NOT NULL default '',"
              "`Region` varchar(255) NOT NULL default '',"
              "`RegionCode` varchar(4) NOT NULL default '',"
              "`Capital` varchar(255) NOT NULL default '',"
              "`Currency` varchar(255) NOT NULL default '',"
              "PRIMARY KEY  (`ISO_Code`)"
              ") TYPE=MyISAM;");
   
   
    string s = _F("LOAD DATA INFILE '%s/cntrys.dat' "
	              "INTO TABLE _countries FIELDS TERMINATED BY ',' "
	              "OPTIONALLY ENCLOSED BY '''' LINES TERMINATED BY '\r\n'",
	             Buz::s_appFolder);
    h_sql.Qry(s.c_str());

    //-------------------------------------------------------------------------
    // COUNTRY IP
    if(bdelete)
        h_sql.Qry("DROP TABLE IF EXISTS anp_ip2country;");
    h_sql.Qry("CREATE TABLE anp_ip2country ("
                "ip_from bigint(10) NOT NULL default '0',"
                "ip_to bigint(10) NOT NULL default '0',"
                "iso_code char(4) NOT NULL default '',"
                "PRIMARY KEY  (ip_from,ip_to), "
                "UNIQUE KEY k_ipfrom (ip_from,ip_to)"
                ") TYPE=MyISAM;" );

    h_sql.Qry(_F("LOAD DATA INFILE '%s/cntry_ips.dat' INTO TABLE"
	            " anp_ip2country FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED "
				"BY '''' LINES TERMINATED BY '\r\n'",Buz::s_appFolder) );
        
    //-----------------------------------------------------------------------
    // 
    if(bdelete)
    {
        for(int i=0;i<99;i++)
        {
            h_sql.Qry(_F("DROP TABLE IF EXISTS `sites%d`;",i));
            h_sql.Qry(_F("DROP TABLE IF EXISTS `docs%d`;",i));
		    h_sql.Qry(_F("DROP TABLE IF EXISTS `words%d`;",i));
		    h_sql.Qry(_F("DROP TABLE IF EXISTS `search%d`;",i));
	    }
    }

	for(int i=0;i<Buz::n_maxsitetebles;i++)
	{
        h_sql.Qry(_F("CREATE TABLE `sites%d` ("
                "`id` bigint(20) NOT NULL default '0',"
                "`name` varchar(64) NOT NULL default '',"
                "`service` int(11) NOT NULL default '0',"
                "`ciso` varchar(4) NOT NULL default '', "
                "`ip` varchar(64) NOT NULL default '',"
                "`port` int(11) NOT NULL default '0',"
                "`priority` int(11) NOT NULL default '0',"
                "`time` timestamp NOT NULL,"
                "`hits`  int(11) NOT NULL default '0',"
                "`robo`  int(11) NOT NULL default '0',"
                "`remote` varchar(64) NOT NULL default ''," // is on remote buzznet
                "`devey` varchar(64) NOT NULL default '',"
                "`cookie`   varchar(128) NOT NULL default '',"
                "`indexed`  int(11) NOT NULL default '0',"
                "PRIMARY KEY  (`id`,`name`), "
                "UNIQUE KEY `unq` (`name`)"
                ") ENGINE=MyISAM; ",i));

        h_sql.Qry(_F("CREATE TABLE `docs%d` ("
                "`id` bigint(20) NOT NULL default '0',"
                "`siteid` bigint(20) NOT NULL default '0',"
                "`parentid` bigint(20) NOT NULL default '0',"
                "`filename` varchar(200) NOT NULL default '',"
                "`time` timestamp NOT NULL,"
                "`indexed`  int(11) NOT NULL default '0',"
                "PRIMARY KEY  (`id`, `filename`), "
                "UNIQUE KEY `unq` (`id`,`filename`)"
                ") ENGINE=MyISAM; ",i));
    }//

    //if(bdelete)
        h_sql.Qry("DROP TABLE IF EXISTS `forb_servers`;");
    h_sql.Qry("CREATE TABLE `forb_servers` ("
             "`id` bigint(20) NOT NULL auto_increment,"
             "`name` varchar(100) NOT NULL default '',"
             "`ip` varchar(100) NOT NULL default '',"
             "`forbiden` int(11) NOT NULL default '0',"
             "PRIMARY KEY  (`id`,`name`), "
             "UNIQUE KEY `unq` (`name`)"
             ") ENGINE=MyISAM; ");
	
     s = _F("LOAD DATA INFILE '%s/forb_servers.dat' "
	              "INTO TABLE forb_servers FIELDS TERMINATED BY ',' "
	              "OPTIONALLY ENCLOSED BY '''' LINES TERMINATED BY '\r\n'",
	             Buz::s_appFolder);
    h_sql.Qry(s.c_str());

    //if(bdelete)
        h_sql.Qry("DROP TABLE IF EXISTS `forb_words`;");

    h_sql.Qry("CREATE TABLE `forb_words` ("
            "`id` bigint(20) NOT NULL auto_increment,"
            "`language` varchar(100) NOT NULL default '',"
            "`word` varchar(100) NOT NULL default '',"
            "PRIMARY KEY  (`id`,`word`), "
            "UNIQUE KEY `unq` (`id`,`word`)"
            ") ENGINE=MyISAM;" );

    s = _F("LOAD DATA INFILE '%s/forb_words.dat' "
	              "INTO TABLE forb_words FIELDS TERMINATED BY ',' "
	              "OPTIONALLY ENCLOSED BY '''' LINES TERMINATED BY '\r\n'",
	             Buz::s_appFolder);
    h_sql.Qry(s.c_str());


    if(bdelete)
        h_sql.Qry("DROP TABLE IF EXISTS `redirects`;");
    h_sql.Qry("CREATE TABLE `redirects` ("
            "`id` bigint(20) NOT NULL auto_increment,"
            "`redirect_id` bigint(20) NOT NULL default '0',"
            "`urlname` varchar(80) NOT NULL default '',"
            "PRIMARY KEY  (`id`, `urlname`),"
            "UNIQUE KEY `unq` (`id`,`urlname`)"
            ") ENGINE=MyISAM; "); 

    if(bdelete)
        h_sql.Qry("DROP TABLE IF EXISTS `redirect_rules`;");
    h_sql.Qry("CREATE TABLE `redirect_rules` ("
            "`id` bigint(20) NOT NULL auto_increment,"
            "`iso_code` varchar(4) NOT NULL default '',"
            "`buzznet_ip` varchar(64) NOT NULL default '',"
            "`buzznet_addr` varchar(32) NOT NULL default '',"
            "`extra` varchar(100) NOT NULL default '',"
            "PRIMARY KEY  (`id`,`buzznet_ip`,`buzznet_addr`)"
            ") ENGINE=MyISAM ; ");

    if(bdelete)
        h_sql.Qry("DROP TABLE IF EXISTS `last_ids`;");

    h_sql.Qry("CREATE TABLE `last_ids` ("
            "`siteid` bigint(20) NOT NULL default '0',"
            "`docid` bigint(20) NOT NULL default '0',"
            "`wordsid` bigint(20) NOT NULL default '0',"
            "`srchid` bigint(20) NOT NULL default '0',"
            "`doc_tbls` bigint(20) NOT NULL default '0',"
            "`site_tbls` bigint(20) NOT NULL default '0'"
            ") ENGINE=MyISAM ; ");


    if(bdelete)
        h_sql.Qry("DROP TABLE IF EXISTS `scheduler`;");

    h_sql.Qry("CREATE TABLE `scheduler` ("
                "`id` bigint(20) NOT NULL default '0',"
                "`docname` varchar(100) NOT NULL default '',"
                "`timediff` int(11) NOT NULL default '0',"
                "`when` time NOT NULL default '00:00:00',"
                "`save` int(11) NOT NULL default '0',"
                "PRIMARY KEY  (`id`,`docname`(10))"
                ") ENGINE=MyISAM ;"); 

    if(bdelete)
        h_sql.Qry("DROP TABLE IF EXISTS `siteconf`;");

    h_sql.Qry("CREATE TABLE `siteconf` ("
                "`site` varchar(100) NOT NULL default '',"
                "`depth` int(11) NOT NULL default '0',"
                "`pages` int(11) NOT NULL default '0',"
                "`id` int(11) NOT NULL default '0',"
                "PRIMARY KEY  (`id`)"
                ") ENGINE=MyISAM;");


    if(bdelete)
    {
        h_sql.Qry(_F("INSERT INTO last_ids VALUES (1,1,0,0,%d,%d);",Buz::n_maxsitetebles, 
                                                                   Buz::n_maxsitetebles));
        printf("Database wiped !!!\r\n");
    }
    return ;
}
