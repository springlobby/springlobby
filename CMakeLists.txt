PROJECT(springlobby)

#set minimum cmake version
cmake_minimum_required(VERSION 2.8)

CMAKE_POLICY(SET CMP0004 OLD)

SET(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

if (CMAKE_GENERATOR MATCHES "Ninja")
	message(STATUS "Ninja detected, enabling install workarround")
	# fixes "ninja install" error:
	#	  file INSTALL cannot find
	# "/home/abma/Projects/SpringLobby/src/CMakeFiles/CMakeRelink.dir/springlobby".
	set(CMAKE_PLATFORM_HAS_INSTALLNAME 1)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	# workarround for include/c++/4.8/type_traits:268:39: error: use of undeclared identifier '__float128'
	add_definitions("-D__STRICT_ANSI__")
endif()

# check for minimal gcc version
set(MIN_GCC_VER "4.7")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	if(NOT ${CMAKE_CXX_COMPILER_VERSION} STREQUAL "")
		if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "${MIN_GCC_VER}")
			message(FATAL_ERROR "gcc >=${MIN_GCC_VER} required")
		else()
			message(STATUS "gcc ${CMAKE_CXX_COMPILER_VERSION} detected")
		endif()
	else()
		message(WARNING "cmake version <2.8.10 found, couldn't check for gcc ${MIN_GCC_VER}")
	endif()
endif()


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
option( OPTION_TRANSLATION_SUPPORT "Enables translation support to the programs and adds facilities for helping translators (requires GNU Gettext)" TRUE)
option( OPTION_NOTIFY "Enables libnotify support for popup status messages (requires libnotify)" TRUE)
option( BUILD_SHARED_LIBS "Chooses whether to link dynamic or static libraries. Recommend keeping this activated unless you know what you're doing." FALSE)

SET(SPRINGLOBBY_REV "${SPRINGLOBBY_REV}")
if (WIN32)
	SET(LOCALE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/locale")
else()
	SET(LOCALE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/locale")
endif()

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


find_program(CLANG_FORMAT_BINARY NAMES clang-format-3.5 clang-format-3.4 clang-format)
if (EXISTS ${CLANG_FORMAT_BINARY})
	file(GLOB_RECURSE files {CMAKE_CURRENT_SOURCE_DIR} src/*.cpp src/*.hpp src/*.h)

	foreach(exclude src/lsl;src/downloader/lib ) # exclude some files
		foreach(file in ${files})
			if (file MATCHES "^${CMAKE_CURRENT_SOURCE_DIR}/${exclude}")
				list(REMOVE_ITEM files ${file})
			endif()
		endforeach()
	endforeach()
	add_custom_target(format COMMAND ${CLANG_FORMAT_BINARY} -i ${files})
else()
	message(WARNING "clang-format not found")
endif()

IF( WIN32 )
	LIST( APPEND SLSharedWithSettings ${CMAKE_CURRENT_SOURCE_DIR}/src/stacktrace.c )
ELSE( WIN32 )
	LIST( APPEND SLSharedWithSettings ${CMAKE_CURRENT_SOURCE_DIR}/src/stacktrace.cpp )
ENDIF( WIN32 )

INCLUDE(cmake/package_config.cmake)
IF(WIN32)
	install(FILES AUTHORS COPYING README THANKS NEWS DESTINATION ${CMAKE_INSTALL_PREFIX})
ELSE (WIN32)
	install(FILES AUTHORS COPYING README THANKS NEWS DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc/springlobby )
	install(FILES src/images/springlobby.svg DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps)
	install(FILES src/springlobby.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
	install(FILES share/freedesktop.org/springlobby.appdata.xml DESTINATION ${CMAKE_INSTALL_PREFIX}/share/appdata)
ENDIF (WIN32)

add_custom_target(pack ${CMAKE_MAKE_PROGRAM} package
			&& ${springlobby_SOURCE_DIR}/tools/transform_release_zip.sh
			${CMAKE_BINARY_DIR}/${PACKAGE_NAME}-${PACKAGE_VERSION}-win32.zip
			${CMAKE_STRIP})

# setup make dist
add_custom_target(dist ${CMAKE_SOURCE_DIR}/tools/make_dist.sh "${springlobby_BINARY_DIR}" "${PACKAGE_NAME}-${PACKAGE_VERSION}" "${PACKAGE_VERSION}")

# setup make distcheck
add_custom_target(distcheck + cd ${CMAKE_BINARY_DIR} &&
			rm -rf ${PACKAGE_NAME}-${PACKAGE_VERSION} &&
			tar -xzf ${PACKAGE_NAME}-${PACKAGE_VERSION}.tar.gz &&
			cd ${PACKAGE_NAME}-${PACKAGE_VERSION}/ &&
			cmake "-DCMAKE_CXX_FLAGS:STRING=-O0" . && ${CMAKE_MAKE_PROGRAM})
add_dependencies( distcheck dist )
#add_dependencies( versiontest version )

