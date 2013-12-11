#ifndef SPRINGLOBBY_HEADERGUARD_SPRING_H
#define SPRINGLOBBY_HEADERGUARD_SPRING_H

/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

DO NOT CHANGE THIS FILE!

this file is deprecated and will be replaced with

lsl/spring/spring.h

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/


#include <wx/event.h>

class wxCommandEvent;
class IBattle;
class SinglePlayerBattle;
class OfflineBattle;
class Battle;
class SpringProcess;
class wxSpringProcess;
class wxString;


class Spring: public wxEvtHandler
{
public:
	explicit Spring();
	~Spring();

	bool IsRunning() const;
	bool Run( OfflineBattle& battle );
	bool Run( Battle& battle );
	bool Run( SinglePlayerBattle& battle );
	wxString WriteScriptTxt( IBattle& battle ) const;


private:
	void OnTerminated( wxCommandEvent& event );
	bool LaunchSpring(const wxString& engineName, const wxString& engineVersion, const wxString& params);

	SpringProcess* m_process;
	wxSpringProcess* m_wx_process;
	bool m_running;

	DECLARE_EVENT_TABLE()
};

Spring& spring();

#endif // SPRINGLOBBY_HEADERGUARD_SPRING_H

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

