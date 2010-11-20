#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"
#include <QDeclarativeContext>
#include <QDesktopWidget>
#include <QtOpenGL/QGLWidget>
#include <springunitsync.h>
#define USE_OPENGL

#include <settings.h>
#include <utils/platform.h>
#include <utils/conversion.h>
#include <iostream>
#include <customizations.h>
#include <qt/imageprovider.h>

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
#include <wx/log.h>

#include <QStringList>
#include <QtArg/Arg>
#include <QtArg/XorArg>
#include <QtArg/CmdLine>
#include <QtArg/Help>
#include <QDeclarativeEngine>
#include <QtCore/QDebug>

bool CmdInit()
{
	PwdGuard pwd_guard;//makes us invulnerabel to cwd changes in usync loading
	QtArgCmdLine cmd;
	QtArg config_file( 'f', "config-file", "absolute path to config file", false, true );
	QtArg customization( 'c', "customize", "Load lobby customizations from game archive. Expects the long name.", false, true );
	QtArgDefaultHelpPrinter helpPrinter( "Testing help printing.\n" );
	QtArgHelp help( &cmd );
	help.setPrinter( &helpPrinter );
	cmd.addArg( config_file );
	cmd.addArg( customization );
	try {
			cmd.parse();
	}
	catch( const QtHelpHasPrintedEx & x )
	{
	}
	catch( const QtArgBaseException & x )
	{
			qDebug() << x.what();
			return false;
	}
	Settings::m_user_defined_config = config_file.isPresent();

	if ( Settings::m_user_defined_config ) {
		Settings::m_user_defined_config_path = TowxString( config_file.value().toString().toStdString() );
		 wxFileName fn ( Settings::m_user_defined_config_path );
		 if ( ! fn.IsAbsolute() ) {
			 qDebug() << "path for parameter \"config-file\" must be absolute";
			 return false;
		 }
		 if ( ! fn.IsFileWritable() ) {
			 qDebug() << "path for parameter \"config-file\" must be writeable";
			 return false;
		 }
		 qDebug() << Settings::m_user_defined_config_path.mb_str();
	}

#ifdef __WXMSW__
	sett().SetSearchSpringOnlyInSLPath( sett().GetSearchSpringOnlyInSLPath() );
#endif
	sett().SetSpringBinary( sett().GetCurrentUsedSpringIndex(), sett().GetCurrentUsedSpringBinary() );
	sett().SetUnitSync( sett().GetCurrentUsedSpringIndex(), sett().GetCurrentUsedUnitSync() );

	if ( !wxDirExists( GetConfigfileDir() ) )
		wxMkdir( GetConfigfileDir() );

	usync().ReloadUnitSyncLib();

	if ( customization.isPresent() ) {
		QString ko = customization.value().toString();
		qDebug() << ko;
		wxString kko = TowxString( ko.toStdString() );
				wxLogError( kko.c_str() );
		if ( !SLcustomizations().Init( kko ) ) {
			qDebug() << "init false";
			return false;
		}
	}

	return true;
}

int main(int argc, char *argv[])
{
		QApplication app(argc, argv);
		if ( !CmdInit() )
			return -1;

		wxLogChain* logchain = 0;
		wxLog::SetActiveTarget( new wxLogChain( new wxLogStream( &std::cout ) ) );

		//this needs to called _before_ mainwindow instance is created
		wxInitAllImageHandlers();
		wxFileSystem::AddHandler(new wxZipFSHandler);
		wxSocketBase::Initialize();

		QDeclarativeView view;
		view.engine()->addImageProvider("minimaps", new MinimapImageProvider);
		view.engine()->addImageProvider("images", new ImageProvider);
		view.engine()->addImportPath("qml/mapview/");
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

		QStringList maps;
		wxString mapname;
		foreach (mapname, usync().GetMapList() ) {
			maps.append( QString(mapname.mb_str()) );
		}

		QString de( sett().GetCachePath().mb_str() );
		QDeclarativeContext* ctxt = view.rootContext();
		ctxt->setContextProperty("myModel", QVariant::fromValue(maps) );
		view.setSource(QUrl("qml/mapview/main.qml"));//usync resets pwd, figure out how to put qml in qrc
		QRect d = app.desktop()->screenGeometry();
		d.height();

		view.showFullScreen();
//		view.show();

	return app.exec();
}

