/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: Battle
//
#include "battle.h"
#include "ui.h"
#include "iunitsync.h"
#include "server.h"
#include "user.h"
#include "utils.h"
#include "uiutils.h"
#include "settings.h"
#include "useractions.h"
#include "settings++/custom_dialogs.h"
#include "springunitsynclib.h"
#include "iconimagelist.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <wx/image.h>
#include <wx/string.h>
#include <wx/log.h>


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


void Battle::FixColours( )
{
    if ( !IsFounderMe() )return;
    std::vector<wxColour> &palette = GetFixColoursPalette();
    std::vector<int> palette_use( palette.size(), 0 );
    user_map_t::size_type max_fix_users = std::min( GetNumUsers(), palette.size() );

    wxColour my_col = GetMe().BattleStatus().colour; // Never changes color of founder (me) :-)
    int my_diff = 0;
    int my_col_i = GetClosestFixColour( my_col, palette_use,my_diff );
    palette_use[my_col_i]++;

    for ( user_map_t::size_type i = 0; i < max_fix_users; i++ )
    {
        if ( &GetUser( i ) == &GetMe() ) continue;
        User &user=GetUser(i);
        wxColour &user_col=user.BattleStatus().colour;
        int user_diff=0;
        int user_col_i=GetClosestFixColour(user_col,palette_use,user_diff);
        palette_use[user_col_i]++;
        if ( user_diff > 60 )
        {
            ForceColour( user, palette[user_col_i]);
            wxLogMessage(_T("Forcing colour on fix. diff=%d"),user_diff);

        }
    }
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


User& Battle::GetMe()
{
    return m_serv.GetMe();
}


User& Battle::OnUserAdded( User& user )
{
		user = IBattle::OnUserAdded( user );
    user.SetBattle( this );

    if ( IsFounderMe() )
    {
        if ( CheckBan( user ) ) return user;

        if ( ( !user.BattleStatus().IsBot() ) && ( m_opts.rankneeded > UserStatus::RANK_1 ) && ( user.GetStatus().rank < m_opts.rankneeded ))
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
    }
    // any code here may be skipped if the user was autokicked
    return user;
}

void Battle::OnUserBattleStatusUpdated( User &user, UserBattleStatus status )
{
		IBattle::OnUserBattleStatusUpdated( user, status );
    if ( status.handicap != 0 )
    {
        ui().OnBattleAction( *this, wxString(_T(" ")) , ( _T("Warning: user ") + user.GetNick() + _T(" got bonus ") ) << status.handicap );
    }
    if ( IsFounderMe() )
    {
        if ( ( m_opts.rankneeded > UserStatus::RANK_1 ) && ( user.GetStatus().rank < m_opts.rankneeded ))
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
                break;
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
        if ( u.BattleStatus().IsBot() ) continue;
        UserBattleStatus& bs = u.BattleStatus();
        if ( !bs.ready && !bs.spectator ) m_serv.Ring( u.GetNick() );
    }
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

class PlayerRankCompareFunction
{
	public:
		PlayerRankCompareFunction( const wxString& modshortname ): m_mod_shortname( modshortname ) {}
		bool operator()( User *a, User *b ) // should never operate on nulls. Hence, ASSERT_LOGIC is appropriate here.
		{
				ASSERT_LOGIC( a, _T("fail in Autobalance, NULL player") );
				ASSERT_LOGIC( b, _T("fail in Autobalance, NULL player") );
				return ( a->GetBalanceRank(m_mod_shortname) > b->GetBalanceRank(m_mod_shortname) );
		}
	private:
		wxString m_mod_shortname;

};

bool PlayerTeamCompareFunction( User *a, User *b ) // should never operate on nulls. Hence, ASSERT_LOGIC is appropriate here.
{
    ASSERT_LOGIC( a, _T("fail in Autobalance, NULL player") );
    ASSERT_LOGIC( b, _T("fail in Autobalance, NULL player") );
    return ( a->BattleStatus().team > b->BattleStatus().team );
}

struct Alliance
{
    std::vector<User *>players;
    float ranksum;
    int allynum;
    wxString shortname;
    Alliance(): ranksum(0) {}
    Alliance(int i, const wxString& modshortname): ranksum(0), allynum(i), shortname( modshortname ) {}
    void AddPlayer( User *player )
    {
        if ( player )
        {
            players.push_back( player );
            ranksum += player->GetBalanceRank(shortname);
        }
    }
    void AddAlliance( Alliance &other )
    {
        for ( std::vector<User *>::iterator i = other.players.begin(); i != other.players.end(); ++i ) AddPlayer( *i );
    }
    bool operator < ( const Alliance &other ) const
    {
        return ranksum < other.ranksum;
    }
};

struct ControlTeam
{
    std::vector<User*> players;
    float ranksum;
    int teamnum;
    wxString shortname;
    ControlTeam(): ranksum(0) {}
    ControlTeam( int i, const wxString& modshortname ): ranksum(0), teamnum(i), shortname( modshortname ) {}
    void AddPlayer( User *player )
    {
        if ( player )
        {
            players.push_back( player );
            ranksum += player->GetBalanceRank(shortname);
        }
    }
    void AddTeam( ControlTeam &other )
    {
        for ( std::vector<User*>::iterator i = other.players.begin(); i != other.players.end(); ++i ) AddPlayer( *i );
    }
    bool operator < (const ControlTeam &other) const
    {
        return ranksum < other.ranksum;
    }
};

int my_random( int range )
{
    return rand() % range;
}

void shuffle(std::vector<User *> &players) // proper shuffle.
{
    for ( size_t i=0; i < players.size(); ++i ) // the players below i are shuffled, the players above arent
    {
        int rn = i + my_random( players.size() - i ); // the top of shuffled part becomes random card from unshuffled part
        User *tmp = players[i];
        players[i] = players[rn];
        players[rn] = tmp;
    }
}

/*
bool ClanRemovalFunction(const std::map<wxString, Alliance>::value_type &v){
  return v.second.players.size()<2;
}
*/
/*
struct ClannersRemovalPredicate{
  std::map<wxString, Alliance> &clans;
  PlayerRemovalPredicate(std::map<wxString, Alliance> &clans_):clans(clans_)
  {
  }
  bool operator()(User *u) const{
    return clans.find(u->GetClan());
  }
}*/

void Battle::Autobalance( BalanceType balance_type, bool support_clans, bool strong_clans, int numallyteams )
{
    wxLogMessage(_T("Autobalancing alliances, type=%d, clans=%d, strong_clans=%d, numallyteams=%d"),balance_type, support_clans,strong_clans, numallyteams);
    DoAction(_T("is auto-balancing alliances ..."));
    //size_t i;
    //int num_alliances;
    CLAMP( numallyteams, 0, 16 ); // 16 max ally teams currently supported by spring
    std::vector<Alliance>alliances;
    if ( numallyteams == 0 ) // 0 == use num start rects
    {
//        int tmp = GetNumRects();
        int ally = 0;
        for ( int i = 0; i < numallyteams; ++i )
        {
            BattleStartRect sr = m_rects[i];
            if ( sr.IsOk() )
            {
                ally=i;
                alliances.push_back( Alliance( ally, m_local_mod.shortname ) );
                ally++;
            }
        }
        // make at least two alliances
        while ( alliances.size() < 2 )
        {
            alliances.push_back( Alliance( ally, m_local_mod.shortname ) );
            ally++;
        }
    }
    else
    {
        for ( int i = 0; i < numallyteams; i++ ) alliances.push_back( Alliance( i, m_local_mod.shortname ) );
    }

    //for(i=0;i<alliances.size();++i)alliances[i].allynum=i;

    wxLogMessage( _T("number of alliances: %u"), alliances.size() );

    std::vector<User*> players_sorted;
    players_sorted.reserve( GetNumUsers() );

    for ( size_t i = 0; i < GetNumUsers(); ++i )
    {
        User& usr = GetUser( i );
        if ( !usr.BattleStatus().spectator )
        {
            players_sorted.push_back( &usr );
        }
    }

    // remove players in the same team so only one remains
    std::map< int, User*> dedupe_teams;
    for ( std::vector<User*>::iterator it = players_sorted.begin(); it != players_sorted.end(); it++ )
    {
        dedupe_teams[(*it)->BattleStatus().team] = *it;
    }
    players_sorted.clear();
    players_sorted.reserve( dedupe_teams.size() );
    for ( std::map<int, User*>::iterator it = dedupe_teams.begin(); it != dedupe_teams.end(); it++ )
    {
        players_sorted.push_back( it->second );
    }

    shuffle( players_sorted );

    std::map<wxString, Alliance> clan_alliances;
    if ( support_clans )
    {
        for ( size_t i=0; i < players_sorted.size(); ++i )
        {
            wxString clan = players_sorted[i]->GetClan();
            if ( !clan.empty() )
            {
                clan_alliances[clan].AddPlayer( players_sorted[i] );
            }
        }
    };

    if ( balance_type != balance_random ) std::sort( players_sorted.begin(), players_sorted.end(), PlayerRankCompareFunction(m_local_mod.shortname) );

    if ( support_clans )
    {
        std::map<wxString, Alliance>::iterator clan_it = clan_alliances.begin();
        while ( clan_it != clan_alliances.end() )
        {
            Alliance &clan = (*clan_it).second;
            // if clan is too small (only 1 clan member in battle) or too big, dont count it as clan
            if ( ( clan.players.size() < 2 ) || ( !strong_clans && ( clan.players.size() > ( ( players_sorted.size() + alliances.size() -1 ) / alliances.size() ) ) ) )
            {
                wxLogMessage( _T("removing clan %s"), (*clan_it).first.c_str() );
                std::map<wxString, Alliance>::iterator next = clan_it;
                ++next;
                clan_alliances.erase( clan_it );
                clan_it = next;
                continue;
            }
            wxLogMessage( _T("Inserting clan %s"), (*clan_it).first.c_str() );
            std::sort( alliances.begin(), alliances.end() );
            float lowestrank = alliances[0].ranksum;
            int rnd_k = 1;// number of alliances with rank equal to lowestrank
            while ( size_t( rnd_k ) < alliances.size() )
            {
                if ( fabs( alliances[rnd_k].ranksum - lowestrank ) > 0.01 ) break;
                rnd_k++;
            }
            wxLogMessage( _T("number of lowestrank alliances with same rank=%d"), rnd_k );
            alliances[my_random( rnd_k )].AddAlliance( clan );
            ++clan_it;
        }
    }

    for ( size_t i = 0; i < players_sorted.size(); ++i )
    {
        // skip clanners, those have been added already.
        if ( clan_alliances.count( players_sorted[i]->GetClan() ) > 0 )
        {
            wxLogMessage( _T("clanner already added, nick=%s"), players_sorted[i]->GetNick().c_str() );
            continue;
        }

        // find alliances with lowest ranksum
        // insert current user into random one out of them
        // since performance doesnt matter here, i simply sort alliances,
        // then find how many alliances in beginning have lowest ranksum
        // note that balance player ranks range from 1 to 1.1 now
        // i.e. them are quasi equal
        // so we're essentially adding to alliance with smallest number of players,
        // the one with smallest ranksum.

        std::sort( alliances.begin(), alliances.end() );
        float lowestrank = alliances[0].ranksum;
        int rnd_k = 1;// number of alliances with rank equal to lowestrank
        while ( size_t( rnd_k ) < alliances.size() )
        {
            if ( fabs( alliances[rnd_k].ranksum - lowestrank ) > 0.01 ) break;
            rnd_k++;
        }
        wxLogMessage( _T("number of lowestrank alliances with same rank=%d"), rnd_k );
        alliances[my_random( rnd_k )].AddPlayer( players_sorted[i] );
    }

    UserList::user_map_t::size_type totalplayers = GetNumUsers();
    for ( size_t i = 0; i < alliances.size(); ++i )
    {
        for ( size_t j = 0; j < alliances[i].players.size(); ++j )
        {
            ASSERT_LOGIC( alliances[i].players[j], _T("fail in Autobalance, NULL player") );
            int balanceteam = alliances[i].players[j]->BattleStatus().team;
            wxLogMessage( _T("setting team %d to alliance %d"), balanceteam, i );
            for ( size_t h = 0; h < totalplayers; h++ ) // change ally num of all players in the team
            {
              User& usr = GetUser( h );
              if ( usr.BattleStatus().team == balanceteam ) ForceAlly( usr, alliances[i].allynum );
            }
        }
    }
}

void Battle::FixTeamIDs( BalanceType balance_type, bool support_clans, bool strong_clans, int numcontrolteams )
{
    wxLogMessage(_T("Autobalancing teams, type=%d, clans=%d, strong_clans=%d, numcontrolteams=%d"),balance_type, support_clans, strong_clans, numcontrolteams);
    //size_t i;
    //int num_alliances;
    std::vector<ControlTeam> teams;

    if ( numcontrolteams == 0 ) numcontrolteams = GetNumUsers(); // 0 == use num players, will use comshare only if no available team slots
    IBattle::StartType position_type = (IBattle::StartType)s2l( CustomBattleOptions().getSingleValue( _T("startpostype"), OptionsWrapper::EngineOption ) );
    if ( ( position_type == ST_Fixed ) || ( position_type == ST_Random ) ) // if fixed start pos type or random, use max teams = start pos count
    {
      try
      {
        numcontrolteams = std::min( numcontrolteams, LoadMap().info.posCount );
      }
      catch( assert_exception ) {}
    }
    numcontrolteams = std::min( numcontrolteams, 16 ); // clamp to 16 (max spring supports)

    if ( numcontrolteams >= (int)( GetNumUsers() - GetSpectators() ) ) // autobalance behaves weird when trying to put one player per team and i CBA to fix it, so i'll reuse the old code :P
    {
      DoAction(_T("is making control teams unique..."));
      // apparently tasserver doesnt like when i fix/force ids of everyone.
      std::set<int> allteams;
      size_t numusers = GetNumUsers();
      for( size_t i = 0; i < numusers; ++i )
      {
        User &user = GetUser(i);
        if( !user.BattleStatus().spectator ) allteams.insert( user.BattleStatus().team );
      }
      std::set<int> teams;
      int t = 0;
      for( size_t i = 0; i < GetNumUsers(); ++i )
      {
        User &user = GetUser(i);
        if( !user.BattleStatus().spectator )
        {
          if( teams.count( user.BattleStatus().team ) )
          {
            while( allteams.count(t) || teams.count( t ) ) t++;
            ForceTeam( GetUser(i), t );
            teams.insert( t );
          }
          else
          {
            teams.insert( user.BattleStatus().team );
          }
        }
      }
      return;
    }
    DoAction(_T("is auto-balancing control teams ..."));
    for ( int i = 0; i < numcontrolteams; i++ ) teams.push_back( ControlTeam( i, m_local_mod.shortname ) );

    wxLogMessage(_T("number of teams: %u"), teams.size() );

    std::vector<User*> players_sorted;
    players_sorted.reserve( GetNumUsers() );

    int player_team_counter = 0;

    for ( size_t i = 0; i < GetNumUsers(); ++i ) // don't count spectators
    {
        if ( !GetUser(i).BattleStatus().spectator )
        {
            players_sorted.push_back( &GetUser(i) );
            // -- server fail? it doesnt work right.
            //ForceTeam(GetUser(i),player_team_counter);
            player_team_counter++;
        }
    }

    shuffle( players_sorted );

    std::map<wxString, ControlTeam> clan_teams;
    if ( support_clans )
    {
        for ( size_t i = 0; i < players_sorted.size(); ++i )
        {
            wxString clan = players_sorted[i]->GetClan();
            if ( !clan.empty() )
            {
                clan_teams[clan].AddPlayer( players_sorted[i] );
            }
        }
    };

    if ( balance_type != balance_random ) std::sort( players_sorted.begin(), players_sorted.end(), PlayerRankCompareFunction(m_local_mod.shortname) );

    if ( support_clans )
    {
        std::map<wxString, ControlTeam>::iterator clan_it = clan_teams.begin();
        while ( clan_it != clan_teams.end() )
        {
            ControlTeam &clan = (*clan_it).second;
            // if clan is too small (only 1 clan member in battle) or too big, dont count it as clan
            if ( ( clan.players.size() < 2 ) || ( !strong_clans && ( clan.players.size() >  ( ( players_sorted.size() + teams.size() -1 ) / teams.size() ) ) ) )
            {
                wxLogMessage(_T("removing clan %s"),(*clan_it).first.c_str());
                std::map<wxString, ControlTeam>::iterator next = clan_it;
                ++next;
                clan_teams.erase( clan_it );
                clan_it = next;
                continue;
            }
            wxLogMessage( _T("Inserting clan %s"), (*clan_it).first.c_str() );
            std::sort( teams.begin(), teams.end() );
            float lowestrank = teams[0].ranksum;
            int rnd_k = 1; // number of alliances with rank equal to lowestrank
            while ( size_t( rnd_k ) < teams.size() )
            {
                if ( fabs( teams[rnd_k].ranksum - lowestrank ) > 0.01 ) break;
                rnd_k++;
            }
            wxLogMessage(_T("number of lowestrank teams with same rank=%d"), rnd_k );
            teams[my_random( rnd_k )].AddTeam( clan );
            ++clan_it;
        }
    }

    for (size_t i = 0; i < players_sorted.size(); ++i )
    {
        // skip clanners, those have been added already.
        if ( clan_teams.count( players_sorted[i]->GetClan() ) > 0 )
        {
            wxLogMessage( _T("clanner already added, nick=%s"),players_sorted[i]->GetNick().c_str() );
            continue;
        }

        // find teams with lowest ranksum
        // insert current user into random one out of them
        // since performance doesnt matter here, i simply sort teams,
        // then find how many teams in beginning have lowest ranksum
        // note that balance player ranks range from 1 to 1.1 now
        // i.e. them are quasi equal
        // so we're essentially adding to teams with smallest number of players,
        // the one with smallest ranksum.

        std::sort( teams.begin(), teams.end() );
        float lowestrank = teams[0].ranksum;
        int rnd_k = 1; // number of alliances with rank equal to lowestrank
        while ( size_t( rnd_k ) < teams.size() )
        {
            if ( fabs ( teams[rnd_k].ranksum - lowestrank ) > 0.01 ) break;
            rnd_k++;
        }
        wxLogMessage( _T("number of lowestrank teams with same rank=%d"), rnd_k );
        teams[my_random( rnd_k )].AddPlayer( players_sorted[i] );
    }


    for ( size_t i=0; i < teams.size(); ++i )
    {
        for ( size_t j = 0; j < teams[i].players.size(); ++j )
        {
            ASSERT_LOGIC( teams[i].players[j], _T("fail in Autobalance teams, NULL player") );
            wxString msg = wxString::Format( _T("setting player %s to team and ally %d"), teams[i].players[j]->GetNick().c_str(), i );
            wxLogMessage( _T("%s"), msg.c_str() );
            ForceTeam( *teams[i].players[j], teams[i].teamnum );
            ForceAlly( *teams[i].players[j], teams[i].teamnum );
        }
    }
}


void Battle::ForceUnsyncedToSpectate()
{
    size_t numusers = GetNumUsers();
    for ( size_t i = 0; i < numusers; ++i )
    {
        User &user = GetUser(i);
        if ( !user.BattleStatus().spectator && !user.BattleStatus().sync ) ForceSpectator( user, true );
    }
}


void Battle::UserPositionChanged( const User& user )
{
	  m_serv.SendUserPosition( user );
}
