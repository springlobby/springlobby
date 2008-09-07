PROJECT(SpringLobby)

#set minimum cmake version
cmake_minimum_required(VERSION 2.4)
SET(CMAKE_COLOR_MAKEFILE ON)
# Add Definitions, Compiler-Switches, etc.: -Wall -O2 -g3 ...
ADD_DEFINITIONS(-Wall -Wno-strict-aliasing )

#----------------------------------------------------------------------------------------------------
# General Settings
#----------------------------------------------------------------------------------------------------
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )
SET(CMAKE_FIND_LIBRARY_PREFIXES "")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".a")

#----------------------------------------------------------------------------------------------------
# Load needed Modules
#----------------------------------------------------------------------------------------------------
# PKG-Config
FIND_PACKAGE( PkgConfig )
INCLUDE(cmake/package_config.cmake)
INCLUDE(CPack)
INCLUDE(cmake/FindSDL.cmake)
INCLUDE(cmake/FindSDL_sound.cmake)
INCLUDE(cmake/FindSDL_mixer.cmake)



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
	1 CACHE BOOL
	"Enables translation support to the programs and adds facilities for helping translators (requires GNU Gettext)" )

#----------------------------------------------------------------------------------------------------
# wxWidgets lib dependency check
#----------------------------------------------------------------------------------------------------

# Here you can define, what Libraries of wxWidgets you need for your Application. You can figure out what Libraries you need here:
# http://www.wxwidgets.org/manuals/2.8/wx_librarieslist.html
SET(wxWidgets_USE_LIBS base core gl net adv qa richtext )

# We need the Find package for wxWidgets to work
IF (NOT WIN32)
    INCLUDE(FindwxWidgets)
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
ELSE (NOT WIN32)
    INCLUDE_DIRECTORIES( ${wxWidgets_INCLUDE_DIRS} )
    SET(wxWidgets_LIBRARIES wx_baseu-2.8-i586-mingw32msvc wx_mswu_core-2.8-i586-mingw32msvc wx_baseu_net-2.8-i586-mingw32msvc wx_mswu_adv-2.8-i586-mingw32msvc wx_mswu_qa-2.8-i586-mingw32msvc wx_mswu_richtext-2.8-i586-mingw32msvc wx_mswu_aui-2.8-i586-mingw32msvc )
    link_directories( ${wxWidgets_LIB_DIR} )
ENDIF(NOT WIN32)

#----------------------------------------------------------------------------------------------------
# SDL libs dependency check
#----------------------------------------------------------------------------------------------------
IF (NOT WIN32)
    FIND_PACKAGE(SDL REQUIRED)
    FIND_PACKAGE(SDL_mixer REQUIRED)
    FIND_PACKAGE(SDL_sound REQUIRED)
    
    # Throw an error to the user if SDL is not found
    if ( NOT SDL_FOUND AND OPTION_SOUND )
        message ( FATAL_ERROR "SDL library was not found! Please install the package or toggle OPTION_SOUND to OFF" )
    endif( NOT SDL_FOUND AND OPTION_SOUND )
    
    # Throw an error to the user if SDL_mixer is not found
    if ( NOT SDLMIXER_FOUND AND OPTION_SOUND )
        message ( FATAL_ERROR "SDL_mixer library was not found! Please install the package or toggle OPTION_SOUND to OFF" )
    endif(NOT SDLMIXER_FOUND AND OPTION_SOUND)
    
    # Throw an error to the user if SDL_sound is not found
    if ( NOT SDL_SOUND_FOUND AND OPTION_SOUND )
        message ( FATAL_ERROR "SDL_sound library was not found! Please install the package or toggle OPTION_SOUND to OFF" )
    endif( NOT SDL_SOUND_FOUND AND OPTION_SOUND )
    
    if ( OPTION_SOUND )
            INCLUDE_DIRECTORIES( ${SDL_SOUND_INCLUDE_DIR} )
            LINK_LIBRARIES( ${SDL_SOUND_LIBRARIES} ${SDLMIXER_LIBRARY}  )	
    else ( OPTION_SOUND )
            ADD_DEFINITIONS( -DDISABLE_SOUND )
    ENDIF( OPTION_SOUND )
ELSE(NOT WIN32)
    IF( OPTION_SOUND )
         INCLUDE_DIRECTORIES( ${sdl_INCLUDE_DIR} )
         LINK_LIBRARIES(SDLmain SDL SDL_mixer)
    ENDIF( OPTION_SOUND )
ENDIF (NOT WIN32)

#----------------------------------------------------------------------------------------------------
# Gettext lib dependency check
#----------------------------------------------------------------------------------------------------

if( OPTION_TRANSLATION_SUPPORT )
    set(Gettext_DIR "po" )
    INCLUDE(cmake/FindGettext.cmake)
    FIND_PACKAGE( Gettext )
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
                    MESSAGE(FATAL_ERROR "libtorrent-rasterbar library is not found! Please install the package or toggle OPTION_USE_TORRENT_SYSTEM to OFF")
            ENDIF()
        ELSE( NOT WIN32)
            INCLUDE_DIRECTORIES(${SpringLobby_SOURCE_DIR}/src/libtorrent/include 
                                ${SpringLobby_SOURCE_DIR}/src/libtorrent/zlib
                                ${SpringLobby_SOURCE_DIR}/src/libtorrent/ )
            ADD_DEFINITIONS( -D_WIN32_WINNT=0x0501 -DBOOST_WINDOWS -DTORRENT_DISABLE_ENCRYPTION )
            LINK_LIBRARIES( boost_thread-mt boost_filesystem-mt  ws2_32 mswsock boost_date_time-mt )
            link_directories( ${boost_LIB_DIR} )
        ENDIF( NOT WIN32)
ELSE( OPTION_TORRENT_SYSTEM )
	ADD_DEFINITIONS( -DNO_TORRENT_SYSTEM )
ENDIF( OPTION_TORRENT_SYSTEM )

# boost and others needed on win
IF ( WIN32 )
    INCLUDE_DIRECTORIES( ${boost_INCLUDE_DIR} )
ENDIF ( WIN32 )

#----------------------------------------------------------------------------------------------------
# Source listing
#----------------------------------------------------------------------------------------------------

# We define the include paths here, our minimal source dir is one

INCLUDE_DIRECTORIES(${SpringLobby_SOURCE_DIR} ${Settings_SOURCE_DIR} )

SET(SpringLobbySrc
	src/boost/md5.cpp
	src/updater/exedownloader.cpp
	src/updater/updater.cpp
	src/updater/versionchecker.cpp
	src/Helper/TextCompletionDatabase.cpp
	src/Helper/wxtextctrlhist.cpp
	src/Helper/colorbutton.cpp
	src/filelister/filelistctrl.cpp
	src/filelister/filelistdialog.cpp
	src/filelister/filelistfilter.cpp
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
        src/spinctld.cpp
)

# If we build for windows Systems, we also include the Resource-File containing the Manifest, Icon and other Resources.
IF(WIN32)
    FIND_PROGRAM(WINDRES NAMES windres i586-mingw32msvc-windres i686-mingw32-windres DOC "path to mingw's windres executable")
    
    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/sl_icon.o COMMAND ${WINDRES} -I${SpringLobby_SOURCE_DIR}/src -I${wxWidgets_RC_DIR} -i${SpringLobby_SOURCE_DIR}/src/springlobby.rc -o ${CMAKE_CURRENT_BINARY_DIR}/sl_icon.o)
    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ss_icon.o COMMAND ${WINDRES} -I${SpringLobby_SOURCE_DIR}/src -I${wxWidgets_RC_DIR} -i${SpringLobby_SOURCE_DIR}/src/settings++/settings.rc -o ${CMAKE_CURRENT_BINARY_DIR}/ss_icon.o)
    
    
    SET(SpringLobby_RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/sl_icon.o )
    SET(SpringSettings_RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/ss_icon.o )
    SET(SpringLobbySrc ${SpringLobbySrc} src/Helper/listctrl.cpp)
    SET(StandAloneSettings ${StandAloneSettings} src/settings++/settings.rc)
ENDIF(WIN32)

#----------------------------------------------------------------------------------------------------
# L10n support
#----------------------------------------------------------------------------------------------------

if( OPTION_TRANSLATION_SUPPORT )
    set ( GETTEXT_XGETTEXT_EXECUTABLE "/usr/bin/xgettext" )
  kwwidgets_create_gettext_targets(
    DOMAIN_NAME "springlobby"
    TARGET_BASENAME "springlobby"
    LOCALE_LIST "da;de;fi;fr;it;ro;ru;sv;uk"
    COPYRIGHT_HOLDER "The SpringLobby team"
    SOURCES "${SpringLobbySrc};${SettingsSrc};${StandAloneSettings}"
    PO_DIR "${CMAKE_SOURCE_DIR}/po"
    POT_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/po"
    PO_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/po"
    MO_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/locale"
    CREATE_POT_TARGET 1
    CREATE_PO_TARGET 1
    #ADD_MO_TARGET_TO_ALL 
    )
endif( OPTION_TRANSLATION_SUPPORT )

#----------------------------------------------------------------------------------------------------
# Build target defintions
#----------------------------------------------------------------------------------------------------

# Here we define the executable SpringLobby ( or on Windows SpringLobby.exe )
IF (WIN32)
    INCLUDE ( ${SpringLobby_SOURCE_DIR}/cmake/libtorrent.cmake )
ENDIF (WIN32)    
   
#libtsource is empty if "NOT WIN32"
ADD_EXECUTABLE(springlobby WIN32 ${SpringLobbySrc} ${SettingsSrc} ${SpringLobby_RC_FILE} ${libtorrent_SRC})
ADD_EXECUTABLE(springsettings WIN32 ${StandAloneSettings} ${SpringSettings_RC_FILE} ${SettingsSrc} ${SLSharedWithSettings} )


# Here the wxWidgets Libraries are added. These are set for us by the FIND Script. If you need other Libraries, you can add them here as well.
TARGET_LINK_LIBRARIES(springlobby ${wxWidgets_LIBRARIES}  )
TARGET_LINK_LIBRARIES(springsettings ${wxWidgets_LIBRARIES}  )

IF (WIN32)
    install(TARGETS springlobby springsettings RUNTIME DESTINATION . )
    install(FILES AUTHORS COPYING NEWS README THANKS DESTINATION . )
    install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/locale DESTINATION . )
ELSE (WIN32)
    install(TARGETS springlobby springsettings RUNTIME DESTINATION bin )
    #install( DIRECTORY locale DESTINATION locale )
    #install( FILES CMakeLists.txt config.h AUTHORS COPYING NEWS README THANKS DESTINATION . )
ENDIF (WIN32)