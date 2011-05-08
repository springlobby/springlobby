#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Strips all STL symbols from the object file %1

import os
import sys

if sys.argv.__len__()!=2:
    print "stl_debug_symbol_remove: Expected precisely the object file"
    sys.exit()

info = os.popen("nm "+sys.argv[1]+" -a -l -s")

cmdLine = "strip"

try:
    for line in info:
      stuff = line.strip().replace('\t',' ').split(" ")        
      if stuff.__len__()==4:
	  if stuff[3].count("/usr/include/c++/")>0:
	    cmdLine = cmdLine + " -N "+stuff[2]

    # Strip symbols
    cmdLine = cmdLine + " " + sys.argv[1]
    print "stl_debug_symbol_remove.py: Calling: " + cmdLine
    os.system(cmdLine)

finally:
    info.close()  
