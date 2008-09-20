/* Author: Tobi Vollebregt */

#include "autohost.h"
#include "battle.h"
#include "server.h"


void AutoHost::OnSaidBattle( Battle& battle, const wxString& nick, const wxString& msg )
{
  // autohost prototype code
  // todo: perform the checks which are now in BattleRoomTab::OnStart
  if (msg == _T("!start")) {
    battle.GetServer().StartHostedBattle();
  }
  else if (msg == _T("!balance")) {
    battle.Autobalance(balance_random, false, false);
  }
  else if (msg == _T("!cbalance")) {
    battle.Autobalance(balance_random, true, false);
  }
}
