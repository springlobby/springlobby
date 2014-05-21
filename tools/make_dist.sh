#!/usr/bin/env bash
set -e
cd $(dirname $0)/..

TARFILE=${1}/${2}.tar
rm -f ${TARFILE}
pwd

#FIXME: hack for getting VERSION file into archive
echo ${3} > ${1}/VERSION

./tools/git-archive-all --prefix ${2}/  ${TARFILE}
tar -Prf ${TARFILE} ${1}/springlobby_config.h ${1}/VERSION \
	--transform "s;${1}/springlobby_config.h;${2}/springlobby_config.h;g" \
	--transform "s;${1}/VERSION;${2}/VERSION;g"
cat ${TARFILE} | gzip > ${TARFILE}.gz
cat ${TARFILE} | bzip2 > ${TARFILE}.bz2
rm ${TARFILE}
