#!/usr/bin/env bash

set -e
cd $(dirname $0)/..

if [ $# -ne 0 ] ; then
	echo "Usage: update-ebuilds version"
    false
	exit
fi

origin=git@springlobby.info:git/buildbot/springlobby.git

publicrepodir=/home/kosh/buildbot/springlobby
if [ ! -d ${publicrepodir} ] ; then
    git-clone --bare . ${publicrepodir}
    ( cd ${publicrepodir} ; git-gc --prune --aggressive ; touch 'git-daemon-export-ok' )
    git-remote add -f my-public ${publicrepodir}
    git-remote add -f origin ${origin}
fi

if ! $(git-remote show origin > /dev/null) ; then
    git-remote add -f origin ${origin}
fi
git fetch
git reset --hard my-public/master
git merge origin/master
version=$(git describe --tags | sed 's/-.*//')
cd gentoo/overlay/games-util/springlobby
cp springlobby-template springlobby-${version}.ebuild
ebuild springlobby-${version}.ebuild digest
git add .
git commit -m "automatic update of springlobby ebuild for automated tarball release"
git push my-public

