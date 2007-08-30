/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: SpringLobbyApp
//

#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/timer.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>

#include "springlobbyapp.h"
#include "mainwindow.h"
#include "settings.h"
#include "utils.h"
#include "ui.h"
#include "iunitsync.h"

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
  debug_func( "" );

  InitDirs();

  usync()->LoadUnitSyncLib();
  m_ui = new Ui();

  m_ui->ShowMainWindow();

  if ( sett().IsFirstRun() ) {
    wxMessageBox(_("Hi ") + wxGetUserName() + _(",\nLooks like this is the first time you use SpringLobby. I have guessed\na configuration that I think will work for you but you should review it,\necpecially the Spring configuration. \n\nWhen you are done you can go to the File menu, connect\nto a server, and enjoy a nice game of Spring :)"), _("Welcome"),
      wxOK | wxICON_INFORMATION, &m_ui->mw() );
    m_ui->mw().ShowConfigure();
  } else {
    if ( usync()->IsLoaded() ) m_ui->Connect();
    else m_ui->mw().ShowConfigure();
  }

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

  return 0;
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

