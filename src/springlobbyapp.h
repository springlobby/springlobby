#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H

#include <wx/app.h>

class wxTimer;
class wxTimerEvent;
class wxIcon;
class wxLocale;
class wxTranslationHelper;
class wxLogWindow;

//! @brief SpringLobby wxApp
class SpringLobbyApp : public wxApp
{
  public:
    SpringLobbyApp();
    ~SpringLobbyApp();

    virtual bool OnInit();
    virtual int OnExit();

    virtual void OnFatalException();

    // System Events
    void OnTimer( wxTimerEvent& event );
    bool SelectLanguage();

    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);


  protected:

    void CacheAndSettingsSetup();

    wxTimer* m_timer;

    bool quit_called;

    wxTranslationHelper* m_translationhelper;

    DECLARE_EVENT_TABLE()

    long m_log_verbosity;
    bool m_log_console;
    bool m_log_window_show;
    bool m_crash_handle_disable;
    bool m_updateing_only;

    wxLogWindow* m_loggerwin;
};

DECLARE_APP(SpringLobbyApp)

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGLOBBYAPP_H

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

