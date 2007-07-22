#!/usr/bin/env sh
#
# Requires: automake 1.9, autoconf 2.57+
# Conflicts: autoconf 2.13
set -e
cd $(dirname $0)

echo Cleaning autotools files...
find -type d -name autom4te.cache -print0 | xargs -0 rm -rf \;
find -type f \( -name missing -o -name install-sh -o -name mkinstalldirs \
  -o -name depcomp -o -name ltmain.sh -o -name configure -o -name config.sub \
  -o -name config.guess -o -name Makefile.in -o -name config.h.in \
  -o -name config.h.in~ -o -name aclocal.m4 \) -print0 | xargs -0 rm -f

echo Running autoreconf...
autoreconf --force --install

test -d debian && {
  rm -f config.sub config.guess
  ln -s /usr/share/misc/config.sub .
  ln -s /usr/share/misc/config.guess .
}

exit 0
