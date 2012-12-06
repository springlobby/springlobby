INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE(compat_flags)

# Add Definitions, Compiler-Switches, etc.: -Wall -O2 -g3 ...
# MSVC compiler (cl.exe) does not accept the same switches as gcc, although preprocessor definitions in the -D form will work for both
IF(NOT MSVC)
	IF( NOT MINGW )
		#wxMSW generates > 9000 warnings with -wextra...
		ADD_DEFINITIONS( -Wextra )
	ENDIF( NOT MINGW )
	ADD_DEFINITIONS( -Wall -Wno-strict-aliasing )
ELSE(NOT MSVC)
	ADD_DEFINITIONS(-D_RC_MSVC)
ENDIF(NOT MSVC)

set(Boost_USE_MULTITHREADED      ON)
if( ${CMAKE_CROSSCOMPILING} )
	set(Boost_USE_STATIC_LIBS       ON)
	set(Boost_USE_STATIC_RUNTIME    ON)
endif()

FIND_PACKAGE(Boost 1.44.0 COMPONENTS thread system REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
add_to_global(link_directories ${Boost_LIBRARY_DIRS} ${BOOST_LIB_DIR})

#----------------------------------------------------------------------------------------------------
# L10n support
#----------------------------------------------------------------------------------------------------
if( OPTION_TRANSLATION_SUPPORT )
	include ( ${CMAKE_SOURCE_DIR}/po/CMakeLists.txt )
endif( OPTION_TRANSLATION_SUPPORT )
ADD_DEFINITIONS( -DLOCALE_INSTALL_DIR="${LOCALE_INSTALL_DIR}" )


#----------------------------------------------------------------------------------------------------
# wxWidgets lib dependency check
#----------------------------------------------------------------------------------------------------
IF(NOT CMAKE_CROSSCOMPILING)
	# Here you can define, what Libraries of wxWidgets you need for your Application. You can figure out what Libraries you need here:
	# http://www.wxwidgets.org/manuals/2.8/wx_librarieslist.html
	SET(wxWidgets_USE_LIBS base core net adv aui html xml qa)
	# We need the Find package for wxWidgets to work, but auto-find is broken in x-compile setup
	FIND_PACKAGE(wxWidgets REQUIRED base core net adv aui html xml qa)
	# Did we find wxWidgets ? This condition will fail for as long as the internal Vars do not point to the proper wxWidgets Configuration.
	IF(wxWidgets_FOUND)
		INCLUDE(${wxWidgets_USE_FILE})
		INCLUDE_DIRECTORIES( ${wxWidgets_INCLUDE_DIRS} )
	ELSE(wxWidgets_FOUND)
		# For Convenience. If we cannot continue, inform the User.
		MESSAGE( FATAL_ERROR "wxWidgets library not found! Please install the package to continue")
	ENDIF(wxWidgets_FOUND)
ELSE (NOT CMAKE_CROSSCOMPILING)
    INCLUDE_DIRECTORIES( ${wxWidgets_INCLUDE_DIRS} )
    add_to_global(link_directories ${wxWidgets_LIB_DIR} )
ENDIF(NOT CMAKE_CROSSCOMPILING)


#----------------------------------------------------------------------------------------------------
# openAL libs dependency check
#----------------------------------------------------------------------------------------------------

IF( ENABLE_DEBUG_REPORT )
	ADD_DEFINITIONS( -DENABLE_DEBUG_REPORT )
	IF( MINGW )
		ADD_DEFINITIONS( -gstabs )
		LINK_LIBRARIES( bfd intl drmingw )
	ENDIF( MINGW )
ENDIF( ENABLE_DEBUG_REPORT )

IF(WIN32)
	ADD_DEFINITIONS( ${CURL_CFLAGS}  )
	add_to_global(link_directories  ${CURL_STATIC_LIBRARY_DIRS} )
	LINK_LIBRARIES( ${CURL_STATIC_LDFLAGS} )
	INCLUDE_DIRECTORIES( ${CURL_INCLUDE_DIR} )
ELSE(WIN32)
	FIND_PACKAGE( CURL REQUIRED libcurl )
	IF( NOT CURL_FOUND )
		MESSAGE( FATAL_ERROR "required library libCURL not found" )
	ENDIF( NOT CURL_FOUND )
	ADD_DEFINITIONS( ${CURL_CFLAGS} )
	add_to_global(link_directories  ${CURL_LIB_DIRS} )
	LINK_LIBRARIES( ${CURL_LDFLAGS} )
	INCLUDE_DIRECTORIES( ${CURL_INCLUDE_DIR} )
ENDIF(WIN32)

#----------------------------------------------------------------------------------------------------
# libnotify dependency check
#----------------------------------------------------------------------------------------------------

IF( OPTION_NOTIFY AND NOT WIN32 )
	pkg_check_modules( LIBNOTIFY libnotify )
	pkg_check_modules( GLIB glib-2.0 )
	IF( LIBNOTIFY_FOUND )
		INCLUDE_DIRECTORIES( ${LIBNOTIFY_INCLUDE_DIRS} )
		LINK_LIBRARIES( ${LIBNOTIFY_LIBRARIES} )
		INCLUDE_DIRECTORIES( ${GLIB_INCLUDE_DIRS} )
		LINK_LIBRARIES( ${GLIB_LIBRARIES} )
		ADD_DEFINITIONS( -DHAVE_LIBNOTIFY )
	ENDIF( LIBNOTIFY_FOUND )
ENDIF( OPTION_NOTIFY AND NOT WIN32 )

#----------------------------------------------------------------------------------------------------
# libalure dependency check
#----------------------------------------------------------------------------------------------------
if   ( OPTION_SOUND )
	message(STATUS "Sound enabled!")
	find_package(Alure REQUIRED)
	include_directories(${ALURE_INCLUDE_DIRS})
else ( OPTION_SOUND )
	# Disable sound.
	message(STATUS "Sound disabled!")
	add_definitions( -DDISABLE_SOUND )
	unset(ALURE_LIBRARIES)
endif ( OPTION_SOUND )

clear(springlobbySrc)
add_sources(springlobbySrc
	chatpanelmenu.cpp
	updater/updater.cpp
	agreementdialog.cpp
	base64.cpp
	battle.cpp
	crc.cpp
	chatlog.cpp
	chatpanel.cpp
	connectwindow.cpp
	countrycodes.cpp
	customlistctrl.cpp
	flagimages.cpp
	ibattle.cpp
	iconimagelist.cpp
	introguide.cpp
	mainchattab.cpp
	maindownloadtab.cpp
	mainwindow.cpp
	mapctrl.cpp
	mapgridctrl.cpp
	mapselectdialog.cpp
	nicklistctrl.cpp
	offlinebattle.cpp
	
	playback/replaylist.cpp
	playback/savegamelist.cpp
	
	selectusersdialog.cpp
	server.cpp
	reconnectdialog.cpp
	serverevents.cpp
	singleplayerbattle.cpp
	singleplayertab.cpp
	socket.cpp
	spring.cpp
	springlobbyapp.cpp
	springprocess.cpp
	tasserver.cpp
	tdfcontainer.cpp
	ui.cpp
	user.cpp
	useractions.cpp
	userlist.cpp
	
	toasterbox/ToasterBox.cpp
	toasterbox/ToasterBoxWindow.cpp
	toasterbox/ToasterBoxWindowList.cpp
	
	pingthread.cpp
	iserverevents.cpp
	sound/alsound.cpp
)

if   (OPTION_SOUND )
	message(STATUS "Sound enabled!")
	find_package(Alure)
	if (ALURE_FOUND)
		include_directories(${ALURE_INCLUDE_DIRS})
	else(ALURE_FOUND)
		message(WARNING "OPTION_SOUND enabled but libalure not found! (maybe install libalure-dev?)")
		add_definitions( -DDISABLE_SOUND )
	endif(ALURE_FOUND)
else ( OPTION_SOUND )
	# Disable sound.
	message(STATUS "Sound disabled!")
	add_definitions( -DDISABLE_SOUND )
endif ( OPTION_SOUND )

clear(SLSharedWithSettings)
add_sources(SLSharedWithSettings
	customizations.cpp
	Helper/wxTranslationHelper.cpp
	uiutils.cpp
	updater/versionchecker.cpp
	updater/updatehelper.cpp
	utils/controls.cpp
	utils/conversion.cpp
	utils/customdialogs.cpp
	utils/debug.cpp
	utils/globalevents.cpp
	utils/platform.cpp
	utils/pathlistfactory.cpp
	utils/sltipwin.cpp
	utils/uievents.cpp
	utils/curlhelper.cpp
	crashreport.cpp
	globalsmanager.cpp
	mmoptionmodel.cpp
	mmoptionswrapper.cpp
	settings.cpp
	springunitsynclib.cpp
	springunitsync.cpp
	thread.cpp
	infodialog.cpp
	gui/windowattributespickle.cpp
	gui/spinctl/generic/spinctlg.cpp
	Helper/slconfig.cpp
)

FILE( GLOB header "*.h" )
FILE( GLOB header2 "*/*.h" )

#grep -lR cpp\"$ * | sed -e "s/\.h/\.cpp/g"
clear( templatesources)
add_sources( templatesources
	hosting/battleroommmoptionstab.cxx
	customvirtlistctrl.cpp
	downloader/httpdownloader.cpp
	playback/playbackfilter.cpp
	playback/playbacklist.cpp
	playback/playbacklistctrl.cpp
	playback/playbackthread.cpp
	playback/playbacktab.cpp
	utils/stringserializer.cpp
)

IF(WIN32)
	add_sources(SLSharedWithSettings stacktrace.c Helper/listctrl.cpp)
ELSE(WIN32)
	add_sources(SLSharedWithSettings stacktrace.cpp)
ENDIF(WIN32)
add_library(slcommon STATIC ${SLSharedWithSettings} )
target_link_libraries(slcommon ${wxWidgets_LIBRARIES} ${CURL_LIBRARY} )

add_subdirectory(aui)
add_subdirectory(battlelist)
add_subdirectory(channel)
add_subdirectory(downloader)
add_subdirectory(gui)
add_subdirectory(Helper)
add_subdirectory(hosting)
add_subdirectory(options)
add_subdirectory(utils)
add_subdirectory(widgets)
add_subdirectory(wxkeybinder)
Add_Subdirectory(lsl)
INCLUDE_DIRECTORIES(lsl/src)
add_subdirectory(springsettings)


LIST( APPEND header ${header2} ${templatesources} )
set_source_files_properties(  ${header} PROPERTIES HEADER_FILE_ONLY 1 )

mylink( ${EXTRA_LIB_DIRS} ${link_directories} ) 

IF(WIN32)
	CreateResourceCompileCommand(springlobby_RC_FILE ${springlobby_SOURCE_DIR}/src/ springlobby.rc sl_icon.o )
	add_subdirectory(updater)
ENDIF()


ADD_EXECUTABLE(springlobby WIN32 MACOSX_BUNDLE ${springlobbySrc} ${springlobby_RC_FILE})
#the updater needs to be build for windows only
IF(WIN32)
	link_directories( ${link_directories} ${EXTRA_LIB_DIRS} )
	TARGET_LINK_LIBRARIES( springlobby iphlpapi )
	install(TARGETS springlobby RUNTIME DESTINATION .)
ELSE (WIN32)
    install(TARGETS springlobby RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
	#only do this on platforms with awk expected to be installed
	ADD_CUSTOM_TARGET(test_susynclib ALL COMMAND tools/test-susynclib.awk src/springunitsynclib.cpp WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ENDIF (WIN32)

message(STATUS "Sound libraries used for linking: " ${sound_libraries})
TARGET_LINK_LIBRARIES(springlobby slcommon libsettings libSpringLobbyUtils pr-downloader_static 
		${wxWidgets_LIBRARIES}
		${Boost_LIBRARIES}
		${ALURE_LIBRARIES}
	)
IF(UNIX)
	TARGET_LINK_LIBRARIES(springlobby X11)
ENDIF(UNIX)
	
# add_dependencies( springlobby alure )
# add_dependencies( springlobby version )
# add_dependencies( springlobby pr-downloader_static )
# add_dependencies( springlobby libSpringLobbyUtils )

