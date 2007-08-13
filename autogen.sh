#!/usr/bin/env sh
#
# Requires: automake 1.10, autoconf 2.59c+
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
autoreconf --force --install || echo \
"Autoreconf failed. You are probably missing the required version of automake
and/or autoconf. If you don't have the versions mentioned on the install
instructions, upgrade them. If that is not possible, you might have luck by
editing configure.ac and Makefile.am, but then you are on your own. If you are
not planning to make patches or maintain packaging for some operating system,
it is recommended to use prebuilt/prepackaged version if available for your
operating system, or tarball source release if not, as the tarball does not
need autogen.sh invocation."

test -d debian && {
  rm -f config.sub config.guess
  ln -s /usr/share/misc/config.sub .
  ln -s /usr/share/misc/config.guess .
}

exit 0
