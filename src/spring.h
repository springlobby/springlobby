#ifndef SPRINGLOBBY_HEADERGUARD_SPRING_H
#define SPRINGLOBBY_HEADERGUARD_SPRING_H

#include <wx/event.h>

class wxCommandEvent;
class IBattle;
class SinglePlayerBattle;
class Battle;
class Ui;
class SpringProcess;
class wxSpringProcess;
class wxString;


class Spring: public wxEvtHandler
{
  public:
    Spring( Ui& ui);
    ~Spring();

    bool IsRunning() const;
    bool Run( Battle& battle );
    bool Run( SinglePlayerBattle& battle );

    //! executes spring with replay as parameter
    /*!
     * \param filename the full path for the replayfile
     */
    bool RunReplay ( const wxString& filename );

    wxString WriteScriptTxt( IBattle& battle ) const;
    void OnTerminated( wxCommandEvent& event );

  protected:
		bool LaunchSpring( const wxString& params );

    Ui& m_ui;
    SpringProcess* m_process;
    wxSpringProcess* m_wx_process;
    bool m_running;

    DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_SPRING_H

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

