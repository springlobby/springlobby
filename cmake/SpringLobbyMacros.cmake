set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

macro(CreateResourceCompileCommand out_var dirIn fileIn fileOut)
# 	set( fileIn "${dirIn}/${fileIn}" )
	set( fileOut "${CMAKE_CURRENT_BINARY_DIR}/${fileOut}" )
	add_custom_command(
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
				"-v")
	set_source_files_properties(${fileOut} PROPERTIES
		GENERATED      TRUE
		OBJECT_DEPENDS ${fileIn})
	set(${out_var} "${fileOut}")
endmacro()

macro(mylink var)
	foreach(lib ${var})
		link_directories( ${lib} )
	endforeach()
endmacro()

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
    endif()
endmacro()

macro(add_format)
    find_program(CLANGFORMAT NAMES clang-format clang-format-3.4)
    if(CLANGFORMAT-NOTFOUND)
        message(WARNING "not adding format target because clang-format is missing")
    else()
        add_custom_target("format" ${CLANGFORMAT} -i -style=file ${ARGN})
    endif()
endmacro()

