#ifndef SPRINGLOBBY_HEADERGUARD_CHANNEL_H
#define SPRINGLOBBY_HEADERGUARD_CHANNEL_H

#include "userlist.h"
#include <set>

class Channel;
class Server;
class Ui;
class ChatPanel;

typedef void(*channel_msg_callback)(Channel&,User&,const wxString&);
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

    void SetName( const wxString& name );
    wxString GetName();
    User& GetMe();

    // filtering functions
    void CheckBanned(const wxString& name);

    // Channel Functions
    void Say( const wxString& message );
    void DoAction( const wxString& action );
    void Leave();

    void Said( User& who, const wxString& message );

    void DidAction( User& who, const wxString& action );

    void Left( User& who, const wxString& reason );
    void Joined( User& who );

    void OnChannelJoin( User& who );

    void SetTopic( const wxString& topic, const wxString& who );
    wxString GetTopic();
    wxString GetTopicSetBy();

    bool ExecuteSayCommand( const wxString& in );

  protected:
    std::set<wxString> banned_users;

    wxString m_topic;
    wxString m_topic_nick;
    wxString m_name;

    Server& m_serv;
    Ui& m_ui;

    void* m_userdata;

    void AddUser( User& user );
    void RemoveUser( const wxString& nick );
};

#endif // SPRINGLOBBY_HEADERGUARD_CHANNEL_H
