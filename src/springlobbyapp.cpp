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
#include "ui.h"
#include "iunitsync.h"
#include "channel/channel.h"
#include "utils/customdialogs.h"
#include "settings++/se_utils.h"
#ifndef NO_TORRENT_SYSTEM
#include "torrentwrapper.h"
#endif
#include "updater/updater.h"
#include "globalsmanager.h"
#include "Helper/wxTranslationHelper.h"
#include "Helper/tasclientimport.h"
#include "playback/playbacktraits.h"
#include "playback/playbacktab.h"
#include "updater/versionchecker.h"
#include "updater/updatermainwindow.h"
#include "defines.h"

const unsigned int TIMER_ID         = 101;
const unsigned int TIMER_INTERVAL   = 100;


#if 0
/// testing TDF parser
#include "tdfcontainer.h"
#include <iostream>
#include <fstream>
void TestTDFParser(){
  PDataList parsetree(new DataList);
  Tokenizer tokenizer;
  std::ifstream f("/home/dmytry/.spring/script.txt");
  tokenizer.EnterStream(f);
  parsetree->Load(tokenizer);
  wxString result;
  TDFWriter writer(result);
  parsetree->Save(writer);
  wxLogMessage(_T("Testing tdf parser: result %s "), result.c_str());
}
#endif

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
    m_log_window_show( false ),
    m_crash_handle_disable( false ),
    m_updateing_only( false ),
    m_updater_window( 0 )
{
    SetAppName( _T("springlobby") );
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
    //this triggers the Cli Parser amongst other stuff
    if (!wxApp::OnInit())
        return false;

    //initialize all loggers, we'll use the returned pointer to set correct parent window later
    wxLogChain* logchain = 0;
    wxLogWindow *loggerwin = InitializeLoggingTargets( 0, m_log_console, m_log_window_show, !m_crash_handle_disable, m_log_verbosity, logchain );

#if wxUSE_ON_FATAL_EXCEPTION
    if (!m_crash_handle_disable) wxHandleFatalExceptions( true );
#endif

    //this needs to called _before_ mainwindow instance is created
    wxInitAllImageHandlers();
     //TODO needed?
    wxImage::AddHandler(new wxPNGHandler);
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxSocketBase::Initialize();


#ifdef __WXMSW__
    wxString path = wxPathOnly( wxStandardPaths::Get().GetExecutablePath() ) + wxFileName::GetPathSeparator() + _T("locale");
#else
    // use a dummy name here, we're only interested in the base path
    wxString path = wxStandardPaths::Get().GetLocalizedResourcesDir(_T("noneWH"),wxStandardPaths::ResourceCat_Messages);
    path = path.Left( path.First(_T("noneWH") ) );
#endif
    m_translationhelper = new wxTranslationHelper( *( (wxApp*)this ), path );
    m_translationhelper->Load();

    if ( !wxDirExists( wxStandardPaths::Get().GetUserDataDir() ) )
        wxMkdir( wxStandardPaths::Get().GetUserDataDir() );

    sett().RefreshSpringVersionList();

#ifdef __WXMSW__
    //everything below should not be executing when updating, so we can ensure no MainWindow window is created, torrent system isn't started, etc.
    // NOTE: this assumes no one will try to update at firstRun
    if ( m_updateing_only ) {
        wxString latestVersion = GetLatestVersion();
        m_updater_window = new UpdaterMainwindow( latestVersion );
        m_updater_window->Show( true );
        SetTopWindow( m_updater_window );
        Updater().StartUpdate( latestVersion );
        return true;
    }
#endif
    usync(); //init object, sink needs to exist before event is posted. next line would do both object(sink) creation and Event posting
    GetGlobalEventSender(GlobalEvents::UnitSyncReloadRequest).SendEvent( 0 ); // request an unitsync reload

    CacheAndSettingsSetup();
    ui().ShowMainWindow();
    SetTopWindow( &ui().mw() );

    if ( sett().IsFirstRun() )
    {
#ifdef __WXMSW__
        sett().SetOldSpringLaunchMethod( true );
#endif

        wxLogMessage( _T("first time startup"));
        wxMessageBox(_("Hi ") + wxGetUserName() + _(",\nIt looks like this is your first time using SpringLobby. I have guessed a configuration that I think will work for you but you should review it, especially the Spring configuration. \n\nWhen you are done you can go to the File menu, connect to a server, and enjoy a nice game of Spring :)"), _("Welcome"),
                     wxOK | wxICON_INFORMATION, &ui().mw() );


        customMessageBoxNoModal(SL_MAIN_ICON, _("By default SpringLobby reports some usage statistics.\nYou can disable that on options tab --> General."),_("Notice"),wxOK );


                // copy uikeys.txt
                wxPathList pl;
                pl.AddEnvList( _T("%ProgramFiles%") );
                pl.AddEnvList( _T("XDG_DATA_DIRS") );
                pl = sett().GetAdditionalSearchPaths( pl );
                wxString uikeyslocation = pl.FindValidPath( _T("uikeys.txt") );
                if ( !uikeyslocation.IsEmpty() )
                {
                    wxCopyFile( uikeyslocation, sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator() + _T("uikeys.txt"), false );
                }

    #ifdef __WXMSW__
        if ( TASClientPresent() &&
                customMessageBox(SL_MAIN_ICON, _("Should I try to import (some) TASClient settings?\n" ),_("Import settings?"), wxYES_NO ) == wxYES )
        {
            ImportTASClientSettings();
        }
    #endif

        ui().mw().ShowConfigure();
    }
    else
    {
        ui().mw().ShowSingleplayer();
    }

#ifndef NO_TORRENT_SYSTEM
    if( sett().GetTorrentSystemAutoStartMode() == 1 ) torrent().ConnectToP2PSystem();
#endif

    //starts the replay loading process in a thread
    ui().mw().GetReplayTab().ReloadList();
    ui().mw().GetSavegameTab().ReloadList();
    wxLogMessage( _T("Replaytab updated") );

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

  DestroyGlobals();

  return 0;
}

//! @brief is called when the app crashes
void SpringLobbyApp::OnFatalException()
{
#if wxUSE_DEBUGREPORT && defined(ENABLE_DEBUG_REPORT)
    crashreport().GenerateReport(wxDebugReport::Context_Exception);
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
        wxCmdLineEntryDesc cmdLineDesc[] =
        {
            { wxCMD_LINE_SWITCH, _T("h"), _T("help"), _("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
            { wxCMD_LINE_SWITCH, _T("nc"), _T("no-crash-handler"), _("don't use the crash handler (useful for debugging)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
        #if wxUSE_STD_IOSTREAM
            { wxCMD_LINE_SWITCH, _T("cl"), _T("console-logging"),  _("shows application log to the console(if available)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
        #endif
            { wxCMD_LINE_SWITCH, _T("gl"), _T("gui-logging"),  _("enables application log window"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
        #ifdef __WXMSW__
            { wxCMD_LINE_SWITCH, _T("u"), _T("update"),  _("only run update, quit immediately afterwards"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
        #endif
            //{ wxCMD_LINE_OPTION, _T("c"), _T("config-file"),  _("override default choice for config-file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_NEEDS_SEPARATOR },
            { wxCMD_LINE_OPTION, _T("l"), _T("log-verbosity"),  _("overrides default logging verbosity, can be:\n                                0: no log\n                                1: critical errors\n                                2: errors\n                                3: warnings (default)\n                                4: messages\n                                5: function trace"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
            { wxCMD_LINE_NONE }
        };
    #else
        wxCmdLineEntryDesc cmdLineDesc[] =
        {
            { wxCMD_LINE_SWITCH, "h", "help", _("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
            { wxCMD_LINE_SWITCH, "nc", "no-crash-handler", _("don't use the crash handler (useful for debugging)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
        #if wxUSE_STD_IOSTREAM
            { wxCMD_LINE_SWITCH, "cl", "console-logging",  _("shows application log to the console(if available)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
        #endif
            { wxCMD_LINE_SWITCH, "gl", "gui-logging",  _("enables application log window"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
        #ifdef __WXMSW__
            { wxCMD_LINE_SWITCH, "u", "update",  _("only run update, quit immediately afterwards"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
        #endif
            //{ wxCMD_LINE_OPTION, _T("c"), _T("config-file"),  _("override default choice for config-file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_NEEDS_SEPARATOR },
            { wxCMD_LINE_OPTION, "l", "log-verbosity",  _("overrides default logging verbosity, can be:\n                                0: no log\n                                1: critical errors\n                                2: errors\n                                3: warnings (default)\n                                4: messages\n                                5: function trace"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
            { wxCMD_LINE_NONE }
        };
    #endif

    parser.SetDesc( cmdLineDesc );
    parser.SetSwitchChars (_T("-"));
}

//! @brief parses the command line and sets global app options like log verbosity or log target
bool SpringLobbyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
  #if wxUSE_CMDLINE_PARSER
    if ( !parser.Parse(true) )
    {
        m_log_console = parser.Found(_T("console-logging"));
        m_log_window_show = parser.Found(_T("gui-logging"));
        m_crash_handle_disable = parser.Found(_T("no-crash-handler"));

//        Settings::m_user_defined_config = parser.Found( _T("config-file"), &Settings::m_user_defined_config_path );

        if ( !parser.Found(_T("log-verbosity"), &m_log_verbosity ) )
            m_log_verbosity = m_log_window_show ? 3 : 5;

        if ( parser.Found(_T("help")) )
            return false; // not a syntax error, but program should stop if user asked for command line usage
#ifdef __WXMSW__
        if ( parser.Found(_T("update")) ) {
            m_updateing_only = true;
            return true;
        }
#endif
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
    if( sett().IsFirstRun() )
    {
        wxString defaultconfigpath = GetExecutableFolder() + wxFileName::GetPathSeparator() + _T("springlobby.global.conf");
        if (  wxFileName::FileExists( defaultconfigpath ) )
        {
            wxFileInputStream instream( defaultconfigpath );

            if ( instream.IsOk() )
            {
                #ifdef __WXMSW__
                SL_WinConf defaultconf( instream );
                #else
                wxConfig defaultconf( instream );
                #endif
                sett().SetDefaultConfigs( defaultconf );
            }
        }
    }

    SetSettingsStandAlone( false );

    if ( sett().IsFirstRun() && !wxDirExists( wxStandardPaths::Get().GetUserDataDir() ) )
        wxMkdir( wxStandardPaths::Get().GetUserDataDir() );

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
    	if ( sett().GetSettingsVersion() < 3 )
            sett().ConvertOldSpringDirsOptions();
        if ( sett().GetSettingsVersion() < 4 )
        {
            if ( sett().GetTorrentPort() == DEFSETT_SPRING_PORT )
                sett().SetTorrentPort( DEFSETT_SPRING_PORT + 1 );
        }
        if ( sett().GetSettingsVersion() < 5 )
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
        if ( sett().GetSettingsVersion() < 6 )
        {
            sett().ConvertOldServerSettings();
        }
        if ( sett().GetSettingsVersion() < 7 )
        {
            sett().AddChannelJoin( _T("springlobby"), _T("") );
        }
        if ( sett().GetSettingsVersion() < 8 )
        {
             sett().DeleteServer( _T("Backup server") );
             sett().SetServer( _T("Backup server 1"), _T("springbackup1.servegame.com"), 8200 );
             sett().SetServer( _T("Backup server 2"), _T("springbackup2.servegame.org"), 8200 );
             sett().SetServer( _T("Test server"), _T("taspringmaster.servegame.com"), 8300 );
        }
        if ( sett().GetSettingsVersion() < 10 )
        {
            sett().ConvertOldColorSettings();
        }
        if ( sett().GetSettingsVersion() < 11 )
        {
            if( IsUACenabled() )
            {
                usync().ReloadUnitSyncLib();
                if ( usync().IsLoaded() )
                    usync().SetSpringDataPath(_T("")); // UAC is on, fix the spring data path
            }
        }
        if ( sett().GetSettingsVersion() < 12 )
        {
            sett().ConvertOldChannelSettings();
        }
        if ( sett().GetSettingsVersion() < 13 )
        {
            sett().ConvertOldHiglightSettings();
        }
        if ( sett().GetSettingsVersion() < 15 )
        {
            sett().TranslateSavedColumWidths();
        }
    }

    if ( sett().ShouldAddDefaultServerSettings() || ( sett().GetSettingsVersion() < 14 && sett().GetServers().Count() < 2  ) )
        sett().SetDefaultServerSettings();

    if ( sett().ShouldAddDefaultChannelSettings() )
    {
        sett().AddChannelJoin( _T("main"), _T("") );
        sett().AddChannelJoin( _T("newbies"), _T("") );
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
         useractions().SetGroupColor( _("Friends"), wxColor( 0, 0, 255 ) );
    }
}
