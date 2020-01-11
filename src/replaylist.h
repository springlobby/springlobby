/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_REPLAYLIST_H_INCLUDED
#define SPRINGLOBBY_REPLAYLIST_H_INCLUDED

#include <wx/string.h>

#include "iplaybacklist.h"

/*
copied from spring sources for reference
struct DemoFileHeader {
	char magic[16];         ///< DEMOFILE_MAGIC
	int version;            ///< DEMOFILE_VERSION
	int headerSize;         ///< Size of the DemoFileHeader, minor version number.
	char versionString[16]; ///< Spring version string, e.g. "0.75b2", "0.75b2+svn4123"
	Uint8 gameID[16];       ///< Unique game identifier. Identical for each player of the game.
	Uint64 unixTime;        ///< Unix time when game was started.
	int scriptSize;         ///< Size of startscript.
	int demoStreamSize;     ///< Size of the demo stream.
	int gameTime;           ///< Total number of seconds game time.
	int wallclockTime;      ///< Total number of seconds wallclock time.
	int maxPlayerNum;       ///< Maximum player number which was used in this game.
	int numPlayers;         ///< Number of players for which stats are saved.
	int playerStatSize;     ///< Size of the entire player statistics chunk.
	int playerStatElemSize; ///< sizeof(CPlayer::Statistics)
	int numTeams;           ///< Number of teams for which stats are saved.
	int teamStatSize;       ///< Size of the entire team statistics chunk.
	int teamStatElemSize;   ///< sizeof(CTeam::Statistics)
	int teamStatPeriod;     ///< Interval (in seconds) between team stats.
	int winningAllyTeam;    ///< The ally team that won the game, -1 if unknown.
};
*/

struct StoredGame;
class PlayBackDataReader;
class wxInputStream;

class ReplayList : public IPlaybackList
{
public:
	ReplayList();

private:
	bool GetReplayInfos(const std::string& ReplayPath, StoredGame& ret) const override;
	int replayVersion(PlayBackDataReader& replay) const;

	std::string GetEngineVersionFromReplay(PlayBackDataReader& replay, const int version) const;
	std::string GetScriptFromReplay(PlayBackDataReader& replay, const int version) const;
	uint64_t GetStartTimeStampFromReplay(PlayBackDataReader& replay, const int version) const;
	//! saves relevant infos from header into replay struct
	void GetHeaderInfo(PlayBackDataReader& replay, StoredGame& rep, const int version) const;
};

IPlaybackList& replaylist();

#endif // SPRINGLOBBY_REPLAYLIST_H_INCLUDED
