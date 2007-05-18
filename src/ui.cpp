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
// Class: Ui
//
#include "ui.h"


Ui::Ui()
{
  cout << "** Ui::Ui()" << endl;
  m_main_win = new MainWindow();
}


Ui::~Ui()
{
  cout << "** Ui::~Ui()" << endl;
}


Ui& ui()
{
  static Ui m_ui;
  return m_ui;
}

MainWindow& Ui::mw()
{
  assert( m_main_win != NULL );
  return *m_main_win;
}

//! @brief Shows the main window on screen
void Ui::ShowMainWindow()
{
  assert( m_main_win != NULL );
  m_main_win->Show(true);
}


//! @brief Show the connect window on screen
//!
//! @note It will create the ConnectWindow if not allready created
void Ui::ShowConnectWindow()
{
  if ( m_con_win == NULL ) {
    assert( m_main_win != NULL );
    m_con_win = new ConnectWindow( m_main_win );
  }
  m_con_win->CenterOnParent();
  m_con_win->Show(true);
}


//! @brief Connects to default server or opens the ConnectWindow
//!
//! @todo Fix Auto Connect
void Ui::Connect()
{
  ShowConnectWindow();
}

//! @brief Quits the entire application
void Ui::Quit()
{
  assert( m_main_win != NULL );
  m_main_win->Close();
}

bool Ui::Ask( wxString heading, wxString question )
{
  wxMessageDialog ask_dlg(NULL, question, heading, wxOK | wxCANCEL );
  return ( ask_dlg.ShowModal() == wxID_OK );
}
