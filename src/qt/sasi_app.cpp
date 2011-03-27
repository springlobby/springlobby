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

#include <customizations.h>
#include <springunitsynclib.h>

#include "audio/audiomanager.h"
#include "imageprovider.h"
#include "converters.h"
#include "maplistmodel.h"
#include "skirmishmodel.h"
#include "sidemodel.h"
#include "qerrorwindow.h"

#define USE_OPENGL

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

	QDeclarativeView view;
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
	qDebug() << "qmldir" << qmldir;
	view.engine()->addImportPath( qmldir );

#ifdef __WXMSW__
	//for webkit declarative plugin
	view.engine()->addImportPath( QDir( QCoreApplication::applicationDirPath() + "/imports").absolutePath() );
#endif

	// Visual initialization
	view.engine()->addImageProvider("minimaps", new MinimapImageProvider);
	view.engine()->addImageProvider("graphics", new GraphicsProvider);
	view.engine()->addImageProvider("sides", new SideImageProvider);

#ifdef USE_OPENGL
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
	view.setFocus();

	MaplistModel maplist_model( usync().GetMapList() );
	SkirmishModel skirmish_model;
	SideModel side_model( SLcustomizations().GetModname() );

	AudioManager* audio_manager = new AudioManager(this);
	audio_manager->start();

	QObject::connect((QObject*)view.engine(), SIGNAL(quit()), this, SLOT(quit()));
	QDeclarativeContext* ctxt = view.rootContext();
	ctxt->setContextProperty("myModel", &maplist_model );
	ctxt->setContextProperty("skirmishModel", &skirmish_model );
	ctxt->setContextProperty("sideModel", &side_model );
	view.setSource(QUrl(qmldir + "/main.qml"));//usync resets pwd, figure out how to put qml in qrc

	//	QRect d = app.desktop()->screenGeometry();

	QList<QDeclarativeError> errors = view.errors();
	if ( errors.size() )
	{
		QErrorWindow error_window ( errors );
		return error_window.exec();
	}


	//	view.showFullScreen();
	view.show();

	return QApplication::exec();
}
