/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_CHANNEL_H
#define SPRINGLOBBY_HEADERGUARD_CHANNEL_H

#include "../userlist.h"
#include <set>
#include <wx/regex.h>
#include "../utils/mixins.hh"

class Channel;
class Server;
class Ui;
class ChatPanel;

struct UiChannelData {
  UiChannelData(): panel(0) {}

  ChatPanel* panel;
};

class Channel : public UserList, public SL::NonCopyable
{
  public:

    UiChannelData uidata;

    //Channel(): m_serv(0),m_userdata(0) {}
    Channel( Server& serv );
    virtual ~Channel();

    Server& GetServer() { return m_serv; }

    void SetName( const wxString& name );
    wxString GetName() const;
    User& GetMe();

    // filtering functions
    void CheckBanned(const wxString& name);
    bool IsBanned(const wxString& name);

    // Channel Functions
    void Say( const wxString& message );
    void DoAction( const wxString& action );
    void Leave();
	void Rejoin();

    void Said( User& who, const wxString& message );

    void DidAction( User& who, const wxString& action );

    void Left( User& who, const wxString& reason );
    void Joined( User& who );

    void OnChannelJoin( User& who );

    void SetTopic( const wxString& topic, const wxString& who );
    wxString GetTopic();
    wxString GetTopicSetBy();

    bool ExecuteSayCommand( const wxString& in );

    wxString GetPassword();
    void SetPassword( const wxString& pw );

  protected:
    Server& m_serv;

    std::set<wxString> m_banned_users;

    bool m_do_ban_regex;
    wxRegEx m_ban_regex;

    bool m_do_unban_regex;
    wxRegEx m_unban_regex;

    wxString m_ban_regex_msg;

    wxString m_topic;
    wxString m_topic_nick;
    wxString m_name;

    void* m_userdata;

    wxString m_password;

    void AddUser( User& user );
    void RemoveUser( const wxString& nick );
};

#endif // SPRINGLOBBY_HEADERGUARD_CHANNEL_H
