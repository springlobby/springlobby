#! /bin/sh

# Specify the Target of the Out-of-Source-Build
DIR_DEBUG="build-CMake/Debug"

# Specify some Option to start CMake with
CMAKE_OPTIONS="
-DCMAKE_COLOR_MAKEFILE:BOOL=ON
-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
-DCMAKE_BUILD_TYPE:STRING=RELEASE
-DBUILD_SHARED_LIBS:BOOL=ON
"

# Create the Target and change to it
mkdir -p "${DIR_DEBUG}"
cd "${DIR_DEBUG}"

# Generate all Stuff to build the SpringLobby Project and additionally generate a KDevelop Project with all Stuff setup.
cmake "${CMAKE_OPTIONS}" -G KDevelop3 ../../
