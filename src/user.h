//
// Class: User
//

#ifndef _USER_H_
#define _USER_H_

#include <string>


#define RANK_0 100
#define RANK_1 200
#define RANK_2 300
#define RANK_3 400
#define RANK_4 500


//! @brief Struct used to store a client's status.
struct UserStatus {
  bool in_game;
  bool away;
  int rank;
  bool moderator;
  bool bot;
  UserStatus(): in_game(false), away(false), rank(RANK_1), moderator(false), bot(false) {}
};


//! Class containing all the information about a user
class User
{
  public:
    User(): m_cpu(0), m_data(NULL) {}
    User( const std::string& nick, const std::string& country, const int& cpu) :
      m_nick(nick), m_country(country), m_cpu(cpu), m_data(NULL) {}
        
    virtual ~User() {}
  
    // User interface
  
    std::string GetNick() const { return m_nick; }
    void SetNick( const std::string& nick ) { m_nick = nick; }
  
    std::string GetCountry() const { return m_country; }
    void SetCountry( const std::string& country ) { m_country = country; }
  
    int GetCpu() const { return m_cpu; }
    void SetCpu( const int& cpu ) { m_cpu = cpu; }
  
    UserStatus GetStatus() const { return m_status; }
    void SetStatus( const UserStatus& status ) { m_status = status; }
  
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

