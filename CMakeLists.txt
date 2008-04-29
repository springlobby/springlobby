PROJECT(SpringLobby)

#if you don't want the full Compiler-Output, remove the following Line
SET(CMAKE_VERBOSE_MAKEFILE ON)

# Add Definitions, Compiler-Switches, etc.: -Wall -O2 -g3 ...
ADD_DEFINITIONS(-Wall -g3)

#----------------------------------------------------------------------------------------------------
# Konfiguration of wxWidgets Configuration
#----------------------------------------------------------------------------------------------------

SET(CMAKE_FIND_LIBRARY_PREFIXES "")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")

# Here you can define, what Libraries of wxWidgets you need for your Application. You can figure out what Libraries you need here;
# http://www.wxwidgets.org/manuals/2.8/wx_librarieslist.html
SET(wxWidgets_USE_LIBS base core gl net adv qa)

# We need the Find package for wxWidgets to work
FIND_PACKAGE(wxWidgets)

# Did we find wxWidgets ? This condition will fail for as long as the internal Vars do not point to the proper wxWidgets Configuration.
IF(wxWidgets_FOUND)

	# Include wxWidgets macros
	INCLUDE(${wxWidgets_USE_FILE})

	# We define the include paths here, our minimal source dir is one,
	# and also the include dirs defined by wxWidgets
	INCLUDE_DIRECTORIES(${SpringLobbySrc_SOURCE_DIR} ${SettingsSrc_SOURCE_DIR}  ${wxWidgets_INCLUDE_DIRS} )
	SET(SpringLobbySrc
		src/boost/md5.cpp
		src/updater/versionchecker.cpp
		src/autobalancedialog.cpp
		src/addbotdialog.cpp
		src/agreementdialog.cpp
		src/base64.cpp
		src/battle.cpp
		src/battlelist.cpp
		src/battlelistctrl.cpp
		src/battlelistfilter.cpp
		src/battlelisttab.cpp
		src/battlemaptab.cpp
		src/battleoptionstab.cpp
		src/battleroomlistctrl.cpp
		src/battleroomtab.cpp
		src/battleroommmoptionstab.cpp
		src/channel.cpp
		src/channellist.cpp
		src/chatlog.cpp
		src/chatoptionstab.cpp
		src/chatpanel.cpp
		src/connectwindow.cpp
		src/countrycodes.cpp
		src/crashreport.cpp
		src/customlistctrl.cpp
		src/flagimages.cpp
		src/hostbattledialog.cpp
		src/ibattle.cpp
		src/iconimagelist.cpp
		src/mainchattab.cpp
		src/mainjoinbattletab.cpp
		src/mainoptionstab.cpp
		src/mainsingleplayertab.cpp
		src/mainwindow.cpp
		src/mapctrl.cpp
		src/maplistctrl.cpp
		src/mapselectdialog.cpp
		src/mmoptionmodel.cpp
		src/mmoptionswrapper.cpp
		src/nicklistctrl.cpp
		src/nonautotools.cpp
		src/server.cpp
		src/serverevents.cpp
		src/settings.cpp
		src/singleplayerbattle.cpp
		src/singleplayertab.cpp
		src/socket.cpp
		src/spinctld.cpp
		src/spring.cpp
		src/springlobbyapp.cpp
		src/springoptionstab.cpp
		src/springprocess.cpp
		src/springunitsync.cpp
		src/springunitsynclib.cpp
		src/tasserver.cpp
		src/tdfcontainer.cpp
		src/ui.cpp
		src/uiutils.cpp
		src/unitsyncthread.cpp
		src/user.cpp
		src/userlist.cpp
		src/utils.cpp
	)
	SET(SettingsSrc
		src/settings++/custom_dialogs.cpp
		src/settings++/frame.cpp
		src/settings++/helpmenufunctions.cpp
		src/settings++/panel_pathoption.cpp
		src/settings++/presets.cpp
		src/settings++/se_settings.cpp
		src/settings++/se_utils.cpp
		src/settings++/tab_abstract.cpp
		src/settings++/tab_audio.cpp
		src/settings++/tab_debug.cpp
		src/settings++/tab_quality_video.cpp
		src/settings++/tab_render_detail.cpp
		src/settings++/tab_simple.cpp
		src/settings++/tab_ui.cpp
	)
	SET(StandAloneSettings
		src/settings++/Main.cpp
	)
	SET(SLSharedWithSettings
		src/updater/versionchecker.cpp
		src/crashreport.cpp
		src/springunitsynclib.cpp
		src/utils.cpp
	)

# Make sure the compiler can find include files from our Hello library.
# include_directories (${HELLO_SOURCE_DIR}/Hello)

# Make sure the linker can find the Hello library once it is built.
# link_directories (${HELLO_BINARY_DIR}/Hello)

# Include_Directories(Libs boost_LIBS)
# Link_Directories(Helfer Behaelter Netzwerk Logik Grafik)

    # If we build for windows Systems, we also include the Resource-File containing the Manifest, Icon and other Resources.
    IF(WIN32)
        SET(SpringLobbySrc ${SpringLobbySrc} src/springlobby.rc)
		SET(StandAloneSettings ${StandAloneSettings} src/settings++/settings.rc)
    ENDIF(WIN32)

    # Here we define the executable SpringLobby ( or on Windows SpringLobby.exe )
    ADD_EXECUTABLE(springlobby WIN32 ${SpringLobbySrc} ${SettingsSrc} )

		# Here the wxWidgets Libraries are added. These are set for us by the FIND Script. If you need other Libraries, you can add them here as well.
    TARGET_LINK_LIBRARIES(springlobby ${wxWidgets_LIBRARIES} )


#need to link to some other libraries ? just add them here
#TARGET_LINK_LIBRARIES(SpringLobby png jpeg)

ELSE(wxWidgets_FOUND)
	# For Convenience. If we cannot continue, inform the User.
	MESSAGE("wxWidgets not found!")
ENDIF(wxWidgets_FOUND)
