/* Author: Tobi Vollebregt */

#include "autohost.h"
#include "battle.h"
#include "server.h"
#include "settings.h"
#include "user.h"
#include <wx/log.h>

template<typename T>
static T min(T a, T b, T c)
{
  return std::min(a, std::min(b, c));
}


/**
 * @brief Computes Levenshtein distance (edit distance) between two strings.
 * @return the Levenshtein distance normalized by the longest string's length.
 * @note Source: http://en.wikipedia.org/wiki/Levenshtein_distance
 */
static double LevenshteinDistance(wxString s, wxString t)
{
  s.MakeLower(); // case insensitive edit distance
  t.MakeLower();

  const int m = s.length(), n = t.length(), _w = m + 1;
  std::vector<unsigned char> _d((m + 1) * (n + 1));
#define D(x, y) _d[(y) * _w + (x)]

  for (int i = 0; i <= m; ++i) D(i,0) = i;
  for (int j = 0; j <= n; ++j) D(0,j) = j;

  for (int i = 1; i <= m; ++i) {
    for (int j = 1; j <= n; ++j) {
      const int cost = (s[i - 1] != t[j - 1]);
      D(i,j) = min(D(i-1,j) + 1, // deletion
                   D(i,j-1) + 1, // insertion
                   D(i-1,j-1) + cost); // substitution
    }
  }
  double d = (double) D(m,n) / std::max(m, n);
  wxLogMessage( _T("LevenshteinDistance('%s', '%s') = %g"), s.c_str(), t.c_str(), d );
  return d;
#undef D
}


/**
 * @brief Gets the closest match for s in a, using LevenshteinDistance.
 * @param distance If not NULL, *distance is set to the edit distance from s to the return value.
 */
static wxString GetBestMatch(const wxArrayString& a, const wxString& s, double* distance = NULL)
{
  const unsigned int count = a.GetCount();
  double minDistance = 1.0;
  int minDistanceIndex = -1;
  for (unsigned int i = 0; i < count; ++i) {
    const double distance = LevenshteinDistance(a[i], s);
    if (distance < minDistance) {
      minDistance = distance;
      minDistanceIndex = i;
    }
  }
  if (distance != NULL) *distance = minDistance;
  if (minDistanceIndex == -1) return _T("");
  return a[minDistanceIndex];
}


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
    m_lastActionTime = currentTime;
  }
  else if ( msg == _T("!ring") ) {
    m_battle.RingNotReadyPlayers();
    m_battle.DoAction( _T( "is ringing players not ready ..." ) );
    m_lastActionTime = currentTime;
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
    m_lastActionTime = currentTime;
  }
  else if ( msg.BeforeFirst( _T(' ') ) == _T("!loadprofile") ) {
    wxString profilename = GetBestMatch( m_battle.GetPresetList(), msg.AfterFirst(_T(' ')) );
    if ( !m_battle.LoadOptionsPreset( profilename ) )
      m_battle.DoAction( _T( "Profile not found, use !listprofiles for a list of available profiles." ) );
    else m_battle.DoAction( _T("has loaded profile: ") + profilename );
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
  else if ( msg == _T("!fixids") ) {
    m_battle.FixTeamIDs();
    m_battle.DoAction( _T( "is making control teams unique." ) );
    m_lastActionTime = currentTime;
  }
  else if ( msg == _T("!spectunsynced") ) {
    m_battle.ForceUnsyncedToSpectate();
    m_battle.DoAction( _T( "is forcing unsynced players to be spectators." ) );
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
    return;
  }

  m_battle.DoAction(_T("is starting game ..."));
  m_battle.GetServer().StartHostedBattle();

  // todo: copied from Ui::StartHostedBattle
  sett().SetLastHostMap( m_battle.GetHostMapName() );
  sett().SaveSettings();
}


wxString AutoHost::GetExtraCommandLineParams()
{
  if (m_enabled) {
    // -m, --minimise          Start minimised
    // -q [T], --quit=[T]      Quit immediately on game over or after T seconds
    #ifndef __WXMSW__
    return _T("--minimise --quit=1000000000");
    #else
    return _T("/minimise /quit=1000000000");
    #endif
  }
  else
    return wxEmptyString;
}
