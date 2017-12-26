FIND_PROGRAM(GETTEXT_MSGFMT_EXECUTABLE msgfmt)

IF(NOT GETTEXT_MSGFMT_EXECUTABLE)
	MESSAGE(
"------
                 NOTE: msgfmt not found. Translations will *not* be installed
------")
ELSE(NOT GETTEXT_MSGFMT_EXECUTABLE)
	SET(catalogname springlobby)
	# ls *.po
	set(LANGUAGES
		ar
		cs
		da
		de
		el
		es
		fi
		fr
		it
		pl
		pt
		ro
		ru
		sv
		uk
		zh_CN
	)

	SET(mofiles "")
	FOREACH(_lang ${LANGUAGES})
		set(moDir ${CMAKE_CURRENT_BINARY_DIR}/locale/${_lang}/LC_MESSAGES)
		set(moFile ${moDir}/${catalogname}.mo)
		set(poFile ${CMAKE_CURRENT_SOURCE_DIR}/${_lang}.po)
		file(MAKE_DIRECTORY ${moDir})
		ADD_CUSTOM_COMMAND(
			OUTPUT ${moFile}
			COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -f --check --strict -v -o ${moFile} ${poFile}
			DEPENDS ${poFile})
		INSTALL(FILES ${moFile} DESTINATION ${LOCALE_INSTALL_DIR}/${_lang}/LC_MESSAGES/)
		LIST(APPEND mofiles ${moFile})
	ENDFOREACH()
	add_custom_target(translations ALL DEPENDS ${mofiles})
ENDIF(NOT GETTEXT_MSGFMT_EXECUTABLE)

file(GLOB_RECURSE srcfiles RELATIVE ${springlobby_SOURCE_DIR}/ FOLLOW_SYMLINKS
	${springlobby_SOURCE_DIR}/src/*.h
	${springlobby_SOURCE_DIR}/src/*.cpp
	${springlobby_SOURCE_DIR}/src/*.hpp
)
list(SORT srcfiles)
SET(MESSAGES_POT_FILE ${springlobby_SOURCE_DIR}/po/springlobby.pot)

FIND_PROGRAM( XGETTEXT NAMES "xgettext" )

IF(${XGETTEXT} STREQUAL "XGETTEXT-NOTFOUND")
	MESSAGE(STATUS "Warning: xgettext not found - will not update springlobby.pot")
ELSE(${XGETTEXT} STREQUAL "XGETTEXT-NOTFOUND")
	MESSAGE(STATUS "Found xgettext: ${XGETTEXT}")
	ADD_CUSTOM_TARGET(
		update-potfile
		COMMAND ${XGETTEXT} -d springlobby --keyword=_ --from-code=utf-8 --no-wrap --directory ${springlobby_SOURCE_DIR} --msgid-bugs-address "devel@www.springlobby.info" -o ${MESSAGES_POT_FILE} ${srcfiles}
		DEPENDS springlobby
		WORKING_DIRECTORY ${springlobby_SOURCE_DIR}
	)
ENDIF(${XGETTEXT} STREQUAL "XGETTEXT-NOTFOUND")

FIND_PROGRAM( MSGMERGE NAMES "msgmerge" )
IF(${MSGMERGE} STREQUAL "MSGMERGE-NOTFOUND")
	MESSAGE(STATUS "Warning: msgmerge not found - will not update po files")
ELSE(${MSGMERGE} STREQUAL "MSGMERGE-NOTFOUND")
	MESSAGE(STATUS "Found msgmerge: ${MSGMERGE}")

	ADD_CUSTOM_TARGET( update-po )

	FOREACH(_poFile ${LANGUAGES})
			ADD_CUSTOM_COMMAND(TARGET update-po
				COMMAND ${MSGMERGE} --no-fuzzy-matching -vU ${_poFile}.po ${MESSAGES_POT_FILE}
				DEPENDS ${_poFile} ${MESSAGES_POT_FILE})
	ENDFOREACH(_poFile ${PO_FILES})
  
ENDIF(${MSGMERGE} STREQUAL "MSGMERGE-NOTFOUND")

ADD_CUSTOM_TARGET( update-translations
		DEPENDS update-potfile update-po)

