if(WIN32)
	set(SPRINGLOBBY_UPDATER_DEFAULT ON)
else()
	set(SPRINGLOBBY_UPDATER_DEFAULT OFF)
endif()

option(SPRINGLOBBY_UPDATER "Compile the springlobby_updater" ${SPRINGLOBBY_UPDATER_DEFAULT})

if(SPRINGLOBBY_UPDATER)
	execute_process(COMMAND ${wxWidgets_CONFIG_EXECUTABLE} "--libs" "core" "${WXSTATIC}"
		        OUTPUT_VARIABLE WX_UPDATER_LD_FLAGS
			OUTPUT_STRIP_TRAILING_WHITESPACE)
	set(updaterSrc
		updaterapp.cpp
		../utils/platform.cpp
		updater.rc
		springlobby_updater.exe.manifest)

	add_executable(springlobby_updater WIN32 MACOSX_BUNDLE ${updaterSrc})
	target_link_libraries(springlobby_updater
			${WX_UPDATER_LD_FLAGS})
	target_include_directories(springlobby_updater
			PRIVATE ${Boost_INCLUDE_DIRS}
			PRIVATE ${springlobby_SOURCE_DIR}/src
			PRIVATE ${springlobby_SOURCE_DIR}/src/downloader/lib/src/lsl)
	install(TARGETS springlobby_updater RUNTIME DESTINATION .)
endif()

