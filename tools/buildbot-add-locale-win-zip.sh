#!/usr/bin/env bash

set -e
cd $(dirname $0)/../build-mingw-wx28/

zip -ur ../../../../../${1}  locale
