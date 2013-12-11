#!/usr/bin/env bash

#regen on bbot releases would screw with version
if [ "x${BUILDBOT_RELEASE}" != "x" ] ; then
	echo "not regenerating version on buildbot release"
	exit 0
fi

set -u

SOURCE_HEADER="${1}"
TARGET_HEADER="${2}"
VERSIONFILE="${3}"

REV_TEMPLATE="@SPRINGLOBBY_REV@"

#don't fail if no .git
git branch &> /dev/null || exit 0

REV="$(git describe --tags)"
OLDREV=""
if [ -s ${VERSIONFILE} ]; then
	OLDREV=$(cat ${VERSIONFILE})
fi

if [ "${OLDREV}" != "${REV}" ]; then # version changed, update file
	echo "Updating from version ${OLDREV} to ${REV}"
	echo -n ${REV}>${VERSIONFILE}
	sed "s;${REV_TEMPLATE};${REV};g" ${SOURCE_HEADER} > ${TARGET_HEADER}
fi

