changequote(`>>>',`<<<')>>>dnl it is possible to make these work with older versions of autoconf+automake
dnl but the developers will NOT support you if it fails
dnl you really should just upgrade your autotools instead

dnl if you have too old autoconf, comment out the following prereq AND edit Makefile.am

AC_INIT([SpringLobby],[<<<esyscmd(/bin/echo -n "$VERSION")>>>],[devel@www.springlobby.info])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/springlobbyapp.cpp])
AC_CONFIG_AUX_DIR([autotools-aux])
dnl if you have too old automake, delete the version number from below AND edit Makefile.am
dnl we use filter-out GNU make extension in Makefile.am so we can't use -Werror below
dnl we can't use -Wall either because we don't want to see the filter-out warning
dnl FIXME add at least some warning categories. obviously we can't use -Wportability
AM_INIT_AUTOMAKE([1.10 -Wnone])

PKG_PROG_PKG_CONFIG

usetorrent=yes
AC_ARG_ENABLE(torrent-system,
 [  --disable-torrent-system  Disable automatic content downloads via torrent (avoids libtorrent dependency)],
 [ usetorrent=$enableval
 ])

sound=yes
AC_ARG_ENABLE(sound,
 [  --disable-sound Enable sound using SDL, needs SDL],
 [ sound=$enableval
 ])

if test x$host_os = xmingw32msvc ; then
	debug=yes
else
	debug=no
fi
AC_ARG_ENABLE(debug,
 [  --enable-debug Enable debugging],
 [ debug=$enableval
 ])

debug_report=yes
AC_ARG_ENABLE(debug-report,
 [  --enable-debug-report Enable automatic debug report uploading, requires libcurl],
 [ debug_report=$enableval
 ])

AC_ARG_WITH(sdl-config,[  --with-sdl-config=/path/to/sdl-config (optional) for finding right sdl includes],
            SDL_CONFIG="$withval")

AC_ARG_WITH(boost-prefix,[  --with-boost-prefix=/path/to/boost_topdir (optional) for finding right boost includes],
      CXXFLAGS="$CXXFLAGS -I$withval ")

AC_ARG_WITH(opt-level,[  --with-opt-level=N (optional) with N = 0..3],
      CXXFLAGS="$CXXFLAGS -O$withval ")

AC_ARG_WITH(aux-version,[  --with-aux-version='some string' (packager use only)],
      AC_DEFINE_UNQUOTED([AUX_VERSION], " $withval", [] ) )


AC_CANONICAL_HOST

AC_LANG([C++])

AC_PROG_CXX

AM_OPTIONS_WXCONFIG


AM_PATH_WXCONFIG([2.8.2], [],
    [AC_MSG_ERROR([
           wxWidgets must be installed on your system
           but wx-config script couldn't be found.

           Please check that wx-config is in path, the directory
           where wxWidgets libraries are installed (returned by
           'wx-config --libs' command) is in LD_LIBRARY_PATH or
           equivalent variable and wxWidgets version is 2.8.2 or above.
   ])], [base,core,net,adv,aui,xml,html,qa])
win_build=0
AC_ARG_VAR([WINDRES], [Windows resource file compiler command])
if test x$host_os = xmingw32msvc ; then
  AC_CHECK_TOOL([WINDRES], [windres], [:])
  win_build=1
  LIBS="$LIBS -liphlpapi"
  if test x$WINDRES = x: ; then
    AC_MSG_ERROR([missing required windres windows resource compiler.])
  fi
fi



dnl On non-Windows build use external libtorrent, on win use included source
if test "$win_build" = 0 ; then
    if test x$usetorrent = xyes ; then
        AC_MSG_CHECKING([for libtorrent from Rasterbar software])

	# Check for libtorrent using the following names: libtorrent-rasterbar, rb-libtorrent,
	# libtorrent.
        PKG_CHECK_EXISTS([libtorrent-rasterbar >= 0.13],[usetorrent=libtorrent-rasterbar],[usetorrent=fail])
        if test x$usetorrent = xfail ; then
	    PKG_CHECK_EXISTS([rb-libtorrent >= 0.13],[usetorrent=rb-libtorrent],[usetorrent=fail])
            if test x$usetorrent = xfail ; then
		PKG_CHECK_EXISTS([libtorrent >= 0.13],[usetorrent=libtorrent],[usetorrent=fail])
		if test x$usetorrent = xfail ; then
		    AC_MSG_RESULT(no)
		    AC_MSG_ERROR([
		    Missing required libtorrent library. You can get it from
		    <http://www.rasterbar.com/products/libtorrent/>.

		    Please note that some distributions name it rb-libtorrent or
		    libtorrent-rasterbar.  This is NOT libtorrent from rakshasa!

		    You can skip this dependency by using --disable-torrent-system]);

		    exit
		fi
	    fi
        fi

	AC_MSG_RESULT(yes)

	# Need to manually define these, since only used PKG_CHECK_EXISTS and not
	# PKG_CHECK_MODULES above.
	LIBTORRENT_CFLAGS=`pkg-config $usetorrent --cflags`
 	LIBTORRENT_LIBS=`pkg-config $usetorrent --libs`

        AC_SUBST(LIBTORRENT_CFLAGS)
        AC_SUBST(LIBTORRENT_LIBS)
    else
    	CXXFLAGS="$CXXFLAGS -DNO_TORRENT_SYSTEM"
    fi
else
    if test x$usetorrent = xyes ; then
        CXXFLAGS="$CXXFLAGS  -D_WIN32_WINNT=0x0501 -DBOOST_WINDOWS -DTORRENT_DISABLE_ENCRYPTION  "
        LIBS="$LIBS -mthreads -Wl,-allow-multiple-definition -L/var/lib/buildbot/lib/mingw/lib -lboost_thread-mt -lboost_filesystem-mt  -lws2_32 -lmswsock -lboost_date_time-mt"
    else
       	CXXFLAGS="$CXXFLAGS -DNO_TORRENT_SYSTEM"
    fi
fi

dnl check if sound configure is enables, if yes check for SDL
if test x$sound = xyes ; then
    AM_PATH_SDL(1.2.10, true , AC_ERROR("Sound requires SDL > 1.2.10 SDL_sound and SDL_mixer use --disable-sound to skip the dependency and the feature") )
else
    CXXFLAGS="$CXXFLAGS -DDISABLE_SOUND "
fi

if test x$debug = xyes ; then
    CXXFLAGS="$CXXFLAGS -DDEBUG -g"
else
    CXXFLAGS="$CXXFLAGS -DNDEBUG "
fi

#on windows append identifier to version string
if test "$win_build" = 1 ; then
    AC_DEFINE([VERSION],["<<<esyscmd(/bin/echo -n "$VERSION")>>> on Windows"]  )
fi

if test x$debug_report = xyes ; then
	if test "$win_build" = 0 ; then
		LIBCURL_CHECK_CONFIG( yes, 7.10.1,
			[CXXFLAGS="$CXXFLAGS $LIBCURL_CPPFLAGS",   LIBS="$LIBS $LIBCURL"] )
#			AC_ERROR("Debug report uplaoding requires libcurl, which could not be found. This dependency can be avoided by passing --disable-debug-report") 		)
	else
		CXXFLAGS="$CXXFLAGS -I/var/lib/buildbot/lib/mingw/include -DCURL_STATICLIB -mno-cygwin "
		#DO NOT change link order here (or anywhere else!)
		LIBS="$LIBS  -L/var/lib/buildbot/lib/mingw/lib -lcurl -lws2_32 -lmswsock -imagehlp "
	fi
	CXXFLAGS="$CXXFLAGS -DENABLE_DEBUG_REPORT"
fi

AM_CONDITIONAL([USE_WINDRES], test "$win_build" = 1)
AM_CONDITIONAL([USE_LIBT_INCLUDED], test x$usetorrent = xyes && test "$win_build" = 1)
AM_CONDITIONAL([ENABLE_DEBUG_REPORT], test x$debug_report = xyes )

AC_MSG_CHECKING([if we can enable extra features that need wxWidgets-2.8])
dnl we probably aren't supposed to use the function below, but it is the simplest way
_WX_PRIVATE_CHECK_VERSION([2], [8], [0])
if test -n "$wx_ver_ok"; then
  AC_MSG_RESULT([yes])
  AC_DEFINE([HAVE_WX28], [1], [Define to 1 if wxWidgets is at least version 2.8.0])
else
  AC_MSG_RESULT([no])
fi

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.17])



AC_CONFIG_FILES([Makefile po/Makefile.in])
AC_OUTPUT
<<<
