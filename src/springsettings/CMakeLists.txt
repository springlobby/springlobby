add_sources(SettingsSrc
	helpmenufunctions.cpp
	panel_pathoption.cpp
	se_utils.cpp
	tab_abstract.cpp
	tab_audio.cpp
	tab_quality_video.cpp
	tab_render_detail.cpp
	tab_simple.cpp
	tab_ui.cpp
)
add_subdirectory(hotkeys)

add_sources(springlobbySrc frame.cpp)
ADD_LIBRARY(libsettings STATIC ${SettingsSrc})
TARGET_LINK_LIBRARIES(libsettings ${wxWidgets_LIBRARIES})

ADD_EXECUTABLE(springsettings WIN32 MACOSX_BUNDLE
		frame.cpp
		main.cpp
		${SpringSettings_RC_FILE}
	)

IF(WIN32)
	CreateResourceCompileCommand(springsettings_RC_FILE ${springlobby_SOURCE_DIR}/src settings.rc ss_icon.o )
	install(TARGETS springsettings RUNTIME DESTINATION .)
ELSE(WIN32)
	set(BINDIR  "bin"                CACHE PATH "Where to install binaries")
	install(TARGETS springsettings RUNTIME DESTINATION ${BINDIR} )
ENDIF (WIN32)
TARGET_LINK_LIBRARIES(springsettings
		libsettings
		slcommon
		lsl-utils
		lsl-unitsync
		${wxWidgets_LIBRARIES}
		${CURL_LINKFLAGS}
		${CMAKE_DL_LIBS}
	)

set_target_properties(springsettings PROPERTIES
		COMPILE_FLAGS -DSPRINGSETTINGS_STANDALONE
	)
