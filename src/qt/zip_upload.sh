#!/bin/bash
set -e
make 
cp ../qt-build/release/sasi.exe .
/opt/mingw_head/usr/bin/i686-pc-mingw32-strip sasi.exe
fn=$(git describe --tags)
7za a -l sasi_${fn}.7z sasi.exe 
scp sasi_${fn}.7z ro:springlobby.info/sasi/
