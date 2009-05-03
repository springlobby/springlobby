#ifndef SPRINGLOBBY_REPLAYLIST_H_INCLUDED
#define SPRINGLOBBY_REPLAYLIST_H_INCLUDED

#include <map>
#include <wx/string.h>
#include <wx/timer.h>
#include <wx/arrstr.h>
#include "../tdfcontainer.h"
#include "../offlinebattle.h"
#include "playbackstructs.h"
#include "../globalsmanager.h"

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


template <class PlaybackImp>
class PlaybackList : public wxEvtHandler
{
  public:
    typedef PlaybackImp
        PlaybackType;

    typedef unsigned int playback_id_t;

    //! @brief mapping from playback id number to playback object
    typedef std::map<playback_id_t, PlaybackType> playback_map_t;
    //! @brief iterator for playback map
    typedef typename playback_map_t::iterator playback_iter_t;
    //! @brief const iterator for playback map
    typedef typename playback_map_t::const_iterator playback_const_iter_t;

    void LoadReplays( const wxArrayString& filenames );
    //!loads replays between two indices
    void LoadReplays( const unsigned int from, const unsigned int to);

    PlaybackType& AddReplay( const PlaybackType& replay );
    void AddReplay( PlaybackType* replay );
    void RemoveReplay( playback_id_t const& id );

    PlaybackType &GetReplayById( playback_id_t const& id );

    ///Replay& GetReplay( int const index ) ;

    bool ReplayExists( playback_id_t const& id );
    bool DeleteReplay( playback_id_t const& id );
    typename playback_map_t::size_type GetNumReplays();

    void RemoveAll();

    const playback_map_t& GetReplaysMap() const;


  protected:
    PlaybackList() {};

    template <class PB>
    friend class GlobalObjectHolder;

    playback_map_t m_replays;

    unsigned long m_fails;

};

template <class PlaybackType>
PlaybackType& PlaybackList<PlaybackType>::AddReplay( const PlaybackType& replay )
{
    m_replays[replay.id] = replay;
    return m_replays[replay.id];
}

template <class PlaybackType>
void PlaybackList<PlaybackType>::RemoveReplay( playback_id_t const& id )
{
    m_replays.erase(id);
}

template <class PlaybackType>
typename PlaybackList<PlaybackType>::playback_map_t::size_type PlaybackList<PlaybackType>::GetNumReplays()
{
    return m_replays.size();
}

template <class PlaybackType>
PlaybackType& PlaybackList<PlaybackType>::GetReplayById( playback_id_t const& id )
{
//TODO catch
    playback_iter_t b = m_replays.find(id);
    if (b == m_replays.end())
        throw std::runtime_error("ReplayList_Iter::GetReplay(): no such replay");

    return b->second;
}

template <class PlaybackType>
bool PlaybackList<PlaybackType>::ReplayExists( playback_id_t const& id )
{
    return m_replays.find(id) != m_replays.end();
}

template <class PlaybackType>
bool PlaybackList<PlaybackType>::DeleteReplay( playback_id_t const& id )
{
    Replay rep = m_replays[id];
    if ( wxRemoveFile( rep.Filename ) ) {

        //m_filenames.resize(std::remove(m_filenames.begin(), m_filenames.end(), rep.Filename)-m_filenames.begin());

        m_replays.erase(id);
        return true;
    }
    return false;
}

template <class PlaybackType>
void PlaybackList<PlaybackType>::RemoveAll()
{
//    m_filenames.clear();
    m_replays.clear();
    m_fails = 0;
}

template <class PlaybackType>
const typename PlaybackList<PlaybackType>::playback_map_t& PlaybackList<PlaybackType>::GetReplaysMap() const {
    return m_replays;
}


class ReplayList : public PlaybackList<Replay>
{
  public:
    ReplayList();

    void LoadReplays( const wxArrayString& filenames );
    //!loads replays between two indices
    void LoadReplays( const unsigned int from, const unsigned int to);

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

};

template <class PlaybackImp>
PlaybackList<PlaybackImp>& playbacklist()
{
    static GlobalObjectHolder<PlaybackList<PlaybackImp> > m_replay_list;
    return m_replay_list;
}

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

