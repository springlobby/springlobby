add_sources(springlobbySrc
	downloader/prdownloader.cpp
	downloader/downloadlistctrl.cpp
)
	
#currently unused
add_sources( filelistSrc
	downloader/filelister/filelistctrl.cpp
	downloader/filelister/filelistdialog.cpp
	downloader/filelister/filelistfilter.cpp 
)

Add_Subdirectory(lib)
