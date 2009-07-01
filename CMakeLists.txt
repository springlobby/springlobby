PROJECT(SpringLobby)

#set minimum cmake version
cmake_minimum_required(VERSION 2.6)
SET(CMAKE_COLOR_MAKEFILE ON)

# Add Definitions, Compiler-Switches, etc.: -Wall -O2 -g3 ...
# MSVC compiler (cl.exe) does not accept the same switches as gcc, although preprocessor definitions in the -D form will work for both
IF(NOT MSVC)
	ADD_DEFINITIONS(-Wall -Wno-strict-aliasing)
ENDIF(NOT MSVC)
IF(WIN32)
	ADD_DEFINITIONS( -DWXUSINGDLL -D__WXMSW__ -D_WIN32_WINNT=0x0501)
ENDIF(WIN32)
IF(MSVC)
	ADD_DEFINITIONS(-D_RC_MSVC)
ENDIF(MSVC)





#----------------------------------------------------------------------------------------------------
# General Settings
#----------------------------------------------------------------------------------------------------
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".lib" ".a")

#----------------------------------------------------------------------------------------------------
# Load needed Modules
#----------------------------------------------------------------------------------------------------
# PKG-Config
FIND_PACKAGE( PkgConfig )
INCLUDE(cmake/package_config.cmake)
INCLUDE(CPack)
IF(CMAKE_CROSSCOMPILING OR NOT WIN32)
	INCLUDE(cmake/FindSDL.cmake)
	INCLUDE(cmake/FindSDL_sound.cmake)
	INCLUDE(cmake/FindSDL_mixer.cmake)
ENDIF(CMAKE_CROSSCOMPILING OR NOT WIN32)


#----------------------------------------------------------------------------------------------------
# Options, that can be changed be the User in order to customise SpringLobby
#----------------------------------------------------------------------------------------------------
SET( OPTION_TORRENT_SYSTEM
	1 CACHE BOOL
	"Enables the builtin map/mod p2p downloader (requires libtorrent-rasterbar library)" )
SET( OPTION_SOUND
	1 CACHE BOOL
	"Enables sound notification functionality (requires SDL and SDL_sound libraries)" )
SET( OPTION_TRANSLATION_SUPPORT
	0 CACHE BOOL
	"Enables translation support to the programs and adds facilities for helping translators (requires GNU Gettext)" )
SET( BUILD_SHARED_LIBS
	1 CACHE BOOL
	"Chooses whether to link dynamic or static libraries. Recommend keeping this activated unless you know what you're doing." )


#----------------------------------------------------------------------------------------------------
# wxWidgets lib dependency check
#----------------------------------------------------------------------------------------------------

# Here you can define, what Libraries of wxWidgets you need for your Application. You can figure out what Libraries you need here:
# http://www.wxwidgets.org/manuals/2.8/wx_librarieslist.html
IF(NOT CMAKE_CROSSCOMPILING)
SET(wxWidgets_USE_LIBS base core net adv aui html xml )

# We need the Find package for wxWidgets to work
FIND_PACKAGE(wxWidgets REQUIRED base core net adv aui html xml)

# Did we find wxWidgets ? This condition will fail for as long as the internal Vars do not point to the proper wxWidgets Configuration.
IF(wxWidgets_FOUND)
	# Include wxWidgets macros
	INCLUDE(${wxWidgets_USE_FILE})
	IF(MSVC)
		INCLUDE_DIRECTORIES( ${wxWidgets_ROOT_DIR}/include/msvc )
	ENDIF(MSVC)
	IF(MINGW)
		SET(wxWidgets_RC_DIR ${wxWidgets_ROOT_DIR}/include)
	ENDIF(MINGW)
	INCLUDE_DIRECTORIES( ${wxWidgets_INCLUDE_DIRS} )
ELSE(wxWidgets_FOUND)
	# For Convenience. If we cannot continue, inform the User.
	MESSAGE( FATAL_ERROR "wxWidgets library is not found! Please install the package to continue")
ENDIF(wxWidgets_FOUND)
ELSE (NOT CMAKE_CROSSCOMPILING)
    INCLUDE_DIRECTORIES( ${wxWidgets_INCLUDE_DIRS} )
    link_directories( ${wxWidgets_LIB_DIR} )
ENDIF(NOT CMAKE_CROSSCOMPILING)

#----------------------------------------------------------------------------------------------------
# SDL libs dependency check
#----------------------------------------------------------------------------------------------------
IF ( OPTION_SOUND )

	IF (CMAKE_CROSSCOMPILING OR NOT WIN32)
		FIND_PACKAGE(SDL REQUIRED)
		FIND_PACKAGE(SDL_mixer REQUIRED)
		FIND_PACKAGE(SDL_sound REQUIRED)

		# Throw an error to the user if SDL is not found
		if ( NOT SDL_FOUND )
		message ( FATAL_ERROR "SDL library was not found! Please install the package or toggle OPTION_SOUND to OFF" )
		endif( NOT SDL_FOUND )

		# Throw an error to the user if SDL_mixer is not found
		if ( NOT SDLMIXER_FOUND )
		message ( FATAL_ERROR "SDL_mixer library was not found! Please install the package or toggle OPTION_SOUND to OFF" )
		endif(NOT SDLMIXER_FOUND)

		# Throw an error to the user if SDL_sound is not found
		if ( NOT SDL_SOUND_FOUND )
		message ( FATAL_ERROR "SDL_sound library was not found! Please install the package or toggle OPTION_SOUND to OFF" )
		endif( NOT SDL_SOUND_FOUND )

		# MESSAGE("sound inc dir: ${SDL_SOUND_INCLUDE_DIR}")
		# MESSAGE("mixer inc dir: ${SDLMIXER_INCLUDE_DIR}")
		# MESSAGE("sound libs: ${SDL_SOUND_LIBRARIES}")
		# MESSAGE("mixer libs: ${SDLMIXER_LIBRARY}")
	ENDIF(CMAKE_CROSSCOMPILING OR NOT WIN32)
	
	IF(NOT WIN32)
		# Set include paths and libraries to link.
		INCLUDE_DIRECTORIES( ${SDL_SOUND_INCLUDE_DIR} ${SDLMIXER_INCLUDE_DIR} )
		LINK_LIBRARIES( # ${SDL_SOUND_LIBRARIES}
			${SDLMIXER_LIBRARY}  )
	ELSE(NOT WIN32)
		SET(SDL_INCLUDE_DIR "SDL_INCLUDE_DIR_NOT_SET" CACHE PATH "Path to SDL header files (sdl_mixer.h should be in the same dir as well)")
		SET(SDLMAIN_LIBRARY "SDLMAIN_LIBRARY_NOT_SET" CACHE FILEPATH "Filepath to SDLmain.lib or libsdlmain.a, depending on whether you're using MSVC or MingW")
		SET(SDL_LIBRARY "SDL_LIBRARY_NOT_SET" CACHE FILEPATH "Filepath to SDL.lib or libsdl.dll.a, depending on whether you're using MSVC or MingW")
		SET(SDLMIXER_LIBRARY "SDLMIXER_LIBRARY_NOT_SET" CACHE FILEPATH "Filepath to SDL_mixer.lib or libSDL_mixer.dll.a, depending on whether you're using MSVC or MingW")
		IF(NOT CMAKE_CROSSCOMPILING)
			INCLUDE_DIRECTORIES( ${SDL_INCLUDE_DIR})
			LINK_LIBRARIES(${SDLMAIN_LIBRARY} ${SDL_LIBRARY} ${SDLMIXER_LIBRARY})
		ELSE(NOT CMAKE_CROSSCOMPILING)
			INCLUDE_DIRECTORIES( ${sdl_INCLUDE_DIR} )
			LINK_LIBRARIES(SDLmain SDL SDL_mixer)
		ENDIF(NOT CMAKE_CROSSCOMPILING)
    ENDIF(NOT WIN32)

ELSE ( OPTION_SOUND )
    # Disable sound.
    ADD_DEFINITIONS( -DDISABLE_SOUND )
ENDIF ( OPTION_SOUND )


#----------------------------------------------------------------------------------------------------
# Gettext lib dependency check
#----------------------------------------------------------------------------------------------------

if( OPTION_TRANSLATION_SUPPORT )
    set(Gettext_DIR "po" )
    INCLUDE(cmake/FindGettext.cmake)
    if(GETTEXT_FOUND)
	    set(SpringLobby_INCLUDE_PATH ${SpringLobby_INCLUDE_PATH}
		${GETTEXT_INCLUDE_DIR} CACHE INTERNAL "" FORCE)

	    include("cmake/KWWidgetsInternationalizationMacros.cmake")
    else(GETTEXT_FOUND)
	message(FATAL_ERROR "The GNU Gettext package is not found! Please install the package or toggle OPTION_TRANSLATION_SUPPORT to OFF")
    endif(GETTEXT_FOUND)
endif( OPTION_TRANSLATION_SUPPORT )

#----------------------------------------------------------------------------------------------------
# libtorrent-rasterbar dependency check
#----------------------------------------------------------------------------------------------------

IF( OPTION_TORRENT_SYSTEM )
	IF( NOT WIN32)
	    SET ( LIBTORRENT_MIN_VERSION 0.13 )
	    pkg_check_modules( LIBTORRENT libtorrent-rasterbar>=${LIBTORRENT_MIN_VERSION} )
	    IF( LIBTORRENT_FOUND )
		    INCLUDE_DIRECTORIES( ${LIBTORRENT_INCLUDE_DIRS} )
		    LINK_LIBRARIES( ${LIBTORRENT_LIBRARIES} )
	    ELSE()
		    MESSAGE(FATAL_ERROR "libtorrent-rasterbar library is not found! Please install the package or toggle OPTION_TORRENT_SYSTEM to OFF")
	    ENDIF()
	ELSE( NOT WIN32)
	    INCLUDE_DIRECTORIES(${SpringLobby_SOURCE_DIR}/src/libtorrent/include
				${SpringLobby_SOURCE_DIR}/src/libtorrent/zlib
				${SpringLobby_SOURCE_DIR}/src/libtorrent/ )
	    INCLUDE ( ${SpringLobby_SOURCE_DIR}/cmake/libtorrent.cmake )
	    ADD_DEFINITIONS( -DBOOST_WINDOWS -DTORRENT_DISABLE_ENCRYPTION )
		IF(NOT CMAKE_CROSSCOMPILING)
			SET(Boost_USE_STATIC_LIBS ON)
			SET(Boost_USE_MULTITHREADED ON)
			SET(Boost_ADDITIONAL_VERSIONS 1.35 1.35.0 1.38 1.38.0)
			FIND_PACKAGE(Boost COMPONENTS system thread filesystem date_time)
			INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )
		ELSE(NOT CMAKE_CROSSCOMPILING)
			link_directories( ${boost_LIB_DIR} )
			INCLUDE_DIRECTORIES( ${boost_INCLUDE_DIR} )
		ENDIF(NOT CMAKE_CROSSCOMPILING)
	ENDIF( NOT WIN32)
ELSE( OPTION_TORRENT_SYSTEM )
	ADD_DEFINITIONS( -DNO_TORRENT_SYSTEM )
ENDIF( OPTION_TORRENT_SYSTEM )


#----------------------------------------------------------------------------------------------------
# Source listing
#----------------------------------------------------------------------------------------------------

# We define the include paths here, our minimal source dir is one

INCLUDE_DIRECTORIES(${SpringLobby_SOURCE_DIR} ${Settings_SOURCE_DIR} )

SET(SpringLobbySrc
	src/utils/math.cpp
	src/utils/misc.cpp
	src/utils/debug.cpp
	src/utils/platform.cpp
	src/utils/conversion.cpp
	src/utils/tasutil.cpp
	src/utils/sltipwin.cpp
	src/utils/controls.cpp
	src/boost/md5.cpp
	src/updater/updater.cpp
	src/updater/versionchecker.cpp
	src/Helper/TextCompletionDatabase.cpp
	src/Helper/imageviewer.cpp
	src/Helper/slhtmlwindow.cpp
	src/channel/channelchooser.cpp
	src/channel/channelchooserdialog.cpp
	src/Helper/wxtextctrlhist.cpp
	src/Helper/colorbutton.cpp
	src/filelister/filelistctrl.cpp
	src/filelister/filelistdialog.cpp
	src/filelister/filelistfilter.cpp
	src/aui/auimanager.cpp
        src/aui/artprovider.cpp
	src/autobalancedialog.cpp
	src/autohost.cpp
	src/channel/autojoinchanneldialog.cpp
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
	src/crc.cpp
	src/channel/channel.cpp
	src/channel/channellist.cpp
	src/channel/channellistctrl.cpp
	src/chatlog.cpp
	src/chatoptionstab.cpp
	src/chatpanel.cpp
	src/connectwindow.cpp
	src/countrycodes.cpp
	src/crashreport.cpp
	src/customlistctrl.cpp
	src/flagimages.cpp
	src/globalevents.cpp
	src/groupoptionspanel.cpp
	src/hostbattledialog.cpp
	src/ibattle.cpp
	src/iconimagelist.cpp
	src/lobbyoptionstab.cpp
	src/mainchattab.cpp
	src/mainjoinbattletab.cpp
	src/mainoptionstab.cpp
	src/mainsingleplayertab.cpp
	src/maintorrenttab.cpp
	src/mainwindow.cpp
	src/mapctrl.cpp
	src/mapgridctrl.cpp
	src/mapselectdialog.cpp
	src/mmoptionwindows.cpp
	src/mmoptionmodel.cpp
	src/mmoptionswrapper.cpp
	src/nicklistctrl.cpp
	src/offlinebattle.cpp
	src/playback/replaylist.cpp
	src/playback/savegamelist.cpp
	src/sdlsound.cpp
	src/selectusersdialog.cpp
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
	src/thread.cpp
	src/torrentlistctrl.cpp
	src/torrentoptionspanel.cpp
	src/torrentwrapper.cpp
	src/ui.cpp
	src/uiutils.cpp
	src/unitsyncthread.cpp
	src/user.cpp
	src/useractions.cpp
	src/userlist.cpp
	src/userlistctrl.cpp
	src/widgets/downloadlistctrl.cpp
	src/widgets/downloaddialog.cpp
	src/widgets/downloadpanel.cpp
	src/widgets/infopanel.cpp
	src/widgets/widget.cpp
	src/globalsmanager.cpp
	src/Helper/wxTranslationHelper.cpp
	src/Helper/tasclientimport.cpp
)

SET(SettingsSrc
	src/utils/debug.cpp
	src/utils/platform.cpp
	src/utils/conversion.cpp
	src/utils/sltipwin.cpp
	src/utils/controls.cpp
	src/settings++/custom_dialogs.cpp
	src/settings++/frame.cpp
	src/settings++/helpmenufunctions.cpp
	src/settings++/panel_pathoption.cpp
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
	src/settings.cpp
	src/uiutils.cpp
	src/settings++/Main.cpp
)

SET(SLSharedWithSettings
	src/updater/versionchecker.cpp
	src/crashreport.cpp
	src/springunitsynclib.cpp
	src/spinctld.cpp
	src/globalsmanager.cpp
)

# If we build for windows Systems, we also include the Resource-File containing the Manifest, Icon and other Resources.
IF(WIN32)
	IF(MINGW OR CMAKE_CROSSCOMPILING)
		FIND_PROGRAM(WINDRES NAMES windres i586-mingw32msvc-windres i686-mingw32-windres DOC "path to mingw's windres executable")
		ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/sl_icon.o COMMAND ${WINDRES} -I${SpringLobby_SOURCE_DIR}/src -I${wxWidgets_RC_DIR} -i${SpringLobby_SOURCE_DIR}/src/springlobby.rc -o ${CMAKE_CURRENT_BINARY_DIR}/sl_icon.o)
		ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ss_icon.o COMMAND ${WINDRES} -I${SpringLobby_SOURCE_DIR}/src -I${wxWidgets_RC_DIR} -i${SpringLobby_SOURCE_DIR}/src/settings++/settings.rc -o ${CMAKE_CURRENT_BINARY_DIR}/ss_icon.o)
		SET(SpringLobby_RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/sl_icon.o )
		SET(SpringSettings_RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/ss_icon.o )
	ENDIF(MINGW OR CMAKE_CROSSCOMPILING)

	SET(SpringLobbySrc ${SpringLobbySrc} src/Helper/listctrl.cpp)
	SET(SpringLobbySrc ${SpringLobbySrc} src/springlobby.rc)
	SET(StandAloneSettings ${StandAloneSettings} src/settings++/settings.rc)
ELSE(WIN32)
# If we don't build for windows, include some source code checks as pre-build step.
    ADD_CUSTOM_TARGET(test_susynclib ALL COMMAND tools/test-susynclib.awk src/springunitsynclib.cpp WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ENDIF(WIN32)

#----------------------------------------------------------------------------------------------------
# L10n support
#----------------------------------------------------------------------------------------------------

if( OPTION_TRANSLATION_SUPPORT )
    set ( GETTEXT_XGETTEXT_EXECUTABLE "/usr/bin/xgettext" )
    kwwidgets_create_gettext_targets(
	    DOMAIN_NAME "springlobby"
	    TARGET_BASENAME "springlobby"
	    LOCALE_LIST "ar;cs;da;de;el;es;fi;fr;it;pl;pt;ro;ru;sv;uk;zh_CN"
	    COPYRIGHT_HOLDER "The SpringLobby team"
	    SOURCES "${SpringLobbySrc};${SettingsSrc};${StandAloneSettings}"
	    PO_DIR "${CMAKE_SOURCE_DIR}/po"
	    PO_PREFIX ""
	    POT_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/po"
	    PO_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/po"
	    MO_INSTALL_DIR "/share/locale"
	    CREATE_POT_TARGET 1
	    CREATE_PO_TARGET 1
	    ADD_MO_TARGET_TO_ALL
    )
endif( OPTION_TRANSLATION_SUPPORT )

#----------------------------------------------------------------------------------------------------
# Build target defintions
#----------------------------------------------------------------------------------------------------

# Here we define the executable SpringLobby ( or on Windows SpringLobby.exe )

#auto-remove whitespaces before/after lib paths
cmake_policy(SET CMP0004 OLD)
#ignore warnings about macosx app bundle output dir
cmake_policy(SET CMP0006 OLD)


ADD_EXECUTABLE(springlobby WIN32 MACOSX_BUNDLE ${SpringLobbySrc} ${SettingsSrc} ${SpringLobby_RC_FILE})
ADD_EXECUTABLE(springsettings WIN32 MACOSX_BUNDLE ${StandAloneSettings} ${SpringSettings_RC_FILE} ${SettingsSrc} ${SLSharedWithSettings} )

#build libtorrent as a static lib to link in on windows
IF(WIN32 AND OPTION_TORRENT_SYSTEM)
ADD_LIBRARY(libtorrent STATIC ${libtorrent_SRC})
SET_TARGET_PROPERTIES(libtorrent PROPERTIES COMPILE_FLAGS -DWIN32_LEAN_AND_MEAN)
TARGET_LINK_LIBRARIES( libtorrent ${Boost_LIBRARIES} ws2_32 mswsock )
TARGET_LINK_LIBRARIES( springlobby libtorrent )
ENDIF(WIN32 AND OPTION_TORRENT_SYSTEM)


# Here the wxWidgets Libraries are added. These are set for us by the FIND Script. If you need other Libraries, you can add them here as well.
TARGET_LINK_LIBRARIES(springlobby ${wxWidgets_LIBRARIES})
TARGET_LINK_LIBRARIES(springsettings ${wxWidgets_LIBRARIES})
IF(MINGW)
	TARGET_LINK_LIBRARIES( springlobby iphlpapi )
	TARGET_LINK_LIBRARIES( springsettings iphlpapi )
ENDIF(MINGW)

IF (WIN32)
    install(TARGETS springlobby springsettings RUNTIME DESTINATION .)
    install(FILES AUTHORS COPYING NEWS README THANKS DESTINATION .)
ELSE (WIN32)
    install(TARGETS springlobby springsettings RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
    install(FILES AUTHORS COPYING NEWS README THANKS DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/springlobby )
    install(FILES src/images/springlobby.svg DESTINATION
	    ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps)
    install(FILES src/springlobby.desktop DESTINATION
	    ${CMAKE_INSTALL_PREFIX}/share/applications)
ENDIF (WIN32)
