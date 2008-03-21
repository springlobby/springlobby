#! /bin/sh

convert -background none images/originals/$1.xcf -flatten images/exported/$1.png 
python tools/png2c.py images/exported/$1.png
mv -f images/exported/$1.png.h src/images/
rm -f images/exported/$1.png



