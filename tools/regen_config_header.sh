#!/bin/sh

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

REV="$(git describe --tags)" 2>/dev/null

if [ -z "$REV" ]; then
	REV="unknown"
fi

OLDREV=""
if [ -s ${VERSIONFILE} ] && [ -s ${TARGET_HEADER} ]; then
	OLDREV=$(cat ${VERSIONFILE})
fi

if [ "${OLDREV}" != "${REV}" ]; then # version changed, update file
	echo "Updating from version ${OLDREV} to ${REV}"
	echo -n ${REV}>${VERSIONFILE}
	sed "s;${REV_TEMPLATE};${REV};g" ${SOURCE_HEADER} > ${TARGET_HEADER}
fi

