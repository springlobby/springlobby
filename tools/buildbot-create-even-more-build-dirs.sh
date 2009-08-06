#!/usr/bin/env bash

source ./tools/buildbot-create-build-dirs.sh

set -e
cd $(dirname $0)/..

create-build-dir build-no-curl "--with-wx-config=/var/lib/buildbot/lib/linux/wx/2.8/bin/wx-config --with-opt-level=0 --disable-debug-report"
create-build-dir build-no-tor "--with-wx-config=/var/lib/buildbot/lib/linux/wx/2.8/bin/wx-config --with-opt-level=0 --disable-torrent-system"
create-build-dir build-no-nothing "--with-wx-config=/var/lib/buildbot/lib/linux/wx/2.8/bin/wx-config --with-opt-level=0 --disable-debug-report --disable-torrent-system --disable-sound"

create-build-dir build-no-curl-win "--build=i686-pc-linux-gnu --host=i586-mingw32msvc --with-wx-config=/var/lib/buildbot/lib/mingw/wx/2.8/bin/wx-config  --with-boost-prefix=/var/lib/buildbot/lib/mingw/include/  --with-sdl-config=/var/lib/buildbot/lib/mingw/bin/sdl-config  --with-opt-level=0 --disable-debug-report"
create-build-dir build-no-tor-win "--build=i686-pc-linux-gnu --host=i586-mingw32msvc --with-wx-config=/var/lib/buildbot/lib/mingw/wx/2.8/bin/wx-config  --with-boost-prefix=/var/lib/buildbot/lib/mingw/include/  --with-sdl-config=/var/lib/buildbot/lib/mingw/bin/sdl-config  --with-opt-level=0 --disable-debug-report --disable-torrent-system"
create-build-dir build-no-nothing-win "--build=i686-pc-linux-gnu --host=i586-mingw32msvc --with-wx-config=/var/lib/buildbot/lib/mingw/wx/2.8/bin/wx-config  --with-boost-prefix=/var/lib/buildbot/lib/mingw/include/  --with-opt-level=0 --disable-debug-report --disable-torrent-system --disable-sound"

