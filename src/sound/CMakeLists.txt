#----------------------------------------------------------------------------------------------------
# libalure dependency check
#----------------------------------------------------------------------------------------------------
if   ( OPTION_SOUND )
        message(STATUS "Sound enabled!")
        find_package(Alure REQUIRED)
        include_directories(${ALURE_INCLUDE_DIRS})
        find_package(OpenAL REQUIRED)
else ( OPTION_SOUND )
        # Disable sound.
        message(STATUS "Sound disabled!")
        add_definitions( -DDISABLE_SOUND )

        unset(ALURE_LIBRARIES)
        unset(OPENAL_LIBRARIES)
endif ( OPTION_SOUND )

add_library(sound STATIC
	alsound.cpp
)

target_link_libraries(sound
	${ALURE_LIBRARIES}
	${OPENAL_LIBRARY}
)
