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
	cd ..
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
		../configure CXXFLAGS="-O0" --config-cache $2
		echo done
	else
		# regenerate Makefile with current config options
		./config.status
		echo regen done
	fi
	)
}

create-build-dir-cmake build "-DOPTION_OPT_LEVEL=0 -DwxWidgets_CONFIG_EXECUTABLE=/var/lib/buildbot/lib/linux/wx/2.8/bin/wx-config"
create-build-dir-cmake build-mingw-wx28 "-DCMAKE_TOOLCHAIN_FILE=${PWD}/cmake/Toolchain-mingw32.cmake"
