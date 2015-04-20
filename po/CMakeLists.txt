FIND_PROGRAM(GETTEXT_MSGFMT_EXECUTABLE msgfmt)

IF(NOT GETTEXT_MSGFMT_EXECUTABLE)
	MESSAGE(
"------
                 NOTE: msgfmt not found. Translations will *not* be installed
------")
ELSE(NOT GETTEXT_MSGFMT_EXECUTABLE)

	SET(catalogname springlobby)
	FILE(GLOB PO_FILES  ${CMAKE_SOURCE_DIR}/po/*.po)

	SET(gmofiles "")
	FOREACH(_poFile ${PO_FILES})
		GET_FILENAME_COMPONENT(_poFileName ${_poFile} NAME)
		STRING(REGEX REPLACE "^${catalogname}_?" "" _langCode ${_poFileName} )
		STRING(REGEX REPLACE "\\.po$" "" _langCode ${_langCode} )
		IF( _langCode )
			GET_FILENAME_COMPONENT(_lang ${_poFile} NAME_WE)
			SET(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)

			ADD_CUSTOM_COMMAND(
				OUTPUT ${_gmoFile}
				COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} --check -o ${_gmoFile} ${_poFile}
				DEPENDS ${_poFile})
			INSTALL(FILES ${_gmoFile} DESTINATION ${LOCALE_INSTALL_DIR}/${_langCode}/LC_MESSAGES/ RENAME ${catalogname}.mo)
			LIST(APPEND gmofiles ${_gmoFile})
		ENDIF( _langCode )

	ENDFOREACH(_poFile ${PO_FILES})
	add_custom_target(translations ALL DEPENDS ${gmofiles})
ENDIF(NOT GETTEXT_MSGFMT_EXECUTABLE)

file(GLOB_RECURSE srcfiles RELATIVE ${springlobby_SOURCE_DIR}/ FOLLOW_SYMLINKS
	${springlobby_SOURCE_DIR}/src/*.h
	${springlobby_SOURCE_DIR}/src/*.cpp
	${springlobby_SOURCE_DIR}/src/*.hpp
)
list(SORT srcfiles)

FIND_PROGRAM( XGETTEXT NAMES "xgettext" )

IF(${XGETTEXT} STREQUAL "XGETTEXT-NOTFOUND")
	MESSAGE(STATUS "Warning: xgettext not found - will not update springlobby.pot")
ELSE(${XGETTEXT} STREQUAL "XGETTEXT-NOTFOUND")
	MESSAGE(STATUS "Found xgettext: ${XGETTEXT}")
	SET(MESSAGES_POT_FILE ${springlobby_SOURCE_DIR}/po/springlobby.pot)
	ADD_CUSTOM_TARGET(
		update-potfile
		COMMAND ${XGETTEXT} -k_ --from-code=utf-8 -C --no-wrap --directory ${springlobby_SOURCE_DIR} --msgid-bugs-address "devel@www.springlobby.info" -o ${MESSAGES_POT_FILE} ${srcfiles}
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
	FILE(GLOB PO_FILES  ${CMAKE_SOURCE_DIR}/po/*.po)
	SET( POTFILE ${springlobby_SOURCE_DIR}/po/springlobby.pot )

	FOREACH(_poFile ${PO_FILES})
			ADD_CUSTOM_COMMAND(TARGET update-po
						COMMAND ${MSGMERGE} -vU ${_poFile} ${POTFILE} 
						DEPENDS ${_poFile})
	ENDFOREACH(_poFile ${PO_FILES})
  
ENDIF(${MSGMERGE} STREQUAL "MSGMERGE-NOTFOUND")

ADD_CUSTOM_TARGET( update-translations
		DEPENDS update-potfile update-po)

