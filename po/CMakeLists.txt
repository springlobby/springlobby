find_program(GETTEXT_MSGFMT_EXECUTABLE msgfmt)

if(NOT GETTEXT_MSGFMT_EXECUTABLE)
	message(WARNING "sgfmt not found. Translations will *not* be installed")
else()
	set(catalogname springlobby)
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
		zh_CN)

	set(mofiles "")
	foreach(_lang ${LANGUAGES})
		set(moDir ${CMAKE_CURRENT_BINARY_DIR}/locale/${_lang}/LC_MESSAGES)
		set(moFile ${moDir}/${catalogname}.mo)
		set(poFile ${CMAKE_CURRENT_SOURCE_DIR}/${_lang}.po)
		file(MAKE_DIRECTORY ${moDir})
		add_custom_command(
			OUTPUT ${moFile}
			COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -f --check --strict -v -o ${moFile} ${poFile}
			DEPENDS ${poFile})
		install(FILES ${moFile} DESTINATION ${LOCALE_INSTALL_DIR}/${_lang}/LC_MESSAGES/)
		list(APPEND mofiles ${moFile})
	endforeach()
	add_custom_target(translations ALL DEPENDS ${mofiles})
endif()

file(GLOB_RECURSE srcfiles RELATIVE ${springlobby_SOURCE_DIR}/ FOLLOW_SYMLINKS
	${springlobby_SOURCE_DIR}/src/*.h
	${springlobby_SOURCE_DIR}/src/*.cpp
	${springlobby_SOURCE_DIR}/src/*.hpp
)
list(SORT srcfiles)
set(MESSAGES_POT_FILE ${springlobby_SOURCE_DIR}/po/springlobby.pot)

find_program(XGETTEXT NAMES "xgettext")

if(${XGETTEXT} STREQUAL "XGETTEXT-NOTFOUND")
	message(STATUS "Warning: xgettext not found - will not update springlobby.pot")
else()
	message(STATUS "Found xgettext: ${XGETTEXT}")
	add_custom_target(
		update-potfile
		COMMAND ${XGETTEXT} -d springlobby --keyword=_ --from-code=utf-8 --no-wrap --directory ${springlobby_SOURCE_DIR} --msgid-bugs-address "https://github.com/springlobby/springlobby/issues" -o ${MESSAGES_POT_FILE} ${srcfiles}
		DEPENDS springlobby srcfiles
		WORKING_DIRECTORY ${springlobby_SOURCE_DIR})
endif()

find_program(MSGMERGE NAMES "msgmerge")
if(${MSGMERGE} STREQUAL "MSGMERGE-NOTFOUND")
	message(STATUS "Warning: msgmerge not found - will not update po files")
else()
	message(STATUS "Found msgmerge: ${MSGMERGE}")

	add_custom_target(update-po)

	foreach(_poFile ${LANGUAGES})
		add_custom_command(TARGET update-po
			COMMAND ${MSGMERGE} --no-fuzzy-matching -vU ${_poFile}.po ${MESSAGES_POT_FILE}
			DEPENDS ${_poFile} ${MESSAGES_POT_FILE})
	endforeach()
endif()

add_custom_target(update-translations DEPENDS update-potfile update-po)

