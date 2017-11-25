# Host: localhost
# Database: buzznet
# Table: 'wk_queue'
# working queue table
CREATE TABLE `wk_queue` (
  `wk_id` bigint(11) NOT NULL auto_increment,
  `wk_pid` bigint(20) NOT NULL default '0',
  `wk_service` varchar(32) NOT NULL default '',
  `wk_port` int(11) NOT NULL default '0',
  `wk_ip` varchar(40) NOT NULL default '',
  `wk_server` varchar(128) NOT NULL default '',
  `wk_page` varchar(255) NOT NULL default '',
  `wk_params` varchar(255) NOT NULL default '',
  `wk_robots` int(11) NOT NULL default '0',
  `wk_recicled` int(11) NOT NULL default '0',
  `wk_depth` int(11) NOT NULL default '0',
  `wk_sparent` varchar(100) NOT NULL default '',
  `wk_hash` varchar(254) NOT NULL default '',
  `wk_timestamp` timestamp NOT NULL,
  `wk_delayed` int(11) NOT NULL default '0',
  PRIMARY KEY  (`wk_id`,`wk_hash`)
) ENGINE=MyISAM; 

# Host: localhost
# Database: buzznet
# Table: 'a_kwords'
# b_kwords c_kwords and so on
CREATE TABLE `a_kwords` (
  `id` bigint(20) NOT NULL auto_increment,
  `firstlet` char(2) NOT NULL default '',
  `word` char(32) NOT NULL default '',
  `devey` decimal(10,0) NOT NULL default '0',
  `unused` int(11) NOT NULL default '0',
  PRIMARY KEY  (`id`,`word`)
) ENGINE=MyISAM; 


# Host: localhost
# Database: buzznet
# Table: 'page_disp'
# page dispatcher
CREATE TABLE `doc_disp` (
  `tblname` char(12) NOT NULL default '',
  `min` bigint(20) NOT NULL default '0',
  `max` bigint(20) NOT NULL default '0',
  PRIMARY KEY  (`min`,`max`)
) ENGINE=MyISAM; 

# Host: localhost
# Database: buzznet
# Table: 'site_disp'
# site table dispatcher
CREATE TABLE `site_disp` (
  `tblname` char(12) NOT NULL default '',
  `min` bigint(20) NOT NULL default '0',
  `max` bigint(20) NOT NULL default '0',
  PRIMARY KEY  (`min`,`max`)
) ENGINE=MyISAM; 

# Host: localhost
# Database: buzznet
# Table: 'docs0'
# pages
CREATE TABLE `docs0` (
  `id` bigint(20) NOT NULL auto_increment,
  `siteid` bigint(20) NOT NULL default '0',
  `parentid` bigint(20) NOT NULL default '0',
  `links` int(11) NOT NULL default '0',
  `filename` varchar(64) NOT NULL default '',
  `params`   varchar(128) NOT NULL default '',
  `words` varchar(200) binary NOT NULL default '',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM; 

# Host: localhost
# Database: buzznet
# Table: 'site0'
# 
CREATE TABLE `sites0` (
  `id` bigint(20) NOT NULL auto_increment,
  `ip` varchar(64) NOT NULL default '',
  `name` varchar(64) NOT NULL default '',
  `service` int(11) NOT NULL default '0' ,
  `ciso` varchar(4) NOT NULL default '', 
  `port` int(11) NOT NULL default '0',
  `priority` int(11) NOT NULL default '0',
  `time` timestamp NOT NULL,
  `pages` int(11) NOT NULL default '0',
  `links` int(11) NOT NULL default '0',
  `depth` int(11) NOT NULL default '0',
  `hits`  int(11) NOT NULL default '0',
  `robots` int(11) NOT NULL default '',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM; 


# Host: localhost
# Database: buzznet
# Table: 'search0'
# 
CREATE TABLE `search0` (
  `id` bigint(20) NOT NULL auto_increment,
  `page_id` bigint(20) NOT NULL default '0',
  `kword_id` bigint(20) NOT NULL default '0',
  `weight` int(11) NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM; 