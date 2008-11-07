#! /bin/bash

set -e
cd $(dirname $0)/../$1

/usr/i586-mingw32msvc/bin/strip *.exe
scp springlobby.exe graasmilk.net:springlobby.info/temp/builds/sl.exe
echo "http://springlobby.info/temp/builds/$1.exe"
