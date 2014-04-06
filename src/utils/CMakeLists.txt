add_custom_target(version ALL ${springlobby_SOURCE_DIR}/tools/regen_config_header.sh ${springlobby_SOURCE_DIR}/cmake/config.h ${springlobby_BINARY_DIR}/springlobby_config.h ${springlobby_BINARY_DIR}/VERSION)


add_library(springlobbyversion STATIC
	version.cpp
)

INCLUDE_DIRECTORIES(${springlobby_BINARY_DIR})
INCLUDE_DIRECTORIES(${springlobby_SOURCE_DIR})

add_dependencies(springlobbyversion version)
