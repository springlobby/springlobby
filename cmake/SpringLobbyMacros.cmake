MACRO(CreateResourceCompileCommand out_var dirIn fileIn fileOut)
# 	SET( fileIn "${dirIn}/${fileIn}" )
	SET( fileOut "${CMAKE_CURRENT_BINARY_DIR}/${fileOut}" )
	ADD_CUSTOM_COMMAND(
		OUTPUT
			"${fileOut}"
		DEPENDS
			"${fileIn}" 
		COMMAND
			"${CMAKE_RC_COMPILER}"
				"-I${dirIn}"
				"-I${wxWidgets_RC_DIR}"
				"-i${CMAKE_CURRENT_SOURCE_DIR}/${fileIn}" 
				"-o" "${fileOut}"
				"-v"
		)
	SET_SOURCE_FILES_PROPERTIES(${fileOut} PROPERTIES
		GENERATED      TRUE
		OBJECT_DEPENDS ${fileIn}
		)
	SET(${out_var} "${fileOut}")
ENDMACRO (CreateResourceCompileCommand out_var dirIn fileIn fileOut)

function (add_sources varname )
    foreach (_src ${ARGN})
		list (APPEND tmp "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
    endforeach()
	set (${varname} ${${varname}} ${tmp} CACHE INTERNAL "${varname}" FORCE)
endfunction()

function (add_to_global varname )
    foreach (_src ${ARGN})
		list (APPEND tmp "${_src}")
    endforeach()
	set (${varname} ${${varname}} ${ARGN} CACHE INTERNAL "${varname}" FORCE)
endfunction()

function (clear varname)
	set (${varname} "" CACHE INTERNAL "" FORCE)
endfunction()

macro(mylink var)
	foreach( lib ${var})
		link_directories( ${lib} )
	endforeach()
endmacro(mylink)

MACRO( HEADERCHECK )
  ADD_CUSTOM_TARGET( headercheck SOURCES ${ARGN} )
  FOREACH( HEADER ${ARGN} )
    string(REPLACE "/" ";" PLIST "${HEADER}")
    list(REVERSE PLIST)
    list(GET PLIST 0 FIRST)
    list(GET PLIST 1 SECOND)
    list(GET PLIST 2 THIRD)
    #pseudo unique name
    SET(TEST_NAME "headercheck__${THIRD}_${SECOND}_${FIRST}")
    SET(TEST_FILE ${CMAKE_CURRENT_BINARY_DIR}/tests/headercheck/${TEST_NAME}.cpp)
    configure_file( ${springlobby_SOURCE_DIR}/cmake/headercheck.cpp.in ${TEST_FILE} )
    ADD_LIBRARY( ${TEST_NAME} STATIC ${TEST_FILE} )
    set_target_properties(${TEST_NAME} PROPERTIES LINKER_LANGUAGE CXX)
    add_dependencies( headercheck ${TEST_NAME} )
  ENDFOREACH( HEADER )
ENDMACRO( HEADERCHECK )