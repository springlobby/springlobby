# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# - Run Doxygen
#
# Adds a doxygen target that runs doxygen to generate the html
# and optionally the LaTeX API documentation.
# The doxygen target is added to the doc target as dependency.
# i.e.: the API documentation is built with:
#  make doc
#
# USAGE: GLOBAL INSTALL
#
# Install it with:
#  cmake ./ && sudo make install
# Add the following to the CMakeLists.txt of your project:
#  include(UseDoxygen OPTIONAL)
# Optionally copy Doxyfile.in in the directory of CMakeLists.txt and edit it.
#
# USAGE: INCLUDE IN PROJECT
#
#  set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
#  include(UseDoxygen)
# Add the Doxyfile.in and UseDoxygen.cmake files to the projects source directory.
#
#
# Variables you may define are:
#  DOXYFILE_OUTPUT_DIR - Path where the Doxygen output is stored. Defaults to "doc".
#
#  DOXYFILE_LATEX_DIR - Directory where the Doxygen LaTeX output is stored. Defaults to "latex".
#
#  DOXYFILE_HTML_DIR - Directory where the Doxygen html output is stored. Defaults to "html".
#

#
#  Copyright (c) 2009 Tobias Rautenkranz <tobias@rautenkranz.ch>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

macro(usedoxygen_set_default name value)
	if(NOT DEFINED "${name}")
		set("${name}" "${value}")
	endif()
endmacro()

find_package(Doxygen)

if(DOXYGEN_FOUND)
	find_file(DOXYFILE_IN "Doxyfile.in"
			PATHS "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_ROOT}/Modules/")

	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(Doxyfile.in DEFAULT_MSG DOXYFILE_IN)
endif()

if(DOXYGEN_FOUND AND DOXYFILE_IN)
	add_custom_target(doxygen ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

	usedoxygen_set_default(DOXYFILE_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc")
	usedoxygen_set_default(DOXYFILE_HTML_DIR "html")

	set_property(DIRECTORY APPEND PROPERTY
			ADDITIONAL_MAKE_CLEAN_FILES "${DOXYFILE_OUTPUT_DIR}/${DOXYFILE_HTML_DIR}")

	set(DOXYFILE_LATEX "NO")
	set(DOXYFILE_PDFLATEX "NO")
	set(DOXYFILE_DOT "NO")

	find_package(LATEX)
	if(LATEX_COMPILER AND MAKEINDEX_COMPILER)
		set(DOXYFILE_LATEX "YES")
		usedoxygen_set_default(DOXYFILE_LATEX_DIR "latex")

		set_property(DIRECTORY APPEND PROPERTY
				ADDITIONAL_MAKE_CLEAN_FILES
				"${DOXYFILE_OUTPUT_DIR}/${DOXYFILE_LATEX_DIR}")

		if(PDFLATEX_COMPILER)
			set(DOXYFILE_PDFLATEX "YES")
		endif()
		if(DOXYGEN_DOT_EXECUTABLE)
			set(DOXYFILE_DOT "YES")
		endif()
	endif()


	configure_file(${DOXYFILE_IN} Doxyfile ESCAPE_QUOTES IMMEDIATE @ONLY)

	get_target_property(DOC_TARGET doc TYPE)
	if(NOT DOC_TARGET)
		add_custom_target(doc)
	endif()
		
	add_dependencies(doc doxygen)
endif()
