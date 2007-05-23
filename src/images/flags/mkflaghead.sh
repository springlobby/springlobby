#!/bin/bash

echo
echo \#ifndef _FLAGIMAGES_H_
echo \#define _FLAGIMAGES_H_
echo

for img in `ls *.xpm`
do
#  convert -page 25x25+0+4 -mosaic $img -crop 25x25\! -delete 1 -resize 16x16 $img.xpm
  echo \#include \"images/flags/`echo $img`\"
#`echo $img | sed s/.xpm/""/`\"
done
echo
echo enum {
echo "  FLAG_NONE = -1,"

for img in `ls *.xpm`
do
  echo "  FLAG_`echo $img | sed s/.xpm/""/`,"
done

echo "  FLAG_LAST"
echo };

echo
echo \#endif
echo
