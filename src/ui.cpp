//
// Class: Ui
//

#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <stdexcept>
#include <wx/msgdlg.h>
#include <wx/thread.h>
#include <wx/intl.h>

#include "spring.h"
#include "ui.h"
#include "tasserver.h"
#include "settings.h"
#include "server.h"
#include "spring.h"
#include "channel.h"
#include "utils.h"
#include "connectwindow.h"
#include "mainwindow.h"
#include "user.h"
#include "chatpanel.h"
#include "battlelisttab.h"
#include "battleroomtab.h"
#include "socket.h"
#include "battle.h"
#include "mainchattab.h"
#include "mainjoinbattletab.h"
#include "agreementdialog.h"


Ui::Ui() :
  m_serv(0),
  m_con_win(0)
{
  m_main_win = new MainWindow( *this );
  m_spring = new Spring(*this);
}

Ui::~Ui() {
  Disconnect();
  if ( m_main_win != 0 ) delete m_main_win;
  delete m_spring;
}

Server& Ui::GetServer()
{
  ASSERT_LOGIC( m_serv != 0, "m_serv NULL!" );
  return *m_serv;
}

MainWindow& Ui::mw()
{
  assert( m_main_win != 0 );
  return *m_main_win;
}

//! @brief Shows the main window on screen
void Ui::ShowMainWindow()
{
  assert( m_main_win != 0 );
  m_main_win->Show(true);
}


//! @brief Show the connect window on screen
//!
//! @note It will create the ConnectWindow if not allready created
void Ui::ShowConnectWindow()
{
  if ( m_con_win == 0 ) {
    assert( m_main_win != 0 );
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
  if ( m_serv != 0 ) {
    m_serv->Disconnect();
    Socket* sock = m_serv->GetSocket();
    m_serv->SetSocket( 0 );
    delete sock; sock = 0;
    delete m_serv; m_serv = 0;
  }
}


//! @brief Opens the accutial connection to a server.
void Ui::DoConnect( const wxString& servername, const wxString& username, const wxString& password )
{
  std::string host;
  int port;
  Socket* sock;

  if ( !sett().ServerExists( STD_STRING(servername) ) ) {
    assert( false );
    return;
  }

  Disconnect();

  // Create new Server object
  m_serv = new TASServer( *this );
  sock = new Socket( *m_serv );
  m_serv->SetSocket( sock );
  //m_serv->SetServerEvents( &se() );

  m_serv->SetUsername( STD_STRING(username) );
  m_serv->SetPassword( STD_STRING(password) );

  if ( sett().GetServerAccountSavePass( STD_STRING(servername) ) ) {
    if ( m_serv->IsPasswordHash(STD_STRING(password)) ) sett().SetServerAccountPass( STD_STRING(servername), STD_STRING(password) );
    else sett().SetServerAccountPass( STD_STRING(servername), m_serv->GetPasswordHash( STD_STRING(password) ) );
  } else {
    sett().SetServerAccountPass( STD_STRING(servername), "" );
  }
  sett().SaveSettings();

  host = sett().GetServerHost( STD_STRING(servername) );
  port = sett().GetServerPort( STD_STRING(servername) );

  m_serv->uidata.panel = m_main_win->GetChatTab().AddChatPannel( *m_serv, servername );
  m_serv->uidata.panel->StatusMessage( _T("Connecting to server ") + servername + _T("...") );

  // Connect
  m_serv->Connect( host, port );

}


bool Ui::DoRegister( const wxString& servername, const wxString& username, const wxString& password )
{
  std::string host;
  int port;

  if ( !sett().ServerExists( STD_STRING(servername) ) ) {
    assert( false );
    return false;
  }

  // Create new Server object
  TASServer* serv = new TASServer( *this );
  Socket* sock = new Socket( *serv, true );
  serv->SetSocket( sock );

  host = sett().GetServerHost( STD_STRING(servername) );
  port = sett().GetServerPort( STD_STRING(servername) );

  return serv->Register( host, port, STD_STRING(username), STD_STRING(password) );

}


bool Ui::IsConnected() const
{
  if ( m_serv != 0 )
    return m_serv->IsConnected();
  return false;
}

void Ui::JoinChannel( const wxString& name, const wxString& password )
{
  if ( m_serv != 0 )
    m_serv->JoinChannel( STD_STRING(name), STD_STRING(password) );
}


//! @brief Quits the entire application
void Ui::Quit()
{
  assert( m_main_win != 0 );
  sett().SaveSettings();
  m_main_win->Close();
}


//! @brief Display a dialog asking a question with OK and Canel buttons
//!
//! @return true if OK button was pressed
//! @note this does not return until the user pressed any of the buttons or closed the dialog.
bool Ui::Ask( const wxString& heading, const wxString& question )
{
  wxMessageDialog ask_dlg(0, question, heading, wxOK | wxCANCEL );
  return ( ask_dlg.ShowModal() == wxID_OK );
}


bool Ui::AskText( const wxString& heading, const wxString& question, wxString& answer )
{
  wxTextEntryDialog name_dlg( 0, question, heading, _T(""), wxOK | wxCANCEL | wxCENTRE );
  int res = name_dlg.ShowModal();
  answer = name_dlg.GetValue();

  return ( res == wxID_OK);
}


void Ui::ShowMessage( const wxString& heading, const wxString& message )
{
  wxMessageDialog msg( 0, heading, message, wxOK);
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

    m_serv->JoinChannel( STD_STRING(channel), STD_STRING(pass) );
    return true;
  }
  return false;
}





////////////////////////////////////////////////////////////////////////////////////////////
// EVENTS
////////////////////////////////////////////////////////////////////////////////////////////


void Ui::OnUpdate()
{
  if ( m_serv != 0 ) {
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
  if ( m_main_win == 0 ) return;

  mw().GetJoinTab().LeaveCurrentBattle();

  server.uidata.panel->StatusMessage( _T("Disconnected from server.") );
  server.uidata.panel->SetServer( 0 );
  server.uidata.panel = 0;
}


//! @brief Called when client has joined a channel
//!
//! @todo Check if a pannel allready exists for this channel
void Ui::OnJoinedChannelSuccessful( Channel& chan )
{
  debug_func( "" );

  chan.uidata.panel = 0;
  m_main_win->OpenChannelChat( chan );
}


//! @brief Called when something is said in a channel
void Ui::OnChannelSaid( Channel& channel, User& user, const std::string& message )
{
  debug_func( "" );
  if ( channel.uidata.panel == 0 ) {
    debug_error( "ud->panel NULL" );
    return;
  }
  channel.uidata.panel->Said( WX_STRING(user.GetNick()), WX_STRING( message ) );
}


void Ui::OnChannelDidAction( Channel& channel , User& user, const std::string& action )
{
  debug_func( "" );
  if ( channel.uidata.panel == 0 ) {
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
  if ( chan.uidata.panel == 0 ) {
    debug_error( "ud->panel NULL" );
    return;
  }
  chan.uidata.panel->Joined( user );
}


void Ui::OnUserLeftChannel( Channel& chan, User& user, const std::string& reason )
{
  //debug_func( "" );
  if ( chan.uidata.panel == 0 ) {
    debug_error( "ud->panel NULL" );
    return;
  }
  chan.uidata.panel->Parted( user, WX_STRING(reason) );
}


void Ui::OnChannelTopic( Channel& channel , const std::string user, const std::string& topic )
{
  debug_func( "" );
  if ( channel.uidata.panel == 0 ) {
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
  data->panel = 0;

  user.SetUserData( (void*)data );*/
}


void Ui::OnUserOffline( User& user )
{
/*  UiUserData* data = (UiUserData*)user.GetUserData();
  if ( data == 0) return;

  delete data;*/
}


void Ui::OnUserStatusChanged( User& user )
{
  for ( int i = 0; i < m_serv->GetNumChannels(); i++ ) {
    Channel& chan = m_serv->GetChannel( i );
    if ( ( chan.UserExists(user.GetNick()) ) && ( chan.uidata.panel != 0 ) ) {
      chan.uidata.panel->UserStatusUpdated( user );
    }
  }
}


void Ui::OnUnknownCommand( Server& server, const std::string& command, const std::string& params )
{
  if ( server.uidata.panel != 0 ) server.uidata.panel->UnknownCommand( WX_STRING(command), WX_STRING(params) );
}


void Ui::OnMotd( Server& server, const std::string& message )
{
  if ( server.uidata.panel != 0 ) server.uidata.panel->Motd( WX_STRING(message) );
}

void Ui::OnUserSaid( User& user, const std::string message, bool fromme )
{
  if ( user.uidata.panel == 0 ) {
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
  if ( br != 0 ) {
    if ( &br->GetBattle() == &battle ) mw().GetJoinTab().LeaveCurrentBattle();
  }
}


void Ui::OnUserJoinedBattle( Battle& battle, User& user )
{
  mw().GetJoinTab().GetBattleListTab().UpdateBattle( battle );

  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    if ( &br->GetBattle() == &battle ) br->OnUserJoined( user );
  }
}


void Ui::OnUserLeftBattle( Battle& battle, User& user )
{
  mw().GetJoinTab().GetBattleListTab().UpdateBattle( battle );
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    if ( &br->GetBattle() == &battle ) {
      br->OnUserLeft( user );
      if ( &user == &m_serv->GetMe() ) mw().GetJoinTab().LeaveCurrentBattle();
    }
  }
}


void Ui::OnBattleInfoUpdated( Battle& battle )
{
  m_main_win->GetJoinTab().GetBattleListTab().UpdateBattle( battle );
  if ( m_main_win->GetJoinTab().GetCurrentBattle() == &battle ) {
    mw().GetJoinTab().UpdateCurrentBattle();
  }
}


void Ui::OnJoinedBattle( Battle& battle )
{
  mw().GetJoinTab().JoinBattle( battle );
  /*if ( !Spring::TestSpringBinary() ) {
    wxMessageBox( _("Your spring settings are probably not configured correctly,\nyou should take another look at your settings before trying\nto play online."), _("Spring settings error"), wxOK );
  }*/
  if ( battle.opts().nattype != NAT_None ) {
    wxMessageBox( _("This game uses NAT traversal that is not yet supported\nby SpringLobby.\n\nYou will not be able to play in this battle."), _("NAT traversal"), wxOK );
  }
}


void Ui::OnUserBattleStatus( Battle& battle, User& user )
{
  mw().GetJoinTab().BattleUserUpdated( user );
}


void Ui::OnRequestBattleStatus( Battle& battle )
{
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    if ( &br->GetBattle() == &battle ) {
      br->GetBattle().OnRequestBattleStatus();
    }
  }
}


void Ui::OnBattleStarted( Battle& battle )
{
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    if ( &br->GetBattle() == &battle ) {
      battle.GetMe().BattleStatus().ready = false;
      battle.SendMyBattleStatus();
      battle.GetMe().Status().in_game = true;
      battle.GetMe().SendMyUserStatus();
      m_spring->Run( battle );
    }
  }
}


void Ui::OnSaidBattle( Battle& battle, const std::string& nick, const std::string& msg )
{
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    br->GetChatPanel().Said( WX_STRING(nick), WX_STRING(msg) );
  }
}


void Ui::OnBattleAction( Battle& battle, const std::string& nick, const std::string& msg )
{
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    br->GetChatPanel().DidAction( WX_STRING(nick), WX_STRING(msg) );
  }
}


void Ui::OnSpringTerminated( bool success )
{
  if ( m_serv == 0 ) return;

  m_serv->GetMe().Status().in_game = false;
  m_serv->GetMe().SendMyUserStatus();
}


void Ui::OnBattleStartRectsUpdated( Battle& battle )
{

  mw().GetJoinTab().UpdateCurrentBattle();
  /*
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    br->GetChatPanel().StatusMessage( _T("Start rects updated.") );
    br->UpdateBattleInfo();
  }*/
}


void Ui::OnBattleDisableUnit( Battle& battle, const std::string& unitname )
{
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    //std::string fullname = usync()->GetFullUnitName( battle.opts().modname, usync()->GetUnitIndex( battle.opts().modname, unitname ) );
    br->GetChatPanel().StatusMessage( WX_STRING( unitname ) + _T(" disabled.") );
  }
}


void Ui::OnBattleEnableUnit( Battle& battle, const std::string& unitname )
{
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    br->GetChatPanel().StatusMessage( WX_STRING(unitname) + _T(" disabled.") );
  }
}


void Ui::OnBattleEnableAllUnits( Battle& battle )
{
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    br->GetChatPanel().StatusMessage( _T("All units enabled.") );
  }

}


void Ui::OnAcceptAgreement( const std::string& agreement )
{
  AgreementDialog dlg( m_main_win, WX_STRING(agreement) );
  if ( dlg.ShowModal() == 1 ) {
    m_serv->AcceptAgreement();
    m_serv->Login();
  }
}

