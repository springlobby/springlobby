#! /usr/bin/env bash

GIT_DIR=$(git rev-parse --git-dir 2>/dev/null)
if [ -z "$GIT_DIR" ]; then
        echo >&2 "fatal: post-receive: GIT_DIR not set"
        exit 1
fi

cd ${GIT_DIR}

REVS1=`git tag -l "r*"`
REVS2=`git tag -l "0*"`
REVS1=($REVS1)
REVS2=($REVS2)

REVS=( ${REVS1[@]} ${REVS2[@]} )

NUMREVS=${#REVS[*]}
OFILE=/tmp/git.html

cat /dev/null > $OFILE
echo "<html><body><ul>" >> $OFILE

for i in $(seq $NUMREVS -1 1) ; do
        if [ $i == $NUMREVS ] ; then
		LINES=$(git diff HEAD ${REVS[$i-1]}  --shortstat)
                echo "<li>HEAD [ $LINES ]:<ul>" >> $OFILE
        else
		LINES=$(git diff ${REVS[$i-1]} ${REVS[$i]} --shortstat) #<-- that's the stats
                echo "<li>${REVS[$i]} [ $LINES ]:<ul>" >> $OFILE
        fi

        git log ${REVS[$i-1]}..${REVS[$i]} --pretty=format:"<li>%s</li>" | grep -v Merge >> $OFILE
        echo "</ul></li>" >> $OFILE
done

echo "</ul></body></html>" >> $OFILE
