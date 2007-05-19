//
// Class: User
//

#include "user.h"


User::User( const std::string& nick, const std::string& country, const int& cpu, const UserStatus& status )
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


std::string User::GetNick() const
{
  return m_nick;
}

void User::SetNick( const std::string& nick )
{
  m_nick = nick;
}
  
std::string User::GetCountry() const
{
  return m_country;
}

void User::SetCountry( const std::string& country )
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

