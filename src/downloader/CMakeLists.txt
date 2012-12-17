add_sources(springlobbySrc
	prdownloader.cpp
	downloadlistctrl.cpp
)
	
#currently unused
clear(filelistSrc)
add_sources( filelistSrc
	filelister/filelistctrl.cpp
	filelister/filelistdialog.cpp
	filelister/filelistfilter.cpp 
)

set(CONSOLETOOL OFF CACHE BOOL "disable console tool compile" FORCE)
set(SHAREDLIB OFF CACHE BOOL "disable shared lib compile" FORCE)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/CMakeLists.txt")
	MESSAGE(FATAL_ERROR "lib/CMakeLists.txt doesn't exist, please run \ngit submodule update --init")
endif()

Add_Subdirectory(lib)
