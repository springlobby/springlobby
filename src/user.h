#ifndef SPRINGLOBBY_HEADERGUARD_USER_H
#define SPRINGLOBBY_HEADERGUARD_USER_H

#include <wx/string.h>
#include <wx/colour.h>
#include "utils/mixins.hh"
class Server;

const unsigned int SYNC_UNKNOWN = 0;
const unsigned int SYNC_SYNCED = 1;
const unsigned int SYNC_UNSYNCED = 2;

//! @brief Struct used to store a client's status.
struct UserStatus
{
    enum RankContainer
    {
      RANK_1,
      RANK_2,
      RANK_3,
      RANK_4,
      RANK_5,
      RANK_6,
	  RANK_7,
	  RANK_8
    };

  bool in_game;
  bool away;
  RankContainer rank;
  bool moderator;
  bool bot;
  UserStatus(): in_game(false), away(false), rank(RANK_1), moderator(false), bot(false) {}
  wxString GetDiffString ( const UserStatus& other ) const;
};

struct UserPosition
{
	int x;
	int y;
	UserPosition(): x(-1), y(-1) {}
};

struct UserBattleStatus
{
  //!!! when adding something to this struct, also modify User::UpdateBattleStatus() !!
  // total 17 members here
  int team;
  int ally;
  wxColour colour;
  int color_index;
  int handicap;
  int side;
  unsigned int sync;
  bool spectator;
  bool ready;
  bool isfromdemo;
  UserPosition pos; // for startpos = 4
  // bot-only stuff
  wxString owner;
  wxString aishortname;
  wxString airawname;
  wxString aiversion;
  int aitype;
  // for nat holepunching
  wxString ip;
  unsigned int udpport;
  wxString scriptPassword;
  bool IsBot() const { return !aishortname.IsEmpty(); }
  UserBattleStatus(): team(0),ally(0),colour(wxColour(0,0,0)),color_index(-1),handicap(0),side(0),sync(SYNC_UNKNOWN),spectator(false),ready(false), isfromdemo(false), aitype(-1), udpport(0) {}
  bool operator == ( const UserBattleStatus& s ) const
  {
    return ( ( team == s.team ) && ( colour == s.colour ) && ( handicap == s.handicap ) && ( side == s.side ) && ( sync == s.sync ) && ( spectator == s.spectator ) && ( ready == s.ready ) && ( owner == s.owner ) && ( aishortname == s.aishortname ) && ( isfromdemo == s.isfromdemo ) && ( aitype == s.aitype ) );
  }
  bool operator != ( const UserBattleStatus& s ) const
  {
    return ( ( team != s.team ) || ( colour != s.colour ) || ( handicap != s.handicap ) || ( side != s.side ) || ( sync != s.sync ) || ( spectator != s.spectator ) || ( ready != s.ready ) || ( owner != s.owner ) || ( aishortname != s.aishortname ) || ( isfromdemo != s.isfromdemo )  || ( aitype != s.aitype ) );
  }
};

class ChatPanel;
class Battle;

struct UiUserData {
  UiUserData(): panel(0) {}
  ChatPanel* panel;
};

//! parent class leaving out server related functionality
class CommonUser
{
    public:
        CommonUser(const wxString& nick, const wxString& country, const int& cpu)
           : m_nick(wxString(nick)), m_country(wxString(country)), m_cpu(cpu)  {}

        virtual ~CommonUser(){}

        const wxString& GetNick() const { return m_nick; }
        virtual void SetNick( const wxString& nick ) { m_nick = nick; }

        const wxString& GetCountry() const { return m_country; }
        virtual void SetCountry( const wxString& country ) { m_country = country; }

        int GetCpu() const { return m_cpu; }
        void SetCpu( const int& cpu ) { m_cpu = cpu; }

        const wxString& GetID() const { return m_id; }
        void SetID( const wxString& id ) { m_id = id; }

        UserStatus& Status() { return m_status; }

        UserStatus GetStatus() const { return m_status; }
        virtual void SetStatus( const UserStatus& status );

        UserBattleStatus& BattleStatus() { return m_bstatus; }

		UserBattleStatus GetBattleStatus() const { return m_bstatus; }

	    /** Read-only variant of BattleStatus() above.
	     */
	    const UserBattleStatus&
	    BattleStatus() const {
			return m_bstatus;
    	}

        //void SetBattleStatus( const UserBattleStatus& status );/// dont use this to avoid overwriting data like ip and port, use following method.
        void UpdateBattleStatus( const UserBattleStatus& status );

    /*    void SetUserData( void* userdata ) { m_data = userdata; }
        void* GetUserData() { return m_data; }*/

        bool Equals( const CommonUser& other ) const { return ( m_nick == other.GetNick() ); }


    protected:
        wxString m_nick;
        wxString m_country;
        wxString m_id;
        int m_cpu;
        UserStatus m_status;
        UserBattleStatus m_bstatus;

        //void* m_data;

};

//! Class containing all the information about a user
class User : public CommonUser
{
  public:

    mutable UiUserData uidata;

    User( Server& serv );
    User( const wxString& nick, Server& serv );
    User( const wxString& nick, const wxString& country, const int& cpu, Server& serv);
    User( const wxString& nick );
    User( const wxString& nick, const wxString& country, const int& cpu );
    User();

    virtual ~User();

    // User interface

    Server& GetServer() const { return *m_serv; }

    void Said( const wxString& message ) const;
    void Say( const wxString& message ) const;
    void DoAction( const wxString& message ) const;

    Battle* GetBattle() const;
    void SetBattle( Battle* battle );

    void SendMyUserStatus() const;
    void SetStatus( const UserStatus& status );
    void SetCountry( const wxString& country );

    bool ExecuteSayCommand( const wxString& cmd ) const;

    static wxString GetRankName(UserStatus::RankContainer rank);

    float GetBalanceRank();
    UserStatus::RankContainer GetRank();
    wxString GetClan();

    int GetFlagIconIndex() const { return m_flagicon_idx; }
    int GetRankIconIndex() const { return m_rankicon_idx; }
    int GetStatusIconIndex() const { return m_statusicon_idx; }
    //bool operator< ( const User& other ) const { return m_nick < other.GetNick() ; }
    //User& operator= ( const User& other );

    int GetSideiconIndex() const { return m_sideicon_idx; }
    void SetSideiconIndex( const int idx ) { m_sideicon_idx = idx; }

  protected:
    // User variables

    Server* m_serv;
    Battle* m_battle;
    int m_flagicon_idx;
    int m_rankicon_idx;
    int m_statusicon_idx;
    int m_sideicon_idx;

	//! copy-semantics?
};

#endif // SPRINGLOBBY_HEADERGUARD_USER_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

