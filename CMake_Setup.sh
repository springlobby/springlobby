#! /bin/sh

# Inform the Uses, what we want to do in this Script.
echo -e "\
This Script generate Makefiles from the CMakeLists.txt Files in the Directories of the SpringLobby Project.
It also generates Project-Files for KDevelop, so you just can start working on the Project in KDevelop.
ATTENTION:
If you run this Script all existing KDevelop-Project-Files will be deleted and new Ones will be generated.
So if you already have used this Script, please make Backups of your Project-Files and copy them back afterwards.

Do you want to continue? (1|0):"

# Read the Decision
read DECISION

# Deciding, what to do
if [ "${DECISION}" == "1" ]; then
	echo "Starting Generation...";
else
	echo "Aborting...";
	exit
fi

# Specify the Target of the Out-of-Source-Build
DIR="build-CMake"

# Specify some Option to start CMake with
CMAKE_OPTIONS="
-DCMAKE_COLOR_MAKEFILE:BOOL=ON
-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
-DBUILD_SHARED_LIBS:BOOL=ON
"

# Create the Targets and change to it
mkdir -p "${DIR}/Debug" "${DIR}/Release" "${DIR}/Release_with_DebugInfos"

# Generate all Stuff to build the SpringLobby Project and additionally generate a KDevelop Project with all Stuff setup.
pushd "${DIR}/Debug"
	cmake "${CMAKE_OPTIONS}" -DCMAKE_BUILD_TYPE:STRING=Debug -G KDevelop3 ../../
popd
pushd "${DIR}/Release"
	cmake "${CMAKE_OPTIONS}" -DCMAKE_BUILD_TYPE:STRING=Release -G KDevelop3 ../../
popd
pushd "${DIR}/Release_with_DebugInfos"
	cmake "${CMAKE_OPTIONS}" -DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo -G KDevelop3 ../../
popd
