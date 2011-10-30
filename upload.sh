#!/bin/bash
/usr/i586-mingw32msvc/bin/strip springlobby.exe
mv springlobby.exe SL.exe
set -e
if [ x$1 == x ]; then
        filename=sl_master.zip
else
        filename=sl_${1}.zip
fi

zip -9 -u ${filename} SL.exe

scp ${filename} rho:/usr/local/www/springlobby.info/temp/builds/koshi/

echo http://springlobby.info/temp/builds/koshi/${filename}

#if [ x$2 == x ] ; then
	#./sl_report.py "http://springlobby.info/temp/builds/koshi/${filename}"
#fi

#notify win-build
