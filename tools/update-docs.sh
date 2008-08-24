#! /bin/sh

set -e
cd $(dirname $0)/..

doxygen

rsync -avzh doc/html /usr/local/www/springlobby.info/docs
