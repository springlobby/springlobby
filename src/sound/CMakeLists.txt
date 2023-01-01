#----------------------------------------------------------------------------------------------------
# libalure dependency check
#----------------------------------------------------------------------------------------------------

option(OPTION_SOUND "enable sound support (requires alure / openal)" ON)

if(OPTION_SOUND AND NOT WIN32)
	message(STATUS "Sound enabled!")
	find_package(Alure REQUIRED)
	find_package(OpenAL REQUIRED)
	add_library(sound STATIC
		alsound.cpp)
	target_link_libraries(sound
		${ALURE_LIBRARIES}
		${OPENAL_LIBRARY})
	target_include_directories(sound
		PRIVATE ${ALURE_INCLUDE_DIRS}
		PRIVATE ${springlobby_SOURCE_DIR}/src
		PRIVATE ${springlobby_SOURCE_DIR}/src/downloader/lib/src/lsl)
elseif(WIN32)
	message(STATUS "Sound enabled! (win32)")
	add_library(sound STATIC winsound.cpp)
	target_link_libraries(sound winmm.lib)
else()
	# Disable sound.
	message(STATUS "Sound disabled!")
	add_library(sound STATIC
		nosound.cpp)
endif()

