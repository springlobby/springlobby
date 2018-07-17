#! /usr/bin/env bash

set -e
cd $(dirname $0)/../${1}
pwd
DEVELOPER=${2}

if [ x$2 == x ]; then
	filename=sl_master_dbg.zip
else
	filename=sl_${2}_dbg.zip
fi

zip -9 -u ${filename} src/springlobby.exe

if [ ! -d /data/www/springlobby.info/temp/builds/$DEVELOPER ] ; then
	mkdir -p /data/www/springlobby.info/temp/builds/$DEVELOPER
fi

/usr/bin/install -m 0755 ${filename} /data/www/springlobby.info/temp/builds/$DEVELOPER/${filename}

echo "https://springlobby.info/temp/builds/$DEVELOPER/${filename}"
