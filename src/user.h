#ifndef SPRINGLOBBY_HEADERGUARD_USER_H
#define SPRINGLOBBY_HEADERGUARD_USER_H

#include <wx/string.h>
#include <wx/colour.h>

class Server;

enum RankContainer
{
  RANK_UNKNOWN,
  RANK_1,
  RANK_2,
  RANK_3,
  RANK_4,
  RANK_5,
  RANK_6,
  RANK_7
};



#define SYNC_UNKNOWN 0
#define SYNC_SYNCED 1
#define SYNC_UNSYNCED 2

//! @brief Struct used to store a client's status.
struct UserStatus {
  bool in_game;
  bool away;
  RankContainer rank;
  bool moderator;
  bool bot;
  UserStatus(): in_game(false), away(false), rank(RANK_UNKNOWN), moderator(false), bot(false) {}
  wxString GetDiffString ( const UserStatus& other );
};

struct UserBattleStatus {
  /// when adding something to this struct, also modify User::UpdateBattleStatus()
  /// total 12 members here
  int order;
  int team;
  int ally;
  wxColour colour;
  int color_index;
  int handicap;
  int side;
  int sync;
  bool spectator;
  bool ready;
  /// for nat holepunching
  wxString ip;
  unsigned int udpport;
  UserBattleStatus(): order(-1),team(0),ally(0),colour(wxColour(0,0,0)),color_index(-1),handicap(0),side(0),sync(SYNC_UNKNOWN),spectator(true),ready(false),udpport(0) {}
};

class ChatPanel;
class Battle;

struct UiUserData {
  UiUserData(): panel(0) {}
  ChatPanel* panel;
};

//! Class containing all the information about a user
class User
{
  public:

    UiUserData uidata;

    User( Server& serv ): m_serv(serv),m_cpu(0), m_battle(0) {}
    User( const wxString& nick, Server& serv ) : m_serv(serv),m_nick(nick), m_cpu(0), m_battle(0) {}
    User( const wxString& nick, const wxString& country, const int& cpu, Server& serv) :
      m_serv(serv),m_nick(nick), m_country(country), m_cpu(cpu), m_battle(0) {}

    virtual ~User();

    // User interface

    Server& GetServer() { return m_serv; }
    wxString GetNick() const { return m_nick; }
    void SetNick( const wxString& nick ) { m_nick = nick; }

    wxString GetCountry() const { return m_country; }
    void SetCountry( const wxString& country ) { m_country = country; }

    int GetCpu() const { return m_cpu; }
    void SetCpu( const int& cpu ) { m_cpu = cpu; }

    UserStatus& Status() { return m_status; }

    UserStatus GetStatus() const { return m_status; }
    void SetStatus( const UserStatus& status );

    UserBattleStatus& BattleStatus() { return m_bstatus; }
    //void SetBattleStatus( const UserBattleStatus& status, bool setorder = false );/// dont use this to avoid overwriting data like ip and port, use following method.
    void UpdateBattleStatus( const UserBattleStatus& status, bool setorder = false );

/*    void SetUserData( void* userdata ) { m_data = userdata; }
    void* GetUserData() { return m_data; }*/

    void Said( const wxString& message );
    void Say( const wxString& message );
    void DoAction( const wxString& message );

    Battle* GetBattle() const;
    void SetBattle( Battle* battle );

    void SendMyUserStatus();

    bool ExecuteSayCommand( const wxString& cmd );

    static wxString GetRankName(RankContainer rank);

    float GetBalanceRank();

    wxString GetClan();

  protected:
    // User variables

    Server& m_serv;
    wxString m_nick;
    wxString m_country;
    int m_cpu;
    UserStatus m_status;
    UserBattleStatus m_bstatus;
    Battle* m_battle;
    //void* m_data;
};

#endif // SPRINGLOBBY_HEADERGUARD_USER_H
