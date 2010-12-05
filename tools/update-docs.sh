#! /usr/bin/env bash

set -e
cd $(dirname $0)/..

doxygen

chmod -R g+rx doc/
