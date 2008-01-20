#!/usr/bin/env bash

set -e
cd $(dirname $0)/..

if [ $# -ne 1 ] ; then
	echo "Usage: update-ebuilds tarball-filename-without-extension"
	exit
fi

if [ x$(hostname) = xDelenn ] ; then
	cd ~/springlobby/gentoo/overlay/games-util/springlobby
	git-pull
	cp springlobby-template $1.ebuild
	ebuild $1 digest
	git-add .
	git-commit -m "automatic update of springlobby ebuild for automated tarball release"
	git-push my-public
else
	ssh zi.fi sh ~/springlobby/tools/update-ebuilds.sh $*
fi

