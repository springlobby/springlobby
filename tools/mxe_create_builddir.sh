#!/bin/bash

set -e

COMMIT=df8926aa61407e560a5c4b20f6d85854ebdf384d
REPO=https://github.com/abma/mxe.git

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
	echo 'MXE_TARGETS := i686-w64-mingw32.static.posix'
	echo 'LOCAL_PKG_LIST := openssl boost curl wxwidgets'
	echo '.DEFAULT_GOAL  := local-pkg-list'
	echo 'local-pkg-list: $(LOCAL_PKG_LIST)'
) > settings.mk


