#!/usr/bin/env bash

set -e

cd $(dirname $0)/..

if [ $# -ne 1 ] ; then
	echo "Usage: update-rpm version"
	exit
fi

version=$1
tarball=springlobby-${version}.tar.bz2

if [ ! -d rpm/home:accAgon/SpringLobby ] ; then
    ( cd rpm ; osc co home:accAgon SpringLobby )
fi

cd rpm/home:accAgon/SpringLobby
osc up
find . -name springlobby-0.0.1.\*.tar.bz2 -exec osc rm {} \;
sed -i 's/^\(%define app_version\) .*/\1 '${version}'/' springlobby_tarball.spec
cp /usr/local/www/springlobby.info/tarballs/${tarball} .
osc add ${tarball}
osc ci -m "autobuild ${version}"

