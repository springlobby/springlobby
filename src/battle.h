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


/** \brief model of a sp/mp battle
 * \todo DOCME */
class Battle : public IBattle
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

    wxColour GetFreeColour( User *for_whom ) const;
    void FixColours( );

    void Update();
    void Update( const wxString& Tag );

    void Join( const wxString& password = _T("") );
    void Leave();

    void KickPlayer( User& user );

    void RingNotReadyPlayers();

    void Say( const wxString& msg );
    void DoAction( const wxString& msg );


    void OnRequestBattleStatus();
    void SendMyBattleStatus();

    bool IsSynced();
    //bool HasMod();

    bool ExecuteSayCommand( const wxString& cmd );

    void AddBot( const wxString& nick, const wxString& owner, UserBattleStatus status, const wxString& aidll );

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

    bool IsFounderMe();

    int GetMyPlayerNum();

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

    User& GetMe();
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

    const int m_id;
};

class OfflineBattle : public IBattle
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

        bool IsFounderMe() { return m_is_founder_me; }

        void SetBattleOptions( const BattleOptions& options ) { m_opts = options;}
        user_map_t::size_type GetNumUsers() const { return m_participants.size(); }

        UserVecCIter GetFirstUser() const { return m_participants.begin(); }
        UserVecCIter GetLastUser() const { return m_participants.end(); }

        virtual bool ModExists();
        virtual bool MapExists();

        User& GetMe();

    protected:

        UserVec m_participants;

				int m_id;
        bool m_is_founder_me;
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLE_H
