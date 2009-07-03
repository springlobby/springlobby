/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: Battle
//
#include "battle.h"
#include "ui.h"
#include "iunitsync.h"
#include "server.h"
#include "user.h"
#include "utils/misc.h"
#include "utils/debug.h"
#include "utils/conversion.h"
#include "utils/math.h"
#include "uiutils.h"
#include "settings.h"
#include "useractions.h"
#include "settings++/custom_dialogs.h"
#include "springunitsynclib.h"
#include "iconimagelist.h"
#include "spring.h"

#include <wx/timer.h>
#include <wx/image.h>
#include <wx/string.h>
#include <wx/log.h>
#include <wx/filename.h>

const unsigned int TIMER_INTERVAL         = 1000;
const unsigned int TIMER_ID               = 101;

BEGIN_EVENT_TABLE(Battle, wxEvtHandler)
    EVT_TIMER(TIMER_ID, Battle::OnTimer)
END_EVENT_TABLE()

Battle::Battle( Server& serv, int id ) :
        m_serv(serv),
        m_ah(*this),
        m_autolock_on_start(false),
        m_id( id )

{
    m_opts.battleid =  m_id;
}


Battle::~Battle()
{
    if ( m_is_self_in )
        susynclib().UnSetCurrentMod();
}


void Battle::SendHostInfo( HostInfo update )
{
    m_serv.SendHostInfo( update );
}


void Battle::SendHostInfo( const wxString& Tag )
{
    m_serv.SendHostInfo( Tag );
}


void Battle::Update()
{
    ui().OnBattleInfoUpdated( *this );
}


void Battle::Update( const wxString& Tag )
{
    ui().OnBattleInfoUpdated( *this, Tag );
}


void Battle::Join( const wxString& password )
{
    m_serv.JoinBattle( m_opts.battleid, password );
    m_is_self_in = true;
}


void Battle::Leave()
{
    m_serv.LeaveBattle( m_opts.battleid );
    m_is_self_in = false;
    susynclib().UnSetCurrentMod( );
}


void Battle::OnRequestBattleStatus()
{
    int lowest = GetFreeTeamNum( true );

    UserBattleStatus& bs = m_serv.GetMe().BattleStatus();
    bs.team = lowest;
    bs.ally = lowest;
    bs.spectator = false;
    bs.colour = sett().GetBattleLastColour();
    // theres some highly annoying bug with color changes on player join/leave.
    if ( !bs.colour.IsColourOk() ) bs.colour = GetFreeColour( GetMe() );

    SendMyBattleStatus();
}


void Battle::SendMyBattleStatus()
{
    UserBattleStatus& bs = m_serv.GetMe().BattleStatus();
    if ( IsSynced() ) bs.sync = SYNC_SYNCED;
    else bs.sync = SYNC_UNSYNCED;
    m_serv.SendMyBattleStatus( bs );
}


void Battle::SetImReady( bool ready )
{
    UserBattleStatus& bs = m_serv.GetMe().BattleStatus();

    bs.ready = ready;

    //m_serv.GetMe().SetBattleStatus( bs );
    SendMyBattleStatus();
}





/*bool Battle::HasMod()
{
  return usync().ModExists( m_opts.modname );
}*/


void Battle::Say( const wxString& msg )
{
    m_serv.SayBattle( m_opts.battleid, msg );
}


void Battle::DoAction( const wxString& msg )
{
    m_serv.DoActionBattle( m_opts.battleid, msg );
}

void Battle::SetLocalMap( const UnitSyncMap& map )
{
	IBattle::SetLocalMap( map );
	if ( IsFounderMe() )  LoadMapDefaults( map.name );
}

User& Battle::GetMe()
{
    return m_serv.GetMe();
}

void Battle::SaveMapDefaults()
{
    // save map preset
		wxString mapname = LoadMap().name;
		wxString startpostype = CustomBattleOptions().getSingleValue( _T("startpostype"), OptionsWrapper::EngineOption );
		sett().SetMapLastStartPosType( mapname, startpostype);
		std::vector<Settings::SettStartBox> rects;
		for( unsigned int i = 0; i <= GetLastRectIdx(); ++i )
		{
			 BattleStartRect rect = GetStartRect( i );
			 if ( rect.IsOk() )
			 {
				 Settings::SettStartBox box;
				 box.ally = rect.ally;
				 box.topx = rect.left;
				 box.topy = rect.top;
				 box.bottomx = rect.right;
				 box.bottomy = rect.bottom;
				 rects.push_back( box );
			 }
		}
		sett().SetMapLastRectPreset( mapname, rects );
}

void Battle::LoadMapDefaults( const wxString& mapname )
{
	CustomBattleOptions().setSingleOption( _T("startpostype"), sett().GetMapLastStartPosType( mapname ), OptionsWrapper::EngineOption );
	SendHostInfo( wxString::Format( _T("%d_startpostype"), OptionsWrapper::EngineOption ) );

	for( unsigned int i = 0; i <= GetLastRectIdx(); ++i ) if ( GetStartRect( i ).IsOk() ) RemoveStartRect(i); // remove all rects
	SendHostInfo( IBattle::HI_StartRects );

	std::vector<Settings::SettStartBox> savedrects = sett().GetMapLastRectPreset( mapname );
	for ( std::vector<Settings::SettStartBox>::iterator itor = savedrects.begin(); itor != savedrects.end(); itor++ )
	{
		AddStartRect( itor->ally, itor->topx, itor->topy, itor->bottomx, itor->bottomy );
	}
	SendHostInfo( IBattle::HI_StartRects );
}

User& Battle::OnUserAdded( User& user )
{
		user = IBattle::OnUserAdded( user );
		if ( &user == &GetMe() )
		{
			 m_timer = new wxTimer(this,TIMER_ID);
			 m_timer->Start( TIMER_INTERVAL );
		}
    user.SetBattle( this );
    user.BattleStatus().isfromdemo = false;

    if ( IsFounderMe() )
    {
        if ( CheckBan( user ) ) return user;

        if ( ( &user != &GetMe() ) && !user.BattleStatus().IsBot() && ( m_opts.rankneeded > UserStatus::RANK_1 ) && ( user.GetStatus().rank < m_opts.rankneeded ))
        {
            switch ( m_opts.ranklimittype )
            {
            case rank_limit_none:
                break;
            case rank_limit_autospec:
                if ( !user.BattleStatus().spectator )
                {
                    DoAction( _T("Rank limit autospec: ") + user.GetNick() );
                    ForceSpectator( user, true );
                }
                break;
            case rank_limit_autokick:
                DoAction( _T("Rank limit autokick: ") + user.GetNick() );
                KickPlayer( user );
                return user;
            }
        }

        m_ah.OnUserAdded( user );
        if ( !user.BattleStatus().IsBot() && sett().GetBattleLastAutoAnnounceDescription() ) DoAction( m_opts.description );
    }
    // any code here may be skipped if the user was autokicked
    return user;
}

void Battle::OnUserBattleStatusUpdated( User &user, UserBattleStatus status )
{
    if ( IsFounderMe() )
    {
        if ( ( &user != &GetMe() ) && !status.IsBot() && ( m_opts.rankneeded > UserStatus::RANK_1 ) && ( user.GetStatus().rank < m_opts.rankneeded ))
        {
            switch ( m_opts.ranklimittype )
            {
            case rank_limit_none:
                break;
            case rank_limit_autospec:
                if ( !status.spectator )
                {
                    DoAction( _T("Rank limit autospec: ") + user.GetNick() );
                    ForceSpectator( user, true );
                }
                break;
            case rank_limit_autokick:
                DoAction( _T("Rank limit autokick: ") + user.GetNick() );
                KickPlayer( user );
                break;
            }
        }

    }
		IBattle::OnUserBattleStatusUpdated( user, status );
    if ( status.handicap != 0 )
    {
        ui().OnBattleAction( *this, wxString(_T(" ")) , ( _T("Warning: user ") + user.GetNick() + _T(" got bonus ") ) << status.handicap );
    }
		if ( IsFounderMe() )
		{
			if ( ShouldAutoStart() )
			{
				if ( sett().GetBattleLastAutoControlState() )
				{
					FixTeamIDs( (IBattle::BalanceType)sett().GetFixIDMethod(), sett().GetFixIDClans(), sett().GetFixIDStrongClans(), sett().GetFixIDGrouping() );
					Autobalance( (IBattle::BalanceType)sett().GetBalanceMethod(), sett().GetBalanceClans(), sett().GetBalanceStrongClans(), sett().GetBalanceGrouping() );
					FixColours();
				}
				if ( sett().GetBattleLastAutoStartState() )
				{
					ui().StartHostedBattle();
				}
			}
		}
		ui().OnUserBattleStatus( *this, user );
}


void Battle::OnUserRemoved( User& user )
{
    m_ah.OnUserRemoved(user);
    IBattle::OnUserRemoved( user );
}


void Battle::RingNotReadyPlayers()
{
    for (user_map_t::size_type i = 0; i < GetNumUsers(); i++)
    {
        User& u = GetUser(i);
        UserBattleStatus& bs = u.BattleStatus();
        if ( bs.IsBot() ) continue;
        if ( !bs.ready && !bs.spectator ) m_serv.Ring( u.GetNick() );
    }
}

void Battle::RingNotSyncedPlayers()
{
    for (user_map_t::size_type i = 0; i < GetNumUsers(); i++)
    {
        User& u = GetUser(i);
        UserBattleStatus& bs = u.BattleStatus();
        if ( bs.IsBot() ) continue;
        if ( !bs.sync && !bs.spectator ) m_serv.Ring( u.GetNick() );
    }
}

void Battle::RingNotSyncedAndNotReadyPlayers()
{
    for (user_map_t::size_type i = 0; i < GetNumUsers(); i++)
    {
        User& u = GetUser(i);
        UserBattleStatus& bs = u.BattleStatus();
        if ( bs.IsBot() ) continue;
        if ( ( !bs.sync || !bs.ready ) && !bs.spectator ) m_serv.Ring( u.GetNick() );
    }
}

void Battle::RingPlayer( const User& u )
{
	if ( u.BattleStatus().IsBot() ) return;
	m_serv.Ring( u.GetNick() );
}

bool Battle::ExecuteSayCommand( const wxString& cmd )
{
    wxString cmd_name=cmd.BeforeFirst(' ').Lower();
    if ( cmd_name == _T("/me") )
    {
        m_serv.DoActionBattle( m_opts.battleid, cmd.AfterFirst(' ') );
        return true;
    }
		if ( cmd_name == _T("/replacehostip") )
		{
				wxString ip = cmd.AfterFirst(' ');
				if ( ip.IsEmpty() ) return false;
				m_opts.ip = ip;
				return true;
		}
    //< quick hotfix for bans
    if (IsFounderMe())
    {
        if ( cmd_name == _T("/ban") )
        {
            wxString nick=cmd.AfterFirst(' ');
            m_banned_users.insert(nick);
            try
						{
							User& user = GetUser( nick );
							m_serv.BattleKickPlayer( m_opts.battleid, user );
						}
						catch( assert_exception ) {}
            ui().OnBattleAction(*this,wxString(_T(" ")),nick+_T(" banned"));
            //m_serv.DoActionBattle( m_opts.battleid, cmd.AfterFirst(' ') );
            return true;
        }
        if ( cmd_name == _T("/unban") )
        {
            wxString nick=cmd.AfterFirst(' ');
            m_banned_users.erase(nick);
            ui().OnBattleAction(*this,wxString(_T(" ")),nick+_T(" unbanned"));
            //m_serv.DoActionBattle( m_opts.battleid, cmd.AfterFirst(' ') );
            return true;
        }
        if ( cmd_name == _T("/banlist") )
        {
            ui().OnBattleAction(*this,wxString(_T(" ")),_T("banlist:"));
            for (std::set<wxString>::iterator i=m_banned_users.begin();i!=m_banned_users.end();++i)
            {
                ui().OnBattleAction(*this,wxString(_T("  ")), *i);
            }
            for (std::set<wxString>::iterator i=m_banned_ips.begin();i!=m_banned_ips.end();++i)
            {
                ui().OnBattleAction(*this,wxString(_T("  ")), *i);
            }
            return true;
        }
        if ( cmd_name == _T("/unban") )
        {
            wxString nick=cmd.AfterFirst(' ');
            m_banned_users.erase(nick);
            m_banned_ips.erase(nick);
            ui().OnBattleAction(*this,wxString(_T(" ")),nick+_T(" unbanned"));
            //m_serv.DoActionBattle( m_opts.battleid, cmd.AfterFirst(' ') );
            return true;
        }
        if ( cmd_name == _T("/ipban") )
        {
            wxString nick=cmd.AfterFirst(' ');
            m_banned_users.insert(nick);
            ui().OnBattleAction(*this,wxString(_T(" ")),nick+_T(" banned"));
            if (UserExists(nick))
            {
                User &user=GetUser(nick);
                if (!user.BattleStatus().ip.empty())
                {
                    m_banned_ips.insert(user.BattleStatus().ip);
                    ui().OnBattleAction(*this,wxString(_T(" ")),user.BattleStatus().ip+_T(" banned"));
                }
                m_serv.BattleKickPlayer( m_opts.battleid, user );
            }
            //m_banned_ips.erase(nick);

            //m_serv.DoActionBattle( m_opts.battleid, cmd.AfterFirst(' ') );
            return true;
        }
    }
    //>
    return false;
}
///< quick hotfix for bans
/// returns true if user is banned (and hence has been kicked)
bool Battle::CheckBan(User &user)
{
    if (IsFounderMe())
    {
        if (m_banned_users.count(user.GetNick())>0
                || useractions().DoActionOnUser(UserActions::ActAutokick, user.GetNick() ) )
        {
            KickPlayer(user);
            ui().OnBattleAction(*this,wxString(_T(" ")),user.GetNick()+_T(" is banned, kicking"));
            return true;
        }
        else if (m_banned_ips.count(user.BattleStatus().ip)>0)
        {
            ui().OnBattleAction(*this,wxString(_T(" ")),user.BattleStatus().ip+_T(" is banned, kicking"));
            KickPlayer(user);
            return true;
        }
    }
    return false;
}
///>

void Battle::SetAutoLockOnStart( bool value )
{
    m_autolock_on_start = value;
}

bool Battle::GetAutoLockOnStart()
{
    return m_autolock_on_start;
}

void Battle::SetLockExternalBalanceChanges( bool value )
{
    if ( value ) DoAction( _T("has locked player balance changes") );
    else DoAction( _T("has unlocked player balance changes") );
    m_opts.lockexternalbalancechanges = value;
}

bool Battle::GetLockExternalBalanceChanges()
{
    return m_opts.lockexternalbalancechanges;
}


void Battle::AddBot( const wxString& nick, UserBattleStatus status )
{
    m_serv.AddBot( m_opts.battleid, nick, status );
}



void Battle::ForceSide( User& user, int side )
{
		m_serv.ForceSide( m_opts.battleid, user, side );
}


void Battle::ForceTeam( User& user, int team )
{
  IBattle::ForceTeam( user, team );
  m_serv.ForceTeam( m_opts.battleid, user, team );
}


void Battle::ForceAlly( User& user, int ally )
{
	IBattle::ForceAlly( user, ally );
  m_serv.ForceAlly( m_opts.battleid, user, ally );

}


void Battle::ForceColour( User& user, const wxColour& col )
{
		IBattle::ForceColour( user, col );
    m_serv.ForceColour( m_opts.battleid, user, col );
}


void Battle::ForceSpectator( User& user, bool spectator )
{
    m_serv.ForceSpectator( m_opts.battleid, user, spectator );
}


void Battle::KickPlayer( User& user )
{
    m_serv.BattleKickPlayer( m_opts.battleid, user );
}

void Battle::SetHandicap( User& user, int handicap)
{
    m_serv.SetHandicap ( m_opts.battleid, user, handicap );
}



void Battle::ForceUnsyncedToSpectate()
{
    size_t numusers = GetNumUsers();
    for ( size_t i = 0; i < numusers; ++i )
    {
        User &user = GetUser(i);
        UserBattleStatus& bs = user.BattleStatus();
        if ( bs.IsBot() ) continue;
        if ( !bs.spectator && !bs.sync ) ForceSpectator( user, true );
    }
}

void Battle::ForceUnReadyToSpectate()
{
    size_t numusers = GetNumUsers();
    for ( size_t i = 0; i < numusers; ++i )
    {
        User &user = GetUser(i);
        UserBattleStatus& bs = user.BattleStatus();
        if ( bs.IsBot() ) continue;
        if ( !bs.spectator && !bs.ready ) ForceSpectator( user, true );
    }
}

void Battle::ForceUnsyncedAndUnreadyToSpectate()
{
    size_t numusers = GetNumUsers();
    for ( size_t i = 0; i < numusers; ++i )
    {
        User &user = GetUser(i);
        UserBattleStatus& bs = user.BattleStatus();
        if ( bs.IsBot() ) continue;
				if ( !bs.spectator && ( !bs.sync || !bs.ready ) ) ForceSpectator( user, true );
    }
}


void Battle::UserPositionChanged( const User& user )
{
	  m_serv.SendUserPosition( user );
}


void Battle::SendScriptToClients()
{
	m_serv.SendScriptToClients( GetScript() );
}


void Battle::StartSpring()
{
	if ( UserExists( GetMe().GetNick() ) )
	{
		if ( IsProxy() )
		{
			wxString hostscript = spring().WriteScriptTxt( *this );
			try
			{
				wxString path = sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator() + _T("relayhost_script.txt");
				if ( !wxFile::Access( path, wxFile::write ) ) wxLogError( _T("Access denied to script.txt.") );

				wxFile f( path, wxFile::write );
				f.Write( hostscript );
				f.Close();

			} catch (...) {}
			m_serv.SendScriptToProxy( hostscript );
		}
		GetMe().BattleStatus().ready = false;
		SendMyBattleStatus();
		GetMe().Status().in_game = true;
		GetMe().SendMyUserStatus();
		if( IsFounderMe() && GetAutoLockOnStart() )
		{
			SetIsLocked( true );
			SendHostInfo( IBattle::HI_Locked );
		}
		ui().OnSpringStarting();
		spring().Run( *this );
	}
	ui().OnBattleStarted( *this );
}

void Battle::OnTimer( wxTimerEvent& event )
{
	if ( !IsFounderMe() ) return;
	if ( m_ingame ) return;
	int autospect_trigger_time = sett().GetBattleLastAutoSpectTime();
	if ( autospect_trigger_time == 0 ) return;
	time_t now = time(0);
	for ( unsigned int i = 0; i < GetNumUsers(); i++)
	{
		User& usr = GetUser( i );
		UserBattleStatus& status = usr.BattleStatus();
		if ( status.IsBot() || status.spectator ) continue;
		if ( status.sync && status.ready ) continue;
		if ( &usr == &GetMe() ) continue;
		std::map<wxString, time_t>::iterator itor = m_ready_up_map.find( usr.GetNick() );
		if ( itor != m_ready_up_map.end() )
		{
			if ( ( now - itor->second ) > autospect_trigger_time )
			{
				ForceSpectator( usr, true );
			}
		}
	}
}

void Battle::SetInGame( bool value )
{
	time_t now = time(0);
	if ( m_ingame && !value )
	{
		for ( int i = 0; i < GetNumUsers(); i++ )
		{
			User& user = GetUser( i );
			UserBattleStatus& status = user.BattleStatus();
			if ( status.IsBot() || status.spectator ) continue;
			if ( status.ready && status.sync ) continue;
			m_ready_up_map[user.GetNick()] = now;
		}
	}
	IBattle::SetInGame( value );
}
