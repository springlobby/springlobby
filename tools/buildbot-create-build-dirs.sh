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

create-build-dir-cmake build-cmake "-DCMAKE_CXX_FLAGS:STRING=-O0 -DSPRINGLOBBY_REV:STRING=${1} -DwxWidgets_CONFIG_EXECUTABLE:STRING=/usr/bin/wx-config"
create-build-dir-cmake build-mingw-wx28 "-DCMAKE_TOOLCHAIN_FILE:STRING=../cmake/Toolchain-mingw32_new.cmake \
	-DOPTION_SOUND:BOOL=OFF -DENABLE_DEBUG_REPORT=ON -DCMAKE_EXE_LINKER_FLAGS:STRING=-L/opt/mingw32/lib \
	-DSPRINGLOBBY_REV:STRING=${1} -DCMAKE_INSTALL_PREFIX:STRING=${PWD}/build-mingw-wx28 -DAUX_VERSION:STRING=msw \
	-DLOCALE_INSTALL_DIR:STRING=${PWD}/build-mingw-wx28/locale -DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo "

	
