#!/bin/bash

set -e

COMMIT=0dcf498f9563fd35eff0273e1b56929fdce60383

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
	echo 'MXE_TARGETS := i686-w64-mingw32.static.posix'
	echo 'LOCAL_PKG_LIST := openssl boost curl wxwidgets'
	echo '.DEFAULT_GOAL  := local-pkg-list'
	echo 'local-pkg-list: $(LOCAL_PKG_LIST)'
) > settings.mk


