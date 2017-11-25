
       Target Country by IP Address - VERSION 2.01 - advanced redirection 
        	- for MySql, Plain text databases -
   Copyright (C) 2005 Jgsoft Associates - http://www.analysespider.com/geo-targeting

This program is free software; you can redistribute it and/or modify it under the terms of 
the GNU General Public License as published by the Free Software Foundation; either version 
2 of the License, or any later version. 

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

CONTACT:
	http://www.analysespider.com
	robert@analysespider.com

DOWNLOAD UPDATES, IP DATABASES AND APPLY FOR WEB SERVICES AT:
	http://www.analysespider.com

WHAT DOES IT DO?

- Target Country by IP Address is a complete web-based software that provides webmasters with the ability to redirect/restrict traffic based on the visitor's country of origin.This allows webmasters to promote their services and products in different countries where they have specific distribution and service capabilities in different countries. 

- It features an IP exception list, where you can place your trusted IPs for exceptions. 

- Blocks certain IP address's from your site and redirects the user to a blocked page.Very simple add the IP range or single IP you want to block then.

- The script should be included in unlimited number of pages on your website so it could redirect visitors to those web pages according to the redirection rules.

- With this script you can create as many "redirection rules" as you want. On the same website, some pages could have different redirection rules than the others. You could use the same databases and the same script. Only you make a new "redirection rules" , just edit the REDIRECTION RULES file. Then include it in your files.

- The country code of the visitors is saved in a cookie or a session variable in order to avoid searching the IP database repeatedly.

- You can specify for each country (or IP database) has a Redirection Rule (a redirection link associated with it). If the visitor`s IP is in an IP database, he is redirected to the redirection link associated with it, or if this one is blank, to the default redirection link. 
If this default redirection link is blank too, the page is loaded. If the visitor`s country has no Redirection Rule specified, he is redirected to the default 'reject' link or if this one is blank, the page is loaded.

- It is avalable for IP databses: "Plain text - one file with IP numbers and country code", "Plain text - one file with IP addresses and country code", "MySql dump - one table with IP numbers and country code".- all available at http://www.analysespider.com. 

- The free IP2Country database(73% accurate) included in this distribution has only 172 
countrieswith 37879 records. it`s by far not as accurate and updated as our commercial database, available at http://www.analysespider.com

- It also creates an array with visitor`s country information

REQUIREMENTS:
	Web server with PHP minimum version 4.1.x, optionaly MySql;

FILES:
	cr/.htaccess - Apache restriction file, to deny access to cr/ folder, to avoid abuse.
	cr/anp_ips.dat - a free IP2Country database - type: Plain text full with IP numbers
	cr/anp_ip2country.sql - Sql Dump of the same free database
	cr/exceptions_ips.dat - A list of IPs who will be excuded from the redirection. It can only have IP addresses (192.168.0.1) or IP address ranges; Edit this file to add exceptions ip address; Exemple:
			128.177.244.100
			194.112.94.250;194.112.94.255
	cr/blocked_ips.dat - A list of IPs who will be blocked from the redirection. It can only have IP addresses (192.168.0.1) or IP address ranges; Edit this file to add blocked ip address; Exemple:
			213.120.148.65
			213.120.148.70;213.120.148.123
	cr/cr.php - main script file
	cr/anp_****.php - aditional functions
	cr/cur_re_rules.php - default redirection rules file.
	cr/_re_rules.php - redirection rules template file.
	cr/Countries.php - Get Country information files
	cr/Countries.* - Country information files
	cr/flags/* - GIF country flags

IP NUMBERS AND IP ADDRESSES:
  The unique 4-part number assigned to each and every computer linked to the Internet (e.g., 207.41.2.111). When you connect to the Internet, your ISP assigns you an IP number for the duration of your connection. 
  The formula to convert an IP Address of the form A.B.C.D to an IP Number is:
   IP Number = A x (256*256*256) + B x (256*256) + C x 256 + D
  Which is the same as:
   IP Number = A x 16777216 + B x 65536 + C x 256 + D

INSTALL:
 - Edit cr/cr.php and modify it as described there.
 - Add the next line at the beginning of EVERY PAGE YOU NEED TO REDIRECT VISITORS:

	<?PHP $anp_path="cr/"; include($anp_path."cr.php"); ?>

		or (if cr/ directory is a level up)

	<?PHP $anp_path="../cr/"; include($anp_path."cr.php"); ?> 
 - If you use MySql, load cr/anp_ip2country.sql into your mysql server:
	mysql -u [username] -p
	mysql>use [your database]
	mysql>source [path to/]anp_ip2country.sql

 - If you want a different set of redirection rules in other pages of your website,you could use the same databases and the same script. All you can make a copy of _re_rules.php -> new_re_rules.php,edit new_re_rules.php, just change the REDIRECTION RULES. Then include it in your files with: 
  <?php $re_rules = "new_re_rules.php";  $anp_path="cr/"; include($anp_path."cr.php"); ?>

THE IP DATABASE
 - The free IP -> Country database has data for 172 countries and the dat file size is 851 Kb.
 The number of records is 37879 and the number of IP addresses covered is 2,351,067,326 .it`s 73% accurate.
 - You can get an updated and optimized IP database from http://www.analysespider.com with data from RIPE, ARIN, APNIC and LARNIC public databases.
  It has data for 209 countries and the data file size is 3.06 Mb
  At the time when this script was released the number of records was 132,532 and the IP addresses covered were 4,145,812,945.
  Target Country by IP Address was designed for this database and we strongly recommend that you install this one. Used with this script is 98% accurate because of the search functions.

TESTING AND DEBUG-ING
 To test and debug you need to set in cr/cr.php the constant _DEBUG_MODE to 1: define("_DEBUG_MODE","1") then poin your browser to http://yoursite.com/demo.php. Now play with the configuration, modify redirection rules. When you are satisfied, set _DEBUG_MODE to 2 in order to test how cookies / session vars are saved. When it`s ok set _DEBUG_MODE to 0.

CONFIGURATION TIPS
 - If you want to use Target Country by IP Address just to detect the country of your visitor and to get the country details in an array, edit cr/cur_re_rules.php comment out all REDIRECTION RULES and make blank the default redirect and reject links ( =""; ). 
 Then edit demo.php to see how you can use the country details of your visitor. 
 Add the next line:
 <?php include($anp_path."countries.php"); $anp_cinfo=get_cinfo($anp_country_code);?>

 This fields can be used in your scripts: $anp_cinfo["code"] $anp_cinfo["name"] $anp_cinfo["region"] $anp_cinfo["capital"] $anp_cinfo["currency"] 
 $rv_cinfo["flag_path"] ; The flag image can be generated like this: <img src="<?php echo $rv_cinfo["flag_path"]; ?>" alt="" border="0">

- If you want to restrict access to visitors from XX country,Or you want to block a whole lot of countries but make exceptions for some search spider ip addresses.
You have to add the IP address or the network`s IP range in the cr/exceptions_ips.dat file in order for the visitors to be able to visit your website.

- Blocks certain IP address's from your site and redirects the user to a blocked page.
 You have to set :$anp_default_reject_link= an invalid url.
 Then Edit blocked_ips.dat,add the IP addresses (197.208.0.1) or IP address ranges you want to block. Exemple:
  213.64.128.65
  213.64.108.50;213.64.148.13

- If you want to use Target Country by IP Address to safely reject all visitors from country XX and YY and to accept all the other visitors you must set : $anp_default_reject_link=""; $anp_default_redirect_link=""; , $anp_url_save_method="session";
 edit the Redirection Rules file(cur_re_rules.php), for country XX uncomment the line and place an invalid url for it (XX=error.php) . Do the same for YY.
 
 - If you need to display language specific pages for Italy and German:     $rv_default_reject_link=""; $rv_default_redirect_link=""; 
  edit the Redirection Rules file(cur_re_rules.php), for country IT uncomment the line,
  as redirection rule set IT=yourpage_en.htm; Do the same for DE;

 - Finally, if you want to be 98% sure that the IP -> Country conversion is realistic, you have to use the best database on the market, database provided by us at http://www.analysespider.com/ip2country

Email us at sales@analysespider.com 