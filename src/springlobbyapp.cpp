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

#define TIMER_ID 101
#define TIMER_INTERVAL 100


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

    if ( sett().IsFirstRun() && !wxDirExists( wxStandardPaths::Get().GetUserDataDir() ) ) wxMkdir( wxStandardPaths::Get().GetUserDataDir() );

    if ( (sett().GetCacheVersion() < CACHE_VERSION) && !sett().IsFirstRun() )
    {
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

    ui().ReloadUnitSync(); /// first time load of unitsync
    ui().ShowMainWindow();

    if ( !sett().IsFirstRun() && sett().IsPortableMode() && usync().IsLoaded()) usync().SetSpringDataPath( sett().GetSpringDir() ); /// update spring's current working dir trough unitsync

    if ( !sett().IsFirstRun() && !wxDirExists( sett().GetSpringDir() ) ) wxMkdir( sett().GetSpringDir() );

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
        wxString destFilename = sett().GetSpringDir() + ( sett().GetSpringDir().EndsWith( sep ) ? _T("") : sep )
                + _T("base") + sep + _T("base-ota-content.zip");
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
                customMessageBox(SL_MAIN_ICON, _("Do you want to download OTA conent?\n"
                                                 "You need this to be able to play TA based mods.\n"
                                                 "You need to own a copy of Total Annihilation do legally download it."),_("Download OTA content?"),wxYES_NO) == wxYES )
        {
            m_otadownloader = new HttpDownloader( url, destFilename );
        }

        customMessageBoxNoModal(SL_MAIN_ICON, _("By default SpringLobby reports some statistics.?\n"
                                                 "You can disable that on options tab --> General."),_("Notice"),wxOK );
        ui().mw().ReloadSpringPathFromConfig();
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

  sett().SetSettingsVersion(); /// bump settings version number


  //TestTDFParser();

    return true;
}


//! @brief Finalizes the application
int SpringLobbyApp::OnExit()
{
    wxLogDebugFunc( _T("") );

    if ( m_otadownloader != 0 )
        delete m_otadownloader ;

  #ifndef NO_TORRENT_SYSTEM
  if( sett().GetTorrentSystemAutoStartMode() == 1 ) torrent().DisconnectToP2PSystem();
  #endif

  m_timer->Stop();

  sett().SaveSettings(); /// to make sure that cache path gets saved before destroying unitsync

  usync().FreeUnitSyncLib();

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
    if ( !wxFileName::DirExists( wxFileName::GetHomeDir() + sep +_T("spring") ) )
    {
        wxArrayString choices;
        choices.Add( _("Create a spring folder in the home directory (reccomended)") );
        choices.Add( _("Create a folder in a custom path (you'll get prompted for the path)") );
        choices.Add( _("I have already a SpringData folder, i want to browse manually for it") );
        choices.Add( _("Do nothing (use this only if you know what you're doing)") );

        int result = wxGetSingleChoiceIndex(
                         _("Looks like you don't have yet a user SpringData folder structure\nWhat would you like to do? (leave default choice if you don't know what is this for)"),
                         _("First time wizard"),
                         choices );

        wxString dir;
        bool createdirs = true;
        if ( result == 2 ) createdirs = false;
        else if ( result == 3 ) return;

        if ( result == 0 ) dir = wxFileName::GetHomeDir() + sep +_T("spring");
        else if ( result == 1 || result == 2 ) dir = wxDirSelector( _("Choose a folder"), wxFileName::GetHomeDir() +  sep + _T("spring") );

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
                if ( dir.IsEmpty() ) dir = wxFileName::GetHomeDir() + sep + _T("spring");
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
        sett().SetSpringDir(dir);
    }
#endif
#endif
}

