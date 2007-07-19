#!/usr/bin/env bash

# This script autogenerates a flagimagedata.cpp file

for img in *.xpm ; do
  echo '#include "images/flags/'${img}'"'
done

echo
echo 'const char* flag_str[] = {'

for img in *.xpm ; do
  echo '"'${img%%.xpm}'",'
done

echo '""'
echo '};'
echo
echo 'const char* const* flag_xpm[] = {'

for img in *.xpm ; do
  echo ${img/./_}','
done

echo '0'
echo '};'
echo
