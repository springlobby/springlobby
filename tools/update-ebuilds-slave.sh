#!/usr/bin/env bash

set -e
cd $(dirname $0)/..

if [ $# -ne 0 ] ; then
	echo "Usage: update-ebuilds version"
    false
	exit
fi

publicrepodir=/var/lib/buildbot/git/springlobby.git
if [ ! -d ${publicrepodir} ] ; then
    git-clone --bare . ${publicrepodir}
    ( cd ${publicrepodir} ; git-gc --prune --aggressive )
    git-remote add -f my-public ${publicrepodir}
fi

version=$(git-describe --tags | sed 's/-.*//')
cd gentoo/overlay/games-util/springlobby
cp springlobby-template springlobby-${version}.ebuild
ebuild springlobby-${version}.ebuild digest
git-add .
git-commit -m "automatic update of springlobby ebuild for automated tarball release"
git-push my-public

