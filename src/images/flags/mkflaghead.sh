#!/usr/bin/env bash

# This script autogenerates a flagimagedata.cpp file

echo '/* This file is part of the Springlobby (GPL v2 or later), see COPYING */'
echo ''
echo '// this header file is automatically generated with a script'

for img in *.xpm ; do
  echo '#include "images/flags/'${img}'"'
done

echo
echo 'const char* flag_str[] = {'

for img in *.xpm ; do
  echo '"'${img%%.xpm}'",'
done

echo '0'
echo '};'
echo
echo 'const char* const* flag_xpm[] = {'

for img in *.xpm ; do
  echo ${img/./_}','
done

echo '0'
echo '};'
echo
