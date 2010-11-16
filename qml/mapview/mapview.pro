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
# DEFINES += QMLJSDEBUGGER

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the 
# MOBILITY variable. 
# CONFIG += mobility
# MOBILITY +=

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    ../../src/springunitsynclib.cpp \
    ../../src/globalsmanager.cpp \
    ../../src/settings.cpp \
    ../../src/Helper/slconfig.cpp \
    ../../src/utils/platform.cpp \
    ../../src/utils/misc.cpp \
    ../../src/utils/curlhelper.cpp \
    stringdummy.cpp \
    ../../src/springunitsync.cpp \
    ../../src/springprocess.cpp \
    ../../src/thread.cpp \
    ../../src/utils/uievents.cpp \
    ../../src/utils/globalevents.cpp \
    ../../src/mmoptionswrapper.cpp \
    ../../src/mmoptionmodel.cpp \
    ../../src/uiutils.cpp \
    ../../src/utils/customdialogs.cpp \
    ../../src/customizations.cpp \
    ../../src/utils/conversion.cpp

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
    stringdummy.h \
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
    ../../src/utils/conversion.h

INCLUDEPATH += ../../src
DEFINES += SL_QT_MODE

unix {
		CONFIG += link_pkgconfig
		PKGCONFIG += libcurl
		LIBS += $$system(wx-config --libs)
		QMAKE_CXXFLAGS += $$system(wx-config --cppflags)
}
