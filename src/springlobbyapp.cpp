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
#include <wx/log.h>

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

  debug_func( "" );
  wxInitAllImageHandlers();

  InitDirs();

  m_ui = new Ui();
  wxLogDebug( _T("Ui created") );

  m_ui->ShowMainWindow();

  if ( sett().IsFirstRun() ) {
    wxLogMessage(_("Hi ") + wxGetUserName() + _(",\nLooks like this is the first time you use SpringLobby. I have guessed a configuration that I think will work for you but you should review it, ecpecially the Spring configuration. \n\nWhen you are done you can go to the File menu, connect to a server, and enjoy a nice game of Spring :)"), _("Welcome"),
      wxOK | wxICON_INFORMATION, &m_ui->mw() );
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
  debug_func( "" );

  m_timer->Stop();
  delete m_ui;

  usync()->FreeUnitSyncLib();

  delete usync();

  return 0;
}


void SpringLobbyApp::OnFatalException()
{

#if wxUSE_STACKWALKER

  wxString DebugInfo = _T("\n-------- Begin StackTrace --------\n");

  DebugInfo += _T("StackTraceID: ") + stacktrace().GetStackTraceHash() + _T("\n");

  stacktrace().WalkFromException();
  DebugInfo += stacktrace().GetStackTrace();

  DebugInfo += _T("-------- End StackTrace --------");

  wxLogError( DebugInfo );
#else
  wxLogError( _T("Stacktrace not possible, please enable wxStackWalker") );
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

