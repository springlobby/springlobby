add_sources(springlobbySrc
	activitynotice.cpp	
	misc.cpp
	tasutil.cpp
	networkevents.cpp
	md5.c
	battleevents.cpp
)
add_library(springlobbyversion STATIC
	version.cpp
)

#if nothing went wrong we have the file and can define HAVE_CONFIG_H
IF ( EXISTS ${springlobby_BINARY_DIR}/springlobby_config.h)
	set_target_properties(springlobbyversion PROPERTIES COMPILE_DEFINITIONS "HAVE_VERSION=1")
	INCLUDE_DIRECTORIES(${springlobby_BINARY_DIR})
ELSEIF ( EXISTS ${springlobby_SOURCE_DIR}/springlobby_config.h  )
	set_target_properties(springlobbyversion PROPERTIES COMPILE_DEFINITIONS "HAVE_VERSION=1")
	INCLUDE_DIRECTORIES(${springlobby_SOURCE_DIR})
ELSE()
	message(WARNING "springlobby_config.h not found!")
ENDIF ()

