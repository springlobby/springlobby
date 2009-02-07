#!/bin/bash

set -e
cd $(dirname $0)/..

cd po/wininst/usr/local/share/ 
zip -ur ../../../../../${1}  locale
