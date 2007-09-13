#ifndef SPRINGLOBBY_HEADERGUARD_BATTLE_H
#define SPRINGLOBBY_HEADERGUARD_BATTLE_H

#include <vector>
#include <list>

#include "userlist.h"
#include "user.h"
#include "ibattle.h"


class Ui;
class Server;


enum NatType {
  NAT_None = 0,
  NAT_Hole_punching,
  NAT_Fixed_source_ports
};


#define DEFAULT_SERVER_PORT 8034


struct BattleOptions
{
  BattleOptions() :
    battleid(-1),islocked(false),isreplay(false),ispassworded(false),rankneeded(0),
    nattype(NAT_None),port(DEFAULT_SERVER_PORT),maxplayers(0),spectators(0),
    startmetal(1000),startenergy(1000),maxunits(500),starttype(ST_Fixed),
    gametype(GT_ComContinue),limitdgun(false),dimmms(false),ghostedbuildings(true) {}

  int battleid;
  bool islocked;
  bool isreplay;
  bool ispassworded;
  int rankneeded;
  std::string founder;

  NatType nattype;
  int port;
  std::string ip;

  int maxplayers;
  int spectators;

  int startmetal;
  int startenergy;
  int maxunits;
  StartType starttype;

  GameType gametype;
  bool limitdgun;
  bool dimmms;
  bool ghostedbuildings;

  std::string maphash;
  std::string modhash;

  std::string description;
  std::string mapname;
  std::string modname;

};


class Battle : public UserList, public IBattle
{
  public:
    Battle( Server& serv, Ui& ui, int id );
    ~Battle();

    //const BattleOptions& opts() { return m_opts; }

    Server& GetServer();

    int GetBattleId() { return m_opts.battleid; }

    void SetInGame( bool ingame ) { m_ingame = ingame; }
    bool GetInGame() { return m_ingame; }

    void SetIsReplay( const bool& isreplay ) { m_opts.isreplay = isreplay; }
    void SetIsLocked( const bool& islocked ) { m_opts.islocked = islocked; }
    bool IsLocked() { return m_opts.islocked; }
    void SetIsPassworded( const bool& ispassworded ) { m_opts.ispassworded = ispassworded; }
    bool IsPassworded() { return m_opts.ispassworded; }

    void SetNatType( const NatType nattype ) { m_opts.nattype = nattype; }
    NatType GetNatType() { return m_opts.nattype; }
    void SetHostPort( int port) { m_opts.port = port; }
    int GetHostPort() { return m_opts.port; }
    void SetFounder( const std::string& nick ) { m_opts.founder = nick; }
    void SetHostIp( const std::string& ip ) { m_opts.ip = ip; }
    std::string GetHostIp() { return m_opts.ip; }

    void SetMaxPlayers( const int& maxplayers ) { m_opts.maxplayers = maxplayers; }
    int GetMaxPlayers() { return m_opts.maxplayers; }
    void SetSpectators( const int& spectators ) { m_opts.spectators = spectators; }
    int GetSpectators() { return m_opts.spectators; }

/*    void SetStartMetal( const int& smetal ) { m_opts.startmetal = smetal; }
    int GetStartMetal() { return m_opts.startmetal; }
    void SetStartEnergy( const int& senergy ) { m_opts.startenergy = senergy; }
    int GetStartEnergy() { return m_opts.startenergy; }
    void SetMaxUnits( const int& maxunits ) { m_opts.maxunits = maxunits; }
    int GetMaxUnits() { return m_opts.maxunits; }
    //void SetStartType( const StartType& starttype ) { m_opts.starttype = starttype; }

    //void SetGameType( const GameType& gt ) { m_opts.gametype = gt; }
    void SetLimitDGun( const bool& limdgun ) { m_opts.limitdgun = limdgun; }
    bool LimitDGun() { return m_opts.limitdgun; }
    void SetDimMMs( const bool& dimmm ) { m_opts.dimmms = dimmm; }
    bool DimMMs() { return m_opts.dimmms; }
    void SetGhostedBuildings( const bool& gbuilds ) { m_opts.ghostedbuildings = gbuilds; }
    bool GhostedBuildings() { return m_opts.ghostedbuildings; }
*/

    void SendHostInfo( HostInfo update );

    void SetRankNeeded( const int& rankneeded ) { m_opts.rankneeded = rankneeded; }
    int GetRankNeeded() { return m_opts.rankneeded; }

    //void SetMapHash( const std::string& maphash ) { m_opts.maphash = maphash; }
    //void SetMapname( const std::string& map ) { m_opts.mapname = map; }
    void SetDescription( const std::string& desc ) { m_opts.description = desc; }
    std::string GetDescription() { return m_opts.description; }
    //void SetModname( const std::string& mod ) { m_opts.modname = mod; }

    void SetImReady( bool ready );

    User& GetFounder() { return GetUser( m_opts.founder ); }
    User& GetMe();
    bool IsFounderMe();

    int GetMyPlayerNum();

    int GetFreeTeamNum( bool excludeme = true );
    void GetFreeColour( int& r, int& g, int& b, bool excludeme = true );

    void Update();

    void Join( const std::string& password = "" );
    void Leave();

    void KickPlayer( User& user );

    bool IsEveryoneReady();
    void RingNotReadyPlayers();

    void Say( const std::string& msg );
    void DoAction( const std::string& msg );

    /*bool IsMapAvailable();
    bool IsModAvailable();*/

    bool HaveMultipleBotsInSameTeam();

    void OnRequestBattleStatus();
    void SendMyBattleStatus();

    bool IsSynced();
    //bool HasMod();

    bool ExecuteSayCommand( const std::string& cmd ) { return false; }

    void AddStartRect( int allyno, int left, int top, int right, int bottom );
    void RemoveStartRect( int allyno );
    void UpdateStartRect( int allyno );

    void AddBot( const std::string& nick, const std::string& owner, UserBattleStatus status, const std::string& aidll );
    void RemoveBot( const std::string& nick );
    void SetBotTeam( const std::string& nick, int team );
    void SetBotAlly( const std::string& nick, int ally );
    void SetBotSide( const std::string& nick, int side );
    void SetBotColour( const std::string& nick, int r, int g, int b );

    BattleBot* GetBot( const std::string& name );
    BattleBot* GetBot( unsigned int index );
    unsigned int GetNumBots();

    void StartRectRemoved( int allyno );
    void StartRectUpdated( int allyno );

    BattleStartRect* GetStartRect( int allyno );
    void ClearStartRects();

    void ForceSide( User& user, int side );
    void ForceTeam( User& user, int team );
    void ForceAlly( User& user, int ally );
    void ForceColour( User& user, int r, int g, int b );
    void ForceSpectator( User& user, bool spectator );
    void BattleKickPlayer( User& user );

    void OnUserAdded( User& user );
    void OnUserRemoved( User& user );

    void OnBotAdded( const std::string& nick, const std::string& owner, const UserBattleStatus& bs, const std::string& aidll );
    void OnBotRemoved( const std::string& nick );
    void OnBotUpdated( const std::string& name, const UserBattleStatus& bs );

    int GetMyAlly() { return GetMe().BattleStatus().ally; }
    void SetMyAlly( int ally ) { GetMe().BattleStatus().ally = ally; SendMyBattleStatus(); }

  protected:
    // Battle variables

    BattleOptions m_opts;
    Server& m_serv;
    Ui& m_ui;

    bool m_ingame;

    int m_order;

    std::vector<BattleStartRect*> m_rects;
    std::list<BattleBot*> m_bots;

    std::list<BattleBot*>::iterator m_bot_seek;
    std::list<BattleBot*>::size_type m_bot_pos;

    void RemoveUser( std::string const& user ) {}
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLE_H
