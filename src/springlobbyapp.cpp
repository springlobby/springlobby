//
// Class: SpringLobbyApp
//

#include "springlobbyapp.h"
#include "system.h"
#include "ui.h"

IMPLEMENT_APP(SpringLobbyApp)

BEGIN_EVENT_TABLE(SpringLobbyApp, wxApp)

    EVT_TIMER(TIMER_ID, SpringLobbyApp::OnTimer)

END_EVENT_TABLE()


//! @brief Initializes the application.
//!
//! It will open the main window and connect default to server or open the connect window.
bool SpringLobbyApp::OnInit()
{
  cout << "** SpringLobbyApp::OnInit()" << endl;
  
  ui().ShowMainWindow();
  ui().Connect();

  m_timer->Start( TIMER_INTERVAL );
  
  return true;
}


//! @brief Finalizes the application
int SpringLobbyApp::OnExit()
{
  cout << "** SpringLobbyApp::OnExit()" << endl;

  return 0;
}


//! @brief Is called every 1/10 seconds to update statuses
void SpringLobbyApp::OnTimer( wxTimerEvent& event )
{
  if ( sys().serv() != NULL ) {
    sys().serv()->Update();
  }
}

