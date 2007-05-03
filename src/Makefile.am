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

bin_PROGRAMS = springlobby

springlobby_SOURCES = \
	main.cpp     \
	base64.cpp     \
	consolecontrol.cpp     \
	md5.c     \
	protocol.cpp     \
	socket.cpp     \
	tasprotocol.cpp     \
	uicontrol.cpp     \
	mainwindow.cpp     \
	mainwindow.h     \
	mainchatwindow.cpp     \
	mainchatwindow.h     \
	chatwindow.cpp     \
	chatwindow.h 

springlobby_LDFLAGS = 

springlobby_LDADD = \
	$(PACKAGE_LIBS) \
	$(WX_LIBS)

