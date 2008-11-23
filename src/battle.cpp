/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: Battle
//
#include <wx/log.h>
#include <stdexcept>

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

#include "iconimagelist.h"

#include <algorithm>
#include <math.h>

#include <wx/image.h>
#include <wx/string.h>

#include "images/fixcolours_palette.xpm"


std::vector<wxColour> &GetFixColoursPalette()
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

wxColour GetFixColour(int i)
{
    std::vector<wxColour> palette=GetFixColoursPalette();
    if (((unsigned int)i)<palette.size())
    {
        return palette[i];
    }
    else
    {
        return wxColour(127,127,127);
    }
}

const std::list<BattleBot*>::size_type BOT_SEEKPOS_INVALID = (std::list<BattleBot*>::size_type)(-1);


Battle::Battle( Server& serv, int id ) :
        CommonBattle(id,false,0), //m_ingame(false),m_order(0)
        m_serv(serv),
        m_ah(*this),
        m_autolock_on_start(false),
        m_generating_script(false)
{


}


Battle::~Battle()
{
    ClearStartRects();
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
}


void Battle::Leave()
{
    m_serv.LeaveBattle( m_opts.battleid );
}


int Battle::GetFreeTeamNum( bool excludeme ) const
{
    int lowest = 0;
    bool changed = true;
    while ( changed )
    {
        changed = false;
        for ( user_map_t::size_type i = 0; i < GetNumUsers(); i++ )
        {
            if ( (&GetUser( i ) == &GetMe()) && excludeme ) continue;
            //if ( GetUser( i ).BattleStatus().spectator ) continue;
            if ( GetUser( i ).BattleStatus().team == lowest )
            {
                lowest++;
                changed = true;
            }
        }
        std::list<BattleBot*>::const_iterator i;
        for ( i = m_bots.begin(); i != m_bots.end(); ++i )
        {
            if ( *i == 0 ) continue;
            if ( (*i)->bs.team == lowest )
            {
                lowest++;
                changed = true;
            }
        }
    }
    return lowest;
}


wxColour Battle::GetFreeColour( User *for_whom ) const
{
    int lowest = 0;
    bool changed = true;
    while ( changed )
    {
        changed = false;
        for ( user_map_t::size_type i = 0; i < GetNumUsers(); i++ )
        {
            if ( &GetUser( i ) == for_whom ) continue;
            //if ( GetUser( i ).BattleStatus().spectator ) continue;
            UserBattleStatus& bs = GetUser( i ).BattleStatus();
            if ( AreColoursSimilar( bs.colour, wxColour(colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2]) ) )
            {
                lowest++;
                changed = true;
            }
        }
        std::list<BattleBot*>::const_iterator i;
        for ( i = m_bots.begin(); i != m_bots.end(); ++i )
        {
            if ( *i == 0 ) continue;
            if ( AreColoursSimilar( (*i)->bs.colour, wxColour(colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2]) ) )
            {
                lowest++;
                changed = true;
            }
        }
    }
    return wxColour( colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2] );

}

int ColourDifference(const wxColour &a, const wxColour &b) // returns max difference of r,g,b.
{
    return std::max(abs(a.Red()-b.Red()),std::max(abs(a.Green()-b.Green()),abs(a.Blue()-b.Blue())));

}

int GetClosestFixColour(const wxColour &col, const std::vector<int> &excludes, int &difference)
{
    std::vector<wxColour> palette=GetFixColoursPalette();
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

void Battle::FixColours( )
{
    if (!IsFounderMe())return;
    std::vector<wxColour> &palette=GetFixColoursPalette();
    std::vector<int> palette_use(palette.size(),0);
    user_map_t::size_type max_fix_users=std::min( GetNumUsers(),palette.size());

    wxColour my_col=GetMe().BattleStatus().colour;/// Never changes color of founder (me) :-)
    int my_diff=0;
    int my_col_i=GetClosestFixColour(my_col,palette_use,my_diff);
    palette_use[my_col_i]++;

    for ( user_map_t::size_type i = 0; i < max_fix_users; i++ )
    {
        if ( &GetUser( i ) == &GetMe() ) continue;
        User &user=GetUser(i);
        wxColour &user_col=user.BattleStatus().colour;
        int user_diff=0;
        int user_col_i=GetClosestFixColour(user_col,palette_use,user_diff);
        palette_use[user_col_i]++;
        if (user_diff>32)
        {
            ForceColour( user, palette[user_col_i]);
            wxLogMessage(_T("Forcing colour on fix. diff=%d"),user_diff);

        }
    }
// TODO (dizekat#1#): Fix colors for bots aswell.
}


void Battle::OnRequestBattleStatus()
{
    int lowest = GetFreeTeamNum();

    UserBattleStatus& bs = m_serv.GetMe().BattleStatus();
    bs.team = lowest;
    bs.ally = lowest;
    bs.spectator = false;
    bs.colour = sett().GetBattleLastColour();
    /// theres some highly annoying bug with color changes on player join/leave.
    if ( !bs.colour.IsColourOk() ) bs.colour = GetFreeColour(&m_serv.GetMe());

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


bool Battle::IsSynced()
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


bool Battle::HaveMultipleBotsInSameTeam() const
{
    std::list<BattleBot*>::const_iterator i;
    wxLogDebugFunc(_T(""));

    std::vector<int> teams ( GetMaxPlayers(), -1 );
    for ( i = m_bots.begin(); i != m_bots.end(); ++i )
    {
        if ( *i == 0 ) continue;
        if ( teams[(*i)->bs.team ] == 1 )return true;
        teams[ (*i)->bs.team ] = 1;
    }
    return false;
}


User& Battle::GetMe() const
{
    return m_serv.GetMe();
}


bool Battle::IsFounderMe() const
{
    return ( ( m_opts.founder == m_serv.GetMe().GetNick() ) || ( m_opts.isproxy  && !m_generating_script ) );
}

int Battle::GetMyPlayerNum() const
{
    for (user_map_t::size_type i = 0; i < GetNumUsers(); i++)
    {
        if ( &GetUser(i) == &m_serv.GetMe() ) return i;
    }
    ASSERT_EXCEPTION(false, _T("You are not in this game.") );
    return -1;
}


void Battle::OnUserAdded( User& user )
{
    user.SetBattle( this );
    UserList::AddUser( user );

    //UserBattleStatus bs;
    //bs.spectator=true;
    //bs.order = m_order++;
    //user.UpdateBattleStatus( bs, true );
    user.BattleStatus().order=m_order++;
    user.BattleStatus().spectator=true;
    user.BattleStatus().ready=false;
    user.BattleStatus().sync=SYNC_UNKNOWN;


    if (IsFounderMe())
    {

        m_opts.spectators+=user.BattleStatus().spectator?1:0;

        if (CheckBan(user))
            return;

        if (m_opts.rankneeded>1 && user.GetStatus().rank<m_opts.rankneeded)
        {
            switch (m_opts.ranklimittype)
            {
            case rank_limit_none:
                break;
            case rank_limit_autospec:
                if (!user.BattleStatus().spectator)
                {
                    DoAction(_T("Rank limit autospec: ")+user.GetNick());
                    ForceSpectator(user,true);
                }
                break;
            case rank_limit_autokick:
                DoAction(_T("Rank limit autokick: ")+user.GetNick());
                BattleKickPlayer(user);
                return;
            }
        }

        m_ah.OnUserAdded(user);
    }
    // any code here may be skipped if the user was autokicked
}

void Battle::OnUserBattleStatusUpdated( User &user, UserBattleStatus status )
{

    bool previousspectatorstatus = user.BattleStatus().spectator;

    user.UpdateBattleStatus( status );

    if (status.handicap!=0)
    {
        ui().OnBattleAction(*this,wxString(_T(" ")),(_T("Warning: user ")+user.GetNick()+_T(" got bonus "))<< status.handicap);
    }

    if (IsFounderMe())
    {

        if ( status.spectator != previousspectatorstatus )
        {
            if ( status.spectator )
            {
                m_opts.spectators++;
                SendHostInfo(HI_Spectators);
            }
            else
            {
                m_opts.spectators--;
                SendHostInfo(HI_Spectators);
            }

        }

        if (m_opts.rankneeded>1 && user.GetStatus().rank<m_opts.rankneeded)
        {
            switch (m_opts.ranklimittype)
            {
            case rank_limit_none:
                break;
            case rank_limit_autospec:
                if (!user.BattleStatus().spectator)
                {
                    DoAction(_T("Rank limit autospec: ")+user.GetNick());
                    ForceSpectator(user,true);
                }
                break;
            case rank_limit_autokick:
                DoAction(_T("Rank limit autokick: ")+user.GetNick());
                BattleKickPlayer(user);
                break;
            }
        }

    }
}


void Battle::OnUserRemoved( User& user )
{
    if (IsFounderMe())
    {
        m_opts.spectators-=user.BattleStatus().spectator?1:0;
    }
    user.SetBattle( 0 );
    UserList::RemoveUser( user.GetNick() );
    m_ah.OnUserRemoved(user);
}


void Battle::KickPlayer( User& user )
{
    m_serv.BattleKickPlayer( m_opts.battleid, user.GetNick() );
}


bool Battle::IsEveryoneReady()
{
    for (user_map_t::size_type i = 0; i < GetNumUsers(); i++)
    {
        UserBattleStatus& bs = GetUser(i).BattleStatus();
        if ( !bs.ready && !bs.spectator ) return false;
    }
    return true;
}


void Battle::RingNotReadyPlayers()
{
    for (user_map_t::size_type i = 0; i < GetNumUsers(); i++)
    {
        User& u = GetUser(i);
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
    ///< quick hotfix for bans
    if (IsFounderMe())
    {
        if ( cmd_name == _T("/ban") )
        {
            wxString nick=cmd.AfterFirst(' ');
            m_banned_users.insert(nick);
            m_serv.BattleKickPlayer( m_opts.battleid, nick );
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
                m_serv.BattleKickPlayer( m_opts.battleid, nick );
            }
            //m_banned_ips.erase(nick);

            //m_serv.DoActionBattle( m_opts.battleid, cmd.AfterFirst(' ') );
            return true;
        }
        if ( cmd_name == _T("/replacehostip") )
        {
            wxString ip = cmd.AfterFirst(' ');
            if ( ip.IsEmpty() ) return false;
            m_opts.ip = ip;
            return true;
        }
    }
    ///>
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
            BattleKickPlayer(user);
            ui().OnBattleAction(*this,wxString(_T(" ")),user.GetNick()+_T(" is banned, kicking"));
            return true;
        }
        else if (m_banned_ips.count(user.BattleStatus().ip)>0)
        {
            ui().OnBattleAction(*this,wxString(_T(" ")),user.BattleStatus().ip+_T(" is banned, kicking"));
            BattleKickPlayer(user);
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

void Battle::SetIsProxy( bool value )
{
    m_opts.isproxy = value;
}

bool Battle::IsProxy()
{
    return m_opts.isproxy;
}


void CommonBattle::AddStartRect( unsigned int allyno, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom )
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


void CommonBattle::RemoveStartRect( unsigned int allyno )
{
    if ( allyno >= m_rects.size() ) return;
    BattleStartRect sr = m_rects[allyno];
    sr.todelete = true;
    m_rects[allyno] = sr;
}


void CommonBattle::ResizeStartRect( unsigned int allyno )
{
    if ( allyno >= m_rects.size() ) return;
    BattleStartRect sr = m_rects[allyno];
    sr.toresize = true;
    m_rects[allyno] = sr;
}


void CommonBattle::StartRectRemoved( unsigned int allyno )
{
    if ( allyno >= m_rects.size() ) return;
    if ( m_rects[allyno].todelete ) m_rects.erase(allyno);
}


void CommonBattle::StartRectResized( unsigned int allyno )
{
    if ( allyno >= m_rects.size() ) return;
    BattleStartRect sr = m_rects[allyno];
    sr.toresize = false;
    m_rects[allyno] = sr;
}


void CommonBattle::StartRectAdded( unsigned int allyno )
{
    if ( allyno >= m_rects.size() ) return;
    BattleStartRect sr = m_rects[allyno];
    sr.toadd = false;
    m_rects[allyno] = sr;
}


BattleStartRect CommonBattle::GetStartRect( unsigned int allyno )
{
    return m_rects[allyno];
}

void CommonBattle::ClearStartRects()
{
    m_rects.clear();
}


void Battle::AddBot( const wxString& nick, const wxString& owner, UserBattleStatus status, const wxString& aidll )
{
    m_serv.AddBot( m_opts.battleid, nick, owner, status, aidll );
}


void Battle::RemoveBot( const wxString& nick )
{
    m_serv.RemoveBot( m_opts.battleid, nick );
}


void Battle::SetBotTeam( const wxString& nick, int team )
{
    BattleBot* bot = GetBot( nick );
    try
    {
        ASSERT_LOGIC( bot != 0, _T("Bot not found") );
    }
    catch (...)
    {
        return;
    }
    bot->bs.team = team;
    m_serv.UpdateBot( m_opts.battleid, bot->name, bot->bs );
}


void Battle::SetBotAlly( const wxString& nick, int ally )
{
    BattleBot* bot = GetBot( nick );
    try
    {
        ASSERT_LOGIC( bot != 0, _T("Bot not found") );
    }
    catch (...)
    {
        return;
    }
    bot->bs.ally = ally;
    m_serv.UpdateBot( m_opts.battleid, bot->name, bot->bs );
}


void Battle::SetBotSide( const wxString& nick, int side )
{
    BattleBot* bot = GetBot( nick );
    try
    {
        ASSERT_LOGIC( bot != 0, _T("Bot not found") );
    }
    catch (...)
    {
        return;
    }
    bot->bs.side = side;
    m_serv.UpdateBot( m_opts.battleid, bot->name, bot->bs );
}


void Battle::SetBotColour( const wxString& nick, const wxColour& col )
{
    BattleBot* bot = GetBot( nick );
    try
    {
        ASSERT_LOGIC( bot != 0, _T("Bot not found") );
    }
    catch (...)
    {
        return;
    }
    bot->bs.colour = col;
    m_serv.UpdateBot( m_opts.battleid, bot->name, bot->bs );
}


void Battle::SetBotHandicap( const wxString& nick, int handicap )
{
    BattleBot* bot = GetBot( nick );
    try
    {
        ASSERT_LOGIC( bot != 0, _T("Bot not found") );
    }
    catch (...)
    {
        return;
    }
    if ( bot->owner != GetMe().GetNick() && !IsFounderMe() )
    {
        m_serv.DoActionBattle( m_opts.battleid, _T("thinks ") + nick + _T(" should get a ") + wxString::Format( _T("%d"),  handicap ) + _T("% resource bonus") );
        return;
    }
    bot->bs.handicap = handicap;
    m_serv.UpdateBot( m_opts.battleid, bot->name, bot->bs );
}

void Battle::ForceSide( User& user, int side )
{
    m_serv.ForceSide( m_opts.battleid, user.GetNick(), side );
}


void Battle::ForceTeam( User& user, int team )
{
    m_serv.ForceTeam( m_opts.battleid, user.GetNick(), team );
}


void Battle::ForceAlly( User& user, int ally )
{
    if (&user==&GetMe())
    {
        SetMyAlly(ally);
    }
    else
    {
        m_serv.ForceAlly( m_opts.battleid, user.GetNick(), ally );
    }
}


void Battle::ForceColour( User& user, const wxColour& col )
{
    m_serv.ForceColour( m_opts.battleid, user.GetNick(), col );
}


void Battle::ForceSpectator( User& user, bool spectator )
{
    m_serv.ForceSpectator( m_opts.battleid, user.GetNick(), spectator );
}


void Battle::BattleKickPlayer( User& user )
{
    m_serv.BattleKickPlayer( m_opts.battleid, user.GetNick() );
}

void Battle::SetHandicap( User& user, int handicap)
{
    m_serv.SetHandicap ( m_opts.battleid, user.GetNick(), handicap );
}

bool PlayerRankCompareFunction( User *a, User *b ) // should never operate on nulls. Hence, ASSERT_LOGIC is appropriate here.
{
    ASSERT_LOGIC( a, _T("fail in Autobalance, NULL player") );
    ASSERT_LOGIC( b, _T("fail in Autobalance, NULL player") );
    return ( a->GetBalanceRank() > b->GetBalanceRank() );
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

void Battle::Autobalance( BalanceType balance_type, bool support_clans, bool strong_clans, int allyteamsize )
{
    wxLogMessage(_T("Autobalancing alliances, type=%d, clans=%d, strong_clans=%d, allyteamsize=%d"),balance_type, support_clans,strong_clans, allyteamsize);
    DoAction(_T("is auto-balancing alliances ..."));
    //size_t i;
    //int num_alliances;
    CLAMP( allyteamsize, 0, 16 ); // 16 max ally teams currently supported by spring
    std::vector<Alliance>alliances;
    if ( allyteamsize == 0 ) // 0 == use num start rects
    {
        int tmp = GetNumRects();
        int ally = 0;
        for ( int i = 0; i < allyteamsize; ++i )
        {
            BattleStartRect sr = m_rects[i];
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
        for ( int i = 0; i < allyteamsize; i++ ) alliances.push_back( Alliance( i ) );
    }

    //for(i=0;i<alliances.size();++i)alliances[i].allynum=i;

    wxLogMessage( _T("number of alliances: %u"), alliances.size() );

    std::vector<User*> players_sorted;
    players_sorted.reserve( GetNumUsers() );

    int player_team_counter = 0;

    for ( size_t i = 0; i < GetNumUsers(); ++i )
    {
        if ( !GetUser( i ).BattleStatus().spectator )
        {
            players_sorted.push_back( &GetUser( i ) );
            // -- server fail? it doesnt work right.
            //ForceTeam(GetUser(i),player_team_counter);
            player_team_counter++;
        }
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
            /// if clan is too small (only 1 clan member in battle) or too big, dont count it as clan
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
        /// skip clanners, those have been added already.
        if ( clan_alliances.count( players_sorted[i]->GetClan() ) > 0 )
        {
            wxLogMessage( _T("clanner already added, nick=%s"), players_sorted[i]->GetNick().c_str() );
            continue;
        }

        /// find alliances with lowest ranksum
        /// insert current user into random one out of them
        /// since performance doesnt matter here, i simply sort alliances,
        /// then find how many alliances in beginning have lowest ranksum
        /// note that balance player ranks range from 1 to 1.1 now
        /// i.e. them are quasi equal
        /// so we're essentially adding to alliance with smallest number of players,
        /// the one with smallest ranksum.

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


    for ( size_t i = 0; i < alliances.size(); ++i )
    {
        for ( size_t j = 0; j < alliances[i].players.size(); ++j )
        {
            ASSERT_LOGIC( alliances[i].players[j], _T("fail in Autobalance, NULL player") );
            wxString msg = wxString::Format( _T("setting player %s to alliance %d"), alliances[i].players[j]->GetNick().c_str(), i );
            wxLogMessage(_T("%s"), msg.c_str() );
            ForceAlly( *alliances[i].players[j], alliances[i].allynum );
        }
    }
}

void Battle::FixTeamIDs( BalanceType balance_type, bool support_clans, bool strong_clans, int controlteamsize )
{
    wxLogMessage(_T("Autobalancing teams, type=%d, clans=%d, strong_clans=%d, controlteamsize=%d"),balance_type, support_clans, strong_clans, controlteamsize);
    DoAction(_T("is auto-balancing control teams ..."));
    //size_t i;
    //int num_alliances;
    std::vector<ControlTeam> teams;

    if ( controlteamsize == 0 ) controlteamsize = GetNumUsers(); // 0 == use num players, will use comshare only if no available team slots
    IBattle::StartType position_type = (IBattle::StartType)s2l( CustomBattleOptions().getSingleValue( _T("startpostype"), OptionsWrapper::EngineOption ) );
    if ( ( position_type == ST_Fixed ) || ( position_type == ST_Random ) ) // if fixed start pos type or random, use max teams = start pos count
    {
      try
      {
        controlteamsize = std::min( controlteamsize, LoadMap().info.posCount );
      }
      catch( assert_exception ) {}
    }
    controlteamsize = std::min( controlteamsize, 16 ); // clamp to 16 (max spring supports)

    for ( int i = 0; i < controlteamsize; i++ ) teams.push_back( ControlTeam( i ) );

    //for(i=0;i<alliances.size();++i)alliances[i].allynum=i;

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
            /// if clan is too small (only 1 clan member in battle) or too big, dont count it as clan
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
        /// skip clanners, those have been added already.
        if ( clan_teams.count( players_sorted[i]->GetClan() ) > 0 )
        {
            wxLogMessage( _T("clanner already added, nick=%s"),players_sorted[i]->GetNick().c_str() );
            continue;
        }

        /// find teams with lowest ranksum
        /// insert current user into random one out of them
        /// since performance doesnt matter here, i simply sort teams,
        /// then find how many teams in beginning have lowest ranksum
        /// note that balance player ranks range from 1 to 1.1 now
        /// i.e. them are quasi equal
        /// so we're essentially adding to teams with smallest number of players,
        /// the one with smallest ranksum.

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

CommonBattle::CommonBattle( const int id, const bool ingame, const int order ):
m_ingame(ingame),
m_order(order),
m_bot_seek(m_bots.end()),
m_bot_pos(BOT_SEEKPOS_INVALID)
{
    m_opts.battleid = id;
}

unsigned int CommonBattle::GetNumRects()
{
    return m_rects.size();
}


void CommonBattle::OnBotAdded( const wxString& nick, const wxString& owner, const UserBattleStatus& bs, const wxString& aidll )
{
    BattleBot* bot = GetBot(nick);
    bool created = true;
    if ( bot == 0 ) bot = new BattleBot();
    else created = false;

    wxLogDebugFunc( wxString::Format( _T("created: %d"), created) );

    bot->name = nick;
    bot->bs = bs;
    bot->bs.order = m_order++;
    bot->owner = owner;
    bot->aidll = aidll;

    if ( created )
    {
        m_bots.push_back( bot );
        m_bot_pos = BOT_SEEKPOS_INVALID;
    }
}


void CommonBattle::OnBotRemoved( const wxString& nick )
{
    BattleBot* bot = GetBot( nick );
    m_bots.remove( bot );
    delete bot;
    m_bot_pos = BOT_SEEKPOS_INVALID;
}


void CommonBattle::OnBotUpdated( const wxString& name, const UserBattleStatus& bs )
{
    BattleBot* bot = GetBot( name );
    try
    {
        ASSERT_LOGIC( bot != 0, _T("Bot not found") );
    }
    catch (...)
    {
        return;
    }
    int order = bot->bs.order;
    bot->bs = bs;
    bot->bs.order = order;
}


BattleBot* CommonBattle::GetBot( const wxString& name ) const
{
    std::list<BattleBot*>::const_iterator i;

    for ( i = m_bots.begin(); i != m_bots.end(); ++i )
    {
        if ( *i == 0 ) continue;
        wxLogMessage( _T("%s"), ((*i)->name).c_str ());
        if ( (*i)->name == name )
        {
            return *i;
        }
    }
    return 0;
}

BattleBot* CommonBattle::GetBot( unsigned int index ) const
{
    if ((m_bot_pos == BOT_SEEKPOS_INVALID) || (m_bot_pos > index))
    {
        m_bot_seek = m_bots.begin();
        m_bot_pos = 0;
    }
    std::advance( m_bot_seek, index - m_bot_pos );
    m_bot_pos = index;
    return *m_bot_seek;
}

unsigned int CommonBattle::GetNumBots() const
{
    return m_bots.size();
}

void OfflineBattle::RemoveUser(OfflineUser& user)
{

}

void OfflineBattle::UpdateUserBattleStatus(OfflineUser &user, UserBattleStatus status)
{

}

void OfflineBattle::AddUser( const OfflineUser& user)
{
    m_participants.push_back(user);
}

OfflineBattle::OfflineBattle(const int id)
        :CommonBattle(id, false, 0)
{}

OfflineBattle::OfflineBattle()
        :CommonBattle(0, false, 0)
{}

void OfflineBattle::SetBotTeam( const wxString& nick, int team )
{
    BattleBot* bot = GetBot( nick );
    try
    {
        ASSERT_LOGIC( bot != 0, _T("Bot not found") );
    }
    catch (...)
    {
        return;
    }
    bot->bs.team = team;
}


void OfflineBattle::SetBotAlly( const wxString& nick, int ally )
{
    BattleBot* bot = GetBot( nick );
    try
    {
        ASSERT_LOGIC( bot != 0, _T("Bot not found") );
    }
    catch (...)
    {
        return;
    }
    bot->bs.ally = ally;
}


void OfflineBattle::SetBotSide( const wxString& nick, int side )
{
    BattleBot* bot = GetBot( nick );
    try
    {
        ASSERT_LOGIC( bot != 0, _T("Bot not found") );
    }
    catch (...)
    {
        return;
    }
    bot->bs.side = side;
}


void OfflineBattle::SetBotColour( const wxString& nick, const wxColour& col )
{
    BattleBot* bot = GetBot( nick );
    try
    {
        ASSERT_LOGIC( bot != 0, _T("Bot not found") );
    }
    catch (...)
    {
        return;
    }
    bot->bs.colour = col;
}


void OfflineBattle::SetBotHandicap( const wxString& nick, int handicap )
{
    BattleBot* bot = GetBot( nick );
    try
    {
        ASSERT_LOGIC( bot != 0, _T("Bot not found") );
    }
    catch (...)
    {
        return;
    }

    bot->bs.handicap = handicap;
}

void OfflineBattle::AddUser( const wxString& nick )
{
    OfflineUser user( nick, wxEmptyString, 0 );
    AddUser( user );
}

bool OfflineBattle::ModExists()
{
    bool tmp = usync().ModExistsCheckHash( m_host_mod.hash );
    return tmp;
}

bool OfflineBattle::MapExists()
{
    return usync().MapExists( m_host_map.name );
}
