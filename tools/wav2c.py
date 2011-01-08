#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import os
import os.path
import re
import array

## no need to call this manually, use make_header_from_png.sh instead
## 

USAGE = """png2c - Embed a PNG in a C header file (like XPM)
Usage: png2c [file ..] Convert all the file <file.png> to <file.png.h>"""
 
if len(sys.argv) < 2:
	print USAGE
	sys.exit(1)
 
for path in sys.argv[1:]:
	filename = os.path.basename(path)
 
	# Read PNG file as character array
	bytes = array.array('B', open(path, "rb").read())
	count = len(bytes)
 
	# Create the C header
	text = "/* %s - %d bytes */\n static const unsigned char %s_data[] = {\n" % (filename, count, filename)
 
	# Iterate the characters, we want
	# lines like:
	#   0x01, 0x02, .... (8 values per line maximum)
	i = 0
	count = len(bytes)
	for byte in bytes:
		# Then the hex data (up to 8 values per line)
		text += "0x%02x" % (byte)
		# Separate all but the last values 
		if (i + 1) < count:
			text += ","
		i += 1
 
	# Now conclude the C source
	text += "};\n/* End Of File */\n"
	open(path + ".h", 'w').write(text)
