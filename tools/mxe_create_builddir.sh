#!/bin/bash

set -e

COMMIT=build-2017-02-11

if [ ! -d mxe ]; then
	git clone https://github.com/mxe/mxe.git
	cd mxe
else
	cd mxe
	git fetch origin
	git clean -f -d -x
fi

git reset --hard $COMMIT

(
	echo 'JOBS := 2'
	echo 'MXE_TARGETS := i686-w64-mingw32.static'
	echo 'LOCAL_PKG_LIST := openssl boost curl wxwidgets'
	echo '.DEFAULT local-pkg-list:'
	echo 'local-pkg-list: $(LOCAL_PKG_LIST)'
) > settings.mk


