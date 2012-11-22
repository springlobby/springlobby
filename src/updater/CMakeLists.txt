clear( updaterSrc)
add_sources( updaterSrc
	updatermainwindow.cpp
	updater.cpp
	updaterapp.cpp
	../utils/activitynotice.cpp
	../utils/misc.cpp
)
CreateResourceCompileCommand(updater_RC_FILE ${CMAKE_CURRENT_SOURCE_DIR} updater.rc su_icon.o )
SET(updaterSrc ${updaterSrc} ${updater_RC_FILE})

ADD_EXECUTABLE(springlobby_updater WIN32 MACOSX_BUNDLE ${updaterSrc} ${SLSharedWithSettings} ${updater_RC_FILE} )
TARGET_LINK_LIBRARIES(springlobby_updater ${wxWidgets_LIBRARIES} libsettings  )
install(TARGETS springlobby_updater RUNTIME DESTINATION .)
