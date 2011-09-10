Include(TestCXXAcceptsFlag)
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
				OUTPUT_VARIABLE GCC_VERSION)

IF( OPTION_TORRENT_SYSTEM )
	if (GCC_VERSION VERSION_GREATER 4.6 OR GCC_VERSION VERSION_EQUAL 4.6)
			set( Boost_VERSION ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION} )
			if ( Boost_VERSION VERSION_LESS 1.42 )
				message(STATUS "Adding -fpermissive since boost ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION} was found using gcc ${GCC_VERSION}" )
				ADD_DEFINITIONS( -fpermissive )
			endif ()
	endif()
ENDIF( OPTION_TORRENT_SYSTEM )

If(NOT DEFINED LTO_FLAGS)
	Set(LTO_FLAGS "")

	Set(LTO       FALSE CACHE BOOL "Link Time Optimizations (LTO)")
	If    (LTO)
		CHECK_CXX_ACCEPTS_FLAG("-flto" HAS_LTO_FLAG)
		If    (HAS_LTO_FLAG)
			Set(LTO_FLAGS "${LTO_FLAGS} -flto")
		Else  (HAS_LTO_FLAG)
			Set(LTO_FLAGS "${LTO_FLAGS} -flto")
		EndIf (HAS_LTO_FLAG)
	EndIf (LTO)

	Set(LTO_WHOPR FALSE CACHE BOOL "Link Time Optimizations (LTO) - Whole program optimizer (WHOPR)")
	If    (LTO_WHOPR)
		CHECK_CXX_ACCEPTS_FLAG("-fwhopr" HAS_LTO_WHOPR_FLAG)
		If    (HAS_LTO_WHOPR_FLAG)
			Set(LTO_FLAGS "${LTO_FLAGS} -fwhopr")
		EndIf (HAS_LTO_WHOPR_FLAG)
	EndIf (LTO_WHOPR)
	
	If (LTO AND LTO_WHOPR)
		Message( FATAL_ERROR "LTO and LTO_WHOPR are mutually exclusive, please enable only one at a time." )
	EndIf (LTO AND LTO_WHOPR)
	
EndIf (NOT DEFINED LTO_FLAGS)

# Add Link-Time-Optimization flags, if supported (GCC >= 4.5) and enabled
Set(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS}           ${LTO_FLAGS}")
Set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    ${LTO_FLAGS}")
Set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LTO_FLAGS}")