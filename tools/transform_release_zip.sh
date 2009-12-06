#!/usr/bin/env bash

#set -e
cd $(dirname $0)/..

#assumes absolute path
ZIPFILE=${1}

if [ ! -w ${ZIPFILE} ]; then
	echo "cannot write to ${ZIPFILE}"
	exit 1
fi

TMPDIR=/tmp/slzip
mkdir -p ${TMPDIR}
cd ${TMPDIR}
rm -rf springlobby*
#just the filename
ZIPFILEBASE=$(echo $(basename $1))
#remove .zip postfix to get the dirname inside the zip
TOPLEVELDIR=${ZIPFILEBASE%".zip"}
unzip ${ZIPFILE}
rm ${ZIPFILE}
cd ${TOPLEVELDIR} && zip ${ZIPFILE} * || exit 1
exit 0
