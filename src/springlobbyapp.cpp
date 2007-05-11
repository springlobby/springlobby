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
#include "mainwindow.h"
#include "chatlist.h"
#include "connectwindow.h"

IMPLEMENT_APP(SpringLobbyApp)


//! @brief Initializes the application.
bool SpringLobbyApp::OnInit()
{
  m_main_win = NULL;
  m_con_win = NULL;

  OpenMainWindow();
  OpenConnectWindow();

  return true;
}


//! @brief Finalizes the application
bool SpringLobbyApp::OnQuit()
{
  return true;
}


//! @brief Returns a reference to the current Settings object
Settings& SpringLobbyApp::Sett()
{
  return m_settings;
}

void SpringLobbyApp::OpenMainWindow()
{
  if ( m_main_win == NULL )
    m_main_win = new MainWindow();
  m_main_win->Show(true);
}

void SpringLobbyApp::OpenConnectWindow()
{
  if ( m_con_win == NULL )
    m_con_win = new ConnectWindow( m_main_win );
  m_con_win->CenterOnParent();
  m_con_win->Show(true);
}

void SpringLobbyApp::Quit()
{
  assert( m_main_win != NULL );
  m_main_win->Close();
}
