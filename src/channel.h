#ifndef SPRINGLOBBY_HEADERGUARD_CHANNEL_H
#define SPRINGLOBBY_HEADERGUARD_CHANNEL_H

#include "userlist.h"
#include <set>
#include <vector>
#include <utility>
#include <wx/regex.h>

class Channel;
class Server;
class Ui;
class ChatPanel;

//! wtf is this even? commenting it makes no error (koshi)
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
    //! stores mutee,description
    typedef std::pair<wxString,wxString> Mute;
    typedef std::vector<Mute> MuteList;


    UiChannelData uidata;

    //Channel(): m_serv(0),m_userdata(0) {}
    Channel( Server& serv, Ui& ui ): m_serv(serv),m_ui(ui),m_do_ban_regex(false), m_do_unban_regex(false) {}
    virtual ~Channel();

    Server& GetServer() { return m_serv; }

    void SetName( const wxString& name );
    wxString GetName();
    User& GetMe();

    // filtering functions
    void CheckBanned(const wxString& name);
    bool IsBanned(const wxString& name);

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

    wxString GetPassword();
    void SetPassword( const wxString& pw );

    void AddMute( const wxString& mutee, const wxString& description );
    void ClearMutelist();

    const MuteList& GetMutelist ();

  protected:
    Server& m_serv;

    Ui& m_ui;

    std::set<wxString> m_banned_users;
    //std::string ban_regex;
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

    MuteList m_mutelist;

    void AddUser( User& user );
    void RemoveUser( const wxString& nick );
};

#endif // SPRINGLOBBY_HEADERGUARD_CHANNEL_H
