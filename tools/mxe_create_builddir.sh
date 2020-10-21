#!/bin/bash

set -e

COMMIT=1024dc7d2db5eb7d5d3c64a2c12b5f592572f1ce
REPO=https://github.com/mxe/mxe.git

if [ ! -d mxe ]; then
	git clone $REPO
	cd mxe
else
	cd mxe
	git remote set-url origin $REPO
	git fetch origin
	git clean -f -d -x
	git reset --hard origin/master
fi

git reset --hard $COMMIT

(
	echo 'JOBS := 2'
	echo 'MXE_TARGETS := x86_64-w64-mingw32.static'
	echo 'LOCAL_PKG_LIST := openssl boost curl wxwidgets'
	echo '.DEFAULT_GOAL  := local-pkg-list'
	echo 'local-pkg-list: $(LOCAL_PKG_LIST)'
	echo 'override MXE_PLUGIN_DIRS += plugins/gcc10'
) > settings.mk


