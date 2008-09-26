#! /bin/bash

REVS1=`git tag -l "r*"`
REVS2=`git tag -l "0*"`
REVS1=($REVS1)
REVS2=($REVS2)

REVS=( ${REVS1[@]} ${REVS2[@]} )

NUMREVS=${#REVS[*]}

for i in $(seq 1 $NUMREVS) ; do
    echo
    echo "diff ${REVS[$i-1]} ${REVS[$i]}"
    git diff ${REVS[$i-1]} ${REVS[$i]} --shortstat
    
 done
