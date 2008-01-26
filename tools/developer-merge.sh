#!/usr/bin/env bash

set -e
cd $(dirname $0)/..

if [ $# -ne 1 ] ; then
    echo "Usage: damage-my-brain who"
    exit
fi

who=$1

git-checkout master
git-fetch ${who}
git-merge ${who}/master
#git-svn -i trunk set-tree HEAD
#git-tag $(git-svn log --limit 1 --oneline | cut -d' ' -f1)
#git-push --tags
git-push
