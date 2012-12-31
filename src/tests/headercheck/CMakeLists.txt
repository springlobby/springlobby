# This file is part of the Spring engine (GPL v2 or later), see LICENSE.html

#FIXME: for some reason the target headercheck is always built. if this can be fixed,
#       this option can be removed
OPTION(HEADER_CHECK "check header files if they compile correctly" OFF)

IF(HEADER_CHECK)
include_directories(
		${springlobby_SOURCE_DIR}/src
		${springlobby_SOURCE_DIR}/src/downloader/lib/src
		${springlobby_SOURCE_DIR}/src/downloader/lib/src/lib
	)

FILE(GLOB_RECURSE headers
		${springlobby_SOURCE_DIR}/src/*.h
	)
ADD_CUSTOM_TARGET( headercheck )

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
	#set_target_properties(${TEST_NAME} PROPERTIES LINKER_LANGUAGE CXX)
	add_dependencies( headercheck ${TEST_NAME} )
ENDFOREACH( HEADER )

ENDIF(HEADER_CHECK)

