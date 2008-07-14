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

#include "../crashreport.h"
#include "../utils.h"

#include <iostream>
#include <wx/msgdlg.h>
#include <wx/intl.h>
#include "../springunitsynclib.h"

IMPLEMENT_APP(Springsettings)

bool Springsettings::OnInit()
{
	//initialize all loggers
	//TODO non-constant parameters
    InitializeLoggingTargets(0,0,1,1);

    settings_frame* frame = new settings_frame(NULL,wxID_ANY,wxT("SpringSettings"),wxDefaultPosition,
    		wxDefaultSize);
    SetTopWindow(frame);
    frame->Show();
    return true;
}

int Springsettings::OnExit()
{
	susynclib()->Unload();
	return 0;
}

//! @brief is called when the app crashes
void Springsettings::OnFatalException()
{
  #if wxUSE_DEBUGREPORT && defined(HAVE_WX28) && defined(ENABLE_DEBUG_REPORT)
  crashreport().GenerateReport(wxDebugReport::Context_Exception);
  #else
  wxMessageBox( _("The application has generated a fatal error and will be terminated\nGenerating a bug report is not possible\n\nplease enable wxUSE_DEBUGREPORT"),_("Critical error"), wxICON_ERROR );
  #endif
}
