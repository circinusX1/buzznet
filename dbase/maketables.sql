# Host: localhost
# Database: buzznet
# Table: 'page_sites'
# 
CREATE TABLE `page_sites` (
  `page_id` bigint(20) NOT NULL auto_increment,
  `page_url_id` bigint(20) NOT NULL default '0',
  `page_subpages` varchar(100) NOT NULL default '',
  `page_robots` int(11) NOT NULL default '0',
  `page_location` bigint(20) NOT NULL default '0',
  `page_hits` bigint(20) NOT NULL default '0',
  `page_parent` bigint(20) NOT NULL default '0',
  `page_words` bigint(20) NOT NULL default '0',
  `page_name` varchar(100) NOT NULL default '',
  `page_params` varchar(100) NOT NULL default '',
  `page_time` timestamp NOT NULL,
  `page_lock` int(11) NOT NULL default '0',
  `page_devey` decimal(10,0) NOT NULL default '0',
  `page_error` int(11) NOT NULL default '0',
  PRIMARY KEY  (`page_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: localhost
# Database: buzznet
# Table: 'url_sites'
# 
CREATE TABLE `url_sites` (
  `url_id` bigint(20) NOT NULL auto_increment,
  `url_ip` bigint(20) NOT NULL default '0',
  `url_name` varchar(100) NOT NULL default '',
  `url_fertched_date` timestamp NOT NULL,
  `url_country` int(11) NOT NULL default '0',
  `url_zone` int(11) NOT NULL default '0',
  `url_username` varchar(100) NOT NULL default '',
  `url_parent_id` bigint(20) NOT NULL default '0',
  `url_password` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`url_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

# Host: localhost
# Database: buzznet
# Table: 'wk_queue'
# 
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
) ENGINE=MyISAM DEFAULT CHARSET=latin1; 

