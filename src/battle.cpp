//
// Class: Battle
//

#include "battle.h"
#include "ui.h"


void Battle::Update() {
  m_ui.OnBattleInfoUpdated( *this );
}
