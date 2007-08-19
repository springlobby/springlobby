#!/usr/bin/env bash
# dunno if it would work with plain sh, please verify
# usage: tools/newfile src/myfilename [h] [cpp]
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
  sed -i 's,^springlobby_SOURCES = \\$,&\n\t'$1' \\,' Makefile.am
  svn add $1
  svn ps svn:eol-style native $1
}

for ext in ${exts} ; do
  src=tools/template.${ext}
  dst=${dir}/${base}.${ext}
  if [ -f ${src} ] ; then
    sed "s/XXX/${base_upper}/g" < ${src} > ${dst}
    add_sourcefile_to_project ${dst}
  elif [ ${ext} = xpm ] ; then
    # work around g++ >=4.2 and included xpm files
    # Warning: deprecated conversion from string constant to »char*«
    sed -i 's/^static char/static const char/' ${dst}
    add_sourcefile_to_project ${dst}
  else
    echo bad extension ${ext} dunno how to process, ignoring
  fi
done
exit 0
