#!/usr/bin/env bash

set -e
#ie /home/buildbot/slaves/springlobby/release/
cd $(dirname $0)/../..

if [ $# -ne 1 ] ; then
	echo "Usage: update-ebuilds version"
	exit
fi

version=$1

overlay_origin=GG:git/koshi/overlay.git

if [ ! -d overlay ] ; then
    git clone ${overlay_origin} overlay
fi

cd overlay
git fetch
git reset --hard 
git checkout master
git reset --hard 
git merge origin/master
cd games-util/springlobby
cp springlobby-template springlobby-${version}.ebuild
ebuild springlobby-${version}.ebuild digest
git add .
git commit -m "automatic update of springlobby ebuild for automated tarball release"
git push origin
