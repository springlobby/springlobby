#!/usr/bin/env bash

set -e
cd $(dirname $0)/..

create-build-dir-cmake ()
{
	echo -n creating $1 ...
	(
	if [ ! -d $1 ] ; then
		mkdir $1
		echo done
	else
		echo skipped
	fi
	)
	echo -n configuring $1 with $2 ...
	(
	cd $1
	cmake .. $2
	)
}

create-build-dir ()
{
	echo -n creating $1 ...
	(
	if [ ! -d $1 ] ; then
		mkdir $1
		echo done
	else
		echo skipped
	fi
	)
	echo -n configuring $1 with $2 ...
	(
	cd $1
	if [ ! -f Makefile ] ; then
		../configure  --config-cache $2
		echo done
	else
		# regenerate Makefile with current config options
		./config.status
		echo regen done
	fi
	)
}

create-build-dir build "--with-wx-config=/var/lib/buildbot/lib/linux/wx/2.8/bin/wx-config --with-opt-level=0"
create-build-dir build-mingw-wx28 "--build=i686-pc-linux-gnu --host=i586-mingw32msvc --with-wx-config=/var/lib/buildbot/lib/mingw/wx/2.8/bin/wx-config  --with-boost-prefix=/var/lib/buildbot/lib/mingw/include/  --with-sdl-config=/var/lib/buildbot/lib/mingw/bin/sdl-config --with-opt-level=2"
if [ -e /usr/bin/g++-4.4 ] ; then
	create-build-dir-cmake build-cmake "-DCMAKE_CXX_COMPILER:STRING=g++-4.4 -DCMAKE_CXX_FLAGS:STRING=-O0"
else
	create-build-dir-cmake build-cmake "-DCMAKE_CXX_FLAGS:STRING=-O0"	
fi
