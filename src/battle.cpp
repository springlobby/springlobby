/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: Battle
//

#include <stdexcept>

#include "battle.h"
#include "ui.h"
#include "iunitsync.h"
#include "server.h"
#include "user.h"
#include "utils.h"
#include "uiutils.h"

#include <algorithm>
#include <math.h>


const std::list<BattleBot*>::size_type BOT_SEEKPOS_INVALID = (std::list<BattleBot*>::size_type)(-1);


Battle::Battle( Server& serv, Ui& ui, int id ) :
  m_serv(serv),
  m_ui(ui),
  m_ingame(false),
  m_order(0),
  m_rects(16, static_cast<BattleStartRect*>(0)),
  m_bot_seek(m_bots.end()),
  m_bot_pos(BOT_SEEKPOS_INVALID)
{
  m_opts.battleid = id;

}


Battle::~Battle() {
  for (user_map_t::size_type i = 0; i < GetNumUsers(); i++ )
//    GetUser(i).SetBattle( 0 );
  ClearStartRects();
}


Server& Battle::GetServer()
{
  return m_serv;
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
  m_ui.OnBattleInfoUpdated( *this );
}


void Battle::Update( const wxString& Tag )
{
  m_ui.OnBattleInfoUpdated( *this, Tag );
}


void Battle::Join( const wxString& password )
{
  m_serv.JoinBattle( m_opts.battleid, password );
}


void Battle::Leave()
{
  m_serv.LeaveBattle( m_opts.battleid );
}


int Battle::GetFreeTeamNum( bool excludeme )
{
  int lowest = 0;
  bool changed = true;
  while ( changed ) {
    changed = false;
    for ( user_map_t::size_type i = 0; i < GetNumUsers(); i++ ) {
      if ( (&GetUser( i ) == &GetMe()) && excludeme ) continue;
      //if ( GetUser( i ).BattleStatus().spectator ) continue;
      if ( GetUser( i ).BattleStatus().team == lowest ) {
        lowest++;
        changed = true;
      }
    }
    std::list<BattleBot*>::const_iterator i;
    for( i = m_bots.begin(); i != m_bots.end(); ++i )
    {
      if ( *i == 0 ) continue;
      if ( (*i)->bs.team == lowest ) {
        lowest++;
        changed = true;
      }
    }
  }
  return lowest;
}


wxColour Battle::GetFreeColour( bool excludeme )
{
  int lowest = 0;
  bool changed = true;
  while ( changed ) {
    changed = false;
    for ( user_map_t::size_type i = 0; i < GetNumUsers(); i++ ) {
      if ( (&GetUser( i ) == &GetMe()) && excludeme ) continue;
      //if ( GetUser( i ).BattleStatus().spectator ) continue;
      UserBattleStatus& bs = GetUser( i ).BattleStatus();
      if ( AreColoursSimilar( bs.colour, wxColour(colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2]) ) ) {
        lowest++;
        changed = true;
      }
    }
    std::list<BattleBot*>::const_iterator i;
    for( i = m_bots.begin(); i != m_bots.end(); ++i )
    {
      if ( *i == 0 ) continue;
      if ( AreColoursSimilar( (*i)->bs.colour, wxColour(colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2]) ) ) {
        lowest++;
        changed = true;
      }
    }
  }
  return wxColour( colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2] );
}


void Battle::OnRequestBattleStatus()
{
  int lowest = GetFreeTeamNum();

  UserBattleStatus& bs = m_serv.GetMe().BattleStatus();
  bs.team = lowest;
  bs.ally = lowest;
  bs.spectator = false;
  bs.colour = GetFreeColour();

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
  if ( MapExists() && ModExists() ) {
    return true;
  } else {
    return false;
  }
}


/*bool Battle::HasMod()
{
  return usync()->ModExists( m_opts.modname );
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
  for( i = m_bots.begin(); i != m_bots.end(); ++i )
  {
    if ( *i == 0 ) continue;
    if ( teams[(*i)->bs.team ] != -1 )return true;
    teams[ (*i)->bs.team ] = 1;
  }
  return false;
}


User& Battle::GetMe()
{
  return m_serv.GetMe();
}


bool Battle::IsFounderMe()
{
  return (m_opts.founder == m_serv.GetMe().GetNick());
}

int Battle::GetMyPlayerNum()
{
  for (user_map_t::size_type i = 0; i < GetNumUsers(); i++) {
    if ( &GetUser(i) == &m_serv.GetMe() ) return i;
  }
  ASSERT_RUNTIME(false, _T("You are not in this game.") );
  return -1;
}


void Battle::OnUserAdded( User& user )
{
  user.SetBattle( this );
  UserList::AddUser( user );
  UserBattleStatus bs;
  bs.order = m_order++;
  user.UpdateBattleStatus( bs, true );
}


void Battle::OnUserRemoved( User& user )
{
  user.SetBattle( 0 );
  UserList::RemoveUser( user.GetNick() );
}


void Battle::KickPlayer( User& user )
{
  m_serv.BattleKickPlayer( m_opts.battleid, user.GetNick() );
}


bool Battle::IsEveryoneReady()
{
  for (user_map_t::size_type i = 0; i < GetNumUsers(); i++) {
    UserBattleStatus& bs = GetUser(i).BattleStatus();
    if ( !bs.ready && !bs.spectator ) return false;
  }
  return true;
}


void Battle::RingNotReadyPlayers()
{
  for (user_map_t::size_type i = 0; i < GetNumUsers(); i++) {
    User& u = GetUser(i);
    UserBattleStatus& bs = u.BattleStatus();
    if ( !bs.ready && !bs.spectator ) m_serv.Ring( u.GetNick() );
  }
}


bool Battle::ExecuteSayCommand( const wxString& cmd )
{
  if ( cmd.BeforeFirst(' ').Lower() == _T("/me") ) {
    m_serv.DoActionBattle( m_opts.battleid, cmd.AfterFirst(' ') );
    return true;
  }  else return false;
}


void Battle::AddStartRect( int allyno, int left, int top, int right, int bottom )
{
  ASSERT_LOGIC( (allyno >= 0 || allyno < int(GetMaxPlayers()) ), _T("Allyno out of bounds.") );
  BattleStartRect* sr;
  bool local;
  if ( allyno >= int(m_rects.size()) ) m_rects.push_back(0); // add new element is it exceeds the vector bounds
  if ( m_rects[allyno] == 0 ) {
    sr = new BattleStartRect();
    local = true;
  } else {
    sr = m_rects[allyno];
    local = false;
  }

  sr->ally = allyno;
  sr->left = left;
  sr->top = top;
  sr->right = right;
  sr->bottom = bottom;
  sr->local = local;
  sr->updated = local;
  sr->deleted = false;
  m_rects[allyno] = sr;
}


void Battle::RemoveStartRect( int allyno )
{
  if ( allyno >= int(m_rects.size() )) return;
  BattleStartRect* sr = m_rects[allyno];
  if ( sr == 0 ) return;
  sr->deleted = true;
}


void Battle::UpdateStartRect( int allyno )
{
  if ( allyno >= int(m_rects.size()) ) return;
  BattleStartRect* sr = m_rects[allyno];
  if ( sr == 0 ) return;
  sr->updated = true;
}


void Battle::StartRectRemoved( int allyno )
{
  if ( allyno >= int(m_rects.size()) ) return;
  BattleStartRect* sr = m_rects[allyno];
  if ( sr == 0 ) return;
  m_rects[allyno] = 0;
  delete sr;
}


void Battle::StartRectUpdated( int allyno )
{
  if ( allyno >= int(m_rects.size()) ) return;
  BattleStartRect* sr = m_rects[allyno];
  if ( sr == 0 ) return;
  sr->updated = false;
  sr->local = false;
}


BattleStartRect* Battle::GetStartRect( int allyno )
{
  ASSERT_LOGIC( (allyno >= 0 || allyno < int(GetMaxPlayers()) ), _T("Allyno out of bounds.") );
  if ( allyno >= int(m_rects.size() )) return 0;
  return m_rects[allyno];
}

void Battle::ClearStartRects()
{
  for ( std::vector<BattleStartRect*>::size_type i = 0; i < GetNumRects(); i++ ) RemoveStartRect( i );
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
  ASSERT_LOGIC( bot != 0, _T("Bot not found") );
  bot->bs.team = team;
  m_serv.UpdateBot( m_opts.battleid, bot->name, bot->bs );
}


void Battle::SetBotAlly( const wxString& nick, int ally )
{
  BattleBot* bot = GetBot( nick );
  ASSERT_LOGIC( bot != 0, _T("Bot not found") );
  bot->bs.ally = ally;
  m_serv.UpdateBot( m_opts.battleid, bot->name, bot->bs );
}


void Battle::SetBotSide( const wxString& nick, int side )
{
  BattleBot* bot = GetBot( nick );
  ASSERT_LOGIC( bot != 0, _T("Bot not found") );
  bot->bs.side = side;
  m_serv.UpdateBot( m_opts.battleid, bot->name, bot->bs );
}


void Battle::SetBotColour( const wxString& nick, const wxColour& col )
{
  BattleBot* bot = GetBot( nick );
  ASSERT_LOGIC( bot != 0, _T("Bot not found") );
  bot->bs.colour = col;
  m_serv.UpdateBot( m_opts.battleid, bot->name, bot->bs );
}


void Battle::SetBotHandicap( const wxString& nick, int handicap )
{
  BattleBot* bot = GetBot( nick );
  ASSERT_LOGIC( bot != 0, _T("Bot not found") );
  if ( bot->owner != GetMe().GetNick() && !IsFounderMe() )
  {
    m_serv.DoActionBattle( m_opts.battleid, _T("thinks ") + nick + _T(" should get a ") + wxString::Format( _T("%d"),  handicap ) + _T("% resource bonus") );
    return;
  }
  bot->bs.handicap = handicap;
  m_serv.UpdateBot( m_opts.battleid, bot->name, bot->bs );
}


void Battle::OnBotAdded( const wxString& nick, const wxString& owner, const UserBattleStatus& bs, const wxString& aidll )
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

  if ( created ) {
    m_bots.push_back( bot );
    m_bot_pos = BOT_SEEKPOS_INVALID;
  }
}


void Battle::OnBotRemoved( const wxString& nick )
{
  BattleBot* bot = GetBot( nick );
  m_bots.remove( bot );
  delete bot;
  m_bot_pos = BOT_SEEKPOS_INVALID;
}


void Battle::OnBotUpdated( const wxString& name, const UserBattleStatus& bs )
{
  BattleBot* bot = GetBot( name );
  ASSERT_LOGIC( bot != 0, _T("Bad bot name") );
  int order = bot->bs.order;
  bot->bs = bs;
  bot->bs.order = order;
}


BattleBot* Battle::GetBot( const wxString& name )
{
  std::list<BattleBot*>::const_iterator i;

  for( i = m_bots.begin(); i != m_bots.end(); ++i )
  {
    if ( *i == 0 ) continue;
    wxLogMessage( _T("%s"), ((*i)->name).c_str ());
    if ( (*i)->name == name ) {
      return *i;
    }
  }
  return 0;
}

BattleBot* Battle::GetBot( unsigned int index )
{
  if ((m_bot_pos == BOT_SEEKPOS_INVALID) || (m_bot_pos > index)) {
    m_bot_seek = m_bots.begin();
    m_bot_pos = 0;
  }
  std::advance( m_bot_seek, index - m_bot_pos );
  m_bot_pos = index;
  return *m_bot_seek;
}


unsigned int Battle::GetNumBots()
{
  return m_bots.size();
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
  if(&user==&GetMe()){
    SetMyAlly(ally);
  }else{
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


std::vector<BattleStartRect*>::size_type Battle::GetNumRects()
{
  return m_rects.size();
}


bool PlayerRankCompareFunction(User *a, User *b){/// should never operate on nulls. Hence, ASSERT_LOGIC is appropriate here.
  ASSERT_LOGIC(a,_T("fail in Autobalance, NULL player"));
  ASSERT_LOGIC(b,_T("fail in Autobalance, NULL player"));
  return a->GetBalanceRank() > b->GetBalanceRank();
}

struct Alliance{
  std::vector<User *>players;
  float ranksum;
  int allynum;
  Alliance():ranksum(0){}
  void AddPlayer(User *player){
    if(player){
      players.push_back(player);
      ranksum+=player->GetBalanceRank();
    }
  }
  void AddAlliance(Alliance &other){
    for(std::vector<User *>::iterator i=other.players.begin();i!=other.players.end();++i)AddPlayer(*i);
  }
  bool operator <(const Alliance &other) const
  {
    return ranksum<other.ranksum;
  }
};

int my_random(int range){
  return rand()%range;
}

void shuffle(std::vector<User *> players){/// proper shuffle.
  for(int i=0;i<players.size();++i){/// the players below i are shuffled, the players above arent
    int rn=i+my_random(players.size()-i);/// the top of shuffled part becomes random card from unshuffled part
    User *tmp=players[i];
    players[i]=players[rn];
    players[rn]=tmp;
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

void Battle::Autobalance(int balance_type, bool support_clans, bool strong_clans){
  wxLogMessage(_T("Autobalancing players"));
  DoAction(_T("is balancing allies"));
  int tmp=GetNumRects();
  size_t i;
  int num_alliances=0;
  for(i=0;i<tmp;++i){
    BattleStartRect* sr = m_rects[i];
    if ( sr == 0 ) break;
    if(sr->deleted)break;
    num_alliances=i;
  }

  if(num_alliances<2)num_alliances=2;

  std::vector<Alliance>alliances(num_alliances);
  for(i=0;i<alliances.size();++i)alliances[i].allynum=i;

  wxLogMessage(_T("number of alliances: %d"),num_alliances);

  std::vector<User*> players_sorted;
  players_sorted.reserve(GetNumUsers());



  for(size_t i=0;i<GetNumUsers();++i){
    if(!GetUser(i).BattleStatus().spectator)players_sorted.push_back(&GetUser(i));
  }

  shuffle(players_sorted);

  std::map<wxString, Alliance> clan_alliances;
  if(support_clans){
    for(size_t i=0;i<players_sorted.size();++i){
      wxString clan=players_sorted[i]->GetClan();
      if(!clan.empty()){
        clan_alliances[clan].AddPlayer(players_sorted[i]);
      }
    }
  };

  if(balance_type!=balance_random)std::sort(players_sorted.begin(),players_sorted.end(),PlayerRankCompareFunction);

  if(support_clans){
    for(std::map<wxString, Alliance>::iterator clan_it=clan_alliances.begin();clan_it!=clan_alliances.end();++clan_it){
      Alliance &clan=(*clan_it).second;
      /// if clan is too small (only 1 clan member in battle) or too big, dont count it as clan
      if(clan.players.size()<2 || (!strong_clans) && (clan.players.size()>(players_sorted.size()+alliances.size()-1)/alliances.size())){
        clan_alliances.erase(clan_it);
        continue;
      }
      wxLogMessage(_T("Inserting clan %s"),(*clan_it).first.c_str());
      std::sort(alliances.begin(),alliances.end());
      float lowestrank=alliances[0].ranksum;
      int rnd_k=1;// number of alliances with rank equal to lowestrank
      while(rnd_k<alliances.size()){
        if(fabs(alliances[my_random(rnd_k)].ranksum-lowestrank)>0.01){
          break;
        }
        rnd_k++;
      }
      wxLogMessage(_T("number of lowestrank alliances with same rank=%d"),rnd_k);
      alliances[my_random(rnd_k)].AddAlliance(clan);
    }
  }

  for(i=0;i<players_sorted.size();++i){
    /// skip clanners, those have been added already.
    if(clan_alliances.count(players_sorted[i]->GetClan())>0)continue;

    /// find alliances with lowest ranksum
    /// insert current user into random one out of them
    /// since performance doesnt matter here, i simply sort alliances,
    /// then find how many alliances in beginning have lowest ranksum
    /// note that balance player ranks range from 1 to 1.1 now
    /// i.e. them are quasi equal
    /// so we're essentially adding to alliance with smallest number of players,
    /// the one with smallest ranksum.

    std::sort(alliances.begin(),alliances.end());
    float lowestrank=alliances[0].ranksum;
    int rnd_k=1;// number of alliances with rank equal to lowestrank
    while(rnd_k<alliances.size()){
      if(fabs(alliances[my_random(rnd_k)].ranksum-lowestrank)>0.01){
        break;
      }
      rnd_k++;
    }
    wxLogMessage(_T("number of lowestrank alliances with same rank=%d"),rnd_k);
    alliances[my_random(rnd_k)].AddPlayer(players_sorted[i]);
  }

  for(i=0;i<alliances.size();++i){
    for(size_t j=0;j<alliances[i].players.size();++j){
      ASSERT_LOGIC(alliances[i].players[j],_T("fail in Autobalance, NULL player"));
      wxLogMessage(_T("setting player %s to alliance %d"),alliances[i].players[j]->GetNick().c_str(),i);
      ForceAlly(*alliances[i].players[j],alliances[i].allynum);
    }
  }
  Update();
}

