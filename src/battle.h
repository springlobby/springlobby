#ifndef SPRINGLOBBY_HEADERGUARD_BATTLE_H
#define SPRINGLOBBY_HEADERGUARD_BATTLE_H

#include <vector>
#include <list>

#include "userlist.h"
#include "user.h"

class Ui;
class Server;


enum NatType {
  NAT_None = 0,
  NAT_Hole_punching,
  NAT_Fixed_source_ports
};

enum StartType {
  ST_Fixed = 0,
  ST_Random,
  ST_Choose
};

enum GameType {
  GT_ComContinue = 0,
  GT_ComEnds = 1,
  GT_Lineage = 2
};


#define DEFAULT_SERVER_PORT 8034


struct BattleOptions
{
  BattleOptions() :
    battleid(-1),islocked(false),isreplay(false),ispassworded(false),rankneeded(0),
    nattype(NAT_None),port(DEFAULT_SERVER_PORT),maxplayers(0),spectators(0),
    startmetal(1000),startenergy(1000),maxunits(500),starttype(ST_Fixed),
    gametype(GT_ComContinue),limitdgun(false),dimmms(false),ghostedbuildings(true),maphash(""),
    modhash("") {}

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


struct BattleStartRect
{
  BattleStartRect() { local = true; updated = false; deleted = false; }
  bool local;
  bool updated;
  bool deleted;

  int ally;
  int top;
  int left;
  int right;
  int bottom;
};


struct BattleBot {
  UserBattleStatus bs;
  std::string name;
  std::string owner;
  std::string aidll;
};


class Battle : public UserList
{
  public:
    Battle( Server& serv, Ui& ui, int id );
    ~Battle();

    const BattleOptions& opts() { return m_opts; }

    void SetIsReplay( const bool& isreplay ) { m_opts.isreplay = isreplay; }
    void SetIsLocked( const bool& islocked ) { m_opts.islocked = islocked; }
    void SetIsPassworded( const bool& ispassworded ) { m_opts.ispassworded = ispassworded; }

    void SetNatType( const NatType nattype ) { m_opts.nattype = nattype; }
    void SetHostPort( int port) { m_opts.port = port; }
    void SetFounder( const std::string& nick ) { m_opts.founder = nick; }
    void SetIp( const std::string& ip ) { m_opts.ip = ip; }

    void SetMaxPlayers( const int& maxplayers ) { m_opts.maxplayers = maxplayers; }
    void SetSpectators( const int& spectators ) { m_opts.spectators = spectators; }

    void SetStartMetal( const int& smetal ) { m_opts.startmetal = smetal; }
    void SetStartEnergy( const int& senergy ) { m_opts.startenergy = senergy; }
    void SetMaxUnits( const int& maxunits ) { m_opts.maxunits = maxunits; }
    void SetStartType( const StartType& starttype ) { m_opts.starttype = starttype; }

    void SetGameType( const GameType& gt ) { m_opts.gametype = gt; }
    void SetLimitDGun( const bool& limdgun ) { m_opts.limitdgun = limdgun; }
    void SetDimMMs( const bool& dimmm ) { m_opts.dimmms = dimmm; }
    void SetGhostedBuildings( const bool& gbuilds ) { m_opts.ghostedbuildings = gbuilds; }

    void SetRankNeeded( const int& rankneeded ) { m_opts.rankneeded = rankneeded; }

    void SetMapHash( const std::string& maphash ) { m_opts.maphash = maphash; }
    void SetMapname( const std::string& map ) { m_opts.mapname = map; }
    void SetDescription( const std::string& desc ) { m_opts.description = desc; }
    void SetModname( const std::string& mod ) { m_opts.modname = mod; }

    void SetImReady( bool ready );

    User& GetFounder() { return GetUser( m_opts.founder ); }
    User& GetMe();
    bool IsFounderMe();

    int GetMyPlayerNum();

    int GetFreeTeamNum( bool excludeme = true );
    void GetFreeColour( int& r, int& g, int& b, bool excludeme = true );

    void AddUser( User& user );
    void RemoveUser( User& user );

    void Update();

    void Join( const std::string& password = "" );
    void Leave();

    void Say( const std::string& msg );

    bool IsMapAvailable();
    bool IsModAvailable();

    void OnRequestBattleStatus();
    void SendMyBattleStatus();

    bool IsSynced();
    bool HasMod();

    bool ExecuteSayCommand( const std::string& cmd ) { return false; }

    void AddStartRect( int allyno, int left, int top, int right, int bottom );
    void RemoveStartRect( int allyno );
    void UpdateStartRect( int allyno );

    void AddBot( const std::string& nick, const std::string& owner, UserBattleStatus status, const std::string& aidll );
    void RemoveBot( const std::string& nick );
    void SetBotTeam( const std::string& nick, int team );
    void SetBotAlly( const std::string& nick, int ally );
    void SetBotSide( const std::string& nick, int side );
    void SetBotColour( const std::string& nick, int Colour );

    BattleBot* GetBot( const std::string& name );
    BattleBot* GetBot( std::list<BattleBot*>::size_type index );
    std::list<BattleBot*>::size_type GetNumBots();

    void StartRectRemoved( int allyno );
    void StartRectUpdated( int allyno );

    BattleStartRect* GetStartRect( int allyno );
    void ClearStartRects();

    void DisableUnit( const std::string& unitname );
    void EnableUnit( const std::string& unitname );
    void EnableAllUnits();
    std::string DisabledUnits();
    int GetNumDisabledUnits() { return m_units_num; }

    void ForceTeam( User& user, int team );
    void ForceAlly( User& user, int ally );
    void ForceColour( User& user, int r, int g, int b );
    void ForceSpectator( User& user, bool spectator );
    void BattleKickPlayer( User& user );

    void OnBotAdded( const std::string& nick, const std::string& owner, const UserBattleStatus& bs, const std::string& aidll );
    void OnBotRemoved( const std::string& nick );
    void OnBotUpdated( const std::string& name, const UserBattleStatus& bs );

  protected:
    // Battle variables

    BattleOptions m_opts;
    Server& m_serv;
    Ui& m_ui;

    int m_order;

    std::vector<BattleStartRect*> m_rects;
    std::list<BattleBot*> m_bots;
    
    std::list<BattleBot*>::iterator m_bot_seek;
    std::list<BattleBot*>::size_type m_bot_pos;
    
    std::string m_units;
    int m_units_num;

    void RemoveUser( std::string const& user ) {}
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLE_H
