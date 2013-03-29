#! /usr/bin/env bash

set -e
cd $(dirname $0)/../${1}
pwd
DEVELOPER=${2}
/opt/mingw32/usr/bin/i686-pc-mingw32-strip src/springlobby.exe

if [ x$3 == x ]; then
	filename=sl_master.zip
else
	filename=sl_${3}.zip
fi

zip -9 -u ${filename} src/springlobby.exe

if [ ! -d /data/www/springlobby.info/temp/builds/$DEVELOPER ] ; then
	mkdir -p /data/www/springlobby.info/temp/builds/$DEVELOPER
fi

/usr/bin/install -m 0755 ${filename} /data/www/springlobby.info/temp/builds/$DEVELOPER/${filename}

echo "http://springlobby.info/temp/builds/$DEVELOPER/${filename}"
