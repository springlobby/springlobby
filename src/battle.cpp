//
// Class: Battle
//

#include "battle.h"
#include "ui.h"


void Battle::Update() {
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

