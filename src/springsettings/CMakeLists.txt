set(SettingsSrc
	panel_pathoption.cpp
	se_utils.cpp
	tab_abstract.cpp
	tab_audio.cpp
	tab_quality_video.cpp
	tab_render_detail.cpp
	tab_simple.cpp
	tab_ui.cpp

	hotkeys/HotkeyForms.cpp
	hotkeys/AddSelectionCmdDlg.cpp
	hotkeys/commandlist.cpp
	hotkeys/hotkey_panel.cpp
	hotkeys/hotkey_parser.cpp
	hotkeys/KeynameConverter.cpp
	hotkeys/wxSpringCommand.cpp
	hotkeys/SpringDefaultProfile.cpp
	hotkeys/HotkeyTypes.cpp
	hotkeys/CommandOrderDlg.cpp
	hotkeys/ChangeMetaDlg.cpp

	wxkeybinder/keybinder.cpp
	wxkeybinder/wxKeyMonitorTextCtrl.cpp
)

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
	install(TARGETS springsettings RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
ENDIF (WIN32)
TARGET_LINK_LIBRARIES(springsettings
		libsettings
		lsl-utils
		lsl-unitsync
		slcommon
		${CURL_LINKFLAGS}
		${WX_LD_FLAGS}
		${CMAKE_DL_LIBS}
	)

set_target_properties(springsettings PROPERTIES
		COMPILE_FLAGS -DSPRINGSETTINGS_STANDALONE
	)
