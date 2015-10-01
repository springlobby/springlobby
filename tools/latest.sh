#!/bin/bash
cd /data/www/springlobby.info/windows
rm latest.zip
ln -sv springlobby-${1}-win32.zip latest.zip
