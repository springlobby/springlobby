#!/bin/bash
# This script autogenerates the Makefile.am in the flags dir
#

echo
echo -e "springlobby_SOURCES = \\"
for img in `ls *.xpm`
do
  echo -n "$img "
done
echo