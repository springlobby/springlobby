/**
    This file is part of springsettings,
    Copyright (C) 2007
    Original work by Kloot
    cross-plattform/UI adaptation and currently maintained by koshi (Ren� Milk)
    visit http://spring.clan-sy.com/phpbb/viewtopic.php?t=12104
    for more info/help

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with springsettings.  If not, see <http://www.gnu.org/licenses/>.
**/
#include "main.h"
#include "frame.h"

#include "../stacktrace.h"
#include "../utils.h"

#include <iostream>
#include <wx/msgdlg.h>
#include <wx/intl.h>
#include "../springunitsynclib.h"

IMPLEMENT_APP(Springsettings)

bool Springsettings::OnInit()
{
	//initialize all loggers
    InitializeLoggingTargets();

    settings_frame* frame = new settings_frame(NULL,wxID_ANY,wxT("springsettings"),wxDefaultPosition,
    		wxDefaultSize,wxMINIMIZE_BOX  | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN);
    SetTopWindow(frame);
    frame->Show();
    return true;
}

int Springsettings::OnExit()
{
	susynclib()->Unload();
	return 0;
}

void Springsettings::OnFatalException()
{
		#if wxUSE_STACKWALKER

		wxMessageBox( _T("SpringSettings has generated a fatal error and will be terminated\nA stacktrace will be dumped to the application's console output"), _T("Critical error"), wxICON_ERROR  );

		wxLogError( _T("uncaught exception") );
		wxString DebugInfo = _T("\n-------- Begin StackTrace --------\n");

		DebugInfo += _T("StackTraceID: ") + stacktrace().GetStackTraceHash() + _T("\n");

		stacktrace().WalkFromException();
		DebugInfo += stacktrace().GetStackTrace();

		DebugInfo += _T("-------- End StackTrace --------");

		wxLogMessage( DebugInfo );
		#else
		wxMessageBox( _("SpringSettings has generated a fatal error and will be terminated\nGenerating a stacktrace is not possible\n\nplease enable wxStackWalker"), _("Critical error"), wxICON_ERROR  );
		#endif
}
