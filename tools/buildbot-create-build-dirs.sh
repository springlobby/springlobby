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

#create-build-dir build "--with-wx-config=/var/lib/buildbot/lib/linux/wx/2.8/bin/wx-config --with-opt-level=0"
create-build-dir-cmake build-cmake "-DCMAKE_CXX_FLAGS:STRING=-O0 -DSPRINGLOBBY_REV:STRING=${1}"
create-build-dir-cmake build-mingw-wx28 "-DCMAKE_TOOLCHAIN_FILE:STRING=../cmake/Toolchain-mingw32.cmake \
	-DSPRINGLOBBY_REV:STRING=${1} -DCMAKE_INSTALL_PREFIX:STRING=${PWD}/build-mingw-wx28 \
	-DLOCALE_INSTALL_DIR:STRING=${PWD}/build-mingw-wx28/locale "

	
