#!/usr/bin/env bash

set -e
cd $(dirname $0)/..

if [ $# -ne 1 ] ; then
	echo "Usage: update-ebuilds tarball-filename"
	exit
fi

if [ x$(hostname) = xDelenn ] ; then
	cd ~/springlobby/gentoo/overlay/games-util/springlobby
	git-pull
	cp springlobby-template $1
	ebuild $1 digest
	git-add .
	git-commit -m "automatic update of springlobby ebuild for automated tarball release"
	git-push my-public
else
	ssh zi.fi ~/springlobby/tools/update-ebuilds.sh $*
fi

