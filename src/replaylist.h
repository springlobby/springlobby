#ifndef SPRINGLOBBY_REPLAYLIST_H_INCLUDED
#define SPRINGLOBBY_REPLAYLIST_H_INCLUDED

#include <map>
#include <wx/string.h>
#include <wx/arrstr.h>
#include "battle.h"
#include "tdfcontainer.h"

//copied from spring sources for reference
//struct DemoFileHeader
//{
//	char magic[16];         ///< DEMOFILE_MAGIC
//	int version;            ///< DEMOFILE_VERSION
//	int headerSize;         ///< Size of the DemoFileHeader, minor version number.
//	char versionString[16]; ///< Spring version string, e.g. "0.75b2", "0.75b2+svn4123"
//	Uint8 gameID[16];       ///< Unique game identifier. Identical for each player of the game.
//	Uint64 unixTime;        ///< Unix time when game was started.
//	int scriptSize;         ///< Size of startscript.
//	int demoStreamSize;     ///< Size of the demo stream.
//	int gameTime;           ///< Total number of seconds game time.
//	int wallclockTime;      ///< Total number of seconds wallclock time.
//	int maxPlayerNum;       ///< Maximum player number which was used in this game.
//	int numPlayers;         ///< Number of players for which stats are saved.
//	int playerStatSize;     ///< Size of the entire player statistics chunk.
//	int playerStatElemSize; ///< sizeof(CPlayer::Statistics)
//	int numTeams;           ///< Number of teams for which stats are saved.
//	int teamStatSize;       ///< Size of the entire team statistics chunk.
//	int teamStatElemSize;   ///< sizeof(CTeam::Statistics)
//	int teamStatPeriod;     ///< Interval (in seconds) between team stats.
//	int winningAllyTeam;    ///< The ally team that won the game, -1 if unknown.
//};

struct Replay
{
    int id;
    long int day;
    long int month;
    long int year;
    int playernum;
    bool can_watch;
    int duration; //in seconds
    wxString MapName;
    wxString ModName;
    wxString SpringVersion;
    wxString ReplayName;
    wxString Filename;
    wxString date;
    OfflineBattle battle;
    Replay():id(0),day(0),year(0),playernum(0),can_watch(false),duration(0){};
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

bool GetReplayInfos ( const wxString& ReplayPath, Replay& ret );
wxString GetScriptFromReplay ( const wxString& ReplayPath );
OfflineBattle GetBattleFromScript( const wxString& script );
BattleOptions GetBattleOptsFromScript( const wxString& script_ );

// load mod/map options
void LoadMMOpts( const wxString& sectionname, OfflineBattle& battle, const PDataList& node );
//load engine options
void LoadMMOpts( OfflineBattle& battle, const PDataList& node );

//saves relevant infos from header into replay struct
void GetHeaderInfo( Replay& rep, const wxString& ReplayPath );

#endif // SPRINGLOBBY_REPLAYLIST_H_INCLUDED
