//
// Class: Ui
//

#include "ui.h"
#include "tasserver.h"
#include "settings.h"
#include "server.h"
#include "spring.h"
#include "channel.h"


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


void Ui::Disconnect()
{
  if ( m_serv != NULL ) {
    m_serv->Disconnect();
    Socket* sock =  m_serv->GetSocket();
    m_serv->SetSocket( NULL );
    delete sock; sock = NULL;
    delete m_serv; m_serv = NULL;  
  }
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
  
  Disconnect();
  
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
  
  m_serv->uidata.panel = m_main_win->GetChatTab().AddChatPannel( *m_serv, servername );
  m_serv->uidata.panel->StatusMessage( _T("Connecting to server ") + servername + _T("...") );

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
  wxTextEntryDialog name_dlg( NULL, question, heading, _T(""), wxOK | wxCANCEL | wxCENTRE );
  int res = name_dlg.ShowModal();
  answer = name_dlg.GetValue();
  
  return ( res == wxID_OK);
}


void Ui::ShowMessage( const wxString& heading, const wxString& message )
{
  wxMessageDialog msg( NULL, heading, message, wxOK);
  msg.ShowModal();
}


bool Ui::ExecuteSayCommand( const wxString& cmd )
{

  if ( !IsConnected() ) return false;
  if ( (cmd.BeforeFirst(' ').Lower() == _T("/join")) || (cmd.BeforeFirst(' ').Lower() == _T("/j")) ) {
    wxString channel = cmd.AfterFirst(' ');
    wxString pass = channel.AfterFirst(' ');
    if ( !pass.IsEmpty() ) channel = channel.BeforeFirst(' ');
    if ( channel.StartsWith(_T("#")) ) channel.Remove( 0, 1 );

    m_serv->JoinChannel( STL_STRING(channel), STL_STRING(pass) );
    return true;
  }
  return false;
}





////////////////////////////////////////////////////////////////////////////////////////////
// EVENTS
////////////////////////////////////////////////////////////////////////////////////////////


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
  debug_func( "" );
  server.uidata.panel->StatusMessage( _T("Connected to ") + WX_STRING(server_name) + _T(".") );
  //server.uidata.panel = m_main_win->GetChatTab().AddChatPannel( server, WX_STRING(server_name) );
}


void Ui::OnDisconnected( Server& server )
{
  debug_func( "" );
  server.uidata.panel->StatusMessage( _T("Disconnected from server.") );
  server.uidata.panel->SetServer( NULL );
  server.uidata.panel = NULL;
}


//! @brief Called when client has joined a channel
//!
//! @todo Check if a pannel allready exists for this channel
void Ui::OnJoinedChannelSuccessful( Channel& chan )
{
  debug_func( "" );

  chan.uidata.panel = NULL;
  m_main_win->OpenChannelChat( chan );
}


//! @brief Called when something is said in a channel
void Ui::OnChannelSaid( Channel& channel, User& user, const std::string& message )
{
  debug_func( "" );
  if ( channel.uidata.panel == NULL ) {
    debug_error( "ud->panel NULL" );
    return;
  }
  channel.uidata.panel->Said( WX_STRING(user.GetNick()), WX_STRING( message ) );
}


void Ui::OnChannelDidAction( Channel& channel , User& user, const std::string& action )
{
  debug_func( "" );
  if ( channel.uidata.panel == NULL ) {
    debug_error( "ud->panel NULL" );
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
  //debug_func( "" );
  if ( chan.uidata.panel == NULL ) {
    debug_error( "ud->panel NULL" );
    return;
  }
  chan.uidata.panel->Joined( user );  
}


void Ui::OnUserLeftChannel( Channel& chan, User& user, const std::string& reason )
{
  //debug_func( "" );
  if ( chan.uidata.panel == NULL ) {
    debug_error( "ud->panel NULL" );
    return;
  }
  chan.uidata.panel->Parted( user, WX_STRING(reason) );  
}


void Ui::OnChannelTopic( Channel& channel , const std::string user, const std::string& topic )
{
  debug_func( "" );
  if ( channel.uidata.panel == NULL ) {
    debug_error( "ud->panel NULL" );
    return;
  }
  channel.uidata.panel->SetTopic( WX_STRING(user), WX_STRING(topic) );   
}


void Ui::OnChannelList( const std::string& channel, const int& numusers )
{
  
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

void Ui::OnUserSaid( User& user, const std::string message, bool fromme )
{
  if ( user.uidata.panel == NULL ) {
    m_main_win->OpenPrivateChat( user );
  }
  if ( fromme ) user.uidata.panel->Said( WX_STRING(m_serv->GetMe().GetNick()), WX_STRING(message) );
  else user.uidata.panel->Said( WX_STRING(user.GetNick()), WX_STRING(message) );
}


void Ui::OnBattleOpened( Battle& battle )
{
  mw().GetJoinTab().GetBattleListTab().AddBattle( battle );
}


void Ui::OnBattleClosed( Battle& battle )
{
  mw().GetJoinTab().GetBattleListTab().RemoveBattle( battle );
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != NULL ) {
    if ( &br->GetBattle() == &battle ) mw().GetJoinTab().LeaveCurrentBattle();
  }
}


void Ui::OnUserJoinedBattle( Battle& battle, User& user )
{
  mw().GetJoinTab().GetBattleListTab().UpdateBattle( battle );
  
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != NULL ) {
    if ( &br->GetBattle() == &battle ) br->OnUserJoined( user );
  }
}


void Ui::OnUserLeftBattle( Battle& battle, User& user )
{
  mw().GetJoinTab().GetBattleListTab().UpdateBattle( battle );
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != NULL ) {
    if ( &br->GetBattle() == &battle ) {
      br->OnUserLeft( user );
      if ( &user == &m_serv->GetMe() ) mw().GetJoinTab().LeaveCurrentBattle();
    }
  }
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


void Ui::OnRequestBattleStatus( Battle& battle )
{
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != NULL ) {
    if ( &br->GetBattle() == &battle ) {
      br->GetBattle().OnRequestBattleStatus();
    }
  }
}


void Ui::OnBattleStarted( Battle& battle )
{
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != NULL ) {
    if ( &br->GetBattle() == &battle ) {
      m_spring->Run( battle );
    }
  }
}


void Ui::OnSaidBattle( Battle& battle, const std::string& nick, const std::string& msg )
{
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != NULL ) {
    br->GetChatPanel().Said( WX_STRING(nick), WX_STRING(msg) );
  }
}


void Ui::OnBattleAction( Battle& battle, const std::string& nick, const std::string& msg )
{
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != NULL ) {
    br->GetChatPanel().DidAction( WX_STRING(nick), WX_STRING(msg) );
  }
}


void Ui::OnSpringTerminated( bool success )
{

}


void Ui::OnBattleStartRectsUpdated( Battle& battle )
{
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != NULL ) {
    br->GetChatPanel().StatusMessage( _T("Start rects updated.") );
  }
}


void Ui::OnBattleDisableUnit( Battle& battle, const std::string& unitname )
{
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != NULL ) {
    //std::string fullname = usync().GetFullUnitName( battle.opts().modname, usync().GetUnitIndex( battle.opts().modname, unitname ) );
    br->GetChatPanel().StatusMessage( WX_STRING( unitname ) + _T(" disabled.") );
  }
}


void Ui::OnBattleEnableUnit( Battle& battle, const std::string& unitname )
{
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != NULL ) {
    br->GetChatPanel().StatusMessage( WX_STRING(unitname) + _T(" disabled.") );
  }
}


void Ui::OnBattleEnableAllUnits( Battle& battle )
{
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != NULL ) {
    br->GetChatPanel().StatusMessage( _T("All units enabled.") );
  }

}

