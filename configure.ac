dnl it is possible to make these work with older versions of autoconf+automake
dnl but the developers will NOT support you if it fails
dnl you really should just upgrade your autotools instead

dnl if you have too old autoconf, comment out the following prereq AND edit Makefile.am
AC_PREREQ([2.59c])
AC_INIT([SpringLobby], [0.0.1-svn], [devel@www.springlobby.info])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/springlobbyapp.cpp])
AC_CONFIG_AUX_DIR([autotools-aux])
dnl if you have too old automake, delete the version number from below AND edit Makefile.am
dnl we use filter-out GNU make extension in Makefile.am so we can't use -Werror below
dnl we can't use -Wall either because we don't want to see the filter-out warning
dnl FIXME add at least some warning categories. obviously we can't use -Wportability
AM_INIT_AUTOMAKE([1.10 -Wnone])

usetorrent=yes
AC_ARG_ENABLE(torrent-system,
 [  --disable-torrent-system  Disable automatic content downloads via torrent (avoids libtorrent dependency)],
 [ usetorrent=no
 ])

sound=yes
AC_ARG_ENABLE(sound,
 [  --disable-sound Enable sound using SDL, needs SDL],
 [ sound=no
 ])

AC_ARG_WITH(sdl-config,[  --with-sdl-config=/path/to/sdl-config (optional) for finding right sdl includes],
            SDL_CONFIG="$withval")

AC_ARG_WITH(boost-prefix,[  --with-boost-prefix=/path/to/boost_topdir (optional) for finding right boost includes],
      CXXFLAGS="$CXXFLAGS -I$withval ")

AC_CANONICAL_HOST

AC_LANG([C++])

AC_PROG_CXX

AM_OPTIONS_WXCONFIG

AM_PATH_WXCONFIG([2.6.3], [],
   [AC_MSG_ERROR([
           wxWidgets must be installed on your system
           but wx-config script couldn't be found.

           Please check that wx-config is in path, the directory
           where wxWidgets libraries are installed (returned by
           'wx-config --libs' command) is in LD_LIBRARY_PATH or
           equivalent variable and wxWidgets version is 2.6.3 or above.
   ])], [base,core,net,adv,qa,richtext])
win_build=0
AC_ARG_VAR([WINDRES], [Windows resource file compiler command])
if test x$host_os = xmingw32msvc ; then
  AC_CHECK_TOOL([WINDRES], [windres], [:])
  win_build=1
  if test x$WINDRES = x: ; then
    AC_MSG_ERROR([missing required windres windows resource compiler.])
  fi
fi



#on non win build use extern libtorrent, on win use included source
if test "$win_build" = 0 ; then
    if test x$usetorrent = xyes ; then
        AC_MSG_CHECKING([checking for libtorrent])
        PKG_CHECK_MODULES(LIBTORRENT, libtorrent >= 0.13, [], usetorrent=fail)
        if test x$usetorrent = xfail ; then
            AC_MSG_ERROR([missing required libtorrent library, get it at http://www.rasterbar.com/products/libtorrent/, please note that some distros name it rb-libtorrent, that is NOT libtorrent from rakshasa]);
            exit
        fi
        AC_SUBST(LIBTORRENT_CFLAGS)
        AC_SUBST(LIBTORRENT_LIBS)
    else
    	CXXFLAGS="$CXXFLAGS -DNO_TORRENT_SYSTEM"
    fi
else
    if test x$usetorrent = xyes ; then
        CXXFLAGS="$CXXFLAGS  -D_WIN32_WINNT=0x0501 -DBOOST_WINDOWS -DNO_RICHTEXT_CHAT -DTORRENT_DISABLE_ENCRYPTION  "
        LIBS=' -Wl,-allow-multiple-definition -L/var/lib/buildbot/lib/mingw/lib -lboost_thread-mt -lboost_filesystem-mt  -lws2_32 -lmswsock -lboost_date_time-mt'
    else
       	CXXFLAGS="$CXXFLAGS -DNO_TORRENT_SYSTEM"
    fi
fi

#check if sound configure is enables, if yes check for SDL
if test x$sound = xyes ; then
    AM_PATH_SDL(1.2.10, true , AC_ERROR("Sound requires SDL > 1.2.10, SDL_sound and SDL_mixer, use --disable-sound to skip the dependency (and the feature)") )
else
    CXXFLAGS="$CXXFLAGS -DDISABLE_SOUND "
fi


AC_ARG_VAR([SVNVERSION], [Produce a compact version number for a working copy.])
AC_CHECK_PROGS([SVNVERSION], [svnversion false])

AM_CONDITIONAL([USE_WINDRES], test "$win_build" = 1)

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
AM_GNU_GETTEXT_VERSION([0.14])



AC_CONFIG_FILES([Makefile po/Makefile.in])
AC_OUTPUT
