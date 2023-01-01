# This file is part of the Spring engine (GPL v2 or later), see LICENSE.html

option(HEADERCHECK "adds build targets to check if all headers compile" OFF)

if(HEADERCHECK)
file(GLOB_RECURSE headers
	${springlobby_SOURCE_DIR}/src/*.h
)

set(headerchecklibs "")
foreach(HEADER ${headers})

	#pseudo unique name
	string(REPLACE "${springlobby_SOURCE_DIR}" "" PLIST "${HEADER}")
	string(REPLACE "/" ";" PLIST "${PLIST}")
	set(TEST_NAME "headercheck_")

	foreach(name ${PLIST})
		set(TEST_NAME "${TEST_NAME}_${name}")
	endforeach()

	set(TEST_FILE ${CMAKE_CURRENT_BINARY_DIR}/src-generated/${TEST_NAME}.cpp)

	# create test file
	configure_file( headercheck.cpp.in ${TEST_FILE} )
	# add library
	add_library(${TEST_NAME} EXCLUDE_FROM_ALL ${TEST_FILE})
	target_include_directories(${TEST_NAME} PRIVATE
		${springlobby_SOURCE_DIR}/src
		${springlobby_SOURCE_DIR}/src/downloader/lib/src
		${springlobby_SOURCE_DIR}/src/downloader/lib/src/lib)

	list(APPEND headerchecklibs ${TEST_NAME})
endforeach()

add_custom_target(headercheck DEPENDS ${headerchecklibs})

endif()

