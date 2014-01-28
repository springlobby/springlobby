add_subdirectory(headercheck)

FIND_PACKAGE(Boost 1.35.0 COMPONENTS unit_test_framework)

If    (NOT Boost_FOUND)
        Message(STATUS "Note: Unit tests will not be built: Boost::test library was not found")
Else  (NOT Boost_FOUND)

add_definitions(-DSYNCCHECK -DUNIT_TEST)
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK) 

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
	set_target_properties(test_${target} PROPERTIES COMPILE_FLAGS "${flags}")
	#install(TARGETS test_${target} DESTINATION ${BINDIR})
endmacro()

set(test_name Config)
Set(test_src
	"${CMAKE_CURRENT_SOURCE_DIR}/config.cpp"
	"${springlobby_SOURCE_DIR}/src/helper/slconfig.cpp"
)

set(test_libs
	${wxWidgets_LIBRARIES}
	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
)
add_springlobby_test(${test_name} "${test_src}" "${test_libs}" "")


endif()
