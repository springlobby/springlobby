#!/usr/bin/env bash
# dunno if it would work with plain sh, please verify
# usage: tools/newfile src/myfilename [h] [cpp] [xpm]
# not a portable app in the sense that it expects a lot of our dir hierarchy and it's own location
# but you can be anywhere to call it
# for xpm assume the file is moved to correct location before running this

set -e
cd $(dirname $0)/..

files=$*

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

for f in ${files} ; do
 	echo ${f}
	ext=$(echo ${f}|awk -F . '{print $NF}')
	if [ -e ${f} ] ; then
		add_sourcefile_to_project ${f} ${ext}
	else
		echo file ${f} does not exist
	fi
done

exit 0
