#!/usr/bin/env bash

set -e

SOURCEDIR=$(readlink -f $(dirname $0)/..)
DIR=$(readlink -f build-${1})

echo SOURCEDIR=${SOURCEDIR}
echo DIR=${DIR}

cd ${SOURCEDIR}
git submodule update --init

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

PARAMS="-DSPRINGLOBBY_REV=${REV} ${@}"

echo configuring ${DIR} with $PARAMS
cd ${DIR}
rm -fv CMakeCache.txt CPackConfig.cmake CPackSourceConfig.cmake
find -name cmake_install.cmake -delete
cmake -DPKG_CONFIG_EXECUTABLE:PATH=/opt/mingw32/usr/bin/i686-w64-mingw32-pkg-config $PARAMS $DIR/..
