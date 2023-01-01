project(springlobby)

#set minimum cmake version
cmake_minimum_required(VERSION 3.1)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if(CMAKE_GENERATOR MATCHES "Ninja")
	message(STATUS "Ninja detected, enabling install workarround")
	# fixes "ninja install" error:
	#	  file INSTALL cannot find
	# "/home/abma/Projects/SpringLobby/src/CMakeFiles/CMakeRelink.dir/springlobby".
	set(CMAKE_PLATFORM_HAS_INSTALLNAME 1)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	# workarround for include/c++/4.8/type_traits:268:39: error: use of undeclared identifier '__float128'
	add_definitions("-D__STRICT_ANSI__")
endif()

# check for minimal gcc version
set(MIN_GCC_VER "8.0")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "${MIN_GCC_VER}")
		message(FATAL_ERROR "gcc >=${MIN_GCC_VER} required")
	else()
		message(STATUS "gcc ${CMAKE_CXX_COMPILER_VERSION} detected")
	endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#----------------------------------------------------------------------------------------------------
# General Settings
#----------------------------------------------------------------------------------------------------
set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )

include(compat_flags)
include(GNUInstallDirs)

#----------------------------------------------------------------------------------------------------
# Load needed Modules
#----------------------------------------------------------------------------------------------------
if(NOT DEFINED SPRINGLOBBY_REV)
	execute_process(COMMAND ${springlobby_SOURCE_DIR}/tools/get-revision.sh
		OUTPUT_VARIABLE SPRINGLOBBY_REV
		ERROR_VARIABLE GIT_ERROR
		OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

# PKG-Config
find_package( PkgConfig )


include(CMakePushCheckState)
include(CheckCXXCompilerFlag)
function(check_linker_flag flag out_var)
  cmake_push_check_state()
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${flag}")
  check_cxx_compiler_flag("" ${out_var})
  cmake_pop_check_state()
endfunction()
check_linker_flag("-Wl,--exclude-libs,ALL" LINKER_HAS_EXCLUDELIBS)
if(LINKER_HAS_EXCLUDELIBS)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--exclude-libs,ALL")
endif()

#----------------------------------------------------------------------------------------------------
# Options, that can be changed be the User in order to customise springlobby
#----------------------------------------------------------------------------------------------------
option(OPTION_TRANSLATION_SUPPORT "Enables translation support to the programs and adds facilities for helping translators (requires GNU Gettext)" TRUE)
option(OPTION_NOTIFY "Enables libnotify support for popup status messages (requires libnotify)" TRUE)
option(BUILD_SHARED_LIBS "Chooses whether to link dynamic or static libraries. Recommend keeping this activated unless you know what you're doing." FALSE)



set(SPRINGLOBBY_REV "${SPRINGLOBBY_REV}")
if(WIN32)
	set(LOCALE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/locale" CACHE STRING "")
else()
	set(LOCALE_INSTALL_DIR "${CMAKE_INSTALL_FULL_DATAROOTDIR}/locale" CACHE STRING "")
endif()
message(STATUS "LOCALE_INSTALL_DIR=${LOCALE_INSTALL_DIR}")

if( AUX_VERSION )
	add_definitions( -DAUX_VERSION="${AUX_VERSION}" )
endif()

include(SpringLobbyMacros)
add_subdirectory(doc)
#----------------------------------------------------------------------------------------------------
# L10n support
#----------------------------------------------------------------------------------------------------
if( OPTION_TRANSLATION_SUPPORT )
        add_subdirectory(po)
endif()
add_definitions( -DLOCALE_INSTALL_DIR="${LOCALE_INSTALL_DIR}" )
add_subdirectory(src)

#cppcheck stuff
find_program( CPPCHECK_BINARY NAMES cppcheck )
if( EXISTS ${CPPCHECK_BINARY} )
	add_custom_target( cppcheck )
	foreach( list ${updaterSrc} ${header} ${SLSharedWithSettings} ${StandAloneSettings} ${curlSRC} ${SettingsSrc} ${springlobbySrc} )
		foreach(SOURCEFILE ${list})
			list(APPEND CHECKPATHS ${SOURCEFILE})
		endforeach()
	endforeach()

	add_custom_target(cppcheck-full ${CPPCHECK_BINARY} "--xml" "--enable=all" "-f" "--quiet"
		${CHECKPATHS} "2>${CMAKE_CURRENT_BINARY_DIR}/cppcheck-full.xml" "&&"
			"${CMAKE_CURRENT_SOURCE_DIR}/tools/cppcheck-htmlreport" "--source-dir=${CMAKE_CURRENT_SOURCE_DIR}/"
				"--file=${CMAKE_CURRENT_BINARY_DIR}/cppcheck-full.xml" "--report-dir=report-full" "--title=SpringLobby - Full")
	add_custom_target(cppcheck-lite ${CPPCHECK_BINARY} "--xml" "-f" "--quiet"
		${CHECKPATHS} "2>${CMAKE_CURRENT_BINARY_DIR}/cppcheck-lite.xml" "&&"
			"${CMAKE_CURRENT_SOURCE_DIR}/tools/cppcheck-htmlreport" "--source-dir=${CMAKE_CURRENT_SOURCE_DIR}/"
				"--file=${CMAKE_CURRENT_BINARY_DIR}/cppcheck-lite.xml" "--report-dir=report-lite" "--title=SpringLobby - Lite")
	add_dependencies(cppcheck cppcheck-full)
	add_dependencies(cppcheck cppcheck-lite)
endif()


find_program(CLANG_FORMAT_BINARY NAMES clang-format-3.5 clang-format-3.4 clang-format)
if(EXISTS ${CLANG_FORMAT_BINARY})
	file(GLOB_RECURSE files {CMAKE_CURRENT_SOURCE_DIR} src/*.cpp src/*.hpp src/*.h)

	foreach(exclude src/lsl;src/downloader/lib) # exclude some files
		foreach(file in ${files})
			if(file MATCHES "^${CMAKE_CURRENT_SOURCE_DIR}/${exclude}")
				list(REMOVE_ITEM files ${file})
			endif()
		endforeach()
	endforeach()
	add_custom_target(format COMMAND ${CLANG_FORMAT_BINARY} -i ${files})
else()
	message(WARNING "clang-format not found")
endif()

if( WIN32 )
	list(APPEND SLSharedWithSettings ${CMAKE_CURRENT_SOURCE_DIR}/src/stacktrace.c)
else()
	list(APPEND SLSharedWithSettings ${CMAKE_CURRENT_SOURCE_DIR}/src/stacktrace.cpp)
endif()

include(cmake/package_config.cmake)
if(WIN32)
	install(FILES AUTHORS COPYING README THANKS NEWS DESTINATION ${CMAKE_INSTALL_PREFIX})
else()
	install(FILES AUTHORS COPYING README THANKS NEWS DESTINATION "${CMAKE_INSTALL_DOCDIR}")
	install(FILES src/images/springlobby.svg DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/icons/hicolor/scalable/apps")
	install(FILES src/springlobby.desktop DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications")
	install(FILES share/freedesktop.org/springlobby.appdata.xml DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/metainfo")
endif()

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

