#http://www.cmake.org/Wiki/CMake:CPackConfiguration

#don't write when git errored out resulting in unset version (ie when compiling from tarball)
IF ( NOT GIT_ERROR )
    CONFIGURE_FILE( ${springlobby_SOURCE_DIR}/cmake/config.h ${springlobby_BINARY_DIR}/springlobby_config.h )
ENDIF ( NOT GIT_ERROR )

SET( PACKAGE_NAME springlobby )
SET( PACKAGE_VERSION ${SPRINGLOBBY_REV} )

IF (WIN32)
    SET(CPACK_GENERATOR "ZIP")
    SET(CPACK_PACKAGE_FILE_NAME "springlobby-${SPRINGLOBBY_REV}-win32")
ELSE (WIN32)
    SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")
    SET(CPACK_GENERATOR "TBZ2;TGZ")
    SET(CPACK_PACKAGE_FILE_NAME "springlobby-${SPRINGLOBBY_REV}")
ENDIF (WIN32)
SET(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${CMAKE_PROJECT_NAME};ALL;/")
# SET(CPACK_OUTPUT_CONFIG_FILE "/home/andy/vtk/CMake-bin/CPackConfig.cmake")
# SET(CPACK_PACKAGE_DESCRIPTION_FILE "/home/andy/vtk/CMake/Copyright.txt")
# SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CMake is a build tool")
#SET(CPACK_PACKAGE_EXECUTABLES "springlobby" "springsettings")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "springlobby")
SET(CPACK_PACKAGE_NAME "springlobby")
SET(CPACK_PACKAGE_VENDOR "The SpringLobby Team")
SET(CPACK_PACKAGE_VERSION ${SPRINGLOBBY_REV})

SET(CPACK_RESOURCE_FILE_LICENSE ${springlobby_SOURCE_DIR}/COPYING)
SET(CPACK_RESOURCE_FILE_README ${springlobby_SOURCE_DIR}/README)
# SET(CPACK_RESOURCE_FILE_WELCOME "/home/andy/vtk/CMake/Templates/CPack.GenericWelcome.txt")
SET(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
# SET(CPACK_SOURCE_OUTPUT_CONFIG_FILE "/home/andy/vtk/CMake-bin/CPackSourceConfig.cmake")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "springlobby-${SPRINGLOBBY_REV}")
SET(CPACK_STRIP_FILES FALSE)
SET(CPACK_SOURCE_STRIP_FILES FALSE)
# SET(CPACK_SYSTEM_NAME "Linux-i686")
# SET(CPACK_TOPLEVEL_TAG "Linux-i686")
set(CPACK_SOURCE_IGNORE_FILES
"^${springlobby_SOURCE_DIR}/build*"
"^${springlobby_SOURCE_DIR}/bin/"
"^${springlobby_SOURCE_DIR}/.*"
"^${springlobby_SOURCE_DIR}/auto*"
"^${springlobby_SOURCE_DIR}/doc/"
"^${springlobby_SOURCE_DIR}/m4/"
"^${springlobby_SOURCE_DIR}/obj/"
"^${springlobby_SOURCE_DIR}/installer/"
)
INCLUDE(CPack)
