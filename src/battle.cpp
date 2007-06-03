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

