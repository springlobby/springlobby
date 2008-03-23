/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: User
//

#include "user.h"
#include "battle.h"
#include "server.h"
#include "utils.h"

#include <wx/string.h>
#include <wx/intl.h>

void User::Said( const wxString& message )
{
}


void User::Say( const wxString& message )
{
  m_serv.SayPrivate( m_nick, message );
}


void User::DoAction( const wxString& message )
{
  m_serv.DoActionPrivate( m_nick, message );
}


Battle* User::GetBattle()
{
  return m_battle;
}


void User::SetBattle( Battle* battle )
{
  m_battle = battle;
}

void User::SetStatus( const UserStatus& status )
{
  m_status = status;
  // If user is host of a game, then his in_game status tells if the game is on!
  if ( m_battle != 0 ) {
    User& user = m_battle->GetFounder();
    if ( user.GetNick() == m_nick ) {
      m_battle->Update();
    }
  }

}
/*
void User::SetBattleStatus( const UserBattleStatus& status, bool setorder )
{
  int order = m_bstatus.order;
  m_bstatus = status;
  if ( !setorder ) m_bstatus.order = order;
}
*/

void User::UpdateBattleStatus( const UserBattleStatus& status, bool setorder )
{

  //int order = m_bstatus.order;
  //m_bstatus = status;
  /// total 12 members to update.

  if ( setorder ) m_bstatus.order=status.order; /// 1
  m_bstatus.team=status.team;
  m_bstatus.ally=status.ally;
  m_bstatus.colour=status.colour;
  m_bstatus.color_index=status.color_index;
  m_bstatus.handicap=status.handicap;
  m_bstatus.side=status.side;
  m_bstatus.sync=status.sync;
  m_bstatus.spectator=status.spectator;
  m_bstatus.ready=status.ready;

  /// update ip and port if those were set.
  if(!status.ip.empty())m_bstatus.ip=status.ip;
  if(status.udpport!=0)m_bstatus.udpport=status.udpport;/// 12



  //if ( !setorder ) m_bstatus.order = order;
}


void User::SendMyUserStatus()
{
  m_serv.SendMyUserStatus();
}


bool User::ExecuteSayCommand( const wxString& cmd )
{
  if ( cmd.BeforeFirst(' ').Lower() == _T("/me") ) {
    m_serv.DoActionPrivate( m_nick, cmd.AfterFirst(' ') );
    return true;
  }  else return false;
}

wxString User::GetRankName(int rank)
{
  //TODO: better interface to ranks?
      switch(rank) {
          case RANK_0: return _("Newbie");
          case RANK_1: return _("Beginner");
          case RANK_2: return _("Average");
          case RANK_3: return _("Above average");
          case RANK_4: return _("Experienced");
          case RANK_5: return _("Highly experienced");
          case RANK_6: return _("Veteran");
      }
      return _("no rank");
}
