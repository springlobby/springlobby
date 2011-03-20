# Add more folders to ship with the application, here
folder_01.source = qml/mapview
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# Avoid auto screen rotation
#DEFINES += ORIENTATIONLOCK

# Needs to be defined for Symbian
DEFINES += NETWORKACCESS

symbian:TARGET.UID3 = 0xE1BC102A

# Define QMLJSDEBUGGER to allow debugging of QML in debug builds
# (This might significantly increase build time)
 DEFINES += QMLJSDEBUGGER

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the 
# MOBILITY variable. 
# CONFIG += mobility
# MOBILITY +=
QT += phonon multimedia
QT += webkit
# PKGCONFIG += vorbis vorbisfile
# LIBS += -lvorbisfile -lvorbis -logg

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    ../../src/qt/imageprovider.cpp \
    ../../src/qt/converters.cpp \
    ../../src/qt/skirmishmodel.cpp \
    ../../src/qt/noguisingleplayerbattle.cpp \
    ../../src/Helper/TextCompletionDatabase.cpp \
	../../src/utils/activitynotice.cpp \
	../../src/chatpanelmenu.cpp \
	../../src/gui/gradientpanel.cpp \
	../../src/gui/toasternotification.cpp \
	../../src/gui/wxgradientbutton.cpp \
	../../src/gui/wxbackgroundimage.cpp \
	../../src/gui/statusbar.cpp \
	../../src/aui/slbook.cpp \
	../../src/utils/math.cpp \
	../../src/utils/misc.cpp \
	../../src/gui/pastedialog.cpp \
	../../src/utils/tasutil.cpp \
	../../src/utils/networkevents.cpp \
	../../src/utils/md5.c \
	../../src/Helper/imageviewer.cpp \
	../../src/Helper/slhtmlwindow.cpp \
	../../src/channel/channelchooser.cpp \
	../../src/channel/channelchooserdialog.cpp \
	../../src/Helper/wxtextctrlhist.cpp \
	../../src/Helper/colorbutton.cpp \
	../../src/filelister/filelistctrl.cpp \
	../../src/filelister/filelistdialog.cpp \
	../../src/filelister/filelistfilter.cpp \
	../../src/aui/auimanager.cpp \
	../../src/aui/artprovider.cpp \
	../../src/autobalancedialog.cpp \
	../../src/autohost.cpp \
	../../src/channel/autojoinchanneldialog.cpp \
	../../src/addbotdialog.cpp \
	../../src/agreementdialog.cpp \
	../../src/base64.cpp \
	../../src/battle.cpp \
	../../src/battlelist.cpp \
	../../src/battlelistctrl.cpp \
	../../src/battlelistfilter.cpp \
	../../src/battlelisttab.cpp \
	../../src/battlemaptab.cpp \
	../../src/battleoptionstab.cpp \
	../../src/battleroomlistctrl.cpp \
	../../src/battleroomtab.cpp \
	../../src/crc.cpp \
	../../src/channel/channel.cpp \
	../../src/channel/channellist.cpp \
	../../src/channel/channellistctrl.cpp \
	../../src/chatlog.cpp \
	../../src/chatoptionstab.cpp \
	../../src/chatpanel.cpp \
	../../src/connectwindow.cpp \
	../../src/countrycodes.cpp \
	../../src/customlistctrl.cpp \
	../../src/flagimages.cpp \
	../../src/groupoptionspanel.cpp \
	../../src/hostbattledialog.cpp \
	../../src/ibattle.cpp \
	../../src/iconimagelist.cpp \
	../../src/introguide.cpp \
	../../src/lobbyoptionstab.cpp \
	../../src/mainchattab.cpp \
	../../src/mainjoinbattletab.cpp \
	../../src/mainoptionstab.cpp \
	../../src/mainsingleplayertab.cpp \
	../../src/maintorrenttab.cpp \
	../../src/mainwindow.cpp \
	../../src/mapctrl.cpp \
	../../src/mapgridctrl.cpp \
	../../src/mapselectdialog.cpp \
	../../src/mmoptionwindows.cpp \
	../../src/nicklistctrl.cpp \
	../../src/offlinebattle.cpp \
	../../src/playback/replaylist.cpp \
	../../src/playback/savegamelist.cpp \
	../../src/alsound.cpp \
	../../src/selectusersdialog.cpp \
	../../src/server.cpp \
	../../src/reconnectdialog.cpp \
	../../src/serverevents.cpp \
	../../src/singleplayerbattle.cpp \
	../../src/singleplayertab.cpp \
	../../src/socket.cpp \
	../../src/spring.cpp \
	../../src/springoptionstab.cpp \
	../../src/springprocess.cpp \
	../../src/tasserver.cpp \
	../../src/tdfcontainer.cpp \
	../../src/torrentlistctrl.cpp \
	../../src/torrentoptionspanel.cpp \
	../../src/torrentwrapper.cpp \
	../../src/ui.cpp \
	../../src/user.cpp \
	../../src/useractions.cpp \
	../../src/userlist.cpp \
	../../src/utils/downloader.cpp \
	../../src/utils/plasmaresourceinfo.cpp \
	../../src/utils/battleevents.cpp \
	../../src/widgets/downloadlistctrl.cpp \
	../../src/widgets/downloaddialog.cpp \
	../../src/widgets/downloadpanel.cpp \
	../../src/widgets/infopanel.cpp \
	../../src/widgets/widget.cpp \
	../../src/Helper/tasclientimport.cpp \
	../../src/simpleinterface/simpleserverevents.cpp \
	../../src/toasterbox/ToasterBox.cpp \
	../../src/toasterbox/ToasterBoxWindow.cpp \
	../../src/toasterbox/ToasterBoxWindowList.cpp \
	../../src/gui/notificationmanager.cpp \
	../../src/pingthread.cpp \
	../../src/iserverevents.cpp \
	../../src/utils/curlhelper.cpp \
	../../src/customizations.cpp \
	../../src/uiutils.cpp \
	../../src/updater/versionchecker.cpp \
	../../src/updater/updatehelper.cpp \
	../../src/utils/controls.cpp \
	../../src/utils/conversion.cpp \
	../../src/utils/customdialogs.cpp \
	../../src/utils/debug.cpp \
	../../src/utils/globalevents.cpp \
	../../src/utils/platform.cpp \
	../../src/utils/sltipwin.cpp \
	../../src/utils/uievents.cpp \
	../../src/crashreport.cpp \
	../../src/globalsmanager.cpp \
	../../src/mmoptionmodel.cpp \
	../../src/mmoptionswrapper.cpp \
	../../src/settings.cpp \
	../../src/springunitsynclib.cpp \
	../../src/springunitsync.cpp \
	../../src/thread.cpp \
	../../src/gui/windowattributespickle.cpp \
	../../src/gui/spinctl/generic/spinctlg.cpp \
	../../src/Helper/slconfig.cpp \
    ../../src/qt/maplistmodel.cpp \
    ../../src/qt/qerrorwindow.cpp \
    ../../src/qt/sidemodel.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    ../../src/springunitsynclib.h \
    ../../src/globalsmanager.h \
    ../../src/settings.h \
    ../../src/Helper/slconfig.h \
    ../../src/utils/platform.h \
    ../../src/utils/misc.h \
    ../../src/utils/curlhelper.h \
    ../../src/springunitsync.h \
    ../../src/springprocess.h \
    ../../src/iunitsync.h \
    ../../src/thread.h \
    ../../src/utils/uievents.h \
    ../../src/utils/globalevents.h \
    ../../src/mmoptionswrapper.h \
    ../../src/mmoptionmodel.h \
    ../../src/uiutils.h \
    ../../src/utils/customdialogs.h \
    ../../src/customizations.h \
    ../../src/utils/conversion.h \
    ../../src/Helper/qtargparser/QtArg/xorarg.hpp \
    ../../src/Helper/qtargparser/QtArg/visitor.hpp \
    ../../src/Helper/qtargparser/QtArg/multiarg.hpp \
    ../../src/Helper/qtargparser/QtArg/helpiface.hpp \
    ../../src/Helper/qtargparser/QtArg/help.hpp \
    ../../src/Helper/qtargparser/QtArg/exceptions.hpp \
    ../../src/Helper/qtargparser/QtArg/cmdlineiface.hpp \
    ../../src/Helper/qtargparser/QtArg/cmdlinecontext.hpp \
    ../../src/Helper/qtargparser/QtArg/cmdline.hpp \
    ../../src/Helper/qtargparser/QtArg/argconstraint.hpp \
    ../../src/Helper/qtargparser/QtArg/arg.hpp \
    ../../src/qt/imageprovider.h \
    ../../src/qt/converters.h \
    ../../src/qt/skirmishmodel.h \
    ../../src/user.h \
    ../../src/ibattle.h \
    ../../src/iconimagelist.h \
    ../../src/userlist.h \
    ../../src/chatpanel.h \
    ../../src/spring.h \
    ../../src/ui.h \
    ../../src/tdfcontainer.h \
    ../../src/qt/noguisingleplayerbattle.h \
    ../../src/battle.h \
    ../../src/Helper/TextCompletionDatabase.hpp \
    ../../src/channel/channel.h \
    ../../src/chatlog.h \
    ../../src/aui/auimanager.h \
    ../../src/flagimages.h \
    ../../src/flagimagedata.h \
    ../../src/server.h \
    ../../src/mainwindow.h \
    ../../src/chatpanelmenu.h \
    ../../src/nicklistctrl.h \
    ../../src/tasserver.h \
    ../../src/utils/battleevents.h \
    ../../src/battlelisttab.h \
    ../../src/autohost.h \
    ../../src/gui/pastedialog.h \
    ../../src/Helper/wxtextctrlhist.h \
    ../../src/mainchattab.h \
    ../../src/reconnectdialog.h \
    ../../src/mainjoinbattletab.h \
    ../../src/battleroomtab.h \
    ../../src/qt/maplistmodel.h \
    ../../src/qt/qerrorwindow.h \
    ../../src/qt/sidemodel.h

INCLUDEPATH += ../../src
INCLUDEPATH += ../../src/Helper/qtargparser
DEFINES += SL_QT_MODE
DEFINES += NO_TORRENT_SYSTEM
DEFINES += DISABLE_SOUND

unix {
	CONFIG += link_pkgconfig
	PKGCONFIG += libcurl
	LIBS += $$system(wx-config --libs --unicode=yes) 
	QMAKE_CXXFLAGS += $$system(wx-config --cxxflags)
}

win32 {
	CONFIG += release
	LIBS += $$system(i686-pc-mingw32-wx-config --libs)
	QMAKE_CXXFLAGS += $$system(i686-pc-mingw32-wx-config --cxxflags)
	INCLUDEPATH += -I/opt/mingw_head/usr/i686-pc-mingw32/include/phonon
	#QMAKE_CXXFLAGS += -DCURL_STATICLIB
	LIBS += -L/opt/mingw_head/usr/i686-pc-mingw32/lib -L/opt/mingw_head/usr/i686-pc-mingw32/plugins/phonon_backend -L/opt/mingw_head/usr/i686-pc-mingw32/bin
	LIBS += -lphonon4 -lphonon_ds94 -lcurl -lgcrypt -liconv -lgpg-error -lidn -lwldap32 -lws2_32 -lgcrypt -liconv -lgpg-error -lz -lgnutls -lws2_32 -liphlpapi -lgcrypt -liconv -lgpg-error
}


OTHER_FILES += \
    ../../src/Helper/qtargparser/QtArg/XorArg \
    ../../src/Helper/qtargparser/QtArg/Visitor \
    ../../src/Helper/qtargparser/QtArg/MultiArg \
    ../../src/Helper/qtargparser/QtArg/Help \
    ../../src/Helper/qtargparser/QtArg/Exceptions \
    ../../src/Helper/qtargparser/QtArg/CmdLineContext \
    ../../src/Helper/qtargparser/QtArg/CmdLine \
    ../../src/Helper/qtargparser/QtArg/ArgConstraint \
    ../../src/Helper/qtargparser/QtArg/Arg \
	images/* \
    ../../src/iconimagelist.
