#!/usr/bin/env python3


import subprocess, os, shutil, glob, sys


print("cwd: ", os.getcwd())

srcdir = os.path.abspath(sys.argv[1])

print("src: ", srcdir)

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


installer = glob.glob(srcdir + "/springlobby-*.exe")
zip = glob.glob(srcdir + "/springlobby-*-win32.zip")
tgz = glob.glob(srcdir + "/*.tar.gz")
bz2 = glob.glob(srcdir + "/*.tar.bz2")

for srcfile in installer + zip + tgz + bz2:
	print(srcfile, " -> ", outdir)
	shutil.move(srcfile, outdir)

print("Rsync...")
try:
	subprocess.check_output(["rsync", "-av", "--remove-source-files", outdir, "springlobby@springrts.com:/home/springlobby/www/dl/" ], timeout = 600)
except subprocess.CalledProcessError as e:
	print(e.output)
	raise e



