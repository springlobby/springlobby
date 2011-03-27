#!/bin/bash
/opt/mingw_head/usr/bin/i686-pc-mingw32-strip mapview.exe
7za a -l mapview_current.7z mapview.exe 
filebin mapview_current.7z
