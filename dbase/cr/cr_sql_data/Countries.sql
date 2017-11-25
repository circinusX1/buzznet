# phpMyAdmin SQL Dump
# version 2.5.3
# http://www.phpmyadmin.net
#
# Host: localhost
# Generation Time: Sep 23, 2003 at 04:42 PM
# Server version: 4.0.10
# PHP Version: 4.3.0
# 
# Database : `ip2`
# 

# --------------------------------------------------------

#
# Table structure for table `_countries`
#

DROP TABLE IF EXISTS `_countries`;
CREATE TABLE `_countries` (
  `ISO_Code` varchar(2) NOT NULL default '',
  `Country` varchar(255) NOT NULL default '',
  `Region` varchar(255) NOT NULL default '',
  `RegionCode` varchar(4) NOT NULL default '',
  `Capital` varchar(255) NOT NULL default '',
  `Currency` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ISO_Code`)
) TYPE=MyISAM;

#
# Dumping data for table `_countries`
#

INSERT INTO `_countries` VALUES ('AD', 'Andorra', 'Europe','EU', 'Andorra la Vella', 'Euro');
INSERT INTO `_countries` VALUES ('AE', 'United Arab Emirates', 'Middle East','ARB', 'Abu Dhabi', 'UAE Dirham');
INSERT INTO `_countries` VALUES ('AF', 'Afghanistan', 'Asia','AS', 'Kabul', 'Afghani');
INSERT INTO `_countries` VALUES ('AG', 'Antigua and Barbuda', 'Central America-Caribbean','CA', 'Saint Johns', 'East Caribbean Dollar');
INSERT INTO `_countries` VALUES ('AI', 'Anguilla', 'Central America-Caribbean','CA', 'The Valley', 'East Caribbean Dollar');
INSERT INTO `_countries` VALUES ('AL', 'Albania', 'Europe','EU', 'Tirana', 'Lek');
INSERT INTO `_countries` VALUES ('AM', 'Armenia', 'Asia','AS', 'Yerevan', 'Armenian Dram');
INSERT INTO `_countries` VALUES ('AN', 'Netherlands Antilles', 'Central America-Caribbean','CA', 'Willemstad', 'Netherlands Antillean guilder');
INSERT INTO `_countries` VALUES ('AO', 'Angola', 'Africa','AF', 'Luanda', 'Kwanza');
INSERT INTO `_countries` VALUES ('AQ', 'Antarctica', 'Antarctic Region','ANT', '--', '');
INSERT INTO `_countries` VALUES ('AR', 'Argentina', 'South America','SA', 'Buenos Aires', 'Argentine Peso');
INSERT INTO `_countries` VALUES ('AS', 'American Samoa', 'Oceania','ANT', 'Pago Pago', 'US Dollar');
INSERT INTO `_countries` VALUES ('AT', 'Austria', 'Europe','EU', 'Vienna', 'Euro');
INSERT INTO `_countries` VALUES ('AU', 'Australia', 'Oceania','ANT', 'Canberra', 'Australian dollar');
INSERT INTO `_countries` VALUES ('AW', 'Aruba', 'Central America-Caribbean','CA', 'Oranjestad', 'Aruban Guilder');
INSERT INTO `_countries` VALUES ('AZ', 'Azerbaijan', 'Asia','AS','Baku (Baki)', 'Azerbaijani Manat');
INSERT INTO `_countries` VALUES ('BA', 'Bosnia and Herzegovina', 'Europe','EU','Sarajevo', 'Convertible Marka');
INSERT INTO `_countries` VALUES ('BB', 'Barbados', 'Central America-Caribbean','CA', 'Bridgetown', 'Barbados Dollar');
INSERT INTO `_countries` VALUES ('BD', 'Bangladesh', 'Asia','AS', 'Dhaka', 'Taka');
INSERT INTO `_countries` VALUES ('BE', 'Belgium', 'Europe','EU', 'Brussels', 'Euro');
INSERT INTO `_countries` VALUES ('BF', 'Burkina Faso', 'Africa','AF', 'Ouagadougou', 'CFA Franc BCEAO');
INSERT INTO `_countries` VALUES ('BG', 'Bulgaria', 'Europe','EU', 'Sofia', 'Lev');
INSERT INTO `_countries` VALUES ('BH', 'Bahrain', 'Middle East','ARB', 'Manama', 'Bahraini Dinar');
INSERT INTO `_countries` VALUES ('BI', 'Burundi', 'Africa','AF', 'Bujumbura', 'Burundi Franc');
INSERT INTO `_countries` VALUES ('BJ', 'Benin', 'Africa','AF', 'Porto-Novo', 'CFA Franc BCEAO');
INSERT INTO `_countries` VALUES ('BM', 'Bermuda', 'North America','NA', 'Hamilton', 'Bermudian Dollar');
INSERT INTO `_countries` VALUES ('BN', 'Brunei Darussalam', 'Southeast Asia','ASE', 'Bandar Seri Begawan', 'Brunei Dollar');
INSERT INTO `_countries` VALUES ('BO', 'Bolivia', 'South America','SA', 'La Paz /Sucre', 'Boliviano');
INSERT INTO `_countries` VALUES ('BR', 'Brazil', 'South America','SA', 'Brasilia', 'Brazilian Real');
INSERT INTO `_countries` VALUES ('BS', 'The Bahamas', 'Central America-Caribbean','CA', 'Nassau', 'Bahamian Dollar');
INSERT INTO `_countries` VALUES ('BT', 'Bhutan', 'Asia','AS', 'Thimphu', 'Ngultrum');
INSERT INTO `_countries` VALUES ('BV', 'Bouvet Island', 'Antarctic Region','ANT', '--', 'Norwegian Krone');
INSERT INTO `_countries` VALUES ('BW', 'Botswana', 'Africa','AF', 'Gaborone', 'Pula');
INSERT INTO `_countries` VALUES ('BY', 'Belarus', 'Europe','EU', 'Minsk', 'Belarussian Ruble');
INSERT INTO `_countries` VALUES ('BZ', 'Belize', 'Central America-Caribbean','CA', 'Belmopan', 'Belize Dollar');
INSERT INTO `_countries` VALUES ('CA', 'Canada', 'North America','NA', 'Ottawa', 'Canadian Dollar');
INSERT INTO `_countries` VALUES ('CC', 'Cocos (Keeling) Islands', 'Southeast Asia','ASE', 'West Island', 'Australian Dollar');
INSERT INTO `_countries` VALUES ('CD', 'Congo, Democratic Republic of the', 'Africa','AF', 'Kinshasa', 'Franc Congolais');
INSERT INTO `_countries` VALUES ('CF', 'Central African Republic', 'Africa','AF', 'Bangui', 'CFA Franc BEAC');
INSERT INTO `_countries` VALUES ('CG', 'Congo, Republic of the', 'Africa','AF', 'Brazzaville', 'CFA Franc BEAC');
INSERT INTO `_countries` VALUES ('CH', 'Switzerland', 'Europe','EU', 'Bern', 'Swiss Franc');
INSERT INTO `_countries` VALUES ('CI', 'Cote Ivoire', 'Africa','AF', 'Yamoussoukro', 'CFA Franc BCEAO');
INSERT INTO `_countries` VALUES ('CK', 'Cook Islands', 'Oceania','ANT', 'Avarua', 'New Zealand Dollar');
INSERT INTO `_countries` VALUES ('CL', 'Chile', 'South America','SA', 'Santiago', 'Chilean Peso');
INSERT INTO `_countries` VALUES ('CM', 'Cameroon', 'Africa','AF', 'Yaounde', 'CFA Franc BEAC');
INSERT INTO `_countries` VALUES ('CN', 'China', 'Asia','AS', 'Beijing', 'Yuan Renminbi');
INSERT INTO `_countries` VALUES ('CO', 'Colombia', 'South America, Central America-Caribbean', 'Bogota', 'Colombian Peso');
INSERT INTO `_countries` VALUES ('CR', 'Costa Rica', 'Central America-Caribbean','CA', 'San Jose', 'Costa Rican Colon');
INSERT INTO `_countries` VALUES ('CU', 'Cuba', 'Central America-Caribbean','CA', 'Havana', 'Cuban Peso');
INSERT INTO `_countries` VALUES ('CV', 'Cape Verde', 'World','ANY', 'Praia', 'Cape Verdean Escudo');
INSERT INTO `_countries` VALUES ('CX', 'Christmas Island', 'Southeast Asia','ASE', 'The Settlement', 'Australian Dollar');
INSERT INTO `_countries` VALUES ('CY', 'Cyprus', 'Middle East','ARB', 'Nicosia', 'Cyprus Pound');
INSERT INTO `_countries` VALUES ('CZ', 'Czech Republic', 'Europe','EU', 'Prague', 'Czech Koruna');
INSERT INTO `_countries` VALUES ('DE', 'Germany', 'Europe','EU', 'Berlin', 'Euro');
INSERT INTO `_countries` VALUES ('DJ', 'Djibouti', 'Africa','AF', 'Djibouti', 'Djibouti Franc');
INSERT INTO `_countries` VALUES ('DK', 'Denmark', 'Europe','EU', 'Copenhagen', 'Danish Krone');
INSERT INTO `_countries` VALUES ('DM', 'Dominica', 'Central America-Caribbean','CA', 'Roseau', 'East Caribbean Dollar');
INSERT INTO `_countries` VALUES ('DO', 'Dominican Republic', 'Central America-Caribbean','CA', 'Santo Domingo', 'Dominican Peso');
INSERT INTO `_countries` VALUES ('DZ', 'Algeria', 'Africa','AF', 'Algiers', 'Algerian Dinar');
INSERT INTO `_countries` VALUES ('EC', 'Ecuador', 'South America','SA', 'Quito', 'US dollar');
INSERT INTO `_countries` VALUES ('EE', 'Estonia', 'Europe','EU', 'Tallinn', 'Kroon');
INSERT INTO `_countries` VALUES ('EG', 'Egypt', 'Africa','AF', 'Cairo', 'Egyptian Pound');
INSERT INTO `_countries` VALUES ('EH', 'Western Sahara', 'Africa','AF', '--', 'Moroccan Dirham');
INSERT INTO `_countries` VALUES ('ER', 'Eritrea', 'Africa','AF', 'Asmara', 'Nakfa');
INSERT INTO `_countries` VALUES ('ES', 'Spain', 'Europe','EU', 'Madrid', 'Euro');
INSERT INTO `_countries` VALUES ('ET', 'Ethiopia', 'Africa','AF', 'Addis Ababa', 'Ethiopian Birr');
INSERT INTO `_countries` VALUES ('FI', 'Finland', 'Europe','EU', 'Helsinki', 'Euro');
INSERT INTO `_countries` VALUES ('FJ', 'Fiji', 'Oceania','ANT', 'Suva', 'Fijian Dollar');
INSERT INTO `_countries` VALUES ('FK', 'Falkland Islands (Islas Malvinas)', 'South America','SA', 'Stanley', 'Falkland Islands Pound');
INSERT INTO `_countries` VALUES ('FM', 'Micronesia, Federated States of', 'Oceania','ANT', 'Palikir', 'US dollar');
INSERT INTO `_countries` VALUES ('FO', 'Faroe Islands', 'Europe','EU', 'Torshavn', 'Danish Krone');
INSERT INTO `_countries` VALUES ('FR', 'France', 'Europe','EU', 'Paris', 'Euro');
INSERT INTO `_countries` VALUES ('FX', 'France, Metropolitan', 'EU', '--', 'Euro');
INSERT INTO `_countries` VALUES ('GA', 'Gabon', 'Africa','AF', 'Libreville', 'CFA Franc BEAC');
INSERT INTO `_countries` VALUES ('GD', 'Grenada', 'Central America-Caribbean','CA', 'Saint Georges', 'East Caribbean Dollar');
INSERT INTO `_countries` VALUES ('GE', 'Georgia', 'Europe','EU', 'Tbilisi', 'Lari');
INSERT INTO `_countries` VALUES ('GF', 'French Guiana', 'South America','SA', 'Cayenne', 'Euro');
INSERT INTO `_countries` VALUES ('GG', 'Guernsey', 'Europe','EU', 'Saint Peter Port', 'Pound Sterling');
INSERT INTO `_countries` VALUES ('GH', 'Ghana', 'Africa','AF', 'Accra', 'Cedi');
INSERT INTO `_countries` VALUES ('GI', 'Gibraltar', 'Europe','EU', 'Gibraltar', 'Gibraltar Pound');
INSERT INTO `_countries` VALUES ('GL', 'Greenland', 'Arctic Region','ANT', 'Nuuk', 'Danish Krone');
INSERT INTO `_countries` VALUES ('GM', 'The Gambia', 'Africa','AF', 'Banjul', 'Dalasi');
INSERT INTO `_countries` VALUES ('GN', 'Guinea', 'Africa','AF', 'Conakry', 'Guinean Franc');
INSERT INTO `_countries` VALUES ('GP', 'Guadeloupe', 'Central America-Caribbean','CA', 'Basse-Terre', 'Euro');
INSERT INTO `_countries` VALUES ('GQ', 'Equatorial Guinea', 'Africa','AF', 'Malabo', 'CFA Franc BEAC');
INSERT INTO `_countries` VALUES ('GR', 'Greece', 'Europe','EU', 'Athens', 'Euro');
INSERT INTO `_countries` VALUES ('GS', 'South Georgia and the South Sandwich Islands', 'Antarctic Region','ANT', '--', 'Pound Sterling');
INSERT INTO `_countries` VALUES ('GT', 'Guatemala', 'Central America-Caribbean','CA', 'Guatemala', 'Quetzal');
INSERT INTO `_countries` VALUES ('GU', 'Guam', 'Oceania','ANT', 'Hagatna', 'US Dollar');
INSERT INTO `_countries` VALUES ('GW', 'Guinea-Bissau', 'Africa','AF', 'Bissau', 'CFA Franc BCEAO');
INSERT INTO `_countries` VALUES ('GY', 'Guyana', 'South America','SA', 'Georgetown', 'Guyana Dollar');
INSERT INTO `_countries` VALUES ('HK', 'Hong Kong (SAR)', 'Southeast Asia','ASE', 'Hong Kong', 'Hong Kong Dollar');
INSERT INTO `_countries` VALUES ('HM', 'Heard Island and McDonald Islands', 'Antarctic Region','ANT', '--', 'Australian Dollar');
INSERT INTO `_countries` VALUES ('HN', 'Honduras', 'Central America-Caribbean','CA', 'Tegucigalpa', 'Lempira');
INSERT INTO `_countries` VALUES ('HR', 'Croatia', 'Europe','EU', 'Zagreb', 'Kuna');
INSERT INTO `_countries` VALUES ('HT', 'Haiti', 'Central America-Caribbean','CA', 'Port-au-Prince', 'Gourde');
INSERT INTO `_countries` VALUES ('HU', 'Hungary', 'Europe','EU', 'Budapest', 'Forint');
INSERT INTO `_countries` VALUES ('ID', 'Indonesia', 'Southeast Asia','ASE', 'Jakarta', 'Rupiah');
INSERT INTO `_countries` VALUES ('IE', 'Ireland', 'Europe','EU', 'Dublin', 'Euro');
INSERT INTO `_countries` VALUES ('IL', 'Israel', 'Middle East','ARB', 'Jerusalem', 'New Israeli Sheqel');
INSERT INTO `_countries` VALUES ('IM', 'Man, Isle of', 'Europe','EU', 'Douglas', 'Pound Sterling');
INSERT INTO `_countries` VALUES ('IN', 'India', 'Asia','AS', 'New Delhi', 'Indian Rupee');
INSERT INTO `_countries` VALUES ('IO', 'British Indian Ocean Territory', 'World','ANY', '--', 'US Dollar');
INSERT INTO `_countries` VALUES ('IQ', 'Iraq', 'Middle East','ARB', 'Baghdad', 'Iraqi Dinar');
INSERT INTO `_countries` VALUES ('IR', 'Iran', 'Middle East','ARB', 'Tehran', 'Iranian Rial');
INSERT INTO `_countries` VALUES ('IS', 'Iceland', 'Arctic Region','ANT', 'Reykjavik', 'Iceland Krona');
INSERT INTO `_countries` VALUES ('IT', 'Italy', 'Europe','EU', 'Rome', 'Euro');
INSERT INTO `_countries` VALUES ('JE', 'Jersey', 'Europe','EU', 'Saint Helier', 'Pound Sterling');
INSERT INTO `_countries` VALUES ('JM', 'Jamaica', 'Central America-Caribbean','CA', 'Kingston', 'Jamaican dollar');
INSERT INTO `_countries` VALUES ('JO', 'Jordan', 'Middle East','ARB', 'Amman', 'Jordanian Dinar');
INSERT INTO `_countries` VALUES ('JP', 'Japan', 'Asia','AS', 'Tokyo', 'Yen');
INSERT INTO `_countries` VALUES ('KE', 'Kenya', 'Africa','AF', 'Nairobi', 'Kenyan shilling');
INSERT INTO `_countries` VALUES ('KG', 'Kyrgyzstan', 'Europe','EU', 'Bishkek', 'Som');
INSERT INTO `_countries` VALUES ('KH', 'Cambodia', 'Southeast Asia','ASE', 'Phnom Penh', 'Riel');
INSERT INTO `_countries` VALUES ('KI', 'Kiribati', 'Oceania','ANT', 'Tarawa', 'Australian dollar');
INSERT INTO `_countries` VALUES ('KM', 'Comoros', 'Africa','AF', 'Moroni', 'Comoro Franc');
INSERT INTO `_countries` VALUES ('KN', 'Saint Kitts and Nevis', 'Central America-Caribbean','CA', 'Basseterre', 'East Caribbean Dollar');
INSERT INTO `_countries` VALUES ('KP', 'Korea, North', 'Asia','AS', 'Pyongyang', 'North Korean Won');
INSERT INTO `_countries` VALUES ('KR', 'Korea, South', 'Asia','AS', 'Seoul', 'Won');
INSERT INTO `_countries` VALUES ('KW', 'Kuwait', 'Middle East','ARB', 'Kuwait', 'Kuwaiti Dinar');
INSERT INTO `_countries` VALUES ('KY', 'Cayman Islands', 'Central America-Caribbean','CA', 'George Town', 'Cayman Islands Dollar');
INSERT INTO `_countries` VALUES ('KZ', 'Kazakhstan', 'Europe','EU', 'Astana', 'Tenge');
INSERT INTO `_countries` VALUES ('LA', 'Laos', 'Southeast Asia','ASE', 'Vientiane', 'Kip');
INSERT INTO `_countries` VALUES ('LB', 'Lebanon', 'Middle East','ARB', 'Beirut', 'Lebanese Pound');
INSERT INTO `_countries` VALUES ('LC', 'Saint Lucia', 'Central America-Caribbean','CA', 'Castries', 'East Caribbean Dollar');
INSERT INTO `_countries` VALUES ('LI', 'Liechtenstein', 'Europe','EU', 'Vaduz', 'Swiss Franc');
INSERT INTO `_countries` VALUES ('LK', 'Sri Lanka', 'Asia','AS', 'Colombo', 'Sri Lanka Rupee');
INSERT INTO `_countries` VALUES ('LR', 'Liberia', 'Africa','AF', 'Monrovia', 'Liberian Dollar');
INSERT INTO `_countries` VALUES ('LS', 'Lesotho', 'Africa','AF', 'Maseru', 'Loti');
INSERT INTO `_countries` VALUES ('LT', 'Lithuania', 'Europe','EU', 'Vilnius', 'Lithuanian Litas');
INSERT INTO `_countries` VALUES ('LU', 'Luxembourg', 'Europe','EU', 'Luxembourg', 'Euro');
INSERT INTO `_countries` VALUES ('LV', 'Latvia', 'Europe','EU', 'Riga', 'Latvian Lats');
INSERT INTO `_countries` VALUES ('LY', 'Libya', 'Africa','AF', 'Tripoli', 'Libyan Dinar');
INSERT INTO `_countries` VALUES ('MA', 'Morocco', 'Africa','AF', 'Rabat', 'Moroccan Dirham');
INSERT INTO `_countries` VALUES ('MC', 'Monaco', 'Europe','EU', 'Monaco', 'Euro');
INSERT INTO `_countries` VALUES ('MD', 'Moldova', 'Europe','EU', 'Chisinau', 'Moldovan Leu');
INSERT INTO `_countries` VALUES ('MG', 'Madagascar', 'Africa','AF', 'Antananarivo', 'Malagasy Franc');
INSERT INTO `_countries` VALUES ('MH', 'Marshall Islands', 'Oceania','ANT', 'Majuro', 'US dollar');
INSERT INTO `_countries` VALUES ('MK', 'Macedonia, The Former Yugoslav Republic of', 'Europe','EU', 'Skopje', 'Denar');
INSERT INTO `_countries` VALUES ('ML', 'Mali', 'Africa','AF', 'Bamako', 'CFA Franc BCEAO');
INSERT INTO `_countries` VALUES ('MM', 'Burma', 'Southeast Asia','ASE', 'Rangoon', 'kyat');
INSERT INTO `_countries` VALUES ('MN', 'Mongolia', 'Asia','AS', 'Ulaanbaatar', 'Tugrik');
INSERT INTO `_countries` VALUES ('MO', 'Macao', 'Southeast Asia','ASE', 'Macao', 'Pataca');
INSERT INTO `_countries` VALUES ('MP', 'Northern Mariana Islands', 'Oceania','ANT', 'Saipan', 'US Dollar');
INSERT INTO `_countries` VALUES ('MQ', 'Martinique', 'Central America-Caribbean','CA', 'Fort-de-France', 'Euro');
INSERT INTO `_countries` VALUES ('MR', 'Mauritania', 'Africa','AF', 'Nouakchott', 'Ouguiya');
INSERT INTO `_countries` VALUES ('MS', 'Montserrat', 'Central America-Caribbean','CA', 'Plymouth', 'East Caribbean Dollar');
INSERT INTO `_countries` VALUES ('MT', 'Malta', 'Europe','EU', 'Valletta', 'Maltese Lira');
INSERT INTO `_countries` VALUES ('MU', 'Mauritius', 'World','ANY', 'Port Louis', 'Mauritius Rupee');
INSERT INTO `_countries` VALUES ('MV', 'Maldives', 'Asia','AS', 'Male', 'Rufiyaa');
INSERT INTO `_countries` VALUES ('MW', 'Malawi', 'Africa','AF', 'Lilongwe', 'Kwacha');
INSERT INTO `_countries` VALUES ('MX', 'Mexico', 'North America','NA', 'Mexico', 'Mexican Peso');
INSERT INTO `_countries` VALUES ('MY', 'Malaysia', 'Southeast Asia','ASE', 'Kuala Lumpur', 'Malaysian Ringgit');
INSERT INTO `_countries` VALUES ('MZ', 'Mozambique', 'Africa','AF', 'Maputo', 'Metical');
INSERT INTO `_countries` VALUES ('NA', 'Namibia', 'Africa','AF', 'Windhoek', 'Namibian Dollar');
INSERT INTO `_countries` VALUES ('NC', 'New Caledonia', 'Oceania','ANT', 'Noumea', 'CFP Franc');
INSERT INTO `_countries` VALUES ('NE', 'Niger', 'Africa','AF', 'Niamey', 'CFA Franc BCEAO');
INSERT INTO `_countries` VALUES ('NF', 'Norfolk Island', 'Oceania','ANT', 'Kingston', 'Australian Dollar');
INSERT INTO `_countries` VALUES ('NG', 'Nigeria', 'Africa','AF', 'Abuja', 'Naira');
INSERT INTO `_countries` VALUES ('NI', 'Nicaragua', 'Central America-Caribbean','CA', 'Managua', 'Cordoba Oro');
INSERT INTO `_countries` VALUES ('NL', 'Netherlands', 'Europe','EU', 'Amsterdam', 'Euro');
INSERT INTO `_countries` VALUES ('NO', 'Norway', 'Europe','EU', 'Oslo', 'Norwegian Krone');
INSERT INTO `_countries` VALUES ('NP', 'Nepal', 'Asia','AS', 'Kathmandu', 'Nepalese Rupee');
INSERT INTO `_countries` VALUES ('NR', 'Nauru', 'Oceania','ANT', '--', 'Australian Dollar');
INSERT INTO `_countries` VALUES ('NU', 'Niue', 'Oceania','ANT', 'Alofi', 'New Zealand Dollar');
INSERT INTO `_countries` VALUES ('NZ', 'New Zealand', 'Oceania','ANT', 'Wellington', 'New Zealand Dollar');
INSERT INTO `_countries` VALUES ('OM', 'Oman', 'Middle East','ARB', 'Muscat', 'Rial Omani');
INSERT INTO `_countries` VALUES ('PA', 'Panama', 'Central America-Caribbean','CA', 'Panama', 'balboa');
INSERT INTO `_countries` VALUES ('PE', 'Peru', 'South America','SA', 'Lima', 'Nuevo Sol');
INSERT INTO `_countries` VALUES ('PF', 'French Polynesia', 'Oceania','ANT', 'Papeete', 'CFP Franc');
INSERT INTO `_countries` VALUES ('PG', 'Papua New Guinea', 'Oceania','ANT', 'Port Moresby', 'Kina');
INSERT INTO `_countries` VALUES ('PH', 'Philippines', 'Southeast Asia','ASE', 'Manila', 'Philippine Peso');
INSERT INTO `_countries` VALUES ('PK', 'Pakistan', 'Asia','AS', 'Islamabad', 'Pakistan Rupee');
INSERT INTO `_countries` VALUES ('PL', 'Poland', 'Europe','EU', 'Warsaw', 'Zloty');
INSERT INTO `_countries` VALUES ('PM', 'Saint Pierre and Miquelon', 'North America','NA', 'Saint-Pierre', 'Euro');
INSERT INTO `_countries` VALUES ('PN', 'Pitcairn Islands', 'Oceania','ANT', 'Adamstown', 'New Zealand Dollar');
INSERT INTO `_countries` VALUES ('PR', 'Puerto Rico', 'Central America-Caribbean','CA', 'San Juan', 'US dollar');
INSERT INTO `_countries` VALUES ('PS', 'Palestinian Territory, Occupied', '', '--', '');
INSERT INTO `_countries` VALUES ('PT', 'Portugal', 'Europe','EU', 'Lisbon', 'Euro');
INSERT INTO `_countries` VALUES ('PW', 'Palau', 'Oceania','ANT', 'Koror', 'US dollar');
INSERT INTO `_countries` VALUES ('PY', 'Paraguay', 'South America','SA', 'Asuncion', 'Guarani');
INSERT INTO `_countries` VALUES ('QA', 'Qatar', 'Middle East','ARB', 'Doha', 'Qatari Rial');
INSERT INTO `_countries` VALUES ('RE', 'Réunion', 'World','ANY', 'Saint-Denis', 'Euro');
INSERT INTO `_countries` VALUES ('RO', 'Romania', 'Europe','EU', 'Bucharest', 'Leu');
INSERT INTO `_countries` VALUES ('RU', 'Russia', 'Asia','AS', 'Moscow', 'Russian Ruble');
INSERT INTO `_countries` VALUES ('RW', 'Rwanda', 'Africa','AF', 'Kigali', 'Rwanda Franc');
INSERT INTO `_countries` VALUES ('SA', 'Saudi Arabia', 'Middle East','ARB', 'Riyadh', 'Saudi Riyal');
INSERT INTO `_countries` VALUES ('SB', 'Solomon Islands', 'Oceania','ANT', 'Honiara', 'Solomon Islands Dollar');
INSERT INTO `_countries` VALUES ('SC', 'Seychelles', 'Africa','AF', 'Victoria', 'Seychelles Rupee');
INSERT INTO `_countries` VALUES ('SD', 'Sudan', 'Africa','AF', 'Khartoum', 'Sudanese Dinar');
INSERT INTO `_countries` VALUES ('SE', 'Sweden', 'Europe','EU', 'Stockholm', 'Swedish Krona');
INSERT INTO `_countries` VALUES ('SG', 'Singapore', 'Southeast Asia','ASE', 'Singapore', 'Singapore Dollar');
INSERT INTO `_countries` VALUES ('SH', 'Saint Helena', 'Africa','AF', 'Jamestown', 'Saint Helenian Pound');
INSERT INTO `_countries` VALUES ('SI', 'Slovenia', 'Europe','EU', 'Ljubljana', 'Tolar');
INSERT INTO `_countries` VALUES ('SJ', 'Svalbard', 'Arctic Region','ANT', 'Longyearbyen', 'Norwegian Krone');
INSERT INTO `_countries` VALUES ('SK', 'Slovakia', 'Europe','EU', 'Bratislava', 'Slovak Koruna');
INSERT INTO `_countries` VALUES ('SL', 'Sierra Leone', 'Africa','AF', 'Freetown', 'Leone');
INSERT INTO `_countries` VALUES ('SM', 'San Marino', 'Europe','EU', 'San Marino', 'Euro');
INSERT INTO `_countries` VALUES ('SN', 'Senegal', 'Africa','AF', 'Dakar', 'CFA Franc BCEAO');
INSERT INTO `_countries` VALUES ('SO', 'Somalia', 'Africa','AF', 'Mogadishu', 'Somali Shilling');
INSERT INTO `_countries` VALUES ('SR', 'Suriname', 'South America','SA', 'Paramaribo', 'Suriname Guilder');
INSERT INTO `_countries` VALUES ('ST', 'São Tom?and Príncipe', 'Africa','AF', 'Sao Tome', 'Dobra');
INSERT INTO `_countries` VALUES ('SV', 'El Salvador', 'Central America-Caribbean','CA', 'San Salvador', 'El Salvador Colon');
INSERT INTO `_countries` VALUES ('SY', 'Syria', 'Middle East','ARB', 'Damascus', 'Syrian Pound');
INSERT INTO `_countries` VALUES ('SZ', 'Swaziland', 'Africa','AF', 'Mbabane', 'Lilangeni');
INSERT INTO `_countries` VALUES ('TC', 'Turks and Caicos Islands', 'Central America-Caribbean','CA', 'Cockburn Town', 'US Dollar');
INSERT INTO `_countries` VALUES ('TD', 'Chad', 'Africa','AF', 'NDjamena', 'CFA Franc BEAC');
INSERT INTO `_countries` VALUES ('TF', 'French Southern and Antarctic Lands', 'Antarctic Region','ANT', '--', 'Euro');
INSERT INTO `_countries` VALUES ('TG', 'Togo', 'Africa','AF', 'Lome', 'CFA Franc BCEAO');
INSERT INTO `_countries` VALUES ('TH', 'Thailand', 'Southeast Asia','ASE', 'Bangkok', 'Baht');
INSERT INTO `_countries` VALUES ('TJ', 'Tajikistan', 'Europe','EU', 'Dushanbe', 'Somoni');
INSERT INTO `_countries` VALUES ('TK', 'Tokelau', 'Oceania','ANT', '--', 'New Zealand Dollar');
INSERT INTO `_countries` VALUES ('TM', 'Turkmenistan', 'Europe','EU', 'Ashgabat', 'Manat');
INSERT INTO `_countries` VALUES ('TN', 'Tunisia', 'Africa','AF', 'Tunis', 'Tunisian Dinar');
INSERT INTO `_countries` VALUES ('TO', 'Tonga', 'Oceania','ANT', 'Nukualofa', 'Paanga');
INSERT INTO `_countries` VALUES ('TP', 'East Timor', '', '--', 'Timor Escudo');
INSERT INTO `_countries` VALUES ('TR', 'Turkey', 'Middle East','ARB', 'Ankara', 'Turkish Lira');
INSERT INTO `_countries` VALUES ('TT', 'Trinidad and Tobago', 'Central America-Caribbean','CA', 'Port-of-Spain', 'Trinidad and Tobago Dollar');
INSERT INTO `_countries` VALUES ('TV', 'Tuvalu', 'Oceania','ANT', 'Funafuti', 'Australian Dollar');
INSERT INTO `_countries` VALUES ('TW', 'Taiwan', 'Southeast Asia','ASE', 'Taipei', 'New Taiwan Dollar');
INSERT INTO `_countries` VALUES ('TZ', 'Tanzania', 'Africa','AF', 'Dar es Salaam', 'Tanzanian Shilling');
INSERT INTO `_countries` VALUES ('UA', 'Ukraine', 'Europe','EU', 'Kiev', 'Hryvnia');
INSERT INTO `_countries` VALUES ('UG', 'Uganda', 'Africa','AF', 'Kampala', 'Uganda Shilling');
INSERT INTO `_countries` VALUES ('UK', 'United Kingdom', 'Europe','EU', 'London', 'Pound Sterling');
INSERT INTO `_countries` VALUES ('UM', 'United States Minor Outlying Islands', '', '--', 'US Dollar');
INSERT INTO `_countries` VALUES ('US', 'United States', 'North America','NA', 'Washington, DC', 'US Dollar');
INSERT INTO `_countries` VALUES ('UY', 'Uruguay', 'South America','SA', 'Montevideo', 'Peso Uruguayo');
INSERT INTO `_countries` VALUES ('UZ', 'Uzbekistan', 'Europe','EU', 'Tashkent', 'Uzbekistan Sum');
INSERT INTO `_countries` VALUES ('VA', 'Holy See (Vatican City)', 'Europe','EU', 'Vatican City', 'Euro');
INSERT INTO `_countries` VALUES ('VC', 'Saint Vincent and the Grenadines', 'Central America-Caribbean','CA', 'Kingstown', 'East Caribbean Dollar');
INSERT INTO `_countries` VALUES ('VE', 'Venezuela', 'South America, Central America-Caribbean', 'Caracas', 'Bolivar');
INSERT INTO `_countries` VALUES ('VG', 'British Virgin Islands', 'Central America-Caribbean','CA', 'Road Town', 'US dollar');
INSERT INTO `_countries` VALUES ('VI', 'Virgin Islands', 'Central America-Caribbean','CA', 'Charlotte Amalie', 'US Dollar');
INSERT INTO `_countries` VALUES ('VN', 'Vietnam', 'Southeast Asia','ASE', 'Hanoi', 'Dong');
INSERT INTO `_countries` VALUES ('VU', 'Vanuatu', 'Oceania','ANT', 'Port-Vila', 'Vatu');
INSERT INTO `_countries` VALUES ('WF', 'Wallis and Futuna', 'Oceania','ANT', 'Mata-Utu', 'CFP Franc');
INSERT INTO `_countries` VALUES ('WS', 'Samoa', 'Oceania','ANT', 'Apia', 'Tala');
INSERT INTO `_countries` VALUES ('YE', 'Yemen', 'Middle East','ARB', 'Sanaa', 'Yemeni Rial');
INSERT INTO `_countries` VALUES ('YT', 'Mayotte', 'Africa','AF', 'Mamoutzou', 'Euro');
INSERT INTO `_countries` VALUES ('YU', 'Yugoslavia', 'Europe','EU', 'Belgrade', 'Yugoslavian Dinar');
INSERT INTO `_countries` VALUES ('ZA', 'South Africa', 'Africa','AF', 'Pretoria', 'Rand');
INSERT INTO `_countries` VALUES ('ZM', 'Zambia', 'Africa','AF', 'Lusaka', 'Kwacha');
INSERT INTO `_countries` VALUES ('ZW', 'Zimbabwe', 'Africa','AF', 'Harare', 'Zimbabwe Dollar');
INSERT INTO `_countries` VALUES ('PX', 'Proxy Server', 'Internet','ANY' ,'', '');
INSERT INTO `_countries` VALUES ('EU', 'European Union', 'Europe','EU', 'none', 'Euro');
