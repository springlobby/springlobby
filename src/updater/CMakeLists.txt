if(WIN32)
	set(SPRINGLOBBY_UPDATER_DEFAULT ON)
else()
	set(SPRINGLOBBY_UPDATER_DEFAULT OFF)
endif()

option(SPRINGLOBBY_UPDATER "Compile the springlobby_updater" ${SPRINGLOBBY_UPDATER_DEFAULT})

if (SPRINGLOBBY_UPDATER)
	execute_process(COMMAND ${wxWidgets_CONFIG_EXECUTABLE} "--libs" "core" "${WXSTATIC}"
		        OUTPUT_VARIABLE WX_UPDATER_LD_FLAGS
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	set(update_RC_FILE updater.rc)
	SET(updaterSrc
		updaterapp.cpp
		../utils/platform.cpp
		${updater_RC_FILE}
		springlobby_updater.exe.manifest
	)

	ADD_EXECUTABLE(springlobby_updater WIN32 MACOSX_BUNDLE ${updaterSrc} ${updater_RC_FILE} )
	TARGET_LINK_LIBRARIES(springlobby_updater
			${WX_UPDATER_LD_FLAGS}
		)
	install(TARGETS springlobby_updater RUNTIME DESTINATION .)
endif()

