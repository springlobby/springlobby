/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: ServerEvents
//

#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/intl.h>
#include <stdexcept>

#include "serverevents.h"
#include "mainwindow.h"
#include "ui.h"
#include "channel/channel.h"
#include "user.h"
#include "utils/debug.h"
#include "uiutils.h"
#include "server.h"
#include "battle.h"
#include "httpdownloader.h"
#include "settings.h"
#include "utils/customdialogs.h"
#include "utils/tasutil.h"
#include "utils/uievents.h"

#ifndef NO_TORRENT_SYSTEM
#include "torrentwrapper.h"
#endif
#include "globalsmanager.h"

BEGIN_EVENT_TABLE(ServerEvents, wxEvtHandler)
    EVT_COMMAND(wxID_ANY, httpDownloadEvtComplete,  ServerEvents::OnSpringDownloadEvent)
    EVT_COMMAND(wxID_ANY, httpDownloadEvtFailed,    ServerEvents::OnSpringDownloadEvent)
END_EVENT_TABLE()

void ServerEvents::OnConnected( const wxString& server_name, const wxString& server_ver, bool supported, const wxString& server_spring_ver, bool /*unused*/ )
{
    wxLogDebugFunc( server_ver + _T(" ") + server_spring_ver );
    m_serv.SetRequiredSpring( server_spring_ver );
    ui().OnConnected( m_serv, server_name, server_ver, supported );
    m_serv.Login();
}


void ServerEvents::OnDisconnected( bool wasonline )
{
    wxLogDebugFunc( _T("") );
    m_serv.SetRequiredSpring (_T(""));
    ui().OnDisconnected( m_serv, wasonline );
}


void ServerEvents::OnLogin()
{
}


void ServerEvents::OnLoginInfoComplete()
{
    wxLogDebugFunc( _T("") );
	wxString nick = m_serv.GetMe().GetNick();
	wxArrayString highlights = sett().GetHighlightedWords();
	if ( highlights.Index( nick ) == -1 )
	{
		highlights.Add( nick );
		sett().SetHighlightedWords( highlights );
	}
    //m_serv.RequestChannels();
	GetGlobalEventSender(GlobalEvents::OnLogin).SendEvent( 0 );
    std::vector<ChannelJoinInfo> autojoin = sett().GetChannelsJoin();
	for ( std::vector<ChannelJoinInfo>::const_iterator itor = autojoin.begin(); itor != autojoin.end(); ++itor )
    {
		if ( itor->name.IsEmpty() )
			continue;
		Channel& chan = m_serv._AddChannel( itor->name );
		chan.SetPassword( itor->password );
		ui().OnJoinedChannelSuccessful( chan, itor == autojoin.begin() );
	}
	for ( std::vector<ChannelJoinInfo>::const_iterator itor = autojoin.begin(); itor != autojoin.end(); ++itor )
        m_serv.JoinChannel( itor->name, itor->password );

    ui().OnLoggedIn( );
}


void ServerEvents::OnLogout()
{
    //wxLogDebugFunc( _T("") );
}


void ServerEvents::OnUnknownCommand( const wxString& command, const wxString& params )
{
    wxLogDebugFunc( _T("") );
    ui().OnUnknownCommand( m_serv, command, params );
}


void ServerEvents::OnSocketError( const Sockerror& /*unused*/ )
{
    //wxLogDebugFunc( _T("") );
}


void ServerEvents::OnProtocolError( const Protocolerror /*unused*/ )
{
    //wxLogDebugFunc( _T("") );
}


void ServerEvents::OnMotd( const wxString& msg )
{
    wxLogDebugFunc( _T("") );
    ui().OnMotd( m_serv, msg );
}


void ServerEvents::OnPong( wxLongLong ping_time )
{
	//wxLongLong is non-POD and cannot be passed to wxFormat as such. use c-string rep instead. converting to long might loose precision
	UiEvents::StatusData data( wxString::Format( _("ping: %s ms"), ping_time.ToString().c_str() ), 2 );
	UiEvents::GetStatusEventSender( UiEvents::addStatusMessage ).SendEvent( data );
}


void ServerEvents::OnNewUser( const wxString& nick, const wxString& country, int cpu, const wxString& id )
{
    wxLogDebugFunc( _T("") );
    try
    {
        ASSERT_LOGIC( !m_serv.UserExists( nick ), _T("New user from server, but already exists!") );
    }
    catch (...)
    {
        return;
    }
    User& user = m_serv._AddUser( nick );
    if ( useractions().DoActionOnUser( UserActions::ActNotifLogin, nick ) )
        actNotifBox( SL_MAIN_ICON, nick + _(" is online") );
    user.SetCountry( country );
    user.SetCpu( cpu );
		user.SetID( id );
    ui().OnUserOnline( user );
}


void ServerEvents::OnUserStatus( const wxString& nick, UserStatus status )
{
    wxLogDebugFunc( _T("") );
    try
    {
        wxLogMessage( _T("calling m_serv.GetUser( nick ) ") );
        User& user = m_serv.GetUser( nick );
        wxLogMessage( _T("calling user.SetStatus( status ) ") );

        UserStatus oldStatus = user.GetStatus();
        user.SetStatus( status );
        if ( useractions().DoActionOnUser( UserActions::ActNotifStatus, nick ) )
        {
            wxString diffString = status.GetDiffString( oldStatus ) ;
            if ( diffString != wxEmptyString )
                actNotifBox( SL_MAIN_ICON, nick + _(" is now ") + diffString );
        }

        wxLogMessage( _T("calling ui().OnUserStatusChanged( user ) ") );
        ui().OnUserStatusChanged( user );
        wxLogMessage( _T("updating battles ") );

        if ( user.GetBattle() != 0 )
        {
            Battle& battle = *user.GetBattle();
            try
            {
            if ( battle.GetFounder().GetNick() == user.GetNick() )
            {
                if ( status.in_game != battle.GetInGame() )
                {
                    battle.SetInGame( status.in_game );
                    if ( status.in_game ) battle.StartSpring();
					else
						BattleEvents::GetBattleEventSender( BattleEvents::BattleInfoUpdate ).SendEvent( std::make_pair(user.GetBattle(),wxString()) );
                }
            }
            }catch(...){}
        }
    }
    catch (...)
    {
        wxLogWarning( _("OnUserStatus() failed ! (exception)") );
    }
}


void ServerEvents::OnUserQuit( const wxString& nick )
{
    wxLogDebugFunc( _T("") );
    try
    {
        User &user=m_serv.GetUser( nick );
				Battle* userbattle = user.GetBattle();
				if ( userbattle )
				{
					int battleid = userbattle->GetID();
					try
					{
						if ( &userbattle->GetFounder() == &user )
						{
							for ( int i = 0; i < int(userbattle->GetNumUsers()); i ++ )
							{
								User& battleuser = userbattle->GetUser( i );
								OnUserLeftBattle( battleid, battleuser.GetNick() );
							}
							 OnBattleClosed( battleid );
						}
						else OnUserLeftBattle( battleid, user.GetNick() );
					}catch(...){}
				}
        ui().OnUserOffline( user );
        m_serv._RemoveUser( nick );
        if ( useractions().DoActionOnUser( UserActions::ActNotifLogin, nick ) )
            actNotifBox( SL_MAIN_ICON, nick + _(" just went offline") );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnBattleOpened( int id, BattleType type, NatType nat, const wxString& nick,
                                   const wxString& host, int port, int maxplayers,
                                   bool haspass, int rank, const wxString& maphash, const wxString& map,
                                   const wxString& title, const wxString& mod )
{
    wxLogDebugFunc( _T("") );
    try
    {
        ASSERT_EXCEPTION( !m_serv.BattleExists( id ), _T("New battle from server, but already exists!") );
        Battle& battle = m_serv._AddBattle( id );

        User& user = m_serv.GetUser( nick );
        battle.OnUserAdded( user );

        battle.SetBattleType( type );
        battle.SetNatType( nat );
        battle.SetFounder( nick );
        battle.SetHostIp( host );
        battle.SetHostPort( port );
        battle.SetMaxPlayers( maxplayers );
        battle.SetIsPassworded( haspass );
        battle.SetRankNeeded( rank );
        battle.SetHostMap( map, maphash );
        battle.SetDescription( title );
        battle.SetHostMod( mod, wxEmptyString );

        if ( useractions().DoActionOnUser( UserActions::ActNotifBattle, user.GetNick() ) )
            actNotifBox( SL_MAIN_ICON, user.GetNick() + _(" opened battle ") + title );

        ui().OnBattleOpened( battle );
        if ( user.Status().in_game )
        {
            battle.SetInGame( true );
            battle.StartSpring();
        }
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnJoinedBattle( int battleid, const wxString& hash )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );

        battle.SetHostMod( battle.GetHostModName(), hash );

        UserBattleStatus& bs = m_serv.GetMe().BattleStatus();
        bs.spectator = false;

        if ( !battle.IsFounderMe() || battle.IsProxy() )
        {
            battle.CustomBattleOptions().loadOptions( OptionsWrapper::MapOption, battle.GetHostMapName() );
            battle.CustomBattleOptions().loadOptions( OptionsWrapper::ModOption, battle.GetHostModName() );
        }

        ui().OnJoinedBattle( battle );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnHostedBattle( int battleid )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );

				if ( battle.GetBattleType() == BT_Played )
				{
					battle.CustomBattleOptions().loadOptions( OptionsWrapper::MapOption, battle.GetHostMapName() );
					battle.CustomBattleOptions().loadOptions( OptionsWrapper::ModOption, battle.GetHostModName() );
				}
				else
				{
					battle.GetBattleFromScript( true );
				}


        wxString presetname = sett().GetModDefaultPresetName( battle.GetHostModName() );
        if ( !presetname.IsEmpty() )
        {
            battle.LoadOptionsPreset( presetname );
        }

        battle.LoadMapDefaults( battle.GetHostMapName() );

        m_serv.SendHostInfo( IBattle::HI_Send_All_opts );

        ui().OnHostedBattle( battle );
    }
    catch (assert_exception) {}
}


void ServerEvents::OnStartHostedBattle( int battleid )
{
    wxLogDebugFunc( _T("") );
    Battle& battle = m_serv.GetBattle( battleid );
    battle.SetInGame( true );
    battle.StartSpring();
}


void ServerEvents::OnClientBattleStatus( int battleid, const wxString& nick, UserBattleStatus status )
{
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        User& user = battle.GetUser( nick );

		//if ( battle.IsFounderMe() ) AutoCheckCommandSpam( battle, user );

        status.color_index = user.BattleStatus().color_index;
        battle.OnUserBattleStatusUpdated( user, status );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnUserJoinedBattle( int battleid, const wxString& nick, const wxString& userScriptPassword )
{
    try
    {
        wxLogDebugFunc( _T("") );
        User& user = m_serv.GetUser( nick );
        Battle& battle = m_serv.GetBattle( battleid );

        battle.OnUserAdded( user );
		user.BattleStatus().scriptPassword = userScriptPassword;
        ui().OnUserJoinedBattle( battle, user );
				try
				{
					if ( &user == &battle.GetFounder() )
					{
							if ( user.Status().in_game )
							{
									battle.SetInGame( true );
									battle.StartSpring();
							}
					}
        }catch(...){}
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnUserLeftBattle( int battleid, const wxString& nick )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        User& user = battle.GetUser( nick );
        // this is necessary since the user will be deleted when the gui function is called
        bool isbot = user.BattleStatus().IsBot();
		user.BattleStatus().scriptPassword.Clear();
        battle.OnUserRemoved( user );
        ui().OnUserLeftBattle( battle, user, isbot );
    }
    catch (std::runtime_error &except)
    {
    }

}


void ServerEvents::OnBattleInfoUpdated( int battleid, int spectators, bool locked, const wxString& maphash, const wxString& map )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );

        battle.SetSpectators( spectators );
        battle.SetIsLocked( locked );

        wxString oldmap = battle.GetHostMapName();

        battle.SetHostMap( map, maphash );

        if ( (oldmap != map) && (battle.UserExists( m_serv.GetMe().GetNick())) )
        {
            battle.SendMyBattleStatus();
            battle.CustomBattleOptions().loadOptions( OptionsWrapper::MapOption, map );
            battle.Update( wxString::Format( _T("%d_mapname"), OptionsWrapper::PrivateOptions ) );
        }

		BattleEvents::GetBattleEventSender( BattleEvents::BattleInfoUpdate ).SendEvent( std::make_pair(&battle,wxString()) );
    }
    catch (assert_exception) {}
}

void ServerEvents::OnSetBattleInfo( int battleid, const wxString& param, const wxString& value )
{
    wxLogDebugFunc( param + _T(", ") + value );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
		battle.m_script_tags[param] = value;
        wxString key = param;
        if ( key.Left( 5 ) == _T("game/") )
        {
            key = key.AfterFirst( '/' );
            if ( key.Left( 11 ) == _T( "mapoptions/" ) )
            {
                key = key.AfterFirst( '/' );
                battle.CustomBattleOptions().setSingleOption( key,  value, OptionsWrapper::MapOption );
								battle.Update( wxString::Format(_T("%d_%s"), OptionsWrapper::MapOption, key.c_str() ) );
            }
            else if ( key.Left( 11 ) == _T( "modoptions/" ) )
            {
                key = key.AfterFirst( '/' );
								battle.CustomBattleOptions().setSingleOption( key, value, OptionsWrapper::ModOption );
                battle.Update(  wxString::Format(_T("%d_%s"), OptionsWrapper::ModOption,  key.c_str() ) );
            }
            else if ( key.Left( 8 ) == _T( "restrict" ) )
            {
            	OnBattleDisableUnit( battleid, key.AfterFirst(_T('/')), s2l(value) );
            }
            else if ( key.Left( 4 ) == _T( "team" ) && key.Find( _T("startpos") ) != wxNOT_FOUND )
            {
            	 int team = s2l( key.BeforeFirst(_T('/')).Mid( 4 ) );
							 if ( key.Find( _T("startposx") ) != wxNOT_FOUND )
							 {
							 	 int numusers = battle.GetNumUsers();
							 	 for ( int i = 0; i < numusers; i++ )
							 	 {
							 	 	 User& usr = battle.GetUser( i );
							 	 	 UserBattleStatus& status = usr.BattleStatus();
							 	 	 if ( status.team == team )
							 	 	 {
							 	 	 	 status.pos.x = s2l( value );
										 battle.OnUserBattleStatusUpdated( usr, status );
							 	 	 }
							 	 }
							 }
							 else if ( key.Find( _T("startposy") ) != wxNOT_FOUND )
							 {
							 	 int numusers = battle.GetNumUsers();
							 	 for ( int i = 0; i < numusers; i++ )
							 	 {
							 	 	 User& usr = battle.GetUser( i );
							 	 	 UserBattleStatus& status = usr.BattleStatus();
							 	 	 if ( status.team == team )
							 	 	 {
							 	 	 	 status.pos.y = s2l( value );
							 	 	 	 battle.OnUserBattleStatusUpdated( usr, status );
							 	 	 }
							 	 }
							 }
            }
            else
            {
							battle.CustomBattleOptions().setSingleOption( key,  value, OptionsWrapper::EngineOption );
							battle.Update( wxString::Format(_T("%d_%s"), OptionsWrapper::EngineOption, key.c_str() ) );
            }
        }
    }
    catch (assert_exception) {}
}


void ServerEvents::OnBattleInfoUpdated( int battleid )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
		BattleEvents::GetBattleEventSender( BattleEvents::BattleInfoUpdate ).SendEvent( std::make_pair(&battle,wxString()) );
    }
    catch ( assert_exception ) {}
}


void ServerEvents::OnBattleClosed( int battleid )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );

        ui().OnBattleClosed( battle );

        m_serv._RemoveBattle( battleid );
    }
    catch ( assert_exception ) {}
}


void ServerEvents::OnBattleDisableUnit( int battleid, const wxString& unitname, int count )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        battle.RestrictUnit( unitname, count );
        battle.Update( wxString::Format( _T("%d_restrictions"), OptionsWrapper::PrivateOptions ) );
    }
    catch ( assert_exception ) {}
}


void ServerEvents::OnBattleEnableUnit( int battleid, const wxString& unitname )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        battle.UnrestrictUnit( unitname );
        battle.Update( wxString::Format( _T("%d_restrictions"), OptionsWrapper::PrivateOptions ) );
    }
    catch ( assert_exception ) {}
}


void ServerEvents::OnBattleEnableAllUnits( int battleid )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        battle.UnrestrictAllUnits();
        battle.Update( wxString::Format( _T("%d_restrictions"), OptionsWrapper::PrivateOptions ) );
    }
    catch ( assert_exception ) {}
}


void ServerEvents::OnJoinChannelResult( bool success, const wxString& channel, const wxString& reason )
{
    wxLogDebugFunc( _T("") );
    if ( success )
    {
        Channel& chan = m_serv._AddChannel( channel );
        chan.SetPassword( m_serv.m_channel_pw[channel] );
        ui().OnJoinedChannelSuccessful( chan );

    }
    else
    {
        ui().ShowMessage( _("Join channel failed"), _("Could not join channel ") + channel + _(" because: ") + reason );
    }
}


void ServerEvents::OnChannelSaid( const wxString& channel, const wxString& who, const wxString& message )
{
    wxLogDebugFunc( _T("") );
    try
    {
        if ( ( m_serv.GetMe().GetNick() ==  who ) || !useractions().DoActionOnUser( UserActions::ActIgnoreChat, who ) )
            m_serv.GetChannel( channel ).Said( m_serv.GetUser( who ), message );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnChannelJoin( const wxString& channel, const wxString& who )
{
    wxLogDebugFunc( _T("") );
    try
    {
        m_serv.GetChannel( channel ).OnChannelJoin( m_serv.GetUser( who ) );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnChannelPart( const wxString& channel, const wxString& who, const wxString& message )
{
    wxLogDebugFunc( _T("") );
    try
    {
        m_serv.GetChannel( channel ).Left( m_serv.GetUser( who ), message );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnChannelTopic( const wxString& channel, const wxString& who, const wxString& message, int /*unused*/ )
{
    wxLogDebugFunc( _T("") );
    try
    {
        m_serv.GetChannel( channel ).SetTopic( message, who );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnChannelAction( const wxString& channel, const wxString& who, const wxString& action )
{
    wxLogDebugFunc( _T("") );
    try
    {
		if ( ( m_serv.GetMe().GetNick() ==  who ) || !useractions().DoActionOnUser( UserActions::ActIgnoreChat, who ) )
			m_serv.GetChannel( channel ).DidAction( m_serv.GetUser( who ), action );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnPrivateMessage( const wxString& user, const wxString& message, bool fromme )
{
    wxLogDebugFunc( _T("") );
    try
    {
        User& who = m_serv.GetUser( user );
        if (!useractions().DoActionOnUser( UserActions::ActIgnorePM, who.GetNick() ) )
            ui().OnUserSaid( who, message, fromme );
    }
    catch (std::runtime_error &except)
    {
    }
}

void ServerEvents::OnPrivateMessageEx( const wxString& user, const wxString& action, bool fromme )
{
	wxLogDebugFunc( _T("") );
	try
	{
		User& who = m_serv.GetUser( user );
		if (!useractions().DoActionOnUser( UserActions::ActIgnorePM, who.GetNick() ) )
			ui().OnUserSaidEx( who, action, fromme );
	}
	catch (std::runtime_error &except)
	{
	}
}

void ServerEvents::OnChannelList( const wxString& channel, const int& numusers, const wxString& topic )
{
    ui().mw().OnChannelList( channel, numusers, topic );
}


void ServerEvents::OnUserJoinChannel( const wxString& channel, const wxString& who )
{
    wxLogDebugFunc( _T("") );
    try
    {
        m_serv.GetChannel( channel ).Joined( m_serv.GetUser( who ) );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnRequestBattleStatus( int battleid )
{
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        ui().OnRequestBattleStatus( battle );
    }
    catch (assert_exception) {}
}


void ServerEvents::OnSaidBattle( int battleid, const wxString& nick, const wxString& msg )
{
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
		if ( ( m_serv.GetMe().GetNick() ==  nick ) || !useractions().DoActionOnUser( UserActions::ActIgnoreChat, nick ) )
		{
			ui().OnSaidBattle( battle, nick, msg );
		}
        battle.GetAutoHost().OnSaidBattle( nick, msg );
    }
    catch (assert_exception) {}
}

void ServerEvents::OnBattleAction( int /*battleid*/, const wxString& nick, const wxString& msg )
{
    try
    {
		UiEvents::GetUiEventSender( UiEvents::OnBattleActionEvent ).SendEvent(
				UiEvents::OnBattleActionData( nick, msg )
			);
    }
    catch (assert_exception) {}
}


void ServerEvents::OnBattleStartRectAdd( int battleid, int allyno, int left, int top, int right, int bottom )
{
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        battle.AddStartRect( allyno, left, top, right, bottom );
        battle.StartRectAdded( allyno );
        battle.Update( wxString::Format( _T("%d_mapname"), OptionsWrapper::PrivateOptions ) );
    }
    catch (assert_exception) {}
}


void ServerEvents::OnBattleStartRectRemove( int battleid, int allyno )
{
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        battle.RemoveStartRect( allyno );
        battle.StartRectRemoved( allyno );
        battle.Update( wxString::Format( _T("%d_mapname"), OptionsWrapper::PrivateOptions ) );
    }
    catch (assert_exception) {}
}


void ServerEvents::OnBattleAddBot( int battleid, const wxString& nick, UserBattleStatus status )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        battle.OnBotAdded( nick, status );
        User& bot = battle.GetUser( nick );
        ASSERT_LOGIC( &bot != 0, _T("Bot null after add.") );
        ui().OnUserJoinedBattle( battle, bot );
    }
    catch (assert_exception) {}
}

void ServerEvents::OnBattleUpdateBot( int battleid, const wxString& nick, UserBattleStatus status )
{
    OnClientBattleStatus( battleid, nick, status );
}


void ServerEvents::OnBattleRemoveBot( int battleid, const wxString& nick )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
		User& user = battle.GetUser( nick );
		bool isbot = user.BattleStatus().IsBot();
		ui().OnUserLeftBattle( battle, user, isbot );
        battle.OnUserRemoved( user );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnAcceptAgreement( const wxString& agreement )
{
    ui().OnAcceptAgreement( agreement );
}


void ServerEvents::OnRing( const wxString& from )
{
    ui().OnRing( from );
}

void ServerEvents::OnServerBroadcast( const wxString& message )
{
	ui().OnServerBroadcast( m_serv, message );
}

void ServerEvents::OnServerMessage( const wxString& message )
{
    ui().OnServerMessage( m_serv, message );
}


void ServerEvents::OnServerMessageBox( const wxString& message )
{
    ui().ShowMessage( _("Server Message"), message );
}


void ServerEvents::OnChannelMessage( const wxString& channel, const wxString& msg )
{
    ui().OnChannelMessage( channel, msg );
}


void ServerEvents::OnHostExternalUdpPort( const unsigned int udpport )
{
    if ( !m_serv.GetCurrentBattle() ) return;
    if ( m_serv.GetCurrentBattle()->GetNatType() == NAT_Hole_punching || m_serv.GetCurrentBattle()->GetNatType() == NAT_Fixed_source_ports ) m_serv.GetCurrentBattle()->SetHostPort( udpport );
}


void ServerEvents::OnMyInternalUdpSourcePort( const unsigned int udpport )
{
    if ( !m_serv.GetCurrentBattle() ) return;
    m_serv.GetCurrentBattle()->SetMyInternalUdpSourcePort(udpport);
}


void ServerEvents::OnMyExternalUdpSourcePort( const unsigned int udpport )
{
    if ( !m_serv.GetCurrentBattle() ) return;
    m_serv.GetCurrentBattle()->SetMyExternalUdpSourcePort(udpport);
}

void ServerEvents::OnClientIPPort( const wxString &username, const wxString &ip, unsigned int udpport )
{
    wxLogMessage(_T("OnClientIPPort(%s,%s,%d)"),username.c_str(),ip.c_str(),udpport);
    if ( !m_serv.GetCurrentBattle() )
    {
        wxLogMessage(_T("GetCurrentBattle() returned null"));
        return;
    }
    try
    {
        User &user=m_serv.GetCurrentBattle()->GetUser( username );

        user.BattleStatus().ip=ip;
        user.BattleStatus().udpport=udpport;
        wxLogMessage(_T("set to %s %d "),user.BattleStatus().ip.c_str(),user.BattleStatus().udpport);

		if (sett().GetShowIPAddresses()) {
			UiEvents::GetUiEventSender( UiEvents::OnBattleActionEvent ).SendEvent(
					UiEvents::OnBattleActionData( username,wxString::Format(_(" has ip=%s"),ip.c_str()) )
				);
		}

        if (m_serv.GetCurrentBattle()->GetNatType() != NAT_None && (udpport==0))
        {
            // todo: better warning message
            //something.OutputLine( _T(" ** ") + who.GetNick() + _(" does not support nat traversal! ") + GetChatTypeStr() + _T("."), sett().GetChatColorJoinPart(), sett().GetChatFont() );
			UiEvents::GetUiEventSender( UiEvents::OnBattleActionEvent ).SendEvent(
					UiEvents::OnBattleActionData( username,_(" does not really support nat traversal") )
				);

        }
        m_serv.GetCurrentBattle()->CheckBan(user);
    }
    catch (std::runtime_error)
    {
        wxLogMessage(_T("runtime_error inside OnClientIPPort()"));
    }
}


void ServerEvents::OnKickedFromBattle()
{
    customMessageBoxNoModal(SL_MAIN_ICON,_("You were kicked from the battle!"),_("Kicked by Host"));
}


void ServerEvents::OnRedirect( const wxString& address,  unsigned int port, const wxString& CurrentNick, const wxString& CurrentPassword )
{
		wxString name = address + _T(":") + TowxString(port);
    sett().SetServer( name, address, port );
    ui().DoConnect( name, CurrentNick, CurrentPassword );
}


void ServerEvents::AutoCheckCommandSpam( Battle& battle, User& user )
{
    wxString nick = user.GetNick();
    MessageSpamCheck info = m_spam_check[nick];
    time_t now = time( 0 );
    if ( info.lastmessage == now ) info.count++;
    else info.count = 0;
    info.lastmessage = now;
    m_spam_check[nick] = info;
    if ( info.count == 7 )
    {
			battle.DoAction( _T("is autokicking ") + nick + _T(" due to command spam.") );
			battle.KickPlayer( user );
    }
}

void ServerEvents::OnMutelistBegin( const wxString& channel )
{
    mutelistWindow( _("Begin mutelist for ") + channel, wxString::Format( _("%s mutelist"), channel.c_str() ) );
}

void ServerEvents::OnMutelistItem( const wxString& /*unused*/, const wxString& mutee, const wxString& description )
{
    wxString message = mutee;
    wxString desc = description;
    wxString mutetime = GetWordParam( desc );
		long time;
		if ( mutetime == _T("indefinite") ) message << _(" indefinite time remaining");
		else if ( mutetime.ToLong(&time) ) message << wxString::Format( _(" %d minutes remaining"), time/60 + 1 );
		else message << mutetime;
		if ( !desc.IsEmpty() )  message << _T(", ") << desc;
    mutelistWindow( message );
}

void ServerEvents::OnMutelistEnd( const wxString& channel )
{
    mutelistWindow( _("End mutelist for ") + channel );
}

void ServerEvents::OnScriptStart( int battleid )
{
	if ( !m_serv.BattleExists( battleid ) )
	{
			return;
	}
	m_serv.GetBattle( battleid ).ClearScript();
}

void ServerEvents::OnScriptLine( int battleid, const wxString& line )
{
	if ( !m_serv.BattleExists( battleid ) )
	{
			return;
	}
	m_serv.GetBattle( battleid ).AppendScriptLine( line );
}

void ServerEvents::OnScriptEnd( int battleid )
{
	if ( !m_serv.BattleExists( battleid ) )
	{
			return;
	}
	m_serv.GetBattle( battleid ).GetBattleFromScript( true );
}


void ServerEvents::OnFileDownload( bool autolaunch, bool autoclose, bool /*disconnectonrefuse*/, const wxString& FileName, const wxString& url, const wxString& description )
{
	if ( sett().IgnoreOfferfile() )
		return;
	wxString refinedurl;
	if ( url.Find(_T("http://")) != wxNOT_FOUND ) refinedurl = url.AfterFirst(_T('/')).AfterFirst(_T('/'));
	else refinedurl = url;
	bool result = ui().Ask( _("Download update"), wxString::Format( _("Would you like to download %s ? The file offers the following updates:\n\n%s\n\nThe download will be started in the background, you will be notified on operation completed."), url.c_str(), description.c_str() ) );
	if ( result )
	{
		m_autoclose = autoclose;
		m_autolaunch = autolaunch;
		wxString filename;
		if ( FileName != _T("*") ) filename = FileName;
		else filename = _T("Spring installer.exe");
		m_savepath = sett().GetCurrentUsedDataDir() + filename;
		wxLogMessage(_T("downloading update in: %s, from: %s"),m_savepath.c_str(),refinedurl.c_str());
		OpenWebBrowser( url );
		//new HttpDownloaderThread<ServerEvents>( refinedurl, m_savepath, *this, wxID_HIGHEST + 100, true, false );
	}
}
void ServerEvents::OnSpringDownloadEvent( wxCommandEvent& event )
{
	int code = event.GetInt();
	wxLogMessage(event.GetString());
  if ( code != 0)
  {
  	 customMessageBox(SL_MAIN_ICON, _("There was an error downloading for the latest version.\n"), _("Error"));
		wxString err;
    switch (code)
    {
      case wxPROTO_NETERR:
        err = _("Network Error");
        break;
      case wxPROTO_PROTERR:
        err = _("Negotiation error");
        break;
      case wxPROTO_CONNERR:
        err = _T("Failed to connect to server");
        break;
      case wxPROTO_INVVAL:
        err = _("Invalid Value");
        break;
      case wxPROTO_NOHNDLR:
        err = _("No Handler");
        break;
      case wxPROTO_NOFILE:
        err = _("File doesn't exit");
        break;
      case wxPROTO_ABRT:
        err = _("Action Aborted");
        break;
      case wxPROTO_RCNCT:
        err = _("Reconnection Error");
        break;
      default:
        err = _("Unknown Error");
        break;
    }

    wxLogDebugFunc(_T("Error connecting! Error is: ") + err);
    customMessageBoxNoModal(SL_MAIN_ICON, _T("Error connecting! (") + err + _T(")\nPlease update manually from http://springrts.com"), _T(""));

  }
  else
  {
			wxString text =  _("Download complete, location is: ") + m_savepath;
			if ( m_autoclose ) text += _("\nlobby will get closed now.");
			customMessageBox(SL_MAIN_ICON, text, _("Download complete.")  );
			if ( m_autolaunch )
			{
				if ( !wxExecute( _T("\"") + m_savepath + _T("\""), wxEXEC_ASYNC ) )
				{
						customMessageBoxNoModal(SL_MAIN_ICON, _("Couldn't launch installer. File location is: ") + m_savepath, _("Couldn't launch installer.")  );
				}
			}
			if ( m_autoclose )
			{
				ui().mw().Close();
			}

  }
}
