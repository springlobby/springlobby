
# Add Definitions, Compiler-Switches, etc.: -Wall -O2 -g3 ...
# MSVC compiler (cl.exe) does not accept the same switches as gcc, although preprocessor definitions in the -D form will work for both
if(NOT MSVC)
	if( NOT MINGW )
		#wxMSW generates > 9000 warnings with -wextra...
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-strict-aliasing")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-strict-aliasing")
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		#FIXME: hide wxwidgets 3.0.2 error spam (should be fixed in 3.0.3)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-potentially-evaluated-expression")
	endif()
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -ggdb")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -ggdb")
		message(STATUS "DEBUG")
	endif()
else()
	add_definitions(-D_RC_MSVC)
endif()

if(WIN32)
	add_definitions(-DBOOST_THREAD_USE_LIB)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	add_definitions(-DUNICODE -D_UNICODE)
	set(Boost_THREADAPI win32)
endif()

set(Boost_USE_MULTITHREADED      ON)
if( ${CMAKE_CROSSCOMPILING} )
	set(Boost_USE_STATIC_LIBS       ON)
	set(Boost_USE_STATIC_RUNTIME    ON)
endif()

find_package(Boost 1.42.0 COMPONENTS system REQUIRED)


add_definitions( -DLOCALE_INSTALL_DIR="${LOCALE_INSTALL_DIR}" )



if(CMAKE_CROSSCOMPILING)
	message(STATUS "Crosscompiling: using static flags for wxwidgets")
	set(WXSTATIC "--static")
	#FIXME: autodetect thise flags:
	add_definitions(-DAL_LIBTYPE_STATIC)
else()
	set(WXSTATIC "")
endif()
find_program(wxWidgets_CONFIG_EXECUTABLE NAMES wx-config-3.0 wx-config ONLY_CMAKE_FIND_ROOT_PATH)
if(wxWidgets_CONFIG_EXECUTABLE-NOTFOUND)
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
	OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Using WX_CXX_FLAGS: ${WX_CXX_FLAGS}")

execute_process(COMMAND ${wxWidgets_CONFIG_EXECUTABLE} "--cflags" "${WXSTATIC}"
	OUTPUT_VARIABLE WX_C_FLAGS
	OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Using WX_C_FLAGS: ${WX_C_FLAGS}")

execute_process(COMMAND ${wxWidgets_CONFIG_EXECUTABLE} "--libs" "net,qa,richtext,adv,aui,core" "${WXSTATIC}"
	OUTPUT_VARIABLE WX_LD_FLAGS
	OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Using WX_LD_FLAGS: ${WX_LD_FLAGS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WX_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WX_C_FLAGS}")


include(CheckIncludeFileCXX)
check_include_file_cxx(wx/config.h HAS_WXGTKHEADERS)
if(NOT HAS_WXGTKHEADERS)
	message(FATAL_ERROR "Couldn't find wx/config.h, is wxgtk-dev installed?")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/downloader/lib/cmake/" ${CMAKE_MODULE_PATH})
find_package(Jsoncpp)
if(${Jsoncpp_FOUND})
	option(PRD_JSONCPP_INTERNAL "use bundled JsonCpp" FALSE)
else()
	option(PRD_JSONCPP_INTERNAL "use bundled JsonCpp" TRUE)
endif()


if(PRD_JSONCPP_INTERNAL)
	# use bundled JsonCpp
	set(PRD_JSONCPP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/downloader/lib/src/lib/jsoncpp/include/)
	set(PRD_JSONCPP_LIBRARIES "")
else()
	find_package(Jsoncpp REQUIRED)
	set(PRD_JSONCPP_INCLUDE_DIR ${Jsoncpp_INCLUDE_DIR})
	set(PRD_JSONCPP_LIBRARIES ${Jsoncpp_LIBRARY})
endif()


#----------------------------------------------------------------------------------------------------
# openAL libs dependency check
#----------------------------------------------------------------------------------------------------

if(MINGW)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gstabs")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -gstabs")
	link_libraries(intl)
endif()

find_package(CURL REQUIRED)
if(NOT CURL_FOUND)
	message( FATAL_ERROR "required library libCURL not found" )
endif()

if(DEFINED PKG_CONFIG_EXECUTABLE) # override CURL_LIBRARIES if curl-config is set
	execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} "libcurl" "--libs"
		OUTPUT_VARIABLE CURL_LINKFLAGS
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	set(CURL_LINKFLAGS "${CURL_LINKFLAGS}" CACHE STRING "" FORCE)
	message(STATUS "${PKG_CONFIG_EXECUTABLE} returned ${CURL_LINKFLAGS} for linking to libcurl")

	execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} "libcurl" "--cflags-only-other"
		OUTPUT_VARIABLE CURL_DEFINITONS
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	add_definitions(${CURL_DEFINITONS})
	message(STATUS "${PKG_CONFIG_EXECUTABLE} returned ${CURL_DEFINITONS} for compiling with libcurl")
endif()



add_definitions( ${CURL_CFLAGS} )
#add_to_global(link_directories  ${CURL_LIB_DIRS} )

#----------------------------------------------------------------------------------------------------
# libnotify dependency check
#----------------------------------------------------------------------------------------------------

if(OPTION_NOTIFY AND NOT WIN32)
	pkg_check_modules( LIBNOTIFY libnotify )
	pkg_check_modules( GLIB glib-2.0 )
endif()

#-----------------------------------------------------------------------------
# wxFormBuilder
#-----------------------------------------------------------------------------
set(wxfb_projects
	AgreementDialog
	ConnectWindow
	CrashReporterDialog
	HostBattleDialog
)

set(wxfb_src ${springlobby_SOURCE_DIR}/src/gui/wxfb)
# For configure_file below, all .fbp should have
# property name="path" set to @WXFB_GENERATION_PATH@.
set(WXFB_GENERATION_PATH ${wxfb_src}/generated)
file(MAKE_DIRECTORY ${WXFB_GENERATION_PATH})
set(wxFB_REQUIRED_VERSION "3.10.0")

find_program(wxFB_EXECUTABLE NAMES wxformbuilder wxFormBuilder)
if("${wxFB_EXECUTABLE}" MATCHES "wxFB_EXECUTABLE-NOTFOUND")
	message(WARNING "Couldn't find wxFormBuilder! Please install wxFB (${wxFB_REQUIRED_VERSION})")
else()
	message(STATUS "found wxFormBuilder: ${wxFB_EXECUTABLE}")
	execute_process(COMMAND ${wxFB_EXECUTABLE} "--version"
		OUTPUT_VARIABLE wxFB_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
	if(NOT ("${wxFB_VERSION}" VERSION_LESS "${wxFB_REQUIRED_VERSION}"))
		message(FATAL_ERROR "wxFormBuilder is not of required version: ${wxFB_REQUIRED_VERSION}")
	endif()


	foreach(proj ${wxfb_projects})
		message(STATUS "Processing wxFormBuilder project: ${proj}")
		# configure_file monitors input for timestamp changes and output for
		# content changes (checksum?). Replaces generation path.
		configure_file(${wxfb_src}/${proj}.fbp ${wxfb_src}/${proj}.fbp @ONLY)

		add_custom_command(COMMAND ${wxFB_EXECUTABLE} "--generate" ${wxfb_src}/${proj}.fbp
			DEPENDS ${wxfb_src}/${proj}.fbp
			OUTPUT ${WXFB_GENERATION_PATH}/${proj}Base.cpp
			       ${WXFB_GENERATION_PATH}/${proj}Base.h)
	endforeach()
endif()

foreach(proj ${wxfb_projects})
	message(STATUS "Adding wxFormBuilder source files for: ${proj}")

	list(APPEND generated_gui_cpp ${WXFB_GENERATION_PATH}/${proj}Base.cpp)
	list(APPEND generated_gui_cpp ${WXFB_GENERATION_PATH}/${proj}Base.h)
endforeach()
# for gcc -I
set(generated_gui_include ${wxfb_src}) # generated/* in source
#-----------------------------------------------------------------------------
# springlobby build
#-----------------------------------------------------------------------------

set(springlobbySrc
	address.cpp
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
	logclear.cpp
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
	gui/crashreporterdialog.cpp
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
	gui/battlelist/battledataviewctrl.cpp
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
	utils/sortutil.cpp
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

if(SPRINGLOBBY_UPDATER)
	set_source_files_properties(gui/ui.cpp PROPERTIES COMPILE_FLAGS "-DAUTOUPDATE=1")
endif()

if(EXISTS ${springlobby_SOURCE_DIR}/springlobby_config.h)
	set(SPRINGLOBBY_CONFIGH_DIR ${springlobby_SOURCE_DIR}) # needed for out of source builds with a tarball
else()
	set(SPRINGLOBBY_CONFIGH_DIR ${springlobby_BINARY_DIR})
endif()

add_custom_command(OUTPUT ${SPRINGLOBBY_CONFIGH_DIR}/springlobby_config.h
	COMMAND ${springlobby_SOURCE_DIR}/cmake/regen_config_header.sh ${springlobby_SOURCE_DIR} ${springlobby_BINARY_DIR}
)
add_custom_target(version ALL ${springlobby_SOURCE_DIR}/cmake/regen_config_header.sh ${springlobby_SOURCE_DIR} ${springlobby_BINARY_DIR}
	DEPENDS ${SPRINGLOBBY_CONFIGH_DIR}/springlobby_config.h
)
set_source_files_properties(utils/version.cpp PROPERTIES COMPILE_DEFINITIONS SPRINGLOBBY_CONFIGH="${SPRINGLOBBY_CONFIGH_DIR}/springlobby_config.h")

file( GLOB header "*.h" )
file( GLOB header2 "*/*.h" )
# FILE( GLOB header3 "*/*/*.h" )

#grep -lR cpp\"$ * | sed -e "s/\.h/\.cpp/g"
set(templatesources
	gui/basedataviewmodel.cpp
	gui/virtualdatactrl.cpp
)

add_subdirectory(downloader)

remove_definitions(-DHAVE_WX -DHAVE_SPRINGLOBBY)

add_subdirectory(sound)


set_source_files_properties(${templatesources} PROPERTIES HEADER_FILE_ONLY 1)


#mylink( ${EXTRA_LIB_DIRS} ${link_directories} )

if(WIN32)
	set(springlobby_RC_FILE springlobby.rc springlobby.exe.manifest)
endif()
add_subdirectory(updater)

add_executable(springlobby WIN32 MACOSX_BUNDLE
	${springlobbySrc}
	${springlobby_RC_FILE}
	${generated_gui_cpp}
	${SPRINGLOBBY_CONFIGH_DIR}/springlobby_config.h
)

add_dependencies(springlobby version)

set(CXX_STANDARD_REQUIRED ON)
set_property(TARGET springlobby PROPERTY CXX_STANDARD 17)

target_include_directories(springlobby
	PRIVATE ${PRD_JSONCPP_INCLUDE_DIR}
)

find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
	message(STATUS "Building with OpenSSL support: ${OPENSSL_VERSION}")
	target_include_directories(springlobby PRIVATE ${OPENSSL_INCLUDE_DIR})
	target_link_libraries(springlobby ${OPENSSL_LIBRARIES})
else()
	message(WARNING "SSL support disabled!")
endif()

if(LIBNOTIFY_FOUND AND OPTION_NOTIFY)
	target_link_libraries(springlobby ${LIBNOTIFY_LIBRARIES} ${GLIB_LIBRARIES})
	target_include_directories(springlobby PRIVATE ${LIBNOTIFY_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS})
	target_compile_definitions(springlobby PRIVATE -DHAVE_LIBNOTIFY)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
		target_link_libraries(springlobby stdc++fs)
	endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0")
		target_link_libraries(springlobby stdc++fs)
	endif()
endif()




#the updater needs to be build for windows only
if(WIN32)
	target_link_libraries( springlobby
		iphlpapi
		ws2_32
	)
	install(TARGETS springlobby RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
else()
	install(TARGETS springlobby
		RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
		BUNDLE DESTINATION  ${CMAKE_INSTALL_PREFIX}/bin)
endif()

target_link_libraries(springlobby lsl-utils lsl-unitsync pr-downloader_static
	${Boost_LIBRARIES}
	${CURL_LIBRARIES}
	${CURL_LINKFLAGS}
	${WX_LD_FLAGS}
	sound
)
target_include_directories(springlobby
	PRIVATE
		${CURL_INCLUDE_DIR}
		${Boost_INCLUDE_DIRS}
		${springlobby_SOURCE_DIR}/src
		${springlobby_SOURCE_DIR}/src/downloader/lib/src
		${springlobby_SOURCE_DIR}/src/downloader/lib/src/lsl
		${generated_gui_include}
)

if(UNIX)
	find_package(X11 REQUIRED)
	target_link_libraries(springlobby ${X11_X11_LIB})
endif()

enable_testing()
add_subdirectory(tests)

add_analyze(${springlobbySoundsrc} ${springlobbySrc} ${SLSharedWithSettings})

