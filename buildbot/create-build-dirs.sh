#!/usr/bin/env bash

set -e
cd $(dirname ${0})/..

git submodule update --init

DIR=build-${1}
shift
REV=${1}
shift
echo -n creating ${DIR} ...

if [ ! -d ${DIR} ] ; then
	mkdir ${DIR}
	echo done
else
	echo skipped
fi

export CXX=/opt/mingw32/usr/bin/i686-pc-mingw32-g++
export CC=/opt/mingw32/usr/bin/i686-pc-mingw32-gcc

PARAMS="-DSPRINGLOBBY_REV=${REV} ${@}"

echo configuring ${DIR} with $PARAMS
cd ${DIR}
rm -fv CMakeCache.txt
cmake $PARAMS ..

