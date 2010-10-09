/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: User
//

#include "user.h"
#include "battle.h"
#include "server.h"
//#include "utils.h"
#include "chatpanel.h"
#include "iconimagelist.h"

#include <wx/string.h>
#include <wx/intl.h>

User::User( Server& serv )
    : CommonUser( _T(""),_T(""),0 ),
    m_serv(&serv),
    m_battle(0),
    m_flagicon_idx( icons().GetFlagIcon( _T("") ) ),
    m_rankicon_idx( icons().GetRankIcon( 0 ) ),
    m_statusicon_idx( icons().GetUserListStateIcon( m_status, false, false ) ),
    m_sideicon_idx( icons().ICON_NONE )
{}

User::User( const wxString& nick, Server& serv )
    : CommonUser( nick,_T(""),0 ),
    m_serv(&serv),
    m_battle(0),
    m_flagicon_idx( icons().GetFlagIcon( _T("") ) ),
    m_rankicon_idx( icons().GetRankIcon( 0 ) ),
    m_statusicon_idx( icons().GetUserListStateIcon( m_status, false, false ) ),
    m_sideicon_idx( icons().ICON_NONE )
{}

User::User( const wxString& nick, const wxString& country, const int& cpu, Server& serv)
    : CommonUser( nick,country,cpu ),
    m_serv(&serv),
    m_battle(0),
    m_flagicon_idx( icons().GetFlagIcon( country ) ),
    m_rankicon_idx( icons().GetRankIcon( 0 ) ),
    m_statusicon_idx( icons().GetUserListStateIcon( m_status, false, false ) ),
    m_sideicon_idx( icons().ICON_NONE )
{}

User::User( const wxString& nick )
    : CommonUser( nick, wxEmptyString, 0 ),
    m_serv(0),
    m_battle(0),
    m_flagicon_idx( icons().GetFlagIcon( _T("") ) ),
    m_rankicon_idx( icons().GetRankIcon( 0 ) ),
    m_statusicon_idx( icons().GetUserListStateIcon( m_status, false, false ) ),
    m_sideicon_idx( icons().ICON_NONE )
{}

User::User( const wxString& nick, const wxString& country, const int& cpu )
    : CommonUser( nick,country,cpu ) ,
    m_serv(0),
    m_battle(0),
    m_flagicon_idx( icons().GetFlagIcon( country ) ),
    m_rankicon_idx( icons().GetRankIcon( 0 ) ),
    m_statusicon_idx( icons().GetUserListStateIcon( m_status, false, false ) ),
    m_sideicon_idx( icons().ICON_NONE )
{}

User::User()
    : CommonUser( wxEmptyString, wxEmptyString, 0 ),
    m_serv(0),
    m_battle(0),
    m_flagicon_idx( icons().GetFlagIcon( _T("") ) ),
    m_rankicon_idx( icons().GetRankIcon( 0 ) ),
    m_statusicon_idx( icons().GetUserListStateIcon( m_status, false, false ) ),
    m_sideicon_idx( icons().ICON_NONE )
{}

User::~User(){
  if(uidata.panel)uidata.panel->SetUser( 0 );
}

wxString UserStatus::GetDiffString ( const UserStatus& old ) const
{
    if ( old.away != away )
        return ( away ? _("away") : _("back") );
    if ( old.in_game != in_game )
        return ( in_game ? _("ingame") : _("back from game") );
    return
        wxEmptyString;
}

void User::Said( const wxString& /*message*/ ) const
{
}


void User::Say( const wxString& message ) const
{
  GetServer().SayPrivate( m_nick, message );
}


void User::DoAction( const wxString& message ) const
{
  GetServer().DoActionPrivate( m_nick, message );
}


Battle* User::GetBattle() const
{
  return m_battle;
}


void User::SetBattle( Battle* battle )
{
  m_battle = battle;
  m_statusicon_idx = icons().GetUserListStateIcon( m_status, false, m_battle != 0 );
}

void User::SetStatus( const UserStatus& status )
{
  m_status = status;
  // If user is host of a game, then his in_game status tells if the game is on!
  if ( m_battle != 0 ) {
  	try
  	{
			User& user = m_battle->GetFounder();
			if ( user.GetNick() == m_nick ) {
				m_battle->Update();
			}
    }catch(...){}
  }

  m_statusicon_idx = icons().GetUserListStateIcon( m_status, false, m_battle != 0 );
  m_rankicon_idx =  icons().GetRankIcon( m_status.rank );
}

void User::SetCountry( const wxString& country )
{
    m_country = country;
    m_flagicon_idx = icons().GetFlagIcon( country );
}

void CommonUser::UpdateBattleStatus( const UserBattleStatus& status )
{

  // total 17 members to update.

  m_bstatus.team = status.team;
  m_bstatus.ally = status.ally;
  m_bstatus.colour = status.colour;
  m_bstatus.color_index = status.color_index;
  m_bstatus.handicap = status.handicap;
  m_bstatus.side = status.side;
  m_bstatus.sync = status.sync;
  m_bstatus.spectator = status.spectator;
  m_bstatus.ready = status.ready;
  if( !status.aishortname.IsEmpty() ) m_bstatus.aishortname = status.aishortname;
  if( !status.airawname.IsEmpty() ) m_bstatus.airawname = status.airawname;
  if( !status.aiversion.IsEmpty() ) m_bstatus.aiversion = status.aiversion;
  if( !status.aitype > 0 ) m_bstatus.aitype = status.aitype;
  if( !status.owner.IsEmpty() ) m_bstatus.owner = status.owner;
  if( status.pos.x > 0 ) m_bstatus.pos.x = status.pos.x;
  if( status.pos.y > 0 ) m_bstatus.pos.y = status.pos.y;

  // update ip and port if those were set.
  if( !status.ip.IsEmpty() ) m_bstatus.ip = status.ip;
  if( status.udpport != 0 ) m_bstatus.udpport = status.udpport;// 15
}


void User::SendMyUserStatus() const
{
  GetServer().SendMyUserStatus();
}


bool User::ExecuteSayCommand( const wxString& cmd ) const
{
  if ( cmd.BeforeFirst(' ').Lower() == _T("/me") ) {
    GetServer().DoActionPrivate( m_nick, cmd.AfterFirst(' ') );
    return true;
  }  else return false;
}

UserStatus::RankContainer User::GetRank()
{
	return GetStatus().rank;
}

wxString User::GetRankName(UserStatus::RankContainer rank)
{
  //TODO: better interface to ranks?
      switch( rank )
      {
          case UserStatus::RANK_1: return _("Newbie");
          case UserStatus::RANK_2: return _("Beginner");
          case UserStatus::RANK_3: return _("Average");
          case UserStatus::RANK_4: return _("Above average");
          case UserStatus::RANK_5: return _("Experienced");
          case UserStatus::RANK_6: return _("Highly experienced");
          case UserStatus::RANK_7: return _("Veteran");
		  case UserStatus::RANK_8: return _("Badly needs to get laid");
          default:                 return _("Unknown");
      }
}

float User::GetBalanceRank()
{
  return 1.0 + 0.1 * float( GetStatus().rank - UserStatus::RANK_1 ) / float( UserStatus::RANK_8 - UserStatus::RANK_1 );
}

wxString User::GetClan()
{
  wxString tmp = m_nick.AfterFirst('[');
  if ( tmp != m_nick )
  {
    wxString clan = tmp.BeforeFirst(']');
    if ( clan != tmp ) return clan;
  }
  return _T("");
}

void CommonUser::SetStatus( const UserStatus& status )
{
  m_status = status;
}

//User& User::operator= ( const User& other )
//{
//    if( this != &other ) {
//        //m_serv = (other.GetServer());
//        m_status = other.GetStatus();
//        m_battle = other.GetBattle();
//        m_nick = other.GetNick();
//        m_cpu = other.GetCpu();
//        m_country = other.GetCountry();
//        m_bstatus = other.GetBattleStatus();
//        uidata = other.uidata;
//
//
//    }
//    return *this;
//}


