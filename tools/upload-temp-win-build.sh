#! /usr/bin/env bash

set -e


cd $(dirname $0)/../${1}
pwd
DEVELOPER=${2}

mv src/springlobby.exe .
mv src/updater/springlobby_updater.exe .
/opt/mingw32/usr/bin/i686-w64-mingw32-strip springlobby.exe
/opt/mingw32/usr/bin/i686-w64-mingw32-strip springlobby_updater.exe

if [ x$3 == x ]; then
	filename=sl_master.zip
else
	filename=sl_${3}.zip
fi

#rm -f ${filename}

zip -9 -u ${filename} springlobby.exe springlobby_updater.exe

if [ ! -d /data/www/springlobby.info/temp/builds/$DEVELOPER ] ; then
	mkdir -p /data/www/springlobby.info/temp/builds/$DEVELOPER
fi

/usr/bin/install -m 0644 ${filename} /data/www/springlobby.info/temp/builds/$DEVELOPER/${filename}

REV=$(grep -o -P '(?<=define VERSION ").*(?=")' springlobby_config.h)
echo $REV
echo $REV > /data/www/springlobby.info/temp/builds/$DEVELOPER/current.txt
chmod 0644 /data/www/springlobby.info/temp/builds/$DEVELOPER/current.txt

