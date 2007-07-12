//
// Class: Battle
//

#include "battle.h"
#include "ui.h"
#include "unitsync.h"


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


void Battle::OnRequestBattleStatus()
{
  int lowest = 1;
  bool changed = true;
  while ( changed ) {
    changed = false;
    for ( int i = 0; i < GetNumUsers(); i++ ) {
      if ( GetUser( i ).GetBattleStatus().team == lowest ) {
        lowest++;
        changed = true;
      }
    }
  }

  UserBattleStatus bs = m_serv.GetMe().GetBattleStatus();
  bs.team = lowest;
  bs.ally = lowest;
  if ( IsSynced() ) bs.sync = SYNC_SYNCED;
  else bs.sync = SYNC_UNSYNCED;
  
  m_serv.GetMe().SetBattleStatus( bs );

  SendMyBattleStatus();

}


void Battle::SendMyBattleStatus()
{
  UserBattleStatus bs = m_serv.GetMe().GetBattleStatus();
  m_serv.SendMyBattleStatus( bs );
}


void Battle::SetImReady( bool ready )
{
  UserBattleStatus bs = m_serv.GetMe().GetBattleStatus();
  
  bs.ready = ready;
  
  m_serv.GetMe().SetBattleStatus( bs );
  SendMyBattleStatus();
}


bool Battle::IsSynced()
{
  return ( usync().MapExists( m_opts.mapname, m_opts.maphash ) && usync().ModExists( m_opts.modname ) );
}


void Battle::Say( const std::string& msg )
{
  m_serv.SayBattle( opts().battleid, msg );
}


bool Battle::IsMapAvailable()
{
  if ( !usync().MapExists( m_opts.mapname ) ) return false;

  UnitsyncMap map = usync().GetMap( m_opts.mapname );
  return ( map.hash == m_opts.maphash );
}


bool Battle::IsModAvailable()
{
  if ( !usync().ModExists( m_opts.modname ) ) return false;

  UnitsyncMod mod = usync().GetMod( m_opts.modname );
  return true;
  //return ( mod.hash == m_opts.hashcode );
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
  for (int i = 0; i < GetNumUsers(); i++) {
    if ( &GetUser(i) == &m_serv.GetMe() ) return i;
  }
  ASSERT_LOGIC(false, "You are not in this game.");
  return -1;
}


void Battle::AddStartRect( int allyno, int left, int top, int right, int bottom )
{
  ASSERT_LOGIC( (allyno >= 0) && (allyno < 16), "Allyno out of bounds." );
  BattleStartRect* sr;
  if ( m_rects[allyno] == NULL ) sr = new BattleStartRect();
  else sr = m_rects[allyno];
  sr->ally = allyno;
  sr->left = left;
  sr->top = top;
  sr->right = right;
  sr->bottom = bottom;
  m_rects[allyno] = sr;
}


void Battle::RemoveStartRect( int allyno )
{
  BattleStartRect* sr = m_rects[allyno];
  if ( sr == NULL ) return;
  m_rects[allyno] = NULL;
  delete sr;
}

void Battle::ClearStartRects()
{
  for ( int i = 0; i < 16; i++ ) RemoveStartRect( i );
}


void Battle::DisableUnit( const std::string& unitname )
{
	std::string::size_type i = m_units.find( unitname, 0 );
  if ( i != std::string::npos ) return;
  m_units += unitname;
  m_units += " ";
  m_units_num++;
}


void Battle::EnableUnit( const std::string& unitname )
{
	std::string::size_type i = m_units.find( unitname, 0 );
  if ( i == std::string::npos ) return;
  m_units.replace( i, unitname.length()+1, "" );
  m_units_num--;
}


void Battle::EnableAllUnits()
{
  m_units = "";
  m_units_num = 0;
}


std::string Battle::DisabledUnits()
{
  return m_units;
}

