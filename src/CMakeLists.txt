ADD_SUBDIRECTORY( boost )
ADD_SUBDIRECTORY( settings++ )

SET(SRCS
	addbotdialog.cpp
	agreementdialog.cpp
	base64.cpp
	battle.cpp
	battlelist.cpp
	battlelistctrl.cpp
	battlelistfilter.cpp
	battlelisttab.cpp
	battlemaptab.cpp
	battleoptionstab.cpp
	battleroomlistctrl.cpp
	battleroommmoptionstab.cpp
	battleroomtab.cpp
	channel.cpp
	channellist.cpp
	chatlog.cpp
	chatpanel.cpp
	connectwindow.cpp
	customlistctrl.cpp
	flagimages.cpp
	hostbattledialog.cpp
	ibattle.cpp
	iconimagelist.cpp
	mainchattab.cpp
	mainjoinbattletab.cpp
	mainoptionstab.cpp
	mainsingleplayertab.cpp
	mainwindow.cpp
	mapctrl.cpp
	maplistctrl.cpp
	mapselectdialog.cpp
	mmoptionmodel.cpp
	mmoptionswrapper.cpp
	nicklistctrl.cpp
	nonautotools.cpp
	server.cpp
	serverevents.cpp
	settings.cpp
	singleplayerbattle.cpp
	singleplayertab.cpp
	socket.cpp
	spinctld.cpp
	spring.cpp
	springlobbyapp.cpp
	springoptionstab.cpp
	springprocess.cpp
	springunitsync.cpp
	springunitsynclib.cpp
	stacktrace.cpp
	tasserver.cpp
	ui.cpp
	uiutils.cpp
	unitsyncthread.cpp
	user.cpp
	userlist.cpp
	utils.cpp
)

INCLUDE_DIRECTORIES( boost settings++ )
LINK_DIRECTORIES( boost settings++ )

# INCLUDE_DIRECTORIES( boost settings++ )
# LINK_DIRECTORIES( boost settings++ )

# Make sure the compiler can find include files from our Hello library.
# include_directories (${HELLO_SOURCE_DIR}/Hello)

# Make sure the linker can find the Hello library once it is built.
# link_directories (${HELLO_BINARY_DIR}/Hello)

# Include_Directories(Libs boost_LIBS)
# Link_Directories(Helfer Behaelter Netzwerk Logik Grafik)

    # If we build for windows Systems, we also include the Resource-File containing the Manifest, Icon and other Resources.
    IF(WIN32)
        SET(SRCS ${SRCS} springlobby.rc)
    ENDIF(WIN32)

    # Here we define the executable SpringLobby ( or on Windows SpringLobby.exe )
    ADD_EXECUTABLE(springlobby WIN32 ${SRCS})

		# Here the wxWidgets Libraries are added. These are set for us by the FIND Script. If you need other Libraries, you can add them here as well.
    TARGET_LINK_LIBRARIES(springlobby ${wxWidgets_LIBRARIES} Boost_SpringLobby Settings++ )


#need to link to some other libraries ? just add them here
#TARGET_LINK_LIBRARIES(SpringLobby png jpeg)
 
