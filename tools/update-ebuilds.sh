#!/usr/bin/env bash

set -e
cd $(dirname $0)/..

if [ $# -ne 1 ] ; then
	echo "Usage: update-ebuilds version"
	exit
fi

version=$1

if [ x$(hostname) = xDelenn ] ; then
	cd ~/springlobby/gentoo/overlay/games-util/springlobby
	git checkout master
	git pull
	cp springlobby-template springlobby-${version}.ebuild
	ebuild springlobby-${version}.ebuild digest
	git add .
	git commit -m "automatic update of springlobby ebuild for automated tarball release"
	git push my-public
else
	git push
	ssh zi.fi sh ~/springlobby/tools/update-ebuilds.sh $*
	git checkout master
	git fetch zifi
	git merge zifi/master
	git svn -i trunk set-tree HEAD
fi

