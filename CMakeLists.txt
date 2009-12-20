PROJECT(springlobby)

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
INCLUDE(cmake/package_config.cmake)
INCLUDE(cmake/sources.list)
# PKG-Config
FIND_PACKAGE( PkgConfig )
IF(CMAKE_CROSSCOMPILING OR NOT WIN32)
	INCLUDE(cmake/FindSDL.cmake)
	INCLUDE(cmake/FindSDL_sound.cmake)
	INCLUDE(cmake/FindSDL_mixer.cmake)
ENDIF(CMAKE_CROSSCOMPILING OR NOT WIN32)

EXECUTE_PROCESS(COMMAND ${springlobby_SOURCE_DIR}/tools/get-revision.sh
        OUTPUT_VARIABLE SPRINGLOBBY_GIT_REV
        RESULT_VARIABLE GIT_ERROR
        OUTPUT_STRIP_TRAILING_WHITESPACE)


#----------------------------------------------------------------------------------------------------
# Options, that can be changed be the User in order to customise springlobby
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
SET( BUILD_SHARED_LIBS
	1 CACHE BOOL
	"Chooses whether to link dynamic or static libraries. Recommend keeping this activated unless you know what you're doing." )

SET( ENABLE_DEBUG_REPORT
	0 CACHE BOOL
	"Enable automatic uploading of crash information (requires libcurl)" )

SET( SPRINGLOBBY_REV
	${SPRINGLOBBY_GIT_REV} CACHE STRING
	"The revision string used in packages etc." )
SET( LOCALE_INSTALL_DIR
	${CMAKE_INSTALL_PREFIX}/share/locale CACHE STRING
	"message catalogs will installed here" )


#----------------------------------------------------------------------------------------------------
# wxWidgets lib dependency check
#----------------------------------------------------------------------------------------------------

# Here you can define, what Libraries of wxWidgets you need for your Application. You can figure out what Libraries you need here:
# http://www.wxwidgets.org/manuals/2.8/wx_librarieslist.html
IF(NOT CMAKE_CROSSCOMPILING)
SET(wxWidgets_USE_LIBS base core net adv aui html xml qa)

# We need the Find package for wxWidgets to work
FIND_PACKAGE(wxWidgets REQUIRED base core net adv aui html xml qa)

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
IF ( CMAKE_CROSSCOMPILING )
	ADD_DEFINITIONS( -DDISABLE_SOUND )
	SET( OPTION_SOUND
	0 FORCE-CACHE BOOL )
ELSE ( CMAKE_CROSSCOMPILING )
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
			LINK_DIRECTORIES( ${sdl_LIB_DIR} )
			LINK_LIBRARIES(SDLmain SDL SDL_mixer)
		ENDIF(NOT CMAKE_CROSSCOMPILING)
    ENDIF(NOT WIN32)

ELSE ( OPTION_SOUND )
    # Disable sound.
    ADD_DEFINITIONS( -DDISABLE_SOUND )
ENDIF ( OPTION_SOUND )
ENDIF( CMAKE_CROSSCOMPILING )

IF( ENABLE_DEBUG_REPORT )
	ADD_DEFINITIONS( -DENABLE_DEBUG_REPORT )
	IF( MINGW )
		ADD_DEFINITIONS( -DENABLE_DEBUG_REPORT -gstabs )
		LINK_LIBRARIES( bfd intl iberty psapi imagehlp)
	ENDIF( MINGW )
ENDIF( ENABLE_DEBUG_REPORT )

pkg_check_modules( CURL REQUIRED libcurl )
IF( NOT CURL_FOUND )
	MESSAGE( FATAL_ERROR "required library libCURL not found" )
ELSE( NOT CURL_FOUND )
	INCLUDE_DIRECTORIES( ${CURL_INCLUDE_DIR} )
	IF(WIN32)
		ADD_DEFINITIONS( ${CURL_CFLAGS}  )
		LINK_DIRECTORIES( ${CURL_STATIC_LIBRARY_DIRS} )
		LINK_LIBRARIES( ${CURL_STATIC_LDFLAGS} )
	ELSE(WIN32)
		ADD_DEFINITIONS( ${CURL_CFLAGS} )
		LINK_DIRECTORIES( ${CURL_LIB_DIRS} )
		LINK_LIBRARIES( ${CURL_LDFLAGS} )
	ENDIF(WIN32)
ENDIF( NOT CURL_FOUND )
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
		    MESSAGE( FATAL_ERROR "libtorrent-rasterbar library not found! Please install the package or toggle OPTION_TORRENT_SYSTEM to OFF")
	    ENDIF()
	ELSE( NOT WIN32)
		link_directories( ${boost_LIB_DIR} )
		INCLUDE_DIRECTORIES( ${boost_INCLUDE_DIR} )
		ADD_DEFINITIONS( -mthreads  )
		LINK_LIBRARIES( -mthreads torrent-rasterbar ${Boost_LIBRARIES} z ws2_32 mswsock  )
	ENDIF( NOT WIN32)
ELSE( OPTION_TORRENT_SYSTEM )
	ADD_DEFINITIONS( -DNO_TORRENT_SYSTEM )
ENDIF( OPTION_TORRENT_SYSTEM )


# If we build for windows Systems, we also include the Resource-File containing the Manifest, Icon and other Resources.
IF(WIN32)
	IF(MINGW OR CMAKE_CROSSCOMPILING)
		FIND_PROGRAM(WINDRES NAMES i586-pc-mingw32-windres windres i686-mingw32-windres DOC "path to mingw's windres executable")
		ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/sl_icon.o COMMAND ${WINDRES} -I${springlobby_SOURCE_DIR}/src -I${wxWidgets_RC_DIR} -i${springlobby_SOURCE_DIR}/src/springlobby.rc -o ${CMAKE_CURRENT_BINARY_DIR}/sl_icon.o)
		ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ss_icon.o COMMAND ${WINDRES} -I${springlobby_SOURCE_DIR}/src -I${wxWidgets_RC_DIR} -i${springlobby_SOURCE_DIR}/src/settings++/settings.rc -o ${CMAKE_CURRENT_BINARY_DIR}/ss_icon.o)
		ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/su_icon.o COMMAND ${WINDRES} -I${springlobby_SOURCE_DIR}/src -I${wxWidgets_RC_DIR} -i${springlobby_SOURCE_DIR}/src/updater/updater.rc -o ${CMAKE_CURRENT_BINARY_DIR}/su_icon.o)
		SET(springlobby_RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/sl_icon.o )
		SET(SpringSettings_RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/ss_icon.o )
		SET(updater_RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/su_icon.o )
	ENDIF(MINGW OR CMAKE_CROSSCOMPILING)

	SET(springlobbySrc ${springlobbySrc} src/Helper/listctrl.cpp)
	SET(springlobbySrc ${springlobbySrc} src/springlobby.rc)
	SET(updaterSrc ${updaterSrc} src/updater/updater.rc)
	SET(StandAloneSettings ${StandAloneSettings} src/settings++/settings.rc)
ELSE(WIN32)
# If we don't build for windows, include some source code checks as pre-build step.
    ADD_CUSTOM_TARGET(test_susynclib ALL COMMAND tools/test-susynclib.awk src/springunitsynclib.cpp WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ENDIF(WIN32)

#----------------------------------------------------------------------------------------------------
# L10n support
#----------------------------------------------------------------------------------------------------
if( OPTION_TRANSLATION_SUPPORT )
	include ( ${CMAKE_SOURCE_DIR}/po/CMakeLists.txt )
endif( OPTION_TRANSLATION_SUPPORT )
ADD_DEFINITIONS( -DLOCALE_INSTALL_DIR="${LOCALE_INSTALL_DIR}" )

#----------------------------------------------------------------------------------------------------
# Build target defintions
#----------------------------------------------------------------------------------------------------

# Here we define the executable springlobby ( or on Windows springlobby.exe )

#auto-remove whitespaces before/after lib paths
cmake_policy(SET CMP0004 OLD)
#ignore warnings about macosx app bundle output dir
cmake_policy(SET CMP0006 OLD)

ADD_EXECUTABLE(springlobby WIN32 MACOSX_BUNDLE ${springlobbySrc} ${SettingsSrc} ${SLSharedWithSettings} ${curlSRC} ${springlobby_RC_FILE})
ADD_EXECUTABLE(springsettings WIN32 MACOSX_BUNDLE ${StandAloneSettings} ${SettingsSrc} ${SLSharedWithSettings} ${curlSRC} ${SpringSettings_RC_FILE} )
#the updater needs to be build for windows only
IF(WIN32) 
       ADD_EXECUTABLE(springlobby_updater WIN32 MACOSX_BUNDLE ${updaterSrc} ${SLSharedWithSettings} ${curlSRC} ${updater_RC_FILE} )
       TARGET_LINK_LIBRARIES(springlobby_updater ${wxWidgets_LIBRARIES} )
ENDIF(WIN32)

# Here the wxWidgets Libraries are added. These are set for us by the FIND Script. If you need other Libraries, you can add them here as well.
TARGET_LINK_LIBRARIES(springlobby ${wxWidgets_LIBRARIES} )
TARGET_LINK_LIBRARIES(springsettings ${wxWidgets_LIBRARIES} )

IF(MINGW)
	TARGET_LINK_LIBRARIES( springlobby iphlpapi )
	TARGET_LINK_LIBRARIES( springsettings iphlpapi  )
ENDIF(MINGW)

IF (WIN32)
    install(TARGETS springlobby springlobby_updater springsettings RUNTIME DESTINATION .)
    install(FILES AUTHORS COPYING NEWS README THANKS  DESTINATION .)
	install(DIRECTORY ${CMAKE_INSTALL_PREFIX}/locale DESTINATION .)
ELSE (WIN32)
    install(TARGETS springlobby springsettings RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
    install(FILES AUTHORS COPYING NEWS README THANKS DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/springlobby )
    install(FILES src/images/springlobby.svg DESTINATION
	    ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps)
    install(FILES src/springlobby.desktop DESTINATION
	    ${CMAKE_INSTALL_PREFIX}/share/applications)
ENDIF (WIN32)

add_custom_target(pack
					${CMAKE_MAKE_PROGRAM} package
					&& ${springlobby_SOURCE_DIR}/tools/transform_release_zip.sh
						${CMAKE_BINARY_DIR}/${PACKAGE_NAME}-${PACKAGE_VERSION}-win32.zip
						${CMAKE_STRIP}
				)

# setup make dist
add_custom_target(dist cd ${CMAKE_SOURCE_DIR} &&
                        git archive --format=tar --prefix=${PACKAGE_NAME}-${PACKAGE_VERSION}/ HEAD |
                            gzip > ${CMAKE_BINARY_DIR}/${PACKAGE_NAME}-${PACKAGE_VERSION}.tar.gz &&
						git archive --format=tar --prefix=${PACKAGE_NAME}-${PACKAGE_VERSION}/ HEAD |
                            bzip2 > ${CMAKE_BINARY_DIR}/${PACKAGE_NAME}-${PACKAGE_VERSION}.tar.bz2
					 )

# setup make distcheck
add_custom_target(distcheck + cd ${CMAKE_BINARY_DIR} &&
                        rm -rf ${PACKAGE_NAME}-${PACKAGE_VERSION} &&
                        tar -xzf ${PACKAGE_NAME}-${PACKAGE_VERSION}.tar.gz &&
                        cd ${PACKAGE_NAME}-${PACKAGE_VERSION}/ &&
                        cmake "-DCMAKE_CXX_FLAGS:STRING=-O0". && ${CMAKE_MAKE_PROGRAM}
					)
add_dependencies( distcheck dist )
