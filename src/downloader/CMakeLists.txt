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

set(OPTION_CONSOLETOOL OFF)
set(OPTION_SHAREDLIB OFF)
Add_Subdirectory(lib)
