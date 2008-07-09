#!/usr/bin/env sh
#
# Requires: automake 1.10, autoconf 2.59c+
# Conflicts: autoconf 2.13
set -e
cd `dirname $0`

# Grab the error-reporting functions
. tools/util.sh

echo -n 'Cleaning autotools files... '
find -type d -name autom4te.cache -print0 | xargs -0 rm -rf \;
find -type f \( -name missing -o -name install-sh -o -name mkinstalldirs \
  -o -name depcomp -o -name ltmain.sh -o -name configure -o -name config.sub \
  -o -name config.guess -o -name Makefile.in -o -name config.h.in \
  -o -name config.h.in~ -o -name aclocal.m4 -o -name configure.ac \) -print0 | xargs -0 rm -f
echo done.

echo -n 'Setting version... '

# If there's an argument to this script, use it as the version string.
if test $# -gt 0; then
    if test x"$1" != x; then
	# configure.ac.m4 uses the VERSION envvar.
	export VERSION="$1"
    else
	die_error 'Empty version string specified.'
    fi
fi


# Set the version string, if it isn't already set.
if test x"$VERSION" = x; then
    export VERSION=`./tools/get-revision.sh`
fi

# If the version string still isn't set, bail out.
if test x"$VERSION" = x; then
    die_error 'Failed to determine version and no version specified.' \
'If you cloned SpringLobby from a git repository, you must leave the
.git subdirectory intact.  If you want to manually set the version
string, specify it as an argument to this script.'
fi	   

echo 'done.'



# Create configure.ac from configure.ac.m4
echo -n 'Creating configure.ac... '
m4 configure.ac.m4 > configure.ac || die_error 'Failed to generate configure.ac from configure.ac.m4.'
echo 'done.'

echo -n 'Running autoreconf... '
true || die_error 'Autoreconf failed.' \
"You are probably missing the required version of automake and/or
autoconf.  If you don't have the versions mentioned on the install
instructions, upgrade them.  If that is not possible, you might have
luck by editing configure.ac and Makefile.am, but then you are on your
own.

		   IF YOU JUST WANT TO PLAY SPRING
If you are _not_ planning to make patches or maintain packaging for
some operating system, we recommend that you use a prebuilt or
prepackaged version of SpringLobby for your system, if available.  If
one is not available, you can install from a tarball source release.

See <http://trac.springlobby.info/wiki/Install> for more information."
echo 'done.'

D="`dirname $0`"
echo ''
echo "Finished.  Now run $D/configure, or $D/configure --help to list
available options." | fmt

exit 0
