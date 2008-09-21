/* Author: Tobi Vollebregt */

#include "autohost.h"
#include "battle.h"
#include "server.h"
#include "user.h"


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
  else if (msg == _T("!help")) {
    m_battle.DoAction( _T( "The following commands are available:" ) );
    m_battle.DoAction( _T( "!help: this guide.\n!start: starts the battle." ) );
    m_battle.DoAction( _T( "!balance: tries to put players into teams by how many start boxes there are.") );
    m_battle.DoAction( _T( "!cbalance: see !balance but tries to put clanmates togheter first." ) );
    m_battle.DoAction( _T( "!listprofiles: lists the available battle profiles." ) );
    m_battle.DoAction( _T( "!loadprofile profilename: loads an available battle profile." ) );
    m_battle.DoAction( _T( "!ring: rings players that are not ready." ) );
    m_battle.DoAction( _T( "!fixcolors: changes players duplicate colours so they are unique." ) );
    m_battle.DoAction( _T( "!lock: prevents more people to join." ) );
    m_battle.DoAction( _T( "!unlock: opens the battle again." ) );
    m_lastActionTime = currentTime;
  }
  else if ( msg == _T("!ring") ) {
    m_battle.RingNotReadyPlayers();
    m_battle.DoAction( _T( "Ringing players not ready." ) );
    m_lastActionTime = currentTime;
  }
  else if ( msg == _T("!listprofiles") ) {
    wxArrayString profilelist = m_battle.GetPresetList();
    unsigned int count = profilelist.GetCount();
    m_battle.DoAction( _T( "The following presets are available:" ) );
    for ( unsigned int i = 0; i < count; i++ ) {
      m_battle.DoAction( profilelist[i] );
    }
    m_lastActionTime = currentTime;
  }
  else if ( msg.BeforeFirst( _T(' ') ) == _T("!loadprofile") ) {
    if ( !m_battle.LoadOptionsPreset( msg.AfterFirst(_T(' ')) ) )
      m_battle.DoAction( _T( "Profile not found, use !listprofiles for a list of available profiles." ) );
    m_lastActionTime = currentTime;
  }
  else if ( msg == _T("!fixcolors") ) {
    m_battle.FixColours();
    m_battle.DoAction( _T( "is fixing colors." ) );
    m_lastActionTime = currentTime;
  }
  else if ( msg == _T("!lock") ) {
    m_battle.SetIsLocked( true );
    m_battle.DoAction( _T( "has locked the battle." ) );
    m_battle.SendHostInfo( HI_Locked );
    m_lastActionTime = currentTime;
  }
  else if ( msg == _T("!unlock") ) {
    m_battle.SetIsLocked( false );
    m_battle.DoAction( _T( "has unlocked the battle." ) );
    m_battle.SendHostInfo( HI_Locked );
    m_lastActionTime = currentTime;
  }
}


/// Should only be called if user isn't immediately kicked (ban / rank limit)
void AutoHost::OnUserAdded( User& user )
{
  // do nothing if autohost functionality is disabled

  if (!m_enabled)
    return;

  m_battle.DoAction(_T("Hi ") + user.GetNick() + _T(", this battle is in SpringLobby autohost mode. For help say !help"));
}


void AutoHost::OnUserRemoved( User& user )
{
  // do nothing if autohost functionality is disabled

  if (!m_enabled)
    return;

  if ( m_battle.GetNumUsers() == 1 && m_battle.IsLocked() )
  {
    m_battle.SetIsLocked( false );
    m_battle.DoAction( _T( "has auto-unlocked the battle." ) );
    m_battle.SendHostInfo( HI_Locked );
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

  m_battle.DoAction(_T("Starting game."));
  m_battle.GetServer().StartHostedBattle();
}


wxString AutoHost::GetExtraCommandLineParams()
{
  if (m_enabled) {
    // -m, --minimise          Start minimised
    // -q [T], --quit=[T]      Quit immediately on game over or after T seconds
    return _T("--minimise --quit=1000000000");
  }
  else
    return wxEmptyString;
}
