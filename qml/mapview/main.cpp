#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"
#include <QDeclarativeContext>
#include <QtOpenGL/QGLWidget>
#include <stringdummy.h>
#include <springunitsync.h>
#define USE_OPENGL

#include <settings.h>
#include <utils/platform.h>
#include <iostream>

#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/timer.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/image.h>
#include <wx/cmdline.h>
#include <wx/choicdlg.h>
#include <wx/filename.h>
#include <wx/dirdlg.h>
#include <wx/tooltip.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h> //filesystem zip handler
#include <wx/socket.h>

#include <QStringList>

int main(int argc, char *argv[])
{

		QApplication app(argc, argv);
		wxLogChain* logchain = 0;
		wxLog::SetActiveTarget( new wxLogChain( new wxLogStream( &std::cout ) ) );

		//this needs to called _before_ mainwindow instance is created
		wxInitAllImageHandlers();
		wxFileSystem::AddHandler(new wxZipFSHandler);
		wxSocketBase::Initialize();

		QDeclarativeView view;
		// Visual initialization
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
		if ( !wxDirExists( GetConfigfileDir() ) )
			wxMkdir( GetConfigfileDir() );

	#ifdef __WXMSW__
		sett().SetSearchSpringOnlyInSLPath( sett().GetSearchSpringOnlyInSLPath() );
	#endif
		sett().SetSpringBinary( sett().GetCurrentUsedSpringIndex(), sett().GetCurrentUsedSpringBinary() );
		sett().SetUnitSync( sett().GetCurrentUsedSpringIndex(), sett().GetCurrentUsedUnitSync() );

		view.setSource(QString("qml/mapview/main.qml"));//usync resets pwd, figure out how to put qml in qrc
		usync().ReloadUnitSyncLib();

		QStringList maps;
		wxString mapname;
		foreach (mapname, usync().GetMapList() ) {
			maps.append( QString(mapname.mb_str()) );
		}

		QString de( sett().GetCachePath().mb_str() );
		StringDummy test(de );
		QDeclarativeContext* ctxt = view.rootContext();
		ctxt->setContextProperty("myAwesomeString", &test );
		ctxt->setContextProperty("myModel", QVariant::fromValue(maps) );

		view.show();


	return app.exec();
}

