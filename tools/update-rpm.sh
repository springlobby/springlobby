#!/usr/bin/env bash

set -e

cd $(dirname $0)/..

if [ $# -ne 1 ] ; then
	echo "Usage: update-rpm version"
	exit
fi

version=$1

if [ ! -d rpm/home:accAgon/SpringLobby ] ; then
    ( cd rpm ; osc co home:accAgon SpringLobby )
fi

cd rpm/home:accAgon/SpringLobby
osc up
echo "<services><service name=\"set_version\"><param name=\"basename\">springlobby</param><param name=\"version\">" > _service
echo $version >> _service
echo "</param><param name=\"file\">springlobby.spec</param></service><service name=\"tar_scm\"><param name=\"url\">git://springlobby.info/git/buildbot/springlobby.git</param><param name=\"filename\">springlobby.tar</param><param name=\"revision\">master</param><param name=\"scm\">git</param><param name=\"version\">" >> _service
echo $version >> _service
echo "</param></service></services>" >> _service
osc ci -m "autobuild ${version}"
