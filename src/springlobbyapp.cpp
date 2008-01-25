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

#include "springlobbyapp.h"
#include "mainwindow.h"
#include "settings.h"
#include "stacktrace.h"
#include "utils.h"
#include "ui.h"
#include "iunitsync.h"
#include "channel.h"

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
    sett().AddChannelJoin( "newbies", "" );
    wxLogMessage( _T("first time startup"));
    wxMessageBox(_("Hi ") + wxGetUserName() + _(",\nLooks like this is the first time you use SpringLobby. I have guessed a configuration that I think will work for you but you should review it, expecially the Spring configuration. \n\nWhen you are done you can go to the File menu, connect to a server, and enjoy a nice game of Spring :)"), _("Welcome"),
      wxOK | wxICON_INFORMATION, &m_ui->mw() );
    #ifdef HAVE_WX26
    wxMessageBox(_("You're using a wxwidgets library of the 2.6.x series\n battle filtering, advanced gui and joining/hosting games using nat traversal\n won't be available"), _("Missing Functionality"), wxICON_INFORMATION, &m_ui->mw() );
    #endif
    m_ui->mw().ShowConfigure();
  } else {
    m_ui->Connect();
  }

  m_ui->ReloadUnitSync();

  m_timer->Start( TIMER_INTERVAL );

  return true;
}


//! @brief Finalizes the application
int SpringLobbyApp::OnExit()
{
  wxLogDebugFunc( _T("") );

  m_timer->Stop();
  delete m_ui;

  usync()->FreeUnitSyncLib();

  delete usync();

  return 0;
}


void SpringLobbyApp::OnFatalException()
{

#if wxUSE_STACKWALKER

  wxMessageBox( _("SpringLobby has generated a fatal error and will be terminated\nA stacktrace will be dumped to the application's console output"), _("Critical error"), wxICON_ERROR  );

  wxLogError( _T("uncaught exception") );
  wxString DebugInfo = _T("\n-------- Begin StackTrace --------\n");

  DebugInfo += _T("StackTraceID: ") + stacktrace().GetStackTraceHash() + _T("\n");

  stacktrace().WalkFromException();
  DebugInfo += stacktrace().GetStackTrace();

  DebugInfo += _T("-------- End StackTrace --------");

  wxLogMessage( DebugInfo );
#else
  wxMessageBox( _("SpringLobby has generated a fatal error and will be terminated\nGenerating a stacktrace is not possible\n\nplease enable wxStackWalker"), _("Critical error"), wxICON_ERROR  );
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

