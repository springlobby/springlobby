#! /bin/bash

cd images/exported
for img in `ls *.png`; do
	python ../../tools/png2c.py $img
done
mv *.h ../../src/images
