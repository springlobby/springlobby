#!/usr/bin/env bash

set -e

if [ -z "$CMAKE" ]; then
	CMAKE=cmake
fi

if [ -x "$(which $CMAKE)" ]; then
	echo CMAKE=$CMAKE
else
	echo "$CMAKE doesn't exist"
	ls -lah $(dirname $CMAKE)
	exit 1
fi

SOURCEDIR=$(readlink -f $(dirname $0)/..)
DIR=$(readlink -f build-${1})

echo SOURCEDIR=${SOURCEDIR}
echo DIR=${DIR}

cd ${SOURCEDIR}
git submodule update --init

shift
REV=$(git describe --tags)
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

$CMAKE $CMAKE_PARAMS $PARAMS $DIR/..
