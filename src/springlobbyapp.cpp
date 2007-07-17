//
// Class: SpringLobbyApp
//

#include <wx/intl.h>
#include <wx/msgdlg.h>

#include "springlobbyapp.h"
#include "mainwindow.h"
#include "settings.h"
#include "utils.h"


IMPLEMENT_APP(SpringLobbyApp)

BEGIN_EVENT_TABLE(SpringLobbyApp, wxApp)

    EVT_TIMER(TIMER_ID, SpringLobbyApp::OnTimer)

END_EVENT_TABLE()


//! @brief Initializes the application.
//!
//! It will open the main window and connect default to server or open the connect window.
bool SpringLobbyApp::OnInit()
{
  debug_func( "" );

  m_ui = new Ui();

  m_ui->ShowMainWindow();

  if ( sett().IsFirstRun() ) {
    wxMessageBox(_("Hi ") + wxGetUserName() + _(",\nLooks like this is the first time you use SpringLobby. I have guessed a configuration that I think will work for you but you should review it, expecially the Spring configuration. \n\nWhen you are done you can go to the File menu, connect to a server, and enjoy a nice game of Spring :)"), _("Welcome"),
      wxOK | wxICON_INFORMATION, &m_ui->mw() );
    m_ui->mw().ShowConfigure();
  } else {
    usync().LoadUnitsyncLib();
    if ( usync().IsLoaded() ) m_ui->Connect();
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

  return 0;
}


//! @brief Is called every 1/10 seconds to update statuses
void SpringLobbyApp::OnTimer( wxTimerEvent& event )
{
  m_ui->OnUpdate();
}

