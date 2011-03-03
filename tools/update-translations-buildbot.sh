#!/usr/bin/env bash

set -e
cd $(dirname $0)/..

#git remote add translations root@springlobby.info:/home/pootle/translations/
git fetch translations 
git merge translations/master
make -C po update-po && make -C po update-pot && git commit -a -m "auto-updated translations"

#for good measure
exit 0
