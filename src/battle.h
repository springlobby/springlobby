#ifndef SPRINGLOBBY_HEADERGUARD_BATTLE_H
#define SPRINGLOBBY_HEADERGUARD_BATTLE_H

#include <map>
#include <list>
#include <set>

#include "autohost.h"
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

enum BalanceType {
  balance_random=0,
  balance_divide
};

enum RankLimitType {
  rank_limit_none=0,
  rank_limit_autospec,
  rank_limit_autokick
};


#define DEFAULT_SERVER_PORT 8034
#define DEFAULT_EXTERNAL_UDP_SOURCE_PORT 16941


struct BattleOptions
{
  BattleOptions() :
    battleid(-1),islocked(false),isreplay(false),ispassworded(false),rankneeded(0),ranklimittype(rank_limit_autospec),
    nattype(NAT_None),port(DEFAULT_SERVER_PORT),externaludpsourceport(DEFAULT_EXTERNAL_UDP_SOURCE_PORT),internaludpsourceport(DEFAULT_EXTERNAL_UDP_SOURCE_PORT),maxplayers(0),spectators(0),
    guilistactiv(false) {}

  int battleid;
  bool islocked;
  bool isreplay;
  bool ispassworded;
  int rankneeded;
  RankLimitType ranklimittype;

  wxString founder;

  NatType nattype;
  unsigned int port;
  wxString ip;
  unsigned int externaludpsourceport;
  unsigned int internaludpsourceport;

  unsigned int maxplayers;
  unsigned int spectators;

  wxString maphash;
  wxString modhash;

  wxString description;
  wxString mapname;
  wxString modname;

  bool guilistactiv;
};

/** \brief model of a sp/mp battle
 * \todo DOCME */
class Battle : public UserList, public IBattle
{
  public:
    Battle( Server& serv, Ui& ui, int id );
    ~Battle();

    //const BattleOptions& opts() { return m_opts; }

    Server& GetServer() { return m_serv; }
    AutoHost& GetAutoHost() { return m_ah; }

    int GetBattleId() const { return m_opts.battleid; }

    bool GetGUIListActiv() const { return m_opts.guilistactiv; }
    void SetGUIListActiv(bool Activ) { m_opts.guilistactiv = Activ; }

    void SetInGame( bool ingame ) { m_ingame = ingame; }
    bool GetInGame() const { return m_ingame; }

    void SetIsReplay( const bool isreplay ) { m_opts.isreplay = isreplay; }
    void SetIsLocked( const bool islocked ) { m_opts.islocked = islocked; }
    bool IsLocked() const { return m_opts.islocked; }
    void SetIsPassworded( const bool ispassworded ) { m_opts.ispassworded = ispassworded; }
    bool IsPassworded() const { return m_opts.ispassworded; }

    void SetNatType( const NatType nattype ) { m_opts.nattype = nattype; }
    NatType GetNatType() const { return m_opts.nattype; }
    void SetHostPort( unsigned int port) { m_opts.port = port; }

    void SetMyExternalUdpSourcePort(unsigned int port){m_opts.externaludpsourceport=port;}
    unsigned int GetMyExternalUdpSourcePort(){return m_opts.externaludpsourceport;}

    void SetMyInternalUdpSourcePort(unsigned int port){m_opts.internaludpsourceport=port;}
    unsigned int GetMyInternalUdpSourcePort(){return m_opts.internaludpsourceport;}

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

    User& GetFounder() const { return GetUser( m_opts.founder ); }
    User& GetMe() const;
    bool IsFounderMe() const;

    bool IsFull() const { return GetMaxPlayers() == ( GetNumUsers() - GetSpectators() ); }

    int GetMyPlayerNum() const;

    int GetFreeTeamNum( bool excludeme = true ) const;

    wxColour GetFreeColour( User *for_whom ) const;
    void FixColours( );

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

    void AddStartRect( unsigned int allyno, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom );
    void RemoveStartRect( unsigned int allyno );
    void ResizeStartRect( unsigned int allyno );

    void AddBot( const wxString& nick, const wxString& owner, UserBattleStatus status, const wxString& aidll );
    void RemoveBot( const wxString& nick );
    void SetBotTeam( const wxString& nick, int team );
    void SetBotAlly( const wxString& nick, int ally );
    void SetBotSide( const wxString& nick, int side );
    void SetBotColour( const wxString& nick, const wxColour& col );
    void SetBotHandicap( const wxString& nick, int handicap );

    BattleBot* GetBot( const wxString& name ) const;
    BattleBot* GetBot( unsigned int index ) const;
    unsigned int GetNumBots() const;

    void StartRectRemoved( unsigned int allyno );
    void StartRectResized( unsigned int allyno );
    void StartRectAdded( unsigned int allyno );

    BattleStartRect GetStartRect( unsigned int allyno );
    void ClearStartRects();

    void ForceSide( User& user, int side );
    void ForceTeam( User& user, int team );
    void ForceAlly( User& user, int ally );
    void ForceColour( User& user, const wxColour& col );
    void ForceSpectator( User& user, bool spectator );
    void BattleKickPlayer( User& user );
    void SetHandicap( User& user, int handicap);

    void OnUserAdded( User& user );
    void OnUserBattleStatusUpdated( User &user, UserBattleStatus status );
    void OnUserRemoved( User& user );

    void OnBotAdded( const wxString& nick, const wxString& owner, const UserBattleStatus& bs, const wxString& aidll );
    void OnBotRemoved( const wxString& nick );
    void OnBotUpdated( const wxString& name, const UserBattleStatus& bs );

    int GetMyAlly() { return GetMe().BattleStatus().ally; }
    void SetMyAlly( int ally ) { GetMe().BattleStatus().ally = ally; SendMyBattleStatus(); }

    unsigned int GetNumRects();

    void Autobalance(int balance_type=0, bool clans=true, bool strong_clans=true);
    void FixTeamIDs();

    ///< quick hotfix for bans
    bool CheckBan(User &user);
    ///>

    void MakeTeamsUnique();

  protected:
    // Battle variables

    ///< quick hotfix for bans
    std::set<wxString> m_banned_users;
    std::set<wxString> m_banned_ips;
    ///>

    BattleOptions m_opts;
    Server& m_serv;
    Ui& m_ui;
    AutoHost m_ah;

    bool m_ingame;

    int m_order;

    std::map<unsigned int,BattleStartRect> m_rects;
    std::list<BattleBot*> m_bots;

    mutable std::list<BattleBot*>::const_iterator m_bot_seek;
    mutable std::list<BattleBot*>::size_type m_bot_pos;

    void RemoveUser( wxString const& user ) {}
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLE_H
