set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

macro(mylink var)
	foreach( lib ${var})
		link_directories( ${lib} )
	endforeach()
endmacro(mylink)

function(TO_LIST_SPACES _LIST_NAME OUTPUT_VAR)
  set(NEW_LIST_SPACE)
  foreach(ITEM ${${_LIST_NAME}})
    set(NEW_LIST_SPACE ${NEW_LIST_SPACE} ${ITEM})
  endforeach()
#   string(STRIP ${NEW_LIST_SPACE} NEW_LIST_SPACE)
  set(${OUTPUT_VAR} "${NEW_LIST_SPACE}" PARENT_SCOPE)
endfunction()

macro(add_analyze)
    find_program(ANALYZER NAMES clang-check)
    to_list_spaces(${ARGN} _sources)
    if(EXISTS ${ANALYZER})
        add_custom_target("analyze" ${ANALYZER} -fixit -p=${CMAKE_CURRENT_BINARY_DIR} -analyze ${ARGN})
    else()
        message(STATUS "not adding analyze target because clang-check is missing")
    endif(EXISTS ${ANALYZER})
endmacro(add_analyze)

macro(add_format)
    find_program(CLANGFORMAT NAMES clang-format clang-format-3.4)
    if(CLANGFORMAT-NOTFOUND)
        message(WARNING "not adding format target because clang-format is missing")
    else()
        add_custom_target("format" ${CLANGFORMAT} -i -style=file ${ARGN} )
    endif()
endmacro(add_format)

