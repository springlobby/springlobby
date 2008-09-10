/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// Class: ChatPanel
//

#if defined(HAVE_WX26) && !defined(NO_RICHTEXT_CHAT)
#define NO_RICHTEXT_CHAT
#endif

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
#include <wx/utils.h>
#include <wx/event.h>
#include <wx/app.h>

#ifndef NO_RICHTEXT_CHAT
#include <wx/richtext/richtextctrl.h>
#endif

#ifndef HAVE_WX26
#include "auimanager.h"
#include <wx/imaglist.h>
#else
#include <wx/notebook.h>
#endif

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
#include "settings++/custom_dialogs.h"
#include "settings.h"
#include "uiutils.h"
#include "Helper/wxtextctrlhist.h"

#ifndef DISABLE_SOUND
#include "sdlsound.h"
#endif
#include "useractions.h"
#define GROUP_ID 24567
/*
BEGIN_EVENT_TABLE(MyTextCtrl, wxTextCtrl)
EVT_PAINT(MyTextCtrl::OnPaint)
END_EVENT_TABLE()
*/


BEGIN_EVENT_TABLE( ChatPanel, wxPanel )

	EVT_TEXT_ENTER( CHAT_TEXT, ChatPanel::OnSay )
	EVT_BUTTON( CHAT_SEND, ChatPanel::OnSay )
	EVT_SIZE( ChatPanel::OnResize )
	EVT_TEXT_URL( CHAT_LOG,  ChatPanel::OnLinkEvent )

	EVT_MENU( CHAT_MENU_CH_LEAVE, ChatPanel::OnChannelMenuLeave )
	EVT_MENU( CHAT_MENU_CH_DISPLAYJOIN, ChatPanel::OnChannelMenuDisplayJoinLeave )
	EVT_MENU( CHAT_MENU_CH_AUTOJOIN, ChatPanel::OnChannelAutoJoin )
	EVT_MENU( CHAT_MENU_CH_INFO, ChatPanel::OnChannelMenuInfo )
	EVT_MENU( CHAT_MENU_CH_TOPIC, ChatPanel::OnChannelMenuTopic )
	EVT_MENU( CHAT_MENU_CH_MSG, ChatPanel::OnChannelMenuMessage )
	EVT_MENU( CHAT_MENU_CH_LOCK, ChatPanel::OnChannelMenuLock )
	EVT_MENU( CHAT_MENU_CH_UNLOCK, ChatPanel::OnChannelMenuUnlock )
	EVT_MENU( CHAT_MENU_CH_REG, ChatPanel::OnChannelMenuRegister )
	EVT_MENU( CHAT_MENU_CH_UNREG, ChatPanel::OnChannelMenuUnregister )
	EVT_MENU( CHAT_MENU_CH_SPAM_ON, ChatPanel::OnChannelMenuSpamOn )
	EVT_MENU( CHAT_MENU_CH_SPAM_OFF, ChatPanel::OnChannelMenuSpanOff )
	EVT_MENU( CHAT_MENU_CH_SPAM_ISON, ChatPanel::OnChannelMenuSpamIsOn )
	EVT_MENU( wxID_COPY, ChatPanel::OnMenuCopy )
	EVT_MENU( wxID_SELECTALL, ChatPanel::OnMenuSelectAll )

	EVT_MENU( CHAT_MENU_SV_DISCON, ChatPanel::OnServerMenuDisconnect )
	EVT_MENU( CHAT_MENU_SV_RECON, ChatPanel::OnServerMenuReconnect )
	EVT_MENU( CHAT_MENU_SV_REMOVE, ChatPanel::OnServerMenuRemove )
	EVT_MENU( CHAT_MENU_SV_CHPWD, ChatPanel::OnServerMenuChangePassword )
	EVT_MENU( CHAT_MENU_SV_ACCESS, ChatPanel::OnServerMenuSetAccess )
	EVT_MENU( CHAT_MENU_SV_BROADCAST, ChatPanel::OnServerMenuBroadcast )

	EVT_MENU( CHAT_MENU_US_CHAT, ChatPanel::OnUserMenuOpenChat )
	EVT_MENU( CHAT_MENU_US_JOIN, ChatPanel::OnUserMenuJoinSame )
	EVT_MENU( CHAT_MENU_US_SLAP, ChatPanel::OnUserMenuSlap )
//	EVT_MENU( CHAT_MENU_US_ADD_TO_GROUP, ChatPanel::OnUserMenuAddToGroup )
	EVT_MENU( CHAT_MENU_US_MUTE, ChatPanel::OnUserMenuMute )
	EVT_MENU( CHAT_MENU_US_UNMUTE, ChatPanel::OnUserMenuUnmute )
	EVT_MENU( CHAT_MENU_US_KICK, ChatPanel::OnUserMenuKick )
	EVT_MENU( CHAT_MENU_US_OP, ChatPanel::OnUserMenuOp )
	EVT_MENU( CHAT_MENU_US_DEOP, ChatPanel::OnUserMenuDeop )
	EVT_MENU( CHAT_MENU_US_MODERATOR_INGAME, ChatPanel::OnUserMenuModeratorIngame )
	EVT_MENU( CHAT_MENU_US_MODERATOR_CURIP, ChatPanel::OnUserMenuModeratorCurrentIP )
	EVT_MENU( CHAT_MENU_US_MODERATOR_KICK, ChatPanel::OnUserMenuModeratorKick )
	EVT_MENU( CHAT_MENU_US_MODERATOR_BAN, ChatPanel::OnUserMenuModeratorBan )
	EVT_MENU( CHAT_MENU_US_MODERATOR_UNBAN, ChatPanel::OnUserMenuModeratorUnban )
	EVT_MENU( CHAT_MENU_US_MODERATOR_MUTE, ChatPanel::OnUserMenuModeratorMute )
	EVT_MENU( CHAT_MENU_US_MODERATOR_MUTE_5, ChatPanel::OnUserMenuModeratorMute5 )
	EVT_MENU( CHAT_MENU_US_MODERATOR_MUTE_10, ChatPanel::OnUserMenuModeratorMute10 )
	EVT_MENU( CHAT_MENU_US_MODERATOR_MUTE_30, ChatPanel::OnUserMenuModeratorMute30 )
	EVT_MENU( CHAT_MENU_US_MODERATOR_MUTE_120, ChatPanel::OnUserMenuModeratorMute120 )
	EVT_MENU( CHAT_MENU_US_MODERATOR_MUTE_1440, ChatPanel::OnUserMenuModeratorMute1440 )
	EVT_MENU( CHAT_MENU_US_MODERATOR_UNMUTE, ChatPanel::OnUserMenuModeratorUnmute )
	EVT_MENU( CHAT_MENU_US_MODERATOR_RING, ChatPanel::OnUserMenuModeratorRing )

END_EVENT_TABLE()


void ChatPanel::OnMouseDown( wxMouseEvent& event ) {
	wxLogDebugFunc( _T( "" ) );
	CreatePopup();
	if ( m_popup_menu != 0 ) PopupMenu( m_popup_menu );
	else event.Skip();
}


ChatPanel::ChatPanel( wxWindow* parent, Ui& ui, Channel& chan, wxImageList* imaglist )
#ifdef HAVE_WX26
		: wxPanel( parent, -1 ), m_show_nick_list( true ), m_chat_tabs(( wxNotebook* )parent ), m_ui( ui ), m_channel( &chan ), m_server( 0 ), m_user( 0 ), m_battle( 0 ), m_type( CPT_Channel ), m_popup_menu( 0 ), m_icon_index( 2 ), m_imagelist(( wxImageList* )imaglist  {
#else
    : wxPanel( parent, -1 ), m_show_nick_list( true ), m_chat_tabs(( wxAuiNotebook* )parent ), m_ui( ui ), m_channel( &chan ), m_server( 0 ), m_user( 0 ), m_battle( 0 ), m_type( CPT_Channel ), m_popup_menu( 0 ), m_icon_index( 2 ), m_imagelist(( wxImageList* )imaglist ) {
GetAui().manager->AddPane( this, wxLEFT, _T("chatpanel-channel-") + chan.GetName() );
#endif
	wxLogDebugFunc( _T( "wxWindow* parent, Channel& chan" ) );
	CreateControls( );
	_SetChannel( &chan );
	m_chatlog_text->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ChatPanel::OnMouseDown ), 0, this );
	#ifdef __WXMSW__
        wxString chan_prefix = _("channel_");
    #else
        wxString chan_prefix = _("#");
    #endif
	m_chat_log = new ChatLog( sett().GetDefaultServer(), chan_prefix + chan.GetName() );
}


ChatPanel::ChatPanel( wxWindow* parent, Ui& ui, User& user, wxImageList* imaglist )
#ifdef HAVE_WX26
		: wxPanel( parent, -1 ), m_show_nick_list( false ),m_nicklist(0), m_chat_tabs(( wxNotebook* )parent ), m_ui( ui ), m_channel( 0 ), m_server( 0 ), m_user( &user ), m_battle( 0 ), m_type( CPT_User ), m_popup_menu( 0 ), m_icon_index( 3 ), m_imagelist(imaglist)  {
#else
		: wxPanel( parent, -1 ), m_show_nick_list( false ),m_nicklist(0), m_chat_tabs(( wxAuiNotebook* )parent ), m_ui( ui ), m_channel( 0 ), m_server( 0 ), m_user( &user ), m_battle( 0 ), m_type( CPT_User ), m_popup_menu( 0 ), m_icon_index( 3 ), m_imagelist(imaglist) {
GetAui().manager->AddPane( this, wxLEFT, _T("chatpanel-pm-") + user.GetNick() );
#endif

	CreateControls( );
	user.uidata.panel = this;
	m_chat_log = new ChatLog( sett().GetDefaultServer(), user.GetNick() );
}


ChatPanel::ChatPanel( wxWindow* parent, Ui& ui, Server& serv, wxImageList* imaglist )
#ifdef HAVE_WX26
		: wxPanel( parent, -1 ), m_show_nick_list( false ), m_nicklist(0),m_chat_tabs(( wxNotebook* )parent ), m_ui( ui ), m_channel( 0 ), m_server( &serv ), m_user( 0 ), m_battle( 0 ), m_type( CPT_Server ), m_popup_menu( 0 ), m_icon_index( 1 ), m_imagelist(imaglist)  {
#else
    : wxPanel( parent, -1 ), m_show_nick_list( false ), m_nicklist(0),m_chat_tabs(( wxAuiNotebook* )parent ), m_ui( ui ), m_channel( 0 ), m_server( &serv ), m_user( 0 ), m_battle( 0 ), m_type( CPT_Server ), m_popup_menu( 0 ), m_icon_index( 1 ), m_imagelist(imaglist) {
  GetAui().manager->AddPane( this, wxLEFT, _T("chatpanel-server") );
#endif

	wxLogDebugFunc( _T( "wxWindow* parent, Server& serv" ) );
	CreateControls( );
	serv.uidata.panel = this;
	m_chat_log = new ChatLog( sett().GetDefaultServer(), _T( "_SERVER" ) );
	m_chatlog_text->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ChatPanel::OnMouseDown ), 0, this );
}


ChatPanel::ChatPanel( wxWindow* parent, Ui& ui, Battle& battle )
		: wxPanel( parent, -1 ), m_show_nick_list( false ), m_nicklist( 0 ), m_chat_tabs( 0 ), m_ui( ui ),
		 m_channel( 0 ), m_server( 0 ), m_user( 0 ), m_battle( &battle ), m_type( CPT_Battle ), m_popup_menu( 0 ) {
	wxLogDebugFunc( _T( "wxWindow* parent, Battle& battle" ) );
	for (unsigned int i = 0; i < battle.GetNumUsers();++i)
    {
       textcompletiondatabase.Insert_Mapping( battle.GetUser(i).GetNick(), battle.GetUser(i).GetNick() );
    }
	CreateControls( );
	wxDateTime now = wxDateTime::Now();
	m_chat_log = new ChatLog( sett().GetDefaultServer(), _T( "_BATTLE_" ) + now.Format( _T( "%Y_%m_%d__%H_%M_%S" ) ) );
}


//! @brief ChatPanel destructor.
ChatPanel::~ChatPanel() {
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


	if ( m_type == CPT_Channel )
	{
        m_chatlog_text->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ChatPanel::OnMouseDown ), 0, 0 );
        #ifndef HAVE_WX26
        GetAui().manager->DetachPane( this );
        #endif
	}
	else if ( m_type == CPT_Server )
	{
        m_chatlog_text->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ChatPanel::OnMouseDown ), 0, 0 );
        #ifndef HAVE_WX26
        GetAui().manager->DetachPane( this );
        #endif
	}
	else if ( m_type == CPT_User )
	{
        #ifndef HAVE_WX26
        GetAui().manager->DetachPane( this );
        #endif
	}
}


void ChatPanel::CreateControls( ) {
	wxLogDebugFunc( _T( "" ) );

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
        m_usermenu = CreateNickListMenu();
		m_nicklist = new NickListCtrl( m_nick_panel, true, m_usermenu );

   // m_nick_filter = new wxComboBox( m_nick_panel, -1, _("Show all"), wxDefaultPosition, wxSize(80,CONTROL_HEIGHT), 0, 0, wxCB_READONLY );
   // m_nick_filter->Disable();

        m_nick_sizer->Add( m_nicklist, 1, wxEXPAND );
   // m_nick_sizer->Add( m_nick_filter, 0, wxEXPAND | wxTOP, 2 );

		m_nick_panel->SetSizer( m_nick_sizer );

	} else {

		m_chat_panel = this;
		m_nick_sizer = 0;
		m_nicklist = 0;
		m_nick_filter = 0;
		m_splitter = 0;

	}

	// Creating ui elements
  #ifndef NO_RICHTEXT_CHAT
	m_chatlog_text = new wxRichTextCtrl( m_chat_panel, CHAT_LOG, _T( "" ), wxDefaultPosition, wxDefaultSize,
	                                     wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_AUTO_URL );

	// Accelerators
	// copied from wxWidgets SVN revision 53442, so this can probably be removed for wx > 2.8.7
	wxAcceleratorEntry entries[2];

	entries[0].Set(wxACCEL_CMD,   (int) 'C',       wxID_COPY);
	entries[1].Set(wxACCEL_CMD,   (int) 'A',       wxID_SELECTALL);

	wxAcceleratorTable accel(2, entries);
	m_chatlog_text->SetAcceleratorTable(accel);
  #else
	m_chatlog_text = new wxTextCtrl( m_chat_panel, CHAT_LOG, _T( "" ), wxDefaultPosition, wxDefaultSize,
																	 wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_AUTO_URL );
  #endif
	if ( m_type == CPT_Channel )
  		m_chatlog_text->SetToolTip( _("right click for options (like autojoin)" ) );

	m_say_text = new wxTextCtrlHist( textcompletiondatabase, m_chat_panel, CHAT_TEXT, _T( "" ), wxDefaultPosition, wxSize( 100, CONTROL_HEIGHT ), wxTE_PROCESS_ENTER | wxTE_MULTILINE | wxTE_PROCESS_TAB );
	m_say_button = new wxButton( m_chat_panel, CHAT_SEND, _( "Send" ), wxDefaultPosition, wxSize( 80, CONTROL_HEIGHT ) );


	// Adding elements to sizers
	m_say_sizer->Add( m_say_text, 1, wxEXPAND );
	m_say_sizer->Add( m_say_button );
	m_chat_sizer->Add( m_chatlog_text, 1, wxEXPAND );
	m_chat_sizer->Add( m_say_sizer, 0, wxEXPAND | wxTOP, 2 );
	if ( m_show_nick_list ) {
		m_chat_panel->SetSizer( m_chat_sizer );
		m_splitter->SplitVertically( m_chat_panel, m_nick_panel, 100 );
		m_splitter->SetMinimumPaneSize( 30 );
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

	m_chatlog_text->SetBackgroundColour( sett().GetChatColorBackground() );
	m_chatlog_text->SetFont( sett().GetChatFont() );

	// Fill up TextCompletionDatabase
	textcompletiondatabase.Insert_Mapping( _T("DLDK"), _T("Der Lockruf des Kaos") );
	textcompletiondatabase.Insert_Mapping( _T("lol"), _T("Laughing out loud") );
	textcompletiondatabase.Insert_Mapping( _T("wb"), _T("Welcome back") );
	textcompletiondatabase.Insert_Mapping( _T("hf"), _T("Have Fun!") );
	textcompletiondatabase.Insert_Mapping( _T("kaot"), _T("Have Fun!") );
	textcompletiondatabase.Insert_Mapping( _T("kaot_H"), _T("Der Kaot aus der Hölle.") );

}


void ChatPanel::CreatePopup() {
	if ( m_popup_menu != 0 ) return;
	wxLogDebugFunc( _T( "" ) );
	if ( m_type == CPT_Channel ) {

		wxLogMessage( _T( "channel" ) );
		m_popup_menu = new wxMenu();
		m_autorejoin = new wxMenuItem( m_popup_menu, CHAT_MENU_CH_AUTOJOIN, _( "Auto join this channel" ), wxEmptyString, wxITEM_CHECK );
		m_popup_menu->Append( m_autorejoin );
		if ( m_channel && m_channel->GetName() != _T( "springlobby" ) ) {
			bool isautojoin = sett().GetChannelJoinIndex( m_channel->GetName() ) >= 0;
			m_autorejoin->Check( isautojoin );
		} else {
			m_autorejoin->Check( true );
			m_autorejoin->Enable( false );
		}

		wxMenuItem* leaveitem = new wxMenuItem( m_popup_menu, CHAT_MENU_CH_LEAVE, _( "Leave" ), wxEmptyString, wxITEM_NORMAL );
		m_popup_menu->Append( leaveitem );

		displayjoinitem = new wxMenuItem( m_popup_menu, CHAT_MENU_CH_DISPLAYJOIN, _( "Display Join/Leave Messages" ), wxEmptyString, wxITEM_CHECK );
		if ( m_channel && m_type == CPT_Channel ) {
			m_popup_menu->Append( displayjoinitem );
			displayjoinitem->Check( sett().GetDisplayJoinLeave( m_channel->GetName() ) );
		}

		m_popup_menu->AppendSeparator();
		wxMenuItem* selectitem = new wxMenuItem( m_popup_menu, wxID_SELECTALL, _( "Select all" ), wxEmptyString, wxITEM_NORMAL );
		m_popup_menu->Append( selectitem );
		wxMenuItem* copyitem = new wxMenuItem( m_popup_menu, wxID_COPY, _( "Copy" ), wxEmptyString, wxITEM_NORMAL );
		m_popup_menu->Append( copyitem );

		m_popup_menu->AppendSeparator();
		wxMenu* m_chanserv;
		m_chanserv = new wxMenu();
		wxMenuItem* infoitem = new wxMenuItem( m_chanserv, CHAT_MENU_CH_INFO, _( "Channel info" ), wxEmptyString, wxITEM_NORMAL );
		m_chanserv->Append( infoitem );

		m_chanserv->AppendSeparator();
		wxMenuItem* chtopicitem = new wxMenuItem( m_chanserv, CHAT_MENU_CH_TOPIC, _( "Set topic..." ), wxEmptyString, wxITEM_NORMAL );
		m_chanserv->Append( chtopicitem );
		wxMenuItem* chmessageitem = new wxMenuItem( m_chanserv, CHAT_MENU_CH_MSG, _( "Channel message..." ), wxEmptyString, wxITEM_NORMAL );
		m_chanserv->Append( chmessageitem );

		m_chanserv->AppendSeparator();
		wxMenuItem* chlockitem = new wxMenuItem( m_chanserv, CHAT_MENU_CH_LOCK, _( "Lock..." ), wxEmptyString, wxITEM_NORMAL );
		m_chanserv->Append( chlockitem );
		wxMenuItem* chunlockitem = new wxMenuItem( m_chanserv, CHAT_MENU_CH_UNLOCK, _( "Unlock" ), wxEmptyString, wxITEM_NORMAL );
		m_chanserv->Append( chunlockitem );

		m_chanserv->AppendSeparator();
		wxMenuItem* chregisteritem = new wxMenuItem( m_chanserv, CHAT_MENU_CH_REG, _( "Register..." ), wxEmptyString, wxITEM_NORMAL );
		m_chanserv->Append( chregisteritem );
		wxMenuItem* chunregisteritem = new wxMenuItem( m_chanserv, CHAT_MENU_CH_UNREG, _( "Unregister" ), wxEmptyString, wxITEM_NORMAL );
		m_chanserv->Append( chunregisteritem );

		m_chanserv->AppendSeparator();
		wxMenu* m_spam;
		m_spam = new wxMenu();
		wxMenuItem* spamprotonitem = new wxMenuItem( m_spam, CHAT_MENU_CH_SPAM_ON, _( "On" ), wxEmptyString, wxITEM_NORMAL );
		m_spam->Append( spamprotonitem );
		wxMenuItem* spamprotoffitem = new wxMenuItem( m_spam, CHAT_MENU_CH_SPAM_OFF, _( "Off" ), wxEmptyString, wxITEM_NORMAL );
		m_spam->Append( spamprotoffitem );

		m_spam->AppendSeparator();
		wxMenuItem* spamprotisonitem = new wxMenuItem( m_spam, CHAT_MENU_CH_SPAM_ISON, _( "Is on?" ), wxEmptyString, wxITEM_NORMAL );
		m_spam->Append( spamprotisonitem );
		m_chanserv->Append( -1, _( "Spam protection" ), m_spam );
		m_popup_menu->Append( -1, _( "ChanServ" ), m_chanserv );

	} else if ( m_type == CPT_Server ) {

		wxLogMessage( _T( "server" ) );
		m_popup_menu = new wxMenu();

		wxMenuItem* disconnectitem = new wxMenuItem( m_popup_menu, CHAT_MENU_SV_DISCON, _( "Disconnect" ), wxEmptyString, wxITEM_NORMAL );
		m_popup_menu->Append( disconnectitem );
		wxMenuItem* reconnectitem = new wxMenuItem( m_popup_menu, CHAT_MENU_SV_RECON, _( "Reconnect" ), wxEmptyString, wxITEM_NORMAL );
		m_popup_menu->Append( reconnectitem );

		m_popup_menu->AppendSeparator();
		wxMenuItem* selectitem = new wxMenuItem( m_popup_menu, wxID_SELECTALL, _( "Select all" ), wxEmptyString, wxITEM_NORMAL );
		m_popup_menu->Append( selectitem );
		wxMenuItem* copyitem = new wxMenuItem( m_popup_menu, wxID_COPY, _( "Copy" ), wxEmptyString, wxITEM_NORMAL );
		m_popup_menu->Append( copyitem );

		m_popup_menu->AppendSeparator();
		wxMenu* m_user_menu;

		m_user_menu = new wxMenu();
		wxMenu* m_accounts;
		m_accounts = new wxMenu();
		wxMenuItem* removeitem = new wxMenuItem( m_accounts, CHAT_MENU_SV_REMOVE, _( "Remove..." ), wxEmptyString, wxITEM_NORMAL );
		m_accounts->Append( removeitem );
		wxMenuItem* chpwditem = new wxMenuItem( m_accounts, CHAT_MENU_SV_CHPWD, _( "Change password..." ), wxEmptyString, wxITEM_NORMAL );
		m_accounts->Append( chpwditem );
		wxMenuItem* setaccessitem = new wxMenuItem( m_accounts, CHAT_MENU_SV_ACCESS, _( "Set access..." ), wxEmptyString, wxITEM_NORMAL );
		m_accounts->Append( setaccessitem );
		m_user_menu->Append( -1, _( "Accounts" ), m_accounts );

		m_user_menu->AppendSeparator();
		wxMenuItem* broadcastitem = new wxMenuItem( m_user_menu, CHAT_MENU_SV_BROADCAST, _( "Broadcast..." ), wxEmptyString, wxITEM_NORMAL );
		m_user_menu->Append( broadcastitem );
		m_popup_menu->Append( -1, _( "Admin" ), m_user_menu );
	} else {
		m_popup_menu = 0;
	}

}


UserMenu* ChatPanel::CreateNickListMenu() {
	UserMenu* m_user_menu;
	m_user_menu = new UserMenu( this );
	wxMenuItem* chatitem = new wxMenuItem( m_user_menu, CHAT_MENU_US_CHAT,  _( "Open Chat" ) , wxEmptyString, wxITEM_NORMAL );
	m_user_menu->Append( chatitem );
    wxMenuItem* joinbattleitem = new wxMenuItem( m_user_menu, CHAT_MENU_US_JOIN,  _( "Join same battle" ) , wxEmptyString, wxITEM_NORMAL );
    m_user_menu->Append( joinbattleitem );

	m_user_menu->AppendSeparator();

	if ( ui().GetServer().GetMe().GetStatus().moderator ) {
		wxMenuItem* modingameitem = new wxMenuItem( m_user_menu, CHAT_MENU_US_MODERATOR_INGAME, _( "Ingame time" ), wxEmptyString, wxITEM_NORMAL );
		m_user_menu->Append( modingameitem );
		wxMenuItem* modipitem = new wxMenuItem( m_user_menu, CHAT_MENU_US_MODERATOR_CURIP, _( "Retrieve IP and Smurfs" ), wxEmptyString, wxITEM_NORMAL );
		m_user_menu->Append( modipitem );

		m_user_menu->AppendSeparator();

		wxMenu* m_user_menu_mute;
		m_user_menu_mute = new wxMenu();
		wxMenuItem* modmuteitem = new wxMenuItem( m_user_menu_mute, CHAT_MENU_US_MODERATOR_MUTE, _( "Mute..." ) , wxEmptyString, wxITEM_NORMAL );
		m_user_menu_mute->Append( modmuteitem );
		wxMenuItem* modmute5item = new wxMenuItem( m_user_menu_mute, CHAT_MENU_US_MODERATOR_MUTE_5, _( "Mute for 5 minutes" ) , wxEmptyString, wxITEM_NORMAL );
		m_user_menu_mute->Append( modmute5item );
		wxMenuItem* modmute10item = new wxMenuItem( m_user_menu_mute, CHAT_MENU_US_MODERATOR_MUTE_10, _( "Mute for 10 minutes" ), wxEmptyString, wxITEM_NORMAL );
		m_user_menu_mute->Append( modmute10item );
		wxMenuItem* modmute30item = new wxMenuItem( m_user_menu_mute, CHAT_MENU_US_MODERATOR_MUTE_30, _( "Mute for 30 minutes" ), wxEmptyString, wxITEM_NORMAL );
		m_user_menu_mute->Append( modmute30item );
		wxMenuItem* modmute120item = new wxMenuItem( m_user_menu_mute, CHAT_MENU_US_MODERATOR_MUTE_120, _( "Mute for 2 hours" ) , wxEmptyString, wxITEM_NORMAL );
		m_user_menu_mute->Append( modmute120item );
		wxMenuItem* modmute1440item = new wxMenuItem( m_user_menu_mute, CHAT_MENU_US_MODERATOR_MUTE_1440, _( "Mute for 1 day" ), wxEmptyString, wxITEM_NORMAL );
		m_user_menu_mute->Append( modmute1440item );
		m_user_menu_mute->AppendSeparator();
		wxMenuItem* modunmuteitem = new wxMenuItem( m_user_menu_mute, CHAT_MENU_US_MODERATOR_UNMUTE, _( "Unmute" ), wxEmptyString, wxITEM_NORMAL );
		m_user_menu_mute->Append( modunmuteitem );
		m_user_menu->Append( -1, _( "Mute" ), m_user_menu_mute );

		wxMenuItem* modkickitem = new wxMenuItem( m_user_menu, CHAT_MENU_US_MODERATOR_KICK, _( "Kick..." ), wxEmptyString, wxITEM_NORMAL );
		m_user_menu->Append( modkickitem );

		m_user_menu->AppendSeparator();
		wxMenuItem* modbanitem = new wxMenuItem( m_user_menu, CHAT_MENU_US_MODERATOR_BAN, _( "Ban..." ), wxEmptyString, wxITEM_NORMAL );
		m_user_menu->Append( modbanitem );
		wxMenuItem* modunbanitem = new wxMenuItem( m_user_menu, CHAT_MENU_US_MODERATOR_UNBAN, _( "Unban" ), wxEmptyString, wxITEM_NORMAL );
		m_user_menu->Append( modunbanitem );

		m_user_menu->AppendSeparator();
		wxMenuItem* modringitem = new wxMenuItem( m_user_menu, CHAT_MENU_US_MODERATOR_RING, _( "Ring" ), wxEmptyString, wxITEM_NORMAL );
		m_user_menu->Append( modringitem );
		//m_user_menu->Append( -1, _("Moderator"), m_user_menu );
	} else {
		wxMenuItem* slapitem = new wxMenuItem( m_user_menu, CHAT_MENU_US_SLAP, _( "Slap!" ), wxEmptyString, wxITEM_NORMAL );
		m_user_menu->Append( slapitem );
	}

	m_user_menu->AppendSeparator();
	wxMenu* m_chanserv;
	m_chanserv = new wxMenu();
	wxMenuItem* chmuteitem = new wxMenuItem( m_chanserv, CHAT_MENU_US_MUTE, _( "Mute..." ), wxEmptyString, wxITEM_NORMAL );
	m_chanserv->Append( chmuteitem );
	wxMenuItem* chunmuteitem = new wxMenuItem( m_chanserv, CHAT_MENU_US_UNMUTE, _( "Unmute" ), wxEmptyString, wxITEM_NORMAL );
	m_chanserv->Append( chunmuteitem );
	m_chanserv->AppendSeparator();
	wxMenuItem* chkickitem = new wxMenuItem( m_chanserv, CHAT_MENU_US_KICK, _( "Kick..." ), wxEmptyString, wxITEM_NORMAL );
	m_chanserv->Append( chkickitem );

	m_chanserv->AppendSeparator();
	wxMenuItem* chopitem = new wxMenuItem( m_chanserv, CHAT_MENU_US_OP, _( "Op" ), wxEmptyString, wxITEM_NORMAL );
	m_chanserv->Append( chopitem );
	wxMenuItem* chdeopitem = new wxMenuItem( m_chanserv, CHAT_MENU_US_DEOP, _( "DeOp" ), wxEmptyString, wxITEM_NORMAL );
	m_chanserv->Append( chdeopitem );
	m_user_menu->Append( -1, _( "ChanServ" ), m_chanserv );

	return m_user_menu;
}


User* ChatPanel::GetSelectedUser() {
	if ( m_nicklist == 0 ) return 0;

	int item = m_nicklist->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	if ( item == -1 ) return 0;

	return ( User* )m_nicklist->GetItemData( item );
}


void ChatPanel::CheckLength() {
	if ( size_t( m_chatlog_text->GetNumberOfLines() ) > sett().GetChatHistoryLenght() && sett().GetChatHistoryLenght() > 0 ) {
		int end = 0;
		for ( int i = 0; i < 20; i++ ) end += m_chatlog_text->GetLineLength( i ) + 1;
		m_chatlog_text->Remove( 0, end );
	}
}

User& ChatPanel::GetMe() {
	return m_ui.GetServer().GetMe();
}

void ChatPanel::OutputLine( const wxString& message, const wxColour& col, const wxFont& fon )
{

	if ( ! m_chatlog_text ) return;
	LogTime();
#ifndef NO_RICHTEXT_CHAT
	int p=m_chatlog_text->GetLastPosition()-1;
	if(p<0)p=0;
	bool at_bottom=m_chatlog_text->IsPositionVisible(p); /// true if we're on bottom of page and must scroll
#endif
	m_chatlog_text->SetDefaultStyle( wxTextAttr( col, sett().GetChatColorBackground(), fon ) );
#if  defined(__WXMSW__) && defined(NO_RICHTEXT_CHAT)
	m_chatlog_text->Freeze();
#endif

#ifndef NO_RICHTEXT_CHAT
	if ( message.Find(_T("://")) != wxNOT_FOUND )
	{
		/* start index of the current word */
		size_t st(0);
		/* end index of the current word */
		size_t en(0);

		size_t len(message.Len());

		while ( en < len && en != wxString::npos && st < len && st != wxString::npos )
		{
			/* Advance to the start of the next word */
			st = message.find_first_not_of(_T(" "), st);

			if ( st == wxString::npos )
				st = len;

			/* Output whatever we skipped */
			/* if ( st > en ) */
			m_chatlog_text->AppendText(message.Mid(en, st - en));

			/* Find the end of the word */
			en = message.find(_T(' '), st);


			if ( st == wxString::npos && en == wxString::npos )
				break; /* Nothing more to do. */

			/* Extract the word */
			wxString word;
			if ( en == wxString::npos )
				word = message.Mid(st);
			else
				word = message.Mid(st, en - st);

			bool isUrl(word.Contains(_T("://")));

			if ( isUrl )
				m_chatlog_text->BeginURL(word);

			m_chatlog_text->AppendText( word );

			if ( isUrl )
			{
				/* The link is broken (http://www.example.com
				 * becomes http://www.example.co) if this space
				 * (or other string, probably) isn't appended
				 * here.
				 */
				m_chatlog_text->AppendText(_T(' '));
				m_chatlog_text->EndURL();
			}

			/* Advance past the end of the current word. Note that
			 * we compensate for the extra space in the workaround
			 * above, to avoid extra spaces in the final output.
			 *
			 * Only problem is, I'm not totally sure this is doing
			 * what it should. -- insane
			 */
			st = en + ( isUrl ? 2 : 1 );
		}
	}
	else
	{
		m_chatlog_text->AppendText( message );
	}
	m_chatlog_text->AppendText( _T( "\n" ) );

	bool enable_autoscroll = sett().GetAlwaysAutoScrollOnFocusLost() || (m_ui.GetActiveChatPanel() == this );

	if ( ( sett().GetSmartScrollEnabled() && at_bottom && enable_autoscroll ) ) { /// view not at the bottom or not focused = disable autoscroll
		m_chatlog_text->ScrollLines( 10 ); /// to prevent for weird empty space appended
		m_chatlog_text->ShowPosition( m_chatlog_text->GetLastPosition() );/// scroll to the bottom
		m_chatlog_text->ScrollLines( 10 ); /// to prevent for weird empty space appended
	}
	CheckLength(); /// crop lines from history that exceeds limit
#else
	m_chatlog_text->AppendText( message + _T( "\n" ) );
	CheckLength(); /// crop lines from history that exceeds limit

	m_chatlog_text->ScrollLines( 10 ); /// to prevent for weird empty space appended
	m_chatlog_text->ShowPosition( m_chatlog_text->GetLastPosition() );/// scroll to the bottom
#endif

	if ( m_chat_log ) m_chat_log->AddMessage( message );

#if  defined(__WXMSW__) && defined(NO_RICHTEXT_CHAT)
	m_chatlog_text->Thaw();
#endif

}


void ChatPanel::OnResize( wxSizeEvent& event ) {
	SetSize( event.GetSize() );
	Layout();
	if ( m_show_nick_list ) {
		wxSize s = m_splitter->GetSize();
		m_splitter->SetSashPosition( s.GetWidth() - 238, true );
	}
}

void ChatPanel::OnLinkEvent( wxTextUrlEvent& event ) {
  #ifdef NO_RICHTEXT_CHAT
  if ( !event.GetMouseEvent().LeftDown() ) return;
  #endif
	wxString url = m_chatlog_text->GetRange( event.GetURLStart(), event.GetURLEnd()+1 );
	m_ui.OpenWebBrowser( url );
}

void ChatPanel::OnSay( wxCommandEvent& event ) {
	Say( m_say_text->GetValue() );
m_say_text->SetValue( _T( "" ) );
}

//! @brief Output a message said in the channel.
//!
//! @param who nick of the person who said something.
//! @param message the message to be outputted.
void ChatPanel::Said( const wxString& who, const wxString& message ) {
	wxString me = GetMe().GetNick();
	wxColour col;
	bool req_user = false;
	if ( m_type == CPT_User ) req_user = true;
	if ( who.Upper() == me.Upper() ) {
		col = sett().GetChatColorMine();
    // change the image of the tab to show new events
    if (  m_ui.GetActiveChatPanel() != this && m_chat_tabs )
    {
      for ( unsigned int i = 0; i <  m_chat_tabs->GetPageCount( ); ++i )
        if ( m_chat_tabs->GetPage( i ) == this ) {
          if ( m_type == CPT_Channel && m_icon_index < 6 )
          {
             m_icon_index = 6;
             #ifdef HAVE_WX26
             m_chat_tabs->SetPageImage( i, m_icon_index );
             #else
             m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
             #endif
          }
          if ( m_type == CPT_User && m_icon_index < 7 )
          {
             m_icon_index = 7;
             #ifdef HAVE_WX26
             m_chat_tabs->SetPageImage( i, m_icon_index );
             #else
             m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
             #endif
          }
        }
    }
	} else if ( message.Upper().Contains( me.Upper() ) )
    {
      for ( unsigned int i = 0; i <  m_chat_tabs->GetPageCount( ); ++i )
        if ( m_chat_tabs->GetPage( i ) == this ) {
          if ( m_type == CPT_Channel && m_icon_index < 8 )
          {
             m_icon_index = 8;
             #ifdef HAVE_WX26
             m_chat_tabs->SetPageImage( i, m_icon_index );
             #else
             m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
             #endif
          }
          if ( m_type == CPT_User && m_icon_index < 9 )
          {
             m_icon_index = 9;
             #ifdef HAVE_WX26
             m_chat_tabs->SetPageImage( i, m_icon_index );
             #else
             m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
             #endif
          }
        }
            col = sett().GetChatColorNotification();
            req_user = true;
	} else {
    // change the image of the tab to show new events
    if (  m_ui.GetActiveChatPanel() != this && m_chat_tabs )
    {
      for ( unsigned int i = 0; i <  m_chat_tabs->GetPageCount( ); ++i )
        if ( m_chat_tabs->GetPage( i ) == this ) {
          if ( m_type == CPT_Channel && m_icon_index < 6 )
          {
             m_icon_index = 6;
             #ifdef HAVE_WX26
             m_chat_tabs->SetPageImage( i, m_icon_index );
             #else
             m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
             #endif
          }
          if ( m_type == CPT_User && m_icon_index < 7 )
          {
             m_icon_index = 7;
             #ifdef HAVE_WX26
             m_chat_tabs->SetPageImage( i, m_icon_index );
             #else
             m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
             #endif
          }
        }
    }
        //process logic for custom word highlights
        if ( ContainsWordToHighlight( message ) )
        {
            req_user = sett().GetRequestAttOnHighlight();
            col = sett().GetChatColorHighlight();
        }
        else
            col = sett().GetChatColorNormal();
	}

	if ( who == _T( "MelBot" ) && message.StartsWith( _T( "<" ) ) && message.Contains( _T( ">" ) ) ) {
		wxString who2;
		wxString message2;
		who2 = message.BeforeFirst( '>' ).AfterFirst( '<' ) + _T( "@IRC" );
		//don't highlight if i'm talking from irc to channel
		if ( who2.Upper() == ( me.Upper() + _T("@IRC") ) )
		{
		    req_user = false;
		    col = sett().GetChatColorNormal();
		}
		message2 = message.AfterFirst( '>' );
		OutputLine( _T( " <" ) + who2 + _T( "> " ) + message2, col, sett().GetChatFont() );
	} else {
		OutputLine( _T( " <" ) + who + _T( "> " ) + message, col, sett().GetChatFont() );
	}


	if ( req_user ) {
     m_ui.mw().RequestUserAttention();
     #ifndef DISABLE_SOUND
     if ( sett().GetChatPMSoundNotificationEnabled() && ( m_ui.GetActiveChatPanel() != this  || !wxTheApp->IsActive() ) )
        sound().pm();
     #endif
	}
}

bool ChatPanel::ContainsWordToHighlight( const wxString& message )
{
    //get list of words to highlight
    wxStringTokenizer words ( sett().GetHighlightedWords(), _T(";") );
    while ( words.HasMoreTokens() )
    {
        if (message.Contains( words.GetNextToken() ) )
            return true;
    }
    return false;

}

void ChatPanel::DidAction( const wxString& who, const wxString& action ) {
  // change the image of the tab to show new events
	if (  m_ui.GetActiveChatPanel() != this && m_chat_tabs )
	{
		for ( unsigned int i = 0; i <  m_chat_tabs->GetPageCount( ); ++i )
			if ( m_chat_tabs->GetPage( i ) == this ) {
				if ( m_type == CPT_Channel && m_icon_index < 6 )
				{
             m_icon_index = 6;
             #ifdef HAVE_WX26
             m_chat_tabs->SetPageImage( i, m_icon_index );
             #else
             m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
             #endif
				}
				if ( m_type == CPT_User && m_icon_index < 7 )
				{
             m_icon_index = 7;
             #ifdef HAVE_WX26
             m_chat_tabs->SetPageImage( i, m_icon_index );
             #else
             m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
             #endif
				}
			}
	}
	OutputLine( _T( " * " ) + who + _T( " " ) + action, sett().GetChatColorAction(), sett().GetChatFont() );
}


//! @brief Output motd sent by server
//!
//! @param message The MOTD message to output
void ChatPanel::Motd( const wxString& message ) {
	wxFont f = m_chatlog_text->GetFont();
	f.SetFamily( wxFONTFAMILY_MODERN );
  // change the image of the tab to show new events
	if (  m_ui.GetActiveChatPanel() != this && m_chat_tabs )
	{
		for ( unsigned int i = 0; i <  m_chat_tabs->GetPageCount( ); ++i )
			if ( m_chat_tabs->GetPage( i ) == this ) {
				if ( m_type == CPT_Channel && m_icon_index < 6 )
				{
             m_icon_index = 6;
             #ifdef HAVE_WX26
             m_chat_tabs->SetPageImage( i, m_icon_index );
             #else
             m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
             #endif
				}
				if ( m_type == CPT_User && m_icon_index < 7 )
				{
             m_icon_index = 7;
             #ifdef HAVE_WX26
             m_chat_tabs->SetPageImage( i, m_icon_index );
             #else
             m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
             #endif
				}
			}
	}
	OutputLine( _T( " ** motd ** " ) + message, sett().GetChatColorServer(), f );
}


void ChatPanel::StatusMessage( const wxString& message ) {
	if ( m_chatlog_text == 0 ) {
		wxLogMessage( _T( "m_chatlog_text is NULL" ) );
	} else {
		wxFont f = m_chatlog_text->GetFont();
		f.SetFamily( wxFONTFAMILY_MODERN );
		OutputLine( _T( " ** Server ** " ) + message, sett().GetChatColorServer(), f );
	}
}


void ChatPanel::ClientMessage( const wxString& message ) {
	wxFont f = m_chatlog_text->GetFont();
	f.SetFamily( wxFONTFAMILY_MODERN );
	OutputLine( _T( " ** " ) + message, sett().GetChatColorClient(), f );
}


void ChatPanel::UnknownCommand( const wxString& command, const wxString& params ) {
	wxFont f = m_chatlog_text->GetFont();
	f.SetFamily( wxFONTFAMILY_MODERN );
  // change the image of the tab to show new events
	if (  m_ui.GetActiveChatPanel() != this && m_chat_tabs )
	{
		for ( unsigned int i = 0; i <  m_chat_tabs->GetPageCount( ); ++i )
			if ( m_chat_tabs->GetPage( i ) == this ) {
				if ( m_type == CPT_Server && m_icon_index != 10 )
				{
             m_icon_index = 10;
             #ifdef HAVE_WX26
             m_chat_tabs->SetPageImage( i, m_icon_index );
             #else
             m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
             #endif
				}
			}
	}
	OutputLine( _( " !! Command: \"" ) + command + _( "\" params: \"" ) + params + _T( "\"." ), sett().GetChatColorError(), f );
}


wxString ChatPanel::GetChatTypeStr() {
	if ( m_type == CPT_Channel ) return _( "channel" );
	else if ( m_type == CPT_Battle ) return _( "battle" );
	else if ( m_type == CPT_Server ) return _( "server" );


	return _T( "ROOMTYPE FIXME" );
}


void ChatPanel::Joined( User& who ) {
	if ( m_type == CPT_Channel ) {
		if ( sett().GetDisplayJoinLeave( m_channel->GetName() ) ) {
      // change the image of the tab to show new events
      if (  m_ui.GetActiveChatPanel() != this && m_chat_tabs )
      {
        for ( unsigned int i = 0; i <  m_chat_tabs->GetPageCount( ); ++i )
          if ( m_chat_tabs->GetPage( i ) == this ) {
            if ( m_type == CPT_Channel && m_icon_index < 4 )
            {
               m_icon_index = 4;
               #ifdef HAVE_WX26
               m_chat_tabs->SetPageImage( i, m_icon_index );
               #else
               m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
               #endif
            }
            if ( m_type == CPT_User && m_icon_index < 5 )
            {
               m_icon_index = 5;
               #ifdef HAVE_WX26
               m_chat_tabs->SetPageImage( i, m_icon_index );
               #else
               m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
               #endif
            }
          }
      }
      OutputLine( _T( " ** " ) + who.GetNick() + _( " joined the " ) + GetChatTypeStr() + _T( "." ), sett().GetChatColorJoinPart(), sett().GetChatFont() );

    }
		if ( m_show_nick_list ) m_nicklist->AddUser( who );
	} else if ( m_type == CPT_Battle ) {
		if ( sett().GetDisplayJoinLeave( _T( "game/battle" ) ) ) { OutputLine( _T( " ** " ) + who.GetNick() + _( " joined the " ) + GetChatTypeStr() + _T( "." ), sett().GetChatColorJoinPart(), sett().GetChatFont() ); }
	}

	// Also add the User to the TextCompletionDatabase
	textcompletiondatabase.Insert_Mapping( who.GetNick(), who.GetNick() );
}


void ChatPanel::OnChannelJoin( User& who ) {
	if ( m_type == CPT_Channel && m_show_nick_list ) m_nicklist->AddUser( who );

	// Also add the User to the TextCompletionDatabase
	textcompletiondatabase.Insert_Mapping( who.GetNick(), who.GetNick() );
}


void ChatPanel::Parted( User& who, const wxString& message ) {
	if ( m_type == CPT_Channel ) {
		if ( sett().GetDisplayJoinLeave( m_channel->GetName() ) ) {
      // change the image of the tab to show new events
      if (  m_ui.GetActiveChatPanel() != this && m_chat_tabs )
      {
        for ( unsigned int i = 0; i <  m_chat_tabs->GetPageCount( ); ++i )
          if ( m_chat_tabs->GetPage( i ) == this ) {
            if ( m_type == CPT_Channel && m_icon_index < 4 )
            {
               m_icon_index = 4;
               #ifdef HAVE_WX26
               m_chat_tabs->SetPageImage( i, m_icon_index );
               #else
               m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
               #endif
            }
            if ( m_type == CPT_User && m_icon_index < 5 )
            {
               m_icon_index = 5;
               #ifdef HAVE_WX26
               m_chat_tabs->SetPageImage( i, m_icon_index );
               #else
               m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
               #endif
            }
          }
      }
		  OutputLine( _T( " ** " ) + who.GetNick() + _( " left the " ) + GetChatTypeStr() + _T( "( " ) + message + _T( " )." ), sett().GetChatColorJoinPart(), sett().GetChatFont() );
    }
		if ( m_show_nick_list ) m_nicklist->RemoveUser( who );

		if ( m_channel == 0 ) return;
		if ( &who == &m_channel->GetMe() ) {
			m_channel->uidata.panel = 0;
			SetChannel( 0 );
		}
	} else if ( m_type == CPT_Battle ) {
		if ( sett().GetDisplayJoinLeave( _T( "game/battle" ) ) )  { OutputLine( _T( " ** " ) + who.GetNick() + _( " left the " ) + GetChatTypeStr() + _T( "( " ) + message + _T( " )." ), sett().GetChatColorJoinPart(), sett().GetChatFont() ); }
	}

	// Also remove the User from the TextCompletionDatabase
	textcompletiondatabase.Delete_Mapping( who.GetNick() );
}


void ChatPanel::SetTopic( const wxString& who, const wxString& message ) {
	wxString refined = message;
	refined.Replace( _T( "\\n" ), _T( "\n" ) );
	/*
	int pos = refined.Find( _T("\\n") ); // serch for the \n string
	while ( pos != -1 )
	{
	  if ( refined.Mid( pos - 1, 3 ) == _T("\\\n") ) continue; // the string \\n means escaped \n
	  refined = refined.Left ( pos -1 ) + _T("\n") + refined.Right( pos +1 ); // replace the /n string with the carriage return char
	  pos = refined.Find( _T("\\n") );
	}
	*/
	wxFont f = m_chatlog_text->GetFont();
	f.SetFamily( wxFONTFAMILY_MODERN );
  // change the image of the tab to show new events
  if (  m_ui.GetActiveChatPanel() != this && m_chat_tabs )
  {
    for ( unsigned int i = 0; i <  m_chat_tabs->GetPageCount( ); ++i )
      if ( m_chat_tabs->GetPage( i ) == this ) {
        if ( m_type == CPT_Channel && m_icon_index < 6 )
        {
             m_icon_index = 6;
               #ifdef HAVE_WX26
               m_chat_tabs->SetPageImage( i, m_icon_index );
               #else
               m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
               #endif
        }
        if ( m_type == CPT_User && m_icon_index < 7 )
        {
             m_icon_index = 7;
               #ifdef HAVE_WX26
               m_chat_tabs->SetPageImage( i, m_icon_index );
               #else
               m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
               #endif
        }
      }
  }
	OutputLine( _T( " ** Channel topic: " ) + refined + _( "\n * Set by " ) + who, sett().GetChatColorServer(), f );
}


void ChatPanel::UserStatusUpdated( User& who ) {
	if ( m_show_nick_list ) {
	  try
	  {
		ASSERT_LOGIC( m_nicklist != 0, _T( "m_nicklist = 0" ) );
		m_nicklist->UserUpdated( who );
	  } catch (...) { return; }
	}
}


Channel& ChatPanel::GetChannel() {
	return *m_channel;
}


void ChatPanel::SetChannel( Channel* chan ) {
	ASSERT_LOGIC( this, _T( "this==null" ) );
	ASSERT_LOGIC( m_type == CPT_Channel, _T( "Not of type channel" ) );

	if (( chan == 0 ) && ( m_channel != 0 ) ) {
		/// causes weird crash.
		StatusMessage( _( "Chat closed." ) );

		m_channel->uidata.panel = 0;
		if ( m_show_nick_list && m_nicklist ) {
			m_nicklist->ClearUsers();
		}
	} else if ( chan != 0 ) {
		chan->uidata.panel = this;
		delete m_chat_log;
		m_chat_log = new ChatLog( sett().GetDefaultServer(), chan->GetName() );
	}
	m_channel = chan;
}


Server* ChatPanel::GetServer() {
	return m_server;
}


void ChatPanel::SetServer( Server* serv ) {
	ASSERT_LOGIC( m_type == CPT_Server, _T( "Not of type server" ) );
	if (( serv == 0 ) && ( m_server != 0 ) ) m_server->uidata.panel = 0;
	else if ( serv != 0 ) serv->uidata.panel = this;
	m_server = serv;
	delete m_chat_log;
	if ( m_server ) m_chat_log = new ChatLog( sett().GetDefaultServer(), _( "_SERVER" ) );
	else m_chat_log = 0;
}


User* ChatPanel::GetUser() {
	return m_user;
}


void ChatPanel::SetUser( User* usr ) {
	ASSERT_LOGIC( m_type == CPT_User, _T( "Not of type user" ) );

	if (( usr == 0 ) && ( m_user != 0 ) ) m_user->uidata.panel = 0;
	else if ( usr != 0 ) usr->uidata.panel = this;

	m_user = usr;

	delete m_chat_log;
	if ( m_user ) m_chat_log = new ChatLog( sett().GetDefaultServer(), usr->GetNick() );
	else m_chat_log = 0;
}


bool ChatPanel::IsServerPanel() {
	return ( m_type == CPT_Server );
}


ChatPanelType ChatPanel::GetPanelType() {
	return m_type;
}


//! @brief Set the Channel object
//!
//! @param channel the Channel object.
void ChatPanel::_SetChannel( Channel* channel ) {
	if ( m_channel != 0 ) {
		m_channel->uidata.panel = 0;
	}

	m_channel = channel;

	if ( m_channel != 0 ) {
		m_channel->uidata.panel = this;
	}

}


void ChatPanel::Say( const wxString& message ) {
	wxLogDebugFunc( _T( "" ) );
	wxStringTokenizer lines( message, _T( "\n" ) );
	if ( lines.CountTokens() > 5 ) {
		wxMessageDialog dlg( &m_ui.mw(), wxString::Format( _( "Are you sure you want to paste %d lines?" ), lines.CountTokens() ), _( "Flood warning" ), wxYES_NO );
		if ( dlg.ShowModal() == wxID_NO ) return;
	}
	while ( lines.HasMoreTokens() ) {
		wxString line = lines.GetNextToken();
		wxLogMessage( _T( "line: %s" ), line.c_str() );

		if ( line.Find( '/' ) == 0 ) {
			if ( m_ui.ExecuteSayCommand( line ) ) return;
		}

		if ( line == _T( "/ver" ) ) {
			OutputLine( _( " You have SpringLobby v" ) + GetSpringLobbyVersion(), sett().GetChatColorNormal() , sett().GetChatFont() );
			return;
		}

		if ( m_type == CPT_Channel ) {

			if ( m_channel == 0 ) {
				OutputLine( _( " You are not in channel or channel does not exist." ), sett().GetChatColorError(), sett().GetChatFont() );
				return;
			}
			if ( line.StartsWith( _T( "/" ) ) ) {
				if ( m_channel->ExecuteSayCommand( line ) ) return;
				if ( m_channel->GetServer().ExecuteSayCommand( line ) ) return;
				OutputLine( wxString::Format( _( " Error: Command (%s) does not exist, use /help for a list of available commands." ), line.c_str() ), sett().GetChatColorError(), sett().GetChatFont() );
				return;
			}
			m_channel->Say( line );

		} else if ( m_type == CPT_Battle ) {

			if ( m_battle == 0 ) {
				OutputLine( _( " You are not in battle or battle does not exist, use /help for a list of available commands." ), sett().GetChatColorError(), sett().GetChatFont() );
				return;
			}
			if ( line.StartsWith( _T( "/" ) ) ) {
				if ( m_battle->ExecuteSayCommand( line ) ) return;
				if ( m_battle->GetServer().ExecuteSayCommand( line ) ) return;
				OutputLine( wxString::Format( _( " Error: Command (%s) does not exist, use /help for a list of available commands." ), line.c_str() ), sett().GetChatColorError(), sett().GetChatFont() );
				return;
			}
			m_battle->Say( line );

		} else if ( m_type == CPT_User ) {

			if ( m_user == 0 ) {
				OutputLine( _( " User is offline." ), sett().GetChatColorError(), sett().GetChatFont() );
				return;
			}
			if ( line.StartsWith( _T( "/" ) ) ) {
				if ( m_user->ExecuteSayCommand( line ) ) return;
				if ( m_user->GetServer().ExecuteSayCommand( line ) ) return;
				OutputLine( wxString::Format( _( " Error: Command (%s) does not exist, use /help for a list of available commands." ), line.c_str() ), sett().GetChatColorError(), sett().GetChatFont() );
				return;
			}
			m_user->Say( line );

		} else if ( m_type == CPT_Server ) {
			if ( m_server == 0 ) return;

			if ( line.StartsWith( _T( "/" ) ) ) {
				if ( m_server->ExecuteSayCommand( line ) ) return;
				OutputLine( wxString::Format( _( " Error: Command (%s) does not exist, use /help for a list of available commands." ), line.c_str() ), sett().GetChatColorError(), sett().GetChatFont() );
				return;
			}

            //we need to close the channel tab if leaving manually
            if (line.Upper().StartsWith( _T( "LEAVE" ) ) ) {
                wxString channame = line.AfterFirst(' ').BeforeFirst(' ');
                ui().OnLeaveChannel( channame );
            }

			m_server->SendRaw( line );
			OutputLine( _( " Sent: \"" ) + line + _( "\"" ), sett().GetChatColorNormal(), sett().GetChatFont() );
		}

	}
}


void ChatPanel::Part() {
	wxLogDebugFunc( _T( "" ) );
	if ( m_type == CPT_Channel ) {
		if ( m_channel == 0 ) return;
		m_channel->Leave();
		m_channel->uidata.panel = 0;
	}
}


void ChatPanel::LogTime() {
	if ( !m_chatlog_text ) return;
	wxDateTime now = wxDateTime::Now();
	m_chatlog_text->SetDefaultStyle( wxTextAttr( sett().GetChatColorTime() ) );
	m_chatlog_text->SetBackgroundColour( sett().GetChatColorBackground() );
	m_chatlog_text->SetFont( sett().GetChatFont() );
	m_chatlog_text->AppendText( _T( "[" ) + now.Format( _T( "%H:%M:%S" ) ) + _T( "]" ) );
}


bool ChatPanel::IsOk() {
	if ( m_type == CPT_Channel ) return ( m_channel != 0 );
	if ( m_type == CPT_Server ) return ( m_server != 0 );
	if ( m_type == CPT_User ) return ( m_user != 0 );
	if ( m_type == CPT_Battle ) return ( m_battle != 0 );
	return false;
}



void ChatPanel::OnUserDisconnected() {
  // change the image of the tab to show new events
  if (  m_ui.GetActiveChatPanel() != this && m_chat_tabs )
  {
    for ( unsigned int i = 0; i <  m_chat_tabs->GetPageCount( ); ++i )
      if ( m_chat_tabs->GetPage( i ) == this ) {
        if ( m_type == CPT_Channel && m_icon_index < 4 )
        {
             m_icon_index = 4;
               #ifdef HAVE_WX26
               m_chat_tabs->SetPageImage( i, m_icon_index );
               #else
               m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
               #endif
        }
        if ( m_type == CPT_User && m_icon_index < 5 )
        {
             m_icon_index = 5;
                #ifdef HAVE_WX26
               m_chat_tabs->SetPageImage( i, m_icon_index );
               #else
               m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
               #endif
        }
      }
  }
	OutputLine( _T( " ** User is now offline." ), sett().GetChatColorJoinPart(), sett().GetChatFont() );
}


void ChatPanel::OnUserConnected() {
  // change the image of the tab to show new events
  if (  m_ui.GetActiveChatPanel() != this && m_chat_tabs )
  {
    for ( unsigned int i = 0; i <  m_chat_tabs->GetPageCount( ); ++i )
      if ( m_chat_tabs->GetPage( i ) == this ) {
        if ( m_type == CPT_Channel && m_icon_index < 4 )
        {
             m_icon_index = 4;
               #ifdef HAVE_WX26
               m_chat_tabs->SetPageImage( i, m_icon_index );
               #else
               m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
               #endif
        }
        if ( m_type == CPT_User && m_icon_index < 5 )
        {
             m_icon_index = 5;
               #ifdef HAVE_WX26
               m_chat_tabs->SetPageImage( i, m_icon_index );
               #else
               m_chat_tabs->SetPageBitmap( i, m_imagelist->GetBitmap(m_icon_index));
               #endif
        }
      }
  }
	OutputLine( _T( " ** User just got online." ), sett().GetChatColorJoinPart(), sett().GetChatFont() );
}


//////////////////////////////////////////////////////////////////////////////////////
// Menu Events
//////////////////////////////////////////////////////////////////////////////////////


void ChatPanel::OnMenuSelectAll( wxCommandEvent& event ) {
	m_chatlog_text->SetSelection( 0, m_chatlog_text->GetLastPosition() );
}


void ChatPanel::OnMenuCopy( wxCommandEvent& event ) {
	m_chatlog_text->Copy();
}


void ChatPanel::OnChannelMenuLeave( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	Part();
	SetChannel( 0 );
}

void ChatPanel::OnChannelMenuDisplayJoinLeave( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( !displayjoinitem->IsChecked() ) {
		sett().SetDisplayJoinLeave( false, m_channel->GetName() );
		displayjoinitem->Check( false );
	} else {
		sett().SetDisplayJoinLeave( true, m_channel->GetName() );
		displayjoinitem->Check( true );
	}
}

void ChatPanel::OnChannelAutoJoin( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( m_autorejoin == 0 ) return;

	if ( m_autorejoin->IsChecked() ) {
			sett().AddChannelJoin( m_channel->GetName(), m_channel->GetPassword() );
			m_autorejoin->Check( true );
	} else {
		sett().RemoveChannelJoin( m_channel->GetName() );
		m_autorejoin->Check( false );
	}
}


void ChatPanel::OnChannelMenuInfo( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( !m_channel->UserExists( _T( "ChanServ" ) ) ) {
		m_ui.ShowMessage( _( "ChanServ error" ), _( "ChanServ is not in this channel." ) );
		return;
	}
	User& cs = m_channel->GetUser( _T( "ChanServ" ) );

	cs.Say( _T( "!INFO #" ) + m_channel->GetName() );
	//INFO /<channame>/
}


void ChatPanel::OnChannelMenuTopic( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( !m_channel->UserExists( _T( "ChanServ" ) ) ) {
		m_ui.ShowMessage( _( "ChanServ error" ), _( "ChanServ is not in this channel." ) );
		return;
	}
	User& cs = m_channel->GetUser( _T( "ChanServ" ) );

	wxString topic = m_channel->GetTopic();
	topic.Replace( _T("\\n"), _T("\n") );
	if ( !m_ui.AskText( _( "Set topic..." ), _( "What should be the new topic?" ), topic, wxOK | wxCANCEL | wxCENTRE | wxTE_MULTILINE ) ) return;
  topic.Replace( _T("\n"), _T("\\n") );
	cs.Say( _T( "!TOPIC #" ) + m_channel->GetName() + _T( " " ) + topic );
	//TOPIC /<channame>/ {topic}
}


void ChatPanel::OnChannelMenuMessage( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( !m_channel->UserExists( _T( "ChanServ" ) ) ) {
		m_ui.ShowMessage( _( "ChanServ error" ), _( "ChanServ is not in this channel." ) );
		return;
	}
	User& cs = m_channel->GetUser( _T( "ChanServ" ) );

	wxString text;
if ( !m_ui.AskText( _( "Channel message..." ), _( "Message:" ), text ) ) return;

	cs.Say( _T( "!CHANMSG #" ) + m_channel->GetName() + _T( " " ) + text );
	//CHANMSG /<channame>/ {message}
}


void ChatPanel::OnChannelMenuLock( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( !m_channel->UserExists( _T( "ChanServ" ) ) ) {
		m_ui.ShowMessage( _( "ChanServ error" ), _( "ChanServ is not in this channel." ) );
		return;
	}
	User& cs = m_channel->GetUser( _T( "ChanServ" ) );

	wxString text;
	if ( !m_ui.AskText( _( "Lock channel..." ), _( "What should the new password be?" ), text ) ) return;

	cs.Say( _T( "!LOCK #" ) + m_channel->GetName() + _T( " " ) + text );
	//LOCK /<channame>/ <key>
}


void ChatPanel::OnChannelMenuUnlock( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( !m_channel->UserExists( _T( "ChanServ" ) ) ) {
		m_ui.ShowMessage( _( "ChanServ error" ), _( "ChanServ is not in this channel." ) );
		return;
	}
	User& cs = m_channel->GetUser( _T( "ChanServ" ) );

	if ( !m_ui.Ask( _( "Unlock Channel" ), _( "Are you sure you want to unlock this channel?" ) ) ) return;

	cs.Say( _T( "!UNLOCK #" ) + m_channel->GetName() );
	//UNLOCK /<channame>/
}


void ChatPanel::OnChannelMenuRegister( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( !m_channel->GetServer().UserExists( _T( "ChanServ" ) ) ) {
		m_ui.ShowMessage( _( "ChanServ error" ), _( "ChanServ is not on this server." ) );
		return;
	}
	User& cs = m_channel->GetServer().GetUser( _T( "ChanServ" ) );

	wxString text = m_channel->GetMe().GetNick();
	if ( !m_ui.AskText( _( "Register Channel" ), _( "Who should be appointed founder of this channel?" ), text ) ) return;

	cs.Say( _T( "!REGISTER #" ) + m_channel->GetName() + _T( " " ) + text );
	//REGISTER <channame> <founder>
}


void ChatPanel::OnChannelMenuUnregister( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( !m_channel->GetServer().UserExists( _T( "ChanServ" ) ) ) {
		m_ui.ShowMessage( _( "ChanServ error" ), _( "ChanServ is not on this server." ) );
		return;
	}
	User& cs = m_channel->GetServer().GetUser( _T( "ChanServ" ) );

	if ( !m_ui.Ask( _( "Unregister Channel" ), _( "Are you sure you want to unregister this channel?" ) ) ) return;

	cs.Say( _T( "!UNREGISTER #" ) + m_channel->GetName() );
	//UNREGISTER <channame>
}


void ChatPanel::OnChannelMenuSpamOn( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( !m_channel->UserExists( _T( "ChanServ" ) ) ) {
		m_ui.ShowMessage( _( "ChanServ error" ), _( "ChanServ is not in this channel." ) );
		return;
	}
	User& cs = m_channel->GetUser( _T( "ChanServ" ) );

	cs.Say( _T( "!SPAMPROTECTION #" ) + m_channel->GetName() + _T( " on" ) );
	//SPAMPROTECTION /<channame>/ <on|off>
}


void ChatPanel::OnChannelMenuSpanOff( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( !m_channel->UserExists( _T( "ChanServ" ) ) ) {
		m_ui.ShowMessage( _( "ChanServ error" ), _( "ChanServ is not in this channel." ) );
		return;
	}
	User& cs = m_channel->GetUser( _T( "ChanServ" ) );

	cs.Say( _T( "!SPAMPROTECTION #" ) + m_channel->GetName() + _T( " off" ) );
	//SPAMPROTECTION /<channame>/ <on|off>
}


void ChatPanel::OnChannelMenuSpamIsOn( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( !m_channel->UserExists( _T( "ChanServ" ) ) ) {
		m_ui.ShowMessage( _( "ChanServ error" ), _( "ChanServ is not in this channel." ) );
		return;
	}
	User& cs = m_channel->GetUser( _T( "ChanServ" ) );

	cs.Say( _T( "!SPAMPROTECTION #" ) + m_channel->GetName() );
	//SPAMPROTECTION /<channame>/
}


void ChatPanel::OnServerMenuDisconnect( wxCommandEvent& event ) {
	m_ui.Disconnect();
}


void ChatPanel::OnServerMenuReconnect( wxCommandEvent& event ) {
	m_ui.Reconnect();
}


void ChatPanel::OnServerMenuRemove( wxCommandEvent& event ) {
	wxString user;
	if ( !m_ui.AskText( _( "Remove User Acount" ), _( "What user account do you want to remove today?" ), user ) ) return;
	if ( !m_ui.Ask( _( "Remove Account" ), _( "Are you sure you want to remove the account " ) + user + _T( "?" ) ) ) return;
	Say( _T( "removeaccount " ) + user );
}


void ChatPanel::OnServerMenuChangePassword( wxCommandEvent& event ) {
	wxString user, password;
	if ( !m_ui.AskText( _( "Change User Acount Password" ), _( "What user account do you want to change the password for?" ), user ) ) return;
	if ( !m_ui.AskPassword( _( "Change User Acount Password" ), _( "What would be the new password?" ), password ) ) return;
	Say( _T( "changeaccountpass " ) + user + _T( " " ) + password );
}


void ChatPanel::OnServerMenuSetAccess( wxCommandEvent& event ) {
	m_ui.ShowMessage( _( "Error" ), _( "Not Implemented" ) );
}


void ChatPanel::OnServerMenuBroadcast( wxCommandEvent& event ) {
	wxString msg;
	if ( !m_ui.AskText( _( "Broadcast Message" ), _( "Message to be broadcasted:" ), msg ) ) return;
	Say( _T( "broadcast " ) + msg );
}


void ChatPanel::OnUserMenuOpenChat( wxCommandEvent& event ) {
	User* user = GetSelectedUser();
	if ( user == 0 ) return;

	m_ui.mw().OpenPrivateChat( *user );
}


void ChatPanel::OnUserMenuJoinSame( wxCommandEvent& event ) {
	User* user = GetSelectedUser();
	if ( user == 0 ) return;
	Battle* battle = user->GetBattle();
	if ( battle == 0 ) return;

	if ( !usync()->ModExists( battle->GetHostModName() ) ) {
		customMessageBoxNoModal( SL_MAIN_ICON, _( "You don't have the mod " ) + battle->GetHostModName()
														 + _( " . Please download it first" ), _( "Mod unavailable" ) );
		return;
	}

	wxString password;
	if ( battle->IsPassworded() ) {
		if ( !m_ui.AskPassword( _( "Battle password" ), _( "This battle is password protected, enter the password." ), password ) ) return;
	}
	battle->Join( password );
}


void ChatPanel::OnUserMenuSlap( wxCommandEvent& event ) {
	User* user = GetSelectedUser();
	if ( user == 0 ) return;

	if ( m_type == CPT_Channel ) {
		if ( m_channel == 0 ) return;
		m_channel->DoAction( _T( "Slaps " ) + user->GetNick() + _T( " around with a large PeeWee!" ) );
	} else if ( m_type == CPT_User ) {
		if ( m_user == 0 ) return;
		m_user->DoAction( _T( "slaps " ) + user->GetNick() + _T( " around with a large PeeWee!" ) );
	}
}


void ChatPanel::OnUserMenuMute( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( !m_channel->UserExists( _T( "ChanServ" ) ) ) {
		m_ui.ShowMessage( _( "ChanServ error" ), _( "ChanServ is not in this channel." ) );
		return;
	}

	wxString mutetime = _T( "5" );
	if ( !m_ui.AskText( _( "Mute User" ), _( "For how many minutes should the user be muted?" ), mutetime ) ) return;

	User& cs = m_channel->GetUser( _T( "ChanServ" ) );

	User* user = GetSelectedUser();
	if ( user == 0 ) return;

	cs.Say( _T( "!MUTE #" ) + m_channel->GetName() + _T( " " ) + user->GetNick() + _T( " " ) + mutetime );
	//MUTE /<channame>/ <username> [<duration>]

}


void ChatPanel::OnUserMenuUnmute( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( !m_channel->UserExists( _T( "ChanServ" ) ) ) {
		m_ui.ShowMessage( _( "ChanServ error" ), _( "ChanServ is not in this channel." ) );
		return;
	}
	User& cs = m_channel->GetUser( _T( "ChanServ" ) );

	User* user = GetSelectedUser();
	if ( user == 0 ) return;

	cs.Say( _T( "!UNMUTE #" ) + m_channel->GetName() + _T( " " ) + user->GetNick() );
	//UNMUTE /<channame>/ <username>
}


void ChatPanel::OnUserMenuKick( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( !m_channel->UserExists( _T( "ChanServ" ) ) ) {
		m_ui.ShowMessage( _( "ChanServ error" ), _( "ChanServ is not in this channel." ) );
		return;
	}

	User* user = GetSelectedUser();
	if ( user == 0 ) return;

	wxString msg;
	if ( !m_ui.AskText( _( "Kick User" ), _( "Reason:" ), msg ) ) return;

	User& cs = m_channel->GetUser( _T( "ChanServ" ) );

	if ( msg != wxEmptyString ) msg = _T( " " ) + msg;
	cs.Say( _T( "!KICK #" ) + m_channel->GetName() + _T( " " ) + user->GetNick() + msg );
	//KICK /<channame>/ <username> [{reason}]
}


void ChatPanel::OnUserMenuOp( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( !m_channel->UserExists( _T( "ChanServ" ) ) ) {
		m_ui.ShowMessage( _( "ChanServ error" ), _( "ChanServ is not in this channel." ) );
		return;
	}

	User* user = GetSelectedUser();
	if ( user == 0 ) return;
	User& cs = m_channel->GetUser( _T( "ChanServ" ) );

	cs.Say( _T( "!OP #" ) + m_channel->GetName() + _T( " " ) + user->GetNick() );
	//OP /<channame>/ <username>
}


void ChatPanel::OnUserMenuDeop( wxCommandEvent& event ) {
	if ( m_channel == 0 ) return;
	if ( !m_channel->UserExists( _T( "ChanServ" ) ) ) {
		m_ui.ShowMessage( _( "ChanServ error" ), _( "ChanServ is not in this channel." ) );
		return;
	}

	User* user = GetSelectedUser();
	if ( user == 0 ) return;
	User& cs = m_channel->GetUser( _T( "ChanServ" ) );

	cs.Say( _T( "!DEOP #" ) + m_channel->GetName() + _T( " " ) + user->GetNick() );
	//DEOP /<channame>/ <username>
}


void ChatPanel::OnUserMenuModeratorIngame( wxCommandEvent& event ) {
	m_ui.GetServer().RequestInGameTime( GetSelectedUser()->GetNick() );
}


void ChatPanel::OnUserMenuModeratorCurrentIP( wxCommandEvent& event ) {
	m_ui.GetServer().ModeratorGetIP( GetSelectedUser()->GetNick() );
}


void ChatPanel::OnUserMenuModeratorKick( wxCommandEvent& event ) {
	wxString reason;
	if ( !m_ui.AskText( _( "Kick user" ), _( "Reason:" ), reason ) ) return;
	m_ui.GetServer().ModeratorKick( GetSelectedUser()->GetNick(), reason );
}


void ChatPanel::OnUserMenuModeratorBan( wxCommandEvent& event ) {
	m_ui.ShowMessage( _( "Error" ), _( "Not Implemented" ) );
}


void ChatPanel::OnUserMenuModeratorUnban( wxCommandEvent& event ) {
	m_ui.ShowMessage( _( "Error" ), _( "Not Implemented" ) );
}


void ChatPanel::OnUserMenuModeratorMute( wxCommandEvent& event ) {
	wxString duration;
	if ( !m_ui.AskText( _( "Mute user" ), _( "Duration:" ), duration ) ) return;
	long int dur = 0;
	duration.ToLong( &dur, dur );
	m_ui.GetServer().ModeratorMute( m_channel->GetName(), GetSelectedUser()->GetNick(), ( int ) dur, false );
}


void ChatPanel::OnUserMenuModeratorMute5( wxCommandEvent& event ) {
	m_ui.GetServer().ModeratorMute( m_channel->GetName(), GetSelectedUser()->GetNick(), 5, false );
}


void ChatPanel::OnUserMenuModeratorMute10( wxCommandEvent& event ) {
	m_ui.GetServer().ModeratorMute( m_channel->GetName(), GetSelectedUser()->GetNick(), 10, false );
}


void ChatPanel::OnUserMenuModeratorMute30( wxCommandEvent& event ) {
	m_ui.GetServer().ModeratorMute( m_channel->GetName(), GetSelectedUser()->GetNick(), 30, false );
}


void ChatPanel::OnUserMenuModeratorMute120( wxCommandEvent& event ) {
	m_ui.GetServer().ModeratorMute( m_channel->GetName(), GetSelectedUser()->GetNick(), 120, false );
}


void ChatPanel::OnUserMenuModeratorMute1440( wxCommandEvent& event ) {
	m_ui.GetServer().ModeratorMute( m_channel->GetName(), GetSelectedUser()->GetNick(), 1440, false );
}



void ChatPanel::OnUserMenuModeratorUnmute( wxCommandEvent& event ) {
	m_ui.GetServer().ModeratorUnmute( m_channel->GetName(), GetSelectedUser()->GetNick() );
}


void ChatPanel::OnUserMenuModeratorRing( wxCommandEvent& event ) {
	m_ui.GetServer().Ring( GetSelectedUser()->GetNick() );
}

void ChatPanel::FocusInputBox()
{
    m_say_text->SetFocus();
}

void ChatPanel::OnUserMenuAddToGroup( wxCommandEvent& event )
{
    int id  = event.GetId() - GROUP_ID;
    wxString groupname = m_usermenu->GetGroupByEvtID(id);
    User* user = GetSelectedUser();
    if ( user )
    useractions().AddUserToGroup( groupname, user->GetNick() );
}

void ChatPanel::OnUserMenuDeleteFromGroup( wxCommandEvent& event )
{
    User* user = GetSelectedUser();
    if ( user )
        useractions().RemoveUser( user->GetNick() );
}

void ChatPanel::OnUserMenuCreateGroup( wxCommandEvent& event )
{
    wxString name;
    if ( ui().AskText( _("Enter name"),
        _("Please enter the name for the new group.\nAfter clicking ok you will be taken to adjust its settings."), name ) )
    {
        User* user = GetSelectedUser();
        if ( user ) {
            useractions().AddGroup( name );
            useractions().AddUserToGroup( name, user->GetNick() );
            ui().mw().ShowConfigure( OPT_PAGE_GROUPS );
        }
        else
            customMessageBox( SL_MAIN_ICON, _("couldn't add user"), _("Error") );
    }


}

UserMenu::UserMenu(ChatPanel* parent,const wxString& title, long style)
    : wxMenu( title, style ),m_groupsMenu(0), m_parent(parent),m_groupCounter(0)
{
    m_groupsMenu = new wxMenu();
    m_groupsnewItem = new wxMenuItem( m_groupsMenu, GROUP_ID - 2, _("Create new group...")  );
    m_parent->Connect( GROUP_ID - 2, wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler( ChatPanel::OnUserMenuCreateGroup ) );
    m_groupsMenu->Append( m_groupsnewItem );
    m_groupsMenu->AppendSeparator();
//    if ( !ui().IsThisMe( m_parent->GetSelectedUser() ) )
    m_groupsMenuItem = AppendSubMenu( m_groupsMenu, _("Add to group..."));
    m_groupsDeleteItem = new wxMenuItem( m_groupsMenu, GROUP_ID - 1, _("Remove from group")  );
    m_parent->Connect( GROUP_ID - 1, wxEVT_COMMAND_MENU_SELECTED,
                            wxCommandEventHandler( ChatPanel::OnUserMenuDeleteFromGroup ) );
    Append( m_groupsDeleteItem );
}

UserMenu::~UserMenu()
{

}
void UserMenu::EnableItems(bool isUserSelected)
{
    if ( isUserSelected )
    {
        User* user = m_parent->GetSelectedUser();
        bool enable = ( user != 0 && ( !ui().IsThisMe( user ) ) );
        m_groupsMenuItem->Enable( enable && !useractions().IsKnown( user->GetNick() ) ) ;
        m_groupsnewItem->Enable( enable && !useractions().IsKnown( user->GetNick() ) ) ;
        m_groupsDeleteItem->Enable( enable && useractions().IsKnown( user->GetNick() ) ) ;
        UpdateGroups();
    }
    else
    {
        m_groupsMenuItem->Enable( false ) ;
        m_groupsDeleteItem->Enable( false ) ;
        m_groupsnewItem->Enable( false );
    }

}

void UserMenu::UpdateGroups()
{
    wxSortedArrayString groupNames = useractions().GetGroupNames();
    bool first = m_oldGroups.GetCount() == 0;
    if ( first )
        m_oldGroups = groupNames;
    for ( unsigned int i = 0; i < groupNames.GetCount(); ++i)
    {
        if ( m_oldGroups.Index( groupNames[i] ) == wxNOT_FOUND || first )
        {
            m_idNameMap[m_groupCounter] = groupNames[i];
            wxMenuItem* addItem = new wxMenuItem( m_groupsMenu, GROUP_ID + m_groupCounter ,  groupNames[i] , wxEmptyString, wxITEM_NORMAL );
            m_groupsMenu->Append( addItem );
            m_parent->Connect( GROUP_ID + m_groupCounter, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( ChatPanel::OnUserMenuAddToGroup ) );
            m_oldGroups.Add( groupNames[i] );
            m_idNameMap[GROUP_ID + m_groupCounter]  = groupNames[i];
            m_NameIdMap[groupNames[i]]  = GROUP_ID + m_groupCounter;
            m_groupCounter++;
        }
        else
        {
            //wxMenuItem* old = FindItem( m_NameIdMap[groupNames[i]] );
            Destroy( m_NameIdMap[groupNames[i]] );
        }
    }
}

wxString UserMenu::GetGroupByEvtID( const unsigned int id )
{
    if ( id < m_idNameMap.size() )
        return m_idNameMap[id];
    else
        return wxEmptyString;
}

void ChatPanel::UpdateNicklistHighlights()
{
    if (m_nicklist != 0) m_nicklist->UpdateHighlights();
}
