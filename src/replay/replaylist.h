#ifndef SPRINGLOBBY_REPLAYLIST_H_INCLUDED
#define SPRINGLOBBY_REPLAYLIST_H_INCLUDED

#include <map>
#include <wx/string.h>
#include <wx/timer.h>
#include <wx/arrstr.h>
#include "../tdfcontainer.h"
#include "../offlinebattle.h"

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
    int playernum;
    bool can_watch;
    int duration; //in seconds
    int size; //in bytes
    wxString MapName;
    wxString ModName;
    wxString SpringVersion;
    wxString Filename;
    wxString date;
    OfflineBattle battle;
    Replay():id(0),playernum(0),can_watch(false),duration(0),size(0){};

    bool Equals( const Replay& other ) const { return Filename == other.Filename; }
};


typedef unsigned int replay_id_t;

//! @brief mapping from replay id number to replay object
typedef std::map<replay_id_t, Replay> replay_map_t;
//! @brief iterator for replay map
typedef replay_map_t::iterator replay_iter_t;
//! @brief const iterator for replay map
typedef replay_map_t::const_iterator replay_const_iter_t;

class ReplayTab;

class ReplayList : public wxEvtHandler
{
  public:
    ReplayList();

    void LoadReplays();
    //!loads replays between two indices
    void LoadReplays( const unsigned int from, const unsigned int to);

    void AddReplay( const Replay& replay );
    void RemoveReplay( replay_id_t const& id );

    Replay &GetReplayById( replay_id_t const& id );

    ///Replay& GetReplay( int const index ) ;

    bool ReplayExists( replay_id_t const& id );
    bool DeleteReplay( replay_id_t const& id );
    replay_map_t::size_type GetNumReplays();

    void RemoveAll();

    const replay_map_t& GetReplaysMap() const;

  protected:

    bool GetReplayInfos ( const wxString& ReplayPath, Replay& ret );
    void GetScriptFromReplay ( const wxString& ReplayPath, wxString& script );
    void GetBattleFromScript( const wxString& script, OfflineBattle& battle, bool loadmod );
    BattleOptions GetBattleOptsFromScript( const wxString& script_ );

    //! load mod/map options
    void LoadMMOpts( const wxString& sectionname, OfflineBattle& battle, const PDataList& node );
    //! load engine options
    void LoadMMOpts( OfflineBattle& battle, const PDataList& node );

    //! saves relevant infos from header into replay struct
    void GetHeaderInfo( Replay& rep, const wxString& ReplayPath );

    replay_map_t m_replays;

    std::vector<wxString> m_filenames;
    unsigned long m_fails;

};


#endif // SPRINGLOBBY_REPLAYLIST_H_INCLUDED

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

