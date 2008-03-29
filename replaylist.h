#ifndef SPRINGLOBBY_REPLAYLIST_H_INCLUDED
#define SPRINGLOBBY_REPLAYLIST_H_INCLUDED

#include <map>
#include <wx/string.h>
#include <wx/arrstr.h>

struct Replay
{
  int id;
  long int day;
  long int month;
  long int year;
  int playernum;
  wxString MapName;
  wxString ModName;
  wxString SpringVersion;
  wxString ReplayName;
  wxString Filename;
  wxString date;
  wxArrayString playernames;

};

typedef unsigned int replay_id_t;

//! @brief mapping from replay id number to replay object
typedef std::map<replay_id_t, Replay> replay_map_t;
//! @brief iterator for replay map
typedef replay_map_t::iterator replay_iter_t;


class ReplayList
{
  public:
    ReplayList();

    void AddReplay( Replay replay );
    void RemoveReplay( replay_id_t const& id );

    Replay GetReplayById( replay_id_t const& id );
    Replay GetReplay( int const index ) ;

    bool ReplayExists( replay_id_t const& id );
    replay_map_t::size_type GetNumReplays();

  private:
    replay_map_t m_replays;
};

Replay GetReplayInfos ( wxString& ReplayPath );
wxString GetScriptFromReplay ( wxString& ReplayPath );

#endif // SPRINGLOBBY_REPLAYLIST_H_INCLUDED
