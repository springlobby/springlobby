/* Author: Tobi Vollebregt */

#include "autohost.h"
#include "battle.h"
#include "server.h"


AutoHost::AutoHost( Battle& battle )
: m_battle(battle), m_enabled(false), m_lastActionTime(0)
{
}


void AutoHost::SetEnabled( const bool enabled )
{
  m_enabled = enabled;
}


void AutoHost::OnSaidBattle( const wxString& nick, const wxString& msg )
{
  // do nothing if autohost functionality is disabled

  if (!m_enabled)
    return;

  // protect against command spam

  time_t currentTime = time(NULL);

  if ((currentTime - m_lastActionTime) < 5)
    return;

  // check for autohost commands

  if (msg == _T("!start")) {
    StartBattle();
    m_lastActionTime = currentTime;
  }
  else if (msg == _T("!balance")) {
    m_battle.Autobalance(balance_random, false, false);
    m_lastActionTime = currentTime;
  }
  else if (msg == _T("!cbalance")) {
    m_battle.Autobalance(balance_random, true, false);
    m_lastActionTime = currentTime;
  }
}


void AutoHost::StartBattle()
{
  // todo: the logic here is copied from BattleRoomTab::OnStart, may wish to refactor this sometime.
  // note: the strings here must remain untranslated because they're visible to everyone in the battle!

  if ( m_battle.HaveMultipleBotsInSameTeam() ) {
    m_battle.DoAction(_T("There are two or more bots on the same team.  Because bots don't know how to share, this won't work."));
    return;
  }

  m_battle.GetMe().BattleStatus().ready = true;

  if ( !m_battle.IsEveryoneReady() ) {
    m_battle.DoAction(_T("Some players are not ready yet."));
    //"Some players are not ready yet.\nRing these players?"
    //m_battle.RingNotReadyPlayers();
    return;
  }

  m_battle.GetServer().StartHostedBattle();
}
