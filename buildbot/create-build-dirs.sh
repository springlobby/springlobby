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

PARAMS="-DSPRINGLOBBY_REV=${REV} -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM:STRING=NEVER -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY:STRING=ONLY -DCMAKE_FIND_ROOT_PATH:PATH=/opt/mingw32/usr/i686-pc-mingw32 -DCMAKE_SYSTEM_NAME:STRING=Windows -DBoost_THREADAPI:STRING=win32 -DCURL_CONFIG:PATH=/opt/mingw32/usr/i686-pc-mingw32/bin/curl-config -DCMAKE_RC_COMPILER:PATH=/opt/mingw32/usr/bin/i686-pc-mingw32-windres"
#FIXME: should be this:(move to buildbot master config!) PARAMS=$@

echo configuring ${DIR} with $PARAMS
cd ${DIR}
rm -fv CMakeCache.txt
cmake $PARAMS ..

