# get revision and put into config.h
# EXECUTE_PROCESS(COMMAND ${SpringLobby_SOURCE_DIR}/tools/get-revision.sh
#         OUTPUT_VARIABLE SPRINGLOBBY_REV
#         RESULT_VARIABLE GIT_ERROR
#         OUTPUT_STRIP_TRAILING_WHITESPACE)

#don't write when git errored out resulting in unset version (ie when compiling from tarball)
IF ( NOT GIT_ERROR)
    CONFIGURE_FILE( ${SpringLobby_SOURCE_DIR}/cmake/config.h ${SpringLobby_BINARY_DIR}/config.h )
ENDIF ( NOT GIT_ERROR)

#if nothing went wrong we have the file and can define HAVE_CONFIG_H
IF ( EXISTS ${SpringLobby_BINARY_DIR}/config.h )
    ADD_DEFINITIONS( -DHAVE_CONFIG_H )
    INCLUDE_DIRECTORIES(${SpringLobby_BINARY_DIR})
ENDIF ( EXISTS ${SpringLobby_BINARY_DIR}/config.h )

IF (WIN32)
    SET(CPACK_GENERATOR "ZIP")
    SET(CPACK_PACKAGE_FILE_NAME "springlobby-${SPRINGLOBBY_REV}-win32")
ELSE (WIN32)
    SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")
    SET(CPACK_GENERATOR "TBZ2;TGZ")
    SET(CPACK_PACKAGE_FILE_NAME "springlobby-${SPRINGLOBBY_REV}")
ENDIF (WIN32)
# SET(CPACK_INSTALL_CMAKE_PROJECTS "/home/andy/vtk/CMake-bin;CMake;ALL;/")
# SET(CPACK_OUTPUT_CONFIG_FILE "/home/andy/vtk/CMake-bin/CPackConfig.cmake")
# SET(CPACK_PACKAGE_DESCRIPTION_FILE "/home/andy/vtk/CMake/Copyright.txt")
# SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CMake is a build tool")
# SET(CPACK_PACKAGE_EXECUTABLES "springlobby")
SET(CPACK_PACKAGE_FILE_NAME "springlobby-${SPRINGLOBBY_REV}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "SpringLobby")
SET(CPACK_PACKAGE_NAME "SpringLobby")
SET(CPACK_PACKAGE_VENDOR "The SpringLobby Team")
SET(CPACK_PACKAGE_VERSION ${SPRINGLOBBY_REV})

SET(CPACK_RESOURCE_FILE_LICENSE ${SpringLobby_SOURCE_DIR}/COPYING)
SET(CPACK_RESOURCE_FILE_README ${SpringLobby_SOURCE_DIR}/README)
# SET(CPACK_RESOURCE_FILE_WELCOME "/home/andy/vtk/CMake/Templates/CPack.GenericWelcome.txt")
SET(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
# SET(CPACK_SOURCE_OUTPUT_CONFIG_FILE "/home/andy/vtk/CMake-bin/CPackSourceConfig.cmake")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "springlobby-${SPRINGLOBBY_REV}")
SET(CPACK_STRIP_FILES TRUE)
SET(CPACK_SOURCE_STRIP_FILES TRUE)
# SET(CPACK_SYSTEM_NAME "Linux-i686")
# SET(CPACK_TOPLEVEL_TAG "Linux-i686")
set(CPACK_SOURCE_IGNORE_FILES
"^${SpringLobby_SOURCE_DIR}/build*"
"^${SpringLobby_SOURCE_DIR}/bin/"
"^${SpringLobby_SOURCE_DIR}/.*"
"^${SpringLobby_SOURCE_DIR}/auto*"
"^${SpringLobby_SOURCE_DIR}/doc/"
"^${SpringLobby_SOURCE_DIR}/m4/"
"^${SpringLobby_SOURCE_DIR}/obj/"
)
