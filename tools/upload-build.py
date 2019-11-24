#!/usr/bin/env python3


import subprocess, os, shutil

print(os.getcwd())


strip = "/opt/mingw32/usr/bin/i686-w64-mingw32-strip"

with open("VERSION") as f:
	version = f.read().strip()
assert(len(version) > 0)

if "-" in version:
	outdir = "develop"
	verfile = "version-develop.txt"
else:
	outdir = "stable"
	verfile = "version.txt"

verfile = os.path.join(outdir, verfile)

with open(verfile, "w") as f:
	f.write(version)

shutil.move("src/springlobby.exe", outdir)
shutil.move("src/springlobby.exe", outdir)

subprocess.checkoutput([strip, "springlobby.exe"])
subprocess.checkoutput([strip, "springlobby_updater.exe"])

print("Rsync...")
subprocess.check_output(["rsync", "-av", outdir, "springlobby@springrts.com:/home/springlobby/www/" ], timeout = 600)

"""
zip -9 -u ${filename} springlobby.exe springlobby_updater.exe
if [ x$3 == x ]; then
	filename=sl_master.zip
else
	filename=sl_${3}.zip
fi
"""

