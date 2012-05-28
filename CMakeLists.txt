PROJECT(springlobby)

#set minimum cmake version
cmake_minimum_required(VERSION 2.6)
SET(CMAKE_COLOR_MAKEFILE ON)

# Add Definitions, Compiler-Switches, etc.: -Wall -O2 -g3 ...
# MSVC compiler (cl.exe) does not accept the same switches as gcc, although preprocessor definitions in the -D form will work for both
IF(NOT MSVC)
	IF( NOT MINGW )
		#wxMSW generates > 9000 warnings with -wextra...
		ADD_DEFINITIONS( -Wextra )
	ENDIF( NOT MINGW )
	ADD_DEFINITIONS( -Wall -Wno-strict-aliasing )
ELSE(NOT MSVC)
	ADD_DEFINITIONS(-D_RC_MSVC)
ENDIF(NOT MSVC)
IF(WIN32)
	ADD_DEFINITIONS( -DWXUSINGDLL -D__WXMSW__ -D_WIN32_WINNT=0x0501)
ENDIF(WIN32)

#----------------------------------------------------------------------------------------------------
# General Settings
#----------------------------------------------------------------------------------------------------
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )
SET( CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".lib" ".a" )

#----------------------------------------------------------------------------------------------------
# Load needed Modules
#----------------------------------------------------------------------------------------------------
if(NOT DEFINED SPRINGLOBBY_REV)
  EXECUTE_PROCESS(COMMAND ${springlobby_SOURCE_DIR}/tools/get-revision.sh
    OUTPUT_VARIABLE SPRINGLOBBY_REV
    ERROR_VARIABLE GIT_ERROR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif(NOT DEFINED SPRINGLOBBY_REV)

INCLUDE(cmake/package_config.cmake)
INCLUDE(cmake/sources.list)

# PKG-Config
FIND_PACKAGE( PkgConfig )

#if nothing went wrong we have the file and can define HAVE_CONFIG_H
IF ( EXISTS ${springlobby_BINARY_DIR}/springlobby_config.h  OR  EXISTS ${springlobby_SOURCE_DIR}/springlobby_config.h  )
    ADD_DEFINITIONS( -DHAVE_CONFIG_H )
    INCLUDE_DIRECTORIES(${springlobby_BINARY_DIR})
ENDIF ( EXISTS ${springlobby_BINARY_DIR}/springlobby_config.h  OR  EXISTS ${springlobby_SOURCE_DIR}/springlobby_config.h  )


#----------------------------------------------------------------------------------------------------
# Options, that can be changed be the User in order to customise springlobby
#----------------------------------------------------------------------------------------------------
SET( OPTION_TORRENT_SYSTEM
	1 CACHE BOOL
	"Enables the builtin map/mod p2p downloader (requires libtorrent-rasterbar library)" )
SET( OPTION_SOUND
	1 CACHE BOOL
	"Enables sound notification functionality (requires openAL libraries)" )
SET( OPTION_TRANSLATION_SUPPORT
	1 CACHE BOOL
	"Enables translation support to the programs and adds facilities for helping translators (requires GNU Gettext)" )
SET( OPTION_NOTIFY
	1 CACHE BOOL
	"Enables libnotify support for popup status messages (requires libnotify)" )
SET( BUILD_SHARED_LIBS
	1 CACHE BOOL
	"Chooses whether to link dynamic or static libraries. Recommend keeping this activated unless you know what you're doing." )

SET( ENABLE_DEBUG_REPORT
	1 CACHE BOOL
	"Enable automatic uploading of crash information (requires libcurl)" )

SET( SPRINGLOBBY_REV
	"${SPRINGLOBBY_REV}" CACHE STRING
	"The revision string used in packages etc." )
SET( LOCALE_INSTALL_DIR
	${CMAKE_INSTALL_PREFIX}/share/locale CACHE STRING
	"message catalogs will installed here" )

IF( AUX_VERSION )
	ADD_DEFINITIONS( -DAUX_VERSION="${AUX_VERSION}" )
ENDIF( AUX_VERSION )

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
# openAL libs dependency check
#----------------------------------------------------------------------------------------------------

IF ( OPTION_SOUND )

	FIND_PACKAGE(OpenAL REQUIRED)
	INCLUDE_DIRECTORIES(${OPENAL_INCLUDE_DIR})

	Add_Subdirectory( ${springlobby_SOURCE_DIR}/src/sound/alure )
	INCLUDE_DIRECTORIES( ${springlobby_SOURCE_DIR}/src/sound/alure/include )
	LINK_LIBRARIES( ${OPENAL_LIBRARY} ${springlobby_BINARY_DIR}/src/sound/alure/libalure.a )
ELSE ( OPTION_SOUND )
    # Disable sound.
    ADD_DEFINITIONS( -DDISABLE_SOUND )
ENDIF ( OPTION_SOUND )


IF( ENABLE_DEBUG_REPORT )
	ADD_DEFINITIONS( -DENABLE_DEBUG_REPORT )
	IF( MINGW )
		ADD_DEFINITIONS( -gstabs )
		LINK_LIBRARIES( bfd iberty intl drmingw )
	ENDIF( MINGW )
	LIST( APPEND SLSharedWithSettings ${curlSRC} )
ENDIF( ENABLE_DEBUG_REPORT )



IF(WIN32)
	ADD_DEFINITIONS( ${CURL_CFLAGS}  )
	LINK_DIRECTORIES( ${CURL_STATIC_LIBRARY_DIRS} )
	LINK_LIBRARIES( ${CURL_STATIC_LDFLAGS} )
	INCLUDE_DIRECTORIES( ${CURL_INCLUDE_DIR} )
ELSE(WIN32)
	pkg_check_modules( CURL REQUIRED libcurl )
	IF( NOT CURL_FOUND )
		MESSAGE( FATAL_ERROR "required library libCURL not found" )
	ENDIF( NOT CURL_FOUND )
	ADD_DEFINITIONS( ${CURL_CFLAGS} )
	LINK_DIRECTORIES( ${CURL_LIB_DIRS} )
	LINK_LIBRARIES( ${CURL_LDFLAGS} )
	INCLUDE_DIRECTORIES( ${CURL_INCLUDE_DIR} )
ENDIF(WIN32)

#----------------------------------------------------------------------------------------------------
# libnotify dependency check
#----------------------------------------------------------------------------------------------------

IF( OPTION_NOTIFY AND NOT WIN32 )
	pkg_check_modules( LIBNOTIFY libnotify )
	pkg_check_modules( GLIB glib-2.0 )
	IF( LIBNOTIFY_FOUND )
		INCLUDE_DIRECTORIES( ${LIBNOTIFY_INCLUDE_DIRS} )
		LINK_LIBRARIES( ${LIBNOTIFY_LIBRARIES} )
		INCLUDE_DIRECTORIES( ${GLIB_INCLUDE_DIRS} )
		LINK_LIBRARIES( ${GLIB_LIBRARIES} )
		ADD_DEFINITIONS( -DHAVE_LIBNOTIFY )
	ENDIF( LIBNOTIFY_FOUND )
ENDIF( OPTION_NOTIFY AND NOT WIN32 )


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

ADD_EXECUTABLE(springlobby WIN32 MACOSX_BUNDLE ${header} ${springlobbySrc} ${SettingsSrc} ${SLSharedWithSettings} ${springlobby_RC_FILE})
ADD_EXECUTABLE(springsettings WIN32 MACOSX_BUNDLE ${StandAloneSettings} ${SettingsSrc} ${SLSharedWithSettings} ${SpringSettings_RC_FILE} )
#the updater needs to be build for windows only
IF(WIN32)
       ADD_EXECUTABLE(springlobby_updater WIN32 MACOSX_BUNDLE ${updaterSrc} ${SLSharedWithSettings} ${updater_RC_FILE} )
       TARGET_LINK_LIBRARIES(springlobby_updater ${wxWidgets_LIBRARIES} )
ENDIF(WIN32)

#git submodules 
Add_Subdirectory(src/downloader/lib)
INCLUDE_DIRECTORIES(src/downloader/lib/src)
Add_Subdirectory(src/lsl)
INCLUDE_DIRECTORIES(src/lsl/src)

#only enable if debugging is necessary
#ADD_EXECUTABLE(versiontest WIN32 MACOSX_BUNDLE src/versiontest.cpp )

# Here the wxWidgets Libraries are added. These are set for us by the FIND Script. If you need other Libraries, you can add them here as well.
TARGET_LINK_LIBRARIES(springlobby ${wxWidgets_LIBRARIES} ${SL_BOOST_LIBS} "unitsync-ext" "libSpringLobbyUtils" "boost_thread-mt" )
TARGET_LINK_LIBRARIES(springsettings ${wxWidgets_LIBRARIES} ${SL_BOOST_LIBS} "libSpringLobbyUtils" )

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

INCLUDE(cmake/compat_flags.cmake)
INCLUDE_DIRECTORIES( ${springlobby_SOURCE_DIR}/src )

add_custom_target(pack
					${CMAKE_MAKE_PROGRAM} package
					&& ${springlobby_SOURCE_DIR}/tools/transform_release_zip.sh
						${CMAKE_BINARY_DIR}/${PACKAGE_NAME}-${PACKAGE_VERSION}-win32.zip
						${CMAKE_STRIP}
				)

# setup make dist
add_custom_target(dist ${CMAKE_SOURCE_DIR}/tools/make_dist.sh "${springlobby_BINARY_DIR}" "${PACKAGE_NAME}-${PACKAGE_VERSION}"
					 )

# setup make distcheck
add_custom_target(distcheck + cd ${CMAKE_BINARY_DIR} &&
                        rm -rf ${PACKAGE_NAME}-${PACKAGE_VERSION} &&
                        tar -xzf ${PACKAGE_NAME}-${PACKAGE_VERSION}.tar.gz &&
                        cd ${PACKAGE_NAME}-${PACKAGE_VERSION}/ &&
                        cmake "-DCMAKE_CXX_FLAGS:STRING=-O0". && ${CMAKE_MAKE_PROGRAM}
					)
add_custom_target( version
				${springlobby_SOURCE_DIR}/tools/regen_config_header.sh ${springlobby_SOURCE_DIR}/cmake/config.h ${springlobby_BINARY_DIR}/springlobby_config.h
				)
add_dependencies( distcheck dist )
add_dependencies( springlobby alure )
add_dependencies( springlobby version )
add_dependencies( springlobby pr-downloader )
add_dependencies( springlobby libSpringLobbyUtils )
#add_dependencies( versiontest version )

