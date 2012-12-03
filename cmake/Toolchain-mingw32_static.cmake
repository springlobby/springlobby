# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_FIND_LIBRARY_SUFFIXES  ".a" )
set(CMAKE_FIND_ROOT_PATH /opt/mingw32/usr/i686-pc-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_SHARED_MODULE_PREFIX "")
SET( ENV{PKG_CONFIG_PATH} /opt/mingw32/usr/i686-pc-mingw32/lib/pkgconfig)
SET( PKG_CONFIG_EXECUTABLE /opt/mingw32/usr/bin/i686-pc-mingw32-pkg-config )

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER /opt/mingw32/usr/bin/i686-pc-mingw32-gcc )
SET(CMAKE_CXX_COMPILER /opt/mingw32/usr/bin/i686-pc-mingw32-g++ )
SET(CMAKE_RC_COMPILER /opt/mingw32/usr/bin/i686-pc-mingw32-windres)
SET( MINGWLIBS /opt/mingw32/ CACHE INTERNAL MINGWLIB_DUMMMY_DIR FORCE  )
SET( BUILD_SHARED_LIBS OFF CACHE INTERNAL MINGWLIB_DUMMMY_DIR FORCE  )
SET(CMAKE_FIND_ROOT_PATH /opt/mingw32 )
SET( wxWidgets_CONFIG_EXECUTABLE /opt/mingw32/usr/i686-pc-mingw32/bin/wx-config )
SET( wxWidgets_LIBRARIES
	wx_baseu-2.8-i686-pc-mingw32.a      wx_mswu_adv-2.8-i686-pc-mingw32.a   wx_mswu_html-2.8-i686-pc-mingw32.a      
	wxregexu-2.8-i686-pc-mingw32.a		wx_baseu_net-2.8-i686-pc-mingw32.a  wx_mswu_aui-2.8-i686-pc-mingw32.a   
	wx_mswu_qa-2.8-i686-pc-mingw32.a 	wx_baseu_xml-2.8-i686-pc-mingw32.a  wx_mswu_core-2.8-i686-pc-mingw32.a  
	wx_mswu_richtext-2.8-i686-pc-mingw32.a
)
SET( wxWidgets_INCLUDE_DIRS /opt/mingw32/usr/i686-pc-mingw32/include/wx-2.8/ /opt/mingw32/usr/i686-pc-mingw32/lib/wx/include/i586-pc-mingw32-msw-unicode-release-2.8/)
SET( wxWidgets_LIB_DIR /opt/mingw32/usr/i686-pc-mingw32/lib )
SET( wxWidgets_CONFIGURATION mswu )
SET( wxWidgets_FOUND ON )
SET( CMAKE_VERBOSE_MAKEFILE OFF )
SET( wxWidgets_RC_DIR /opt/mingw32/usr/i686-pc-mingw32/include/wx-2.8)

#otherwise cmake finds linux lib for win...
SET( ENV{OPENALDIR} /opt/mingw32/ )
SET( OPENAL_LIBRARY OpenAL32 )
SET( OPENAL_INCLUDE_DIR /opt/mingw32/include/AL )
SET( OPENAL_LIBRARY /opt/mingw32/usr/i686-pc-mingw32/lib/libOpenAL32.a )

SET( Boost_NO_SYSTEM_PATHS TRUE )
SET( BOOST_ROOT /opt/mingw32/usr/i686-pc-mingw32 )
set( EXTRA_LIB_DIRS /opt/mingw32/usr/i686-pc-mingw32/lib /opt/mingw32/usr/lib  )

INCLUDE_DIRECTORIES( /opt/mingw32/usr/i686-pc-mingw32/lib/wx/include/i686-pc-mingw32-msw-unicode-release-static-2.8/ /opt/mingw32/usr/lib/gcc/i686-pc-mingw32/4.7.0/include/ /opt/mingw32/usr/i686-pc-mingw32/include )

SET( LOCALE_INSTALL_DIR "${CMAKE_BINARY_DIR}/locale" CACHE STRING
	"message catalogs will installed here" FORCE )
SET( CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE STRING
	"install prefix" FORCE )
#3/4 make linking satic possible
ADD_DEFINITIONS(-D__WXMSW__ -mthreads -D_WIN32_WINNT=0x0501  -DBOOST_THREAD_USE_LIB -DAL_LIBTYPE_STATIC )
LINK_LIBRARIES(
	${wxWidgets_LIBRARIES}
	png tiff jpeg lzma xml2 gnutls-openssl ssh2 idn OpenAL32 nettle iconv gcrypt wldap32 gnutls 
	expat gmp hogweed gpg-error SDL gnutls nettle curl ws2_32 intl shell32 setupapi comctl32 OpenAL32 z winmm gmp
	hogweed gmp)
	SET( ASSHOLE 
	${wxWidgets_LIBRARIES}
	png tiff jpeg lzma xml2 gnutls-openssl ssh2 idn OpenAL32 nettle iconv gcrypt wldap32 gnutls 
	expat gmp hogweed gpg-error SDL gnutls nettle curl ws2_32 intl shell32 setupapi comctl32 OpenAL32 z winmm gmp)
SET( CURL_CFLAGS "-I/opt/mingw32/usr/i686-pc-mingw32/include" )
# SET( CURL_STATIC_LIBRARY_DIRS "/opt/mingw32/usr/i686-pc-mingw32/lib")
# SET( CURL_STATIC_LDFLAGS "-L/opt/mingw32/usr/i686-pc-mingw32/lib;-lcurl;-lws2_32")
SET( CURL_LIBRARY "/opt/mingw32/usr/i686-pc-mingw32/lib/libcurl.a")
SET( CURL_INCLUDE_DIR "/opt/mingw32/usr/i686-pc-mingw32/include")
SET( PNG_LIBRARY "/opt/mingw32/usr/i686-pc-mingw32/lib/libpng.a")
SET( PNG_PNG_INCLUDE_DIR "/opt/mingw32/usr/i686-pc-mingw32/include")
#SET( ZLIB_LIBRARY "/opt/mingw32/usr/i686-pc-mingw32/lib/libz.a")
#SET( ZLIB_LIBRARIES ${ZLIB_LIBRARY} )
#SET( ZLIB_INCLUDE_DIR "/opt/mingw32/usr/i686-pc-mingw32/include")
#SET( ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIR} )
SET(CROSS_COMPILING "ON")
