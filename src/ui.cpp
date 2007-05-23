//
// Class: Ui
//

#include "ui.h"
#include "utils.h"


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


//! @brief Display a dialog asking a question with OK and Canel buttons
//!
//! @return true if OK button was pressed
//! @note this does not return until the user pressed any of the buttons or closed the dialog.
bool Ui::Ask( const wxString& heading, const wxString& question )
{
  wxMessageDialog ask_dlg(NULL, question, heading, wxOK | wxCANCEL );
  return ( ask_dlg.ShowModal() == wxID_OK );
}


//! @brief Called when connected to a server
//!
//! @todo Display in servertab
void Ui::OnConnected( const std::string& server_name, const std::string& server_ver, bool supported )
{
  
}


//! @brief Called when client has joined a channel
//!
//! @todo Check if a pannel allready exists for this channel
void Ui::OnJoinedChannelSuccessful( Channel& chan )
{
  std::cout << "** Ui::OnJoinedChannelSuccessful()" << std::endl;
  UiChannelData* chandata = new UiChannelData();
  chandata->panel = NULL;
  
  chan.SetUserData( (void*)chandata );
  mw().OpenChannelChat( chan );
}


//! @brief Called when something is said in a channel
void Ui::OnChannelSaid( Channel& channel, User& user, const std::string& message )
{
  std::cout << "** Ui::OnChannelSaid()" << std::endl;
  UiChannelData* ud = (UiChannelData*)channel.GetUserData();
  assert( ud != NULL );
  if ( ud->panel == NULL ) {
    std::cout << "   !! ud->panel NULL" << std::endl;
    return;
  }
  ud->panel->Said( WX_STRING(user.GetNick()), WX_STRING( message ) );
}


void Ui::OnChannelDidAction( Channel& channel , User& user, const std::string& action )
{
  std::cout << "** Ui::OnChannelDidAction()" << std::endl;
  UiChannelData* ud = (UiChannelData*)channel.GetUserData();
  assert( ud != NULL );
  if ( ud->panel == NULL ) {
    std::cout << "   !! ud->panel NULL" << std::endl;
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
  //std::cout << "** Ui::OnUserJoinedChannel()" << std::endl;
  UiChannelData* ud = (UiChannelData*)chan.GetUserData();
  assert( ud != NULL );
  if ( ud->panel == NULL ) {
    std::cout << "   !! ud->panel NULL" << std::endl;
    return;
  }
  ud->panel->Joined( user );  
}


void Ui::OnUserLeftChannel( Channel& chan, User& user, const std::string& reason )
{
  std::cout << "** Ui::OnUserLeftChannel()" << std::endl;
  UiChannelData* ud = (UiChannelData*)chan.GetUserData();
  assert( ud != NULL );
  if ( ud->panel == NULL ) {
    std::cout << "   !! ud->panel NULL" << std::endl;
    return;
  }
  ud->panel->Parted( user, WX_STRING(reason) );  
}


void Ui::OnChannelTopic( Channel& channel , const std::string user, const std::string& topic )
{
  std::cout << "** Ui::OnChannelTopic()" << std::endl;
  UiChannelData* ud = (UiChannelData*)channel.GetUserData();
  assert( ud != NULL );
  if ( ud->panel == NULL ) {
    std::cout << "   !! ud->panel NULL" << std::endl;
    return;
  }
  ud->panel->SetTopic( WX_STRING(user), WX_STRING(topic) );   
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

