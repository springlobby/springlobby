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
  m_serv.GetMe().SetBattleStatus( bs );

  m_serv.SendMyBattleStatus( bs );

}


void Battle::Say( const std::string& msg )
{
  m_serv.SayBattle( opts().battleid, msg );
}


bool Battle::IsMapAvailable()
{
  if ( !sync().MapExists( m_opts.mapname ) ) return false;

  UnitsyncMap map = sync().GetMap( m_opts.mapname );
  return ( map.hash == m_opts.maphash );
}


bool Battle::IsModAvailable()
{
  if ( !sync().ModExists( m_opts.modname ) ) return false;

  UnitsyncMod mod = sync().GetMod( m_opts.modname );
  return ( mod.hash == m_opts.hashcode );
}
