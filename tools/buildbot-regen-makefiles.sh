#!/usr/bin/env bash

set -e
cd $(dirname $0)/..

for f in build build-linux-wx28 build-mingw-wx28 ; do
    ( cd $f ; ./config.status )
done

