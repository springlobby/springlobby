#!/bin/sh
# Determine a revision or version string to use for the current source
# tree.

# It's actually this simple.  We might want to keep the script around
# in case we want to re-introduce subversion compatibility.
git-describe --tags
