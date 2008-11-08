#! /bin/bash

set -e
cd $(dirname $0)/../$1

/usr/i586-mingw32msvc/bin/strip *.exe
branch=(git branch | grep "*") | sed -e "s;*\ ;;g" -e "s;\n;;g"
filename=sl_${branch}.exe
scp springlobby.exe graasmilk.net:springlobby.info/temp/builds/${filename}
echo "http://springlobby.info/temp/builds/${filename}"
