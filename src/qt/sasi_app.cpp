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
#include <QTimer>
#include <QDir>
#include <QDebug>
#include <QtDeclarative>
#include <QDeclarativeContext>
#include <QtDeclarative/QDeclarativeView>
#include <QDesktopWidget>
#include <QtOpenGL/QGLWidget>
#include <QSplashScreen>
#include <QMessageBox>

#include <QtArg/Arg>
#include <QtArg/XorArg>
#include <QtArg/CmdLine>
#include <QtArg/Help>

#include <wx/fs_zip.h> //filesystem zip handler
#include <wx/socket.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/filename.h>

#include <iostream>

#include <customizations.h>
#include <springunitsynclib.h>
#include <utils/platform.h>
#include <springunitsync.h>
#include <settings.h>
#include <utils/conversion.h>
#include <customizations.h>
#include <globalsmanager.h>
#include "audio/audiomanager.h"
#include "imageprovider.h"
#include "converters.h"
#include "maplistmodel.h"
#include "skirmishmodel.h"
#include "sidemodel.h"
#include "qerrorwindow.h"
#include "engineconfig.h"
#include <spring.h>
#include "battlelistmodel.h"
#include "server.h"
#include <tasserver.h>
#include <iserverevents.h>
#include "qbattleroom.h"
#include "qminimap.h"
#include "missionmodel.h"

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

	qmlRegisterType<QBattleroom>("Sasi", 1, 0, "Battleroom");
	qmlRegisterType<QMinimap>("Sasi", 1, 0, "ExtendedMinimap");

    //	QIcon icon( wxBitmap(SLcustomizations().GetAppIcon()) );
    //	setWindowIcon( icon );
}

SasiApp::~SasiApp()
{
    delete bl_model;
    delete m_server;
}

int SasiApp::exec()
{		
    QSplashScreen* splash = 0;
    QPixmap splash_pixmap;
    QWidget* show_screen = desktop()->screen( 0 );
    try {
        QString graphic_dir = SLcustomizations().GraphicsDir() ;
    }
    catch ( std::exception& e ) {
            qDebug() << e.what();
            throw e;
    }
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

    m_server = new TASServer( IServerEvents::simple );
    serverSelector().SetCurrentServer( m_server );
    const wxString servername = sett().GetDefaultServer();
    const wxString username = sett().GetServerAccountNick( servername );
    const wxString password = sett().GetServerAccountPass( servername );

    serverSelector().GetServer().SetUsername( username );
    serverSelector().GetServer().SetPassword( password );
    if ( sett().GetServerAccountSavePass( servername ) )
    {
        if ( serverSelector().GetServer().IsPasswordHash(password) ) sett().SetServerAccountPass( servername, password );
        else sett().SetServerAccountPass( servername, serverSelector().GetServer().GetPasswordHash( password ) );
    }
    else
    {
        sett().SetServerAccountPass( servername, _T("") );
    }
    const wxString host = sett().GetServerHost( servername );
    const int port = sett().GetServerPort( servername );
    serverSelector().GetServer().Connect( servername, host, port );

    //reordering will prolly break stuff like sides list
    MaplistModel maplist_model( usync().GetMapList() );
    SideModel side_model( SLcustomizations().GetModname() );
    SkirmishModel skirmish_model;
    PresetModel preset_model(this);
    ScreenResolutionModel screenres_model(this);
    MissionModel mission_model(this);

    //! TODO switch bakc to modname
    wxString modname = SLcustomizations().GetModname();
    bl_model = new BattlelistModel( modname.SubString(0,10), this );

    serverSelector().GetServer().Update( 100 );

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
    ctxt->setContextProperty("missionModel", &mission_model );
    ctxt->setContextProperty("battlelistModel", bl_model);

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

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(Update()));
    timer->start(100);
	bl_model->reload();
    view.show();
    view.setFocus();
    emit appLoaded();

    int ret = QApplication::exec();
    serverSelector().GetServer().Disconnect();
    audio_manager.wait( 5 /*seconds*/ );
    sett().SaveSettings();
    return ret;
}

bool SasiApp::CmdInit()
{
    PwdGuard pwd_guard;//makes us invulnerabel to cwd changes in usync loading
    QtArgCmdLine cmd;
    QtArg config_file( 'f', "config-file", "absolute path to config file", false, true );
    QtArg customization( 'c', "customize", "Load lobby customizations from special archive.", true, true );
    QtArg shortname( 's', "shortname", "shortname.", true, true );
    QtArg version( 'r', "revision", "revision.", true, true );
    QtArg appname( 'n', "name", "name", true, true );
    cmd.addArg( config_file );
    cmd.addArg( customization );
    cmd.addArg( shortname );
    cmd.addArg( version );
    cmd.addArg( appname );
	QtArgDefaultHelpPrinter helpPrinter( "Testing help printing.\n" );
	QtArgHelp help( &cmd );
	help.setPrinter( &helpPrinter );
    try {
        cmd.parse();
    }
    catch( const QtHelpHasPrintedEx & x ) {	}
    catch( const QtArgBaseException & x )
    {
        qDebug() << x.what();
        QMessageBox::critical( 0, "Fatal error", QString("Parsing command line failed:\n").append(x.what()) );
        return false;
    }
    Settings::m_user_defined_config = config_file.isPresent();

    if ( Settings::m_user_defined_config ) {
        Settings::m_user_defined_config_path = TowxString( config_file.value().toString() );
        qDebug() << ToQString( Settings::m_user_defined_config_path );
    }

#ifdef __WXMSW__
    sett().SetSearchSpringOnlyInSLPath( false );
#endif
    QString customization_value = customization.value().toString();
    QString shortname_value = shortname.value().toString();
    QString version_value = version.value().toString();
    setApplicationName(appname.value().toString());
    //must go BEFORE usync loading
    sett().SetForcedSpringConfigFilePath( GetCustomizedEngineConfigFilePath() );
    //	sett().SetSpringBinary( sett().GetCurrentUsedSpringIndex(), sett().GetCurrentUsedSpringBinary() );
    //	sett().SetUnitSync( sett().GetCurrentUsedSpringIndex(), sett().GetCurrentUsedUnitSync() );

    qDebug() << "current used usync path" << ToQString(sett().GetCurrentUsedUnitSync()) ;

    if ( !wxDirExists( GetConfigfileDir() ) )
        wxMkdir( GetConfigfileDir() );

    usync().FastLoadUnitSyncLib( sett().GetCurrentUsedUnitSync() );

    qDebug() << QString( "shortname: %1").arg( shortname_value );
    if ( !SLcustomizations().Init( TowxString(customization_value), shortname_value, version_value ) )
    {
        qDebug() << "init false";
        QMessageBox::critical( 0, "Fatal error", QString("loading customizations failed for ").append( shortname_value ) );
        return false;
    }
    return true;
}

void SasiApp::Update()
{
    static unsigned int count = 0;
    serverSelector().GetServer().Update( 100 );
    count++;
	if ( count % 113 == 0 )
		bl_model->reload();
}
