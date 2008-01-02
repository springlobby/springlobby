SET(SRCS
	custom_dialogs.cpp
	frame.cpp
	helpmenufunctions.cpp
	Main.cpp
	panel_pathoption.cpp
	presets.cpp
	se_settings.cpp
	se_utils.cpp
	tab_abstract.cpp
	tab_audio.cpp
	tab_debug.cpp
	tab_quality_video.cpp
	tab_render_detail.cpp
	tab_simple.cpp
	tab_ui.cpp
)

ADD_LIBRARY(Settings++ SHARED ${SRCS})
