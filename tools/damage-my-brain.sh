#!/usr/bin/env bash

set -e

cd ~/checkouts/git/springlobby/
git-checkout master
git-fetch $1
git-merge $1/master
git-svn -i trunk set-tree HEAD
git-tag $(git-svn log --limit 1 --oneline | cut -d' ' -f1)
git-push --tags my-public

