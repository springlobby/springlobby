#!/usr/bin/env bash
#
# Usage: $(basename $0) [PROJECTROOT]
#
# Determine a revision or version string to use for the current source
# tree.

# First argument, if given, is the location of the project root.  Set
# here any environment variables for VCs that care about the project
# root.
if test x"$1" != x; then
    export GIT_DIR="$1/.git"
else
    export GIT_DIR="$(dirname $0)/../.git"
fi

# It's actually this simple.  We might want to keep the script around in
# case we want to re-introduce subversion compatibility.
#i needed to get rid of extra newline
git describe --tags || exit 1

