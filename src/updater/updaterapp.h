#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H

#include <wx/app.h>

class wxTimer;
class wxTimerEvent;
class UpdaterMainwindow;

//! @brief SpringLobby wxApp
class UpdaterApp : public wxApp
{
  public:
    UpdaterApp();
    ~UpdaterApp();

    virtual bool OnInit();
    virtual int OnExit();

    virtual void OnFatalException();

    // System Events
    void OnTimer( wxTimerEvent& event );

    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);


  protected:

    wxTimer* m_timer;
	wxString m_exe_to_update;
	wxString m_version;

    UpdaterMainwindow* m_updater_window;

private:
	std::ofstream* m_logstream_target;


    DECLARE_EVENT_TABLE()
};

DECLARE_APP(UpdaterApp)

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

