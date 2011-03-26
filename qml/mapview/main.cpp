#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"
#include <QDeclarativeContext>
#include <QDesktopWidget>
#include <QtOpenGL/QGLWidget>
#include <springunitsync.h>
#include <springunitsynclib.h>
#define USE_OPENGL

#include <settings.h>
#include <utils/platform.h>
#include <utils/conversion.h>
#include <iostream>
#include <customizations.h>
#include <qt/imageprovider.h>
#include <qt/converters.h>
#include <qt/maplistmodel.h>
#include <qt/skirmishmodel.h>
#include <qt/sidemodel.h>
#include <qt/qerrorwindow.h>

#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/timer.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/image.h>
#include <wx/cmdline.h>
#include <wx/filename.h>
#include <wx/dirdlg.h>
#include <wx/tooltip.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h> //filesystem zip handler
#include <wx/socket.h>
#include <wx/log.h>

#include <QtMultimedia/QAudioOutput>

#include <QtArg/Arg>
#include <QtArg/XorArg>
#include <QtArg/CmdLine>
#include <QtArg/Help>
#include <QDeclarativeEngine>
#include <QtCore/QDebug>
#include <QStringList>
#include <QIcon>
#include <QDir>

#include "qt/audio/audiomanager.h"

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
	QCoreApplication::addLibraryPath( QCoreApplication::applicationDirPath() );
	app.setOrganizationName("SpringLobby");
	app.setOrganizationDomain("SpringLobby.info");
	app.setApplicationName("awesomeFrontEnd");
	if ( !CmdInit() )
		return -1;

//	QIcon icon( wxBitmap(SLcustomizations().GetAppIcon()).GetHandle() );
//	app.setWindowIcon( icon );
	wxLogChain* logchain = 0;
	wxLog::SetActiveTarget( new wxLogChain( new wxLogStream( &std::cout ) ) );

	//this needs to called _before_ mainwindow instance is created
	wxInitAllImageHandlers();
	wxFileSystem::AddHandler(new wxZipFSHandler);
	wxSocketBase::Initialize();


	QDeclarativeView view;
	QString qmldir = SLcustomizations().QmlDir();
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

	AudioManager* audio_manager = new AudioManager(&app);
	audio_manager->enqueue( SLcustomizations().MusicDir() + "/bg_music.ogg" );
	audio_manager->start();
	    
	QObject::connect((QObject*)view.engine(), SIGNAL(quit()), &app, SLOT(quit()));
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

	return app.exec();
}

