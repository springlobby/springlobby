#! /bin/bash

set -e
cd $(dirname $0)/../${1}
pwd
DEVELOPER=${3}
/usr/i586-mingw32msvc/bin/strip springlobby.exe

if [ x$3 == x ]; then
	filename=sl_master.zip
else
	filename=sl_${3}.zip
fi

zip -9 -u ${filename} springlobby.exe

if [ ! -d /usr/local/www/springlobby.info/temp/builds/$DEVELOPER ] ; then
	mkdir -p /usr/local/www/springlobby.info/temp/builds/$DEVELOPER
fi

/usr/bin/install -m 0755 ${filename} /usr/local/www/springlobby.info/temp/builds/$DEVELOPER/${filename}

echo "http://springlobby.info/temp/builds/$DEVELOPER/${filename}"
