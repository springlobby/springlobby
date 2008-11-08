#! /bin/bash

set -e
cd $(dirname $0)/../$1
pwd

/usr/i586-mingw32msvc/bin/strip springlobby.exe
#branch=$((git branch | grep "*") | sed -e "s;*\ ;;g" -e "s;\n;;g")
filename=sl_${2}.exe
scp springlobby.exe graasmilk.net:springlobby.info/temp/builds/${filename}
ssh rg chown www-data:wwwdata /usr/local/www/springlobby.info/temp/builds/${filename}
springlobby.info/temp/builds/${filename}
echo "http://springlobby.info/temp/builds/${filename}"
