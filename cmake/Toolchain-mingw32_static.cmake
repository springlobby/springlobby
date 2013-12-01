SET( LOCALE_INSTALL_DIR "${CMAKE_BINARY_DIR}/locale" CACHE STRING
	"message catalogs will installed here" FORCE )
SET( CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE STRING
	"install prefix" FORCE )

# FIXME: move to buildbot's master.cfg
include(/opt/mingw32/usr/i686-w64-mingw32/share/cmake/mxe-conf.cmake)

