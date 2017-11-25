#
# phpMyAdmin SQL Dump
# version 2.5.3
# http://www.phpmyadmin.net
#
# Host: localhost
# Generation Time: Sep 23, 2003 at 04:17 PM
# Server version: 4.0.10
# PHP Version: 4.3.0
# 
# Database : `ip2`
# 

# --------------------------------------------------------

#
# Table structure for table `anp_ip2country`
#
# Creation: Sep 23, 2003 at 04:15 PM
# Last update: Sep 23, 2003 at 04:16 PM
# Last check: Sep 23, 2003 at 04:16 PM
#

DROP TABLE IF EXISTS anp_ip2country;
CREATE TABLE anp_ip2country (
  ip_from bigint(10) NOT NULL default '0',
  ip_to bigint(10) NOT NULL default '0',
  iso_code char(2) NOT NULL default '',
  PRIMARY KEY  (ip_from,ip_to),
  UNIQUE KEY k_ipfrom (ip_from,ip_to)
) TYPE=MyISAM;

#
# Dumping data for table `anp_ip2country`
#
