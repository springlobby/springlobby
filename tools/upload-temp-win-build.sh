#! /bin/bash

set -e
cd $(dirname $0)/../$1
pwd

/usr/i586-mingw32msvc/bin/strip springlobby.exe

if [ x$2 == x ]; then
	filename=sl_master.exe
else
	filename=sl_${2}.exe
fi

scp springlobby.exe graasmilk.net:springlobby.info/temp/builds/${filename}
ssh rg chown www-data:www-data /usr/local/www/springlobby.info/temp/builds/${filename}

echo "http://springlobby.info/temp/builds/${filename}"
