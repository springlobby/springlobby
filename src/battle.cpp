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


void Battle::Update()
{
  m_ui.OnBattleInfoUpdated( *this );
}


void Battle::Join( const std::string& password )
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


void Battle::GetFreeColour( int& r, int& g, int& b, bool excludeme )
{
  int lowest = 0;
  bool changed = true;
  while ( (changed) && (lowest < 16) ) {
    changed = false;
    for ( user_map_t::size_type i = 0; i < GetNumUsers(); i++ ) {
      if ( (&GetUser( i ) == &GetMe()) && excludeme ) continue;
      //if ( GetUser( i ).BattleStatus().spectator ) continue;
      UserBattleStatus& bs = GetUser( i ).BattleStatus();
      if ( AreColoursSimilar( bs.color_r, bs.color_g, bs.color_b, colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2] ) ) {
        lowest++;
        changed = true;
        if ( lowest >= 16 ) break;
      }
    }
    if ( lowest >= 16 ) break;
    std::list<BattleBot*>::const_iterator i;
    for( i = m_bots.begin(); i != m_bots.end(); ++i )
    {
      if ( *i == 0 ) continue;
      if ( AreColoursSimilar( (*i)->bs.color_r, (*i)->bs.color_g, (*i)->bs.color_b, colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2] ) ) {
        lowest++;
        changed = true;
        if ( lowest >= 16 ) break;
      }
    }
  }
  if ( lowest >= 16 ) lowest = 0;

  r = colour_values[lowest][0];
  g = colour_values[lowest][1];
  b = colour_values[lowest][2];
}


void Battle::OnRequestBattleStatus()
{
  int lowest = GetFreeTeamNum();

  UserBattleStatus& bs = m_serv.GetMe().BattleStatus();
  bs.team = lowest;
  bs.ally = lowest;
  bs.spectator = false;
  GetFreeColour( bs.color_r, bs.color_g, bs.color_b );

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


void Battle::Say( const std::string& msg )
{
  m_serv.SayBattle( m_opts.battleid, msg );
}


void Battle::DoAction( const std::string& msg )
{
  m_serv.DoActionBattle( m_opts.battleid, msg );
}


bool Battle::HaveMultipleBotsInSameTeam() const
{
  std::list<BattleBot*>::const_iterator i;
  wxLogDebugFunc(_T(""));

  int teams[16] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
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
  ASSERT_LOGIC(false, _T("You are not in this game.") );
  return -1;
}


void Battle::OnUserAdded( User& user )
{
  user.SetBattle( this );
  UserList::AddUser( user );
  UserBattleStatus bs;
  bs.order = m_order++;
  user.SetBattleStatus( bs, true );
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


void Battle::AddStartRect( int allyno, int left, int top, int right, int bottom )
{
  ASSERT_LOGIC( (allyno >= 0) && (allyno < 16), _T("Allyno out of bounds.") );
  BattleStartRect* sr;
  bool local;
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
  BattleStartRect* sr = m_rects[allyno];
  if ( sr == 0 ) return;
  sr->deleted = true;
}


void Battle::UpdateStartRect( int allyno )
{
  BattleStartRect* sr = m_rects[allyno];
  if ( sr == 0 ) return;
  sr->updated = true;
}


void Battle::StartRectRemoved( int allyno )
{
  BattleStartRect* sr = m_rects[allyno];
  if ( sr == 0 ) return;
  m_rects[allyno] = 0;
  delete sr;
}


void Battle::StartRectUpdated( int allyno )
{
  BattleStartRect* sr = m_rects[allyno];
  if ( sr == 0 ) return;
  sr->updated = false;
  sr->local = false;
}


BattleStartRect* Battle::GetStartRect( int allyno )
{
  ASSERT_LOGIC( (allyno >= 0) && (allyno < 16), _T("Allyno out of bounds.") );
  return m_rects[allyno];
}

void Battle::ClearStartRects()
{
  for ( int i = 0; i < 16; i++ ) RemoveStartRect( i );
}


void Battle::AddBot( const std::string& nick, const std::string& owner, UserBattleStatus status, const std::string& aidll )
{
  m_serv.AddBot( m_opts.battleid, nick, owner, status, aidll );
}


void Battle::RemoveBot( const std::string& nick )
{
  m_serv.RemoveBot( m_opts.battleid, nick );
}


void Battle::SetBotTeam( const std::string& nick, int team )
{
  BattleBot* bot = GetBot( nick );
  ASSERT_LOGIC( bot != 0, _T("Bot not found") );
  bot->bs.team = team;
  m_serv.UpdateBot( m_opts.battleid, bot->name, bot->bs );
}


void Battle::SetBotAlly( const std::string& nick, int ally )
{
  BattleBot* bot = GetBot( nick );
  ASSERT_LOGIC( bot != 0, _T("Bot not found") );
  bot->bs.ally = ally;
  m_serv.UpdateBot( m_opts.battleid, bot->name, bot->bs );
}


void Battle::SetBotSide( const std::string& nick, int side )
{
  BattleBot* bot = GetBot( nick );
  ASSERT_LOGIC( bot != 0, _T("Bot not found") );
  bot->bs.side = side;
  m_serv.UpdateBot( m_opts.battleid, bot->name, bot->bs );
}


void Battle::SetBotColour( const std::string& nick, int r, int g, int b )
{
  BattleBot* bot = GetBot( nick );
  ASSERT_LOGIC( bot != 0, _T("Bot not found") );
  bot->bs.color_r = r;
  bot->bs.color_g = g;
  bot->bs.color_b = b;
  m_serv.UpdateBot( m_opts.battleid, bot->name, bot->bs );
}


void Battle::SetBotHandicap( const std::string& nick, int handicap )
{
  BattleBot* bot = GetBot( nick );
  ASSERT_LOGIC( bot != 0, _T("Bot not found") );
  if ( bot->owner != GetMe().GetNick() && !IsFounderMe() )
  {
    m_serv.DoActionBattle( m_opts.battleid, "thinks " + nick + " should get a " + i2s( handicap ) + "% resource bonus" );
    return;
  }
  bot->bs.handicap = handicap;
  m_serv.UpdateBot( m_opts.battleid, bot->name, bot->bs );
}


void Battle::OnBotAdded( const std::string& nick, const std::string& owner, const UserBattleStatus& bs, const std::string& aidll )
{
  BattleBot* bot = GetBot(nick);
  bool created = true;
  if ( bot == 0 ) bot = new BattleBot();
  else created = false;

  wxLogDebugFunc( _T("created: ") + WX_STRING(i2s(created)) );

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


void Battle::OnBotRemoved( const std::string& nick )
{
  BattleBot* bot = GetBot( nick );
  m_bots.remove( bot );
  delete bot;
  m_bot_pos = BOT_SEEKPOS_INVALID;
}


void Battle::OnBotUpdated( const std::string& name, const UserBattleStatus& bs )
{
  BattleBot* bot = GetBot( name );
  ASSERT_LOGIC( bot != 0, _T("Bad bot name") );
  int order = bot->bs.order;
  bot->bs = bs;
  bot->bs.order = order;
}


BattleBot* Battle::GetBot( const std::string& name )
{
  std::list<BattleBot*>::const_iterator i;

  for( i = m_bots.begin(); i != m_bots.end(); ++i )
  {
    if ( *i == 0 ) continue;
    wxLogMessage( WX_STRING((*i)->name) );
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
  m_serv.ForceAlly( m_opts.battleid, user.GetNick(), ally );
}


void Battle::ForceColour( User& user, int r, int g, int b )
{
  m_serv.ForceColour( m_opts.battleid, user.GetNick(), r, g, b );
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

