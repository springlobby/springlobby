/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/


#include "sasi_app.h"

#include <QDeclarativeEngine>
#include <QIcon>
#include <QDir>
#include <QDebug>
#include <QDeclarativeContext>
#include <QtDeclarative/QDeclarativeView>
#include <QDesktopWidget>
#include <QtOpenGL/QGLWidget>
#include <QSplashScreen>

#include <customizations.h>
#include <springunitsynclib.h>

#include <wx/fs_zip.h> //filesystem zip handler
#include <wx/socket.h>
#include <wx/filename.h>

#include <iostream>

#include "audio/audiomanager.h"
#include "imageprovider.h"
#include "converters.h"
#include "maplistmodel.h"
#include "skirmishmodel.h"
#include "sidemodel.h"
#include "qerrorwindow.h"
#include "engineconfig.h"
#include <spring.h>

#ifdef __WXMSW__
	#include <windows.h>
	void sleep(int n) { Sleep(1000 * n); }
#endif

#if defined(Q_WS_X11) && !defined(QT_NO_DEBUG)
	//QGlWidget segfaults in debug builds..
	#define USE_OPENGL 0
#else
	#define USE_OPENGL 1
#endif

SasiApp::SasiApp(int argc, char *argv[])
	: QApplication(argc,argv)
{
	QCoreApplication::addLibraryPath( QCoreApplication::applicationDirPath() );
	setOrganizationName("SpringLobby");
	setOrganizationDomain("SpringLobby.info");
	setApplicationName("saSi");

//	QIcon icon( wxBitmap(SLcustomizations().GetAppIcon()) );
//	setWindowIcon( icon );
}

int SasiApp::exec()
{		
	QSplashScreen* splash = 0;
	QPixmap splash_pixmap;
	QWidget* show_screen = desktop()->screen( 0 );
	if ( splash_pixmap.load( SLcustomizations().GraphicsDir() + "/splash.png" ) )
	{
		splash = new QSplashScreen(show_screen,splash_pixmap);
		splash->show();
	}
	wxLogChain* logchain = 0;
	wxLog::SetActiveTarget( new wxLogChain( new wxLogStream( &std::cout ) ) );

	//this needs to called _before_ mainwindow instance is created
	wxInitAllImageHandlers();
	wxFileSystem::AddHandler(new wxZipFSHandler);
	wxSocketBase::Initialize();

	usync().FastLoadUnitSyncLibInit( );

	QDeclarativeView view(show_screen);
	QString qmldir;
	try {
		qmldir = SLcustomizations().QmlDir();
	}
	catch ( Customizations::DataException& e )
	{
		//for some fucked up reason the strings get internally fucked up w/o the hardcopy
		QList<QString> copy = e.errors_;
		QErrorWindow error_win ( copy );
		return error_win.exec();
	}

	AudioManager audio_manager (this);
	audio_manager.start();

	view.engine()->addImportPath( qmldir );
#ifdef __WXMSW__
	//for webkit declarative plugin
	view.engine()->addImportPath( QDir( QCoreApplication::applicationDirPath() + "/imports").absolutePath() );
#endif

	// Visual initialization
	view.engine()->addImageProvider("minimaps", new MinimapImageProvider);
	view.engine()->addImageProvider("graphics", new GraphicsProvider);
	view.engine()->addImageProvider("sides", new SideImageProvider);
#if USE_OPENGL
	QGLFormat format = QGLFormat::defaultFormat();
	#ifdef Q_WS_MAC
		format.setSampleBuffers(true);
	#else
		format.setSampleBuffers(false);
	#endif
	QGLWidget *glWidget = new QGLWidget(format, &view);
	view.setViewport(glWidget);
	view.setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
#endif
	view.setAttribute(Qt::WA_OpaquePaintEvent);
	view.setAttribute(Qt::WA_NoSystemBackground);
	view.setResizeMode(QDeclarativeView::SizeRootObjectToView);

	MaplistModel maplist_model( usync().GetMapList() );
	SkirmishModel skirmish_model;
	SideModel side_model( SLcustomizations().GetModname() );
	PresetModel preset_model(this);
	ScreenResolutionModel screenres_model(this);

	spring().connect( &spring(), SIGNAL(springStarted()), &audio_manager, SLOT(pause()));
	spring().connect( &spring(), SIGNAL(springStopped()), &audio_manager, SLOT(resume()));

	QObject::connect((QObject*)view.engine(), SIGNAL(quit()), this, SLOT(quit()));
	QObject::connect((QObject*)view.engine(), SIGNAL(quit()), &audio_manager, SLOT(doQuit()));
	QDeclarativeContext* ctxt = view.rootContext();
	ctxt->setContextProperty("maplistModel", &maplist_model );
	ctxt->setContextProperty("skirmishModel", &skirmish_model );
	ctxt->setContextProperty("sideModel", &side_model );
	ctxt->setContextProperty("audioManager", &audio_manager );
	ctxt->setContextProperty("presetModel", &preset_model );
	ctxt->setContextProperty("screenresModel", &screenres_model );

	const int sleep_seconds = -1;
	for ( int i = sleep_seconds; splash && i > 0; i-- ) {
		splash->showMessage( QString("sleeping for %1 seconds, just to show you this").arg( i ), Qt::AlignHCenter | Qt::AlignBottom );
		processEvents();
		sleep( 1 );
	}

	//	view.showFullScreen();
	if ( splash )
		splash->finish(&view);
	view.setSource(QUrl(qmldir + "/main.qml"));
	QObject::connect(this, SIGNAL(appLoaded()), (QObject*)view.rootObject(), SLOT(onAppLoaded()));

	QList<QDeclarativeError> errors = view.errors();
	if ( errors.size() )
	{
		QErrorWindow error_window ( errors );
		return error_window.exec();
	}
	view.show();
	view.setFocus();
	emit appLoaded();
	int ret = QApplication::exec();
	audio_manager.wait( 5 /*seconds*/ );
	return ret;
}
