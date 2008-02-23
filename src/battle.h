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
#define DEFAULT_EXTERNAL_UDP_SOURCE_PORT 12345


struct BattleOptions
{
  BattleOptions() :
    battleid(-1),islocked(false),isreplay(false),ispassworded(false),rankneeded(0),
    nattype(NAT_None),port(DEFAULT_SERVER_PORT),externaludpsourceport(DEFAULT_EXTERNAL_UDP_SOURCE_PORT),maxplayers(0),spectators(0),
    guilistactiv(false) {}

  int battleid;
  bool islocked;
  bool isreplay;
  bool ispassworded;
  int rankneeded;
  wxString founder;

  NatType nattype;
  int port;
  wxString ip;
  int externaludpsourceport;

  unsigned int maxplayers;
  unsigned int spectators;

  wxString maphash;
  wxString modhash;

  wxString description;
  wxString mapname;
  wxString modname;

  bool guilistactiv;
};


class Battle : public UserList, public IBattle
{
  public:
    Battle( Server& serv, Ui& ui, int id );
    ~Battle();

    //const BattleOptions& opts() { return m_opts; }

    Server& GetServer();

    int GetBattleId() const { return m_opts.battleid; }

    bool GetGUIListActiv() const { return m_opts.guilistactiv; }
    void SetGUIListActiv(bool Activ) { m_opts.guilistactiv = Activ; }

    void SetInGame( bool ingame ) { m_ingame = ingame; }
    bool GetInGame() const { return m_ingame; }

    void SetIsReplay( const bool& isreplay ) { m_opts.isreplay = isreplay; }
    void SetIsLocked( const bool& islocked ) { m_opts.islocked = islocked; }
    bool IsLocked() const { return m_opts.islocked; }
    void SetIsPassworded( const bool& ispassworded ) { m_opts.ispassworded = ispassworded; }
    bool IsPassworded() const { return m_opts.ispassworded; }

    void SetNatType( const NatType nattype ) { m_opts.nattype = nattype; }
    NatType GetNatType() const { return m_opts.nattype; }
    void SetHostPort( int port) { m_opts.port = port; }

    void SetExternalUdpSourcePort(int port){m_opts.externaludpsourceport=port;}
    int GetExternalUdpSourcePort(){return m_opts.externaludpsourceport;}

    int GetHostPort() const { return m_opts.port; }
    void SetFounder( const wxString& nick ) { m_opts.founder = nick; }
    void SetHostIp( const wxString& ip ) { m_opts.ip = ip; }
    wxString GetHostIp() const { return m_opts.ip; }

    void SetMaxPlayers( const int& maxplayers ) { m_opts.maxplayers = maxplayers; }
    unsigned int GetMaxPlayers() const { return m_opts.maxplayers; }
    void SetSpectators( const int& spectators ) { m_opts.spectators = spectators; }
    int GetSpectators() const { return m_opts.spectators; }

    void SendHostInfo( HostInfo update );
    void SendHostInfo( const wxString& Tag );

    void SetRankNeeded( const int& rankneeded ) { m_opts.rankneeded = rankneeded; }
    int GetRankNeeded() const { return m_opts.rankneeded; }

    //void SetMapHash( const wxString& maphash ) { m_opts.maphash = maphash; }
    //void SetMapname( const wxString& map ) { m_opts.mapname = map; }
    void SetDescription( const wxString& desc ) { m_opts.description = desc; }
    wxString GetDescription() const { return m_opts.description; }
    //void SetModname( const wxString& mod ) { m_opts.modname = mod; }

    void SetImReady( bool ready );

    User& GetFounder() { return GetUser( m_opts.founder ); }
    User& GetMe();
    bool IsFounderMe();

    bool IsFull() { return GetMaxPlayers() == ( GetNumUsers() - GetSpectators() ); }

    int GetMyPlayerNum();

    int GetFreeTeamNum( bool excludeme = true );
    wxColour GetFreeColour( bool excludeme = true );

    void Update();
    void Update( const wxString& Tag );

    void Join( const wxString& password = _T("") );
    void Leave();

    void KickPlayer( User& user );

    bool IsEveryoneReady();
    void RingNotReadyPlayers();

    void Say( const wxString& msg );
    void DoAction( const wxString& msg );

    /*bool IsMapAvailable();
    bool IsModAvailable();*/

    bool HaveMultipleBotsInSameTeam() const;

    void OnRequestBattleStatus();
    void SendMyBattleStatus();

    bool IsSynced();
    //bool HasMod();

    bool ExecuteSayCommand( const wxString& cmd );

    void AddStartRect( int allyno, int left, int top, int right, int bottom );
    void RemoveStartRect( int allyno );
    void UpdateStartRect( int allyno );

    void AddBot( const wxString& nick, const wxString& owner, UserBattleStatus status, const wxString& aidll );
    void RemoveBot( const wxString& nick );
    void SetBotTeam( const wxString& nick, int team );
    void SetBotAlly( const wxString& nick, int ally );
    void SetBotSide( const wxString& nick, int side );
    void SetBotColour( const wxString& nick, const wxColour& col );
    void SetBotHandicap( const wxString& nick, int handicap );

    BattleBot* GetBot( const wxString& name );
    BattleBot* GetBot( unsigned int index );
    unsigned int GetNumBots();

    void StartRectRemoved( int allyno );
    void StartRectUpdated( int allyno );

    BattleStartRect* GetStartRect( int allyno );
    void ClearStartRects();

    void ForceSide( User& user, int side );
    void ForceTeam( User& user, int team );
    void ForceAlly( User& user, int ally );
    void ForceColour( User& user, const wxColour& col );
    void ForceSpectator( User& user, bool spectator );
    void BattleKickPlayer( User& user );
    void SetHandicap( User& user, int handicap);

    void OnUserAdded( User& user );
    void OnUserRemoved( User& user );

    void OnBotAdded( const wxString& nick, const wxString& owner, const UserBattleStatus& bs, const wxString& aidll );
    void OnBotRemoved( const wxString& nick );
    void OnBotUpdated( const wxString& name, const UserBattleStatus& bs );

    int GetMyAlly() { return GetMe().BattleStatus().ally; }
    void SetMyAlly( int ally ) { GetMe().BattleStatus().ally = ally; SendMyBattleStatus(); }

    std::vector<BattleStartRect*>::size_type GetNumRects();

    mmOptionsWrapper* CustomBattleOptions() { return &m_opt_wrap; }

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

    mmOptionsWrapper m_opt_wrap;

    void RemoveUser( wxString const& user ) {}
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLE_H
