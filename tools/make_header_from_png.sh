#! /usr/bin/env bash

### to be called from project root
### no input arguments
### converts all found png files into c headers 
### and moves them into src/images

cd images/exported
for img in `ls *.png`; do
	python ../../tools/png2c.py $img
done
mv *.h ../../src/images
