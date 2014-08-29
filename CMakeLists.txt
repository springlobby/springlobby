PROJECT(springlobby)

#set minimum cmake version
cmake_minimum_required(VERSION 2.8)

#ignore warnings about macosx app bundle output dir
cmake_policy(SET CMP0006 OLD)
#don't ouch (supposedly) relative paths passed to link_directories
cmake_policy(SET CMP0015 OLD)

SET(CMAKE_COLOR_MAKEFILE ON)
SET(CROSS_COMPILING "FALSE")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

#----------------------------------------------------------------------------------------------------
# General Settings
#----------------------------------------------------------------------------------------------------
SET( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )

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

# PKG-Config
FIND_PACKAGE( PkgConfig )


#----------------------------------------------------------------------------------------------------
# Options, that can be changed be the User in order to customise springlobby
#----------------------------------------------------------------------------------------------------
SET( OPTION_TRANSLATION_SUPPORT
	TRUE CACHE BOOL
	"Enables translation support to the programs and adds facilities for helping translators (requires GNU Gettext)" )
SET( OPTION_NOTIFY
	TRUE CACHE BOOL
	"Enables libnotify support for popup status messages (requires libnotify)" )
SET( BUILD_SHARED_LIBS
	FALSE CACHE BOOL
	"Chooses whether to link dynamic or static libraries. Recommend keeping this activated unless you know what you're doing." )

SET(SPRINGLOBBY_REV "${SPRINGLOBBY_REV}")
SET(LOCALE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/locale")

IF( AUX_VERSION )
	ADD_DEFINITIONS( -DAUX_VERSION="${AUX_VERSION}" )
ENDIF( AUX_VERSION )

INCLUDE(SpringLobbyMacros)
Add_Subdirectory(doc)
#----------------------------------------------------------------------------------------------------
# L10n support
#----------------------------------------------------------------------------------------------------
if( OPTION_TRANSLATION_SUPPORT )
        add_subdirectory(po)
endif( OPTION_TRANSLATION_SUPPORT )
ADD_DEFINITIONS( -DLOCALE_INSTALL_DIR="${LOCALE_INSTALL_DIR}" )
Add_Subdirectory(src)

#cppcheck stuff
find_program( CPPCHECK_BINARY NAMES cppcheck )
IF( EXISTS ${CPPCHECK_BINARY} )
	ADD_CUSTOM_TARGET( cppcheck )
	FOREACH( list ${updaterSrc} ${header} ${SLSharedWithSettings} ${StandAloneSettings} ${curlSRC} ${SettingsSrc} ${springlobbySrc} )
		FOREACH( SOURCEFILE ${list} )
			LIST( APPEND CHECKPATHS ${SOURCEFILE} )
		ENDFOREACH( SOURCEFILE )
	ENDFOREACH( list )

	ADD_CUSTOM_TARGET( cppcheck-full ${CPPCHECK_BINARY} "--xml" "--enable=all" "-f" "--quiet"
		${CHECKPATHS} "2>${CMAKE_CURRENT_BINARY_DIR}/cppcheck-full.xml" "&&" 
			"${CMAKE_CURRENT_SOURCE_DIR}/tools/cppcheck-htmlreport" "--source-dir=${CMAKE_CURRENT_SOURCE_DIR}/" 
				"--file=${CMAKE_CURRENT_BINARY_DIR}/cppcheck-full.xml" "--report-dir=report-full" "--title=SpringLobby - Full")
	ADD_CUSTOM_TARGET( cppcheck-lite ${CPPCHECK_BINARY} "--xml" "-f" "--quiet"
		${CHECKPATHS} "2>${CMAKE_CURRENT_BINARY_DIR}/cppcheck-lite.xml" "&&" 
			"${CMAKE_CURRENT_SOURCE_DIR}/tools/cppcheck-htmlreport" "--source-dir=${CMAKE_CURRENT_SOURCE_DIR}/" 
				"--file=${CMAKE_CURRENT_BINARY_DIR}/cppcheck-lite.xml" "--report-dir=report-lite" "--title=SpringLobby - Lite")
	add_dependencies( cppcheck cppcheck-full )
	add_dependencies( cppcheck cppcheck-lite )
ENDIF( EXISTS ${CPPCHECK_BINARY} )

IF( WIN32 )
	LIST( APPEND SLSharedWithSettings ${CMAKE_CURRENT_SOURCE_DIR}/src/stacktrace.c )
ELSE( WIN32 )
	LIST( APPEND SLSharedWithSettings ${CMAKE_CURRENT_SOURCE_DIR}/src/stacktrace.cpp )
ENDIF( WIN32 )

INCLUDE(cmake/package_config.cmake)
IF(WIN32)
    install(FILES AUTHORS COPYING README THANKS NEWS DESTINATION .)
	install(DIRECTORY ${CMAKE_INSTALL_PREFIX}/locale DESTINATION .)
ELSE (WIN32)
    install(FILES AUTHORS COPYING README THANKS NEWS DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/springlobby )
    install(FILES src/images/springlobby.svg DESTINATION
	    ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps)
    install(FILES src/springlobby.desktop DESTINATION
	    ${CMAKE_INSTALL_PREFIX}/share/applications)
    install(FILES share/freedesktop.org/springlobby.appdata.xml DESTINATION ${CMAKE_INSTALL_PREFIX}/share/appdata)
ENDIF (WIN32)

add_custom_target(pack
					${CMAKE_MAKE_PROGRAM} package
					&& ${springlobby_SOURCE_DIR}/tools/transform_release_zip.sh
						${CMAKE_BINARY_DIR}/${PACKAGE_NAME}-${PACKAGE_VERSION}-win32.zip
						${CMAKE_STRIP}
				)

# setup make dist
add_custom_target(dist ${CMAKE_SOURCE_DIR}/tools/make_dist.sh "${springlobby_BINARY_DIR}" "${PACKAGE_NAME}-${PACKAGE_VERSION}" "${PACKAGE_VERSION}"
					 )

# setup make distcheck
add_custom_target(distcheck + cd ${CMAKE_BINARY_DIR} &&
                        rm -rf ${PACKAGE_NAME}-${PACKAGE_VERSION} &&
                        tar -xzf ${PACKAGE_NAME}-${PACKAGE_VERSION}.tar.gz &&
                        cd ${PACKAGE_NAME}-${PACKAGE_VERSION}/ &&
                        cmake "-DCMAKE_CXX_FLAGS:STRING=-O0" . && ${CMAKE_MAKE_PROGRAM}
					)
add_dependencies( distcheck dist )
#add_dependencies( versiontest version )

