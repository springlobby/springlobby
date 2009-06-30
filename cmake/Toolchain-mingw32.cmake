# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER /var/lib/buildbot/lib/mingw_/usr/bin/i386-mingw32msvc-gcc )
SET(CMAKE_CXX_COMPILER /var/lib/buildbot/lib/mingw_/usr/bin/i386-mingw32msvc-g++)
SET( wxWidgets_ROOT_DIR /var/lib/buildbot/lib/mingw_/usr/i386-mingw32msvc )
# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH  /var/lib/buildbot/lib/mingw_/usr/i386-mingw32msvc/bin /var/lib/buildbot/lib/mingw_/usr/i386-mingw32msvc /var/lib/buildbot/lib/mingw_/usr/i386-mingw32msvc/include/wx-2.8)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
# SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET( wxWidgets_CONFIG_EXECUTABLE /var/lib/buildbot/lib/mingw_/usr/i386-mingw32msvc/bin/wx-config )
SET( wxWidgets_LIBRARIES wx_baseu-2.8-i586-mingw32msvc wx_mswu_core-2.8-i586-mingw32msvc wx_baseu_net-2.8-i586-mingw32msvc wx_mswu_adv-2.8-i586-mingw32msvc  wx_mswu_aui-2.8-i586-mingw32msvc wx_mswu_html-2.8-i586-mingw32msvc wx_baseu_xml-2.8-i586-mingw32msvc)
SET( wxWidgets_INCLUDE_DIRS /var/lib/buildbot/lib/mingw_/usr/i386-mingw32msvc/include/wx-2.8 )
SET( wxWidgets_LIB_DIR /var/lib/buildbot/lib/mingw_/usr/i386-mingw32msvc/lib )
SET( wxWidgets_CONFIGURATION mswu )
SET( CMAKE_VERBOSE_MAKEFILE ON )
SET( Boost_LIBRARIES boost_thread-mt boost_filesystem-mt boost_date_time-mt )
SET( boost_LIB_DIR /var/lib/buildbot/lib/mingw_/usr/i386-mingw32msvc/lib )
SET( boost_INCLUDE_DIR /var/lib/buildbot/lib/mingw_/usr/i386-mingw32msvc/include )
SET( sdl_INCLUDE_DIR /var/lib/buildbot/lib/mingw_/usr/i386-mingw32msvc/include/SDL )
SET( wxWidgets_RC_DIR /var/lib/buildbot/lib/mingw_/usr/i386-mingw32msvc/include/wx-2.8 )


ADD_DEFINITIONS( -DWXUSINGDLL -D__WXMSW__)

