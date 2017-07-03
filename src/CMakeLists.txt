INCLUDE(compat_flags)


# Add Definitions, Compiler-Switches, etc.: -Wall -O2 -g3 ...
# MSVC compiler (cl.exe) does not accept the same switches as gcc, although preprocessor definitions in the -D form will work for both
IF(NOT MSVC)
	IF( NOT MINGW )
		#wxMSW generates > 9000 warnings with -wextra...
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
	ENDIF( NOT MINGW )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-strict-aliasing")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-strict-aliasing")
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		#FIXME: hide wxwidgets 3.0.2 error spam (should be fixed in 3.0.3)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-potentially-evaluated-expression")
	endif()
ELSE(NOT MSVC)
	ADD_DEFINITIONS(-D_RC_MSVC)
ENDIF(NOT MSVC)

if(WIN32)
	add_definitions(-DBOOST_THREAD_USE_LIB)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	set(Boost_THREADAPI win32)
endif(WIN32)

set(Boost_USE_MULTITHREADED      ON)
if( ${CMAKE_CROSSCOMPILING} )
	set(Boost_USE_STATIC_LIBS       ON)
	set(Boost_USE_STATIC_RUNTIME    ON)
endif()

FIND_PACKAGE(Boost 1.42.0 COMPONENTS thread system REQUIRED)

ADD_DEFINITIONS( -DLOCALE_INSTALL_DIR="${LOCALE_INSTALL_DIR}" )



if (CMAKE_CROSSCOMPILING)
	message(STATUS "Crosscompiling: using static flags for wxwidgets")
	set(WXSTATIC "--static")
	#FIXME: autodetect thise flags:
	add_definitions(-DAL_LIBTYPE_STATIC)
else()
	set(WXSTATIC "")
endif()
find_program(wxWidgets_CONFIG_EXECUTABLE NAMES wx-config-3.0 wx-config ONLY_CMAKE_FIND_ROOT_PATH)
if (wxWidgets_CONFIG_EXECUTABLE-NOTFOUND)
	message(FATAL_ERROR "couldn't find wx-config!")
else()
	message(STATUS "found wx-config: ${wxWidgets_CONFIG_EXECUTABLE}")
endif()

execute_process(COMMAND ${wxWidgets_CONFIG_EXECUTABLE} "--version-full"
	OUTPUT_VARIABLE WX_VERSION
	OUTPUT_STRIP_TRAILING_WHITESPACE)

set(WX_VERSION_MIN "3.0")
if("${WX_VERSION}" VERSION_LESS "${WX_VERSION_MIN}")
	set(WX_VERSION_DEFAULT "${WX_VERSION}")
	# Append version selector to find the right one in a multi-slot install if the default is lower than required.
	set(wxWidgets_CONFIG_EXECUTABLE ${wxWidgets_CONFIG_EXECUTABLE} "--version=${WX_VERSION_MIN}")
	execute_process(COMMAND ${wxWidgets_CONFIG_EXECUTABLE} "--version-full"
		OUTPUT_VARIABLE WX_VERSION
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	if("${WX_VERSION}" VERSION_LESS "${WX_VERSION_MIN}")
		message(FATAL_ERROR "couldn't find wx-widgets >= ${WX_VERSION_MIN}, found ${WX_VERSION_DEFAULT}")
	endif()
endif()
message(STATUS "found wxwidgets: ${WX_VERSION}")

execute_process(COMMAND ${wxWidgets_CONFIG_EXECUTABLE} "--cppflags" "${WXSTATIC}"
	OUTPUT_VARIABLE WX_CXX_FLAGS
	OUTPUT_STRIP_TRAILING_WHITESPACE )
message(STATUS "Using WX_CXX_FLAGS: ${WX_CXX_FLAGS}")

execute_process(COMMAND ${wxWidgets_CONFIG_EXECUTABLE} "--cflags" "${WXSTATIC}"
	OUTPUT_VARIABLE WX_C_FLAGS
	OUTPUT_STRIP_TRAILING_WHITESPACE )
message(STATUS "Using WX_C_FLAGS: ${WX_C_FLAGS}")

execute_process(COMMAND ${wxWidgets_CONFIG_EXECUTABLE} "--libs" "net,qa,richtext,adv,aui,core" "${WXSTATIC}"
	OUTPUT_VARIABLE WX_LD_FLAGS
	OUTPUT_STRIP_TRAILING_WHITESPACE )
message(STATUS "Using WX_LD_FLAGS: ${WX_LD_FLAGS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WX_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WX_C_FLAGS}")

#----------------------------------------------------------------------------------------------------
# openAL libs dependency check
#----------------------------------------------------------------------------------------------------

IF( MINGW )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gstabs")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -gstabs")
	LINK_LIBRARIES( intl)
ENDIF( MINGW )

FIND_PACKAGE( CURL REQUIRED libcurl )
IF( NOT CURL_FOUND )
	MESSAGE( FATAL_ERROR "required library libCURL not found" )
ENDIF( NOT CURL_FOUND )

if(DEFINED PKG_CONFIG_EXECUTABLE) # override CURL_LIBRARIES if curl-config is set
	execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} "libcurl" "--libs"
		OUTPUT_VARIABLE CURL_LINKFLAGS
		OUTPUT_STRIP_TRAILING_WHITESPACE )

	SET(CURL_LINKFLAGS "${CURL_LINKFLAGS}" CACHE STRING "" FORCE)
	message(STATUS "${PKG_CONFIG_EXECUTABLE} returned ${CURL_LINKFLAGS} for linking to libcurl")

	execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} "libcurl" "--cflags-only-other"
		OUTPUT_VARIABLE CURL_DEFINITONS
		OUTPUT_STRIP_TRAILING_WHITESPACE )
	add_definitions(${CURL_DEFINITONS})
	message(STATUS "${PKG_CONFIG_EXECUTABLE} returned ${CURL_DEFINITONS} for compiling with libcurl")
endif()



ADD_DEFINITIONS( ${CURL_CFLAGS} )
#add_to_global(link_directories  ${CURL_LIB_DIRS} )

#----------------------------------------------------------------------------------------------------
# libnotify dependency check
#----------------------------------------------------------------------------------------------------

IF( OPTION_NOTIFY AND NOT WIN32 )
	pkg_check_modules( LIBNOTIFY libnotify )
	pkg_check_modules( GLIB glib-2.0 )
ENDIF( OPTION_NOTIFY AND NOT WIN32 )


set(springlobbySrc
	autohost.cpp
	autohostmanager.cpp
	battlelist.cpp
	battle.cpp
	channel.cpp
	channellist.cpp
	chatlog.cpp
	countrycodes.cpp
	contentsearchresult.cpp
	flagimages.cpp
	httpfile.cpp
	ibattle.cpp
	iconimagelist.cpp
	iplaybacklist.cpp
	iserver.cpp
	offlinebattle.cpp
	offlineserver.cpp
	playbackthread.cpp
	replaylist.cpp
	servermanager.cpp
	savegamelist.cpp
	singleplayerbattle.cpp
	serverselector.cpp
	serverevents.cpp
	socket.cpp
	spring.cpp
	springlobbyapp.cpp
	springprocess.cpp
	sysinfo.cpp
	tasserver.cpp
	user.cpp
	useractions.cpp
	userlist.cpp
	log.cpp
	settings.cpp
	updatehelper.cpp
	stacktrace.cpp
	exception.cpp
	
	aui/auimanager.cpp

	gui/controls/bitmapcombobox.cpp
	
	gui/activitynotice.cpp
	gui/agreementdialog.cpp
	gui/chatpanelmenu.cpp
	gui/chatpanel.cpp
	gui/contentdownloaddialog.cpp
	gui/contentsearchresultview.cpp
	gui/contentsearchresultdatamodel.cpp
	gui/connectwindow.cpp
	gui/colorbutton.cpp
	gui/mainchattab.cpp
	gui/maindownloadtab.cpp
	gui/mainwindow.cpp
	gui/mapctrl.cpp
	gui/mapgridctrl.cpp
	gui/mapselectdialog.cpp
	gui/pastedialog.cpp
	gui/slbook.cpp
	gui/statusbar.cpp
	gui/singleplayertab.cpp
	gui/selectusersdialog.cpp
	gui/taskbar.cpp
	gui/textentrydialog.cpp
	gui/ui.cpp
	gui/wxtextctrlhist.cpp
	gui/iconscollection.cpp
	gui/nickdataviewctrl.cpp
	gui/nickdataviewmodel.cpp
	gui/dataviewctrlheadermenu.cpp
	
	gui/notifications/libnotify.cpp
	gui/notifications/notificationmanager.cpp
	gui/notifications/toasternotification.cpp
	gui/notifications/toasterbox/ToasterBox.cpp
	gui/notifications/toasterbox/ToasterBoxWindow.cpp
	gui/notifications/toasterbox/ToasterBoxWindowList.cpp

	gui/battlelist/battlelistfilter.cpp
	gui/battlelist/battlelisttab.cpp
	gui/battlelist/battledataviewcttrl.cpp
	gui/battlelist/battledataviewmodel.cpp

	gui/channel/channelchooser.cpp
	gui/channel/channelchooserdialog.cpp
	gui/channel/autojoinchanneldialog.cpp
	gui/channel/channellistview.cpp
	gui/channel/channellistdataviewmodel.cpp
	
	gui/hosting/addbotdialog.cpp
	gui/hosting/autobalancedialog.cpp
	gui/hosting/battlemaptab.cpp
	gui/hosting/battlerestrictionstab.cpp
	gui/hosting/battleroomtab.cpp
	gui/hosting/hostbattledialog.cpp
	gui/hosting/mainjoinbattletab.cpp
	gui/hosting/mainsingleplayertab.cpp
	gui/hosting/mmoptionwindows.cpp
	gui/hosting/battleroomdownloads.cpp
	gui/hosting/battleroommmoptionstab.cpp
	gui/hosting/votepanel.cpp
	gui/hosting/battleroomdataviewctrl.cpp
	gui/hosting/battleroomdataviewmodel.cpp

	gui/options/downloadoptionspanel.cpp
	gui/options/mainoptionstab.cpp
	gui/options/springoptionstab.cpp
	gui/options/groupoptionspanel.cpp
	gui/options/lobbyoptionstab.cpp
	gui/options/chatoptionstab.cpp

	gui/playback/playbackfilter.cpp
	gui/playback/playbacktab.cpp
	gui/playback/playbackdataview.cpp
	gui/playback/playbackdatamodel.cpp

	downloader/prdownloader.cpp
	gui/downloaddataviewctrl.cpp
	gui/downloaddataviewmodel.cpp

	utils/base64.cpp
	utils/crc.cpp
	utils/TextCompletionDatabase.cpp
	utils/md5.c
	utils/misc.cpp
	utils/lslconversion.cpp
	utils/tasutil.cpp
	utils/version.cpp

	springsettings/frame.cpp
	springsettings/tab_abstract.cpp
	springsettings/tab_audio.cpp
	springsettings/tab_quality_video.cpp
	springsettings/tab_render_detail.cpp
	springsettings/tab_simple.cpp
	springsettings/tab_ui.cpp


	downloader/lib/src/lsl/lsl/battle/tdfcontainer.cpp #FIXME
	downloader/lib/src/lsl/lslutils/conversion.cpp #FIXME

	gui/uiutils.cpp
	gui/controls.cpp
	gui/customdialogs.cpp
	gui/sltipwin.cpp
	gui/infodialog.cpp
	gui/windowattributespickle.cpp
	gui/aboutbox.cpp


	utils/wxTranslationHelper.cpp
	utils/conversion.cpp
	utils/globalevents.cpp
	utils/platform.cpp
	utils/slpaths.cpp
	utils/uievents.cpp
	utils/curlhelper.cpp
	utils/slconfig.cpp

)


if (EXISTS ${springlobby_SOURCE_DIR}/springlobby_config.h)
	set(SPRINGLOBBY_CONFIGH_DIR ${springlobby_SOURCE_DIR}) # needed for out of source builds with a tarball
else()
	set(SPRINGLOBBY_CONFIGH_DIR ${springlobby_BINARY_DIR})
endif()

add_custom_target(version ALL ${springlobby_SOURCE_DIR}/tools/regen_config_header.sh ${springlobby_SOURCE_DIR} ${springlobby_BINARY_DIR})
add_custom_command(OUTPUT ${SPRINGLOBBY_CONFIGH_DIR}/springlobby_config.h
			COMMAND ${springlobby_SOURCE_DIR}/tools/regen_config_header.sh ${springlobby_SOURCE_DIR} ${springlobby_BINARY_DIR}
		)

set_property(SOURCE utils/version.cpp APPEND PROPERTY OBJECT_DEPENDS ${SPRINGLOBBY_CONFIGH_DIR}/springlobby_config.h)

set_source_files_properties(utils/version.cpp PROPERTIES COMPILE_DEFINITIONS SPRINGLOBBY_CONFIGH="${SPRINGLOBBY_CONFIGH_DIR}/springlobby_config.h")


FILE( GLOB header "*.h" )
FILE( GLOB header2 "*/*.h" )
# FILE( GLOB header3 "*/*/*.h" )

#grep -lR cpp\"$ * | sed -e "s/\.h/\.cpp/g"
SET( templatesources
	gui/basedataviewmodel.cpp
	gui/virtualdatactrl.cpp
)

add_subdirectory(downloader)

remove_definitions(-DHAVE_WX -DHAVE_SPRINGLOBBY)

add_subdirectory(sound)


set_source_files_properties(  ${templatesources} PROPERTIES HEADER_FILE_ONLY 1 )

#mylink( ${EXTRA_LIB_DIRS} ${link_directories} )

IF(WIN32)
	set(springlobby_RC_FILE springlobby.rc springlobby.exe.manifest)
ENDIF()
add_subdirectory(updater)

ADD_EXECUTABLE(springlobby WIN32 MACOSX_BUNDLE
		${springlobbySrc}
		${springlobby_RC_FILE}
	)

IF( LIBNOTIFY_FOUND AND OPTION_NOTIFY )
	TARGET_LINK_LIBRARIES(springlobby ${LIBNOTIFY_LIBRARIES} ${GLIB_LIBRARIES} )
	target_include_directories(springlobby PRIVATE ${LIBNOTIFY_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS})
	target_compile_definitions(springlobby PRIVATE -DHAVE_LIBNOTIFY)
ENDIF( LIBNOTIFY_FOUND AND OPTION_NOTIFY )

#the updater needs to be build for windows only
IF(WIN32)
	TARGET_LINK_LIBRARIES( springlobby
		iphlpapi
		nettle
		ws2_32
	)
	install(TARGETS springlobby RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
ELSE (WIN32)
	install(TARGETS springlobby
		RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
		BUNDLE DESTINATION  ${CMAKE_INSTALL_PREFIX}/bin
		)
ENDIF (WIN32)

TARGET_LINK_LIBRARIES(springlobby lsl-utils lsl-unitsync pr-downloader_static
		${Boost_LIBRARIES}
		${CURL_LIBRARIES}
		${CURL_LINKFLAGS}
		${WX_LD_FLAGS}
		sound
	)
target_include_directories(springlobby
		PRIVATE ${CURL_INCLUDE_DIR}
		PRIVATE ${Boost_INCLUDE_DIRS}
		PRIVATE ${springlobby_SOURCE_DIR}/src
		PRIVATE ${springlobby_SOURCE_DIR}/src/downloader/lib/src
		PRIVATE ${springlobby_SOURCE_DIR}/src/downloader/lib/src/lsl
	)

IF(UNIX)
	FIND_PACKAGE(X11 REQUIRED)
	TARGET_LINK_LIBRARIES(springlobby ${X11_X11_LIB})
ENDIF(UNIX)

ENABLE_TESTING()
add_subdirectory(tests)

add_analyze(${springlobbySoundsrc} ${springlobbySrc} ${SLSharedWithSettings})
#add_format(${springlobbySoundsrc} ${springlobbySrc} ${SLSharedWithSettings} ${templatesources} ${header} ${header2})

