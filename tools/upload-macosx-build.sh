#!/bin/sh

# exit on error
set -e

#input from buildbot is "build-dir developer version"

#make sure we're in the right build dir
cd $(dirname $0)/../${1}
DEVELOPER=${2}

VERSION="$(git describe --tags)"

OUTPUTDIR=/tmp/springlobby/${VERSION}/

rm -rf ${OUTPUTDIR}/*

# TODO: fix hardcoded prefix
PREFIX=/usr/local

make install DESTDIR=${OUTPUTDIR}

BUNDLENAME=SpringLobby_${VERSION}.app

INSTALLPATH=${OUTPUTDIR}/${PREFIX}
BUNDLEPATH=${OUTPUTDIR}/${BUNDLENAME}

# Move and rename current springlobby.app
mv ${INSTALLPATH}/bin/springlobby.app ${BUNDLEPATH}

# Move springsettings executable next to springlobby
mv ${INSTALLPATH}/bin/springsettings.app/Contents/MacOS/springsettings ${BUNDLEPATH}/Contents/MacOS/

# Move shared content inside the bundle => Change SL code to fetch content there instead of ${PREFIX}
mv ${INSTALLPATH}/share ${BUNDLEPATH}/Contents/

cd ${BUNDLEPATH}/Contents/

# create lib dir to receive 3rd-party libs
mkdir lib

echo "copying 3rd-party libraries"
for i in `ls MacOS`
do
   # for each dylib required by the executable
   for j in `otool -L MacOS/$i | egrep -o opt/local.*dylib | egrep -o lib.*dylib`
   do 
      #echo "copying $j to the lib folder"
      cp /opt/local/$j lib
      chmod u+w $j
      strip -r -u $j
      
      # adjust it's id to it's new location
      install_name_tool -id @executable_path/../$j $j
      install_name_tool -change /opt/local/$j @executable_path/../$j MacOS/$i
   done
done

# continue with the recursive dependencies
end=0
while [ $end -eq 0 ]
do
   end=1
   # for each dylib in the folder
   for i in `ls lib`
   do
      #echo "processing $i:"
      # check if all dependencies are present
      for j in `otool -L lib/$i | egrep -o /opt/local.*dylib | egrep -o lib.*dylib`
      do
         #echo "lib required: $j"
         if [ ! -f $j ]
         then
            #echo "bundling $j"
            cp /opt/local/$j lib
            chmod u+w $j
            strip -r -u $j
            end=0
         fi
         install_name_tool -id @executable_path/../$j $j
         install_name_tool -change /opt/local/$j @executable_path/../$j lib/$i
      done
   done
done

ARCHIVE_NAME=${BUNDLENAME}-MacOSX-10.6-SnowLeopard.zip
zip -r9 ${OUTPUTDIR}/${ARCHIVE_NAME} ${OUTPUTDIR}/${BUNDLE_NAME}
rsync --port 50 -azv --bwlimit 50  ${OUTPUTDIR}/${ARCHIVE_NAME} buildbot@springlobby.info:/usr/local/www/springlobby.info/macosx/${DEVELOPER}/
rm -rf ${OUTPUTDIR}
