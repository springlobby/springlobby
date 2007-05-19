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
#include "utils.h"

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


//! @brief Called when connected to a server
//!
//! @todo Display in servertab
void Ui::OnConnected( string server_name, string server_ver, bool supported )
{
  
}


//! @brief Called when client has joined a channel
//!
//! @todo Check if a pannel allready exists for this channel
void Ui::OnJoinedChannelSuccessful( Channel& chan )
{
  cout << "** Ui::OnJoinedChannelSuccessful()" << endl;
  UiChannelData* chandata = new UiChannelData();
  chandata->panel = NULL;
  
  chan.SetUserData( (void*)chandata );
  mw().OpenChannelChat( chan );
}


//! @brief Called when something is said in a channel
void Ui::OnChannelSaid( Channel& channel, User& user, const string& message )
{
  cout << "** Ui::OnChannelSaid()" << endl;
  UiChannelData* ud = (UiChannelData*)channel.GetUserData();
  assert( ud != NULL );
  if ( ud->panel == NULL ) {
    cout << "   !! ud->panel NULL" << endl;
    return;
  }
  ud->panel->Said( WX_STRING(user.GetNick()), WX_STRING( message ) );
}


void Ui::OnChannelDidAction( Channel& channel , User& user, const string& action )
{
  cout << "** Ui::OnChannelDidAction()" << endl;
  UiChannelData* ud = (UiChannelData*)channel.GetUserData();
  assert( ud != NULL );
  if ( ud->panel == NULL ) {
    cout << "   !! ud->panel NULL" << endl;
    return;
  }
  ud->panel->DidAction( WX_STRING(user.GetNick()), WX_STRING( action ) );  
}


//! @brief Called when client is leaving a channel
//!
//! @todo Tell ChatPanel the channel is no longer joined
void Ui::OnLeaveChannel( Channel& channel )
{
  assert( channel.GetUserData() != NULL );
  assert( ((UiChannelData*)channel.GetUserData())->panel != NULL );
  
  ((UiChannelData*)channel.GetUserData())->panel->SetChannel( NULL );
  
  delete (UiChannelData*)channel.GetUserData();
  channel.SetUserData( NULL );
  
}


void Ui::OnUserJoinedChannel( Channel& chan, User& user )
{
  //cout << "** Ui::OnUserJoinedChannel()" << endl;
  UiChannelData* ud = (UiChannelData*)chan.GetUserData();
  assert( ud != NULL );
  if ( ud->panel == NULL ) {
    cout << "   !! ud->panel NULL" << endl;
    return;
  }
  ud->panel->Joined( WX_STRING(user.GetNick()) );  
}


void Ui::OnUserLeftChannel( Channel& chan, User& user, const string& reason )
{
  cout << "** Ui::OnUserLeftChannel()" << endl;
  UiChannelData* ud = (UiChannelData*)chan.GetUserData();
  assert( ud != NULL );
  if ( ud->panel == NULL ) {
    cout << "   !! ud->panel NULL" << endl;
    return;
  }
  ud->panel->Parted( WX_STRING(user.GetNick()), WX_STRING(reason) );  
}


void Ui::OnChannelTopic( Channel& channel , User& user, const string& topic )
{
  cout << "** Ui::OnChannelTopic()" << endl;
  UiChannelData* ud = (UiChannelData*)channel.GetUserData();
  assert( ud != NULL );
  if ( ud->panel == NULL ) {
    cout << "   !! ud->panel NULL" << endl;
    return;
  }
  ud->panel->SetTopic( WX_STRING(user.GetNick()), WX_STRING(topic) );   
}




void Ui::OnUserOnline( User& user )
{
}


void Ui::OnUserOffline( User& user )
{
}


void Ui::OnUserStatusChanged( User& user )
{
}

