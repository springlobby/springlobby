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

#include "springlobbyapp.h"
#include "mainwindow.h"
#include "settings.h"
#include "crashreport.h"
#include "utils.h"
#include "ui.h"
#include "iunitsync.h"
#include "channel.h"
#ifndef NO_TORRENT_SYSTEM
#include "torrentwrapper.h"
#endif

#define TIMER_ID 101
#define TIMER_INTERVAL 100


IMPLEMENT_APP(SpringLobbyApp)

BEGIN_EVENT_TABLE(SpringLobbyApp, wxApp)

    EVT_TIMER(TIMER_ID, SpringLobbyApp::OnTimer)

END_EVENT_TABLE()

SpringLobbyApp::SpringLobbyApp()
{
  m_timer = new wxTimer(this, TIMER_ID);
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

#if wxUSE_ON_FATAL_EXCEPTION
  wxHandleFatalExceptions( true );
#endif

  //initialize all loggers
  InitializeLoggingTargets();

  wxLogDebugFunc( _T("") );
  wxInitAllImageHandlers();

  //TODO needed?
  wxImage::AddHandler(new wxPNGHandler);

  m_locale = new wxLocale( );
  m_locale->Init();
  m_locale->AddCatalog( _T("springlobby") );

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

  InitDirs();

  m_ui = new Ui();
  wxLogMessage( _T("Ui created") );

  m_ui->ShowMainWindow();


  if ( sett().IsFirstRun() ) {
    #ifdef __WXMSW__
      sett().SetOldSpringLaunchMethod( true );
    #endif
    sett().AddChannelJoin( _T("newbies"), _T("") );
    wxLogMessage( _T("first time startup"));
    wxMessageBox(_("Hi ") + wxGetUserName() + _(",\nIt looks like this is your first time using SpringLobby. I have guessed a configuration that I think will work for you but you should review it, especially the Spring configuration. \n\nWhen you are done you can go to the File menu, connect to a server, and enjoy a nice game of Spring :)"), _("Welcome"),
      wxOK | wxICON_INFORMATION, &m_ui->mw() );
    #ifdef HAVE_WX26
    wxMessageBox(_("You're using a wxwidgets library of the 2.6.x series\n battle filtering, advanced gui and joining/hosting games using nat traversal\n won't be available"), _("Missing Functionality"), wxICON_INFORMATION, &m_ui->mw() );
    #endif
    SetupUserFolders();
    m_ui->mw().ShowConfigure();
  } else {
    m_ui->Connect();
  }

  //m_ui->ReloadUnitSync();

  #ifndef NO_TORRENT_SYSTEM
  if( sett().GetTorrentSystemAutoStartMode() == 1 ) torrent()->ConnectToP2PSystem();
  #endif

  m_timer->Start( TIMER_INTERVAL );

  return true;
}


//! @brief Finalizes the application
int SpringLobbyApp::OnExit()
{
  wxLogDebugFunc( _T("") );

  #ifndef NO_TORRENT_SYSTEM
  if( sett().GetTorrentSystemAutoStartMode() == 1 ) torrent()->DisconnectToP2PSystem();
  #endif

  m_timer->Stop();
  delete m_ui;

  usync()->FreeUnitSyncLib();

  delete usync();

  return 0;
}

//! @brief is called when the app crashes
void SpringLobbyApp::OnFatalException()
{
  #if wxUSE_DEBUGREPORT && defined(HAVE_WX28)
  crashreport().GenerateReport(wxDebugReport::Context_Exception);
  #else
  wxMessageBox( _("The application has generated a fatal error and will be terminated\nGenerating a bug report is not possible\n\nplease get a wxWidgets library that supports wxUSE_DEBUGREPORT"),_("Critical error"), wxICON_ERROR | wxOK );
  #endif
}


//! @brief Is called every 1/10 seconds to update statuses
void SpringLobbyApp::OnTimer( wxTimerEvent& event )
{
  m_ui->OnUpdate( event.GetInterval() );
}


void SpringLobbyApp::InitDirs()
{
  wxString path = wxStandardPaths::Get().GetUserDataDir();
  if ( !wxDirExists( path ) ) wxMkdir( path );
  path += wxFILE_SEP_PATH; path += _T("cache"); path += wxFILE_SEP_PATH;
  if ( !wxDirExists( path ) ) wxMkdir( path );
}


void SpringLobbyApp::SetupUserFolders()
{
  #ifdef __WXGTK__
    #ifndef HAVE_WX26
     if ( !wxFileName::DirExists( wxFileName::GetHomeDir() + _("/.spring") ) )
     {
       wxArrayString choices;
       choices.Add( _("Create a .spring folder in the home directory (reccomended)") );
       choices.Add( _("Create a folder in a custom path (you'll get prompted for the path)") );
       choices.Add( _("I have already a SpringData folder, i want to browse manually for it") );
       choices.Add( _("Do nothing (use this only if you know what you're doing)") );

       int result = wxGetSingleChoiceIndex(
          _("Looks like you don't have yet a user SpringData folder structure\nWhat would you like to do? (leave default choice if you don't know what is this for)"),
          _("Chose an action"),
          choices );

       wxString dir;
       bool createdirs = true;
       if ( result == 2 ) createdirs = false;
       else if ( result == 3 ) return;

       if ( result == 0 ) dir = wxFileName::GetHomeDir() + _("/.spring");
       else if ( result == 1 || result == 2 ) dir = wxDirSelector( _("Choose a folder"), wxFileName::GetHomeDir() + _T("/.spring") );

       if ( createdirs )
       {
         if ( dir.IsEmpty() || ( !wxFileName::Mkdir( dir ) || ( !wxFileName::Mkdir( dir + _T("/mods") ) || !wxFileName::Mkdir( dir + _T("/maps") ) || !wxFileName::Mkdir( dir + _T("/base") ) || !wxFileName::Mkdir( dir + _T("/demos") ) || !wxFileName::Mkdir( dir + _T("/screenshots")  ) ) ) )
         {
           if ( dir.IsEmpty() ) dir = wxFileName::GetHomeDir() + _("/.spring");
          wxMessageBox( _("Something went wrong when creating the directories\nPlease create manually the following folders:") + wxString(_T("\n")) + dir +  _T("\n") + dir + _T("/mods\n") + dir + _T("/maps\n") + dir + _T("/base\n") );
         }
       }

     }
    #endif
  #endif
}

