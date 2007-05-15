## Process this file with automake to produce Makefile.in

## Created by Anjuta

INCLUDES = \
	$(WX_CXXFLAGS) \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" $(PACKAGE_CFLAGS)

AM_CFLAGS =\
	 -Wall \
	 -g \
	 $(WX_CPPFLAGS)

bin_PROGRAMS = springlobby \
	test 

test_SOURCES = \
	chatlist.cpp\
	chatlist.h                    \
	test.cpp                    \
	settings.cpp                    \
	settings.h 

test_LDADD = \
	$(PACKAGE_LIBS) \
	$(WX_LIBS) 

springlobby_SOURCES = \
	base64.cpp     \
	md5.c     \
	socket.cpp     \
	mainwindow.cpp     \
	mainwindow.h     \
	serverevents.cpp     \
	base64.h     \
	md5.h     \
	serverevents.h     \
	socket.h     \
	chatlist.cpp     \
	chatlist.h     \
	chatpanel.cpp     \
	chatpanel.h     \
	connectwindow.cpp     \
	connectwindow.h     \
	server.cpp     \
	server.h     \
	tasserver.cpp     \
	tasserver.h     \
	mainchattab.cpp     \
	mainchattab.h     \
	settings.cpp     \
	settings.h     \
	springlobbyapp.cpp     \
	springlobbyapp.h     \
	user.cpp     \
	user.h     \
	userlist.cpp     \
	userlist.h 

springlobby_LDFLAGS = 

springlobby_LDADD = \
	$(PACKAGE_LIBS) \
	$(WX_LIBS)

