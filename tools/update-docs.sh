#! /bin/sh

set -e
cd $(dirname $0)/..

doxygen

rsync -aqzh doc/html /usr/local/www/springlobby.info/docs
