dnl it is possible to make these work with older versions of autoconf+automake
dnl but the developers will NOT support you if it fails
dnl you really should just upgrade your autotools instead

dnl if you have too old autoconf, comment out the following prereq AND edit Makefile.am
AC_PREREQ([2.59c])
AC_INIT([SpringLobby], [0.0.1-svn], [FIXME_our_bugs_email_address])
dnl if you have too old automake, delete the version number from below AND edit Makefile.am
dnl we use filter-out GNU make extension in Makefile.am so we can't use -Werror below
AM_INIT_AUTOMAKE([1.10 -Wall])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/springlobbyapp.cpp])

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
   ])])

AC_ARG_VAR([WINDRES], [Windows resource file compiler command])
if test x$host_os = xmingw32msvc ; then
  AC_CHECK_TOOL([WINDRES], [windres], [:])
  if test x$WINDRES = x: ; then
    AC_MSG_ERROR([missing required windres windows resource compiler.])
  fi
fi

AC_ARG_VAR([SVNVERSION], [Produce a compact version number for a working copy.])
AC_CHECK_PROGS([SVNVERSION], [svnversion false])

AM_CONDITIONAL([USE_WINDRES], [test x$host_os = xmingw32msvc])

AC_MSG_CHECKING([if we can enable extra features that need wxWidgets-2.8])
dnl we probably aren't supposed to use the function below, but it is the simplest way
_WX_PRIVATE_CHECK_VERSION([2], [8], [0])
if test -n "$wx_ver_ok"; then
  AC_MSG_RESULT([yes])
  AC_DEFINE([HAVE_WX28], [1], [Define to 1 if wxWidgets is at least version 2.8.0])
else
  AC_MSG_RESULT([no])
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
