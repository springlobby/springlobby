#!/bin/sh

set -e

cd mxe
make

echo "done! now use mxe/usr/bin/i686-w64-mingw32.static-cmake to configure!"
