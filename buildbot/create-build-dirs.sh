#!/usr/bin/env bash

set -e
cd $(dirname ${0})/..

git submodule update --init

DIR=build-${1}
shift
REV=${1}
shift
echo -n creating ${DIR} ...

if [ ! -d ${DIR} ] ; then
	mkdir ${DIR}
	echo done
else
	echo skipped
fi

echo configuring ${DIR} with -DSPRINGLOBBY_REV="${REV}" $@

cd ${DIR}
cmake -DSPRINGLOBBY_REV="${REV}" $@ ..
