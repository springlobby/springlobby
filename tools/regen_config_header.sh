#!/bin/sh

#regen on bbot releases would screw with version
if [ "x${BUILDBOT_RELEASE}" != "x" ] ; then
	echo "not regenerating version on buildbot release"
	exit 0
fi

set -u

SOURCE="$1"
OUTPUT="$2"

REV="$(git -C "$SOURCE" describe --tags)" 2>/dev/null

if [ -z "$REV" ]; then
	REV="unknown"
fi

OLDREV=""
if [ -s "$SOURCE/VERSION" ] && [ -s "$SOURCE/springlobby_config.h" ]; then
	read OLDREV < "$SOURCE/VERSION"
fi

if [ "$OLDREV" != "$REV" ]; then # version changed, update file
	if [ "$REV" = "unknown" ]; then # new version could not be determined
		if [ -n "$OLDREV" ]; then # don't set as old rev is known
			echo "Could not determine version, using old rev $OLDREV"
			exit
		fi
	fi

	echo "Updating from version $OLDREV to $REV"
	echo -n "$REV" > "$OUTPUT/VERSION"
	sed "s;@SPRINGLOBBY_REV@;${REV};g" "$SOURCE/cmake/config.h" > "$OUTPUT/springlobby_config.h"
fi
