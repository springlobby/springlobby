# This file is part of the Spring engine (GPL v2 or later), see LICENSE.html

OPTION(HEADERCHECK "adds build targets to check if all headers compile" OFF)

if (HEADERCHECK)
include_directories(
		${springlobby_SOURCE_DIR}/src
		${springlobby_SOURCE_DIR}/src/downloader/lib/src
		${springlobby_SOURCE_DIR}/src/downloader/lib/src/lib
	)

FILE(GLOB_RECURSE headers
		${springlobby_SOURCE_DIR}/src/*.h
	)

SET(headerchecklibs "")
FOREACH( HEADER ${headers} )

	#pseudo unique name
	string(REPLACE "${springlobby_SOURCE_DIR}" "" PLIST "${HEADER}")
	string(REPLACE "/" ";" PLIST "${PLIST}")
	SET(TEST_NAME "headercheck_")

	foreach(name ${PLIST})
		SET(TEST_NAME "${TEST_NAME}_${name}")
	endforeach()

	SET(TEST_FILE ${CMAKE_CURRENT_BINARY_DIR}/src-generated/${TEST_NAME}.cpp)

	# create test file
	configure_file( headercheck.cpp.in ${TEST_FILE} )
	# add library
	add_library( ${TEST_NAME} EXCLUDE_FROM_ALL ${TEST_FILE} )
	list(APPEND headerchecklibs ${TEST_NAME})
ENDFOREACH( HEADER )

ADD_CUSTOM_TARGET( headercheck DEPENDS ${headerchecklibs})

endif()

