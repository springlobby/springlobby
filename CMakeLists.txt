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
	MESSAGE( FATAL_ERROR "wxWidgets library not found! Please install the package to continue")
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

		# Throw an error to the user if SDL not found
		if ( NOT SDL_FOUND )
		message ( FATAL_ERROR "SDL library not found! Please install the package or toggle OPTION_SOUND to OFF" )
		endif( NOT SDL_FOUND )

		# Throw an error to the user if SDL_mixer not found
		if ( NOT SDLMIXER_FOUND )
		message ( FATAL_ERROR "SDL_mixer library not found! Please install the package or toggle OPTION_SOUND to OFF" )
		endif(NOT SDLMIXER_FOUND)

		# Throw an error to the user if SDL_sound not found
		if ( NOT SDL_SOUND_FOUND )
		message ( FATAL_ERROR "SDL_sound library not found! Please install the package or toggle OPTION_SOUND to OFF" )
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
	message(FATAL_ERROR "GNU Gettext package not found! Please install the package or toggle OPTION_TRANSLATION_SUPPORT to OFF")
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
		    LINK_LIBRARIES( springlobby ${LIBTORRENT_LIBRARIES} )
	    ELSE()
		    MESSAGE(FATAL_ERROR "libtorrent-rasterbar library not found! Please install the package or toggle OPTION_TORRENT_SYSTEM to OFF")
	    ENDIF()
	ELSE( NOT WIN32)
			link_directories( ${boost_LIB_DIR} )
			INCLUDE_DIRECTORIES( ${boost_INCLUDE_DIR} )
		LINK_LIBRARIES( -mthreads torrent-rasterbar ${Boost_LIBRARIES} ws2_32 mswsock  )
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
	src/updater/updatermainwindow
	src/utils/activitynotice
	src/chatpanelmenu
	src/customizations
	src/gui/gradientpanel
	src/gui/wxgradientbutton
	src/gui/skirmish_dialog
	src/gui/wxbackgroundimage
	src/gui/simplefront
	src/updater/updatermainwindow
	src/utils/activitynotice
	src/aui/slbook
	src/utils/math
	src/utils/misc
	src/utils/debug
	src/utils/platform
	src/utils/conversion
	src/utils/tasutil
	src/utils/sltipwin
	src/utils/controls
	src/utils/globalevents
	src/utils/networkevents
	src/utils/uievents
	src/utils/md5.c
	src/updater/updater
	src/updater/versionchecker
	src/Helper/TextCompletionDatabase
	src/Helper/imageviewer
	src/Helper/slhtmlwindow
	src/channel/channelchooser
	src/channel/channelchooserdialog
	src/Helper/wxtextctrlhist
	src/Helper/colorbutton
	src/filelister/filelistctrl
	src/filelister/filelistdialog
	src/filelister/filelistfilter
	src/aui/auimanager
	src/aui/artprovider
	src/autobalancedialog
	src/autohost
	src/channel/autojoinchanneldialog
	src/addbotdialog
	src/agreementdialog
	src/base64
	src/battle
	src/battlelist
	src/battlelistctrl
	src/battlelistfilter
	src/battlelisttab
	src/battlemaptab
	src/battleoptionstab
	src/battleroomlistctrl
	src/battleroomtab
	src/crc
	src/channel/channel
	src/channel/channellist
	src/channel/channellistctrl
	src/chatlog
	src/chatoptionstab
	src/chatpanel
	src/connectwindow
	src/countrycodes
	src/crashreport
	src/customlistctrl
	src/flagimages
	src/groupoptionspanel
	src/hostbattledialog
	src/ibattle
	src/iconimagelist
	src/lobbyoptionstab
	src/mainchattab
	src/mainjoinbattletab
	src/mainoptionstab
	src/mainsingleplayertab
	src/maintorrenttab
	src/mainwindow
	src/mapctrl
	src/mapgridctrl
	src/mapselectdialog
	src/mmoptionwindows
	src/mmoptionmodel
	src/mmoptionswrapper
	src/nicklistctrl
	src/offlinebattle
	src/playback/replaylist
	src/playback/savegamelist
	src/sdlsound
	src/selectusersdialog
	src/server
	src/serverevents
	src/settings
	src/singleplayerbattle
	src/singleplayertab
	src/socket
	src/spinctld
	src/spring
	src/springlobbyapp
	src/springoptionstab
	src/springprocess
	src/springunitsync
	src/springunitsynclib
	src/tasserver
	src/tdfcontainer
	src/thread
	src/torrentlistctrl
	src/torrentoptionspanel
	src/torrentwrapper
	src/ui
	src/uiutils
	src/user
	src/useractions
	src/userlist
	src/userlistctrl
	src/widgets/downloadlistctrl
	src/widgets/downloaddialog
	src/widgets/downloadpanel
	src/widgets/infopanel
	src/widgets/widget
	src/globalsmanager
	src/Helper/wxTranslationHelper
	src/Helper/tasclientimport
)

SET(SettingsSrc
	src/utils/customdialogs
	src/settings++/frame
	src/settings++/helpmenufunctions
	src/settings++/panel_pathoption
	src/settings++/se_utils
	src/settings++/tab_abstract
	src/settings++/tab_audio
	src/settings++/tab_quality_video
	src/settings++/tab_render_detail
	src/settings++/tab_simple
	src/settings++/tab_ui
)

SET(StandAloneSettings
	src/settings
	src/uiutils
	src/settings++/Main
)

SET(SLSharedWithSettings
	src/utils/debug
	src/utils/platform
	src/utils/conversion
	src/utils/sltipwin
	src/utils/controls
	src/updater/versionchecker
	src/crashreport
	src/springunitsynclib
	src/springunitsync
	src/thread
	src/spinctld
	src/globalsmanager
	src/utils/globalevents
	src/mmoptionmodel
	src/mmoptionswrapper
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

	SET(SpringLobbySrc ${SpringLobbySrc} src/Helper/listctrl)
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

# Here the wxWidgets Libraries are added. These are set for us by the FIND Script. If you need other Libraries, you can add them here as well.
TARGET_LINK_LIBRARIES(springlobby ${wxWidgets_LIBRARIES})
TARGET_LINK_LIBRARIES(springsettings ${wxWidgets_LIBRARIES})
IF(MINGW)
	TARGET_LINK_LIBRARIES( springlobby iphlpapi )
	TARGET_LINK_LIBRARIES( springsettings iphlpapi  )
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
