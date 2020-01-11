#!/bin/sh

set -e

cd mxe
make MXE_USE_CCACHE= MXE_PLUGIN_DIRS=plugins/gcc8

echo "done! now use mxe/usr/bin/i686-w64-mingw32.static-cmake to configure!"
