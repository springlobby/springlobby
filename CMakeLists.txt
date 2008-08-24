PROJECT(SpringLobby)

#set minimum cmake version
cmake_minimum_required(VERSION 2.4)

# Add Definitions, Compiler-Switches, etc.: -Wall -O2 -g3 ...
ADD_DEFINITIONS(-Wall)

#----------------------------------------------------------------------------------------------------
# General Settings
#----------------------------------------------------------------------------------------------------
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )
SET(CMAKE_FIND_LIBRARY_PREFIXES "")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")

#----------------------------------------------------------------------------------------------------
# Load needed Modules
#----------------------------------------------------------------------------------------------------
# PKG-Config
FIND_PACKAGE( PkgConfig )
INCLUDE(CPack)
INCLUDE(FindSDL)
INCLUDE(FindSDL_sound)
INCLUDE(FindSDL_mixer)
INCLUDE(FindwxWidgets)
INCLUDE(FindGettext)

#----------------------------------------------------------------------------------------------------
# Options, that can be changed be the User in order to customise SpringLobby
#----------------------------------------------------------------------------------------------------
SET( OPTION_TORRENT_SYSTEM
	1 CACHE BOOL
	"Enables the builtin map/mod p2p downloader (requires libtorrent-rasterbar library)" )
SET( OPTION_SOUND
	0 CACHE BOOL
	"Enables sound notification functionality (requires SDL and SDL_sound libraries)" )
SET( OPTION_TRANSLATION_SUPPORT
	1 CACHE BOOL
	"Enables translation support to the programs and adds facilities for helping translators (requires GNU Gettext)" )

#----------------------------------------------------------------------------------------------------
# wxWidgets lib dependency check
#----------------------------------------------------------------------------------------------------

# Here you can define, what Libraries of wxWidgets you need for your Application. You can figure out what Libraries you need here:
# http://www.wxwidgets.org/manuals/2.8/wx_librarieslist.html
SET(wxWidgets_USE_LIBS base core gl net adv qa richtext aui )

# We need the Find package for wxWidgets to work
FIND_PACKAGE(wxWidgets)

# Did we find wxWidgets ? This condition will fail for as long as the internal Vars do not point to the proper wxWidgets Configuration.
IF(wxWidgets_FOUND)
	# Include wxWidgets macros
	INCLUDE(${wxWidgets_USE_FILE})
	INCLUDE_DIRECTORIES( ${wxWidgets_INCLUDE_DIRS} )
ELSE(wxWidgets_FOUND)
	# For Convenience. If we cannot continue, inform the User.
	MESSAGE( FATAL_ERROR "wxWidgets library is not found! Please install the package to continue")
ENDIF(wxWidgets_FOUND)

#----------------------------------------------------------------------------------------------------
# SDL libs dependency check
#----------------------------------------------------------------------------------------------------

FIND_PACKAGE(SDL)
FIND_PACKAGE(SDL_mixer)
FIND_PACKAGE(SDL_sound)

# Throw an error to the user if SDL is not found
if ( NOT SDL_FOUND AND OPTION_SOUND )
   message ( FATAL_ERROR "SDL library is not found! Please install the package or toggle OPTION_SOUND to OFF" )
endif()

# Throw an error to the user if SDL_mixer is not found
if ( NOT SDL_MIXER_FOUND AND OPTION_NO_SOUND )
   message ( FATAL_ERROR "SDL_mixer library is not found! Please install the package or toggle OPTION_SOUND to OFF" )
endif()

# Throw an error to the user if SDL_sound is not found
if ( NOT SDL_SOUND_FOUND AND OPTION_NO_SOUND )
   message ( FATAL_ERROR "SDL_sound library is not found! Please install the package or toggle OPTION_SOUND to OFF" )
endif()

if ( OPTION_SOUND )
	INCLUDE_DIRECTORIES( ${SDLSOUND_INCLUDE_DIRS} )
	LINK_LIBRARIES( ${SDLSOUND_LIBRARY} )
else ( OPTION_SOUND )
	ADD_DEFINITIONS( -DDISABLE_SOUND )
ENDIF()

#----------------------------------------------------------------------------------------------------
# Gettext lib dependency check
#----------------------------------------------------------------------------------------------------

if( OPTION_TRANSLATION_SUPPORT )
	FIND_PACKAGE( Gettext )
  if(GETTEXT_FOUND)
  	INCLUDE_DIRECTORIES( ${GETTEXT_INCLUDE_DIR} )
  	include("cmake/KWWidgetsInternationalizationMacros.cmake")
  else(GETTEXT_FOUND)
    message(FATAL_ERROR "The GNU Gettext package is not found! Please install the package or toggle OPTION_TRANSLATION_SUPPORT to OFF")
  endif(GETTEXT_FOUND)
endif( OPTION_TRANSLATION_SUPPORT )

#----------------------------------------------------------------------------------------------------
# libtorrent-rasterbar dependency check
#----------------------------------------------------------------------------------------------------

IF( OPTION_TORRENT_SYSTEM )
	SET ( LIBTORRENT_MIN_VERSION 0.13 )
	pkg_check_modules( LIBTORRENT libtorrent-rasterbar>=${LIBTORRENT_MIN_VERSION} )
	IF( LIBTORRENT_FOUND )
		INCLUDE_DIRECTORIES( ${LIBTORRENT_INCLUDE_DIRS} )
		LINK_LIBRARIES( ${LIBTORRENT_LIBRARIES} )
	ELSE()
		MESSAGE(FATAL_ERROR "libtorrent-rasterbar library is not found! Please install the package or toggle OPTION_USE_TORRENT_SYSTEM to OFF")
	ENDIF()
ELSE( OPTION_TORRENT_SYSTEM )
	ADD_DEFINITIONS( -DNO_TORRENT_SYSTEM )
ENDIF( OPTION_TORRENT_SYSTEM )

#----------------------------------------------------------------------------------------------------
# Source listing
#----------------------------------------------------------------------------------------------------

# We define the include paths here, our minimal source dir is one

INCLUDE_DIRECTORIES(${SpringLobbySrc_SOURCE_DIR} ${SettingsSrc_SOURCE_DIR} )

SET(SpringLobbySrc
	src/boost/md5.cpp
	src/updater/versionchecker.cpp
	src/Helper/TextCompletionDatabase.cpp
  src/Helper/colorbutton.cpp
  src/Helper/wxtextctrlhist.cpp
	src/filelister/filelistctrl.cpp
	src/filelister/filelistdialog.cpp
	src/filelister/filelistfilter.cpp
	src/auimanager.cpp
	src/autobalancedialog.cpp
	src/autojoinchanneldialog.cpp
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
	src/globalevents.cpp
  src/groupuserdialog.cpp
	src/httpdownloader.cpp
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
	src/managegroupspanel.cpp
	src/mapctrl.cpp
	src/maplistctrl.cpp
	src/mapselectdialog.cpp
	src/mmoptionmodel.cpp
	src/mmoptionswrapper.cpp
	src/nicklistctrl.cpp
	src/sdlsound.cpp
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
	src/utils.cpp
	src/useractions.cpp
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

# If we build for windows Systems, we also include the Resource-File containing the Manifest, Icon and other Resources.
IF(WIN32)
  SET(SpringLobbySrc ${SpringLobbySrc} src/springlobby.rc)
	SET(StandAloneSettings ${StandAloneSettings} src/settings++/settings.rc)
ENDIF(WIN32)

#----------------------------------------------------------------------------------------------------
# L10n support
#----------------------------------------------------------------------------------------------------

if( OPTION_TRANSLATION_SUPPORT )
  kwwidgets_create_gettext_targets(
    DOMAIN_NAME "SpringLobby"
    LOCALE_LIST "da;de;fi;fr;it;ro;ru;sv;uk"
    COPYRIGHT_HOLDER "The SpringLobby team"
    SOURCES "${SpringLobbySrc};${SettingsSrc};${StandAloneSettings}"
    PO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Resources/po"
    POT_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/Resources/po"
    PO_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/Resources/po"
    MO_INSTALL_DIR "${SpringLobby_INSTALL_DATA_DIR}/locale"
    CREATE_POT_TARGET
    CREATE_PO_TARGET )
endif( OPTION_TRANSLATION_SUPPORT )

#----------------------------------------------------------------------------------------------------
# Build target defintions
#----------------------------------------------------------------------------------------------------

# Here we define the executable SpringLobby ( or on Windows SpringLobby.exe )
ADD_EXECUTABLE(springlobby WIN32 ${SpringLobbySrc} ${SettingsSrc} )

# Here the wxWidgets Libraries are added. These are set for us by the FIND Script. If you need other Libraries, you can add them here as well.
TARGET_LINK_LIBRARIES(springlobby ${wxWidgets_LIBRARIES} png jpeg )

