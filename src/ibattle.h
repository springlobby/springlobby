/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_IBATTLE_H
#define SPRINGLOBBY_HEADERGUARD_IBATTLE_H


/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

TODO: remove all wx stuff & merge / move to

lsl/battle/ibattle.h

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/

#include <lslunitsync/data.h>
#include <lsl/battle/tdfcontainer.h>

#include "user.h"
#include "userlist.h"
#include <lslunitsync/optionswrapper.h>

const unsigned int DEFAULT_SERVER_PORT = 8452;
const unsigned int DEFAULT_EXTERNAL_UDP_SOURCE_PORT = 16941;

class IBattle;
class IServer;
class AutoHost;
class AutohostManager;
namespace LSL
{
class OptionsWrapper;
};

struct BattleStartRect
{
	BattleStartRect()
	    : toadd(false)
	    , todelete(false)
	    , toresize(false)
	    , exist(false)
	    , ally(-1)
	    , top(-1)
	    , left(-1)
	    , right(-1)
	    , bottom(-1)
	{
	}

	bool toadd;
	bool todelete;
	bool toresize;
	bool exist;

	int ally;
	int top;
	int left;
	int right;
	int bottom;

	bool IsOk() const
	{
		return exist && !todelete;
	}
};


enum NatType {
	NAT_None = 0,
	NAT_Hole_punching,
	NAT_Fixed_source_ports
};


enum BattleType {
	BT_Played,
	BT_Replay,
	BT_Savegame
};


struct BattleOptions
{
	BattleOptions()
	    : battleid(-1)
	    , islocked(false)
	    , battletype(BT_Played)
	    , ispassworded(false)
	    , rankneeded(0)
	    , userelayhost(false)
	    , lockexternalbalancechanges(false)
	    , nattype(NAT_None)
	    , port(DEFAULT_SERVER_PORT)
	    , externaludpsourceport(DEFAULT_EXTERNAL_UDP_SOURCE_PORT)
	    , internaludpsourceport(DEFAULT_EXTERNAL_UDP_SOURCE_PORT)
	    , maxplayers(0)
	    , spectators(0)
	    , guilistactiv(false)
	{
	}

	int battleid;
	bool islocked;
	BattleType battletype;
	bool ispassworded;
	int rankneeded;
	std::string proxyhost;
	bool userelayhost;
	bool lockexternalbalancechanges;

	std::string founder;

	NatType nattype;
	unsigned int port;
	std::string ip;
	unsigned int externaludpsourceport;
	unsigned int internaludpsourceport;

	unsigned int maxplayers;
	unsigned int spectators;
	std::string relayhost;
	std::string maphash;
	std::string gamehash;

	std::string description;
	std::string engineVersion;
	std::string engineName;
	std::string mapname;
	std::string gamename;

	bool guilistactiv;
};

class IBattle : public UserList //, private SL::NonCopyable
{
public:
	IBattle();
	virtual ~IBattle();

	/** @name Constants
	 * @{
	 */
	enum HostInfo {
		HI_None = 0,
		HI_Map = 1,
		HI_Locked = 2,
		HI_Spectators = 4,
		HI_StartResources = 8,
		HI_MaxUnits = 16,
		HI_StartType = 32,
		HI_GameType = 64,
		HI_Options = 128,
		HI_StartRects = 256,
		HI_Restrictions = 512,
		HI_Map_Changed = 1024,
		HI_Game_Changed = 2048,
		HI_User_Positions = 4096,
		HI_Send_All_opts = 8192
	};

	/**@}*/

	/** @name Enums
	 * @{
	 */


	enum BalanceType {
		balance_divide,
		balance_random
	};

	enum StartType {
		ST_Fixed = 0,
		ST_Random = 1,
		ST_Choose = 2,
		ST_Pick = 3
	};

	enum GameType {
		GT_ComContinue = 0,
		GT_ComEnds = 1,
		GT_Lineage = 2
	};


	struct TeamInfoContainer
	{
		bool exist;
		int TeamLeader;
		int StartPosX;
		int StartPosY;
		int AllyTeam;
		LSL::lslColor RGBColor;
		std::string SideName;
		int Handicap;
		int SideNum;
	};

	struct AllyInfoContainer
	{
		bool exist;
		int NumAllies;
		int StartRectLeft;
		int StartRectTop;
		int StartRectRight;
		int StartRectBottom;
	};


	/**@}*/

	virtual void SetHostMap(const std::string& mapname, const std::string& hash);
	virtual void SetLocalMap(const std::string& mapname);
	virtual const LSL::UnitsyncMap& LoadMap();
	virtual const std::string& GetHostMapName() const;
	virtual const std::string& GetHostMapHash() const;

	virtual void SetProxy(const std::string& proxyhost);
	virtual const std::string& GetProxy() const;
	virtual bool IsProxy() const;

	virtual bool IsSynced(); //cannot be const

	virtual bool IsFounderMe() const;
	virtual bool IsFounder(const User& user) const;

	virtual void SetHostGame(const std::string& gamename, const std::string& hash);
	virtual void SetLocalGame(const LSL::UnitsyncGame& game);
	virtual const LSL::UnitsyncGame& LoadGame();
	virtual const std::string& GetHostGameName() const;
	virtual const std::string& GetHostGameHash() const;

	virtual bool MapExists(bool comparehash = true) const;
	virtual bool GameExists(bool comparehash = true) const;

	virtual BattleStartRect GetStartRect(unsigned int allyno) const;
	virtual User& OnUserAdded(User& user);
	virtual void OnUserBattleStatusUpdated(User& user, UserBattleStatus status);
	virtual void OnUserRemoved(User& user);

	virtual bool IsEveryoneReady() const;

	virtual void ForceSide(User& user, int side);
	virtual void ForceAlly(User& user, int ally);
	virtual void ForceTeam(User& user, int team);
	virtual void ForceColour(User& user, const LSL::lslColor& col);
	virtual void ForceSpectator(User& user, bool spectator);
	virtual void SetHandicap(User& user, int handicap);
	virtual void KickPlayer(User& user);


	virtual void AddStartRect(unsigned int allyno, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom);
	virtual void RemoveStartRect(unsigned int allyno);
	virtual void ResizeStartRect(unsigned int allyno);
	virtual void StartRectRemoved(unsigned int allyno);
	virtual void StartRectResized(unsigned int allyno);
	virtual void StartRectAdded(unsigned int allyno);
	virtual void ClearStartRects();
	virtual unsigned int GetNumRects() const;
	virtual unsigned int GetLastRectIdx() const;
	virtual unsigned int GetNextFreeRectIdx() const;

	virtual int GetFreeTeam(bool excludeme = false) const;

	virtual User& GetMe() = 0;
	virtual const User& GetMe() const = 0;

	virtual void SendHostInfo(HostInfo /*update*/)
	{
	}
	virtual void SendHostInfo(const std::string& /*Tag*/)
	{
	}

	virtual void OnPlayerTrueskillChanged(const std::string& /*NickName*/, double /*TrueSkill value*/)
	{
	}

	virtual unsigned int GetNumBots() const;
	virtual User& OnBotAdded(const std::string& nick, const UserBattleStatus& bs);

	virtual UserPosition GetFreePosition();
	virtual int GetFreeAlly(bool excludeme = false) const;

	virtual void RestrictUnit(const std::string& unitname, int count = 0);
	virtual void UnrestrictUnit(const std::string& unitname);
	virtual void UnrestrictAllUnits();
	virtual const std::map<std::string, int>& GetRestrictedUnits() const;

	virtual LSL::OptionsWrapper& CustomBattleOptions()
	{
		return m_opt_wrap;
	}
	virtual const LSL::OptionsWrapper& CustomBattleOptions() const
	{
		return m_opt_wrap;
	}

	virtual bool LoadOptionsPreset(const std::string& name);
	virtual void SaveOptionsPreset(const std::string& name);
	virtual const std::string& GetCurrentPreset() const;
	virtual void DeletePreset(const std::string& name);
	virtual LSL::StringVector GetPresetList();

	virtual const std::vector<LSL::lslColor>& GetFixColoursPalette(int numteams) const;
	virtual int GetClosestFixColour(const LSL::lslColor& col, const std::vector<int>& excludes, int difference) const;
	virtual LSL::lslColor GetFixColour(int i) const;
	virtual LSL::lslColor GetFreeColour(User& for_whom) const;
	virtual LSL::lslColor GetFreeColour(User* for_whom = NULL) const;
	virtual LSL::lslColor GetNewColour() const;
	virtual int ColourDifference(const LSL::lslColor& a, const LSL::lslColor& b) const;

	virtual User& GetFounder() const;

	virtual bool IsFull() const
	{
		return GetMaxPlayers() == GetNumActivePlayers();
	}

	virtual unsigned int GetNumPlayers() const;
	virtual unsigned int GetNumActivePlayers() const;

	virtual unsigned int GetNumReadyPlayers() const
	{
		return m_players_ready;
	}
	virtual unsigned int GetNumSyncedPlayers() const
	{
		return m_players_sync;
	}
	virtual unsigned int GetNumOkPlayers() const
	{
		return m_players_ok;
	}

	virtual int GetBattleId() const
	{
		return m_opts.battleid;
	}

	virtual bool GetGUIListActiv() const
	{
		return m_opts.guilistactiv;
	}
	virtual void SetGUIListActiv(bool Activ)
	{
		m_opts.guilistactiv = Activ;
	}

	virtual void SetInGame(bool ingame);
	virtual bool GetInGame() const
	{
		return m_ingame;
	}

	virtual void SetBattleType(BattleType type)
	{
		m_opts.battletype = type;
	}
	virtual const BattleType& GetBattleType() const
	{
		return m_opts.battletype;
	}

	virtual void SetIsLocked(const bool islocked)
	{
		m_opts.islocked = islocked;
	}
	virtual bool IsLocked() const
	{
		return m_opts.islocked;
	}
	virtual void SetIsPassworded(const bool ispassworded)
	{
		m_opts.ispassworded = ispassworded;
	}
	virtual bool IsPassworded() const
	{
		return m_opts.ispassworded;
	}

	virtual void SetNatType(const NatType nattype)
	{
		m_opts.nattype = nattype;
	}
	virtual const NatType& GetNatType() const
	{
		return m_opts.nattype;
	}
	virtual void SetHostPort(unsigned int port)
	{
		m_opts.port = port;
	}

	virtual void SetMyExternalUdpSourcePort(unsigned int port)
	{
		m_opts.externaludpsourceport = port;
	}
	virtual unsigned int GetMyExternalUdpSourcePort()
	{
		return m_opts.externaludpsourceport;
	}

	virtual void SetMyInternalUdpSourcePort(unsigned int port)
	{
		m_opts.internaludpsourceport = port;
	}
	virtual unsigned int GetMyInternalUdpSourcePort()
	{
		return m_opts.internaludpsourceport;
	}

	virtual int GetHostPort() const
	{
		return m_opts.port;
	}
	virtual void SetFounder(const std::string& nick)
	{
		m_opts.founder = nick;
	}
	virtual void SetHostIp(const std::string& ip)
	{
		m_opts.ip = ip;
	}
	virtual const std::string& GetHostIp() const
	{
		return m_opts.ip;
	}

	virtual void SetMaxPlayers(const int& maxplayers)
	{
		m_opts.maxplayers = maxplayers;
	}
	virtual unsigned int GetMaxPlayers() const
	{
		return m_opts.maxplayers;
	}
	virtual void SetSpectators(const int& spectators)
	{
		m_opts.spectators = spectators;
	}
	virtual int GetSpectators() const
	{
		return m_opts.spectators;
	}

	virtual void SetRankNeeded(const int& rankneeded)
	{
		m_opts.rankneeded = rankneeded;
	}
	virtual int GetRankNeeded() const
	{
		return m_opts.rankneeded;
	}

	// virtual void SetMapHash( const std::string& maphash ) { m_opts.maphash = maphash; }
	// virtual void SetMapname( const std::string& map ) { m_opts.mapname = map; }
	virtual void SetDescription(const std::string& desc)
	{
		m_opts.description = desc;
	}
	virtual const std::string& GetDescription() const
	{
		return m_opts.description;
	}
	// virtual void SetGamename( const std::string& game ) { m_opts.gamename = game; }


	typedef std::map<std::string, User> UserVec;
	typedef UserVec::const_iterator UserVecCIter;
	typedef UserVec::iterator UserVecIter;

	virtual void SetBattleOptions(const BattleOptions& options)
	{
		m_opts = options;
	}

	virtual void OnSelfLeftBattle();

	/// replay&savegame parsing stuff
	typedef std::map<int, TeamInfoContainer> TeamVec;
	typedef TeamVec::const_iterator TeamVecCIter;
	typedef TeamVec::iterator TeamVecIter;

	typedef std::map<int, AllyInfoContainer> AllyVec;
	typedef AllyVec::const_iterator AllyVecCIter;
	typedef AllyVec::iterator AllyVecIter;

	virtual TeamVec GetParsedTeamsVec()
	{
		return m_parsed_teams;
	}
	virtual AllyVec GetParsedAlliesVec()
	{
		return m_parsed_allies;
	}

	virtual void SetParsedTeamsVec(const TeamVec& t)
	{
		m_parsed_teams = t;
	}
	virtual void SetParsedAlliesVec(const AllyVec& a)
	{
		m_parsed_allies = a;
	}

	virtual const BattleOptions& GetBattleOptions() const
	{
		return m_opts;
	}

	virtual bool Equals(const IBattle& other) const
	{
		return m_opts.battleid == other.GetBattleOptions().battleid;
	}

	virtual void DisableHostStatusInProxyMode(bool value)
	{
		m_generating_script = value;
	}

	virtual void UserPositionChanged(const User& usr);

	virtual void SetScript(const std::string& script)
	{
		m_script = script;
	}
	virtual void AppendScriptLine(const std::string& line)
	{
		m_script += line;
	}
	virtual void ClearScript()
	{
		m_script.clear();
	}
	virtual const std::string& GetScript() const
	{
		return m_script;
	}

	virtual void SetPlayBackFilePath(const std::string& path);
	virtual const std::string& GetPlayBackFilePath() const
	{
		return m_playback_file_path;
	}
	virtual const std::string& GetPlayBackFileName() const
	{
		return m_playback_file_name;
	}

	virtual void AddUserFromDemo(User& user);

	void GetBattleFromScript(bool loadmapmod);

	virtual bool ShouldAutoStart() const;

	virtual void StartSpring() = 0;

	virtual const std::map<int, int>& GetAllySizes() const
	{
		return m_ally_sizes;
	}
	virtual const std::map<int, int>& GetTeamSizes() const
	{
		return m_teams_sizes;
	}

	//Extra tags for numerious battle options
	std::map<std::string, std::string> m_script_tags;

	virtual long GetBattleRunningTime() const; // returns 0 if not started

	virtual bool EngineExists() const;

	virtual void SetEngineName(const std::string& name)
	{
		m_opts.engineName = name;
	}
	virtual void SetEngineVersion(const std::string& version)
	{
		m_opts.engineVersion = version;
	}
	virtual const std::string& GetEngineName() const
	{
		return m_opts.engineName;
	}
	virtual const std::string& GetEngineVersion() const
	{
		return m_opts.engineVersion;
	}
	virtual bool ExecuteSayCommand(const std::string& /*line*/)
	{
		return true;
	}
	virtual int GetID() const
	{
		return 0;
	}
	virtual void Join(const std::string& /*password*/ = "")
	{
	}
	virtual IServer& GetServer();
	virtual void Say(const std::string& /*msg*/)
	{
	}
	virtual bool CheckBan(User& /*user*/)
	{
		return false;
	}
	virtual bool GetAutoLockOnStart()
	{
		return false;
	}
	virtual void LoadMapDefaults(const std::string&)
	{
	}
	virtual void SendMyBattleStatus()
	{
	}
	virtual AutoHost* GetAutoHost()
	{
		return NULL;
	}
	virtual void Autobalance(BalanceType /*balance_type*/, bool /*support_clans*/, bool /*strong_clans*/, int /*numallyteams*/)
	{
	}
	virtual void DoAction(const std::string& /*action*/)
	{
	}
	virtual void RingPlayer(const User& /*user*/)
	{
	}
	virtual void RingNotReadyPlayers()
	{
	}
	virtual void FixColours()
	{
	}
	virtual void FixTeamIDs(BalanceType /*balance_type*/ = balance_divide, bool /*clans*/ = true, bool /*strong_clans*/ = true, int /*controlteamsize*/ = 0)
	{
	}
	virtual void ForceUnsyncedToSpectate()
	{
	}
	virtual void StartHostedBattle()
	{
	}
	virtual void SaveMapDefaults()
	{
	}
	virtual void OnRequestBattleStatus()
	{
	}
	virtual void Leave()
	{
	}
	virtual bool GetAutoUnspec()
	{
		return false;
	}
	virtual void SetAutoUnspec(bool /*value*/)
	{
	}
	virtual void SetImReady(bool /*ready*/)
	{
	}
	virtual void SetAutolaunchGame(bool /*autolaunch*/)
	{
	}
	virtual void SetLastAutolockStatus(bool /*autolock*/)
	{
	}
	virtual void SetLockExternalBalanceChanges(bool /*block*/)
	{
	}
	virtual void ForceUnReadyToSpectate()
	{
	}
	virtual void ForceUnsyncedAndUnreadyToSpectate()
	{
	}
	virtual void RingNotSyncedPlayers()
	{
	}
	virtual void RingNotSyncedAndNotReadyPlayers()
	{
	}
	virtual void SetAutoLockOnStart(bool /*autolock*/)
	{
	}
	void RemoveUnfittingBots();
	AutohostManager* m_autohost_manager;

protected:
	BattleOptions m_opts;
	bool m_generating_script;
	std::map<int, int> m_teams_sizes; // controlteam -> number of people in
	bool m_is_self_in;
	std::map<std::string, time_t> m_ready_up_map; // player name -> time counting from join/unspect

private:
	void LoadScriptMMOpts(const std::string& sectionname, const LSL::TDF::PDataList& node);
	void LoadScriptMMOpts(const LSL::TDF::PDataList& node);

	void PlayerLeftTeam(int team);
	void PlayerLeftAlly(int ally);
	void PlayerJoinedTeam(int team);
	void PlayerJoinedAlly(int ally);

	bool m_ingame;
	bool m_map_loaded;
	bool m_game_loaded;
	LSL::UnitsyncMap m_local_map;
	LSL::UnitsyncMap m_host_map;
	LSL::UnitsyncGame m_host_game;
	LSL::UnitsyncGame m_local_game;

	std::map<std::string, int> m_restricted_units;
	std::string m_previous_local_game_name;

	LSL::OptionsWrapper m_opt_wrap;

	std::map<unsigned int, BattleStartRect> m_rects;

	unsigned int m_players_ready;
	unsigned int m_players_sync;
	unsigned int m_players_ok;       // players which are ready and in sync
	std::map<int, int> m_ally_sizes; // allyteam -> number of people in

	std::string m_preset;

	UserVec m_internal_bot_list;

	/// replay&savegame stuff
	std::string m_script;
	std::string m_playback_file_name;
	std::string m_playback_file_path;

	TeamVec m_parsed_teams;
	AllyVec m_parsed_allies;
	UserVec m_internal_user_list; /// to store users from savegame/replay

	long m_start_time;
};

#endif // SPRINGLOBBY_HEADERGUARD_IBATTLE_H
