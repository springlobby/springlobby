clear( updaterSrc)
add_sources( updaterSrc
	updatermainwindow.cpp
	updater.cpp
	updaterapp.cpp
	../utils/activitynotice.cpp
	../utils/misc.cpp
)
set(update_RC_FILE updater.rc)
SET(updaterSrc ${updaterSrc} ${updater_RC_FILE})

ADD_EXECUTABLE(springlobby_updater WIN32 MACOSX_BUNDLE ${updaterSrc} ${SLSharedWithSettings} ${updater_RC_FILE} )
TARGET_LINK_LIBRARIES(springlobby_updater
		${wxWidgets_LIBRARIES}
		libsettings
		lsl-utils
		lsl-unitsync
		${CURL_LIBRARIES}
		${CURL_LINKFLAGS}
	)
install(TARGETS springlobby_updater RUNTIME DESTINATION .)
