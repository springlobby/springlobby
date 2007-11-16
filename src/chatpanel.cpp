/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: ChatPanel
//

#include <stdexcept>
#include <wx/intl.h>
#include <wx/datetime.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/tokenzr.h>
#include <wx/msgdlg.h>
#include <wx/menu.h>
#include <wx/log.h>
#include <wx/utils.h>

#include "channel.h"
#include "chatpanel.h"
#include "utils.h"
#include "ui.h"
#include "server.h"
#include "user.h"
#include "battle.h"
#include "nicklistctrl.h"
#include "mainwindow.h"
#include "chatlog.h"
#include "settings.h"


BEGIN_EVENT_TABLE(ChatPanel, wxPanel)

  EVT_TEXT_ENTER  ( CHAT_TEXT, ChatPanel::OnSay   )
  EVT_BUTTON      ( CHAT_SEND, ChatPanel::OnSay   )
  EVT_SIZE        (            ChatPanel::OnResize)
  EVT_TEXT_URL    ( CHAT_LOG,  ChatPanel::OnLinkEvent )

  EVT_MENU        ( CHAT_MENU_CH_LEAVE, ChatPanel::OnChannelMenuLeave )
  EVT_MENU        ( CHAT_MENU_CH_AUTOJOIN, ChatPanel::OnChannelAutoJoin )
  EVT_MENU        ( CHAT_MENU_CH_INFO, ChatPanel::OnChannelMenuInfo )
  EVT_MENU        ( CHAT_MENU_CH_TOPIC, ChatPanel::OnChannelMenuTopic )
  EVT_MENU        ( CHAT_MENU_CH_MSG, ChatPanel::OnChannelMenuMessage )
  EVT_MENU        ( CHAT_MENU_CH_LOCK, ChatPanel::OnChannelMenuLock )
  EVT_MENU        ( CHAT_MENU_CH_UNLOCK, ChatPanel::OnChannelMenuUnlock )
  EVT_MENU        ( CHAT_MENU_CH_REG, ChatPanel::OnChannelMenuRegister )
  EVT_MENU        ( CHAT_MENU_CH_UNREG, ChatPanel::OnChannelMenuUnregister )
  EVT_MENU        ( CHAT_MENU_CH_SPAM_ON, ChatPanel::OnChannelMenuSpamOn )
  EVT_MENU        ( CHAT_MENU_CH_SPAM_OFF, ChatPanel::OnChannelMenuSpanOff )
  EVT_MENU        ( CHAT_MENU_CH_SPAM_ISON, ChatPanel::OnChannelMenuSpamIsOn )
  EVT_MENU        ( wxID_COPY, ChatPanel::OnMenuCopy )
  EVT_MENU        ( wxID_SELECTALL, ChatPanel::OnMenuSelectAll )

  EVT_MENU        ( CHAT_MENU_SV_DISCON, ChatPanel::OnServerMenuDisconnect )
  EVT_MENU        ( CHAT_MENU_SV_RECON, ChatPanel::OnServerMenuReconnect )
  EVT_MENU        ( CHAT_MENU_SV_REMOVE, ChatPanel::OnServerMenuRemove )
  EVT_MENU        ( CHAT_MENU_SV_CHPWD, ChatPanel::OnServerMenuChangePassword )
  EVT_MENU        ( CHAT_MENU_SV_ACCESS, ChatPanel::OnServerMenuSetAccess )
  EVT_MENU        ( CHAT_MENU_SV_BROADCAST, ChatPanel::OnServerMenuBroadcast )

  EVT_MENU        ( CHAT_MENU_US_CHAT, ChatPanel::OnUserMenuOpenChat )
  EVT_MENU        ( CHAT_MENU_US_JOIN, ChatPanel::OnUserMenuJoinSame )
  EVT_MENU        ( CHAT_MENU_US_SLAP, ChatPanel::OnUserMenuSlap )
  EVT_MENU        ( CHAT_MENU_US_MUTE, ChatPanel::OnUserMenuMute )
  EVT_MENU        ( CHAT_MENU_US_UNMUTE, ChatPanel::OnUserMenuUnmute )
  EVT_MENU        ( CHAT_MENU_US_KICK, ChatPanel::OnUserMenuKick )
  EVT_MENU        ( CHAT_MENU_US_OP, ChatPanel::OnUserMenuOp )
  EVT_MENU        ( CHAT_MENU_US_DEOP, ChatPanel::OnUserMenuDeop )
  EVT_MENU        ( CHAT_MENU_US_ADMIN_INGAME, ChatPanel::OnUserMenuAdminIngame )
  EVT_MENU        ( CHAT_MENU_US_ADMIN_LASTLOGIN, ChatPanel::OnUserMenuAdminLastLogin )
  EVT_MENU        ( CHAT_MENU_US_ADMIN_CURIP, ChatPanel::OnUserMenuAdminCurrentIP )
  EVT_MENU        ( CHAT_MENU_US_ADMIN_FINDIP, ChatPanel::OnUserMenuAdminFindIP )
  EVT_MENU        ( CHAT_MENU_US_ADMIN_KICK, ChatPanel::OnUserMenuAdminKick )
  EVT_MENU        ( CHAT_MENU_US_ADMIN_BAN, ChatPanel::OnUserMenuAdminBan )
  EVT_MENU        ( CHAT_MENU_US_ADMIN_UNBAN, ChatPanel::OnUserMenuAdminUnban )
  EVT_MENU        ( CHAT_MENU_US_ADMIN_MUTE, ChatPanel::OnUserMenuAdminMute )
  EVT_MENU        ( CHAT_MENU_US_ADMIN_UNMUTE, ChatPanel::OnUserMenuAdminUnmute )
  EVT_MENU        ( CHAT_MENU_US_ADMIN_RING, ChatPanel::OnUserMenuAdminRing )

END_EVENT_TABLE()


void ChatPanel::OnMouseDown( wxMouseEvent& event )
{
  wxLogDebugFunc( _T("") );
  _CreatePopup();
  if ( m_popup_menu != 0 ) PopupMenu( m_popup_menu );
  else event.Skip();
}


ChatPanel::ChatPanel( wxWindow* parent, Ui& ui, Channel& chan )
: wxPanel( parent, -1),m_show_nick_list(true),m_ui(ui),m_channel(&chan),m_server(0),m_user(0),m_battle(0),m_type(CPT_Channel),m_popup_menu(0)
{
  wxLogDebugFunc( _T("wxWindow* parent, Channel& chan") );
  _CreateControls( );
  _SetChannel( &chan );
  m_chatlog_text->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler(ChatPanel::OnMouseDown), 0, this );
  m_chat_log = new ChatLog(WX_STRING(sett().GetDefaultServer()),WX_STRING(chan.GetName()));
}


ChatPanel::ChatPanel( wxWindow* parent, Ui& ui, User& user )
: wxPanel( parent, -1),m_show_nick_list(false),m_ui(ui),m_channel(0),m_server(0),m_user(&user),m_battle(0),m_type(CPT_User),m_popup_menu(0)
{
  _CreateControls( );
  user.uidata.panel = this;
  m_chat_log = new ChatLog(WX_STRING(sett().GetDefaultServer()),WX_STRING(user.GetNick()));
}


ChatPanel::ChatPanel( wxWindow* parent, Ui& ui, Server& serv )
: wxPanel( parent, -1),m_show_nick_list(false),m_ui(ui),m_channel(0),m_server(&serv),m_user(0),m_battle(0),m_type(CPT_Server),m_popup_menu(0)
{
  wxLogDebugFunc( _T("wxWindow* parent, Server& serv") );
  _CreateControls( );
  serv.uidata.panel = this;
  m_chat_log = new ChatLog(WX_STRING(sett().GetDefaultServer()),_("_SERVER"));
  m_chatlog_text->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler(ChatPanel::OnMouseDown), 0, this );
}


ChatPanel::ChatPanel( wxWindow* parent, Ui& ui, Battle& battle )
: wxPanel( parent, -1),m_show_nick_list(false),m_ui(ui),m_channel(0),m_server(0),m_user(0),m_battle(&battle),m_type(CPT_Battle),m_popup_menu(0)
{
  wxLogDebugFunc( _T("wxWindow* parent, Battle& battle") );
  _CreateControls( );
  wxDateTime now = wxDateTime::Now();
  m_chat_log = new ChatLog(WX_STRING(sett().GetDefaultServer()),_("_BATTLE_")+WX_STRING(now.Format( _T("%Y_%m_%d__%H_%M_%S"))));
}


//! @brief ChatPanel destructor.
ChatPanel::~ChatPanel()
{
  if ( m_server != 0 ) {
    if ( m_server->uidata.panel == this ) m_server->uidata.panel = 0;
  }
  if ( m_user != 0 ) {
    if ( m_user->uidata.panel == this ) m_user->uidata.panel = 0;
  }
  if ( m_channel != 0 ) {
    if ( m_channel->uidata.panel == this ) m_channel->uidata.panel = 0;
  }
  delete m_chat_log;

  if ( m_type == CPT_Channel ) m_chatlog_text->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler(ChatPanel::OnMouseDown), 0, 0 );
  if ( m_type == CPT_Server ) m_chatlog_text->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler(ChatPanel::OnMouseDown), 0, 0 );
}


void ChatPanel::_CreateControls( )
{
  wxLogDebugFunc( _T("") );

  m_autorejoin = 0;
  // Creating sizers
  m_main_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_chat_sizer = new wxBoxSizer( wxVERTICAL );
  m_say_sizer = new wxBoxSizer( wxHORIZONTAL );

  if ( m_show_nick_list ) {

    m_splitter = new wxSplitterWindow( this, -1, wxDefaultPosition, wxDefaultSize, wxSP_3D );
    m_nick_panel = new wxPanel( m_splitter, -1 );
    m_chat_panel = new wxPanel( m_splitter, -1 );

    m_nick_sizer = new wxBoxSizer( wxVERTICAL );

    m_nicklist = new NickListCtrl( m_nick_panel, m_ui, true, _CreateNickListMenu() );

    m_nick_filter = new wxComboBox( m_nick_panel, -1, _T("Show all"), wxDefaultPosition, wxSize(80,CONTROL_HEIGHT), 0, 0, wxCB_READONLY );
    m_nick_filter->Disable();

    m_nick_sizer->Add( m_nicklist, 1, wxEXPAND );
    m_nick_sizer->Add( m_nick_filter, 0, wxEXPAND | wxTOP, 2 );

    m_nick_panel->SetSizer( m_nick_sizer );

  } else {

    m_chat_panel = this;
    m_nick_sizer = 0;
    m_nicklist = 0;
    m_nick_filter = 0;
    m_splitter = 0;

  }

  // Creating ui elements
  m_chatlog_text = new wxTextCtrl( m_chat_panel, CHAT_LOG, _T(""), wxDefaultPosition, wxDefaultSize,
                             wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_AUTO_URL );

  m_say_text = new wxTextCtrl( m_chat_panel, CHAT_TEXT, _T(""), wxDefaultPosition, wxSize(100,CONTROL_HEIGHT), wxTE_PROCESS_ENTER );
  m_say_button = new wxButton( m_chat_panel, CHAT_SEND, _("Send"), wxDefaultPosition, wxSize(80,CONTROL_HEIGHT) );

  // Adding elements to sizers
  m_say_sizer->Add( m_say_text, 1, wxEXPAND );
  m_say_sizer->Add( m_say_button );

  m_chat_sizer->Add( m_chatlog_text, 1, wxEXPAND );
  m_chat_sizer->Add( m_say_sizer, 0, wxEXPAND | wxTOP, 2 );

  if ( m_show_nick_list ) {
    m_chat_panel->SetSizer( m_chat_sizer );

    m_splitter->SplitVertically( m_chat_panel, m_nick_panel, 100 );

    m_main_sizer->Add( m_splitter, 1, wxEXPAND | wxALL, 2 );

  } else {
    m_main_sizer->Add( m_chat_sizer, 4, wxEXPAND | wxALL, 2 );
  }

  // Assign sizer to panel
  SetSizer( m_main_sizer );

  if ( m_show_nick_list ) {
    wxSize s = m_splitter->GetSize();
    m_splitter->SetSashPosition( s.GetWidth() - 238, true );
  }
}


void ChatPanel::_CreatePopup()
{
  if ( m_popup_menu != 0 ) return;
  wxLogDebugFunc( _T("") );
  if ( m_type == CPT_Channel ) {

    wxLogDebug(_T("channel"));
    m_popup_menu = new wxMenu();
    m_autorejoin = new wxMenuItem( m_popup_menu, CHAT_MENU_CH_AUTOJOIN, _("Auto join this channel"), wxEmptyString, wxITEM_CHECK );
    m_popup_menu->Append( m_autorejoin );
    if ( m_channel->GetName() != "springlobby" ) {
      bool isautojoin = sett().GetChannelJoinIndex( m_channel->GetName() ) >= 0;
      m_autorejoin->Check( isautojoin );
    } else {
      m_autorejoin->Check( true );
      m_autorejoin->Enable( false );
    }

    wxMenuItem* leaveitem = new wxMenuItem( m_popup_menu, CHAT_MENU_CH_LEAVE, _("Leave"), wxEmptyString, wxITEM_NORMAL );
    m_popup_menu->Append( leaveitem );

    m_popup_menu->AppendSeparator();
    wxMenuItem* selectitem = new wxMenuItem( m_popup_menu, wxID_SELECTALL, _("Select all"), wxEmptyString, wxITEM_NORMAL );
    m_popup_menu->Append( selectitem );
    wxMenuItem* copyitem = new wxMenuItem( m_popup_menu, wxID_COPY, _("Copy"), wxEmptyString, wxITEM_NORMAL );
    m_popup_menu->Append( copyitem );

    m_popup_menu->AppendSeparator();
    wxMenu* m_chanserv;
    m_chanserv = new wxMenu();
    wxMenuItem* infoitem = new wxMenuItem( m_chanserv, CHAT_MENU_CH_INFO, _("Channel info"), wxEmptyString, wxITEM_NORMAL );
    m_chanserv->Append( infoitem );

    m_chanserv->AppendSeparator();
    wxMenuItem* chtopicitem = new wxMenuItem( m_chanserv, CHAT_MENU_CH_TOPIC, _("Set topic..."), wxEmptyString, wxITEM_NORMAL );
    m_chanserv->Append( chtopicitem );
    wxMenuItem* chmessageitem = new wxMenuItem( m_chanserv, CHAT_MENU_CH_MSG, _("Channel message..."), wxEmptyString, wxITEM_NORMAL );
    m_chanserv->Append( chmessageitem );

    m_chanserv->AppendSeparator();
    wxMenuItem* chlockitem = new wxMenuItem( m_chanserv, CHAT_MENU_CH_LOCK, _("Lock..."), wxEmptyString, wxITEM_NORMAL );
    m_chanserv->Append( chlockitem );
    wxMenuItem* chunlockitem = new wxMenuItem( m_chanserv, CHAT_MENU_CH_UNLOCK, _("Unlock"), wxEmptyString, wxITEM_NORMAL );
    m_chanserv->Append( chunlockitem );

    m_chanserv->AppendSeparator();
    wxMenuItem* chregisteritem = new wxMenuItem( m_chanserv, CHAT_MENU_CH_REG, _("Register..."), wxEmptyString, wxITEM_NORMAL );
    m_chanserv->Append( chregisteritem );
    wxMenuItem* chunregisteritem = new wxMenuItem( m_chanserv, CHAT_MENU_CH_UNREG, _("Unregister"), wxEmptyString, wxITEM_NORMAL );
    m_chanserv->Append( chunregisteritem );

    m_chanserv->AppendSeparator();
    wxMenu* m_spam;
    m_spam = new wxMenu();
    wxMenuItem* spamprotonitem = new wxMenuItem( m_spam, CHAT_MENU_CH_SPAM_ON, _("On"), wxEmptyString, wxITEM_NORMAL );
    m_spam->Append( spamprotonitem );
    wxMenuItem* spamprotoffitem = new wxMenuItem( m_spam, CHAT_MENU_CH_SPAM_OFF, _("Off"), wxEmptyString, wxITEM_NORMAL );
    m_spam->Append( spamprotoffitem );

    m_spam->AppendSeparator();
    wxMenuItem* spamprotisonitem = new wxMenuItem( m_spam, CHAT_MENU_CH_SPAM_ISON, _("Is on?"), wxEmptyString, wxITEM_NORMAL );
    m_spam->Append( spamprotisonitem );
    m_chanserv->Append( -1, _("Spam protection"), m_spam );
    m_popup_menu->Append( -1,_("ChanServ"), m_chanserv );

  } else if ( m_type == CPT_Server ) {

    wxLogDebug( _T("server") );
    m_popup_menu = new wxMenu();

    wxMenuItem* disconnectitem = new wxMenuItem( m_popup_menu, CHAT_MENU_SV_DISCON, _("Disconnect"), wxEmptyString, wxITEM_NORMAL );
    m_popup_menu->Append( disconnectitem );
    wxMenuItem* reconnectitem = new wxMenuItem( m_popup_menu, CHAT_MENU_SV_RECON, _("Reconnect"), wxEmptyString, wxITEM_NORMAL );
    m_popup_menu->Append( reconnectitem );

    m_popup_menu->AppendSeparator();
    wxMenuItem* selectitem = new wxMenuItem( m_popup_menu, wxID_SELECTALL, _("Select all"), wxEmptyString, wxITEM_NORMAL );
    m_popup_menu->Append( selectitem );
    wxMenuItem* copyitem = new wxMenuItem( m_popup_menu, wxID_COPY, _("Copy"), wxEmptyString, wxITEM_NORMAL );
    m_popup_menu->Append( copyitem );

    m_popup_menu->AppendSeparator();
    wxMenu* m_admin;

    m_admin = new wxMenu();
    wxMenu* m_accounts;
    m_accounts = new wxMenu();
    wxMenuItem* removeitem = new wxMenuItem( m_accounts, CHAT_MENU_SV_REMOVE, _("Remove..."), wxEmptyString, wxITEM_NORMAL );
    m_accounts->Append( removeitem );
    wxMenuItem* chpwditem = new wxMenuItem( m_accounts, CHAT_MENU_SV_CHPWD, _("Change password..."), wxEmptyString, wxITEM_NORMAL );
    m_accounts->Append( chpwditem );
    wxMenuItem* setaccessitem = new wxMenuItem( m_accounts, CHAT_MENU_SV_ACCESS, _("Set access..."), wxEmptyString, wxITEM_NORMAL );
    m_accounts->Append( setaccessitem );
    m_admin->Append( -1, wxT("Accounts"), m_accounts );

    m_admin->AppendSeparator();
    wxMenuItem* broadcastitem = new wxMenuItem( m_admin, CHAT_MENU_SV_BROADCAST, _("Broadcast..."), wxEmptyString, wxITEM_NORMAL );
    m_admin->Append( broadcastitem );
    m_popup_menu->Append( -1, wxT("Admin"), m_admin );
  } else {
    m_popup_menu = 0;
  }

}


wxMenu* ChatPanel::_CreateNickListMenu()
{
  wxMenu* m_user_menu;
  m_user_menu = new wxMenu();
  wxMenuItem* chatitem = new wxMenuItem( m_user_menu, CHAT_MENU_US_CHAT, wxString( wxT("Open Chat") ) , wxEmptyString, wxITEM_NORMAL );
  m_user_menu->Append( chatitem );
  wxMenuItem* joinbattleitem = new wxMenuItem( m_user_menu, CHAT_MENU_US_JOIN, wxString( wxT("Join same battle") ) , wxEmptyString, wxITEM_NORMAL );
  m_user_menu->Append( joinbattleitem );

  m_user_menu->AppendSeparator();
  wxMenuItem* slapitem = new wxMenuItem( m_user_menu, CHAT_MENU_US_SLAP, wxString( wxT("Slap!") ) , wxEmptyString, wxITEM_NORMAL );
  m_user_menu->Append( slapitem );

  m_user_menu->AppendSeparator();
  wxMenu* m_chanserv;
  m_chanserv = new wxMenu();
  wxMenuItem* chmuteitem = new wxMenuItem( m_chanserv, CHAT_MENU_US_MUTE, wxString( wxT("Mute...") ) , wxEmptyString, wxITEM_NORMAL );
  m_chanserv->Append( chmuteitem );
  wxMenuItem* chunmuteitem = new wxMenuItem( m_chanserv, CHAT_MENU_US_UNMUTE, wxString( wxT("Unmute") ) , wxEmptyString, wxITEM_NORMAL );
  m_chanserv->Append( chunmuteitem );
  m_chanserv->AppendSeparator();
  wxMenuItem* chkickitem = new wxMenuItem( m_chanserv, CHAT_MENU_US_KICK, wxString( wxT("Kick...") ) , wxEmptyString, wxITEM_NORMAL );
  m_chanserv->Append( chkickitem );

  m_chanserv->AppendSeparator();
  wxMenuItem* chopitem = new wxMenuItem( m_chanserv, CHAT_MENU_US_OP, wxString( wxT("Op") ) , wxEmptyString, wxITEM_NORMAL );
  m_chanserv->Append( chopitem );
  wxMenuItem* chdeopitem = new wxMenuItem( m_chanserv, CHAT_MENU_US_DEOP, wxString( wxT("DeOp") ) , wxEmptyString, wxITEM_NORMAL );
  m_chanserv->Append( chdeopitem );
  m_user_menu->Append( -1, wxT("ChanServ"), m_chanserv );
  wxMenu* m_admin;
  m_admin = new wxMenu();
  wxMenu* m_admin_info;
  m_admin_info = new wxMenu();
  wxMenuItem* admingameitem = new wxMenuItem( m_admin_info, CHAT_MENU_US_ADMIN_INGAME, wxString( wxT("Ingame time") ) , wxEmptyString, wxITEM_NORMAL );
  m_admin_info->Append( admingameitem );
  wxMenuItem* admlastloginitem = new wxMenuItem( m_admin_info, CHAT_MENU_US_ADMIN_LASTLOGIN, wxString( wxT("Last login") ) , wxEmptyString, wxITEM_NORMAL );
  m_admin_info->Append( admlastloginitem );

  m_admin_info->AppendSeparator();
  wxMenuItem* admipitem = new wxMenuItem( m_admin_info, CHAT_MENU_US_ADMIN_CURIP, wxString( wxT("Current IP") ) , wxEmptyString, wxITEM_NORMAL );
  m_admin_info->Append( admipitem );
  wxMenuItem* admfindipitem = new wxMenuItem( m_admin_info, CHAT_MENU_US_ADMIN_FINDIP, wxString( wxT("Find IPs") ) , wxEmptyString, wxITEM_NORMAL );
  m_admin_info->Append( admfindipitem );
  m_admin->Append( -1, wxT("Info"), m_admin_info );

  m_admin->AppendSeparator();
  wxMenuItem* admkickitem = new wxMenuItem( m_admin, CHAT_MENU_US_ADMIN_KICK, wxString( wxT("Kick...") ) , wxEmptyString, wxITEM_NORMAL );
  m_admin->Append( admkickitem );

  m_admin->AppendSeparator();
  wxMenuItem* admbanitem = new wxMenuItem( m_admin, CHAT_MENU_US_ADMIN_BAN, wxString( wxT("Ban...") ) , wxEmptyString, wxITEM_NORMAL );
  m_admin->Append( admbanitem );
  wxMenuItem* admunbanitem = new wxMenuItem( m_admin, CHAT_MENU_US_ADMIN_UNBAN, wxString( wxT("Unban") ) , wxEmptyString, wxITEM_NORMAL );
  m_admin->Append( admunbanitem );

  m_admin->AppendSeparator();
  wxMenuItem* admmuteitem = new wxMenuItem( m_admin, CHAT_MENU_US_ADMIN_MUTE, wxString( wxT("Mute...") ) , wxEmptyString, wxITEM_NORMAL );
  m_admin->Append( admmuteitem );
  wxMenuItem* admunmuteitem = new wxMenuItem( m_admin, CHAT_MENU_US_ADMIN_UNMUTE, wxString( wxT("Unmute") ) , wxEmptyString, wxITEM_NORMAL );
  m_admin->Append( admunmuteitem );

  m_admin->AppendSeparator();
  wxMenuItem* admringitem = new wxMenuItem( m_admin, CHAT_MENU_US_ADMIN_RING, wxString( wxT("Ring") ) , wxEmptyString, wxITEM_NORMAL );
  m_admin->Append( admringitem );
  m_user_menu->Append( -1, wxT("Admin"), m_admin );

  return m_user_menu;
}


User* ChatPanel::_GetSelectedUser()
{
  if ( m_nicklist == 0 ) return 0;

  int item = m_nicklist->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
  if ( item == -1 ) return 0;

  return (User*)m_nicklist->GetItemData( item );
}


void ChatPanel::CheckLength()
{
  if ( m_chatlog_text->GetNumberOfLines() > 1000 ) {
    int end = 0;
    for ( int i = 0; i < 20; i++ ) end += m_chatlog_text->GetLineLength( i ) + 1;
    m_chatlog_text->Remove( 0, end );
  }
}


User& ChatPanel::GetMe()
{
  return m_ui.GetServer().GetMe();
}


void ChatPanel::_OutputLine( const wxString& message, const wxColour& col )
{
  LogTime();
  m_chatlog_text->SetDefaultStyle(wxTextAttr(col));
  m_chatlog_text->Freeze();
  m_chatlog_text->AppendText( message + _T("\n") );
  if ( m_chat_log ) m_chat_log->AddMessage(message);
  CheckLength();
#ifdef __WXMSW__
  m_chatlog_text->ScrollLines( 10 );
  m_chatlog_text->ShowPosition( m_chatlog_text->GetLastPosition() );
#endif
  m_chatlog_text->Thaw();
}


void ChatPanel::OnResize( wxSizeEvent& event )
{
  SetSize( event.GetSize() );
  Layout();
  if ( m_show_nick_list ) {
    wxSize s = m_splitter->GetSize();
    m_splitter->SetSashPosition( s.GetWidth() - 238, true );
  }
}

void ChatPanel::OnLinkEvent( wxTextUrlEvent& event )
{
  if (!event.GetMouseEvent().LeftDown() ) return;
  wxString url = m_chatlog_text->GetRange( event.GetURLStart(), event.GetURLEnd());
  m_ui.OpenWebBrowser( url );
}


void ChatPanel::OnSay( wxCommandEvent& event )
{
  Say( m_say_text->GetValue() );
  m_say_text->SetValue( _T("") );
}

//! @brief Output a message said in the channel.
//!
//! @param who nick of the person who said something.
//! @param message the message to be outputted.
void ChatPanel::Said( const wxString& who, const wxString& message )
{
  wxString me = WX_STRING(GetMe().GetNick());
  wxColour col;
  bool req_user = false;
  if ( m_type == CPT_User ) req_user = true;
  if ( who.Upper() == me.Upper() ) {
    col.Set( 100,100,140 );
  } else if ( message.Upper().Contains( me.Upper() ) ) {
    col.Set( 255,40,40 );
    req_user = true;
  } else {
    col.Set( 0,0,0 );
  }

  _OutputLine( _T(" <") + who + _T("> ")+ message, col );

  if ( req_user ) {
    if ( !m_ui.mw().IsActive() ) m_ui.mw().RequestUserAttention();
  }
}


void ChatPanel::DidAction( const wxString& who, const wxString& action )
{
  _OutputLine( _T(" * ") + who + _T(" ") + action, wxColour( 230, 0, 255 ) );
}


//! @brief Output motd sent by server
//!
//! @param message The MOTD message to output
void ChatPanel::Motd( const wxString& message )
{
  _OutputLine( _T(" ** motd ** ") + message, wxColour(0, 80, 128) );
}


void ChatPanel::StatusMessage( const wxString& message )
{
  _OutputLine( _T(" ** Server ** ")+ message, wxColour(255, 150, 80) );
}


void ChatPanel::ClientMessage( const wxString& message )
{
  _OutputLine( _T(" ** ") + message, wxColour( 20, 200, 25 ) );
}


void ChatPanel::UnknownCommand( const wxString& command, const wxString& params )
{
  _OutputLine( _(" !! Command: \"") + command + _("\" params: \"") + params + _T("\"."), wxColour(128, 0, 0) );
}


wxString ChatPanel::GetChatTypeStr()
{
  if ( m_type == CPT_Channel ) return _("channel");
  else if ( m_type == CPT_Battle ) return _("battle");
  else if ( m_type == CPT_Server ) return _("server");

  return _T("ROOMTYPE FIXME");
}


void ChatPanel::Joined( User& who )
{
  _OutputLine( _T(" ** ") + WX_STRING(who.GetNick()) + _(" joined the ") + GetChatTypeStr() + _T("."), wxColour(0, 80, 0) );
  if ( m_show_nick_list ) m_nicklist->AddUser( who );
}


void ChatPanel::Parted( User& who, const wxString& message )
{
  _OutputLine( _T(" ** ")+ WX_STRING(who.GetNick()) + _(" left the channel ( ") + message + _T(" )."), wxColour(0, 80, 0) );
  if ( m_show_nick_list ) m_nicklist->RemoveUser( who );

  if ( m_type == CPT_Channel ) {
    if ( m_channel == 0 ) return;
    if ( &who == &m_channel->GetMe() ) {
      m_channel->uidata.panel = 0;
      SetChannel( 0 );
    }
  }

}


void ChatPanel::SetTopic( const wxString& who, const wxString& message )
{
  _OutputLine( _T(" ** Channel topic: ")+ message + _("\n * Set by ") + who, wxColour(0, 0, 80) );
}


void ChatPanel::UserStatusUpdated( User& who )
{
  if ( m_show_nick_list ) {
    ASSERT_LOGIC( m_nicklist != 0, "m_nicklist = 0" );
    m_nicklist->UserUpdated( who );
  }
}


Channel& ChatPanel::GetChannel()
{
  return *m_channel;
}


void ChatPanel::SetChannel( Channel* chan )
{
  ASSERT_LOGIC(m_type == CPT_Channel, "Not of type channel" );
  if ( (chan == 0) && (m_channel != 0) ) {
    StatusMessage( _("Chat closed.") );
    m_channel->uidata.panel = 0;
    if ( m_show_nick_list ) {
      m_nicklist->ClearUsers();
    }
  } else if ( chan != 0 ) {
    chan->uidata.panel = this;
    delete m_chat_log;
    m_chat_log = new ChatLog(WX_STRING(sett().GetDefaultServer()),WX_STRING(chan->GetName()));
  }
  m_channel = chan;
}


Server* ChatPanel::GetServer()
{
  return m_server;
}


void ChatPanel::SetServer( Server* serv )
{
  ASSERT_LOGIC(m_type == CPT_Server, "Not of type server" );
  if ( (serv == 0) && (m_server != 0) ) m_server->uidata.panel = 0;
  else if ( serv != 0 ) serv->uidata.panel = this;
  m_server = serv;
  delete m_chat_log;
  if ( m_server ) m_chat_log = new ChatLog(WX_STRING(sett().GetDefaultServer()),_("_SERVER"));
  else m_chat_log = 0;
}


User* ChatPanel::GetUser()
{
  return m_user;
}


void ChatPanel::SetUser( User* usr )
{
  ASSERT_LOGIC(m_type == CPT_User, "Not of type user" );

  if ( (usr == 0) && (m_user != 0) ) m_user->uidata.panel = 0;
  else if ( usr != 0 ) usr->uidata.panel = this;

  m_user = usr;

  delete m_chat_log;
  if ( m_user ) m_chat_log = new ChatLog(WX_STRING(sett().GetDefaultServer()),WX_STRING(usr->GetNick()));
  else m_chat_log = 0;
}


bool ChatPanel::IsServerPanel()
{
  return (m_type == CPT_Server);
}


ChatPanelType ChatPanel::GetPanelType()
{
  return m_type;
}


//! @brief Set the Channel object
//!
//! @param channel the Channel object.
void ChatPanel::_SetChannel( Channel* channel )
{
  if ( m_channel != 0 ) {
    m_channel->uidata.panel = 0;
  }

  m_channel = channel;

  if ( m_channel != 0 ) {
    m_channel->uidata.panel = this;
  }

}


void ChatPanel::Say( const wxString& message )
{
  wxLogDebugFunc( _T("") );

  wxStringTokenizer lines( message, _T("\n") );
  if ( lines.CountTokens() > 5 ) {
    wxMessageDialog dlg( &m_ui.mw(), wxString::Format( _("Are you sure you want to paste %d lines?"), lines.CountTokens() ), _("Flood warning"), wxYES_NO );
    if ( dlg.ShowModal() == wxID_NO ) return;
  }
  while ( lines.HasMoreTokens() ) {
    wxString line = lines.GetNextToken();
    wxLogDebug(_T("line: ") + line );

    if ( line.Find('/') == 0 ) {
      if ( m_ui.ExecuteSayCommand( line ) ) return;
    }

    if ( line == _T("/ver") ) {
      _OutputLine( _(" You have SpringLobby v") + WX_STRING( GetSpringLobbyVersion() ), *wxBLACK );
      return;
    }

    if ( m_type == CPT_Channel ) {

      if ( m_channel == 0 ) {
        _OutputLine( _(" You are not in channel or channel does not exist."), *wxRED );
        return;
      }
      if ( line.StartsWith( _T("/") ) ) {
        if ( m_channel->ExecuteSayCommand( STD_STRING(line) ) ) return;
        if ( m_channel->GetServer().ExecuteSayCommand( line ) ) return;
      }
      m_channel->Say( STD_STRING(line) );

    } else if ( m_type == CPT_Battle ) {

      if ( m_battle == 0 ) {
        _OutputLine( _(" You are not in battle or battle does not exist."), *wxRED );
        return;
      }
      if ( line.StartsWith(_T("/")) ) {
        if ( m_battle->ExecuteSayCommand( STD_STRING(line) ) ) return;
        if ( m_battle->GetServer().ExecuteSayCommand( line ) ) return;
      }
      m_battle->Say( STD_STRING(line) );

    } else if ( m_type == CPT_User ) {

      if ( m_user == 0 ) {
        _OutputLine( _(" User is offline."), *wxRED );
        return;
      }
      if ( line.StartsWith(_T("/")) ) {
        if ( m_user->ExecuteSayCommand( STD_STRING(line) ) ) return;
        if ( m_user->GetServer().ExecuteSayCommand( line ) ) return;
      }
      m_user->Say( STD_STRING(line) );

    } else if ( m_type == CPT_Server ) {
      if ( m_server == 0 ) return;

      if ( line.StartsWith(_T("/")) ) {
        if ( m_server->ExecuteSayCommand( line ) ) return;
      }

      m_server->SendRaw( STD_STRING(line) );
      _OutputLine( _(" Sent: \"") + line + _("\""), *wxBLACK );
    }

  }
}


void ChatPanel::Part()
{
  wxLogDebugFunc( _T("") );
  if ( m_type == CPT_Channel ) {
    if ( m_channel == 0 ) return;
    m_channel->Leave();
    m_channel->uidata.panel = 0;
  }
}


void ChatPanel::LogTime()
{
  wxDateTime now = wxDateTime::Now();
  m_chatlog_text->SetDefaultStyle(wxTextAttr( wxColour( 100,100,140 ) ));
  m_chatlog_text->AppendText( _T("[") + now.Format( _T("%H:%M") ) + _T("]") );
}


bool ChatPanel::IsOk()
{
  if ( m_type == CPT_Channel ) return (m_channel != 0);
  if ( m_type == CPT_Server ) return (m_server != 0);
  if ( m_type == CPT_User ) return (m_user != 0);
  if ( m_type == CPT_Battle ) return (m_battle != 0);
  return false;
}



void ChatPanel::OnUserDisconnected()
{
  _OutputLine( _T(" ** User is now offline."), wxColour( 255, 0, 0 ) );
}


void ChatPanel::OnUserConnected()
{
  _OutputLine( _T(" ** User just got online."), wxColour( 20, 200, 25 ) );
}


  //////////////////////////////////////////////////////////////////////////////////////
 // Menu Events
//////////////////////////////////////////////////////////////////////////////////////


void ChatPanel::OnMenuSelectAll( wxCommandEvent& event )
{
  m_chatlog_text->SetSelection( 0, m_chatlog_text->GetLastPosition() );
}


void ChatPanel::OnMenuCopy( wxCommandEvent& event )
{
  m_chatlog_text->Copy();
}


void ChatPanel::OnChannelMenuLeave( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  Part();
  SetChannel( 0 );
}


void ChatPanel::OnChannelAutoJoin( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  if ( m_autorejoin == 0 ) return;

  if ( m_autorejoin->IsChecked() ) {
    wxString password;
    if ( m_ui.AskPassword( _("Auto join channel"), _("Please enter password needed to join this channel, leave blank for no passwrd."), password )) {
      sett().AddChannelJoin( m_channel->GetName(), STD_STRING(password) );
      m_autorejoin->Check( true );
    }
  } else {
    sett().RemoveChannelJoin( m_channel->GetName() );
    m_autorejoin->Check( false );
  }
}


void ChatPanel::OnChannelMenuInfo( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  if ( !m_channel->UserExists( "ChanServ" ) ) {
    m_ui.ShowMessage( _("ChanServ error"), _("ChanServ is not in this channel.") );
    return;
  }
  User& cs = m_channel->GetUser( "ChanServ" );

  cs.Say( "!INFO #" + m_channel->GetName() );
  //INFO /<channame>/
}


void ChatPanel::OnChannelMenuTopic( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  if ( !m_channel->UserExists( "ChanServ" ) ) {
    m_ui.ShowMessage( _("ChanServ error"), _("ChanServ is not in this channel.") );
    return;
  }
  User& cs = m_channel->GetUser( "ChanServ" );

  wxString topic = WX_STRING(m_channel->GetTopic());
  if ( !m_ui.AskText( _("Set topic..."), _("What should be the new topic?"), topic ) ) return;

  cs.Say( "!TOPIC #" + m_channel->GetName() + " " + STD_STRING(topic) );
  //TOPIC /<channame>/ {topic}
}


void ChatPanel::OnChannelMenuMessage( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  if ( !m_channel->UserExists( "ChanServ" ) ) {
    m_ui.ShowMessage( _("ChanServ error"), _("ChanServ is not in this channel.") );
    return;
  }
  User& cs = m_channel->GetUser( "ChanServ" );

  wxString text;
  if ( !m_ui.AskText( _("Channel message..."), _("Message:"), text ) ) return;

  cs.Say( "!CHANMSG #" + m_channel->GetName() + " " + STD_STRING(text) );
  //CHANMSG /<channame>/ {message}
}


void ChatPanel::OnChannelMenuLock( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  if ( !m_channel->UserExists( "ChanServ" ) ) {
    m_ui.ShowMessage( _("ChanServ error"), _("ChanServ is not in this channel.") );
    return;
  }
  User& cs = m_channel->GetUser( "ChanServ" );

  wxString text;
  if ( !m_ui.AskText( _("Lock channel..."), _("What should the new password be?"), text ) ) return;

  cs.Say( "!LOCK #" + m_channel->GetName() + " " + STD_STRING(text) );
  //LOCK /<channame>/ <key>
}


void ChatPanel::OnChannelMenuUnlock( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  if ( !m_channel->UserExists( "ChanServ" ) ) {
    m_ui.ShowMessage( _("ChanServ error"), _("ChanServ is not in this channel.") );
    return;
  }
  User& cs = m_channel->GetUser( "ChanServ" );

  if ( !m_ui.Ask( _("Unlock Channel"), _("Are you sure you want to unlock this channel?") )) return;

  cs.Say( "!UNLOCK #" + m_channel->GetName() );
  //UNLOCK /<channame>/
}


void ChatPanel::OnChannelMenuRegister( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  if ( !m_channel->GetServer().UserExists( "ChanServ" ) ) {
    m_ui.ShowMessage( _("ChanServ error"), _("ChanServ is not on this server.") );
    return;
  }
  User& cs = m_channel->GetServer().GetUser( "ChanServ" );

  wxString text = WX_STRING(m_channel->GetMe().GetNick());
  if ( !m_ui.AskText( _("Register Channel"), _("Who should be appointed founder of this channel?"), text ) ) return;

  cs.Say( "!REGISTER #" + m_channel->GetName() + " " + STD_STRING(text) );
  //REGISTER <channame> <founder>
}


void ChatPanel::OnChannelMenuUnregister( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  if ( !m_channel->GetServer().UserExists( "ChanServ" ) ) {
    m_ui.ShowMessage( _("ChanServ error"), _("ChanServ is not on this server.") );
    return;
  }
  User& cs = m_channel->GetServer().GetUser( "ChanServ" );

  if ( !m_ui.Ask( _("Unregister Channel"), _("Are you sure you want to unregister this channel?") )) return;

  cs.Say( "!UNREGISTER #" + m_channel->GetName() );
  //UNREGISTER <channame>
}


void ChatPanel::OnChannelMenuSpamOn( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  if ( !m_channel->UserExists( "ChanServ" ) ) {
    m_ui.ShowMessage( _("ChanServ error"), _("ChanServ is not in this channel.") );
    return;
  }
  User& cs = m_channel->GetUser( "ChanServ" );

  cs.Say( "!SPAMPROTECTION #" + m_channel->GetName() + " on" );
  //SPAMPROTECTION /<channame>/ <on|off>
}


void ChatPanel::OnChannelMenuSpanOff( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  if ( !m_channel->UserExists( "ChanServ" ) ) {
    m_ui.ShowMessage( _("ChanServ error"), _("ChanServ is not in this channel.") );
    return;
  }
  User& cs = m_channel->GetUser( "ChanServ" );

  cs.Say( "!SPAMPROTECTION #" + m_channel->GetName() + " off" );
  //SPAMPROTECTION /<channame>/ <on|off>
}


void ChatPanel::OnChannelMenuSpamIsOn( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  if ( !m_channel->UserExists( "ChanServ" ) ) {
    m_ui.ShowMessage( _("ChanServ error"), _("ChanServ is not in this channel.") );
    return;
  }
  User& cs = m_channel->GetUser( "ChanServ" );

  cs.Say( "!SPAMPROTECTION #" + m_channel->GetName() );
  //SPAMPROTECTION /<channame>/
}


void ChatPanel::OnServerMenuDisconnect( wxCommandEvent& event )
{
  m_ui.Disconnect();
}


void ChatPanel::OnServerMenuReconnect( wxCommandEvent& event )
{
  m_ui.Reconnect();
}


void ChatPanel::OnServerMenuRemove( wxCommandEvent& event )
{
  wxString user;
  if ( !m_ui.AskText( _("Remove User Acount"), _("What user account do you want to remove today?"), user ) ) return;
  if ( !m_ui.Ask( _("Remove Account"), _("Are you sure you want to remove the account ") + user + _("?") ) ) return;
  Say( _T("removeaccount ") + user );
}


void ChatPanel::OnServerMenuChangePassword( wxCommandEvent& event )
{
  wxString user, password;
  if ( !m_ui.AskText( _("Change User Acount Password"), _("What user account do you want to change the password for?"), user ) ) return;
  if ( !m_ui.AskPassword( _("Change User Acount Password"), _("What would be the new password?"), password ) ) return;
  Say( _T("changeaccountpass ") + user + _T(" ") + password );
}


void ChatPanel::OnServerMenuSetAccess( wxCommandEvent& event )
{
  m_ui.ShowMessage( _("Error"), _("Not Implemented") );
}


void ChatPanel::OnServerMenuBroadcast( wxCommandEvent& event )
{
  wxString msg;
  if ( !m_ui.AskText( _("Broadcast Message"), _("Message to be broadcasted:"), msg ) ) return;
  Say( _T("broadcast ") + msg );
}


void ChatPanel::OnUserMenuOpenChat( wxCommandEvent& event )
{
  User* user = _GetSelectedUser();
  if ( user == 0 ) return;

  m_ui.mw().OpenPrivateChat( *user );
}


void ChatPanel::OnUserMenuJoinSame( wxCommandEvent& event )
{
  User* user = _GetSelectedUser();
  if ( user == 0 ) return;
  Battle* battle = user->GetBattle();
  if ( battle == 0 ) return;

  wxString password;
  if ( battle->IsPassworded() ) {
    if ( !m_ui.AskPassword( _("Battle password"), _("This battle is password protected, enter the password."), password ) ) return;
  }
  battle->Join( STD_STRING(password) );
}


void ChatPanel::OnUserMenuSlap( wxCommandEvent& event )
{
  User* user = _GetSelectedUser();
  if ( user == 0 ) return;

  if ( m_type == CPT_Channel ) {
    if ( m_channel == 0 ) return;
    m_channel->DoAction( "Slaps " + user->GetNick() + " around with a large PeeWee!" );
  } else if ( m_type == CPT_User ) {
    if ( m_user == 0 ) return;
    m_user->DoAction( "slaps " + user->GetNick() + " around with a large PeeWee!" );
  }
}


void ChatPanel::OnUserMenuMute( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  if ( !m_channel->UserExists( "ChanServ" ) ) {
    m_ui.ShowMessage( _("ChanServ error"), _("ChanServ is not in this channel.") );
    return;
  }

  wxString mutetime = _T("5");
  if ( !m_ui.AskText( _("Mute User"), _("For how many minutes should the user be muted?"), mutetime ) ) return;

  User& cs = m_channel->GetUser( "ChanServ" );

  User* user = _GetSelectedUser();
  if ( user == 0 ) return;

  cs.Say( "!MUTE #" + m_channel->GetName() + " " + user->GetNick() + " " + STD_STRING(mutetime) );
  //MUTE /<channame>/ <username> [<duration>]

}


void ChatPanel::OnUserMenuUnmute( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  if ( !m_channel->UserExists( "ChanServ" ) ) {
    m_ui.ShowMessage( _("ChanServ error"), _("ChanServ is not in this channel.") );
    return;
  }
  User& cs = m_channel->GetUser( "ChanServ" );

  User* user = _GetSelectedUser();
  if ( user == 0 ) return;

  cs.Say( "!UNMUTE #" + m_channel->GetName() + " " + user->GetNick() );
  //UNMUTE /<channame>/ <username>
}


void ChatPanel::OnUserMenuKick( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  if ( !m_channel->UserExists( "ChanServ" ) ) {
    m_ui.ShowMessage( _("ChanServ error"), _("ChanServ is not in this channel.") );
    return;
  }

  User* user = _GetSelectedUser();
  if ( user == 0 ) return;

  wxString msg;
  if ( !m_ui.AskText( _("Kick User"), _("Reason:"), msg ) ) return;

  User& cs = m_channel->GetUser( "ChanServ" );

  if ( msg != wxEmptyString ) msg = _T(" ") + msg;
  cs.Say( "!KICK #" + m_channel->GetName() + " " + user->GetNick() + STD_STRING(msg) );
  //KICK /<channame>/ <username> [{reason}]
}


void ChatPanel::OnUserMenuOp( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  if ( !m_channel->UserExists( "ChanServ" ) ) {
    m_ui.ShowMessage( _("ChanServ error"), _("ChanServ is not in this channel.") );
    return;
  }

  User* user = _GetSelectedUser();
  if ( user == 0 ) return;
  User& cs = m_channel->GetUser( "ChanServ" );

  cs.Say( "!OP #" + m_channel->GetName() + " " + user->GetNick() );
  //OP /<channame>/ <username>
}


void ChatPanel::OnUserMenuDeop( wxCommandEvent& event )
{
  if ( m_channel == 0 ) return;
  if ( !m_channel->UserExists( "ChanServ" ) ) {
    m_ui.ShowMessage( _("ChanServ error"), _("ChanServ is not in this channel.") );
    return;
  }

  User* user = _GetSelectedUser();
  if ( user == 0 ) return;
  User& cs = m_channel->GetUser( "ChanServ" );

  cs.Say( "!DEOP #" + m_channel->GetName() + " " + user->GetNick() );
  //DEOP /<channame>/ <username>
}


void ChatPanel::OnUserMenuAdminIngame( wxCommandEvent& event )
{
  m_ui.ShowMessage( _("Error"), _("Not Implemented") );
}


void ChatPanel::OnUserMenuAdminLastLogin( wxCommandEvent& event )
{
  m_ui.ShowMessage( _("Error"), _("Not Implemented") );
}


void ChatPanel::OnUserMenuAdminCurrentIP( wxCommandEvent& event )
{
  m_ui.ShowMessage( _("Error"), _("Not Implemented") );
}


void ChatPanel::OnUserMenuAdminFindIP( wxCommandEvent& event )
{
  m_ui.ShowMessage( _("Error"), _("Not Implemented") );
}


void ChatPanel::OnUserMenuAdminKick( wxCommandEvent& event )
{
  m_ui.ShowMessage( _("Error"), _("Not Implemented") );
}


void ChatPanel::OnUserMenuAdminBan( wxCommandEvent& event )
{
  m_ui.ShowMessage( _("Error"), _("Not Implemented") );
}


void ChatPanel::OnUserMenuAdminUnban( wxCommandEvent& event )
{
  m_ui.ShowMessage( _("Error"), _("Not Implemented") );
}


void ChatPanel::OnUserMenuAdminMute( wxCommandEvent& event )
{
  m_ui.ShowMessage( _("Error"), _("Not Implemented") );
}


void ChatPanel::OnUserMenuAdminUnmute( wxCommandEvent& event )
{
  m_ui.ShowMessage( _("Error"), _("Not Implemented") );
}


void ChatPanel::OnUserMenuAdminRing( wxCommandEvent& event )
{
  m_ui.ShowMessage( _("Error"), _("Not Implemented") );
}


