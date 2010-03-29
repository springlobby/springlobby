#ifndef CHATLOG_H_INCLUDED
#define CHATLOG_H_INCLUDED

#include <wx/string.h>
#include <wx/file.h>
#include <wx/arrstr.h>

class ChatLog
{
  public:
    ChatLog(const wxString& server,const wxString& room);
    ~ChatLog();
    bool AddMessage(const wxString& text);
    bool LogEnabled();
    wxString LogTime();
    void OpenInEditor();
    const wxArrayString& GetLastLines( ) const;
//    void SetTarget( const wxString& server,const wxString& room );
  protected:
    bool CreateFolder(const wxString& server);
    bool WriteLine(const wxString& text);
    bool OpenLogFile(const wxString& server,const wxString& room);
    wxString _GetPath();
    wxFile m_logfile;
    wxString m_server;
    wxString m_room;
    wxString m_current_logfile_path;
    bool m_active;
    static bool m_parent_dir_exists;
    wxArrayString m_last_lines;

    void FillLastLineArray();

};

#endif // CHATLOG_H_INCLUDED

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

