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
//#include "../utils.h"

#include <iostream>
#include <wx/msgdlg.h>
#include <wx/intl.h>
#include <wx/log.h>
#include "../springunitsynclib.h"

IMPLEMENT_APP(Springsettings)

//! @brief Initializes the logging functions.
///initializes logging in an hidden stream and std::cout/gui messages
void Springsettings::InitializeLoggingTargets()

{
	#if wxUSE_STD_IOSTREAM
    #if wxUSE_DEBUGREPORT && defined(HAVE_WX28) && defined(ENABLE_DEBUG_REPORT)
      ///hidden stream logging for crash reports
      wxLog *loggercrash = new wxLogStream( &crashreport().crashlog );
      wxLogChain *logCrashChain = new wxLogChain( loggercrash );
      logCrashChain->SetLogLevel( wxLOG_Trace );
      logCrashChain->SetVerbose( true );
    #endif
    ///std::cout logging
    wxLog *loggerconsole = new wxLogStream( &std::cout );
    wxLogChain *logChain = new wxLogChain( loggerconsole );
    logChain->SetLogLevel( wxLOG_Trace );
    logChain->SetVerbose( true );
  #elif defined ( USE_LOG_WINDOW )
    ///gui window fallback logging if console/stream output not available
    wxLog *loggerwin = new wxLogWindow(0, _T("SpringLobby error console")  );
    wxLogChain *logChain = new wxLogChain( loggerwin );
    logChain->SetLogLevel( wxLOG_Trace );
    logChain->SetVerbose( true );
    logChain->GetOldLog()->SetLogLevel( wxLOG_Warning );
  #else
    /// if all fails, no log is better than msg box spam :P
    new wxLogNull();
  #endif
}


bool Springsettings::OnInit()
{
	//initialize all loggers
	//TODO non-constant parameters
    InitializeLoggingTargets(0,0,1,1);

    #ifdef __WXMSW__
		sett().ForcePortableMode( true );
		#endif
    settings_frame* frame = new settings_frame(NULL,wxID_ANY,wxT("SpringSettings"),wxDefaultPosition,
    		wxDefaultSize);
    SetTopWindow(frame);
    frame->Show();
    return true;
}

int Springsettings::OnExit()
{
	susynclib().Unload();
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
