#! /bin/bash

# Quit on error.
set -e

if [ ! -e installer ]; then
	echo "Error: This script needs to be run from the root directory of the archive"
	exit 1
fi

TAG=$(git describe --tags|tr -d '\n')

if [ "$TAG" = "" ]; then
	echo "Error running git describe"
	exit 1
fi
NSISDEFINES="-DVERSION_TAG="$TAG""

# Evaluate the engines version
if ! git describe --candidate=0 --tags 2>/dev/null; then
	NSISDEFINES="$NSISDEFINES -DTEST_BUILD"
	echo "Creating test installer for revision $TAG"
fi

#create uninstall.nsh
installer/make_uninstall_nsh.py $1 >installer/uninstall.nsh

makensis -V3 $NSISDEFINES -DNSI_UNINSTALL_FILES=uninstall.nsh \
-DSP_BASENAME=$(pwd)/build-msw-default/springlobby-$TAG \
-DSPRINGLOBBY_ARCHIVE=$(pwd)/$1 \
 installer/springlobby.nsi

