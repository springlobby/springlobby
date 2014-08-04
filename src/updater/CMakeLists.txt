set(update_RC_FILE updater.rc)
SET(updaterSrc
	updatermainwindow.cpp
	updaterapp.cpp
	../gui/activitynotice.cpp
	../utils/platform.cpp
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
			springlobbyversion
			${WX_LD_FLAGS}
		)
	install(TARGETS springlobby_updater RUNTIME DESTINATION .)
endif()

