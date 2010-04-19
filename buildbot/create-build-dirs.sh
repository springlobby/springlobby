#!/usr/bin/env bash

set -e
cd $(dirname ${0})/..

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

echo -n configuring ${DIR} with -DSPRINGLOBBY_REV:STRING=${1} $@ ...

cd ${DIR}
cmake ..  -DSPRINGLOBBY_REV="${1}" $@