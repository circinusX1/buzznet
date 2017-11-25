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
