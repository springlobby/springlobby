#!/usr/bin/env bash

set -e
cd $(dirname $0)/..

if [ $# -ne 1 ] ; then
    echo "Usage: $0 <who>"
    exit
fi

who=$1

git checkout master
git fetch ${who}
git merge ${who}/master
git push origin master
git fetch --tags origin
git push --tags github master
