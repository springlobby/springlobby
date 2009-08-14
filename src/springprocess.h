#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGPROCESS_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGPROCESS_H

#include <wx/thread.h>
#include <wx/string.h>
#include <wx/process.h>

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_LOCAL_EVENT_TYPE( wxEVT_SPRING_EXIT, 1 )
END_DECLARE_EVENT_TYPES()

class Spring;

class SpringProcess: public wxThread
{
  public:
    SpringProcess( Spring& spring );
    ~SpringProcess();

    void OnExit();

    void SetCommand( const wxString& cmd );

    void* Entry();

  protected:
    Spring& m_spring;
    wxString m_cmd;
    int m_exit_code;
};

class wxSpringProcess: public wxProcess
{
  public:
    wxSpringProcess( Spring& spring );
    ~wxSpringProcess();

    void OnTerminate( int pid, int status );

  protected:
    Spring& m_spring;
};

const int PROC_SPRING = wxID_HIGHEST;

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGPROCESS_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

