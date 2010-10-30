#ifndef SPRINGLOBBY_REPLAYLIST_H_INCLUDED
#define SPRINGLOBBY_REPLAYLIST_H_INCLUDED


#include <wx/string.h>
#include <wx/timer.h>
#include <wx/arrstr.h>

#include "playbacklist.h"
#include "playbacktraits.h"
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

struct Replay;

template <class PB, class I>
class GlobalObjectHolder;

class ReplayList : public PlaybackList<Replay>
{
  public:

    typedef Replay
        PlaybackType;

   virtual  void LoadPlaybacks( const wxArrayString& filenames );

  protected:
    ReplayList();

    template <class PB, class T>
    friend class GlobalObjectHolder;

	bool GetReplayInfos ( const wxString& ReplayPath, Replay& ret ) const;
	wxString GetScriptFromReplay ( const wxString& ReplayPath ) const;
	BattleOptions GetBattleOptsFromScript( const wxString& script_ ) const;

    //! load mod/map options
    void LoadMMOpts( const wxString& sectionname, IBattle& battle, const PDataList& node );
    //! load engine options
    void LoadMMOpts( IBattle& battle, const PDataList& node );

    //! saves relevant infos from header into replay struct
	void GetHeaderInfo( Replay& rep, const wxString& ReplayPath ) const;

};

#endif // SPRINGLOBBY_REPLAYLIST_H_INCLUDED

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

