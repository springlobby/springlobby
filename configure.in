dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59c])
AC_INIT([springlobby], [0.1], [FIXME_our_bugs_email_address])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/springlobbyapp.cpp])

AC_LANG([C++])

AC_PROG_CPP
AC_PROG_CXX

AM_OPTIONS_WXCONFIG

reqwx=2.6.0
AM_PATH_WXCONFIG([$reqwx], [wxWin=1])
if test "$wxWin" != 1; then
   AC_MSG_ERROR([
           wxWidgets must be installed on your system
           but wx-config script couldn't be found.

           Please check that wx-config is in path, the directory
           where wxWidgets libraries are installed (returned by
           'wx-config --libs' command) is in LD_LIBRARY_PATH or
           equivalent variable and wxWidgets version is $reqwx or above.
   ])
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
