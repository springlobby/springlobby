#!/usr/bin/env python3


import subprocess, os, shutil, glob

print(os.getcwd())


strip = "/opt/mingw32/usr/bin/i686-w64-mingw32-strip"

version = subprocess.check_output(["git", "describe", "--tags"], universal_newlines=True).strip()
assert(len(version) > 0)

if "-" in version:
	outdir = "develop"
	verfile = "version-develop.txt"
else:
	outdir = "stable"
	verfile = "version.txt"

os.makedirs(outdir, exist_ok=True)
verfile = os.path.join(outdir, verfile)

with open(verfile, "w") as f:
	f.write(version)


installer = glob.glob("SpringLobby-*.exe")
zip = glob.glob("springlobby-*-win32.zip")


for srcfile in installer + zip:
	print(srcfile, " -> ", outdir)
	shutil.move(srcfile, outdir)

print("Rsync...")
print(subprocess.check_output(["rsync", "-av", "--remove-source-files", outdir, "springlobby@springrts.com:/home/springlobby/www/" ], timeout = 600, universal_newlines=True))


