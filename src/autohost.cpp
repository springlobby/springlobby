/* Author: Tobi Vollebregt */

#include "autohost.h"
#include "battle.h"
#include "server.h"


void AutoHost::OnSaidBattle( Battle& battle, const wxString& nick, const wxString& msg )
{
  if (msg == _T("!start")) {
    StartBattle(battle);
  }
  else if (msg == _T("!balance")) {
    battle.Autobalance(balance_random, false, false);
  }
  else if (msg == _T("!cbalance")) {
    battle.Autobalance(balance_random, true, false);
  }
}


void AutoHost::StartBattle( Battle& battle )
{
  // todo: the logic here is copied from BattleRoomTab::OnStart, may wish to refactor this sometime.
  // note: the strings here must remain untranslated because they're visible to everyone in the battle!

  if ( battle.HaveMultipleBotsInSameTeam() ) {
    battle.DoAction(_T("There are two or more bots on the same team.  Because bots don't know how to share, this won't work."));
    return;
  }

  battle.GetMe().BattleStatus().ready = true;

  if ( !battle.IsEveryoneReady() ) {
    battle.DoAction(_T("Some players are not ready yet."));
    //"Some players are not ready yet.\nRing these players?"
    //m_battle.RingNotReadyPlayers();
    return;
  }

  battle.GetServer().StartHostedBattle();
}
