/* Copyright (C) 2007-2011 The SpringLobby Team. All rights reserved. */
//
// Class: Ui
//

#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER


#include <wx/textdlg.h>
#include <stdexcept>
#include <wx/intl.h>
#include <wx/utils.h>
#include <wx/debugrpt.h>
#include <wx/filename.h>
#include <wx/app.h>

#include "introguide.h"
#include "ui.h"
#include "tasserver.h"
#include "settings.h"
#include "server.h"
#include "spring.h"
#include "channel/channel.h"
#include "connectwindow.h"
#include "mainwindow.h"
#include "user.h"
#include "utils/debug.h"
#include "utils/conversion.h"
#include "utils/uievents.h"
#include "updater/updatehelper.h"
#include "uiutils.h"
#include "chatpanel.h"
#include "battlelist/battlelisttab.h"
#include "hosting/battleroomtab.h"
#include "hosting/mainjoinbattletab.h"
#include "hosting/mainsingleplayertab.h"
#include "battle.h"
#include "mainchattab.h"
#include "crashreport.h"
#include "maindownloadtab.h"
#include "downloader/prdownloader.h"

#include "agreementdialog.h"

#include "updater/updatehelper.h"
#include <wx/stdpaths.h>

#include "reconnectdialog.h"
#include "utils/customdialogs.h"
#include "utils/platform.h"
#include "updater/versionchecker.h"
#include "sound/alsound.h"
#include <lslutils/globalsmanager.h>
#include "utils/misc.h"

static const unsigned int s_reconnect_delay_ms = 6000;

Ui& ui()
{
	assert(wxThread::IsMain());
    static LSL::Util::LineInfo<Ui> m( AT );
    static LSL::Util::GlobalObjectHolder<Ui, LSL::Util::LineInfo<Ui> > m_ui( m );
    return m_ui;
}

Ui::Ui() :
		wxEvtHandler(),
		m_serv(0),
		m_main_win(0),
		m_con_win(0),
		m_reconnect_dialog(0),
		m_upd_counter_torrent(0),
		m_first_update_trigger(true),
		m_recconecting_wait(false),
		m_battle_info_updatedSink( this, &BattleEvents::GetBattleEventSender( ( BattleEvents::BattleInfoUpdate ) ) )
{
	m_main_win = new MainWindow( );
	CustomMessageBoxBase::setLobbypointer(m_main_win);
	m_serv = new TASServer();
	serverSelector().SetCurrentServer( m_serv );
	ConnectGlobalEvent(this, GlobalEvent::OnSpringTerminated, wxObjectEventFunction(&Ui::OnSpringTerminated));
	ConnectGlobalEvent(this, GlobalEvent::OnQuit, wxObjectEventFunction(&Ui::OnQuit));
}

Ui::~Ui()
{
    Disconnect();
    delete m_serv;
}

ChatPanel* Ui::GetActiveChatPanel()
{
    return mw().GetActiveChatPanel();
}


MainWindow& Ui::mw()
{
	assert(wxThread::IsMain());
	ASSERT_LOGIC( m_main_win != 0, _T("m_main_win = 0") );
	return *m_main_win;
}

const MainWindow& Ui::mw() const
{
	ASSERT_LOGIC( m_main_win != 0, _T("m_main_win = 0") );
	return *m_main_win;
}


bool Ui::IsMainWindowCreated() const
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
	if ( IsConnecting() || IsConnected() || m_recconecting_wait )
		return;
	if ( m_con_win == 0 )
    {
        ASSERT_LOGIC( m_main_win != 0, _T("m_main_win = 0") );
        m_con_win = new ConnectWindow( m_main_win, *this );
    }
	m_con_win->CenterOnParent();
	m_con_win->Show(true);
	m_con_win->Raise();
}


//! @brief Connects to default server or opens the ConnectWindow
//!
//! @todo Fix Auto Connect
//! @see DoConnect
void Ui::Connect()
{
	wxString server_name = sett().GetDefaultServer();
	wxString nick = sett().GetServerAccountNick( server_name );
	bool autoconnect = sett().GetAutoConnect();
	if ( !autoconnect || server_name.IsEmpty() || nick.IsEmpty() )
        ShowConnectWindow();
    else
    {
        m_con_win = 0;
        wxString pass = sett().GetServerAccountPass( server_name );
        DoConnect( server_name, nick, pass);
    }
}


void Ui::Reconnect()
{
    wxString servname = sett().GetDefaultServer();

    wxString pass  = sett().GetServerAccountPass(servname);
    if ( !sett().GetServerAccountSavePass(servname) )
    {
		if ( !AskPassword( _("Server password"), _("Password"), pass ) ) return;
    }

    Disconnect();
    DoConnect( servname, sett().GetServerAccountNick(servname), pass );
}


void Ui::Disconnect()
{
    if ( m_serv != 0 )
    {
        if ( IsConnected() ) {
		serverSelector().GetServer().Disconnect();
        }
    }
}


//! @brief Opens the accutial connection to a server.
void Ui::DoConnect( const wxString& servername, const wxString& username, const wxString& password )
{
	if ( m_reconnect_delay_timer.IsRunning() ) {
		m_recconecting_wait = true;
		AutocloseMessageBox m( &mw(), _("Waiting for reconnect"), wxMessageBoxCaptionStr, s_reconnect_delay_ms );
		int res = m.ShowModal();
		m_recconecting_wait = false;
		if ( res == wxID_CANCEL )
		{
			m_reconnect_delay_timer.Stop();
			return;
		}
	}

    wxString host;
    int port;

	if ( servername != m_last_used_backup_server ) // do not save the server as default if it's a backup one
	{
		sett().SetDefaultServer( servername );
	}
	else
	{
		m_last_used_backup_server = _T("");
	}

    if ( !sett().ServerExists( servername ) )
    {
        ASSERT_LOGIC( false, _T("Server does not exist in settings") );
        return;
    }

    Disconnect();

	serverSelector().GetServer().SetUsername( username );
	serverSelector().GetServer().SetPassword( password );

    if ( sett().GetServerAccountSavePass( servername ) )
    {
		if ( serverSelector().GetServer().IsPasswordHash(password) ) sett().SetServerAccountPass( servername, password );
		else sett().SetServerAccountPass( servername, serverSelector().GetServer().GetPasswordHash( password ) );
    }
    else
    {
        sett().SetServerAccountPass( servername, _T("") );
    }
    sett().SaveSettings();

    host = sett().GetServerHost( servername );
    port = sett().GetServerPort( servername );

	AddServerWindow( servername );
	serverSelector().GetServer().uidata.panel->StatusMessage( _T("Connecting to server ") + servername + _T("...") );

    // Connect
	serverSelector().GetServer().Connect( servername, host, port );

}

void Ui::AddServerWindow( const wxString& servername )
{
	if ( !serverSelector().GetServer().uidata.panel )
	{
		serverSelector().GetServer().uidata.panel = m_main_win->GetChatTab().AddChatPanel( *m_serv, servername );

	}
}


void Ui::ReopenServerTab()
{
	if ( serverSelector().GetServer().IsOnline() )
	{
		AddServerWindow( serverSelector().GetServer().GetServerName() );
		// re-add all users to the user list
		const UserList& list = serverSelector().GetServer().GetUserList();
		for ( unsigned int i = 0; i < list.GetNumUsers(); i++ )
		{
			serverSelector().GetServer().uidata.panel->OnChannelJoin( list.GetUser(i) );
		}
	}
}

bool Ui::DoRegister( const wxString& servername, const wxString& username, const wxString& password,wxString& reason)
{
    wxString host;
    int port;

    if ( !sett().ServerExists( servername ) )
    {
        ASSERT_LOGIC( false, _T("Server does not exist in settings") );
        return false;
    }

    host = sett().GetServerHost( servername );
    port = sett().GetServerPort( servername );
	bool success = serverSelector().GetServer().Register( host, port, username, password,reason );
    if ( success )
    {
			customMessageBox(SL_MAIN_ICON, _("Registration successful,\nyou should now be able to login."), _("Registration successful"), wxOK );
    }
    else
    {
			wxLogWarning( _T("registration failed, reason: %s"), reason.c_str()  );
			if ( reason == _("Connection timed out") || reason.IsEmpty() ) ConnectionFailurePrompt();
			else	customMessageBox(SL_MAIN_ICON,_("Registration failed, the reason was:\n")+ reason , _("Registration failed."), wxOK );
    }
    return success;

}

bool Ui::IsConnecting() const
{
	if ( m_reconnect_dialog != 0 ) return m_reconnect_dialog->IsShown();
	return false;
}

bool Ui::IsConnected() const
{
    if ( m_serv != 0 ) return m_serv->IsConnected();
    return false;
}

void Ui::JoinChannel( const wxString& name, const wxString& password )
{
	if ( m_serv != 0 ) serverSelector().GetServer().JoinChannel( name, password );
}


bool Ui::IsSpringRunning() const
{
    return spring().IsRunning();
}


//! @brief Quits the entire application
void Ui::Quit()
{
    Disconnect();
    if ( m_con_win != 0 )
        m_con_win->Close();
}

void Ui::Download( const wxString& category, const wxString& name, const wxString& hash )
{
	const std::string scat = STD_STRING(category);
	const std::string sname = STD_STRING(name);
	int count = prDownloader().GetDownload(scat, sname);
	assert( count > 0 );
}

//void Ui::DownloadFileWebsite( const wxString& name )
//{
//	wxString newname = name;
//	newname.Replace( _T(" "), _T("+") );
//	wxString url = _T("http://spring.jobjol.nl/search_result.php?search_cat=1&select_select=select_file_subject&Submit=Search&search=") + newname;
//	OpenWebBrowser ( url );
//}

//! @brief Display a dialog asking a question with OK and Canel buttons
//!
//! @return true if OK button was pressed
//! @note this does not return until the user pressed any of the buttons or closed the dialog.
bool Ui::Ask( const wxString& heading, const wxString& question ) const
{
    int answer = customMessageBox( SL_MAIN_ICON, question, heading, wxYES_NO );
    return ( answer == wxYES );
}

//! cannot be const because parent window cannot be const
bool Ui::AskPassword( const wxString& heading, const wxString& message, wxString& password )
{
    wxPasswordEntryDialog pw_dlg( &mw(), message, heading, password );
    int res = pw_dlg.ShowModal();
    password = pw_dlg.GetValue();
    return ( res == wxID_OK );
}


bool Ui::AskText( const wxString& heading, const wxString& question, wxString& answer, long style )
{
    wxTextEntryDialog name_dlg( &mw(), question, heading, answer, style );
    int res = name_dlg.ShowModal();
    answer = name_dlg.GetValue();

    return ( res == wxID_OK );
}


void Ui::ShowMessage( const wxString& heading, const wxString& message ) const
{
    if ( m_main_win == 0 ) return;
    serverMessageBox( SL_MAIN_ICON, message, heading, wxOK);
}


bool Ui::ExecuteSayCommand( const wxString& cmd )
{
    if ( !IsConnected() ) return false;

    if ( (cmd.BeforeFirst(' ').Lower() == _T("/join")) || (cmd.BeforeFirst(' ').Lower() == _T("/j")) )
    {
        wxString channel = cmd.AfterFirst(' ');
        wxString pass = channel.AfterFirst(' ');
        if ( !pass.IsEmpty() ) channel = channel.BeforeFirst(' ');
        if ( channel.StartsWith(_T("#")) ) channel.Remove( 0, 1 );
		serverSelector().GetServer().JoinChannel( channel, pass );
        return true;
    }
    else if ( cmd.BeforeFirst(' ').Lower() == _T("/away") )
    {
		serverSelector().GetServer().GetMe().Status().away = true;
		serverSelector().GetServer().GetMe().SendMyUserStatus();
		mw().GetJoinTab().GetBattleRoomTab().UpdateMyInfo();
        return true;
    }
    else if ( cmd.BeforeFirst(' ').Lower() == _T("/back") )
    {
        if ( IsConnected() )
        {
			serverSelector().GetServer().GetMe().Status().away = false;
			serverSelector().GetServer().GetMe().SendMyUserStatus();
			mw().GetJoinTab().GetBattleRoomTab().UpdateMyInfo();
            return true;
        }
    }
    else if ( cmd.BeforeFirst(' ').Lower() == _T("/ingame") )
    {
        wxString nick = cmd.AfterFirst(' ');
		serverSelector().GetServer().RequestInGameTime( nick );
        return true;
    }
    else if ( cmd.BeforeFirst(' ').Lower() == _T("/help") )
    {
        wxString topic = cmd.AfterFirst(' ');
        ConsoleHelp( topic.Lower() );
        return true;
    }
    else if ( cmd.BeforeFirst(' ').Lower() == _T("/msg") )
    {
        wxString user = cmd.AfterFirst(' ').BeforeFirst(' ');
        wxString msg = cmd.AfterFirst(' ').AfterFirst(' ');
		serverSelector().GetServer().SayPrivate( user, msg );
        return true;
    }
    else if ( cmd.BeforeFirst(' ').Lower() == _T("/channels") )
    {
        mw().ShowChannelChooser();
        return true;
    }
    return false;
}


void Ui::ConsoleHelp( const wxString& topic )
{
    ChatPanel* panel = GetActiveChatPanel();
    if ( panel == 0 )
    {
		ShowMessage( _("Help error"), _("Type /help in a chat box. (A bug currently prevents this from working in battleroom") );
        return;
    }
    if ( topic == wxEmptyString )
    {
		panel->ClientMessage( IdentityString( _("%s commands help.") ) );
        panel->ClientMessage( _T("") );
        panel->ClientMessage( _("Global commands:") );
        panel->ClientMessage( _("  \"/away\" - Sets your status to away.") );
        panel->ClientMessage( _("  \"/back\" - Resets your away status.") );
        panel->ClientMessage( _("  \"/changepassword2 newpassword\" - Changes the current active account's password, needs the old password saved in login box") );
        panel->ClientMessage( _("  \"/changepassword oldpassword newpassword\" - Changes the current active account's password, password cannot contain spaces") );
        panel->ClientMessage( _("  \"/channels\" - Lists currently active channels.") );
        panel->ClientMessage( _("  \"/help [topic]\" - Put topic if you want to know more specific information about a command.") );
		panel->ClientMessage( _("  \"/join channel [password]\" - Joins a channel.") );
        panel->ClientMessage( _("  \"/j\" - Alias to /join.") );
        panel->ClientMessage( _("  \"/ingame\" - Shows how much time you have in game.") );
        panel->ClientMessage( _("  \"/msg username [text]\" - Sends a private message containing text to username.") );
        panel->ClientMessage( _("  \"/part\" - Leaves current channel.") );
        panel->ClientMessage( _("  \"/p\" - Alias to /part.") );
        panel->ClientMessage( _("  \"/rename newalias\" - Changes your nickname to newalias.") );
		panel->ClientMessage( IdentityString( _("  \"/sayver\" - Says what version of %s you have in chat.") ) );
        panel->ClientMessage( _("  \"/testmd5 text\" - Returns md5-b64 hash of given text.") );
		panel->ClientMessage( IdentityString( _("  \"/ver\" - Displays what version of %s you have.") ) );
        panel->ClientMessage( _("  \"/clear\" - Clears all text from current chat panel") );
        panel->ClientMessage( _T("") );
        panel->ClientMessage( _("Chat commands:") );
        panel->ClientMessage( _("  \"/me action\" - Say IRC style action message.") );
        panel->ClientMessage( _T("") );
        panel->ClientMessage( _("If you are missing any commands, go to #springlobby and try to type it there :)") );
//    panel->ClientMessage( _("  \"/\" - .") );
    }
    else if ( topic == _T("topics") )
    {
        panel->ClientMessage( _("No topics written yet.") );
    }
    else
    {
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
	if ( IsConnected() && serverSelector().GetServerStatus() )
    {
		serverSelector().GetServer().Update( mselapsed );
    }

    if ( m_first_update_trigger )
    {
        m_first_update_trigger = false;

        if ( sett().GetAutoConnect() ) {
            Connect(); //the start tab is set from UI::onLoggedin
        }
        else {
            mw().ShowTab( sett().GetStartTab() );
        }
		//don't ask for updates on first run, that's a bit much for a newbie
		if ( sett().GetAutoUpdate() && !sett().IsFirstRun() )
            CheckForUpdates();
    }

    if (m_upd_counter_torrent % 20 == 0 )
    {
		mw().GetDownloadTab().OnUpdate();
		mw().GetJoinTab().OnUpdate();
    }
//    prDownloader().UpdateFromTimer( mselapsed );
    m_upd_counter_torrent++;
}


//! @brief Called when connected to a server
//!
//! @todo Display in servertab
void Ui::OnConnected( Server& server, const wxString& server_name, const wxString& /*unused*/, bool /*supported*/ )
{
    wxLogDebugFunc( _T("") );

    if ( server.uidata.panel ) server.uidata.panel->StatusMessage( _T("Connected to ") + server_name + _T(".") );
		mw().GetBattleListTab().OnConnected();

	delete m_con_win;
	m_con_win = 0;

	delete m_reconnect_dialog;
	m_reconnect_dialog = 0;
}


bool Ui::IsSpringCompatible(const wxString& engine, const wxString& version)
{
	assert(engine == _T("spring"));
    sett().RefreshSpringVersionList();
    if ( sett().GetDisableSpringVersionCheck() ) return true;
	const std::string neededversion = STD_STRING(version);
	std::string hackversion;
	if (neededversion == "94.1") //workarrounds for https://github.com/spring/LobbyProtocol/issues/15
		hackversion = "94";
	else if (neededversion == "91.0")
		hackversion = "91";

    const auto versionlist = sett().GetSpringVersionList();
    if ( versionlist.size() == 0 )
    {
      wxLogWarning( _T("can't get spring version from any unitsync") );
      customMessageBoxNoModal(SL_MAIN_ICON,  _("Couldn't get your spring versions from any unitsync library.\n\nOnline play is currently disabled."), _("Spring error"), wxICON_EXCLAMATION|wxOK );
      return false;
    }
    for ( const auto pair : versionlist )
    {
      if ( (STD_STRING(pair.second) == neededversion) || (STD_STRING(pair.second)==hackversion) )
      {
        if ( sett().GetCurrentUsedSpringIndex() != pair.first )
        {
          wxLogMessage(_T("server enforce usage of version: %s, switching to profile: %s"), neededversion.c_str(), pair.first.c_str() );
          sett().SetUsedSpringIndex( pair.first );
		  LSL::usync().AddReloadEvent();
        }
        return true;
      }
    }
	wxString message = wxFormat( _("No compatible installed spring version has been found, this server requires version: %s\n") ) % neededversion;
    message << _("Your current installed versions are:");
    for (const auto pair : versionlist)
        message << _T(" ") << TowxString(pair.second);
    message << _T("\n") << _("Online play is currently disabled.");
    customMessageBoxNoModal (SL_MAIN_ICON, message, _("Spring error"), wxICON_EXCLAMATION|wxOK );
    wxLogWarning ( _T("no spring version supported by the server found") );
    return false; // no compatible version found
}


void Ui::OnLoggedIn( )
{
    if ( m_main_win == 0 ) return;
    mw().GetChatTab().RejoinChannels();
    if ( sett().GetAutoConnect() )
        mw().ShowTab( sett().GetStartTab() );
}


void Ui::OnDisconnected( Server& server, bool wasonline )
{
	m_reconnect_delay_timer.Start( s_reconnect_delay_ms, true );
    if ( m_main_win == 0 ) return;
    wxLogDebugFunc( _T("") );
    if (!&server)
    {
        wxLogError(_T("WTF got null reference!!!"));
        return;
    }

    mw().GetJoinTab().LeaveCurrentBattle();
    mw().GetBattleListTab().RemoveAllBattles();

    mw().GetChatTab().LeaveChannels();

	wxString disconnect_msg = wxFormat(_("disconnected from server: %s") ) % server.GetServerName();
	UiEvents::GetStatusEventSender( UiEvents::addStatusMessage ).SendEvent(
			UiEvents::StatusData( disconnect_msg, 1 ) );
	if ( !wxTheApp->IsActive() )
	{
		UiEvents::GetNotificationEventSender().SendEvent(
				UiEvents::NotficationData( UiEvents::ServerConnection, disconnect_msg ) );
	}
    if ( server.uidata.panel )
    {
		server.uidata.panel->StatusMessage( disconnect_msg );

        server.uidata.panel->SetServer( 0 );
    }
	if ( !wasonline ) // couldn't even estabilish a socket, prompt the user to switch to another server
	{
		ConnectionFailurePrompt();
	}
}

void Ui::ConnectionFailurePrompt()
{
	if ( m_reconnect_dialog != 0 )
	{
		return;
	}
	if ( m_recconecting_wait )
	{
		return;
	}
	// if connect window instance exists, delete it to avoid 2 windows which do the same task
	delete m_con_win;
	m_con_win = 0;
	m_reconnect_dialog = new ReconnectDialog();
	int returnval = m_reconnect_dialog->ShowModal();
	delete m_reconnect_dialog;
	m_reconnect_dialog = 0;
	switch ( returnval )
	{
		case wxID_YES: // try again with the same server/settings
		{
			Reconnect();
			break;
		}
		case wxID_NO: // switch to next server in the list
		{
			m_last_used_backup_server = GetNextServer();
			wxString current_server = sett().GetDefaultServer();
			sett().SetDefaultServer( m_last_used_backup_server );// temp set backup server as default
			Connect();
			sett().SetDefaultServer( current_server ); // restore original serv
			break;
		}
		default:
		case wxID_CANCEL: // do nothing
		{
			return;
		}
	}
}



wxString Ui::GetNextServer()
{
		wxString previous_server = m_last_used_backup_server;
		if ( previous_server.IsEmpty() ) previous_server = sett().GetDefaultServer();
		wxArrayString serverlist = sett().GetServers();
		int position = serverlist.Index( previous_server );
		if ( position == wxNOT_FOUND ) position = -1;
		position = ( position + 1) % serverlist.GetCount(); // switch to next in the list
		return serverlist[position];
}

static inline bool IsAutoJoinChannel( Channel& chan )
{
    typedef std::vector<ChannelJoinInfo>
        Vec;
    typedef Vec::const_iterator
        VecIt;
    const Vec chans = sett().GetChannelsJoin();
    for ( VecIt it = chans.begin(); it != chans.end(); ++it ) {
        if ( it->name == chan.GetName() )
            return true;
    }
    return false;
}
//! @brief Called when client has joined a channel
//!
//! @todo Check if a pannel allready exists for this channel
void Ui::OnJoinedChannelSuccessful( Channel& chan )
{
	bool doFocus = !sett().GetAutoConnect() || !IsAutoJoinChannel( chan );
	bool panel_exists = mw().GetChannelChatPanel( chan.GetName() ) != NULL;
	OnJoinedChannelSuccessful( chan, !panel_exists && doFocus );
}

void Ui::OnJoinedChannelSuccessful( Channel& chan, bool focusTab )
{
	if ( m_main_win == 0 ) return;
	wxLogDebugFunc( _T("") );

	chan.uidata.panel = 0;

	mw().OpenChannelChat( chan, focusTab );
}

//! @brief Called when something is said in a channel
void Ui::OnChannelSaid( Channel& channel, User& user, const wxString& message )
{
    wxLogDebugFunc( _T("") );
    if ( channel.uidata.panel == 0 )
    {
        wxLogError( _T("OnChannelSaid: ud->panel NULL") );
        return;
    }
    channel.uidata.panel->Said( user.GetNick(), message );
}


void Ui::OnChannelDidAction( Channel& channel , User& user, const wxString& action )
{
    wxLogDebugFunc( _T("") );
    if ( channel.uidata.panel == 0 )
    {
        wxLogError( _T("OnChannelDidAction: ud->panel NULL") );
        return;
    }
    channel.uidata.panel->DidAction( user.GetNick(), action );
}


void Ui::OnChannelMessage( const wxString& channel, const wxString& msg )
{
    ChatPanel* panel = GetChannelChatPanel( channel );
    if ( panel != 0 )
    {
        panel->StatusMessage( msg );
    }
}


/** \brief this was used when channel was left via raw command in server tab, now it's not used by anything */
void Ui::OnLeaveChannel( wxString& name )
{
    ChatPanel* panel = GetChannelChatPanel( name );

    if (panel)
        mw().GetChatTab().RemoveChatPanel( panel );
}


void Ui::OnUserJoinedChannel( Channel& chan, User& user )
{
    //wxLogDebugFunc( _T("") );
    if ( chan.uidata.panel == 0 )
    {
        wxLogError( _T("OnUserJoinedChannel: ud->panel NULL") );
        return;
    }
    chan.uidata.panel->Joined( user );
}


void Ui::OnChannelJoin( Channel& chan, User& user )
{
    //wxLogDebugFunc( _T("") );
    if ( chan.uidata.panel == 0 )
    {
        wxLogError( _T("OnChannelJoin: ud->panel NULL") );
        return;
    }
    chan.uidata.panel->OnChannelJoin( user );
}


void Ui::OnUserLeftChannel( Channel& chan, User& user, const wxString& reason )
{
    //wxLogDebugFunc( _T("") );
    if ( chan.uidata.panel == 0 )
    {
        wxLogError( _T("OnUserLeftChannel: ud->panel NULL") );
        return;
    }
    chan.uidata.panel->Parted( user, reason );
}


void Ui::OnChannelTopic( Channel& channel, const wxString& user, const wxString& topic )
{
    wxLogDebugFunc( _T("") );
    if ( channel.uidata.panel == 0 )
    {
        wxLogError( _T("OnChannelTopic: ud->panel NULL") );
        return;
    }
    channel.uidata.panel->SetTopic( user, topic );
}


void Ui::OnChannelList( const wxString& channel, const int& numusers )
{
    ChatPanel* panel = GetActiveChatPanel();
    if ( panel == 0 )
    {
        ShowMessage( _("error"), _("no active chat panels open.") );
        return;
    }
	panel->StatusMessage( wxFormat( _("%s (%d users)") ) % channel % numusers );
}


void Ui::OnUserOnline( User& user )
{
    if ( m_main_win == 0 ) return;
    /*  UiUserData* data = new UiUserData();
      data->panel = 0;

      user.SetUserData( (void*)data );*/

    mw().GetChatTab().OnUserConnected( user );
}


void Ui::OnUserOffline( User& user )
{
    if ( m_main_win == 0 ) return;
    mw().GetChatTab().OnUserDisconnected( user );
    if ( user.uidata.panel )
    {
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
	for ( int i = 0; i < serverSelector().GetServer().GetNumChannels(); i++ )
    {
		Channel& chan = serverSelector().GetServer().GetChannel( i );
        if ( ( chan.UserExists(user.GetNick()) ) && ( chan.uidata.panel != 0 ) )
        {
            chan.uidata.panel->UserStatusUpdated( user );
        }
    }
	if ( user.uidata.panel )
	{
		user.uidata.panel->UserStatusUpdated( user );
	}
    if ( user.GetStatus().in_game ) mw().GetBattleListTab().UserUpdate( user );
    try
    {
			ChatPanel& server = mw().GetChatTab().ServerChat();
			server.UserStatusUpdated( user );
		}
		catch(...){}
}


void Ui::OnUnknownCommand( Server& server, const wxString& command, const wxString& params )
{
    if ( server.uidata.panel != 0 ) server.uidata.panel->UnknownCommand( command, params );
}


void Ui::OnMotd( Server& server, const wxString& message )
{
    if ( server.uidata.panel != 0 ) server.uidata.panel->Motd( message );
}

void Ui::OnServerBroadcast( Server& /*server*/, const wxString& message )
{
	if ( m_main_win == 0 ) return;
	mw().GetChatTab().BroadcastMessage( message );
	try // send it to battleroom too
	{
		mw().GetJoinTab().GetBattleRoomTab().GetChatPanel().StatusMessage(message);
	}
	catch(...) {}
}


void Ui::OnServerMessage( Server& server, const wxString& message )
{
	if ( !sett().GetBroadcastEverywhere() )
	{
		if ( server.uidata.panel != 0 ) server.uidata.panel->StatusMessage( message );
	}
	else
	{
		if ( server.uidata.panel != 0 ) server.uidata.panel->StatusMessage( message );
		if ( m_main_win == 0 ) return;
		ChatPanel* activepanel = mw().GetChatTab().GetActiveChatPanel();
		if ( activepanel != 0 )
		{
			if (activepanel != server.uidata.panel) activepanel->StatusMessage(message); // don't send it twice to the server tab
		}
		try // send it to battleroom too
		{
			mw().GetJoinTab().GetBattleRoomTab().GetChatPanel().StatusMessage(message);
		}
		catch(...) {}
	}
}


void Ui::OnUserSaid( User& user, const wxString& message, bool fromme )
{
    if ( m_main_win == 0 ) return;
    if ( user.uidata.panel == 0 )
    {
        mw().OpenPrivateChat( user );
    }
	if ( fromme ) user.uidata.panel->Said( serverSelector().GetServer().GetMe().GetNick(), message );
    else user.uidata.panel->Said( user.GetNick(), message );
}

void Ui::OnUserSaidEx( User& user, const wxString& action, bool fromme )
{
	if ( m_main_win == 0 ) return;
	if ( user.uidata.panel == 0 )
	{
		mw().OpenPrivateChat( user );
	}
	if ( fromme ) user.uidata.panel->DidAction ( serverSelector().GetServer().GetMe().GetNick(), action );
	else user.uidata.panel->DidAction( user.GetNick(), action );
}

void Ui::OnBattleOpened( IBattle& battle )
{
    if ( m_main_win == 0 ) return;
    mw().GetBattleListTab().AddBattle( battle );
    try
    {
			User& user = battle.GetFounder();
			for ( int i = 0; i < serverSelector().GetServer().GetNumChannels(); i++ )
			{
					Channel& chan = serverSelector().GetServer().GetChannel( i );
					if ( ( chan.UserExists(user.GetNick()) ) && ( chan.uidata.panel != 0 ) )
					{
							chan.uidata.panel->UserStatusUpdated( user );
					}
			}
    }catch(...){}
}


void Ui::OnBattleClosed( IBattle& battle )
{
    if ( m_main_win == 0 ) return;
    mw().GetBattleListTab().RemoveBattle( battle );
    try
    {
        if ( mw().GetJoinTab().GetBattleRoomTab().GetBattle() == &battle )
        {
            if (!battle.IsFounderMe() )
                customMessageBoxNoModal(SL_MAIN_ICON,_("The current battle was closed by the host."),_("Battle closed"));
            mw().GetJoinTab().LeaveCurrentBattle();
        }
    }
    catch (...){}
    for ( unsigned int b = 0; b < battle.GetNumUsers(); b++ )
    {
        User& user = battle.GetUser( b );
        user.SetBattle(0);
		for ( int i = 0; i < serverSelector().GetServer().GetNumChannels(); i++ )
        {
			Channel& chan = serverSelector().GetServer().GetChannel( i );
            if ( ( chan.UserExists(user.GetNick()) ) && ( chan.uidata.panel != 0 ) )
            {
                chan.uidata.panel->UserStatusUpdated( user );
            }
        }
    }
}


void Ui::OnUserJoinedBattle( IBattle& battle, User& user )
{
    if ( m_main_win == 0 ) return;
    mw().GetBattleListTab().UpdateBattle( battle );

    try
    {
        if ( mw().GetJoinTab().GetBattleRoomTab().GetBattle() == &battle )
        {
        	 mw().GetJoinTab().GetBattleRoomTab().OnUserJoined( user );
			 OnBattleInfoUpdated( std::make_pair(&battle,wxString()) );
        }
    }
    catch (...){}

	for ( int i = 0; i < serverSelector().GetServer().GetNumChannels(); i++ )
    {
		Channel& chan = serverSelector().GetServer().GetChannel( i );
        if ( ( chan.UserExists(user.GetNick()) ) && ( chan.uidata.panel != 0 ) )
        {
            chan.uidata.panel->UserStatusUpdated( user );
        }
    }
}


void Ui::OnUserLeftBattle( IBattle& battle, User& user, bool isbot )
{
    if ( m_main_win == 0 ) return;
    user.SetSideiconIndex( -1 ); //just making sure he's not running around with some icon still set
	user.BattleStatus().side = 0; // and reset side, so after rejoin we don't potentially stick with a num higher than avail
    mw().GetBattleListTab().UpdateBattle( battle );
    try
    {
        if ( mw().GetJoinTab().GetBattleRoomTab().GetBattle() == &battle )
        {
            mw().GetJoinTab().GetBattleRoomTab().OnUserLeft( user );
		 OnBattleInfoUpdated( std::make_pair(&battle,wxString()) );
		if ( &user == &serverSelector().GetServer().GetMe() )
            {
                mw().GetJoinTab().LeaveCurrentBattle();
            }
        }
    }
    catch (...) {}
    if ( isbot ) return;
	for ( int i = 0; i < serverSelector().GetServer().GetNumChannels(); i++ )
    {
		Channel& chan = serverSelector().GetServer().GetChannel( i );
        if ( ( chan.UserExists(user.GetNick()) ) && ( chan.uidata.panel != 0 ) )
        {
            chan.uidata.panel->UserStatusUpdated( user );
        }
    }
}

void Ui::OnBattleInfoUpdated( BattleEvents::BattleEventData data )
{
	IBattle& battle = *data.first;
	const wxString& Tag = data.second;
    if ( m_main_win == 0 ) return;
    mw().GetBattleListTab().UpdateBattle( battle );
    if ( mw().GetJoinTab().GetCurrentBattle() == &battle )
    {
		if ( Tag.IsEmpty() )
			mw().GetJoinTab().UpdateCurrentBattle();
		else
			mw().GetJoinTab().UpdateCurrentBattle( Tag );
    }
}

//void Ui::OnBattleInfoUpdated( IBattle& battle, const wxString& Tag )
//{
//    if ( m_main_win == 0 ) return;
//    mw().GetBattleListTab().UpdateBattle( battle );
//    if ( mw().GetJoinTab().GetCurrentBattle() == &battle )
//    {
//        mw().GetJoinTab().UpdateCurrentBattle( Tag );
//    }
//}


void Ui::OnJoinedBattle( Battle& battle )
{
    if ( m_main_win == 0 ) return;
    mw().GetJoinTab().JoinBattle( battle );
    mw().FocusBattleRoomTab();
    if ( !LSL::usync().IsLoaded() )
    {
        customMessageBox(SL_MAIN_ICON, _("Your spring settings are probably not configured correctly,\nyou should take another look at your settings before trying\nto play online."), _("Spring settings error"), wxOK );
    }
    if ( battle.GetNatType() != NAT_None )
    {
        wxLogWarning( _T("joining game with NAT transversal") );
    }
}


void Ui::OnHostedBattle( Battle& battle )
{
	if ( m_main_win == 0 )
		return;
    mw().GetJoinTab().HostBattle( battle );
	mw().FocusBattleRoomTab();
}


void Ui::OnUserBattleStatus( IBattle& battle, User& user )
{
    if ( m_main_win == 0 ) return;
    mw().GetJoinTab().BattleUserUpdated( user );
	OnBattleInfoUpdated( std::make_pair(&battle,wxString()) );
}


void Ui::OnRequestBattleStatus( IBattle& battle )
{
    if ( m_main_win == 0 ) return;
    try
    {
        if ( mw().GetJoinTab().GetBattleRoomTab().GetBattle() == &battle )
        {
            mw().GetJoinTab().GetBattleRoomTab().GetBattle()->OnRequestBattleStatus();
        }
    }
    catch (...) {}
}


void Ui::OnBattleStarted( Battle& battle )
{
    if ( m_main_win == 0 ) return;
    wxLogDebugFunc( _T("") );
    mw().GetBattleListTab().UpdateBattle( battle );
}


void Ui::OnSaidBattle( IBattle& /*battle*/, const wxString& nick, const wxString& msg )
{
    if ( m_main_win == 0 ) return;
    try
    {
        mw().GetJoinTab().GetBattleRoomTab().GetChatPanel().Said( nick, msg );
    }
    catch (...) {}
}

void Ui::OnSpringTerminated( wxCommandEvent& data  )
{
	const int exit_code  = data.GetInt();

    if ( !m_serv ) return;

    try {
		serverSelector().GetServer().GetMe().Status().in_game = false;
		serverSelector().GetServer().GetMe().SendMyUserStatus();
		Battle *battle = serverSelector().GetServer().GetCurrentBattle();
        if ( !battle )
            return;
        if( battle->IsFounderMe() && battle->GetAutoLockOnStart() ) {
          battle->SetIsLocked( false );
          battle->SendHostInfo( IBattle::HI_Locked );
        }
    } catch ( assert_exception ){}

	if (exit_code != 0) {
		#if wxUSE_DEBUGREPORT && defined(ENABLE_DEBUG_REPORT)
			SpringDebugReport report;
			if ( wxDebugReportPreviewStd().Show( report ) )
				report.Process();
		#else
			if ( customMessageBox( SL_MAIN_ICON, _T("The game has crashed.\nOpen infolog.txt?"), _T("Crash"), wxYES_NO ) == wxYES )
				OpenFileInEditor( sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator() + _T("infolog.txt") );
		#endif
    }
}


void Ui::OnAcceptAgreement( const wxString& agreement )
{
    AgreementDialog dlg( m_main_win, agreement );
    if ( dlg.ShowModal() == 1 )
    {
		serverSelector().GetServer().AcceptAgreement();
		serverSelector().GetServer().Login();
    }
}


void Ui::OnRing( const wxString& from )
{
    if ( m_main_win == 0 ) return;
    m_main_win->RequestUserAttention();

	if ( !wxTheApp->IsActive() )
	{
		const wxString msg = wxFormat(_("%s:\nring!") ) % from;
		UiEvents::GetNotificationEventSender().SendEvent(
				UiEvents::NotficationData( UiEvents::ServerConnection, msg ) );
	}

//    if(serverSelector().GetServer().GetCurrentBattle()->GetMe().GetBattleStatus().sync == SYNC_UNSYNCED) {
//        wxString host_map_name = serverSelector().GetServer().GetCurrentBattle()->GetHostMapName();
//        if(! usync().MapExists(host_map_name)) {//TODO

//			map_infos info_map = prDownloader().CollectGuiInfos();
//            bool dling = false;

//            for(map_infos_iter iter = info_map.begin(); iter != info_map.end(); ++iter){
//                if(iter->first == wxString(_T("global")))
//                    continue;
//                else if(iter->first == host_map_name) {
//                    int eta = iter->second.eta;

//                    if(eta <= 0) {
//						serverSelector().GetServer().GetCurrentBattle()->ExecuteSayCommand(_("/me map is not available for automatic downloading."));
//                    } else {
//                        serverSelector().GetServer().GetCurrentBattle()->ExecuteSayCommand(wxString::Format(
//                                _("/me downloading map eta: %d s"), eta)
//                        );
//                    dling = true;
//                    }
//                }
//            }
//            if(! dling) { //XXX is it possible to get eta from web dl in sl?, anyone using it instead torrent system? //there is no direct from-web downloading in SL currently
//                serverSelector().GetServer().GetCurrentBattle()->ExecuteSayCommand(
//                        _("/me is not downloading map with SL torrent system"));
//            }
//        }
//    }

#ifndef DISABLE_SOUND
    if ( sett().GetChatPMSoundNotificationEnabled() )
        sound().ring();
#else
    wxBell();
#endif
}

bool Ui::IsThisMe(User& other) const
{
    return IsThisMe( other.GetNick() );
}

bool Ui::IsThisMe(User* other) const
{
    return ( ( other != 0 ) && IsThisMe( other->GetNick() ) );
}

bool Ui::IsThisMe(const wxString& other) const
{
    //if i'm not connected i have no identity
    if (!IsConnected() || m_serv==0)
        return false;
    else
		return ( other == serverSelector().GetServer().GetMe().GetNick() );
}

int Ui::TestHostPort( unsigned int port ) const
{
	return serverSelector().GetServer().TestOpenPort( port );
}

void Ui::ReloadPresetList()
{
    try
    {
        mw().GetSPTab().ReloadPresetList();
    }
    catch (...) {}
    try
    {
        mw().GetJoinTab().ReloadPresetList();
    }
    catch (...) {}
}

bool Ui::OnPresetRequiringMap( const wxString& mapname )
{
    if ( wxYES == customMessageBox( SL_MAIN_ICON,
                        _("The selected preset requires the map ") + mapname + _(". Should it be downloaded? \
                                    \n Selecting \"no\" will remove the missing map from the preset.\n\
                                    Please reselect the preset after download finished"),
                        _("Map missing"),
                        wxYES_NO ) )
    {
        Download( _T("map") , mapname, _T("") );
        return true;
    }
    return false;
}

void Ui::OpenFileInEditor( const wxString& filepath )
{
    wxString editor_path = sett().GetEditorPath( );
    if ( editor_path == wxEmptyString ) {
        customMessageBoxNoModal( SL_MAIN_ICON, _T("You have not chosen an external text editor to open files with.\nPlease Select one now."), _T("No editor set") );
        mw().ShowConfigure( MainWindow::OPT_PAGE_GENERAL );
        return;
    }
    bool success = ( wxExecute( editor_path + _T(" \"") + filepath + _T("\""), wxEXEC_ASYNC ) != 0 );
    if ( !success ) {
        customMessageBoxNoModal( SL_MAIN_ICON, _T("There was a problem launching the editor.\nPlease make sure the path is correct and the binary executable for your user.\nNote it's currently not possible to use shell-only editors like ed, vi, etc."), _T("Problem launching editor") );
        mw().ShowConfigure( MainWindow::OPT_PAGE_GENERAL );
    }
}

void Ui::FirstRunWelcome()
{
    if ( sett().IsFirstRun() )
    {
#ifdef __WXMSW__
        sett().SetOldSpringLaunchMethod( true );
#endif

        wxLogMessage( _T("first time startup"));
		wxMessageBox( wxFormat( _("Hi %s,\nIt looks like this is your first time using %s. I have guessed a configuration that I think will work for you but you should review it, especially the Spring configuration.") )
										% wxGetUserName()
										% GetAppName(),
					  _("Welcome"),
					  wxOK | wxICON_INFORMATION, &mw() );

		IntroGuide* intro = new IntroGuide();
		intro->Show();

		// copy uikeys.txt
		wxPathList pl;
		pl.AddEnvList( _T("%ProgramFiles%") );
		pl.AddEnvList( _T("XDG_DATA_DIRS") );
        pl = PathlistFactory::AdditionalSearchPaths( pl );
		wxString uikeyslocation = pl.FindValidPath( _T("uikeys.txt") );
		if ( !uikeyslocation.IsEmpty() )
		{
			wxCopyFile( uikeyslocation, sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator() + _T("uikeys.txt"), false );
		}

        //this ensures that for new configs there's a default perspective to fall back on
        mw().SavePerspectives( _T("SpringLobby-default") );
        mw().ShowConfigure();
    }
    else
    {
        mw().ShowSingleplayer();
    }
}


void Ui::CheckForUpdates()
{
    wxString latestVersion = GetLatestVersion();

    if (latestVersion == _T("-1"))
    {
        customMessageBoxNoModal(SL_MAIN_ICON, _("There was an error checking for the latest version.\nPlease try again later.\nIf the problem persists, please use Help->Report Bug to report this bug."), _("Error"));
        return;
    }
	//get current rev w/o AUX_VERSION added
	wxString myVersion = GetSpringLobbyVersion( false ) ;

    wxString msg = _("Your Version: ") + myVersion + _T("\n") + _("Latest Version: ") + latestVersion;
    if ( !latestVersion.IsSameAs(myVersion, false) )
    {
        #ifdef __WXMSW__
		int answer = customMessageBox(SL_MAIN_ICON,
									  wxFormat( _("Your %s version is not up to date.\n\n%s\n\nWould you like for me to autodownload the new version? Changes will take effect next you launch the lobby again.") )
														% GetAppName()
														% msg,
									  _("Not up to date"), wxYES_NO);
        if (answer == wxYES)
        {
            wxString command = _T("\"") + wxPathOnly( wxStandardPaths::Get().GetExecutablePath() ) + wxFileName::GetPathSeparator() + _T("springlobby_updater.exe\"");
            wxString params = _T("-f \"") + wxStandardPaths::Get().GetExecutablePath() + _T("\"") + _T(" -r ") +  latestVersion  ;
            if( WinExecute( command, params ) > 0 ) {
                //returned pid > 0 -> proc started successfully
                // now close this instance immeadiately
                wxCloseEvent dummy;
                ui().mw().OnClose( dummy );
            }
            else
            {//this will also happen if updater exe is not present so we don't really ne special check for existance of it
                customMessageBox(SL_MAIN_ICON, _("Automatic update failed\n\nyou will be redirected to a web page with instructions and the download link will be opened in your browser.") + msg, _("Updater error.") );
				OpenWebBrowser( _T("http://projects.springlobby.info/wiki/springlobby/Install#Windows-Binary") );
                OpenWebBrowser( GetDownloadUrl( latestVersion ) );

            }
        }
        #else
        customMessageBoxNoModal(SL_MAIN_ICON, _("Your SpringLobby version is not up to date.\n\n") + msg, _("Not up to Date") );
        #endif
    }
    /* TODO currently not usable cause automatic update calls this function too and we don't want a msg box everytime the check succeeds
    else
        customMessageBoxNoModal(SL_MAIN_ICON, _("Your SpringLobby version is up to date.\n\n") + msg, _("Up to Date") );
    */
}


void Ui::OnQuit(wxCommandEvent& /*data*/)
{
	Disconnect();
	delete m_serv;
	m_serv = NULL;
}
