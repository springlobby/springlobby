#!/bin/bash

set -e

COMMIT=a65e97c541d9fd6a881771b89fa7190bdfc49650

if [ ! -d mxe ]; then
	git clone https://github.com/mxe/mxe.git
fi

cd mxe
git reset --hard $COMMIT
git clean -f -d -x

(
	echo 'JOBS := 2'
	echo 'MXE_TARGETS := i686-w64-mingw32.static'
	echo 'LOCAL_PKG_LIST := boost curl wxwidgets'
	echo '.DEFAULT local-pkg-list:'
	echo 'local-pkg-list: $(LOCAL_PKG_LIST)'
) > settings.mk


