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
Add_Subdirectory(lib)
