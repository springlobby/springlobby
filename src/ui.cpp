/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: Ui
//

#include <wx/textdlg.h>
#include <stdexcept>
#include <wx/thread.h>
#include <wx/intl.h>
#include <wx/utils.h>

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
#ifndef NO_TORRENT_SYSTEM
#include "maintorrenttab.h"
#include "torrentwrapper.h"
#endif
#include "agreementdialog.h"
#include "unitsyncthread.h"

#include "settings++/custom_dialogs.h"

Ui::Ui() :
  m_serv(0),
  m_main_win(0),
  m_con_win(0)
{
   ReloadUnitSync();

  m_upd_intv_counter = 0;
  m_main_win = new MainWindow( *this );
  CustomMessageBoxBase::setLobbypointer(m_main_win);
  m_spring = new Spring(*this);
  m_thread = new UnitSyncThread( *this );
  m_thread->Init();
}

Ui::~Ui() {
  Disconnect();

  delete m_main_win;
  delete m_spring;
  m_thread->Delete();

  m_thread_wait.Enter();
  m_thread_wait.Leave();
}

Server& Ui::GetServer()
{
  ASSERT_LOGIC( m_serv != 0, _T("m_serv NULL!") );
  return *m_serv;
}

bool Ui::GetServerStatus() const
{
  return (bool)(m_serv);
}


ChatPanel* Ui::GetActiveChatPanel()
{
  return mw().GetActiveChatPanel();
}


MainWindow& Ui::mw()
{
  ASSERT_LOGIC( m_main_win != 0, _T("m_main_win = 0") );
  return *m_main_win;
}


bool Ui::IsMainWindowCreated()
{
  if ( m_main_win == 0 ) return false;
  else return true;
}


//! @brief Shows the main window on screen
void Ui::ShowMainWindow()
{
  ASSERT_LOGIC( m_main_win != 0, _T("m_main_win = 0") );
  mw().Show(true);
}


//! @brief Show the connect window on screen
//!
//! @note It will create the ConnectWindow if not allready created
void Ui::ShowConnectWindow()
{
  if ( m_con_win == 0 ) {
    ASSERT_LOGIC( m_main_win != 0, _T("m_main_win = 0") );
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
    bool doit = sett().GetAutoConnect();
    if ( !doit )
        ShowConnectWindow();
    else
    {
        // do something when pw isn't remembered
        wxString server_name = sett().GetDefaultServer();
        wxString nick = sett().GetServerAccountNick( server_name );
        wxString pass = sett().GetServerAccountPass( server_name );
        DoConnect( server_name, nick, pass);
    }
}


void Ui::Reconnect()
{
  wxString servname = sett().GetDefaultServer();

  wxString pass  = sett().GetServerAccountPass(servname);
  if ( !sett().GetServerAccountSavePass(servname) ) {
    wxString pass2 = pass;
    if ( !AskPassword( _("Server password"), _("Password"), pass2 ) ) return;
    pass = pass2;
  }

  if ( IsConnected() ) Disconnect();
  DoConnect( servname, sett().GetServerAccountNick(servname), pass );
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
  wxString host;
  int port;
  Socket* sock;

  if ( !sett().ServerExists( servername ) ) {
    ASSERT_LOGIC( false, _T("Server does not exist in settings") );
    return;
  }

  Disconnect();

  // Create new Server object
  m_serv = new TASServer( *this );
  sock = new Socket( *m_serv );
  m_serv->SetSocket( sock );
  //m_serv->SetServerEvents( &se() );

  m_serv->SetUsername( username );
  m_serv->SetPassword( password );

  if ( sett().GetServerAccountSavePass( servername ) ) {
    if ( m_serv->IsPasswordHash(password) ) sett().SetServerAccountPass( servername, password );
    else sett().SetServerAccountPass( servername, m_serv->GetPasswordHash( password ) );
  } else {
    sett().SetServerAccountPass( servername, _T("") );
  }
  sett().SaveSettings();

  host = sett().GetServerHost( servername );
  port = sett().GetServerPort( servername );

  m_serv->uidata.panel = m_main_win->GetChatTab().AddChatPannel( *m_serv, servername );
  m_serv->uidata.panel->StatusMessage( _T("Connecting to server ") + servername + _T("...") );

  // Connect
  m_serv->Connect( host, port );

}


bool Ui::DoRegister( const wxString& servername, const wxString& username, const wxString& password,wxString& reason)
{
  wxString host;
  int port;

  if ( !sett().ServerExists( servername ) ) {
    ASSERT_LOGIC( false, _T("Server does not exist in settings") );
    return false;
  }

  // Create new Server object
  TASServer* serv = new TASServer( *this );
  Socket* sock = new Socket( *serv, true );
  serv->SetSocket( sock );

  host = sett().GetServerHost( servername );
  port = sett().GetServerPort( servername );

  return serv->Register( host, port, username, password,reason );

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
    m_serv->JoinChannel( name, password );
}


void Ui::StartHostedBattle()
{
  ASSERT_LOGIC( m_serv != 0, _T("m_serv = 0") );
  m_serv->StartHostedBattle();
  sett().SetLastHostMap( m_serv->GetCurrentBattle()->GetMapName() );
  sett().SaveBattleMapOptions(m_serv->GetCurrentBattle());
  sett().SaveSettings();
}


void Ui::StartSinglePlayerGame( SinglePlayerBattle& battle )
{
  #ifndef NO_TORRENT_SYSTEM
  torrent()->SetIngameStatus(true);
  #endif
  m_spring->Run( battle );
}


bool Ui::IsSpringRunning()
{
   return m_spring->IsRunning();
}


//! @brief Quits the entire application
void Ui::Quit()
{
  ASSERT_LOGIC( m_main_win != 0, _T("m_main_win = 0") );
  sett().SaveSettings();
  m_main_win->forceSettingsFrameClose();

  m_main_win->Close();
  m_thread->Kill();
  m_con_win->Close();
  if (m_serv != 0 ) m_serv->Disconnect();
}


void Ui::ReloadUnitSync()
{
  usync()->ReloadUnitSyncLib();
  if ( m_main_win != 0 ) m_main_win->OnUnitSyncReloaded();
}


void Ui::DownloadMap( const wxString& hash, const wxString& name )
{
  #ifndef NO_TORRENT_SYSTEM
  if ( !hash.IsEmpty() ) torrent()->RequestFileByHash( hash );
  else if ( !name.IsEmpty() ) torrent()->RequestFileByName( name );
  #else
  wxString url = _T("http://spring.jobjol.nl/search.php");
  OpenWebBrowser ( url );
  #endif
}


void Ui::DownloadMod( const wxString& hash, const wxString& name )
{
  #ifndef NO_TORRENT_SYSTEM
  if ( !hash.IsEmpty() ) torrent()->RequestFileByHash( hash );
  else if ( !name.IsEmpty() ) torrent()->RequestFileByName( name );
  #else
  wxString url = _T("http://spring.jobjol.nl/search.php");
  OpenWebBrowser ( url );
  #endif
}


void Ui::OpenWebBrowser( const wxString& url )
{
  if ( sett().GetWebBrowserPath() == _T("use default") || sett().GetWebBrowserPath().IsEmpty() )
  {
      if ( !wxLaunchDefaultBrowser( url ) )
      {
        wxLogWarning( _T("can't launch default browser") );
        customMessageBoxNoModal(SL_MAIN_ICON, _("Couldn't launch browser. URL is: ") + url, _("Couldn't launch browser.")  );
      }
  }
  else
  {
    if ( !wxExecute ( sett().GetWebBrowserPath() + _T(" ") + url, wxEXEC_ASYNC ) )
    {
      wxLogWarning( _T("can't launch browser: %s"), sett().GetWebBrowserPath().c_str() );
      customMessageBoxNoModal(SL_MAIN_ICON, _("Couldn't launch browser. URL is: ") + url + _("\nBroser path is: ") + sett().GetWebBrowserPath(), _("Couldn't launch browser.")  );
    }

  }
}


//! @brief Display a dialog asking a question with OK and Canel buttons
//!
//! @return true if OK button was pressed
//! @note this does not return until the user pressed any of the buttons or closed the dialog.
bool Ui::Ask( const wxString& heading, const wxString& question )
{
  wxMessageDialog ask_dlg( &mw(), question, heading, wxYES_NO );
  return ( ask_dlg.ShowModal() == wxID_YES );
}


bool Ui::AskPassword( const wxString& heading, const wxString& message, wxString& password )
{
  wxPasswordEntryDialog pw_dlg( &mw(), message, heading, password );
  int res = pw_dlg.ShowModal();
  password = pw_dlg.GetValue();
  return ( res == wxID_OK );
}


bool Ui::AskText( const wxString& heading, const wxString& question, wxString& answer )
{
  wxTextEntryDialog name_dlg( &mw(), question, heading, answer, wxOK | wxCANCEL | wxCENTRE );
  int res = name_dlg.ShowModal();
  answer = name_dlg.GetValue();

  return ( res == wxID_OK );
}


void Ui::ShowMessage( const wxString& heading, const wxString& message )
{

  if ( m_main_win == 0 ) return;
  serverMessageBox( SL_MAIN_ICON, message, heading, wxOK);

}


bool Ui::ExecuteSayCommand( const wxString& cmd )
{

  if ( !IsConnected() ) return false;
  //TODO insert logic for joining multiple channels at once
  //or remove that from "/help"
  if ( (cmd.BeforeFirst(' ').Lower() == _T("/join")) || (cmd.BeforeFirst(' ').Lower() == _T("/j")) ) {
    wxString channel = cmd.AfterFirst(' ');
    wxString pass = channel.AfterFirst(' ');
    if ( !pass.IsEmpty() ) channel = channel.BeforeFirst(' ');
    if ( channel.StartsWith(_T("#")) ) channel.Remove( 0, 1 );
    m_serv->JoinChannel( channel, pass );
    return true;
  } else if ( cmd.BeforeFirst(' ').Lower() == _T("/away") ) {
    m_serv->GetMe().Status().away = true;
    m_serv->GetMe().SendMyUserStatus();
    return true;
  } else if ( cmd.BeforeFirst(' ').Lower() == _T("/back") ) {
    if ( IsConnected() ) {
      m_serv->GetMe().Status().away = false;
      m_serv->GetMe().SendMyUserStatus();
      return true;
    }
  } else if ( cmd.BeforeFirst(' ').Lower() == _T("/ingame") ) {
    m_serv->RequestInGameTime( _T("") );
    return true;
  } else if ( cmd.BeforeFirst(' ').Lower() == _T("/help") ) {
    wxString topic = cmd.AfterFirst(' ');
    ConsoleHelp( topic.Lower() );
    return true;
  } else if ( cmd.BeforeFirst(' ').Lower() == _T("/msg") ) {
    wxString user = cmd.AfterFirst(' ').BeforeFirst(' ');
    wxString msg = cmd.AfterFirst(' ').AfterFirst(' ');
    m_serv->SayPrivate( user, msg );
    return true;
  } else if ( cmd.BeforeFirst(' ').Lower() == _T("/channels") ) {
    ChatPanel* panel = GetActiveChatPanel();
    if ( panel == 0 ) {
      ShowMessage( _("error"), _("no active chat panels open.") );
      return false;
    }
    panel->StatusMessage(_("Active chat channels:"));
    m_serv->RequestChannels();
    return true;
  }
  return false;
}


void Ui::ConsoleHelp( const wxString& topic )
{
  ChatPanel* panel = GetActiveChatPanel();
  if ( panel == 0 ) {
    ShowMessage( _("Help error"), _("Type /help in a chat box.") );
    return;
  }
  if ( topic == wxEmptyString ) {
    panel->ClientMessage( _("SpringLobby commands help.") );
    panel->ClientMessage( _T("") );
    panel->ClientMessage( _("Global commands:") );
    panel->ClientMessage( _("  \"/away\" - Sets your status to away.") );
    panel->ClientMessage( _("  \"/back\" - Resets your away status.") );
    panel->ClientMessage( _("  \"/changepassword oldpassword newpassword\" - Changes the current active account's password.") );
    panel->ClientMessage( _("  \"/channels\" - Lists currently active channels.") );
    panel->ClientMessage( _("  \"/help [topic]\" - Put topic if you want to know more specific information about a command.") );
    panel->ClientMessage( _("  \"/join channel [password] [,channel2 [password2]]\" - Join a channel.") );
    panel->ClientMessage( _("  \"/j\" - Alias to /join.") );
    panel->ClientMessage( _("  \"/ingame\" - Show how much time you have in game.") );
    panel->ClientMessage( _("  \"/msg username [text]\" - sends a private message containing text to username.") );
    panel->ClientMessage( _("  \"/rename newalias\" - Changes your nickname to newalias.") );
    panel->ClientMessage( _("  \"/sayver\" - Say what version of springlobby you have in chat.") );
    panel->ClientMessage( _("  \"/testmd5 text\" - Returns md5-b64 hash of given text.") );
    panel->ClientMessage( _("  \"/ver\" - Display what version of SpringLobby you have.") );
    panel->ClientMessage( _T("") );
    panel->ClientMessage( _("Chat commands:") );
    panel->ClientMessage( _("  \"/me action\" - Say IRC style action message.") );
    panel->ClientMessage( _T("") );
    panel->ClientMessage( _("If you are missing any commands, go to #springlobby and try to type it there :)") );
//    panel->ClientMessage( _("  \"/\" - .") );
  } else if ( topic == _T("topics") ) {
    panel->ClientMessage( _("No topics written yet.") );
  } else {
    panel->ClientMessage( _("The topic \"") + topic + _("\" was not found. Type \"/help topics\" only for available topics.") );
  }
}


ChatPanel* Ui::GetChannelChatPanel( const wxString& channel )
{
  return mw().GetChannelChatPanel( channel );
}


////////////////////////////////////////////////////////////////////////////////////////////
// EVENTS
////////////////////////////////////////////////////////////////////////////////////////////


void Ui::OnUpdate( int mselapsed )
{
  if ( m_serv != 0 ) {
    m_serv->Update( mselapsed );
  }
  #ifndef NO_TORRENT_SYSTEM
  if (m_upd_intv_counter % 20 == 0 )
  {
      m_main_win->GetTorrentTab().OnUpdate();

  }
  torrent()->UpdateFromTimer( mselapsed );
  m_upd_intv_counter++;
  #endif
}


//! @brief Called when connected to a server
//!
//! @todo Display in servertab
void Ui::OnConnected( Server& server, const wxString& server_name, const wxString& server_ver, bool supported )
{
  wxLogDebugFunc( _T("") );

  if ( !IsSpringCompatible () ){
    if ( m_spring->TestSpringBinary() ) {
      wxString message = _("Your spring version");
      message += _T(" (") + usync()->GetSpringVersion() + _T(") ");
      message +=  _("is not supported by the lobby server that requires version");
      message += _T(" (") +  m_serv->GetRequiredSpring() + _T(").\n\n");
      message += _("Online play will be disabled.");
      wxLogWarning ( _T("server not supports current spring version") );
      customMessageBox (SL_MAIN_ICON, message, _("Spring error"), wxICON_EXCLAMATION|wxOK );
    } else {
      wxLogWarning( _T("can't get spring version from unitsync") );
      customMessageBox(SL_MAIN_ICON,  _("Couldn't get your spring version from the unitsync library.\n\nOnline play will be disabled."), _("Spring error"), wxICON_EXCLAMATION|wxOK );
    }
  }
  server.uidata.panel->StatusMessage( _T("Connected to ") + server_name + _T(".") );

  //server.uidata.panel = m_main_win->GetChatTab().AddChatPannel( server, server_name );
}


bool Ui::IsSpringCompatible( )
{
  if ( sett().GetDisableSpringVersionCheck() ) return true;
  if ( !m_spring->TestSpringBinary() ) return false;
  if ( m_serv->GetRequiredSpring() == _T("*") ) return true; // Server accepts any version.
  if ( (usync()->GetSpringVersion() == m_serv->GetRequiredSpring() ) && !m_serv->GetRequiredSpring().IsEmpty() ) return true;
  else return false;
}


void Ui::OnLoggedIn( )
{
  if ( m_main_win == 0 ) return;
  mw().GetChatTab().RejoinChannels();
}


void Ui::OnDisconnected( Server& server )
{
  if ( m_main_win == 0 ) return;
  wxLogDebugFunc( _T("") );
  if ( m_main_win == 0 ) return;

  mw().GetJoinTab().GetBattleListTab().SetFilterActiv( false );
  mw().GetJoinTab().LeaveCurrentBattle();
  mw().GetJoinTab().GetBattleListTab().RemoveAllBattles();

  if ( server.uidata.panel != 0 ) {
    server.uidata.panel->StatusMessage( _T("Disconnected from server.") );
    server.uidata.panel->SetServer( 0 );
    server.uidata.panel = 0;
  }
  mw().GetChatTab().CloseAllChats();

}


//! @brief Called when client has joined a channel
//!
//! @todo Check if a pannel allready exists for this channel
void Ui::OnJoinedChannelSuccessful( Channel& chan )
{
  if ( m_main_win == 0 ) return;
  wxLogDebugFunc( _T("") );

  chan.uidata.panel = 0;
  m_main_win->OpenChannelChat( chan );
  if ( chan.GetName() == _T("springlobby") ) {
    chan.uidata.panel->ClientMessage( wxEmptyString );
    chan.uidata.panel->ClientMessage( _("This is the SpringLobby channel, please report any problems you are having with SpringLobby here and the friendly developers will help you.") );
    chan.uidata.panel->ClientMessage( wxEmptyString );
  }
}


//! @brief Called when something is said in a channel
void Ui::OnChannelSaid( Channel& channel, User& user, const wxString& message )
{
  wxLogDebugFunc( _T("") );
  if ( channel.uidata.panel == 0 ) {
    wxLogError( _T("ud->panel NULL") );
    return;
  }
  channel.uidata.panel->Said( user.GetNick(), message );
}


void Ui::OnChannelDidAction( Channel& channel , User& user, const wxString& action )
{
  wxLogDebugFunc( _T("") );
  if ( channel.uidata.panel == 0 ) {
    wxLogError( _T("ud->panel NULL") );
    return;
  }
  channel.uidata.panel->DidAction( user.GetNick(), action );
}


void Ui::OnChannelMessage( const wxString& channel, const wxString& msg )
{
  ChatPanel* panel = GetChannelChatPanel( channel );
  if ( panel != 0 ) {
    panel->StatusMessage( msg );
  }
}


//! @brief Called when client is leaving a channel
//!
//! @todo Tell ChatPanel the channel is no longer joined
void Ui::OnLeaveChannel( Channel& channel )
{

}


void Ui::OnUserJoinedChannel( Channel& chan, User& user )
{
  //wxLogDebugFunc( _T("") );
  if ( chan.uidata.panel == 0 ) {
    wxLogError( _T("ud->panel NULL") );
    return;
  }
  chan.uidata.panel->Joined( user );
}


void Ui::OnChannelJoin( Channel& chan, User& user )
{
  //wxLogDebugFunc( _T("") );
  if ( chan.uidata.panel == 0 ) {
    wxLogError( _T("ud->panel NULL") );
    return;
  }
  chan.uidata.panel->OnChannelJoin( user );
}


void Ui::OnUserLeftChannel( Channel& chan, User& user, const wxString& reason )
{
  //wxLogDebugFunc( _T("") );
  if ( chan.uidata.panel == 0 ) {
    wxLogError( _T("ud->panel NULL") );
    return;
  }
  chan.uidata.panel->Parted( user, reason );
}


void Ui::OnChannelTopic( Channel& channel, const wxString& user, const wxString& topic )
{
  wxLogDebugFunc( _T("") );
  if ( channel.uidata.panel == 0 ) {
    wxLogError( _T("ud->panel NULL") );
    return;
  }
  channel.uidata.panel->SetTopic( user, topic );
}


void Ui::OnChannelList( const wxString& channel, const int& numusers )
{
  ChatPanel* panel = GetActiveChatPanel();
  if ( panel == 0 ) {
    ShowMessage( _("error"), _("no active chat panels open.") );
    return;
  }
  panel->StatusMessage( channel + wxString::Format( _("(%d users)"), numusers) );
}


void Ui::OnUserOnline( User& user )
{
  if ( m_main_win == 0 ) return;
/*  UiUserData* data = new UiUserData();
  data->panel = 0;

  user.SetUserData( (void*)data );*/

  m_main_win->GetChatTab().OnUserConnected( user );
}


void Ui::OnUserOffline( User& user )
{
  if ( m_main_win == 0 ) return;
  m_main_win->GetChatTab().OnUserDisconnected( user );
  if ( user.uidata.panel ) {
    user.uidata.panel->SetUser( 0 );
    user.uidata.panel = 0;
  }
/*  UiUserData* data = (UiUserData*)user.GetUserData();
  if ( data == 0) return;

  delete data;*/
}


void Ui::OnUserStatusChanged( User& user )
{
  if ( m_main_win == 0 ) return;
  for ( int i = 0; i < m_serv->GetNumChannels(); i++ ) {
    Channel& chan = m_serv->GetChannel( i );
    if ( ( chan.UserExists(user.GetNick()) ) && ( chan.uidata.panel != 0 ) ) {
      chan.uidata.panel->UserStatusUpdated( user );
    }
  }
  if ( user.GetStatus().in_game ) mw().GetJoinTab().GetBattleListTab().UserUpdate( user );
}


void Ui::OnUnknownCommand( Server& server, const wxString& command, const wxString& params )
{
  if ( server.uidata.panel != 0 ) server.uidata.panel->UnknownCommand( command, params );
}


void Ui::OnMotd( Server& server, const wxString& message )
{
  if ( server.uidata.panel != 0 ) server.uidata.panel->Motd( message );
}


void Ui::OnServerMessage( Server& server, const wxString& message )
{
  ChatPanel* panel = GetActiveChatPanel();
  if ( panel != 0 )
  {
    panel->StatusMessage( message );
  } else {
    ShowMessage( _("Server message"), message );
  }
}


void Ui::OnUserSaid( User& user, const wxString& message, bool fromme )
{
  if ( m_main_win == 0 ) return;
  if ( user.uidata.panel == 0 ) {
    m_main_win->OpenPrivateChat( user );
  }
  if ( fromme ) user.uidata.panel->Said( m_serv->GetMe().GetNick(), message );
  else user.uidata.panel->Said( user.GetNick(), message );
}


void Ui::OnBattleOpened( Battle& battle )
{
  if ( m_main_win == 0 ) return;
  mw().GetJoinTab().GetBattleListTab().AddBattle( battle );
  User& user = battle.GetFounder();
  for ( int i = 0; i < m_serv->GetNumChannels(); i++ ) {
    Channel& chan = m_serv->GetChannel( i );
    if ( ( chan.UserExists(user.GetNick()) ) && ( chan.uidata.panel != 0 ) ) {
      chan.uidata.panel->UserStatusUpdated( user );
    }
  }
}


void Ui::OnBattleClosed( Battle& battle )
{
  if ( m_main_win == 0 ) return;
  mw().GetJoinTab().GetBattleListTab().RemoveBattle( battle );
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    if ( &br->GetBattle() == &battle )
	{
	    if (!battle.IsFounderMe() )
            customMessageBoxNoModal(SL_MAIN_ICON,_("The current battle was closed by the host."),_("Battle closed"));
		mw().GetJoinTab().LeaveCurrentBattle();
	}
  }
  for ( unsigned int b = 0; b < battle.GetNumUsers(); b++ ) {
    User& user = battle.GetUser( b );
    user.SetBattle(0);
    for ( int i = 0; i < m_serv->GetNumChannels(); i++ ) {
      Channel& chan = m_serv->GetChannel( i );
      if ( ( chan.UserExists(user.GetNick()) ) && ( chan.uidata.panel != 0 ) ) {
        chan.uidata.panel->UserStatusUpdated( user );
      }
    }
  }
}


void Ui::OnUserJoinedBattle( Battle& battle, User& user )
{
  if ( m_main_win == 0 ) return;
  mw().GetJoinTab().GetBattleListTab().UpdateBattle( battle );

  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    if ( &br->GetBattle() == &battle ) br->OnUserJoined( user );
  }
  for ( int i = 0; i < m_serv->GetNumChannels(); i++ ) {
    Channel& chan = m_serv->GetChannel( i );
    if ( ( chan.UserExists(user.GetNick()) ) && ( chan.uidata.panel != 0 ) ) {
      chan.uidata.panel->UserStatusUpdated( user );
    }
  }
}


void Ui::OnUserLeftBattle( Battle& battle, User& user )
{
  if ( m_main_win == 0 ) return;
  mw().GetJoinTab().GetBattleListTab().UpdateBattle( battle );
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    if ( &br->GetBattle() == &battle ) {
      br->OnUserLeft( user );
      if ( &user == &m_serv->GetMe() ) mw().GetJoinTab().LeaveCurrentBattle();
    }
  }
  for ( int i = 0; i < m_serv->GetNumChannels(); i++ ) {
    Channel& chan = m_serv->GetChannel( i );
    if ( ( chan.UserExists(user.GetNick()) ) && ( chan.uidata.panel != 0 ) ) {
      chan.uidata.panel->UserStatusUpdated( user );
    }
  }
}

void Ui::OnBattleMapRefresh()
{
    if ( m_main_win == 0 ) return;
    BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
    if ( br != 0 ) br->UpdateBattleInfo( true, false );
}

void Ui::OnBattleInfoUpdated( Battle& battle )
{
  if ( m_main_win == 0 ) return;
  m_main_win->GetJoinTab().GetBattleListTab().UpdateBattle( battle );
  if ( m_main_win->GetJoinTab().GetCurrentBattle() == &battle ) {
    mw().GetJoinTab().UpdateCurrentBattle();
  }
}

void Ui::OnBattleInfoUpdated( Battle& battle, const wxString& Tag )
{
  if ( m_main_win == 0 ) return;
  m_main_win->GetJoinTab().GetBattleListTab().UpdateBattle( battle );
  if ( m_main_win->GetJoinTab().GetCurrentBattle() == &battle ) {
    mw().GetJoinTab().UpdateCurrentBattle( Tag );
  }
}


void Ui::OnJoinedBattle( Battle& battle )
{
  if ( m_main_win == 0 ) return;
  mw().GetJoinTab().JoinBattle( battle );
  if ( !Spring::TestSpringBinary() ) {
    customMessageBox(SL_MAIN_ICON, _("Your spring settings are probably not configured correctly,\nyou should take another look at your settings before trying\nto play online."), _("Spring settings error"), wxOK );
  }
  if ( battle.GetNatType() != NAT_None ) {
    wxLogWarning( _T("joining game with NAT transversal") );
#ifdef HAVE_WX26
    customMessageBox(SL_MAIN_ICON, _("This game uses NAT traversal that is not supported by wx 2.6 build of springlobby. \n\nYou will not be able to play in this battle. \nUpdate your wxwidgets to 2.8 or newer to enable NAT traversal support."), _("NAT traversal"), wxOK );
#endif
  }
}


void Ui::OnHostedBattle( Battle& battle )
{
  if ( m_main_win == 0 ) return;
  mw().GetJoinTab().HostBattle( battle );
}


void Ui::OnUserBattleStatus( Battle& battle, User& user )
{
  if ( m_main_win == 0 ) return;
  mw().GetJoinTab().BattleUserUpdated( user );
}


void Ui::OnRequestBattleStatus( Battle& battle )
{
  if ( m_main_win == 0 ) return;
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    if ( &br->GetBattle() == &battle ) {
      br->GetBattle().OnRequestBattleStatus();
    }
  }
}


void Ui::OnBattleStarted( Battle& battle )
{
  if ( m_main_win == 0 ) return;
  wxLogDebugFunc( _T("") );
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    if ( &br->GetBattle() == &battle ) {
      battle.GetMe().BattleStatus().ready = false;
      battle.SendMyBattleStatus();
      battle.GetMe().Status().in_game = true;
      battle.GetMe().SendMyUserStatus();
      #ifndef NO_TORRENT_SYSTEM
      torrent()->SetIngameStatus(true);
      #endif
      m_spring->Run( battle );
    }
  }
  mw().GetJoinTab().GetBattleListTab().UpdateBattle( battle );
}


void Ui::OnSaidBattle( Battle& battle, const wxString& nick, const wxString& msg )
{
  if ( m_main_win == 0 ) return;
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    br->GetChatPanel().Said( nick, msg );
  }
}


void Ui::OnBattleAction( Battle& battle, const wxString& nick, const wxString& msg )
{
  if ( m_main_win == 0 ) return;
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    br->GetChatPanel().DidAction( nick, msg );
  }
}


void Ui::OnSpringTerminated( bool success )
{
  #ifndef NO_TORRENT_SYSTEM
  torrent()->SetIngameStatus(false);
  #endif
  if ( !m_serv ) return;

  m_serv->GetMe().Status().in_game = false;
  m_serv->GetMe().SendMyUserStatus();
}


void Ui::OnBattleStartRectsUpdated( Battle& battle )
{
  if ( m_main_win == 0 ) return;
  mw().GetJoinTab().UpdateCurrentBattle( false, true );
}


void Ui::OnBattleMapChanged( Battle& battle )
{
  if ( m_main_win == 0 ) return;
  mw().GetJoinTab().UpdateCurrentBattle( true );
  if (battle.IsFounderMe())
  {
	  battle.CustomBattleOptions()->loadMapOptions(battle.GetMapName());
	  mw().GetJoinTab().ReloadMMoptTab();
  }
}


void Ui::OnBattleDisableUnit( Battle& battle, const wxString& unitname )
{
  if ( m_main_win == 0 ) return;
  mw().GetJoinTab().UpdateCurrentBattle( false, true );
}


void Ui::OnBattleEnableUnit( Battle& battle, const wxString& unitname )
{
  if ( m_main_win == 0 ) return;
  mw().GetJoinTab().UpdateCurrentBattle( false, true );
}


void Ui::OnBattleEnableAllUnits( Battle& battle )
{
  if ( m_main_win == 0 ) return;
  mw().GetJoinTab().UpdateCurrentBattle( false, true );
}


void Ui::OnAcceptAgreement( const wxString& agreement )
{
  AgreementDialog dlg( m_main_win, agreement );
  if ( dlg.ShowModal() == 1 ) {
    m_serv->AcceptAgreement();
    m_serv->Login();
  }
}


void Ui::OnBattleBotAdded( Battle& battle, BattleBot& bot )
{
  if ( m_main_win == 0 ) return;
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    if ( &br->GetBattle() == &battle ) br->OnBotAdded( bot );
  }
}


void Ui::OnBattleBotRemoved( Battle& battle, BattleBot& bot )
{
  if ( m_main_win == 0 ) return;
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    if ( &br->GetBattle() == &battle ) br->OnBotRemoved( bot );
  }
}


void Ui::OnBattleBotUpdated( Battle& battle, BattleBot& bot )
{
  if ( m_main_win == 0 ) return;
  BattleRoomTab* br = mw().GetJoinTab().GetBattleRoomTab();
  if ( br != 0 ) {
    if ( &br->GetBattle() == &battle ) br->OnBotUpdated( bot );
  }
}


void Ui::OnRing( const wxString& from )
{
  if ( m_main_win == 0 ) return;
  m_main_win->RequestUserAttention();
  wxBell();
}


void Ui::OnMapInfoCached( const wxString& mapname )
{
  if ( m_main_win == 0 ) return;
  mw().OnUnitSyncReloaded();
}


void Ui::OnMinimapCached( const wxString& mapname )
{
  if ( m_main_win == 0 ) return;
  mw().OnUnitSyncReloaded();
}


void Ui::OnModUnitsCached( const wxString& modname )
{
}


void Ui::OnCachedThreadStarted()
{
  m_thread_wait.Enter();
}


void Ui::OnCachedThreadTerminated()
{
  m_thread_wait.Leave();
}

void Ui::OnMainWindowDestruct()
{
	m_main_win = 0;
}

bool Ui::IsThisMe(User& other)
{
	//if i'm not connected i have no identity
	if (!IsConnected() || m_serv==0)
		return false;
	else
		return ( other.GetNick()==m_serv->GetMe().GetNick() );
}

int Ui::TestHostPort( unsigned int port )
{
  return m_serv->TestOpenPort( port );
}
