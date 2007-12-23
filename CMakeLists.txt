PROJECT(SpringLobby)

#if you don't want the full Compiler-Output, remove the following Line
SET(CMAKE_VERBOSE_MAKEFILE ON)

# Add Definitions, Compiler-Switches, etc.: -Wall -O2 -g3 ...
ADD_DEFINITIONS(-Wall -g3)

#----------------------------------------------------------------------------------------------------
# Konfiguration of wxWidgets Configuration
#----------------------------------------------------------------------------------------------------

SET(CMAKE_FIND_LIBRARY_PREFIXES "")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")

# Here you can define, what Libraries of wxWidgets you need for your Application. You can figure out what Libraries you need here;
# http://www.wxwidgets.org/manuals/2.8/wx_librarieslist.html
SET(wxWidgets_USE_LIBS base core gl net adv)
 
# We need the Find package for wxWidgets to work
FIND_PACKAGE(wxWidgets)

# Did we find wxWidgets ? This condition will fail for as long as the internal Vars do not point to the proper wxWidgets Configuration.
IF(wxWidgets_FOUND)
     
	# Include wxWidgets macros
	INCLUDE(${wxWidgets_USE_FILE})
 
	# We define the include paths here, our minimal source dir is one,
	# and also the include dirs defined by wxWidgets
	INCLUDE_DIRECTORIES(${src_SOURCE_DIR} ${wxWidgets_INCLUDE_DIRS} )

	ADD_SUBDIRECTORY( src )

ELSE(wxWidgets_FOUND)
	# For Convenience. If we cannot continue, inform the User.
	MESSAGE("wxWidgets not found!")
ENDIF(wxWidgets_FOUND)
