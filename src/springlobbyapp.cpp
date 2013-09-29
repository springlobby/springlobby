/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// Class: SpringLobbyApp
//

#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

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
#ifdef __WXMSW__
	#include <wx/msw/registry.h>
#endif
#include <wx/utils.h>
#include <wx/wfstream.h>

#include "useractions.h"
#include "springlobbyapp.h"
#include "mainwindow.h"
#include "settings.h"
#include "crashreport.h"
#include "utils/controls.h"
#include "utils/platform.h"
#include "channel/channel.h"
#include "utils/customdialogs.h"
#include "springsettings/se_utils.h"
#include "downloader/prdownloader.h"
#include "updater/updater.h"
#include <lslutils/globalsmanager.h>
#include "gui/notificationmanager.h"
#include "helper/wxTranslationHelper.h"
#include "playback/playbacktraits.h"
#include "playback/playbacktab.h"
#include "defines.h"
#include "customizations.h"
#include "sound/alsound.h"
#ifdef HAVE_CONFIG_H
#include "springlobby_config.h"
#else
	#define VERSION "unknown"
#endif

#include <wx/debugrpt.h>
#include <wx/intl.h>
#include "utils/misc.h"

#if wxUSE_UNIX
	#include <X11/Xlib.h>
#endif
const unsigned int TIMER_ID         = 101;
const unsigned int TIMER_INTERVAL   = 100;

IMPLEMENT_APP(SpringLobbyApp)

BEGIN_EVENT_TABLE(SpringLobbyApp, wxApp)
    EVT_TIMER(TIMER_ID, SpringLobbyApp::OnTimer)
END_EVENT_TABLE()

SpringLobbyApp::SpringLobbyApp()
    : 	m_timer ( new wxTimer(this, TIMER_ID) ),
    quit_called( false ),
    m_translationhelper( NULL ),
    m_log_verbosity( 3 ),
    m_log_console( true ),
	m_log_file( false ),
    m_log_window_show( false ),
    m_crash_handle_disable( false ),
	m_appname( _T("SpringLobby") )
{
#if wxUSE_UNIX
XInitThreads();
#endif
}

SpringLobbyApp::~SpringLobbyApp()
{
    delete m_timer;
}

//! @brief Initializes the application.
//!
//! It will open the main window and connect default to server or open the connect window.
bool SpringLobbyApp::OnInit()
{

	wxSetEnv( _T("UBUNTU_MENUPROXY"), _T("0") );
    //this triggers the Cli Parser amongst other stuff
    if (!wxApp::OnInit())
		return false;
	SetAppName( m_appname );

    if (!m_crash_handle_disable) {
    #if wxUSE_ON_FATAL_EXCEPTION
        wxHandleFatalExceptions( true );
    #endif
    }

    //initialize all loggers, we'll use the returned pointer to set correct parent window later
    wxLogChain* logchain = 0;
	wxLogWindow *loggerwin = InitializeLoggingTargets( 0, m_log_console, m_log_file_path, m_log_window_show, !m_crash_handle_disable, m_log_verbosity, logchain );

	wxLogMessage( _T("SpringLobby %s started"), _T(VERSION));

    //this needs to called _before_ mainwindow instance is created
    wxInitAllImageHandlers();
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxSocketBase::Initialize();


#ifdef __WXMSW__
    wxString path = wxPathOnly( wxStandardPaths::Get().GetExecutablePath() ) + wxFileName::GetPathSeparator() + _T("locale");
#else
	#if defined(LOCALE_INSTALL_DIR)
		wxString path ( _T(LOCALE_INSTALL_DIR) );
	#else
		// use a dummy name here, we're only interested in the base path
		wxString path = wxStandardPaths::Get().GetLocalizedResourcesDir(_T("noneWH"),wxStandardPaths::ResourceCat_Messages);
		path = path.Left( path.First(_T("noneWH") ) );
	#endif
#endif
    m_translationhelper = new wxTranslationHelper( *( (wxApp*)this ), path );
    m_translationhelper->Load();

	if ( !wxDirExists( GetConfigfileDir() ) )
		wxMkdir( GetConfigfileDir() );

	sett().SetSpringBinary( sett().GetCurrentUsedSpringIndex(), sett().GetCurrentUsedSpringBinary() );
	sett().SetUnitSync( sett().GetCurrentUsedSpringIndex(), sett().GetCurrentUsedUnitSync() );

	if ( sett().DoResetPerspectives() )
	{
		//we do this early on and reset the config var a little later so we can save a def. perps once mw is created
		sett().RemoveLayouts();
		sett().SetDoResetPerspectives( false );
		ui().mw().SavePerspectives( _T("SpringLobby-default") );
	}

	sett().RefreshSpringVersionList();


	if ( !m_customizer_archive_name.IsEmpty() )
	{//this needsto happen before usync load
		sett().SetForcedSpringConfigFilePath( GetCustomizedEngineConfigFilePath() );
	}
	//unitsync first load, NEEDS to be blocking
	const bool usync_loaded = LSL::usync().ReloadUnitSyncLib();
	if ( !sett().IsFirstRun() && !usync_loaded )
	{
		customMessageBox( SL_MAIN_ICON, _("Please check that the file given in Preferences->Spring is a proper, readable unitsync library"),
						 _("Coulnd't load required unitsync library"), wxOK );
	}

	#ifndef DISABLE_SOUND
		//sound sources/buffer init
		sound();
	#endif


	CacheAndSettingsSetup();

	if ( !m_customizer_archive_name.IsEmpty() ) {
		if ( SLcustomizations().Init( m_customizer_archive_name ) ) {
			ui().mw().SetIcons( SLcustomizations().GetAppIconBundle() );
		}
		else {
			customMessageBox( SL_MAIN_ICON, _("Couldn't load customizations for ") + m_customizer_archive_name + _("\nPlease check that that is the correct name, passed in qoutation"), _("Fatal error"), wxOK );
//            wxLogError( _("Couldn't load customizations for ") + m_customizer_archive_name + _("\nPlease check that that is the correct name, passed in qoutation"), _("Fatal error") );
			exit( OnExit() );//for some twisted reason returning false here does not terminate the app
		}
	}

	notificationManager(); //needs to be initialized too
    ui().ShowMainWindow();
    SetTopWindow( &ui().mw() );
	if ( sett().DoResetPerspectives() )
	{
		//now that mainwindow is shown, we can save what is the default layout and remove the flag to reset
		sett().SetDoResetPerspectives( false );
		ui().mw().SavePerspectives( _T("SpringLobby-default") );
	}

    ui().FirstRunWelcome();
    m_timer->Start( TIMER_INTERVAL );

	ui().mw().SetLogWin( loggerwin, logchain );
	return true;
}


//! @brief Finalizes the application
int SpringLobbyApp::OnExit()
{
    if ( quit_called )
        return 0;

    quit_called = true;
    wxLogDebugFunc( _T("") );

    if(m_translationhelper)
    {
        wxDELETE(m_translationhelper);
    }

  	m_timer->Stop();

  	sett().SaveSettings(); // to make sure that cache path gets saved before destroying unitsync

    SetEvtHandlerEnabled(false);
	UiEvents::GetNotificationEventSender().Enable( false );
    LSL::Util::DestroyGlobals();

    return 0;
}

//! @brief is called when the app crashes
void SpringLobbyApp::OnFatalException()
{
#if wxUSE_DEBUGREPORT && defined(ENABLE_DEBUG_REPORT)
    #if wxUSE_STACKWALKER
        CrashReport::instance().GenerateReport();
    #else
        EXCEPTION_POINTERS* p = new EXCEPTION_POINTERS; //lets hope this'll never get called
        CrashReport::instance().GenerateReport(p);
    #endif
#else
    wxMessageBox( _("The application has generated a fatal error and will be terminated\nGenerating a bug report is not possible\n\nplease get a wxWidgets library that supports wxUSE_DEBUGREPORT"),_("Critical error"), wxICON_ERROR | wxOK );
#endif
}


//! @brief Is called every 1/10 seconds to update statuses
void SpringLobbyApp::OnTimer( wxTimerEvent& event )
{
    ui().OnUpdate( event.GetInterval() );
}

bool SpringLobbyApp::SelectLanguage()
{
    wxArrayString names;
    wxArrayLong identifiers;
    int current_selection_index;
    m_translationhelper->GetInstalledLanguages( names, identifiers, current_selection_index );
    bool ret = m_translationhelper->AskUserForLanguage( names, identifiers, current_selection_index );
    if ( ret ) m_translationhelper->Save();
    return ret;
}

void SpringLobbyApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    #ifndef HAVE_WX29
        #define STR _T
    #else
        #define STR
    #endif

	#ifndef HAVE_WX29
		#define CSTR
	#else
		#define CSTR .c_str()
	#endif

    static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
		{ wxCMD_LINE_SWITCH, STR("h"), STR("help"), wxTRANSLATE("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
		{ wxCMD_LINE_SWITCH, STR("nc"), STR("no-crash-handler"), wxTRANSLATE("don't use the crash handler (useful for debugging)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
		{ wxCMD_LINE_OPTION, STR("fl"), STR("file-logging"),  wxTRANSLATE("dumps application log to a file ( enter path )"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_NEEDS_SEPARATOR },
		{ wxCMD_LINE_SWITCH, STR("cl"), STR("console-logging"),  wxTRANSLATE("shows application log to the console(if available)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
		{ wxCMD_LINE_SWITCH, STR("gl"), STR("gui-logging"),  wxTRANSLATE("enables application log window"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
		{ wxCMD_LINE_OPTION, STR("f"), STR("config-file"),  wxTRANSLATE("override default choice for config-file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_NEEDS_SEPARATOR },
		{ wxCMD_LINE_OPTION, STR("l"), STR("log-verbosity"),  wxTRANSLATE("overrides default logging verbosity, can be:\n                                0: no log\n                                1: critical errors\n                                2: errors\n                                3: warnings (default)\n                                4: messages\n                                5: function trace"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
		{ wxCMD_LINE_OPTION, STR("c"), STR("customize"),  wxTRANSLATE("load lobby customizations from game archive. Expects the shortname."), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
		{ wxCMD_LINE_OPTION, STR("n"), STR("name"),  wxTRANSLATE("overrides default application name"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
		{ wxCMD_LINE_NONE, NULL, NULL, NULL, wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL } //this is mandatory according to http://docs.wxwidgets.org/stable/wx_wxcmdlineparser.html

    };

    parser.SetDesc( cmdLineDesc );
    parser.SetSwitchChars (_T("-"));

    #undef STR
    #undef CSTR
}

//! @brief parses the command line and sets global app options like log verbosity or log target
bool SpringLobbyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
  #if wxUSE_CMDLINE_PARSER
    if ( !parser.Parse(true) )
    {
        m_log_console = parser.Found(_T("console-logging"));
        m_log_window_show = parser.Found(_T("gui-logging"));
		m_log_file = parser.Found(_T("file-logging"), &m_log_file_path);
        m_crash_handle_disable = parser.Found(_T("no-crash-handler"));

        Settings::m_user_defined_config = parser.Found( _T("config-file"), &Settings::m_user_defined_config_path );
        if ( Settings::m_user_defined_config ) {
             wxFileName fn ( Settings::m_user_defined_config_path );
             if ( ! fn.IsAbsolute() ) {
                 wxLogError ( _T("path for parameter \"config-file\" must be absolute") );
                 return false;
             }
             if ( ! fn.IsFileWritable() ) {
                 wxLogError ( _T("path for parameter \"config-file\" must be writeable") );
                 return false;
             }
        }

        if ( !parser.Found(_T("log-verbosity"), &m_log_verbosity ) )
            m_log_verbosity = m_log_window_show ? 3 : 5;
		if ( !parser.Found(_T("customize"), &m_customizer_archive_name ) )
			m_customizer_archive_name = _T("");
		if ( !parser.Found(_T("name"), &m_appname ) )
			m_appname = _T("SpringLobby");


        if ( parser.Found(_T("help")) )
            return false; // not a syntax error, but program should stop if user asked for command line usage

        return true;
    }
    else
    {
        return false;
    }
  #else // wxUSE_CMDLINE_PARSER
  return true;
  #endif
}

void SpringLobbyApp::CacheAndSettingsSetup()
{
    SetSettingsStandAlone( false );

	const wxString userConfigDir = GetConfigfileDir();
	if ( sett().IsFirstRun() && !wxDirExists( userConfigDir ) )
    {
		wxMkdir( userConfigDir );
    }
    if ( (sett().GetCacheVersion() < CACHE_VERSION) && !sett().IsFirstRun() )
    {
        sett().SetMapCachingThreadProgress( 0 ); // reset map cache thread
        sett().SetModCachingThreadProgress( 0 ); // reset mod cache thread
        if ( wxDirExists( sett().GetCachePath() )  )
        {
            wxLogWarning( _T("erasing old cache ver %d (app cache ver %d)"), sett().GetCacheVersion(), CACHE_VERSION );
            wxString file = wxFindFirstFile( sett().GetCachePath() + wxFILE_SEP_PATH + _T("*") );
            while ( !file.empty() )
            {
                wxRemoveFile( file );
                file = wxFindNextFile();
            }
        }
    }

    if ( !sett().IsFirstRun() )
    {
    	int settversion = sett().GetSettingsVersion();
    	if ( settversion < 3 )
    	{
				sett().ConvertOldSpringDirsOptions();
    	}
			if ( settversion < 4 )
			{
				if ( sett().GetTorrentPort() == DEFSETT_SPRING_PORT ) sett().SetTorrentPort( DEFSETT_SPRING_PORT + 1 );
			}
			if ( settversion < 5 )
			{
				wxArrayString list = sett().GetServers();
				int count = list.GetCount();
				wxArrayString wordlist = sett().GetHighlightedWords();
				for ( int i= 0; i < count; i++ )
				{
					wxString nick = sett().GetServerAccountNick( list[i] );
					if ( wordlist.Index( nick ) == -1 )
					{
						wordlist.Add( nick );
					}
				}
					sett().SetHighlightedWords( wordlist );
			}
			if ( settversion < 6 )
			{
				sett().ConvertOldServerSettings();
			}
			if ( settversion < 7 )
			{
				sett().AddChannelJoin( _T("springlobby"), _T("") );
			}
			if ( settversion < 10 )
			{
				sett().ConvertOldColorSettings();
			}
			if ( settversion < 11 )
			{
				if( IsUACenabled() )
				{
					LSL::usync().ReloadUnitSyncLib();
                    if ( LSL::usync().IsLoaded() ) LSL::usync().SetSpringDataPath(""); // UAC is on, fix the spring data path
				}
			}
			if ( settversion < 12 )
			{
				sett().ConvertOldChannelSettings();
			}
			if ( settversion < 13 )
			{
				sett().ConvertOldHiglightSettings();
			}
			if ( settversion < 15 )
			{
				sett().TranslateSavedColumWidths();
			}
			if ( settversion < 21 )
			{
				sett().RemoveLayouts();
			}
			if ( settversion < 18 )
			{
				//new downloader was introduced
				sett().ClearTorrentListToResume();
			}
			if( settversion < 19 )
			{
				//the dummy column hack was removed on win
				sett().NukeColumnWidths();
			}
			if ( settversion < 22 )
			{
				if ( m_translationhelper )
				{
					// add locale's language code to autojoin
					if ( m_translationhelper->GetLocale() )
					{
						wxString localecode = m_translationhelper->GetLocale()->GetCanonicalName();
						if ( localecode.Find(_T("_")) != -1 ) localecode = localecode.BeforeFirst(_T('_'));
						if ( localecode == _T("en") ) // we'll skip en for now, maybe in the future we'll reactivate it
							sett().AddChannelJoin( localecode, _T("") );
					}
				}
			}
            if ( settversion < 23 )
            {
                sett().ConvertLists();
                sett().AddKnownMatchmakerCPU(6667);
            }
		if ( settversion < 24 ) {
			sett().DeleteServer( _T("Backup server") );
			sett().DeleteServer( _T("Backup server 1") );
			sett().DeleteServer( _T("Backup server 2") );
			sett().SetDefaultServerSettings();
		}
	}

    if ( sett().ShouldAddDefaultServerSettings() || ( sett().GetSettingsVersion() < 14 && sett().GetServers().Count() < 2  ) )
        sett().SetDefaultServerSettings();

    if ( sett().ShouldAddDefaultChannelSettings() )
    {
        sett().AddChannelJoin( _T("main"), _T("") );
        sett().AddChannelJoin( _T("newbies"), _T("") );
		if ( m_translationhelper )
		{
			if ( m_translationhelper->GetLocale() )
			{
				wxString localecode = m_translationhelper->GetLocale()->GetCanonicalName();
				if ( localecode.Find(_T("_")) != -1 ) localecode = localecode.BeforeFirst(_T('_'));
				sett().AddChannelJoin( localecode, _T("") ); // add locale's language code to autojoin
			}
		}
    }

    if ( sett().ShouldAddDefaultGroupSettings() )
    {
         sett().AddGroup( _("Default") );
         sett().AddGroup( _("Ignore PM") );
         useractions().ChangeAction( _("Ignore PM"), UserActions::ActIgnorePM );
         sett().AddGroup( _("Ignore chat") );
         useractions().ChangeAction( _("Ignore chat"), UserActions::ActIgnoreChat );
         sett().AddGroup( _("Battle Autokick") );
         useractions().ChangeAction( _("Battle Autokick"), UserActions::ActAutokick );
         sett().AddGroup( _("Friends") );
         useractions().ChangeAction( _("Friends"), UserActions::ActNotifBattle );
         useractions().ChangeAction( _("Friends"), UserActions::ActHighlight );
         useractions().ChangeAction( _("Friends"), UserActions::ActNotifLogin );
         useractions().SetGroupColor( _("Friends"), wxColour( 0, 0, 255 ) );
    }
}

void SpringLobbyApp::OnQuit( wxCommandEvent& /*data*/ )
{
	m_timer->Stop();
	GlobalEvent::Send(GlobalEvent::OnQuit);
}

