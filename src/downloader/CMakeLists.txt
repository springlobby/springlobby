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

Add_Subdirectory(lib)
