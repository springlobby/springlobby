AC_PREREQ([2.59c])
AC_INIT([SpringLobby], [0.1], [FIXME_our_bugs_email_address])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/springlobbyapp.cpp])

AC_LANG([C++])

AC_PROG_CXX

AM_OPTIONS_WXCONFIG

AM_PATH_WXCONFIG([2.6.0], [],
   [AC_MSG_ERROR([
           wxWidgets must be installed on your system
           but wx-config script couldn't be found.

           Please check that wx-config is in path, the directory
           where wxWidgets libraries are installed (returned by
           'wx-config --libs' command) is in LD_LIBRARY_PATH or
           equivalent variable and wxWidgets version is 2.6.0 or above.
   ])])

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
