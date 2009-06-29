#!/usr/bin/env bash
# dunno if it would work with plain sh, please verify
# usage: tools/newfile src/myfilename [h] [cpp] [xpm]
# not a portable app in the sense that it expects a lot of our dir hierarchy and it's own location
# but you can be anywhere to call it
# for xpm assume the file is moved to correct location before running this

set -e
cd $(dirname $0)/..

base=$(basename $1)
dir=$(dirname $1)
base_upper=$(echo $base | tr [:lower:] [:upper:])
shift
exts=$*

# takes 1 parameter, f.ex. src/images/foo.xpm
add_sourcefile_to_project () {
  echo adding $1
  sed -i 's,^springlobby_SOURCES = \\$,&\n\t'$1' \\,' Makefile.am
  if [ $2 = cpp ] ; then
	sed -i 's,^SET(SpringLobbySrc$,&\n\t'$1',' CMakeLists.txt
	git add CMakeLists.txt
  fi
  sed -i 's,.<Project>$,&\n\t<Unit filename=\"'$1'\" />,' springlobby.cbp
  git add $1
  git add Makefile.am
  git add springlobby.cbp
  
}

for ext in ${exts} ; do
  src=tools/template.${ext}
  dst=${dir}/${base}.${ext}
  if [ -f ${src} ] ; then
    if [ -f ${dst} ] ; then
      echo file already exists: ${dst}, skipping
    else
      sed "s/XXX/${base_upper}/g" < ${src} > ${dst}
      add_sourcefile_to_project ${dst} ${ext}
    fi
  elif [ ${ext} = xpm ] ; then
    # work around g++ >=4.2 and included xpm files
    # Warning: deprecated conversion from string constant to »char*«
    sed -i 's/^static char/static const char/' ${dst}
    add_sourcefile_to_project ${dst} ${ext}
  else
    echo bad extension ${ext} dunno how to process, ignoring
  fi
done
exit 0
