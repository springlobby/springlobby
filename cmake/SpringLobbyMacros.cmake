MACRO(CreateResourceCompileCommand out_var dirIn fileIn fileOut)
	SET( fileIn "${dirIn}/${fileIn}" )
	SET( fileOut "${CMAKE_CURRENT_BINARY_DIR}/${fileOut}" )
	ADD_CUSTOM_COMMAND(
		OUTPUT
			"${fileOut}"
		DEPENDS
			"${fileIn}" 
		COMMAND
			"${CMAKE_RC_COMPILER}"
				"-I${dirIn}"
				"-i${fileIn}" 
				"-o" "${fileOut}"
				"-v"
		)
	SET_SOURCE_FILES_PROPERTIES(${fileOut} PROPERTIES
		GENERATED      TRUE
		OBJECT_DEPENDS ${fileIn}
		)
	SET(${out_var} "${fileOut}")
ENDMACRO (CreateResourceCompileCommand out_var dirIn fileIn fileOut)

function (add_sources varname sources )
    foreach (_src ${sources})
		list (APPEND tmp "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
    endforeach()
	set (${varname} ${${varname}} ${tmp})
endfunction()