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
  std::cout << "** SpringLobbyApp::OnInit()" << std::endl;
  
  m_ui = new Ui();
  
  m_ui->ShowMainWindow();
  m_ui->Connect();

  m_timer->Start( TIMER_INTERVAL );
  
  return true;
}


//! @brief Finalizes the application
int SpringLobbyApp::OnExit()
{
  std::cout << "** SpringLobbyApp::OnExit()" << std::endl;
  
  m_timer->Stop();
  delete m_ui;
  
  return 0;
}


//! @brief Is called every 1/10 seconds to update statuses
void SpringLobbyApp::OnTimer( wxTimerEvent& event )
{
  m_ui->OnUpdate();
}

