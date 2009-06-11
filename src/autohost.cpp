/* Author: Tobi Vollebregt */

#include "autohost.h"
#include "battle.h"
#include "server.h"
#include "settings.h"
#include "user.h"
#include "utils.h"
#include <wx/log.h>


AutoHost::AutoHost( Battle& battle )
: m_battle(battle), m_enabled(false), m_lastActionTime(0)
{
}


void AutoHost::SetEnabled( const bool enabled )
{
  m_enabled = enabled;
}

bool AutoHost::GetEnabled()
{
	return m_enabled;
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
  }
  else if ( msg.StartsWith( _T("!balance") ) ) {
    unsigned int num = s2l( msg.AfterFirst( _T(' ') ) );
    m_battle.Autobalance(IBattle::balance_random, false, false, num);
  }
  else if ( msg.StartsWith( _T("!cbalance") ) ) {
    unsigned int num = s2l( msg.AfterFirst( _T(' ') ) );
    m_battle.Autobalance(IBattle::balance_random, true, false, num);
  }
  else if (msg == _T("!help")) {
    m_battle.DoAction( _T( "The following commands are available:" ) );
    m_battle.DoAction( _T( "!help: this guide." ) );
    m_battle.DoAction( _T( "!start: starts the battle." ) );
    m_battle.DoAction( _T( "!balance: tries to put players into teams by how many start boxes there are.") );
    m_battle.DoAction( _T( "!cbalance: see !balance but tries to put clanmates together first." ) );
    m_battle.DoAction( _T( "!fixcolors: changes players duplicate colours so they are unique." ) );
    m_battle.DoAction( _T( "!fixids: makes control teams unique (disables comsharing)." ) );
    m_battle.DoAction( _T( "!spectunsynced: sets all players with unsynced status to be spectators." ) );
    m_battle.DoAction( _T( "!listprofiles: lists the available battle profiles." ) );
    m_battle.DoAction( _T( "!loadprofile profilename: loads an available battle profile." ) );
    m_battle.DoAction( _T( "!ring: rings players that are not ready." ) );
    m_battle.DoAction( _T( "!lock: prevents more people to join." ) );
    m_battle.DoAction( _T( "!unlock: opens the battle again." ) );
  }
  else if ( msg == _T("!ring") ) {
    m_battle.RingNotReadyPlayers();
    m_battle.DoAction( _T( "is ringing players not ready ..." ) );
  }
  else if ( msg == _T("!listprofiles") ) {
    wxArrayString profilelist = m_battle.GetPresetList();
    unsigned int count = profilelist.GetCount();
    if (count == 0) {
      m_battle.DoAction( _T( "There are no presets available.") );
    }
    else {
      m_battle.DoAction( _T( "The following presets are available:" ) );
      for ( unsigned int i = 0; i < count; i++ ) {
        m_battle.DoAction( profilelist[i] );
      }
    }
  }
  else if ( msg.BeforeFirst( _T(' ') ) == _T("!loadprofile") ) {
    wxString profilename = GetBestMatch( m_battle.GetPresetList(), msg.AfterFirst(_T(' ')) );
    if ( !m_battle.LoadOptionsPreset( profilename ) )
      m_battle.DoAction( _T( "Profile not found, use !listprofiles for a list of available profiles." ) );
    else m_battle.DoAction( _T("has loaded profile: ") + profilename );
  }
  else if ( msg == _T("!fixcolors") ) {
    m_battle.FixColours();
    m_battle.DoAction( _T( "is fixing colors." ) );
  }
  else if ( msg == _T("!lock") ) {
    m_battle.SetIsLocked( true );
    m_battle.DoAction( _T( "has locked the battle." ) );
    m_battle.SendHostInfo( IBattle::HI_Locked );
  }
  else if ( msg == _T("!unlock") ) {
    m_battle.SetIsLocked( false );
    m_battle.DoAction( _T( "has unlocked the battle." ) );
    m_battle.SendHostInfo( IBattle::HI_Locked );
  }
  else if ( msg.StartsWith( _T("!fixids") ) ) {
    unsigned int num = s2l( msg.AfterFirst( _T(' ') ) );
    m_battle.FixTeamIDs( IBattle::balance_divide, false, false, num );
  }
  else if ( msg.StartsWith( _T("!cfixids") ) ) {
    unsigned int num = s2l( msg.AfterFirst( _T(' ') ) );
    m_battle.FixTeamIDs( IBattle::balance_divide, true, true, num );
  }
  else if ( msg == _T("!spectunsynced") ) {
    m_battle.ForceUnsyncedToSpectate();
    m_battle.DoAction( _T( "is forcing unsynced players to be spectators." ) );
  }
  else if ( msg == _T("!map") ) {
    wxString mapname = GetBestMatch( usync().GetMapList(), msg.AfterFirst(_T(' ')) );
		try
		{
			UnitSyncMap map = usync().GetMap( index );
			m_battle.SetLocalMap( map );
			m_battle.DoAction( _T( "is switching to map " ) + mapname );
			m_battle.SendHostInfo( IBattle::HI_Map );
			for( unsigned int i=0; i < m_battle.GetNumRects(); ++i ) if ( m_battle.GetStartRect( i ).exist ) m_battle.RemoveStartRect(i);
			m_battle.SendHostInfo( IBattle::HI_StartRects );
		} catch (...) {}
  }
  else return;
  m_lastActionTime = currentTime;
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
    m_battle.SendHostInfo( IBattle::HI_Locked );
  }
}


void AutoHost::StartBattle()
{
  // todo: the logic here is copied from BattleRoomTab::OnStart, may wish to refactor this sometime.
  // note: the strings here must remain untranslated because they're visible to everyone in the battle!

  m_battle.GetMe().BattleStatus().ready = true;

  if ( !m_battle.IsEveryoneReady() ) {
    m_battle.DoAction(_T("Some players are not ready yet."));
    return;
  }

  m_battle.DoAction(_T("is starting game ..."));
  m_battle.GetServer().StartHostedBattle();

  // todo: copied from Ui::StartHostedBattle
  sett().SetLastHostMap( m_battle.GetHostMapName() );
  sett().SaveSettings();
}
