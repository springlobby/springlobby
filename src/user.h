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
#ifndef _USER_H_
#define _USER_H_

#include <string>

using namespace std;


#define RANK_1 100
#define RANK_2 200
#define RANK_3 300
#define RANK_4 400


//! @brief Struct used to store a client's status.
struct UserStatus {
  bool in_game;
  bool away;
  int rank;
  bool moderator;
  bool bot;
};


//! Class containing all the information about a user
class User
{
  public:
    User();
    User( const string& nick, const string& country, const int& cpu, const UserStatus& status );
    virtual ~User();
  
    // User interface
  
    string GetNick() const;
    void SetNick( const string& nick );
  
    string GetCountry() const;
    void SetCountry( const string& country );
  
    int GetCpu() const;
    void SetCpu( const int& cpu );
  
    UserStatus GetStatus() const;
    void SetStatus( const UserStatus& status );
  
  protected:
    // User variables
  
    string m_nick;
    string m_country;
    int m_cpu;
    UserStatus m_status;
  
};


#endif	//_USER_H_

