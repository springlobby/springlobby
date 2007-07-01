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
