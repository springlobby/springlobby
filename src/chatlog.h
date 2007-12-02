#ifndef CHATLOG_H_INCLUDED
#define CHATLOG_H_INCLUDED

#include <wx/string.h>
#include <wx/file.h>

class ChatLog
{
  public:
    ChatLog(const wxString& server,const wxString& room);
    ~ChatLog();
    bool AddMessage(const wxString& text);
    bool LogEnabled();
    wxString LogTime();
  protected:
    bool _CreateFolder(const wxString& server);
    bool _WriteLine(const wxString& text);
    bool _OpenLogFile(const wxString& server,const wxString& room);
    wxString _GetPath();
    wxFile* m_logfile;
    wxString m_server;
    wxString m_room;
    bool m_active;
    static bool m_parent_dir_exists;
};

#endif // CHATLOG_H_INCLUDED
