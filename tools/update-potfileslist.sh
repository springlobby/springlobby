#!/usr/bin/env bash
#
# Usage: $(basename $0) [PROJECTROOT]
#
# Update the list of files containing translatable strings.
#
# First argument, if given, is the location of the project root.  If
# it is omitted, we determine it from the path of this script.
if test x"$1" != x; then
    # Use the passed directory name, sans any trailing slash.
    SPRINGLOBBY_BASE_DIR="${1%/}"
else
    SPRINGLOBBY_BASE_DIR="$(dirname $0)/.."
fi

grep -lR \
    --exclude-dir=libtorrent \
    --exclude=\*.xpm \
    --exclude=\*.png.h \
    '_(' "${SPRINGLOBBY_BASE_DIR}/src/" \
    | sort -u \
    | sed -e "s#${SPRINGLOBBY_BASE_DIR}/##" \
    > "${SPRINGLOBBY_BASE_DIR}"/po/POTFILES.in
