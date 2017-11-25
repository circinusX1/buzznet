## Process this file with automake to produce Makefile.in

## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

INCLUDES =\
	 -I../openssl/include

AM_CXXFLAGS =\
	 -DAM_GLIB_GNU_GETTEXT -D_CONSOLE\
	 -Wall\
	 -g

bin_PROGRAMS = buzznet

buzznet_SOURCES = \
	buzznet.cpp\
	buzznet.h\
	connection.cpp\
	connection.h\
	core.cpp\
	core.h\
	huffman.cpp\
	huffman.h\
	ionet.cpp\
	ionet.h\
	osg.h\
	oslinux.h\
	oswindows.h\
	sock.cpp\
	sock.h\
	statistics.cpp\
	statistics.h\
	stdafx.cpp\
	stdafx.h\
	strops.cpp\
	strops.h\
	url.cpp\
	url.h\
	buzznetconf.cpp\
	bzipper.cpp\
	bzipper.h\
	hashvisit.cpp\
	hashvisit.h\
	htmlparser.cpp\
	htmlparser.h\
	httpserver.cpp\
	httpserver.h\
	mymyysql.cpp\
	mymyysql.h\
	pageparser.cpp\
	watchdog.cpp\
	watchdog.h\
	main.cpp\
	pageparser.h\
	dbsto.cpp\
	dbsto.h\
	dbstocreate.cpp\
	filesto.cpp\
	filesto.h\
	storage.cpp\
	storage.h\
	wrkqueues.cpp\
	wrkqueues.h

buzznet_LDFLAGS = \
	 -L/home/marius/develop/BUZZNET/openssl/libcrypto/src/ -L/home/marius/develop/BUZZNET/openssl/libssl/src/ -L/home/marius/develop/BUZZNET/z_lib/src/

buzznet_LDADD = \
	 -llibssl -llibcrypto -lpthread -lz_lib -lmysqlclient

