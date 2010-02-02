# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER i586-pc-mingw32-gcc)
SET(CMAKE_CXX_COMPILER i586-pc-mingw32-g++)
SET( wxWidgets_ROOT_DIR /opt/mingw32/ )
# here is the target environment located

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
# SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET( wxWidgets_CONFIG_EXECUTABLE /opt/mingw32/bin/wx-config )
SET( wxWidgets_LIBRARIES
	wx_baseu_net-2.8-i586-pc-mingw32.dll
	wx_mswu_core-2.8-i586-pc-mingw32.dll
	wx_baseu-2.8-i586-pc-mingw32.dll
	wx_mswu_adv-2.8-i586-pc-mingw32.dll
	wx_mswu_aui-2.8-i586-pc-mingw32.dll
	wx_mswu_html-2.8-i586-pc-mingw32.dll
	wx_mswu_gl-2.8-i586-pc-mingw32.dll
	wx_baseu_xml-2.8-i586-pc-mingw32.dll
	wx_mswu_qa-2.8-i586-pc-mingw32.dll
	wx_mswu_richtext-2.8-i586-pc-mingw32.dll
	wx_mswu_xrc-2.8-i586-pc-mingw32.dll)
SET( wxWidgets_INCLUDE_DIRS /opt/mingw32/include/wx-2.8/ /opt/mingw32/lib/wx/include/i586-pc-mingw32-msw-unicode-release-2.8/)
SET( wxWidgets_LIB_DIR /opt/mingw32/lib )
SET( wxWidgets_CONFIGURATION mswu )
SET( CMAKE_VERBOSE_MAKEFILE ON )
SET( wxWidgets_RC_DIR /opt/mingw32/include/wx-2.8 )
#otherwise cmake finds linux lib for win...
SET( ENV{OPENALDIR} /opt/mingw32/ )
SET( OPENAL_LIBRARY OpenAL32 )
SET( OPENAL_INCLUDE_DIR /opt/mingw32/include/AL )
SET( Boost_LIBRARIES
	boost_thread-gcc44-mt-1_41
	boost_filesystem-gcc44-mt-1_41
	boost_system-gcc44-mt-1_41 )
SET( boost_LIB_DIR /opt/mingw32/lib )
SET( boost_INCLUDE_DIR /opt/mingw32/include )
link_directories( /opt/mingw32/lib )
INCLUDE_DIRECTORIES(/opt/mingw32/include  )

ADD_DEFINITIONS( -DCURL_STATICLIB -DWXUSINGDLL -D__WXMSW__ -mthreads)
