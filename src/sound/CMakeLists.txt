#----------------------------------------------------------------------------------------------------
# libalure dependency check
#----------------------------------------------------------------------------------------------------
option(OPTION_SOUND "enable sound support (requires alure / openal)" ON)

if   ( OPTION_SOUND )
	message(STATUS "Sound enabled!")
	find_package(Alure REQUIRED)
	include_directories(${ALURE_INCLUDE_DIRS})
	find_package(OpenAL REQUIRED)
	add_library(sound STATIC
		alsound.cpp
	)
	target_link_libraries(sound
		${ALURE_LIBRARIES}
		${OPENAL_LIBRARY}
	)
else ()
	# Disable sound.
	message(STATUS "Sound disabled!")
	add_library(sound STATIC
		nosound.cpp
	)
endif ()

