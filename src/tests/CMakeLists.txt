add_subdirectory(headercheck)

FIND_PACKAGE(Boost 1.35.0 COMPONENTS unit_test_framework)

If    (NOT Boost_FOUND)
        Message(STATUS "Note: Unit tests will not be built: Boost::test library was not found")
Else  (NOT Boost_FOUND)

If      (NOT (WIN32 OR Boost_USE_STATIC_LIBS))
	#Win32 tests links static
	add_definitions(-DBOOST_TEST_DYN_LINK)
EndIf   (NOT (WIN32 OR Boost_USE_STATIC_LIBS))
add_definitions(-DTESTS)

add_custom_target(tests)
add_custom_target(check ${CMAKE_CTEST_COMMAND} --output-on-failure -V)
add_custom_target(install-tests)

macro (add_springlobby_test target sources libraries flags)
	ADD_TEST(NAME test${target} COMMAND test_${target})
	add_dependencies(tests test_${target})
	add_dependencies(check test_${target})
	add_dependencies(install-tests test_${target})
	add_executable(test_${target} EXCLUDE_FROM_ALL ${sources})
	target_link_libraries(test_${target} ${libraries})
	target_include_directories(test_${target}
		PRIVATE ${springlobby_SOURCE_DIR}/src
		PRIVATE ${springlobby_SOURCE_DIR}/src/downloader/lib/src/lsl
		PRIVATE ${Boost_INCLUDE_DIRS}
	)
	set_target_properties(test_${target} PROPERTIES COMPILE_FLAGS "${flags}")
	#install(TARGETS test_${target} DESTINATION ${BINDIR})
endmacro()

################################################################################
set(test_name GlobalEvents)
Set(test_src
	"${CMAKE_CURRENT_SOURCE_DIR}/utils/globalevents.cpp"
	"${springlobby_SOURCE_DIR}/src/utils/globalevents.cpp"
)

set(test_libs
	${WX_LD_FLAGS}
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
)
add_springlobby_test(${test_name} "${test_src}" "${test_libs}" "")
################################################################################
set(test_name Config)
Set(test_src
	"${CMAKE_CURRENT_SOURCE_DIR}/config.cpp"
	"${springlobby_SOURCE_DIR}/src/utils/slconfig.cpp"
	"${springlobby_SOURCE_DIR}/src/utils/slpaths.cpp"
	"${springlobby_SOURCE_DIR}/src/utils/conversion.cpp"
	"${springlobby_SOURCE_DIR}/src/downloader/lib/src/Logger.cpp"
)

set(test_libs
	${WX_LD_FLAGS}
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	lsl-utils
	pr-downloader_static
)
add_springlobby_test(${test_name} "${test_src}" "${test_libs}" "")
################################################################################
set(test_name ChatLog)
Set(test_src
	"${CMAKE_CURRENT_SOURCE_DIR}/chatlog.cpp"
	"${springlobby_SOURCE_DIR}/src/chatlog.cpp"
)

set(test_libs
	${WX_LD_FLAGS}
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
)
add_springlobby_test(${test_name} "${test_src}" "${test_libs}" "-DTEST")
################################################################################
set(test_name lobbyid)
Set(test_src
	"${CMAKE_CURRENT_SOURCE_DIR}/lobbyid.cpp"
	"${springlobby_SOURCE_DIR}/src/utils/crc.cpp"
	"${springlobby_SOURCE_DIR}/src/address.cpp"
)

set(test_libs
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
)
if(WIN32)
	set(test_libs ${test_libs} iphlpapi)
endif()
add_springlobby_test(${test_name} "${test_src}" "${test_libs}" "-DTEST")
################################################################################
set(test_name lslconversion)
Set(test_src
	"${CMAKE_CURRENT_SOURCE_DIR}/lslconversion.cpp"
	"${springlobby_SOURCE_DIR}/src/utils/lslconversion.cpp"
	"${springlobby_SOURCE_DIR}/src/utils/conversion.cpp"
	"${springlobby_SOURCE_DIR}/src/utils/tasutil.cpp"
	"${springlobby_SOURCE_DIR}/src/downloader/lib/src/lsl/lslutils/misc.cpp"
	"${springlobby_SOURCE_DIR}/src/downloader/lib/src/lsl/lslutils/conversion.cpp"
)

set(test_libs
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${WX_LD_FLAGS}
)
add_springlobby_test(${test_name} "${test_src}" "${test_libs}" "-DTEST")
################################################################################

set(test_name slpaths)
Set(test_src
	"${CMAKE_CURRENT_SOURCE_DIR}/slpaths.cpp"
	"${springlobby_SOURCE_DIR}/src/downloader/lib/src/lsl/lslutils/misc.cpp"
	"${springlobby_SOURCE_DIR}/src/downloader/lib/src/lsl/lslutils/conversion.cpp"
	"${springlobby_SOURCE_DIR}/src/downloader/lib/src/Logger.cpp"
	"${springlobby_SOURCE_DIR}/src/utils/conversion.cpp"
	"${springlobby_SOURCE_DIR}/src/utils/slpaths.cpp"
	"${springlobby_SOURCE_DIR}/src/utils/sortutil.cpp"
)

set(test_libs
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	pr-downloader_static
	${WX_LD_FLAGS}
)
add_springlobby_test(${test_name} "${test_src}" "${test_libs}" "-DTEST")
################################################################################
endif()
