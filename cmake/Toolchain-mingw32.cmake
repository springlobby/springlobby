# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER i586-mingw32msvc-gcc)
SET(CMAKE_CXX_COMPILER i586-mingw32msvc-g++)
SET( wxWidgets_ROOT_DIR /var/lib/buildbot/lib/mingw/wx/2.8/ )
# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH  /usr/bin/i586-mingw32msvc /var/lib/buildbot/lib/mingw /var/lib/buildbot/lib/mingw/wx/2.8/include/wx-2.8)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
# SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET( wxWidgets_CONFIG_EXECUTABLE /var/lib/buildbot/lib/mingw/wx/2.8/bin/wx-config )
SET( wxWidgets_LIBRARIES
	wx_baseu-2.8-i586-mingw32msvc
	wx_mswu_core-2.8-i586-mingw32msvc
	wx_baseu_net-2.8-i586-mingw32msvc
	wx_mswu_adv-2.8-i586-mingw32msvc
	wx_mswu_aui-2.8-i586-mingw32msvc
	wx_mswu_html-2.8-i586-mingw32msvc
	wx_baseu_xml-2.8-i586-mingw32msvc)
SET( wxWidgets_INCLUDE_DIRS /var/lib/buildbot/lib/mingw/wx/2.8/include/wx-2.8 /var/lib/buildbot/lib/mingw/wx/2.8/lib/wx/include/i586-mingw32msvc-msw-unicode-release-2.8/)
SET( wxWidgets_LIB_DIR /var/lib/buildbot/lib/mingw/wx/2.8/lib )
SET( wxWidgets_CONFIGURATION mswu )
SET( CMAKE_VERBOSE_MAKEFILE ON )
SET( sdl_INCLUDE_DIR /var/lib/buildbot/lib/mingw/include/SDL )
SET( sdl_LIB_DIR /var/lib/buildbot/lib/mingw/lib )
SET( libcurl_INCLUDE_DIR /var/lib/buildbot/lib/mingw/include/ )
SET( libcurl_LIB_DIR /var/lib/buildbot/lib/mingw/lib/ )
SET( wxWidgets_RC_DIR /var/lib/buildbot/lib/mingw/wx/2.8/include/wx-2.8 )

SET( Boost_LIBRARIES libboost_thread-gcc42-mt-d-1_41.a boost_filesystem-gcc42-mt-d-1_41 boost_date_time-gcc42-mt-d-1_41 boost_system-gcc42-mt-d-1_41 )
SET( boost_LIB_DIR /var/lib/buildbot/lib/mingw/lib )
SET( boost_INCLUDE_DIR /var/lib/buildbot/lib/mingw/include )
link_directories( /var/lib/buildbot/lib/mingw/lib )
INCLUDE_DIRECTORIES(/var/lib/buildbot/lib/mingw/include /var/lib/buildbot/lib/mingw/include/zlib/ )

ADD_DEFINITIONS( -DWXUSINGDLL -D__WXMSW__)

