#!/bin/bash
set -e
cd $(dirname $0)/..

git archive --format=tar --prefix=${2}/ HEAD  > ${1}/${2}.tar
tar -Prf ${1}/${2}.tar ${1}/config.h --transform "s;${1}/config\.h;${2}/config.h;g"
cat ${1}/${2}.tar | gzip > ${1}/${2}.tar.gz
cat ${1}/${2}.tar | bzip2 > ${1}/${2}.tar.bz2
rm ${1}/${2}.tar
