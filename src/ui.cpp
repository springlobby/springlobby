//
// Class: Ui
//

#include "ui.h"
#include "tasserver.h"
#include "settings.h"
#include "server.h"


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
    m_con_win = new ConnectWindow( m_main_win, *this );
  }
  m_con_win->CenterOnParent();
  m_con_win->Show(true);
}


//! @brief Connects to default server or opens the ConnectWindow
//!
//! @todo Fix Auto Connect
//! @see DoConnect
void Ui::Connect()
{
  ShowConnectWindow();
}


//! @brief Opens the accutial connection to a server.
void Ui::DoConnect( const wxString& servername, const wxString& username, const wxString& password )
{
  std::string host;
  int port;
  Socket* sock;
  
  if ( !sett().ServerExists( STL_STRING(servername) ) ) {
    assert( false );
    return;
  }
  
  if ( m_serv != NULL ) {
    // Delete old Server object
    
    //if ( Ask( _T("Already connected"), _T("You are already connected to a\nserver. Do you want to disconnect?") ) ) {
      m_serv->Disconnect();
      sock =  m_serv->GetSocket();
      m_serv->SetSocket( NULL );
      delete sock; sock = NULL;
      delete m_serv; m_serv = NULL;
    //}
    
  }
  
  // Create new Server object
  m_serv = new TASServer( *this );
  sock = new Socket( *m_serv );
  m_serv->SetSocket( sock );
  //m_serv->SetServerEvents( &se() );
  
  m_serv->SetUsername( STL_STRING(username) );
  m_serv->SetPassword( STL_STRING(password) );
  
  if ( sett().GetServerAccountSavePass( STL_STRING(servername) ) ) {
    if ( m_serv->IsPasswordHash(STL_STRING(password)) ) sett().SetServerAccountPass( STL_STRING(servername), STL_STRING(password) );
    else sett().SetServerAccountPass( STL_STRING(servername), m_serv->GetPasswordHash( STL_STRING(password) ) );
  } else {
    sett().SetServerAccountPass( STL_STRING(servername), "" );    
  }
  sett().SaveSettings();
  
  host = sett().GetServerHost( STL_STRING(servername) );
  port = sett().GetServerPort( STL_STRING(servername) );
  
  // Connect
  m_serv->Connect( host, port );

}


//! @brief Quits the entire application
void Ui::Quit()
{
  assert( m_main_win != NULL );
  sett().SaveSettings();
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


bool Ui::AskText( const wxString& heading, const wxString& question, wxString& answer )
{
  wxTextEntryDialog name_dlg( NULL, question, heading, _(""), wxOK | wxCANCEL | wxCENTRE );
  int res = name_dlg.ShowModal();
  answer = name_dlg.GetValue();
  std::cout << "** Ui::AskText(): a: " << answer.c_str() << std::endl;
  return ( res == wxID_OK);
}


void Ui::ShowMessage( const wxString& heading, const wxString& message )
{
  wxMessageDialog msg( NULL, heading, message, wxOK);
  msg.ShowModal();
}


void Ui::OnUpdate()
{
  if ( m_serv != NULL ) {
    m_serv->Update();
  }
}


//! @brief Called when connected to a server
//!
//! @todo Display in servertab
void Ui::OnConnected( Server& server, const std::string& server_name, const std::string& server_ver, bool supported )
{
  std::cout << "** Ui::OnConnected()" << std::endl;
  server.uidata.panel = m_main_win->GetChatTab().AddChatPannel( server, WX_STRING(server_name) );
}


//! @brief Called when client has joined a channel
//!
//! @todo Check if a pannel allready exists for this channel
void Ui::OnJoinedChannelSuccessful( Channel& chan )
{
  std::cout << "** Ui::OnJoinedChannelSuccessful()" << std::endl;

  chan.uidata.panel = NULL;
  m_main_win->OpenChannelChat( chan );
}


//! @brief Called when something is said in a channel
void Ui::OnChannelSaid( Channel& channel, User& user, const std::string& message )
{
  std::cout << "** Ui::OnChannelSaid()" << std::endl;
  if ( channel.uidata.panel == NULL ) {
    std::cout << "   !! ud->panel NULL" << std::endl;
    return;
  }
  channel.uidata.panel->Said( WX_STRING(user.GetNick()), WX_STRING( message ) );
}


void Ui::OnChannelDidAction( Channel& channel , User& user, const std::string& action )
{
  std::cout << "** Ui::OnChannelDidAction()" << std::endl;
  if ( channel.uidata.panel == NULL ) {
    std::cout << "   !! ud->panel NULL" << std::endl;
    return;
  }
  channel.uidata.panel->DidAction( WX_STRING(user.GetNick()), WX_STRING( action ) );  
}


//! @brief Called when client is leaving a channel
//!
//! @todo Tell ChatPanel the channel is no longer joined
void Ui::OnLeaveChannel( Channel& channel )
{

}


void Ui::OnUserJoinedChannel( Channel& chan, User& user )
{
  //std::cout << "** Ui::OnUserJoinedChannel()" << std::endl;
  if ( chan.uidata.panel == NULL ) {
    std::cout << "   !! ud->panel NULL" << std::endl;
    return;
  }
  chan.uidata.panel->Joined( user );  
}


void Ui::OnUserLeftChannel( Channel& chan, User& user, const std::string& reason )
{
  //std::cout << "** Ui::OnUserLeftChannel()" << std::endl;
  if ( chan.uidata.panel == NULL ) {
    std::cout << "   !! ud->panel NULL" << std::endl;
    return;
  }
  chan.uidata.panel->Parted( user, WX_STRING(reason) );  
}


void Ui::OnChannelTopic( Channel& channel , const std::string user, const std::string& topic )
{
  std::cout << "** Ui::OnChannelTopic()" << std::endl;
  if ( channel.uidata.panel == NULL ) {
    std::cout << "   !! ud->panel NULL" << std::endl;
    return;
  }
  channel.uidata.panel->SetTopic( WX_STRING(user), WX_STRING(topic) );   
}


void Ui::OnChannelList( const std::string& channel, const int& numusers )
{
  std::cout << "** Ui::OnChannelList( " << channel.c_str() << ", " << numusers << " )" << std::endl;
}


void Ui::OnUserOnline( User& user )
{
/*  UiUserData* data = new UiUserData();
  data->panel = NULL;
  
  user.SetUserData( (void*)data );*/
}


void Ui::OnUserOffline( User& user )
{
/*  UiUserData* data = (UiUserData*)user.GetUserData();
  if ( data == NULL) return;
    
  delete data;*/
}


void Ui::OnUserStatusChanged( User& user )
{
  for ( int i = 0; i < m_serv->GetNumChannels(); i++ ) {
    Channel& chan = m_serv->GetChannel( i );
    if ( ( chan.UserExists(user.GetNick()) ) && ( chan.uidata.panel != NULL ) ) {
      chan.uidata.panel->UserStatusUpdated( user );
    }
  }
}


void Ui::OnUnknownCommand( Server& server, const std::string& command, const std::string& params )
{
  if ( server.uidata.panel != NULL ) server.uidata.panel->UnknownCommand( WX_STRING(command), WX_STRING(params) );
}


void Ui::OnMotd( Server& server, const std::string& message )
{
  if ( server.uidata.panel != NULL ) server.uidata.panel->Motd( WX_STRING(message) );  
}

void Ui::OnUserSaid( User& user, const std::string message )
{
  if ( user.uidata.panel == NULL ) {
    m_main_win->OpenPrivateChat( user );
  }
  user.uidata.panel->Said( WX_STRING(user.GetNick()), WX_STRING(message) );
}


void Ui::OnBattleOpened( Battle& battle )
{
  m_main_win->GetJoinTab().GetBattleListTab().AddBattle( battle );
}


void Ui::OnBattleClosed( Battle& battle )
{
  m_main_win->GetJoinTab().GetBattleListTab().RemoveBattle( battle );
}


void Ui::OnUserJoinedBattle( Battle& battle, User& user )
{
  m_main_win->GetJoinTab().GetBattleListTab().UpdateBattle( battle );
}


void Ui::OnUserLeftBattle( Battle& battle, User& user )
{
  m_main_win->GetJoinTab().GetBattleListTab().UpdateBattle( battle );
}


void Ui::OnBattleInfoUpdated( Battle& battle )
{
  m_main_win->GetJoinTab().GetBattleListTab().UpdateBattle( battle );
}

void Ui::OnJoinedBattle( Battle& battle )
{
  mw().GetJoinTab().JoinBattle( battle );
}

void Ui::OnUserBattleStatus( Battle& battle, User& user )
{
  mw().GetJoinTab().BattleUserUpdated( user );
}

