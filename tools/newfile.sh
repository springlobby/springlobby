#!/usr/bin/env bash
# dunno if it would work with plain sh, please verify
# usage: tools/newfile myfilename [h] [cpp]
# not a portable app in the sense that it expects a lot of our dir hierarchy and it's own location
# but you can be anywhere to call it

cd $(dirname $0)/..

base=$1
base_upper=$(echo $base | tr [:lower:] [:upper:])
shift
exts=$*

for ext in ${exts} ; do
  src=tools/template.${ext}
  if [ -f ${src} ] ; then
    dst=src/${base}.${ext}
    sed "s/XXX/${base_upper}/g" < ${src} > ${dst}
    sed -i 's,^springlobby_SOURCES = \\$,&\n\t'${dst}' \\,' Makefile.am
    svn add ${dst}
    svn ps svn:eol-style native ${dst}
  else
    echo bad extension ${ext} has no template for it, ignoring
  fi
done
