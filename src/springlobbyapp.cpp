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
#include <wx/choicdlg.h>
#include <wx/filename.h>
#include <wx/dirdlg.h>
#include <wx/tooltip.h>
#include <wx/file.h>
#include <wx/fs_zip.h> //filesystem zip handler
#include <wx/socket.h>
#ifdef __WXMSW__
	#include <wx/msw/registry.h>
#endif
#include <wx/utils.h>

#include "springlobbyapp.h"
#include "mainwindow.h"
#include "settings.h"
#include "crashreport.h"
#include "utils.h"
#include "ui.h"
#include "iunitsync.h"
#include "channel/channel.h"
#include "httpdownloader.h"
#include "settings++/custom_dialogs.h"
#include "settings++/se_utils.h"
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


#if wxUSE_ON_FATAL_EXCEPTION && wxUSE_DEBUGREPORT && defined(HAVE_WX28) && defined(ENABLE_DEBUG_REPORT)
    wxHandleFatalExceptions( true );
#endif

    //initialize all loggers
    InitializeLoggingTargets();
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

    SetSettingsStandAlone( false );

    if ( sett().IsFirstRun() && !wxDirExists( wxStandardPaths::Get().GetUserDataDir() ) ) wxMkdir( wxStandardPaths::Get().GetUserDataDir() );

    if ( (sett().GetCacheVersion() < CACHE_VERSION) && !sett().IsFirstRun() )
    {
        sett().SetMapCachingThreadProgress( 0 ); // reset map cache thread
        sett().SetModCachingThreadProgress( 0 ); // reset mod cache thread
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
    if ( !sett().IsFirstRun() && ( sett().GetSettingsVersion() < 4 ) )
    {
    	if ( sett().GetTorrentPort() == DEFSETT_SPRING_PORT ) sett().SetTorrentPort( DEFSETT_SPRING_PORT + 1 );
    }

    ui().ReloadUnitSync(); // first time load of unitsync
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
        // ask for downloading ota content if archive not found, start downloader in background
	if ( !wxDirExists( sett().GetCurrentUsedDataDir() + sep + _T("base") ) ) wxMkdir( sett().GetCurrentUsedDataDir() + sep + _T("base") );
        wxString url= _T("ipxserver.dyndns.org/games/spring/mods/xta/base-ota-content.zip");
        wxString destFilename = sett().GetCurrentUsedDataDir() + sep + _T("base") + sep + _T("base-ota-content.zip");
        bool contentExists = usync().FileExists(_T("base/otacontent.sdz")) && usync().FileExists(_T("base/tacontent_v2.sdz")) && usync().FileExists(_T("base/tatextures_v062.sdz"));

        if ( !contentExists &&
                customMessageBox(SL_MAIN_ICON, _("Do you want to download OTA content?\n"
                                                 "You need this to be able to play TA based mods.\n"
                                                 "You need to own a copy of Total Annihilation do legally download it."),_("Download OTA content?"),wxYES_NO) == wxYES )
        {
            m_otadownloader = new HttpDownloader( url, destFilename );
        }

        customMessageBoxNoModal(SL_MAIN_ICON, _("By default SpringLobby reports some statistics.\n"
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
#ifndef HAVE_WX26
      wxString sep = wxFileName::GetPathSeparator();
      wxString defaultdir = wxFileName::GetHomeDir() + sep +_T("spring");
      wxArrayString choices;
#ifdef __WXMSW__
      wxRegKey UACpath( _T("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System") ); // check if UAC is on, skip dialog if not
      if( !UACpath.Exists() ) return;
      long value;
      if( !UACpath.QueryValue( _T("EnableLUA"), &value ) ) return; // reg key not present -> not vista -> dialog useless
			if( value == 0 ) return; // UAC is off -> skip dialog

			int createdefault = choices.Add( _("Create a spring directory in my documents folder") );
#endif

      int donothing = choices.Add( _("Do nothing") );
      int createcustompath = choices.Add( _("Create a folder in a custom path (you'll get prompted for the path)") );
      int choseexisting = choices.Add( _("I have already a SpringData folder, i want to browse manually for it") );

      int result = wxGetSingleChoiceIndex(
                       _("Looks like you don't have yet a user SpringData folder structure\nWhat would you like to do? (leave default choice if you don't know what this is for)"),
                       _("First time wizard"),
                       choices );

      wxString dir;
      bool createdirs = true;
      if ( result == choseexisting ) createdirs = false;
      else if ( result == donothing ) return;
      #ifdef __WXMSW__
      else if ( result == createdefault ) dir = defaultdir;
      #endif

      if ( result == createcustompath || result == choseexisting ) dir = wxDirSelector( _("Choose a folder"), defaultdir );

      if ( createdirs )
      {
          if ( dir.IsEmpty() ||
           ( !wxFileName::Mkdir( dir, 0775 ) ||
              ( !wxFileName::Mkdir( dir + sep + _T("mods"), 0775 ) ||
                !wxFileName::Mkdir( dir + sep + _T("maps"), 0775 ) ||
                !wxFileName::Mkdir( dir + sep + _T("base"), 0775 ) ||
                !wxFileName::Mkdir( dir + sep + _T("demos"), 0775 ) ||
                !wxFileName::Mkdir( dir + sep + _T("screenshots"), 0775  ) )
              )
            )
          {
              if ( dir.IsEmpty() ) dir = defaultdir;
              wxMessageBox( _("Something went wrong when creating the directories\nPlease create manually the following folders:") + wxString(_T("\n")) + dir +  _T("\n") + dir + sep + _T("mods\n") + dir + sep + _T("maps\n") + dir + sep + _T("base\n") );
              return;
          }
          else
          {
          	wxPathList pl;
						pl.AddEnvList( _T("%ProgramFiles%") );
						pl.AddEnvList( _T("XDG_DATA_DIRS") );
						pl = sett().GetAdditionalSearchPaths( pl );
          	wxString uikeyslocation = pl.FindValidPath( _T("uikeys.txt") );
            if ( !uikeyslocation.IsEmpty() )
            {
              wxCopyFile( uikeyslocation, dir + sep + _T("uikeys.txt"), false );
            }
          }
      }
      usync().SetSpringDataPath(dir);
#endif
}

