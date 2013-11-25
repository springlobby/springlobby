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

#include "springlobbyapp.h"
#include "mainwindow.h"
#include "settings.h"
#include "helper/slconfig.h"
#include "crashreport.h"
#include "utils/controls.h"
#include "utils/platform.h"
#include "channel/channel.h"
#include "utils/customdialogs.h"
#include "downloader/prdownloader.h"
#include "updater/updater.h"
#include <lslutils/globalsmanager.h>
#include "gui/notificationmanager.h"
#include "helper/wxTranslationHelper.h"
#include "playback/playbacktraits.h"
#include "playback/playbacktab.h"
#include "defines.h"
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

IMPLEMENT_APP(SpringLobbyApp)

BEGIN_EVENT_TABLE(SpringLobbyApp, wxApp)
END_EVENT_TABLE()

SpringLobbyApp::SpringLobbyApp():
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
    m_translationhelper = new wxTranslationHelper( GetAppName().Lower(), path );

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


	//unitsync first load, NEEDS to be blocking
	const bool usync_loaded = LSL::usync().ReloadUnitSyncLib();
	if ( !cfg().ReadBool(_T("/General/firstrun")) && !usync_loaded )
	{
		customMessageBox( SL_MAIN_ICON, _("Please check that the file given in Preferences->Spring is a proper, readable unitsync library"),
						 _("Coulnd't load required unitsync library"), wxOK );
	}

	#ifndef DISABLE_SOUND
		//sound sources/buffer init
		sound();
	#endif

	sett().Setup(m_translationhelper);

	notificationManager(); //needs to be initialized too
    ui().ShowMainWindow();
    SetTopWindow( &ui().mw() );
	if ( sett().DoResetPerspectives() )
	{
		//now that mainwindow is shown, we can save what is the default layout and remove the flag to reset
		sett().SetDoResetPerspectives( false );
		ui().mw().SavePerspectives( _T("SpringLobby-default") );
	}

    ui().OnInit();

	ui().mw().SetLogWin( loggerwin, logchain );
	return true;
}


//! @brief Finalizes the application
int SpringLobbyApp::OnExit()
{
    if ( quit_called )
        return 0;

    quit_called = true;
    wxLogDebugFunc( wxEmptyString );

    if(m_translationhelper)
    {
        wxDELETE(m_translationhelper);
    }

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


bool SpringLobbyApp::SelectLanguage()
{
	return m_translationhelper->AskUserForLanguage();
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

        // TODO make sure this is called before settings are accessed
        slConfig::m_user_defined_config = parser.Found( _T("config-file"), &slConfig::m_user_defined_config_path );
        if ( slConfig::m_user_defined_config ) {
             wxFileName fn ( slConfig::m_user_defined_config_path );
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


void SpringLobbyApp::OnQuit( wxCommandEvent& /*data*/ )
{
	GlobalEvent::Send(GlobalEvent::OnQuit);
}

