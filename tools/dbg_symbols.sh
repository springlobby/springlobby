#!/bin/bash

REV=${1}
STRIP=/opt/mingw32/usr/bin/i686-pc-mingw32-strip 
OBJCOPY=/opt/mingw32/usr/bin/i686-pc-mingw32-objcopy

PREF=build-msw-default
ROOT=""
for binary in springlobby_updater.exe springsettings.exe springlobby.exe ; do
	DBG=${binary}.dbg
	DBGZ=${binary}.dbg.7z
	BIN=${PREF}/${binary}
	${OBJCOPY} ${BIN} --only-keep-debug ${DBG}
	${OBJCOPY} ${BIN} --add-gnu-debuglink=${DBG}
	7za a ${DBGZ} ${DBG}
done

