NAME=springlobby

usage() {
    echo "$0 URL [RELEASE] [SERIES] .."
    exit 1
}

test $# -lt 1 && usage


PPA=$1
shift

RELEASE=1
test $# -ge 1 && RELEASE=$1 && shift

SERIES=${*:-"precise quantal raring saucy"}



VERSION=`head -1 debian/changelog | grep -o '[0-9.]*' | head -1`
ORIG_SERIES=`head -1 debian/changelog | sed 's/.*) \(.*\);.*/\1/'`

for S in $SERIES ;
do
  cp debian/changelog debian/changelog.backup
  sed -i "s/[0-9]*ubuntu.*)/0ubuntu1~${S}${RELEASE})/;s/${ORIG_SERIES}/${S}/" debian/changelog
  git-buildpackage -S --git-ignore-new
  dput ${PPA} ../${NAME}_${VERSION}-0ubuntu1~${S}${RELEASE}_source.changes
  mv debian/changelog.backup debian/changelog
done
