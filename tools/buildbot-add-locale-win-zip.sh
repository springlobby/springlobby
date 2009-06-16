#!/bin/bash

set -e
cd $(dirname $0)/..

cd build-mingw-wx28/po/wininst/usr/local/share/ 
zip -ur ../../../../../${1}  locale
