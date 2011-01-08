#! /usr/bin/env bash

set -e

#expects tarball path as input

if [ ! -e $1 ] ; then
    echo "damn";
    exit 1;
fi

if [ ! -d build-distcheck ] ; then
    mkdir build-distcheck;
fi

cp $1 build-distcheck/
cd build-distcheck
tar xfz $1
tbdir=`echo "$1" | sed -e 's,.tar.gz,,g'`
cd $tbdir
cmake .
make && cd ../.. ; rm -rf build-distcheck