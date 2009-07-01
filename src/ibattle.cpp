/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/tokenzr.h>
#include <wx/image.h>
#include <sstream>

#include "ibattle.h"
#include "utils.h"
#include "uiutils.h"
#include "settings.h"
#include "ui.h"
#include "springunitsynclib.h"
#include "images/fixcolours_palette.xpm"

#include <list>
#include <algorithm>
#include <cmath>
#include <set>

IBattle::IBattle():
  m_map_loaded(false),
  m_mod_loaded(false),
  m_map_exists(false),
  m_mod_exists(false),
  m_ingame(false),
  m_generating_script(false),
  m_is_self_in(false)

{
}


IBattle::~IBattle()
{
}

bool IBattle::IsSynced()
{
    LoadMod();
    LoadMap();
    bool synced = true;
    if ( !m_host_map.hash.IsEmpty() ) synced = synced && (m_local_map.hash == m_host_map.hash);
    if ( !m_host_map.name.IsEmpty() ) synced = synced && (m_local_map.name == m_host_map.name);
    if ( !m_host_mod.hash.IsEmpty() ) synced = synced && (m_local_mod.hash == m_host_mod.hash);
    if ( !m_host_mod.name.IsEmpty() ) synced = synced && (m_local_mod.name == m_host_mod.name);
    return synced;
}





std::vector<wxColour> &IBattle::GetFixColoursPalette()
{
    static std::vector<wxColour> result;
    if (result.empty())
    {
        wxImage image(fixcolours_palette_xpm);
        unsigned char* data=image.GetData();
        size_t len=image.GetWidth()*image.GetHeight();
        for (size_t i=0;i<len;++i)
        {
            int r=data[i*3];
            int g=data[i*3+1];
            int b=data[i*3+2];
            if (r||g||b)
            {
                result.push_back(wxColour(r,g,b));
            }
        }
    }
    return result;
}

wxColour IBattle::GetFixColour(int i)
{
    std::vector<wxColour> palette = GetFixColoursPalette();
    if (((unsigned int)i)<palette.size())
    {
        return palette[i];
    }
    else
    {
        return wxColour(127,127,127);
    }
}

int IBattle::GetPlayerNum( const User& user )
{
    for (user_map_t::size_type i = 0; i < GetNumUsers(); i++)
    {
        if ( &GetUser(i) == &user ) return i;
    }
    ASSERT_EXCEPTION(false, _T("The player is not in this game.") );
    return -1;
}

wxColour IBattle::GetFreeColour( User &for_whom ) const
{
    int lowest = 0;
    bool changed = true;
    while ( changed )
    {
        changed = false;
        for ( user_map_t::size_type i = 0; i < GetNumUsers(); i++ )
        {
            if ( &GetUser( i ) == &for_whom ) continue;
            UserBattleStatus& bs = GetUser( i ).BattleStatus();
            if ( bs.spectator ) continue;
            if ( AreColoursSimilar( bs.colour, wxColour(colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2]) ) )
            {
                lowest++;
                changed = true;
            }
        }
    }
    return wxColour( colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2] );
}


wxColour IBattle::GetNewColour() const
{
    int lowest = 0;
    bool changed = true;
    while ( changed )
    {
        changed = false;
        for ( user_map_t::size_type i = 0; i < GetNumUsers(); i++ )
        {
            UserBattleStatus& bs = GetUser( i ).BattleStatus();
            if ( bs.spectator ) continue;
            if ( AreColoursSimilar( bs.colour, wxColour(colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2]) ) )
            {
                lowest++;
                changed = true;
            }
        }
    }
    return wxColour( colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2] );
}

int IBattle::ColourDifference(const wxColour &a, const wxColour &b) // returns max difference of r,g,b.
{
    return std::max(abs(a.Red()-b.Red()),std::max(abs(a.Green()-b.Green()),abs(a.Blue()-b.Blue())));

}

int IBattle::GetFreeTeamNum( bool excludeme )
{
    int lowest = 0;
    bool changed = true;
    while ( changed )
    {
        changed = false;
        for ( user_map_t::size_type i = 0; i < GetNumUsers(); i++ )
        {
            if ( ( &GetUser( i ) == &GetMe() ) && excludeme ) continue;
            //if ( GetUser( i ).BattleStatus().spectator ) continue;
            if ( GetUser( i ).BattleStatus().team == lowest )
            {
                lowest++;
                changed = true;
            }
        }
    }
    return lowest;
}

int IBattle::GetClosestFixColour(const wxColour &col, const std::vector<int> &excludes, int &difference)
{
    std::vector<wxColour> palette = GetFixColoursPalette();
    int mindiff=1024;
    int result=0;
    int t1=palette.size();
    int t2=excludes.size();
    wxLogMessage(_T("GetClosestFixColour %d %d"),t1,t2);
    for (size_t i=0;i<palette.size();++i)
    {
        if ((i>=excludes.size()) || (!excludes[i]))
        {
            int diff=ColourDifference(palette[i],col);
            if (diff<mindiff)
            {
                mindiff=diff;
                result=i;
            }
        }
    }
    difference=mindiff;
    wxLogMessage(_T("GetClosestFixColour result=%d diff=%d"),result,difference);
    return result;
}


void IBattle::SendHostInfo( HostInfo update )
{
}

void IBattle::SendHostInfo( const wxString& Tag )
{
}

void IBattle::Update ( const wxString& Tag )
{
}

User& IBattle::OnUserAdded( User& user )
{
    UserList::AddUser( user );
		UserBattleStatus& bs = user.BattleStatus();
    bs.spectator = false;
    bs.ready = false;
    bs.sync = SYNC_UNKNOWN;
    if ( !bs.IsBot() )
    {
			bs.team = GetFreeTeamNum( &user == &GetMe() );
			bs.ally = GetFreeAlly( &user == &GetMe() );
			bs.colour = GetFreeColour( user );
    }
    if ( IsFounderMe() && ( ( bs.pos.x < 0 ) || ( bs.pos.y < 0 ) ) )
    {
    	 UserPosition& pos = bs.pos;
    	 pos = GetFreePosition();
    	 UserPositionChanged( user );
    }
		if ( !bs.spectator )
		{
			m_teams_sizes[bs.team] = m_teams_sizes[bs.team] + 1;
			m_ally_sizes[bs.ally] = m_ally_sizes[bs.ally] + 1;
		}
		if ( bs.ready && !bs.IsBot() ) m_players_ready++;
		if ( bs.sync && !bs.IsBot() ) m_players_sync++;
    return user;
}

User& IBattle::OnBotAdded( const wxString& nick, const UserBattleStatus& bs )
{
		m_internal_bot_list[nick] = User( nick );
		User& user = m_internal_bot_list[nick];
		user.UpdateBattleStatus( bs );
		User& usr = OnUserAdded( user );
		if ( GetMe().GetNick() == bs.owner )
		{
			 if ( bs.aitype >= 0 ) OptionsWrapper().loadAIOptions( GetHostModName(), bs.aitype, nick );
		}
		return usr;
}

unsigned int IBattle::GetNumBots() const
{
		return m_internal_bot_list.size();
}

unsigned int IBattle::GetNumPlayers() const
{
		return GetNumUsers() - GetNumBots();
}

void IBattle::OnUserBattleStatusUpdated( User &user, UserBattleStatus status )
{

    UserBattleStatus previousstatus = user.BattleStatus();

    user.UpdateBattleStatus( status );

    if ( IsFounderMe() )
    {
			if ( status.spectator != previousstatus.spectator )
			{
					if ( status.spectator )
					{
							m_opts.spectators++;
							if ( previousstatus.ready && !status.IsBot() ) m_players_ready--;
							if ( previousstatus.sync && !status.IsBot() ) m_players_sync--;
					}
					else
					{
							m_opts.spectators--;
							if ( status.ready && !status.IsBot() ) m_players_sync++;
							if ( status.sync && !status.IsBot() ) m_players_sync++;
					}
					SendHostInfo( HI_Spectators );
			}
			if ( m_opts.lockexternalbalancechanges )
			{
				if ( previousstatus.team != status.team ) ForceTeam( user, previousstatus.team );
				if ( previousstatus.ally != status.ally ) ForceAlly( user, previousstatus.ally );
			}
			else
			{
				if ( status.spectator != previousstatus.spectator )
				{
					if ( !status.spectator )
					{
						m_teams_sizes[status.team] = m_teams_sizes[status.team] + 1;
						m_ally_sizes[status.ally] = m_ally_sizes[status.ally] + 1;
					}
					else
					{
						m_teams_sizes[status.team] = m_teams_sizes[status.team] - 1;
						m_ally_sizes[status.ally] = m_ally_sizes[status.ally] - 1;
					}
				}
				else
				{
					m_teams_sizes[previousstatus.team] = m_teams_sizes[previousstatus.team] - 1;
					m_teams_sizes[status.team] = m_teams_sizes[status.team] + 1;
					m_ally_sizes[previousstatus.ally] = m_ally_sizes[previousstatus.ally] - 1;
					m_ally_sizes[status.ally] = m_ally_sizes[status.ally] + 1;
				}
			}
	}
	if ( !status.IsBot() )
	{
		if ( ( previousstatus.ready != status.ready ) && !status.spectator && !previousstatus.spectator )
		{
			 if ( status.ready ) m_players_ready++;
			 else m_players_ready--;
		}
		if ( ( previousstatus.sync != status.sync ) && !status.spectator && !previousstatus.spectator )
		{
			 if ( status.sync ) m_players_sync++;
			 else m_players_sync--;
		}
	}

	if ( m_autocontrol_balance )
	{
		if ( ShouldAutoStart() )
		{
			FixTeamIDs( (IBattle::BalanceType)sett().GetFixIDMethod(), sett().GetFixIDClans(), sett().GetFixIDStrongClans(), sett().GetFixIDGrouping() );
			Autobalance( (IBattle::BalanceType)sett().GetBalanceMethod(), sett().GetBalanceClans(), sett().GetBalanceStrongClans(), sett().GetBalanceGrouping() );
			FixColours();
		}
	}
}

bool IBattle::ShouldAutoStart()
{
	return ( GetNumPlayers() - m_opts.spectators - m_players_ready < 1 ) && ( GetNumPlayers() - m_opts.spectators - m_players_sync < 1 );
}

void IBattle::OnUserRemoved( User& user )
{
		UserBattleStatus& bs = user.BattleStatus();
		if ( !bs.spectator )
		{
			m_teams_sizes[bs.team] = m_teams_sizes[bs.team] - 1;
			m_ally_sizes[bs.ally] = m_ally_sizes[bs.ally] - 1;
		}
		if ( bs.ready && !bs.IsBot() ) m_players_ready--;
		if ( bs.sync && !bs.IsBot() ) m_players_sync--;
    if ( IsFounderMe() && bs.spectator )
    {
      m_opts.spectators--;
      SendHostInfo( HI_Spectators );
    }
    if ( &user == &GetMe() ) OnSelfLeftBattle();
    UserList::RemoveUser( user.GetNick() );
    if ( !bs.IsBot() ) user.SetBattle( 0 );
    else
    {
    	UserVecIter itor = m_internal_bot_list.find( user.GetNick() );
			if ( itor != m_internal_bot_list.end() )
			{
    			m_internal_bot_list.erase( itor );
			}
    }
}


void IBattle::FixColours()
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


bool PlayerRankCompareFunction( User *a, User *b ) // should never operate on nulls. Hence, ASSERT_LOGIC is appropriate here.
{
    ASSERT_LOGIC( a, _T("fail in Autobalance, NULL player") );
    ASSERT_LOGIC( b, _T("fail in Autobalance, NULL player") );
    return ( a->GetBalanceRank() > b->GetBalanceRank() );
}

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
    Alliance(): ranksum(0) {}
    Alliance(int i): ranksum(0), allynum(i) {}
    void AddPlayer( User *player )
    {
        if ( player )
        {
            players.push_back( player );
            ranksum += player->GetBalanceRank();
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
    ControlTeam(): ranksum(0) {}
    ControlTeam( int i ): ranksum(0), teamnum(i) {}
    void AddPlayer( User *player )
    {
        if ( player )
        {
            players.push_back( player );
            ranksum += player->GetBalanceRank();
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

void IBattle::Autobalance( BalanceType balance_type, bool support_clans, bool strong_clans, int numallyteams )
{
    wxLogMessage(_T("Autobalancing alliances, type=%d, clans=%d, strong_clans=%d, numallyteams=%d"),balance_type, support_clans,strong_clans, numallyteams);
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
            BattleStartRect sr = GetStartRect(i);
            if ( sr.IsOk() )
            {
                ally=i;
                alliances.push_back( Alliance( ally ) );
                ally++;
            }
        }
        // make at least two alliances
        while ( alliances.size() < 2 )
        {
            alliances.push_back( Alliance( ally ) );
            ally++;
        }
    }
    else
    {
        for ( int i = 0; i < numallyteams; i++ ) alliances.push_back( Alliance( i ) );
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

    if ( balance_type != balance_random ) std::sort( players_sorted.begin(), players_sorted.end(), PlayerRankCompareFunction );

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

void IBattle::FixTeamIDs( BalanceType balance_type, bool support_clans, bool strong_clans, int numcontrolteams )
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
    for ( int i = 0; i < numcontrolteams; i++ ) teams.push_back( ControlTeam( i ) );

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

    if ( balance_type != balance_random ) std::sort( players_sorted.begin(), players_sorted.end(), PlayerRankCompareFunction );

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

bool IBattle::IsEveryoneReady()
{
    for (user_map_t::size_type i = 0; i < GetNumUsers(); i++)
    {
				User& usr = GetUser(i);
				if ( usr.BattleStatus().IsBot() ) continue;
        UserBattleStatus& bs = usr.BattleStatus();
        if ( !bs.ready && !bs.spectator ) return false;
    }
    return true;
}



void IBattle::AddStartRect( unsigned int allyno, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom )
{
    BattleStartRect sr;

    sr.ally = allyno;
    sr.left = left;
    sr.top = top;
    sr.right = right;
    sr.bottom = bottom;
    sr.toadd = true;
    sr.todelete = false;
    sr.toresize = false;
    sr.exist = true;

    m_rects[allyno] = sr;
}



void IBattle::RemoveStartRect( unsigned int allyno )
{
    std::map<unsigned int,BattleStartRect>::iterator rect_it = m_rects.find(allyno);
	if( rect_it == m_rects.end() )
		return;

	rect_it->second.todelete = true;
    //BattleStartRect sr = m_rects[allyno];
    //sr.todelete = true;
    //m_rects[allyno] = sr;
}


void IBattle::ResizeStartRect( unsigned int allyno )
{
    std::map<unsigned int,BattleStartRect>::iterator rect_it = m_rects.find(allyno);
	if( rect_it == m_rects.end() )
		return;

	rect_it->second.toresize = true;
    //BattleStartRect sr = m_rects[allyno];
    //&&sr.toresize = true;
    //m_rects[allyno] = sr;
}


void IBattle::StartRectRemoved( unsigned int allyno )
{
	std::map<unsigned int,BattleStartRect>::iterator rect_it = m_rects.find(allyno);
	if( rect_it == m_rects.end() )
		return;

	if ( rect_it->second.todelete ) m_rects.erase(allyno);
}


void IBattle::StartRectResized( unsigned int allyno )
{
    std::map<unsigned int,BattleStartRect>::iterator rect_it = m_rects.find(allyno);
	if( rect_it == m_rects.end() )
		return;

	rect_it->second.toresize = false;
    //BattleStartRect sr = m_rects[allyno];
    //sr.toresize = false;
    //m_rects[allyno] = sr;
}


void IBattle::StartRectAdded( unsigned int allyno )
{
    std::map<unsigned int,BattleStartRect>::iterator rect_it = m_rects.find(allyno);
	if( rect_it == m_rects.end() )
		return;

	rect_it->second.toadd = false;
    //BattleStartRect sr = m_rects[allyno];
    //sr.toadd = false;
    //m_rects[allyno] = sr;
}


BattleStartRect IBattle::GetStartRect( unsigned int allyno )
{
	std::map<unsigned int,BattleStartRect>::iterator rect_it = m_rects.find(allyno);
	if( rect_it != m_rects.end() )
		return (*rect_it).second;
	return BattleStartRect();
}

//total number of start rects
unsigned int IBattle::GetNumRects()
{
    return m_rects.size();
}

//key of last start rect in the map
unsigned int IBattle::GetLastRectIdx()
{
	if(GetNumRects() > 0)
		return m_rects.rbegin()->first;

	return 0;

}

//return  the lowest currently unused key in the map of rects.
unsigned int IBattle::GetNextFreeRectIdx()
{
	//check for unused allyno keys
	for(unsigned int i = 0; i <= GetLastRectIdx(); i++)
	{
		if(!GetStartRect(i).IsOk())
			return i;
	}
	return GetNumRects(); //if all rects are in use, or no elements exist, return first possible available allyno.
}

void IBattle::ClearStartRects()
{
    m_rects.clear();
}

void IBattle::ForceSide( User& user, int side )
{
	if ( IsFounderMe() || user.BattleStatus().IsBot() )
	{
		 user.BattleStatus().side = side;
	}
}

void IBattle::ForceTeam( User& user, int team )
{
  if ( IsFounderMe() || user.BattleStatus().IsBot() )
  {
    user.BattleStatus().team = team;
  }
}


void IBattle::ForceAlly( User& user, int ally )
{

  if ( IsFounderMe() || user.BattleStatus().IsBot() )
  {
    user.BattleStatus().ally = ally;
  }

}


void IBattle::ForceColour( User& user, const wxColour& col )
{
  if ( IsFounderMe() || user.BattleStatus().IsBot() )
		{
			 user.BattleStatus().colour = col;
		}

}


void IBattle::ForceSpectator( User& user, bool spectator )
{
		if ( IsFounderMe() || user.BattleStatus().IsBot() )
		{
			 user.BattleStatus().spectator = spectator;
		}
}

void IBattle::SetHandicap( User& user, int handicap)
{
		if ( IsFounderMe() || user.BattleStatus().IsBot() )
		{
			 user.BattleStatus().handicap = handicap;
		}
}


void IBattle::KickPlayer( User& user )
{
		if ( IsFounderMe() || user.BattleStatus().IsBot() )
		{
			 OnUserRemoved( user );
		}
}

int IBattle::GetFreeAlly( bool excludeme )
{
  int lowest = 0;
  bool changed = true;
  while ( changed )
   {
    changed = false;
    for ( unsigned int i = 0; i < GetNumUsers(); i++ )
    {
      User& user = GetUser( i );
      if ( ( &GetUser( i ) == &GetMe() ) && excludeme ) continue;
      if ( user.BattleStatus().ally == lowest )
      {
        lowest++;
        changed = true;
      }
    }
  }
  return lowest;
}

UserPosition IBattle::GetFreePosition()
{
	UserPosition ret;
  UnitSyncMap map = LoadMap();
  for ( int i = 0; i < map.info.posCount; i++ )
	{
    bool taken = false;
    for ( unsigned int bi = 0; bi < GetNumUsers(); bi++ )
    {
      User& user = GetUser( bi );
      UserBattleStatus& status = user.BattleStatus();
      if ( status.spectator ) continue;
      if ( ( map.info.positions[i].x == status.pos.x ) && ( map.info.positions[i].y == status.pos.y ) )
      {
        taken = true;
        break;
      }
    }
    if ( !taken )
    {
      ret.x = CLAMP(map.info.positions[i].x, 0, map.info.width);
      ret.y = CLAMP(map.info.positions[i].y, 0, map.info.height);
      return ret;
    }
  }
  ret.x = map.info.width / 2;
  ret.y = map.info.height / 2;
  return ret;
}


void IBattle::SetHostMap(const wxString& mapname, const wxString& hash)
{
  if ( mapname != m_host_map.name || hash != m_host_map.hash )
  {
    m_map_loaded = false;
    m_host_map.name = mapname;
    m_host_map.hash = hash;
    if ( !m_host_map.hash.IsEmpty() ) m_map_exists = usync().MapExists( m_host_map.name, m_host_map.hash );
    else m_map_exists = usync().MapExists( m_host_map.name );
    #ifndef __WXMSW__
		if ( m_map_exists && !ui().IsSpringRunning() ) usync().PrefetchMap( m_host_map.name );
		#endif
  }
}


void IBattle::SetLocalMap(const UnitSyncMap& map)
{
  if ( map.name != m_local_map.name || map.hash != m_local_map.hash ) {
    m_local_map = map;
    m_map_loaded = true;
    if ( !m_host_map.hash.IsEmpty() ) m_map_exists = usync().MapExists( m_host_map.name, m_host_map.hash );
    else m_map_exists = usync().MapExists( m_host_map.name );
    #ifndef __WXMSW__
    if ( m_map_exists && !ui().IsSpringRunning() ) usync().PrefetchMap( m_host_map.name );
    #endif
    if ( IsFounderMe() ) // save all rects infos
    {

    }
  }
}


const UnitSyncMap& IBattle::LoadMap()
{

  if ( !m_map_loaded ) {
    try {
      ASSERT_EXCEPTION( m_map_exists, _T("Map does not exist.") );
      m_local_map = usync().GetMapEx( m_host_map.name );
      m_map_loaded = true;

    } catch (...) {}
  }
  return m_local_map;
}


wxString IBattle::GetHostMapName() const
{
  return m_host_map.name;
}


wxString IBattle::GetHostMapHash() const
{
  return m_host_map.hash;
}


void IBattle::SetHostMod( const wxString& modname, const wxString& hash )
{
  if ( m_host_mod.name != modname || m_host_mod.hash != hash )
  {
    m_mod_loaded = false;
    m_host_mod.name = modname;
    m_host_mod.hash = hash;
    if ( !m_host_mod.hash.IsEmpty() ) m_mod_exists = usync().ModExists( m_host_mod.name, m_host_mod.hash );
    else m_mod_exists = usync().ModExists( m_host_mod.name );
  }
}


void IBattle::SetLocalMod( const UnitSyncMod& mod )
{
  if ( mod.name != m_local_mod.name || mod.hash != m_local_mod.hash )
  {
    m_local_mod = mod;
    m_mod_loaded = true;
    if ( !m_host_mod.hash.IsEmpty() ) m_mod_exists = usync().ModExists( m_host_mod.name, m_host_mod.hash );
    else m_mod_exists = usync().ModExists( m_host_mod.name );
  }
}


const UnitSyncMod& IBattle::LoadMod()
{
  if ( !m_mod_loaded )
   {
    try {
      ASSERT_EXCEPTION( m_mod_exists, _T("Mod does not exist.") );
      m_local_mod = usync().GetMod( m_host_mod.name );
      m_mod_loaded = true;
    } catch (...) {}
  }
  return m_local_mod;
}


wxString IBattle::GetHostModName() const
{
  return m_host_mod.name;
}


wxString IBattle::GetHostModHash() const
{
  return m_host_mod.hash;
}


bool IBattle::MapExists() const
{
  return m_map_exists;
  //return usync().MapExists( m_map_name, m_map.hash );
}


bool IBattle::ModExists() const
{
  return m_mod_exists;
  //return usync().ModExists( m_mod_name );
}



void IBattle::RestrictUnit( const wxString& unitname, int count )
{
  m_restricted_units[ unitname ] = count;
}


void IBattle::UnrestrictUnit( const wxString& unitname )
{
  std::map<wxString,int>::iterator pos = m_restricted_units.find( unitname );
  if ( pos == m_restricted_units.end() ) return;
  m_restricted_units.erase( pos );
}


void IBattle::UnrestrictAllUnits()
{
  m_restricted_units.clear();
}


std::map<wxString,int> IBattle::RestrictedUnits()
{
  return m_restricted_units;
}

void IBattle::OnSelfLeftBattle()
{
    susynclib().UnSetCurrentMod(); //left battle
    m_is_self_in = false;
    ClearStartRects();
}

void IBattle::OnUnitSyncReloaded()
{
  if ( !m_host_mod.hash.IsEmpty() ) m_mod_exists = usync().ModExists( m_host_mod.name, m_host_mod.hash);
  else m_mod_exists = usync().ModExists( m_host_mod.name );
  if ( !m_host_map.hash.IsEmpty() )  m_map_exists = usync().MapExists( m_host_map.name, m_host_map.hash );
  else  m_map_exists = usync().MapExists( m_host_map.name );
}



static wxString FixPresetName( const wxString& name )
{
  // look name up case-insensitively
  const wxArrayString& presetList = sett().GetPresetList();
  int index = presetList.Index( name, false /*case insensitive*/ );
  if ( index == -1 ) return _T("");

  // set preset to the actual name, with correct case
  return presetList[index];
}


bool IBattle::LoadOptionsPreset( const wxString& name )
{
  wxString preset = FixPresetName(name);
  if (preset == _T("")) return false; //preset not found
  m_preset = preset;

  for ( unsigned int i = 0; i < OptionsWrapper::LastOption; i++)
  {
    std::map<wxString,wxString> options = sett().GetHostingPreset( m_preset, i );
    if ( (OptionsWrapper::GameOption)i != OptionsWrapper::PrivateOptions )
    {
      for ( std::map<wxString,wxString>::iterator itor = options.begin(); itor != options.end(); itor++ )
      {
            wxLogWarning( itor->first + _T(" ::: ") + itor->second );
            CustomBattleOptions().setSingleOption( itor->first, itor->second, (OptionsWrapper::GameOption)i );
      }
    }
    else
    {
      if ( !options[_T("mapname")].IsEmpty() )
      {
        if ( usync().MapExists( options[_T("mapname")] ) ) {
            UnitSyncMap map = usync().GetMapEx( options[_T("mapname")] );
            SetLocalMap( map );
            SendHostInfo( HI_Map );
        }
        else if ( !ui().OnPresetRequiringMap( options[_T("mapname")] ) ) {
            //user didn't want to download the missing map, so set to empty to not have it tried to be loaded again
            options[_T("mapname")] = _T("");
            sett().SetHostingPreset( m_preset, i, options );
        }
      }

			for( unsigned int i = 0; i <= GetLastRectIdx(); ++i ) if ( GetStartRect( i ).IsOk() ) RemoveStartRect(i); // remove all rects that might come from map presets
			SendHostInfo( IBattle::HI_StartRects );

      unsigned int rectcount = s2l( options[_T("numrects")] );
      for ( unsigned int loadrect = 0; loadrect < rectcount; loadrect++)
      {
        int ally = s2l(options[_T("rect_") + TowxString(loadrect) + _T("_ally")]);
        if ( ally == 0 ) continue;
        AddStartRect( ally - 1, s2l(options[_T("rect_") + TowxString(loadrect) + _T("_left")]), s2l(options[_T("rect_") + TowxString(loadrect) + _T("_top")]), s2l(options[_T("rect_") + TowxString(loadrect) + _T("_right")]), s2l(options[_T("rect_") + TowxString(loadrect) + _T("_bottom")]) );
      }
      SendHostInfo( HI_StartRects );

      wxStringTokenizer tkr( options[_T("restrictions")], _T('\t') );
      m_restricted_units.clear();
      while( tkr.HasMoreTokens() )
      {
      	wxString unitinfo = tkr.GetNextToken();
      	RestrictUnit( unitinfo.BeforeLast(_T('=')), s2l( unitinfo.AfterLast(_T('=')) ) );
      }
      SendHostInfo( HI_Restrictions );
      Update( wxString::Format( _T("%d_restrictions"), OptionsWrapper::PrivateOptions ) );

    }
  }
  SendHostInfo( HI_Send_All_opts );
  ui().ReloadPresetList();
  return true;
}


void IBattle::SaveOptionsPreset( const wxString& name )
{
  m_preset = FixPresetName(name);
  if (m_preset == _T("")) m_preset = name; //new preset

  for ( int i = 0; i < (int)OptionsWrapper::LastOption; i++)
  {
    if ( (OptionsWrapper::GameOption)i != OptionsWrapper::PrivateOptions )
    {
      sett().SetHostingPreset( m_preset, (OptionsWrapper::GameOption)i, CustomBattleOptions().getOptionsMap( (OptionsWrapper::GameOption)i ) );
    }
    else
    {
      std::map<wxString,wxString> opts;
      opts[_T("mapname")] = GetHostMapName();
      unsigned int validrectcount = 0;
      if ( s2l (CustomBattleOptions().getSingleValue( _T("startpostype"), OptionsWrapper::EngineOption ) ) == ST_Choose )
      {
        unsigned int boxcount = GetLastRectIdx();
        for ( unsigned int boxnum = 0; boxnum <= boxcount; boxnum++ )
        {
          BattleStartRect rect = GetStartRect( boxnum );
          if ( rect.IsOk() )
          {
            opts[_T("rect_") + TowxString(validrectcount) + _T("_ally")] = TowxString( rect.ally + 1 );
            opts[_T("rect_") + TowxString(validrectcount) + _T("_left")] = TowxString( rect.left );
            opts[_T("rect_") + TowxString(validrectcount) + _T("_top")] = TowxString( rect.top );
            opts[_T("rect_") + TowxString(validrectcount) + _T("_bottom")] = TowxString( rect.bottom );
            opts[_T("rect_") + TowxString(validrectcount) + _T("_right")] = TowxString( rect.right );
            validrectcount++;
          }
        }
      }
      opts[_T("numrects")] = TowxString( validrectcount );

      wxString restrictionsstring;
      for ( std::map<wxString, int>::iterator itor = m_restricted_units.begin(); itor != m_restricted_units.end(); itor++ )
      {
        restrictionsstring << itor->first << _T('=') << TowxString(itor->second) << _T('\t');
      }
      opts[_T("restrictions")] = restrictionsstring;

      sett().SetHostingPreset( m_preset, (OptionsWrapper::GameOption)i, opts );
    }
  }
  sett().SaveSettings();
  ui().ReloadPresetList();
}


wxString IBattle::GetCurrentPreset()
{
  return m_preset;
}


void IBattle::DeletePreset( const wxString& name )
{
  wxString preset = FixPresetName(name);
  if ( m_preset == preset ) m_preset = _T("");
  sett().DeletePreset( preset );
  ui().ReloadPresetList();
}

wxArrayString IBattle::GetPresetList()
{
  return sett().GetPresetList();
}

void IBattle::UserPositionChanged( const User& user )
{
}

void IBattle::AddUserFromDemo( User& user )
{
	user.BattleStatus().isfromdemo = true;
	m_internal_user_list[user.GetNick()] = user;
	UserList::AddUser( m_internal_user_list[user.GetNick()] );
}

void IBattle::SetIsProxy( bool value )
{
    m_opts.isproxy = value;
}

bool IBattle::IsProxy()
{
    return m_opts.isproxy;
}

bool IBattle::IsFounderMe()
{
    return ( ( m_opts.founder == GetMe().GetNick() ) || ( m_opts.isproxy  && !m_generating_script ) );
}

bool IBattle::IsFounder( const User& user ) const
{
    if ( UserExists( m_opts.founder ) ) {
        return &GetFounder() == &user;
    }
    else
        return false;
}

int IBattle::GetMyPlayerNum()
{
    return GetPlayerNum( GetMe() );
}


void IBattle::LoadScriptMMOpts( const wxString& sectionname, const PDataList& node )
{
		if ( !node.ok() ) return;
    PDataList section ( node->Find(sectionname) );
    if ( !section.ok() ) return;
    OptionsWrapper& opts = CustomBattleOptions();
    for ( PNode n = section->First(); n != section->Last(); n = section->Next( n ) )
    {
				if ( !n.ok() ) continue;
        opts.setSingleOption( n->Name(), section->GetString( n->Name() ) );
    }
}

void IBattle::LoadScriptMMOpts( const PDataList& node )
{
		if ( !node.ok() ) return;
    OptionsWrapper& opts = CustomBattleOptions();
    typedef std::map<wxString,wxString> optMap;
    optMap options = opts.getOptionsMap(OptionsWrapper::EngineOption);
    for ( optMap::const_iterator i = options.begin(); i != options.end(); ++i)
    {
        opts.setSingleOption( i->first, node->GetString( i->first, i->second ) );
    }
}

//! (koshi) don't delete commented things please, they might be need in the future and i'm lazy
void IBattle::GetBattleFromScript( bool loadmapmod )
{

    BattleOptions opts;
    std::stringstream ss ( (const char *)GetScript().mb_str(wxConvUTF8) );// no need to convert wxstring-->std::string-->std::stringstream, convert directly.
    PDataList script( ParseTDF(ss) );

    PDataList replayNode ( script->Find(_T("GAME") ) );
    if ( replayNode.ok() )
    {

        wxString modname = replayNode->GetString( _T("GameType") );
        wxString modhash = replayNode->GetString( _T("ModHash") );
        if ( !modhash.IsEmpty() ) modhash = MakeHashUnsigned( modhash );
        SetHostMod( modname, modhash );

        //don't have the maphash, what to do?
        //ui download function works with mapname if hash is empty, so works for now
        wxString mapname    = replayNode->GetString( _T("MapName") );
			  wxString maphash    = replayNode->GetString( _T("MapHash") );
        if ( !maphash.IsEmpty() ) maphash = MakeHashUnsigned( maphash );
        SetHostMap( mapname, maphash );

//        opts.ip         = replayNode->GetString( _T("HostIP") );
//        opts.port       = replayNode->GetInt  ( _T("HostPort"), DEFAULT_EXTERNAL_UDP_SOURCE_PORT );
        opts.spectators = 0;

        int playernum = replayNode->GetInt  ( _T("NumPlayers"), 0);
        int usersnum = replayNode->GetInt  ( _T("NumUsers"), 0);
        if ( usersnum > 0 ) playernum = usersnum;
//        int allynum = replayNode->GetInt  ( _T("NumAllyTeams"), 1);
//        int teamnum = replayNode->GetInt  ( _T("NumTeams"), 1);



        wxArrayString sides;
        if ( loadmapmod )
        {
        	sides = usync().GetSides( modname );
        }

				IBattle::TeamVec parsed_teams = GetParsedTeamsVec();
				IBattle::AllyVec parsed_allies = GetParsedAlliesVec();

        //[PLAYERX] sections
        for ( int i = 0; i < playernum ; ++i )
        {
            PDataList player ( replayNode->Find( _T("PLAYER") + i2s(i) ) );
            PDataList bot ( replayNode->Find( _T("AI") + i2s(i) ) );
            if ( player.ok() || bot.ok() )
            {
								if ( bot.ok() ) player = bot;
                User user ( player->GetString( _T("Name") ), (player->GetString( _T("CountryCode")).Upper() ), 0);
                user.BattleStatus().isfromdemo = true;
                user.BattleStatus().spectator = player->GetInt( _T("Spectator"), 0 );
                opts.spectators += user.BattleStatus().spectator;
                user.BattleStatus().team = player->GetInt( _T("Team") );
                user.BattleStatus().sync = true;
                user.BattleStatus().ready = true;
                //! (koshi) changed this from ServerRankContainer to RankContainer
                user.Status().rank = (UserStatus::RankContainer)player->GetInt( _T("Rank"), -1 );

                if ( bot.ok() )
                {
                	user.BattleStatus().aishortname = bot->GetString( _T("ShortName" ) );
                	user.BattleStatus().aiversion = bot->GetString( _T("Version" ) );
                	int ownerindex = bot->GetInt( _T("Host" ) );
                	PDataList aiowner ( replayNode->Find( _T("PLAYER") + i2s(ownerindex) ) );
                	if ( aiowner.ok() )
                	{
                		user.BattleStatus().owner = aiowner->GetString( _T("Name") );
                	}
                }

                IBattle::TeamInfoContainer teaminfos = parsed_teams[user.BattleStatus().team];
                if ( !teaminfos.exist )
                {
									PDataList team( replayNode->Find( _T("TEAM") + i2s( user.BattleStatus().team ) ) );
									if ( team.ok() )
									{
											teaminfos.exist = true;
											teaminfos.TeamLeader = team->GetInt( _T("TeamLeader"), 0 );
											teaminfos.StartPosX = team->GetInt( _T("StartPosX"), -1 );
											teaminfos.StartPosY = team->GetInt( _T("StartPosY"), -1 );
											teaminfos.TeamLeader = team->GetInt( _T("AllyTeam"), 0 );
											teaminfos.RGBColor = GetColorFromFloatStrng( team->GetString( _T("RGBColor") ) );
											teaminfos.SideName = team->GetString( _T("Side"), _T("") );
											teaminfos.Handicap = team->GetInt( _T("Handicap"), 0 );
											int sidepos = sides.Index( teaminfos.SideName );
											teaminfos.SideNum = sidepos;
											parsed_teams[ user.BattleStatus().team ] = teaminfos;
									}
                }
                if ( teaminfos.exist )
                {
										user.BattleStatus().ally = teaminfos.AllyTeam;
										user.BattleStatus().pos.x = teaminfos.StartPosX;
										user.BattleStatus().pos.y = teaminfos.StartPosY;
										user.BattleStatus().colour = teaminfos.RGBColor;
										user.BattleStatus().handicap = teaminfos.Handicap;
										if ( teaminfos.SideNum >= 0 ) user.BattleStatus().side = teaminfos.SideNum;
										IBattle::AllyInfoContainer allyinfos = parsed_allies[user.BattleStatus().ally];
										if ( !allyinfos.exist )
										{
												PDataList ally( replayNode->Find( _T("ALLYTEAM") + i2s( user.BattleStatus().ally ) ) );
												if ( ally.ok() )
												{
													allyinfos.exist = true;
													allyinfos.NumAllies = ally->GetInt( _T("NumAllies"), 0 );
													allyinfos.StartRectLeft = ally->GetInt( _T("StartRectLeft"), 0 );
													allyinfos.StartRectTop = ally->GetInt( _T("StartRectTop"), 0 );
													allyinfos.StartRectRight = ally->GetInt( _T("StartRectRight"), 0 );
													allyinfos.StartRectBottom = ally->GetInt( _T("StartRectBottom"), 0 );
													parsed_allies[ user.BattleStatus().ally ] = allyinfos;
													AddStartRect( user.BattleStatus().ally, allyinfos.StartRectTop, allyinfos.StartRectTop, allyinfos.StartRectRight, allyinfos.StartRectBottom );
												}
										}
                }

                AddUserFromDemo( user );
            }

        }
        SetParsedTeamsVec( parsed_teams );
        SetParsedAlliesVec( parsed_allies );

        //MMoptions, this'll fail unless loading map/mod into wrapper first
        if ( loadmapmod )
        {
					LoadScriptMMOpts( _T("mapoptions"), replayNode );
					LoadScriptMMOpts( _T("modoptions"), replayNode );
        }

        opts.maxplayers = playernum ;

    }
    SetBattleOptions( opts );
}

