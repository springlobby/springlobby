include(qmlapplicationviewer.pri)

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
QT += multimedia opengl webkit
# PKGCONFIG += vorbis vorbisfile
# LIBS += -lvorbisfile -lvorbis -logg

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += sasi_main.cpp \
    imageprovider.cpp \
    converters.cpp \
    skirmishmodel.cpp \
    noguisingleplayerbattle.cpp \
    ../Helper/TextCompletionDatabase.cpp \
	../utils/activitynotice.cpp \
	../chatpanelmenu.cpp \
	../gui/gradientpanel.cpp \
	../gui/toasternotification.cpp \
	../gui/wxgradientbutton.cpp \
	../gui/wxbackgroundimage.cpp \
	../gui/statusbar.cpp \
	../aui/slbook.cpp \
	../utils/math.cpp \
	../utils/misc.cpp \
	../gui/pastedialog.cpp \
	../utils/tasutil.cpp \
	../utils/networkevents.cpp \
	../utils/md5.c \
	../Helper/imageviewer.cpp \
	../Helper/slhtmlwindow.cpp \
	../channel/channelchooser.cpp \
	../channel/channelchooserdialog.cpp \
	../Helper/wxtextctrlhist.cpp \
	../Helper/colorbutton.cpp \
	../filelister/filelistctrl.cpp \
	../filelister/filelistdialog.cpp \
	../filelister/filelistfilter.cpp \
	../aui/auimanager.cpp \
	../aui/artprovider.cpp \
	../autobalancedialog.cpp \
	../autohost.cpp \
	../channel/autojoinchanneldialog.cpp \
	../addbotdialog.cpp \
	../agreementdialog.cpp \
	../base64.cpp \
	../battle.cpp \
	../battlelist.cpp \
	../battlelistctrl.cpp \
	../battlelistfilter.cpp \
	../battlelisttab.cpp \
	../battlemaptab.cpp \
	../battleoptionstab.cpp \
	../battleroomlistctrl.cpp \
	../battleroomtab.cpp \
	../crc.cpp \
	../channel/channel.cpp \
	../channel/channellist.cpp \
	../channel/channellistctrl.cpp \
	../chatlog.cpp \
	../chatoptionstab.cpp \
	../chatpanel.cpp \
	../connectwindow.cpp \
	../countrycodes.cpp \
	../customlistctrl.cpp \
	../flagimages.cpp \
	../groupoptionspanel.cpp \
	../hostbattledialog.cpp \
	../ibattle.cpp \
	../iconimagelist.cpp \
	../introguide.cpp \
	../lobbyoptionstab.cpp \
	../mainchattab.cpp \
	../mainjoinbattletab.cpp \
	../mainoptionstab.cpp \
	../mainsingleplayertab.cpp \
	../maintorrenttab.cpp \
	../mainwindow.cpp \
	../mapctrl.cpp \
	../mapgridctrl.cpp \
	../mapselectdialog.cpp \
	../mmoptionwindows.cpp \
	../nicklistctrl.cpp \
	../offlinebattle.cpp \
	../playback/replaylist.cpp \
	../playback/savegamelist.cpp \
	../alsound.cpp \
	../selectusersdialog.cpp \
	../server.cpp \
	../reconnectdialog.cpp \
	../serverevents.cpp \
	../singleplayerbattle.cpp \
	../singleplayertab.cpp \
	../socket.cpp \
	../spring.cpp \
	../springoptionstab.cpp \
	../springprocess.cpp \
	../tasserver.cpp \
	../tdfcontainer.cpp \
	../torrentlistctrl.cpp \
	../torrentoptionspanel.cpp \
	../torrentwrapper.cpp \
	../ui.cpp \
	../user.cpp \
	../useractions.cpp \
	../userlist.cpp \
	../utils/downloader.cpp \
	../utils/plasmaresourceinfo.cpp \
	../utils/battleevents.cpp \
	../widgets/downloadlistctrl.cpp \
	../widgets/downloaddialog.cpp \
	../widgets/downloadpanel.cpp \
	../widgets/infopanel.cpp \
	../widgets/widget.cpp \
	../Helper/tasclientimport.cpp \
	../simpleinterface/simpleserverevents.cpp \
	../toasterbox/ToasterBox.cpp \
	../toasterbox/ToasterBoxWindow.cpp \
	../toasterbox/ToasterBoxWindowList.cpp \
	../gui/notificationmanager.cpp \
	../pingthread.cpp \
	../iserverevents.cpp \
	../utils/curlhelper.cpp \
	../customizations.cpp \
	../uiutils.cpp \
	../updater/versionchecker.cpp \
	../updater/updatehelper.cpp \
	../utils/controls.cpp \
	../utils/conversion.cpp \
	../utils/customdialogs.cpp \
	../utils/debug.cpp \
	../utils/globalevents.cpp \
	../utils/platform.cpp \
	../utils/sltipwin.cpp \
	../utils/uievents.cpp \
	../crashreport.cpp \
	../globalsmanager.cpp \
	../mmoptionmodel.cpp \
	../mmoptionswrapper.cpp \
	../settings.cpp \
	../springunitsynclib.cpp \
	../springunitsync.cpp \
	../thread.cpp \
	../gui/windowattributespickle.cpp \
	../gui/spinctl/generic/spinctlg.cpp \
	../Helper/slconfig.cpp \
    maplistmodel.cpp \
    qerrorwindow.cpp \
    sidemodel.cpp \
    audio/audiomanager.cpp \
    audio/VorbisShared.cpp \
    audio/OggStream.cpp \
    audio/GEInterfaces.cpp \
    audio/GEAudioOut.cpp \
    audio/GEAudioBuffer.cpp \
    audio/ALShared.cpp \
    FileHandler.cpp \
    sasi_app.cpp

HEADERS += \
    ../springunitsynclib.h \
    ../globalsmanager.h \
    ../settings.h \
    ../Helper/slconfig.h \
    ../utils/platform.h \
    ../utils/misc.h \
    ../utils/curlhelper.h \
    ../springunitsync.h \
    ../springprocess.h \
    ../iunitsync.h \
    ../thread.h \
    ../utils/uievents.h \
    ../utils/globalevents.h \
    ../mmoptionswrapper.h \
    ../mmoptionmodel.h \
    ../uiutils.h \
    ../utils/customdialogs.h \
    ../customizations.h \
    ../utils/conversion.h \
    ../Helper/qtargparser/QtArg/xorarg.hpp \
    ../Helper/qtargparser/QtArg/visitor.hpp \
    ../Helper/qtargparser/QtArg/multiarg.hpp \
    ../Helper/qtargparser/QtArg/helpiface.hpp \
    ../Helper/qtargparser/QtArg/help.hpp \
    ../Helper/qtargparser/QtArg/exceptions.hpp \
    ../Helper/qtargparser/QtArg/cmdlineiface.hpp \
    ../Helper/qtargparser/QtArg/cmdlinecontext.hpp \
    ../Helper/qtargparser/QtArg/cmdline.hpp \
    ../Helper/qtargparser/QtArg/argconstraint.hpp \
    ../Helper/qtargparser/QtArg/arg.hpp \
    imageprovider.h \
    converters.h \
    skirmishmodel.h \
    ../user.h \
    ../ibattle.h \
    ../iconimagelist.h \
    ../userlist.h \
    ../chatpanel.h \
    ../spring.h \
    ../ui.h \
    ../tdfcontainer.h \
    noguisingleplayerbattle.h \
    ../battle.h \
    ../Helper/TextCompletionDatabase.hpp \
    ../channel/channel.h \
    ../chatlog.h \
    ../aui/auimanager.h \
    ../flagimages.h \
    ../flagimagedata.h \
    ../server.h \
    ../mainwindow.h \
    ../chatpanelmenu.h \
    ../nicklistctrl.h \
    ../tasserver.h \
    ../utils/battleevents.h \
    ../battlelisttab.h \
    ../autohost.h \
    ../gui/pastedialog.h \
    ../Helper/wxtextctrlhist.h \
    ../mainchattab.h \
    ../reconnectdialog.h \
    ../mainjoinbattletab.h \
    ../battleroomtab.h \
    maplistmodel.h \
    qerrorwindow.h \
    sidemodel.h \
    audio/audiomanager.h \
    audio/VorbisShared.h \
    audio/OggStream.h \
    audio/GEInterfaces.h \
    audio/GEAudioOut.h \
    audio/GEAudioBuffer.h \
    audio/ALShared.h \
    FileHandler.h \
    sasi_app.h

INCLUDEPATH += ..
INCLUDEPATH += ../Helper/qtargparser
DEFINES += SL_QT_MODE NO_TORRENT_SYSTEM DISABLE_SOUND

PKGCONFIG += openal sdl_sound sdl vorbis ogg vorbisfile

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
	QMAKE_CXXFLAGS += -DAL_LIBTYPE_STATIC
	LIBS += -lz -lgnutls -lws2_32 -liphlpapi -lgcrypt -liconv -lgpg-error -lqmlwebkitplugin
	LIBS += -lOpenAL32 -lidn -lwldap32 -lSDL -lSDL_sound -lvorbisenc -lvorbisfile -luuid  -lvorbis -logg
}


OTHER_FILES += \
    ../Helper/qtargparser/QtArg/XorArg \
    ../Helper/qtargparser/QtArg/Visitor \
    ../Helper/qtargparser/QtArg/MultiArg \
    ../Helper/qtargparser/QtArg/Help \
    ../Helper/qtargparser/QtArg/Exceptions \
    ../Helper/qtargparser/QtArg/CmdLineContext \
    ../Helper/qtargparser/QtArg/CmdLine \
    ../Helper/qtargparser/QtArg/ArgConstraint \
    ../Helper/qtargparser/QtArg/Arg 
