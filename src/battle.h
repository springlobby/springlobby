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

const unsigned int DEFAULT_SERVER_PORT = 8452;
const unsigned int DEFAULT_EXTERNAL_UDP_SOURCE_PORT = 16941;

struct BattleOptions
{
  BattleOptions() :
    battleid(-1),islocked(false),isreplay(false),ispassworded(false),rankneeded(0),isproxy(false),ranklimittype(IBattle::rank_limit_autospec),
    nattype(IBattle::NAT_None),port(DEFAULT_SERVER_PORT),externaludpsourceport(DEFAULT_EXTERNAL_UDP_SOURCE_PORT),internaludpsourceport(DEFAULT_EXTERNAL_UDP_SOURCE_PORT),maxplayers(0),spectators(0),
    guilistactiv(false) {}

  int battleid;
  bool islocked;
  bool isreplay;
  bool ispassworded;
  int rankneeded;
  bool isproxy;
  bool lockexternalbalancechanges;
  IBattle::RankLimitType ranklimittype;

  wxString founder;

  IBattle::NatType nattype;
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

class CommonBattle : public UserList, public IBattle
{
    public:
        CommonBattle( const int id, const bool ingame, const int order );

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

        void SetRankNeeded( const int& rankneeded ) { m_opts.rankneeded = rankneeded; }
        int GetRankNeeded() const { return m_opts.rankneeded; }

        //void SetMapHash( const wxString& maphash ) { m_opts.maphash = maphash; }
        //void SetMapname( const wxString& map ) { m_opts.mapname = map; }
        void SetDescription( const wxString& desc ) { m_opts.description = desc; }
        wxString GetDescription() const { return m_opts.description; }
        //void SetModname( const wxString& mod ) { m_opts.modname = mod; }

        void AddStartRect( unsigned int allyno, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom );
        void RemoveStartRect( unsigned int allyno );
        void ResizeStartRect( unsigned int allyno );
        void StartRectRemoved( unsigned int allyno );
        void StartRectResized( unsigned int allyno );
        void StartRectAdded( unsigned int allyno );
        BattleStartRect GetStartRect( unsigned int allyno );
        void ClearStartRects();
        unsigned int GetNumRects();

        BattleBot* GetBot( const wxString& name ) const;
        BattleBot* GetBot( unsigned int index ) const;
        unsigned int GetNumBots() const;
        void OnBotAdded( const wxString& nick, const wxString& owner, const UserBattleStatus& bs, const wxString& aidll );
        void OnBotRemoved( const wxString& nick );
        void OnBotUpdated( const wxString& name, const UserBattleStatus& bs );
        virtual void SetBotTeam( const wxString& nick, int team ) = 0;
        virtual void SetBotAlly( const wxString& nick, int ally ) = 0;
        virtual void SetBotSide( const wxString& nick, int side ) = 0;
        virtual void SetBotColour( const wxString& nick, const wxColour& col ) = 0;
        virtual void SetBotHandicap( const wxString& nick, int handicap ) = 0;

        User& GetFounder() const { return GetUser( m_opts.founder ); }

        bool IsFull() const { return GetMaxPlayers() == ( GetNumUsers() - GetSpectators() ); }


    protected:

        bool m_ingame;
        int m_order;
        BattleOptions m_opts;

        std::map<unsigned int,BattleStartRect> m_rects;
        std::list<BattleBot*> m_bots;

        mutable std::list<BattleBot*>::const_iterator m_bot_seek;
        mutable std::list<BattleBot*>::size_type m_bot_pos;
};

/** \brief model of a sp/mp battle
 * \todo DOCME */
class Battle : public CommonBattle
{
  public:
    Battle( Server& serv, int id );
    ~Battle();

    //const BattleOptions& opts() { return m_opts; }

    Server& GetServer() { return m_serv; }
    AutoHost& GetAutoHost() { return m_ah; }

    void SendHostInfo( HostInfo update );
    void SendHostInfo( const wxString& Tag );

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

    void AddBot( const wxString& nick, const wxString& owner, UserBattleStatus status, const wxString& aidll );
    void RemoveBot( const wxString& nick );
    void SetBotTeam( const wxString& nick, int team );
    void SetBotAlly( const wxString& nick, int ally );
    void SetBotSide( const wxString& nick, int side );
    void SetBotColour( const wxString& nick, const wxColour& col );
    void SetBotHandicap( const wxString& nick, int handicap );

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

    int GetMyAlly() { return GetMe().BattleStatus().ally; }
    void SetMyAlly( int ally ) { GetMe().BattleStatus().ally = ally; SendMyBattleStatus(); }


    void Autobalance( BalanceType balance_type = balance_divide, bool clans = true, bool strong_clans = true, int allyteamsize = 0 );
    void FixTeamIDs( BalanceType balance_type = balance_divide, bool clans = true, bool strong_clans = true, int controlteamsize = 0 );
    void ForceUnsyncedToSpectate();

    void SetAutoLockOnStart( bool value );
    bool GetAutoLockOnStart();

    void SetIsProxy( bool value );
    bool IsProxy();

    void SetLockExternalBalanceChanges( bool value );
    bool GetLockExternalBalanceChanges();

    ///< quick hotfix for bans
    bool CheckBan(User &user);
    ///>

    void SetImReady( bool ready );

    void DisableHostStatusInProxyMode( bool value ) { m_generating_script = value; }

    User& GetMe() const;
    bool IsFounderMe() const;

  protected:
    // Battle variables

    ///< quick hotfix for bans
    std::set<wxString> m_banned_users;
    std::set<wxString> m_banned_ips;
    ///>

    Server& m_serv;
    AutoHost m_ah;
    bool m_autolock_on_start;
    bool m_generating_script;

    void RemoveUser( wxString const& user ) {}
};

class OfflineBattle : public CommonBattle
{
    public:
        OfflineBattle ( const int id );
        OfflineBattle ( );
        ~OfflineBattle (){};

        void AddUser( const OfflineUser& user );
        void AddUser( const wxString& nick );
        void UpdateUserBattleStatus( OfflineUser &user, UserBattleStatus status );
        void RemoveUser( OfflineUser& user );

        //funcs from Ibattle i'm too lazy to refactor in a meaningful way
        wxColour GetFreeColour( User *for_whom ) const {return wxColour(); };
        int GetMyAlly() { return 0; }
        void SetMyAlly( int ally ) { }
        bool IsFounderMe() const {return true;}
        void SendHostInfo( HostInfo update ) {}
        void SendHostInfo( const wxString& Tag ) {}
        void Update ( const wxString& Tag ) {}
        //---

        void SetBotTeam( const wxString& nick, int team );
        void SetBotAlly( const wxString& nick, int ally );
        void SetBotSide( const wxString& nick, int side );
        void SetBotColour( const wxString& nick, const wxColour& col );
        void SetBotHandicap( const wxString& nick, int handicap );

        void SetBattleOptions( const BattleOptions& options ) { m_opts = options;}
        user_map_t::size_type GetNumUsers() const { return m_participants.size(); }

        typedef std::vector<OfflineUser> UserVec;
        typedef UserVec::const_iterator UserVecCIter;

        UserVecCIter GetFirstUser() const { return m_participants.begin(); }
        UserVecCIter GetLastUser() const { return m_participants.end(); }

        virtual bool ModExists();
        virtual bool MapExists();

    protected:

        UserVec m_participants;
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLE_H
