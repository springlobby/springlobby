/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: SpringLobbyApp
//

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
#include <wx/fs_zip.h> //filesystem zip handler
#include <wx/socket.h>


#include "springlobbyapp.h"
#include "mainwindow.h"
#include "settings.h"
#include "crashreport.h"
#include "utils.h"
#include "ui.h"
#include "iunitsync.h"
#include "channel.h"
#include "httpdownloader.h"
#include "settings++/custom_dialogs.h"
#ifndef NO_TORRENT_SYSTEM
#include "torrentwrapper.h"
#endif
#include "updater/updater.h"
#include "unitsyncthread.h"
#include "replay/replaytab.h"
#include "globalsmanager.h"

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
{
    m_timer = new wxTimer(this, TIMER_ID);
    m_locale = NULL;
    m_otadownloader = NULL;
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


  if ( !ParseCmdLine() ) return false; ///command line parsing failed, close the app

#if wxUSE_ON_FATAL_EXCEPTION
  if (!m_crash_handle_disable) wxHandleFatalExceptions( true );

#endif


  //initialize all loggers
  InitializeLoggingTargets( m_log_console, m_log_window_show, !m_crash_handle_disable, m_log_verbosity );

    wxSocketBase::Initialize();


    wxLogDebugFunc( _T("") );
    wxInitAllImageHandlers();


     //TODO needed?
    wxImage::AddHandler(new wxPNGHandler);
    wxFileSystem::AddHandler(new wxZipFSHandler);

    m_locale = new wxLocale( );
    m_locale->Init();
#ifdef __WXMSW__
    wxString path = wxStandardPaths::Get().GetExecutablePath().BeforeLast( wxFileName::GetPathSeparator() );
    m_locale->AddCatalogLookupPathPrefix(path +  wxFileName::GetPathSeparator() + _T("locale") );
#endif
    m_locale->AddCatalog( _T("springlobby") );

    if ( sett().IsFirstRun() && !wxDirExists( wxStandardPaths::Get().GetUserDataDir() ) ) wxMkdir( wxStandardPaths::Get().GetUserDataDir() );

    if ( (sett().GetCacheVersion() < CACHE_VERSION) && !sett().IsFirstRun() )
    {
        sett().SetMapCachingThreadProgress( 0 ); /// reset map cache thread
        sett().SetModCachingThreadProgress( 0 ); /// reset mod cache thread
        CacheThread().LoadSettingsFromFile();
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

    if ( !sett().IsFirstRun() && ( sett().GetSettingsVersion() < 3 ) ) sett().ConvertOldSpringDirsOptions();

    ui().ReloadUnitSync(); /// first time load of unitsync
    ui().ShowMainWindow();

    if ( sett().IsFirstRun() )
    {
#ifdef __WXMSW__
        sett().SetOldSpringLaunchMethod( true );
#endif
        sett().AddChannelJoin( _T("newbies"), _T("") );
        wxLogMessage( _T("first time startup"));
        wxMessageBox(_("Hi ") + wxGetUserName() + _(",\nIt looks like this is your first time using SpringLobby. I have guessed a configuration that I think will work for you but you should review it, especially the Spring configuration. \n\nWhen you are done you can go to the File menu, connect to a server, and enjoy a nice game of Spring :)"), _("Welcome"),
                     wxOK | wxICON_INFORMATION, &ui().mw() );
#ifdef HAVE_WX26
        wxMessageBox(_("You're using a wxwidgets library of the 2.6.x series\n battle filtering, advanced gui and joining/hosting games using nat traversal\n won't be available"), _("Missing Functionality"), wxICON_INFORMATION, &ui().mw() );
#endif

        SetupUserFolders();

        if ( !wxDirExists( wxStandardPaths::Get().GetUserDataDir() ) ) wxMkdir( wxStandardPaths::Get().GetUserDataDir() );
        wxString sep ( wxFileName::GetPathSeparator() );
        //! ask for downloading ota content if archive not found, start downloader in background
        wxString url= _T("ipxserver.dyndns.org/games/spring/mods/xta/base-ota-content.zip");
        wxString destFilename = sett().GetCurrentUsedDataDir() + sep + _T("base") + sep + _T("base-ota-content.zip");
        bool contentExists = false;
        if ( usync().IsLoaded() )
        {
            contentExists = usync().FileExists(_T("base/otacontent.sdz")) && usync().FileExists(_T("base/tacontent_v2.sdz")) && usync().FileExists(_T("base/tatextures_v062.sdz"));
        }
        else
        {
            contentExists = wxFile::Exists(destFilename);
        }

        if ( !contentExists &&
                customMessageBox(SL_MAIN_ICON, _("Do you want to download OTA content?\n"
                                                 "You need this to be able to play TA based mods.\n"
                                                 "You need to own a copy of Total Annihilation do legally download it."),_("Download OTA content?"),wxYES_NO) == wxYES )
        {
            m_otadownloader = new HttpDownloader( url, destFilename );
        }

        customMessageBoxNoModal(SL_MAIN_ICON, _("By default SpringLobby reports some statistics.?\n"
                                                 "You can disable that on options tab --> General."),_("Notice"),wxOK );
        ui().mw().ShowConfigure();
    }
    else
    {
        ui().Connect();
    }

  #ifndef NO_TORRENT_SYSTEM
  if( sett().GetTorrentSystemAutoStartMode() == 1 ) torrent().ConnectToP2PSystem();
  #endif

  m_timer->Start( TIMER_INTERVAL );

//  #ifdef __WXMSW__
//  if ( sett().GetAutoUpdate() )Updater().CheckForUpdates();
//  #endif


  ui().mw().GetReplayTab().AddAllReplays();

    return true;
}


//! @brief Finalizes the application
int SpringLobbyApp::OnExit()
{
    wxLogDebugFunc( _T("") );

    if ( m_otadownloader != 0 )
        delete m_otadownloader ;

  #ifndef NO_TORRENT_SYSTEM
  //if( sett().GetTorrentSystemAutoStartMode() == 1 )
  torrent().DisconnectFromP2PSystem();/// Cant hurt to disconnect unconditionally.
  #endif

  m_timer->Stop();

  sett().SaveSettings(); /// to make sure that cache path gets saved before destroying unitsync

  usync().FreeUnitSyncLib();

  DestroyGlobals();

  return 0;
}

//! @brief is called when the app crashes
void SpringLobbyApp::OnFatalException()
{
#if wxUSE_DEBUGREPORT && defined(HAVE_WX28) && defined(ENABLE_DEBUG_REPORT)
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


void SpringLobbyApp::SetupUserFolders()
{
#ifdef __WXGTK__
#ifndef HAVE_WX26
      wxString sep = wxFileName::GetPathSeparator();
      wxString defaultdir = wxFileName::GetHomeDir() + sep +_T("spring");

      wxArrayString choices;
      choices.Add( _("Do nothing") );
      choices.Add( _("Create a folder in a custom path (you'll get prompted for the path)") );
      choices.Add( _("I have already a SpringData folder, i want to browse manually for it") );

      int result = wxGetSingleChoiceIndex(
                       _("Looks like you don't have yet a user SpringData folder structure\nWhat would you like to do? (leave default choice if you don't know what is this for)"),
                       _("First time wizard"),
                       choices );

      wxString dir;
      bool createdirs = true;
      if ( result == 2 ) createdirs = false;
      else if ( result == 0 ) return;

      if ( result == 1 || result == 2 ) dir = wxDirSelector( _("Choose a folder"), defaultdir );

      if ( createdirs )
      {
          if ( dir.IsEmpty() ||
           ( !wxFileName::Mkdir( dir ) ||
              ( !wxFileName::Mkdir( dir + sep + _T("mods") ) ||
                !wxFileName::Mkdir( dir + sep + _T("maps") ) ||
                !wxFileName::Mkdir( dir + sep + _T("base") ) ||
                !wxFileName::Mkdir( dir + sep + _T("demos") ) ||
                !wxFileName::Mkdir( dir + sep + _T("screenshots")  ) )
              )
            )
          {
              if ( dir.IsEmpty() ) dir = defaultdir;
              wxMessageBox( _("Something went wrong when creating the directories\nPlease create manually the following folders:") + wxString(_T("\n")) + dir +  _T("\n") + dir + sep + _T("mods\n") + dir + sep + _T("maps\n") + dir + sep + _T("base\n") );
              return;
          }
          else
          {
            #ifdef __WXGTK__
            if ( wxFileName::FileExists( _T("/usr/share/games/spring/uikeys.txt") ) ) /// this hardcoded path is a bit dumb but it's too early in the code to do proper spring path detection
            {
              wxCopyFile( _T("/usr/share/games/spring/uikeys.txt"), dir + sep + _T("uikeys.txt"), false );
            }
            #endif
          }
      }
      usync().SetSpringDataPath(dir);
#endif
#endif
}


//! @brief parses the command line and sets global app options like log verbosity or log target
bool SpringLobbyApp::ParseCmdLine()
{
  #if wxUSE_CMDLINE_PARSER

    wxCmdLineEntryDesc cmdLineDesc[] =
    {
      { wxCMD_LINE_SWITCH, _T("h"), _T("help"), _("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
      { wxCMD_LINE_SWITCH, _T("nc"), _T("no-crash-handler"), _("don't use the crash handler (useful for debugging)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
      { wxCMD_LINE_SWITCH, _T("cl"), _T("console-logging"),  _("shows application log to the console(if available)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
      { wxCMD_LINE_SWITCH, _T("gl"), _T("gui-logging"),  _("enables application log window"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
      { wxCMD_LINE_OPTION, _T("l"), _T("log-verbosity"),  _("overrides default logging verbosity, can be:\n                                0: no log\n                                1: critical errors\n                                2: errors\n                                3: warnings (default)\n                                4: messages\n                                5: function trace"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
    };

    wxCmdLineParser parser( cmdLineDesc, argc, argv );

    if ( !parser.Parse(true) )
    {
      m_log_console = parser.Found(_T("console-logging"));
      m_log_window_show = parser.Found(_T("gui-logging"));
      m_crash_handle_disable = parser.Found(_T("no-crash-handler"));
      if ( !parser.Found(_T("log-verbosity"), &m_log_verbosity ) ) m_log_verbosity = 3;
      if ( parser.Found(_T("help")) ) return false; // not a syntax error, but program should stop if user asked for command line usage
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
