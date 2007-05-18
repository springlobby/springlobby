/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


//
// Class: SpringLobbyApp
//
// Created on: Fri May 11 19:19:23 2007
//

#include "springlobbyapp.h"
#include "system.h"
#include "ui.h"

IMPLEMENT_APP(SpringLobbyApp)

BEGIN_EVENT_TABLE(SpringLobbyApp, wxApp)

    EVT_TIMER(TIMER_ID, SpringLobbyApp::OnTimer)

END_EVENT_TABLE()


//! @brief Constructor
SpringLobbyApp::SpringLobbyApp()
{
  m_timer = new wxTimer(this, TIMER_ID);
}


//! @brief Desstructor
SpringLobbyApp::~SpringLobbyApp()
{
  
}


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

