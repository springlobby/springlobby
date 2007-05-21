//
// Class: User
//

#ifndef _USER_H_
#define _USER_H_

#include <string>


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
    User( const std::string& nick, const std::string& country, const int& cpu);
    virtual ~User();
  
    // User interface
  
    std::string GetNick() const;
    void SetNick( const std::string& nick );
  
    std::string GetCountry() const;
    void SetCountry( const std::string& country );
  
    int GetCpu() const;
    void SetCpu( const int& cpu );
  
    UserStatus GetStatus() const;
    void SetStatus( const UserStatus& status );
  
    void SetUserData( void* userdata ) { m_data = userdata; }
    void* GetUserData() { return m_data; }
  
    void Said( const std::string& message );
    void Say( const std::string& message );
  
  protected:
    // User variables
  
    std::string m_nick;
    std::string m_country;
    int m_cpu;
    UserStatus m_status;
    void* m_data;
  
};


#endif	//_USER_H_

