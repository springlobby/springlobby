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

#if nothing went wrong we have the file and can define HAVE_CONFIG_H
IF ( EXISTS ${springlobby_BINARY_DIR}/springlobby_config.h  OR  EXISTS ${springlobby_SOURCE_DIR}/springlobby_config.h  )
    ADD_DEFINITIONS( -DHAVE_CONFIG_H )
    INCLUDE_DIRECTORIES(${springlobby_BINARY_DIR})
ENDIF ( EXISTS ${springlobby_BINARY_DIR}/springlobby_config.h  OR  EXISTS ${springlobby_SOURCE_DIR}/springlobby_config.h  )


#----------------------------------------------------------------------------------------------------
# Options, that can be changed be the User in order to customise springlobby
#----------------------------------------------------------------------------------------------------
SET( OPTION_SOUND
	0 CACHE BOOL
	"Enables sound notification functionality (requires openAL libraries)" )
SET( OPTION_TRANSLATION_SUPPORT
	1 CACHE BOOL
	"Enables translation support to the programs and adds facilities for helping translators (requires GNU Gettext)" )
SET( OPTION_NOTIFY
	1 CACHE BOOL
	"Enables libnotify support for popup status messages (requires libnotify)" )
SET( BUILD_SHARED_LIBS
	0 CACHE BOOL
	"Chooses whether to link dynamic or static libraries. Recommend keeping this activated unless you know what you're doing." )

SET( ENABLE_DEBUG_REPORT
	0 CACHE BOOL
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

INCLUDE(SpringLobbyMacros)
clear(link_directories)
clear(springlobbySrc)
Add_Subdirectory(doc)
Add_Subdirectory(src)

#cppcheck stuff
find_program( CPPCHECK_BINARY NAMES cppcheck )
IF( EXISTS ${CPPCHECK_BINARY} )
	ADD_CUSTOM_TARGET( cppcheck )
	FOREACH( list ${updaterSrc} ${header} ${SLSharedWithSettings} ${StandAloneSettings} ${curlSRC} ${SettingsSrc} ${springlobbySrc} )
		FOREACH( SOURCEFILE ${list} )
			LIST( APPEND CHECKPATHS ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCEFILE} )
		ENDFOREACH( SOURCEFILE )
	ENDFOREACH( list )

	ADD_CUSTOM_TARGET( cppcheck-full ${CPPCHECK_BINARY} "--xml" "--enable=all" "-f" "--quiet" "-j 3" 
		${CHECKPATHS} "2>${CMAKE_CURRENT_BINARY_DIR}/cppcheck-full.xml" "&&" 
			"${CMAKE_CURRENT_SOURCE_DIR}/tools/cppcheck-htmlreport" "--source-dir=${CMAKE_CURRENT_SOURCE_DIR}/" 
				"--file=${CMAKE_CURRENT_BINARY_DIR}/cppcheck-full.xml" "--report-dir=report-full" "--title=SpringLobby - Full")
	ADD_CUSTOM_TARGET( cppcheck-lite ${CPPCHECK_BINARY} "--xml" "-f" "--quiet" "-j 3" 
		${CHECKPATHS} "2>${CMAKE_CURRENT_BINARY_DIR}/cppcheck-lite.xml" "&&" 
			"${CMAKE_CURRENT_SOURCE_DIR}/tools/cppcheck-htmlreport" "--source-dir=${CMAKE_CURRENT_SOURCE_DIR}/" 
				"--file=${CMAKE_CURRENT_BINARY_DIR}/cppcheck-lite.xml" "--report-dir=report-lite" "--title=SpringLobby - Lite")
	add_dependencies( cppcheck cppcheck-full )
	add_dependencies( cppcheck cppcheck-lite )
ENDIF( EXISTS ${CPPCHECK_BINARY} )

IF( WIN32 )
	LIST( APPEND SLSharedWithSettings stacktrace.c )
ELSE( WIN32 )
	LIST( APPEND SLSharedWithSettings stacktrace.cpp )
ENDIF( WIN32 )


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
add_dependencies( springlobby unitsync-ext )
add_dependencies( springlobby libSpringLobbyUtils )
#add_dependencies( versiontest version )

