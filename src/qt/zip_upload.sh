#!/bin/bash
set -e
make 
cp ../qt-build/release/sasi.exe .
/opt/mingw_head/usr/bin/i686-pc-mingw32-strip sasi.exe
7za a -l mapview_current.7z sasi.exe 
filebin mapview_current.7z
