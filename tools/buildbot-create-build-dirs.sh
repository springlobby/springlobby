#!/usr/bin/env bash

set -e
cd $(dirname $0)/..

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
	cmake $2 ..
        )
}

#create-build-dir build
create-build-dir build-linux-wx28 
create-build-dir build-mingw-wx28 "-DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-mingw32.cmake"
    