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