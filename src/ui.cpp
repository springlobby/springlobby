/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */
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
#include <wx/thread.h>
#include <wx/intl.h>
#include <wx/utils.h>
#include <wx/debugrpt.h>
#include <wx/filename.h>
#include <wx/timer.h>

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
#include "uiutils.h"
#include "chatpanel.h"
#include "battlelisttab.h"
#include "battleroomtab.h"
#include "battle.h"
#include "mainchattab.h"
#include "mainjoinbattletab.h"
#include "mainsingleplayertab.h"
#ifndef NO_TORRENT_SYSTEM
#include "maintorrenttab.h"
#include "torrentwrapper.h"
#endif
#include "unitsyncthread.h"
#include "agreementdialog.h"
#ifdef __WXMSW__
    #include "updater/updater.h"
    #include "Helper/tasclientimport.h"
#endif

#include "utils/customdialogs.h"

#include "sdlsound.h"
#include "globalsmanager.h"

const unsigned int TIMER_ID         = wxNewId();
const unsigned int TIMER_INTERVAL   = 100;

BEGIN_EVENT_TABLE(Ui, wxEvtHandler)
    EVT_TIMER(TIMER_ID, Ui::OnUpdate )
END_EVENT_TABLE()

Ui& ui()
{
    static GlobalObjectHolder<Ui> m_ui;
    return m_ui;
}

Ui::Ui() :
        m_serv(0),
        m_main_win(0),
        m_con_win(0),
        m_upd_counter_torrent(0),
        m_first_update_trigger(true),
        m_ingame(false),
        m_timer ( new wxTimer(this, TIMER_ID) )
{
    m_main_win = new MainWindow( );
    CustomMessageBoxBase::setLobbypointer(m_main_win);
    m_serv = new TASServer();

}

Ui::~Ui()
{
    Disconnect();
    delete m_timer;
    delete m_serv;
}
void Ui::StartUpdateTimer() {
    m_timer->Start( TIMER_INTERVAL );
}

void Ui::StopUpdateTimer() {
    m_timer->Stop();
}

Server& Ui::GetServer()
{
    ASSERT_LOGIC( m_serv != 0, _T("m_serv NULL!") );
    return *m_serv;
}

const Server& Ui::GetServer() const
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
    bool doit = sett().GetAutoConnect();
    if ( !doit )
        ShowConnectWindow();
    else
    {
        m_con_win = 0;
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
    if ( !sett().GetServerAccountSavePass(servname) )
    {
        wxString pass2 = pass;
        if ( !AskPassword( _("Server password"), _("Password"), pass2 ) ) return;
        pass = pass2;
    }

    Disconnect();
    DoConnect( servname, sett().GetServerAccountNick(servname), pass );
}


void Ui::Disconnect()
{
    m_timer->Stop();
    if ( m_serv != 0 )
    {
        if ( IsConnected() ) {
            GetServer().Disconnect();
        }
    }
}


//! @brief Opens the accutial connection to a server.
void Ui::DoConnect( const wxString& servername, const wxString& username, const wxString& password )
{
    wxString host;
    int port;

    if ( !sett().ServerExists( servername ) )
    {
        ASSERT_LOGIC( false, _T("Server does not exist in settings") );
        return;
    }

    Disconnect();

    GetServer().SetUsername( username );
    GetServer().SetPassword( password );

    if ( sett().GetServerAccountSavePass( servername ) )
    {
        if ( GetServer().IsPasswordHash(password) ) sett().SetServerAccountPass( servername, password );
        else sett().SetServerAccountPass( servername, GetServer().GetPasswordHash( password ) );
    }
    else
    {
        sett().SetServerAccountPass( servername, _T("") );
    }
    sett().SaveSettings();

    host = sett().GetServerHost( servername );
    port = sett().GetServerPort( servername );

    GetServer().uidata.panel = m_main_win->GetChatTab().AddChatPanel( *m_serv, servername );
    GetServer().uidata.panel->StatusMessage( _T("Connecting to server ") + servername + _T("...") );

    // Connect
    GetServer().Connect( servername, host, port );

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
    bool success = GetServer().Register( host, port, username, password,reason );
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


bool Ui::IsConnected() const
{
    if ( m_serv != 0 ) return m_serv->IsConnected();
    return false;
}

void Ui::JoinChannel( const wxString& name, const wxString& password )
{
    if ( m_serv != 0 ) GetServer().JoinChannel( name, password );
}


bool Ui::IsSpringRunning() const
{
    return spring().IsRunning();
}


//! @brief Quits the entire application
void Ui::Quit()
{
    Disconnect();

    #ifndef NO_TORRENT_SYSTEM
        torrent().DisconnectFromP2PSystem();// Cant hurt to disconnect unconditionally.
    #endif

    if ( m_con_win != 0 )
        m_con_win->Close();
}

void Ui::DownloadMap( const wxString& hash, const wxString& name )
{
#ifndef NO_TORRENT_SYSTEM
    DownloadFileP2P( hash, name );
#else
		wxString newname = name;
		newname.Replace( _T(" "), _T("+") );
    wxString url = _T(" http://spring.jobjol.nl/search_result.php?search_cat=1&select_select=select_file_subject&Submit=Search&search=") + newname;
    OpenWebBrowser ( url );
#endif
}


void Ui::DownloadMod( const wxString& hash, const wxString& name )
{
#ifndef NO_TORRENT_SYSTEM
    DownloadFileP2P( hash, name );
#else
		wxString newname = name;
		newname.Replace( _T(" "), _T("+") );
    wxString url = _T(" http://spring.jobjol.nl/search_result.php?search_cat=1&select_select=select_file_subject&Submit=Search&search=") + newname;
    OpenWebBrowser ( url );
#endif
}

void Ui::DownloadFileP2P( const wxString& hash, const wxString& name )
{
#ifndef NO_TORRENT_SYSTEM
    if ( !torrent().IsConnectedToP2PSystem() ){
        wxArrayString hashesToResume = sett().GetTorrentListToResume();
        hashesToResume.Add( hash );
        sett().SetTorrentListToResume( hashesToResume );
        torrent().ConnectToP2PSystem();
    }
    else {

    //we need a way to have the request happen only after connect is complete
        TorrentWrapper::DownloadRequestStatus status;
        if ( !hash.IsEmpty() ) {
             status = torrent().RequestFileByHash( hash );
        }
        else if ( !name.IsEmpty() )
            status = torrent().RequestFileByName( name );

//!TODO: put some meaningful err msg here
//        if ( status != TorrentWrapper::success ){
//            customMessageBoxNoModal( SL_MAIN_ICON, _(""), _("") );
//        }
    }
#endif
}


//! @brief Display a dialog asking a question with OK and Canel buttons
//!
//! @return true if OK button was pressed
//! @note this does not return until the user pressed any of the buttons or closed the dialog.
bool Ui::Ask( const wxString& heading, const wxString& question )
{
    int answer = customMessageBox( SL_MAIN_ICON, question, heading, wxYES_NO );
    return ( answer == wxYES );
}


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
    if ( (cmd.BeforeFirst(' ').Lower() == _T("/join")) || (cmd.BeforeFirst(' ').Lower() == _T("/j")) )
    {
        wxString channel = cmd.AfterFirst(' ');
        wxString pass = channel.AfterFirst(' ');
        if ( !pass.IsEmpty() ) channel = channel.BeforeFirst(' ');
        if ( channel.StartsWith(_T("#")) ) channel.Remove( 0, 1 );
        GetServer().JoinChannel( channel, pass );
        return true;
    }
    else if ( cmd.BeforeFirst(' ').Lower() == _T("/away") )
    {
        GetServer().GetMe().Status().away = true;
        GetServer().GetMe().SendMyUserStatus();
        return true;
    }
    else if ( cmd.BeforeFirst(' ').Lower() == _T("/back") )
    {
        if ( IsConnected() )
        {
            GetServer().GetMe().Status().away = false;
            GetServer().GetMe().SendMyUserStatus();
            return true;
        }
    }
    else if ( cmd.BeforeFirst(' ').Lower() == _T("/ingame") )
    {
        wxString nick = cmd.AfterFirst(' ');
        GetServer().RequestInGameTime( nick );
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
        GetServer().SayPrivate( user, msg );
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
        ShowMessage( _("Help error"), _("Type /help in a chat box.") );
        return;
    }
    if ( topic == wxEmptyString )
    {
        panel->ClientMessage( _("SpringLobby commands help.") );
        panel->ClientMessage( _T("") );
        panel->ClientMessage( _("Global commands:") );
        panel->ClientMessage( _("  \"/away\" - Sets your status to away.") );
        panel->ClientMessage( _("  \"/back\" - Resets your away status.") );
        panel->ClientMessage( _("  \"/changepassword oldpassword newpassword\" - Changes the current active account's password.") );
        panel->ClientMessage( _("  \"/channels\" - Lists currently active channels.") );
        panel->ClientMessage( _("  \"/help [topic]\" - Put topic if you want to know more specific information about a command.") );
        panel->ClientMessage( _("  \"/join channel [password] [,channel2 [password2]]\" - Joins a channel.") );
        panel->ClientMessage( _("  \"/j\" - Alias to /join.") );
        panel->ClientMessage( _("  \"/ingame\" - Shows how much time you have in game.") );
        panel->ClientMessage( _("  \"/msg username [text]\" - Sends a private message containing text to username.") );
        panel->ClientMessage( _("  \"/part\" - Leaves current channel.") );
        panel->ClientMessage( _("  \"/p\" - Alias to /part.") );
        panel->ClientMessage( _("  \"/rename newalias\" - Changes your nickname to newalias.") );
        panel->ClientMessage( _("  \"/sayver\" - Says what version of springlobby you have in chat.") );
        panel->ClientMessage( _("  \"/testmd5 text\" - Returns md5-b64 hash of given text.") );
        panel->ClientMessage( _("  \"/ver\" - Displays what version of SpringLobby you have.") );
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

void Ui::OnUpdate( wxTimerEvent& event )
{
    int mselapsed = event.GetInterval();
    if ( GetServerStatus() )
    {
        GetServer().Update( mselapsed );
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
#ifdef __WXMSW__
        if ( sett().GetAutoUpdate() )Updater().CheckForUpdates();
#endif
    }

#ifndef NO_TORRENT_SYSTEM
    if (m_upd_counter_torrent % 20 == 0 )
    {
        if ( sett().GetTorrentSystemAutoStartMode() == 1 && !torrent().IsConnectedToP2PSystem() ) torrent().ConnectToP2PSystem();
        else if ( GetServerStatus() && GetServer().IsOnline() && !torrent().IsConnectedToP2PSystem() && sett().GetTorrentSystemAutoStartMode() == 0 ) torrent().ConnectToP2PSystem();
        if ( ( !GetServerStatus() || !GetServer().IsOnline() ) && torrent().IsConnectedToP2PSystem() && sett().GetTorrentSystemAutoStartMode() == 0 ) torrent().DisconnectFromP2PSystem();
        mw().GetTorrentTab().OnUpdate();
    }
    torrent().UpdateFromTimer( mselapsed );
    m_upd_counter_torrent++;
#endif
}


//! @brief Called when connected to a server
//!
//! @todo Display in servertab
void Ui::OnConnected( Server& server, const wxString& server_name, const wxString& /*unused*/, bool /*supported*/ )
{
    wxLogDebugFunc( _T("") );
    if ( !m_last_used_backup_server.IsEmpty() )
    {
    	 m_last_used_backup_server = _T("");
    }
    else // connect successful & it's not a backup server fallback -> save as default
    {
			 sett().SetDefaultServer( server_name );
    }
    if ( !IsSpringCompatible() )
    {
    	#ifdef __WXMSW__
    	if ( Ask( _T("Request update"), _T("Would you like to query the server for a spring update?\n The server is totally demential and will disconnect you if no automatic update will be available") ) ) server.RequestSpringUpdate();
    	#endif
    }

    if ( server.uidata.panel ) server.uidata.panel->StatusMessage( _T("Connected to ") + server_name + _T(".") );
    mw().GetJoinTab().OnConnected();

}


bool Ui::IsSpringCompatible()
{
    sett().RefreshSpringVersionList();
    if ( sett().GetDisableSpringVersionCheck() ) return true;
    wxString neededversion = GetServer().GetRequiredSpring();
    if ( neededversion == _T("*") ) return true; // Server accepts any version.
    else if ( neededversion.IsEmpty() ) return false;
    std::map<wxString, wxString> versionlist = sett().GetSpringVersionList();
    if ( versionlist.size() == 0 )
    {
      wxLogWarning( _T("can't get spring version from any unitsync") );
      customMessageBoxNoModal(SL_MAIN_ICON,  _("Couldn't get your spring versions from any unitsync library.\n\nOnline play is currently disabled."), _("Spring error"), wxICON_EXCLAMATION|wxOK );
      return false;
    }
    for ( std::map<wxString, wxString>::iterator itor = versionlist.begin(); itor != versionlist.end(); itor++ )
    {
      if ( itor->second == neededversion )
      {
        if ( sett().GetCurrentUsedSpringIndex() != itor->first )
        {
          wxLogMessage(_T("server enforce usage of version: %s, switching to profile: %s"), neededversion.c_str(), itor->first.c_str() );
          sett().SetUsedSpringIndex( itor->first );
          GetGlobalEventSender(GlobalEvents::UnitSyncReloadRequest).SendEvent( 0 ); // request an unitsync reload
        }
        return true;
      }
    }
    wxString message = wxString::Format( _("No compatible installed spring version has been found, this server requires version: %s\n"), neededversion.c_str() );
    message << _("Your current installed versions are:");
    for ( std::map<wxString, wxString>::iterator itor = versionlist.begin(); itor != versionlist.end(); itor++ ) message << _T(" ") << itor->second;
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
    if ( m_main_win == 0 ) return;
    wxLogDebugFunc( _T("") );
    if (!&server)
    {
        wxLogError(_T("WTF got null reference!!!"));
        return;
    }

    mw().GetJoinTab().LeaveCurrentBattle();
    mw().GetJoinTab().GetBattleListTab().RemoveAllBattles();

    mw().GetChatTab().LeaveChannels();

    if ( server.uidata.panel )
    {
        server.uidata.panel->StatusMessage( _("Disconnected from server.") );

        server.uidata.panel->SetServer( 0 );
    }
		if ( !wasonline ) // couldn't even estabilish a socket, prompt the user to switch to another server
		{
			ConnectionFailurePrompt();
		}
}

void Ui::ConnectionFailurePrompt()
{
	wxMessageDialog dlg( &mw(), _("A connection couldn't be established with the server\nWould you like to try again with the same server?\nNo to switch to next server in the list"), _("Connection failure"), wxYES_NO | wxCANCEL | wxNO_DEFAULT );
	switch ( dlg.ShowModal() )
	{
		case wxID_YES: // try again with the same server/settings
		{
			Reconnect();
			break;
		}
		case wxID_NO: // switch to next server in the list
		{
			SwitchToNextServer();
			ShowConnectWindow();
			break;
		}
		default:
		case wxID_CANCEL: // do nothing
		{
			return;
		}
	}
}

void Ui::SwitchToNextServer()
{
		wxString previous_server = m_last_used_backup_server;
		if ( previous_server.IsEmpty() ) previous_server = sett().GetDefaultServer();
		wxArrayString serverlist = sett().GetServers();
		int position = serverlist.Index( previous_server );
		if ( position == wxNOT_FOUND ) position = -1;
		position = ( position + 1) % serverlist.GetCount(); // switch to next in the list
		m_last_used_backup_server = serverlist[position];
		sett().SetDefaultServer( m_last_used_backup_server );
		if ( m_con_win ) // we don't necessarily have that constructed yet (autojoin)
            m_con_win->ReloadServerList();
		sett().SetDefaultServer( previous_server ); // don't save the new server as default when switched this way
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
    if ( m_main_win == 0 ) return;
    wxLogDebugFunc( _T("") );

    chan.uidata.panel = 0;

    mw().OpenChannelChat( chan, !sett().GetAutoConnect() || !IsAutoJoinChannel( chan ) );
}


//! @brief Called when something is said in a channel
void Ui::OnChannelSaid( Channel& channel, User& user, const wxString& message )
{
    wxLogDebugFunc( _T("") );
    if ( channel.uidata.panel == 0 )
    {
        wxLogError( _T("ud->panel NULL") );
        return;
    }
    channel.uidata.panel->Said( user.GetNick(), message );
}


void Ui::OnChannelDidAction( Channel& channel , User& user, const wxString& action )
{
    wxLogDebugFunc( _T("") );
    if ( channel.uidata.panel == 0 )
    {
        wxLogError( _T("ud->panel NULL") );
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
        wxLogError( _T("ud->panel NULL") );
        return;
    }
    chan.uidata.panel->Joined( user );
}


void Ui::OnChannelJoin( Channel& chan, User& user )
{
    //wxLogDebugFunc( _T("") );
    if ( chan.uidata.panel == 0 )
    {
        wxLogError( _T("ud->panel NULL") );
        return;
    }
    chan.uidata.panel->OnChannelJoin( user );
}


void Ui::OnUserLeftChannel( Channel& chan, User& user, const wxString& reason )
{
    //wxLogDebugFunc( _T("") );
    if ( chan.uidata.panel == 0 )
    {
        wxLogError( _T("ud->panel NULL") );
        return;
    }
    chan.uidata.panel->Parted( user, reason );
}


void Ui::OnChannelTopic( Channel& channel, const wxString& user, const wxString& topic )
{
    wxLogDebugFunc( _T("") );
    if ( channel.uidata.panel == 0 )
    {
        wxLogError( _T("ud->panel NULL") );
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
    panel->StatusMessage( channel + wxString::Format( _("(%d users)"), numusers) );
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
    for ( int i = 0; i < GetServer().GetNumChannels(); i++ )
    {
        Channel& chan = GetServer().GetChannel( i );
        if ( ( chan.UserExists(user.GetNick()) ) && ( chan.uidata.panel != 0 ) )
        {
            chan.uidata.panel->UserStatusUpdated( user );
        }
    }
    if ( user.GetStatus().in_game ) mw().GetJoinTab().GetBattleListTab().UserUpdate( user );
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


void Ui::OnServerMessage( Server& server, const wxString& message )
{
    if ( server.uidata.panel != 0 ) server.uidata.panel->StatusMessage( message );
    else
    {
        ShowMessage( _("Server message"), message );
    }
}


void Ui::OnUserSaid( User& user, const wxString& message, bool fromme )
{
    if ( m_main_win == 0 ) return;
    if ( user.uidata.panel == 0 )
    {
        mw().OpenPrivateChat( user );
    }
    if ( fromme ) user.uidata.panel->Said( GetServer().GetMe().GetNick(), message );
    else user.uidata.panel->Said( user.GetNick(), message );
}


void Ui::OnBattleOpened( IBattle& battle )
{
    if ( m_main_win == 0 ) return;
    mw().GetJoinTab().GetBattleListTab().AddBattle( battle );
    try
    {
			User& user = battle.GetFounder();
			for ( int i = 0; i < GetServer().GetNumChannels(); i++ )
			{
					Channel& chan = GetServer().GetChannel( i );
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
    mw().GetJoinTab().GetBattleListTab().RemoveBattle( battle );
    try
    {
        if ( &mw().GetJoinTab().GetBattleRoomTab().GetBattle() == &battle )
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
        for ( int i = 0; i < GetServer().GetNumChannels(); i++ )
        {
            Channel& chan = GetServer().GetChannel( i );
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
    mw().GetJoinTab().GetBattleListTab().UpdateBattle( battle );

    try
    {
        if ( &mw().GetJoinTab().GetBattleRoomTab().GetBattle() == &battle )
        {
        	 mw().GetJoinTab().GetBattleRoomTab().OnUserJoined( user );
        	 OnBattleInfoUpdated( battle );
        }
    }
    catch (...){}

    for ( int i = 0; i < GetServer().GetNumChannels(); i++ )
    {
        Channel& chan = GetServer().GetChannel( i );
        if ( ( chan.UserExists(user.GetNick()) ) && ( chan.uidata.panel != 0 ) )
        {
            chan.uidata.panel->UserStatusUpdated( user );
        }
    }
}


void Ui::OnUserLeftBattle( IBattle& battle, User& user )
{
    if ( m_main_win == 0 ) return;
    user.SetSideiconIndex( -1 ); //just making sure he's not running around with some icon still set
	user.BattleStatus().side = 0; // and reset side, so after rejoin we don't potentially stick with a num higher than avail
    mw().GetJoinTab().GetBattleListTab().UpdateBattle( battle );
    try
    {
        if ( &mw().GetJoinTab().GetBattleRoomTab().GetBattle() == &battle )
        {
            mw().GetJoinTab().GetBattleRoomTab().OnUserLeft( user );
						OnBattleInfoUpdated( battle );
            if ( &user == &GetServer().GetMe() )
            {
                mw().GetJoinTab().LeaveCurrentBattle();
            }
        }
    }
    catch (...) {}
    if ( user.BattleStatus().IsBot() ) return;
    for ( int i = 0; i < GetServer().GetNumChannels(); i++ )
    {
        Channel& chan = GetServer().GetChannel( i );
        if ( ( chan.UserExists(user.GetNick()) ) && ( chan.uidata.panel != 0 ) )
        {
            chan.uidata.panel->UserStatusUpdated( user );
        }
    }
}

void Ui::OnBattleInfoUpdated( IBattle& battle )
{
    if ( m_main_win == 0 ) return;
    mw().GetJoinTab().GetBattleListTab().UpdateBattle( battle );
    if ( mw().GetJoinTab().GetCurrentBattle() == &battle )
    {
        mw().GetJoinTab().UpdateCurrentBattle();
    }
}

void Ui::OnBattleInfoUpdated( IBattle& battle, const wxString& Tag )
{
    if ( m_main_win == 0 ) return;
    mw().GetJoinTab().GetBattleListTab().UpdateBattle( battle );
    if ( mw().GetJoinTab().GetCurrentBattle() == &battle )
    {
        mw().GetJoinTab().UpdateCurrentBattle( Tag );
    }
}


void Ui::OnJoinedBattle( Battle& battle )
{
    if ( m_main_win == 0 ) return;
    mw().GetJoinTab().JoinBattle( battle );
    if ( !usync().IsLoaded() )
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
    if ( m_main_win == 0 ) return;
    mw().GetJoinTab().HostBattle( battle );
}


void Ui::OnUserBattleStatus( IBattle& battle, User& user )
{
    if ( m_main_win == 0 ) return;
    mw().GetJoinTab().BattleUserUpdated( user );
    OnBattleInfoUpdated( battle );
}


void Ui::OnRequestBattleStatus( IBattle& battle )
{
    if ( m_main_win == 0 ) return;
    try
    {
        if ( &mw().GetJoinTab().GetBattleRoomTab().GetBattle() == &battle )
        {
            mw().GetJoinTab().GetBattleRoomTab().GetBattle().OnRequestBattleStatus();
        }
    }
    catch (...) {}
}


void Ui::OnBattleStarted( Battle& battle )
{
    if ( m_main_win == 0 ) return;
    wxLogDebugFunc( _T("") );
    mw().GetJoinTab().GetBattleListTab().UpdateBattle( battle );
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


void Ui::OnBattleAction( IBattle& /*battle*/, const wxString& nick, const wxString& msg )
{
    if ( m_main_win == 0 ) return;
    try
    {
        mw().GetJoinTab().GetBattleRoomTab().GetChatPanel().DidAction( nick, msg );
    }
    catch (...){}
}

void Ui::OnSpringStarting()
{
  m_ingame = true;
#ifndef NO_TORRENT_SYSTEM
  torrent().SetIngameStatus(m_ingame);
#endif
}


void Ui::OnSpringTerminated( long exit_code )
{
    m_ingame = false;
#ifndef NO_TORRENT_SYSTEM
    torrent().SetIngameStatus(m_ingame);
#endif
    if ( !m_serv ) return;

    try {
        GetServer().GetMe().Status().in_game = false;
        GetServer().GetMe().SendMyUserStatus();
        Battle *battle = GetServer().GetCurrentBattle();
        if ( !battle )
            return;
        if( battle->IsFounderMe() && battle->GetAutoLockOnStart() ) {
          battle->SetIsLocked( false );
          battle->SendHostInfo( IBattle::HI_Locked );
        }
    } catch ( assert_exception ){}

    if ( exit_code ) {
//        wxDebugReportCompress report;
//        wxString dir = sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator();
//        wxString tmp_filename = wxPathOnly( wxFileName::CreateTempFileName(_T("dummy")) ) + wxFileName::GetPathSeparator() + _T("settings.txt");
//        wxCopyFile( sett().GetCurrentUsedSpringConfigFilePath(), tmp_filename );
//        report.AddFile( dir + _T("infolog.txt"), _T("Infolog") );
//        report.AddFile( dir + _T("script.txt"), _T("Script") );
//        report.AddFile( dir + _T("ext.txt"), _T("Infolog") );
//        report.AddFile( dir + _T("unitsync.log"), _T("Infolog") );
//        report.AddFile( tmp_filename, _T("Settings") );
//        wxString info;
//        info << wxGetOsDescription() << ( wxIsPlatform64Bit() ? _T(" 64bit\n") : _T(" 32bit\n") );
//        report.AddText( _T("platform.txt"), info, _T("Platform") );
//        wxDebugReportPreviewStd().Show( report );
//        report.Process();
        if ( customMessageBox( SL_MAIN_ICON, _T("The game has crashed.\nOpen infolog.txt?"), _T("Crash"), wxYES_NO ) == wxYES )
            OpenFileInEditor( sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator() + _T("infolog.txt") );
    }
}


void Ui::OnAcceptAgreement( const wxString& agreement )
{
    AgreementDialog dlg( m_main_win, agreement );
    if ( dlg.ShowModal() == 1 )
    {
        GetServer().AcceptAgreement();
        GetServer().Login();
    }
}


void Ui::OnRing( const wxString& /*from */)
{
    if ( m_main_win == 0 ) return;
    m_main_win->RequestUserAttention();

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
        return ( other == GetServer().GetMe().GetNick() );
}

int Ui::TestHostPort( unsigned int port ) const
{
    return GetServer().TestOpenPort( port );
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
        DownloadMap( _T("") , mapname );
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
        wxMessageBox(_("Hi ") + wxGetUserName() + _(",\nIt looks like this is your first time using SpringLobby. I have guessed a configuration that I think will work for you but you should review it, especially the Spring configuration. \n\nWhen you are done you can go to the File menu, connect to a server, and enjoy a nice game of Spring :)"), _("Welcome"),
                     wxOK | wxICON_INFORMATION, &mw() );


        customMessageBoxNoModal(SL_MAIN_ICON, _("By default SpringLobby reports some usage statistics.\nYou can disable that on options tab --> General."),_("Notice"),wxOK );


                // copy uikeys.txt
                wxPathList pl;
                pl.AddEnvList( _T("%ProgramFiles%") );
                pl.AddEnvList( _T("XDG_DATA_DIRS") );
                pl = sett().GetAdditionalSearchPaths( pl );
                wxString uikeyslocation = pl.FindValidPath( _T("uikeys.txt") );
                if ( !uikeyslocation.IsEmpty() )
                {
                    wxCopyFile( uikeyslocation, sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator() + _T("uikeys.txt"), false );
                }

    #ifdef __WXMSW__
        if ( TASClientPresent() &&
                customMessageBox(SL_MAIN_ICON, _("Should I try to import (some) TASClient settings?\n" ),_("Import settings?"), wxYES_NO ) == wxYES )
        {
            ImportTASClientSettings();
        }
    #endif
        mw().SavePerspectives( _T("SpringLobby-default") );
        mw().ShowConfigure();
    }
    else
    {
        mw().ShowSingleplayer();
    }
}
