#ifndef SPRINGLOBBY_HEADERGUARD_CHANNEL_H
#define SPRINGLOBBY_HEADERGUARD_CHANNEL_H

#include "userlist.h"

class Channel;
class Server;
class Ui;
class ChatPanel;

typedef void(*channel_msg_callback)(Channel&,User&,const std::string&);
typedef void(*channel_cmd_callback)(Channel&);
typedef void(*channel_whocmd_callback)(Channel&,User&);

struct UiChannelData {
  UiChannelData(): panel(0) {}

  ChatPanel* panel;
};

class Channel : public UserList
{
  public:

    UiChannelData uidata;

    //Channel(): m_serv(0),m_userdata(0) {}
    Channel( Server& serv, Ui& ui ): m_serv(serv),m_ui(ui) {}
    virtual ~Channel() {}

    Server& GetServer() { return m_serv; }

    void SetName( const std::string& name );
    std::string GetName();
    User& GetMe();

    // Channel Functions
    void Say( const std::string& message );
    void DoAction( const std::string& action );
    void Leave();

    void Said( User& who, const std::string& message );

    void DidAction( User& who, const std::string& action );

    void Left( User& who, const std::string& reason );
    void Joined( User& who );

    void SetTopic( const std::string& topic, const std::string& who );
    std::string GetTopic();
    std::string GetTopicSetBy();

    bool ExecuteSayCommand( const std::string& in );

  protected:
    std::string m_topic;
    std::string m_topic_nick;
    std::string m_name;

    Server& m_serv;
    Ui& m_ui;

    void* m_userdata;

    void AddUser( User& user );
    void RemoveUser( const std::string& nick );
};

#endif // SPRINGLOBBY_HEADERGUARD_CHANNEL_H
