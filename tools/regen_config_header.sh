#!/bin/bash

set -u

SOURCE_HEADER="${1}"
TARGET_HEADER="${2}"

REV_TEMPLATE="@SPRINGLOBBY_REV@"

#don't fail if no .git
git branch &> /dev/null || exit 0

REV="$(git describe --tags)"

sed "s;${REV_TEMPLATE};${REV};g" ${SOURCE_HEADER} > ${TARGET_HEADER}


