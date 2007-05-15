/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


//
// Class: User
//
#include "user.h"

User::User( const string& nick, const string& country, const int& cpu, const UserStatus& status )
{
  m_nick = nick;
  m_country = country;
  m_cpu = cpu;
  m_status = status;
}

User::User()
{
  m_nick = "";
  m_country = "";
  m_cpu = 0;
  
  m_status.in_game = false;
  m_status.away = false;
  m_status.rank = RANK_1;
  m_status.moderator = false;
  m_status.bot = false;
}


User::~User()
{

}


string User::GetNick() const
{
  return m_nick;
}

void User::SetNick( const string& nick )
{
  m_nick = nick;
}
  
string User::GetCountry() const
{
  return m_country;
}

void User::SetCountry( const string& country )
{
  m_country = country;
}

  
int User::GetCpu() const
{
  return m_cpu;
}

void User::SetCpu( const int& cpu )
{
  m_cpu = cpu;
}
  
UserStatus User::GetStatus() const
{
  return m_status;
}

void User::SetStatus( const UserStatus& status )
{
  m_status = status;
}

