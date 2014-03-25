INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR})
add_library(json STATIC
	jsonreader.cpp
	jsonval.cpp
)
