set(update_RC_FILE updater.rc)
SET(updaterSrc
	updatermainwindow.cpp
	updaterapp.cpp
	../utils/activitynotice.cpp
	../utils/misc.cpp
	${updater_RC_FILE}
)

if(WIN32)
	option(SPRINGLOBBY_UPDATER "Compile the springlobby_updater" ON)
else()
	option(SPRINGLOBBY_UPDATER "Compile the springlobby_updater" OFF)
endif()
if (SPRINGLOBBY_UPDATER)
	ADD_EXECUTABLE(springlobby_updater WIN32 MACOSX_BUNDLE ${updaterSrc} ${updater_RC_FILE} )
	TARGET_LINK_LIBRARIES(springlobby_updater
			libsettings
			lsl-utils
			lsl-unitsync
			springlobbyversion
			slcommon
			${wxWidgets_LIBRARIES}
			${CURL_LIBRARIES}
			${CURL_LINKFLAGS}
			${CMAKE_DL_LIBS}
		)
	install(TARGETS springlobby_updater RUNTIME DESTINATION .)
endif()

