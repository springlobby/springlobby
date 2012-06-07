#!/usr/bin/env bash
set -e
cd $(dirname $0)/..

TARFILE=${1}/${2}.tar
rm -f ${TARFILE}
pwd
./tools/git-archive-all --prefix ${2}/  ${TARFILE}
tar -Prf ${TARFILE} ${1}/springlobby_config.h --transform "s;${1}/springlobby_config.h;${2}/springlobby_config.h;g"
cat ${TARFILE} | gzip > ${TARFILE}.gz
cat ${TARFILE} | bzip2 > ${TARFILE}.bz2
rm ${TARFILE}
