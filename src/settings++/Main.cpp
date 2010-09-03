/**
    This file is part of springsettings,
    Copyright (C) 2007
    Original work by Kloot
    cross-plattform/UI adaptation and currently maintained by koshi (Ren� Milk)
    visit http://spring.clan-sy.com/phpbb/viewtopic.php?t=12104
    for more info/help

    SpringLobby is free software: you can redistribute it and/or modify
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
#include "../utils/conversion.h"
#include "../utils/platform.h"
#include "../settings.h"
#include "se_utils.h"
#include "../defines.h"

#include <iostream>
#include <wx/msgdlg.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/cmdline.h>
#include <wx/frame.h>

#include "../springunitsynclib.h"

IMPLEMENT_APP(Springsettings)

Springsettings::Springsettings()
    :  m_log_verbosity( 3 ),
    m_log_console( true ),
    m_log_window_show( false ),
    m_crash_handle_disable( false )
{
    SetAppName( _T("springsettings") );
}

bool Springsettings::OnInit()
{
    //this triggers the Cli Parser amongst other stuff
    if (!wxApp::OnInit())
        return false;

    #if wxUSE_ON_FATAL_EXCEPTION
      if (!m_crash_handle_disable) wxHandleFatalExceptions( true );
    #endif

    //initialize all loggers
	//TODO non-constant parameters
	wxLogChain* logchain  = 0;
    wxLogWindow* loggerwin = InitializeLoggingTargets( 0, m_log_console, m_log_window_show, !m_crash_handle_disable, m_log_verbosity, logchain );

    SetSettingsStandAlone( true );
    settings_frame* frame = new settings_frame(NULL,wxID_ANY,wxT("SpringSettings"),wxDefaultPosition,
    		wxDefaultSize);
    SetTopWindow(frame);
    frame->Show();

    if ( loggerwin ) { // we got a logwindow, lets set proper parent win
        loggerwin->GetFrame()->SetParent( frame );
    }

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
#if wxUSE_DEBUGREPORT && defined(ENABLE_DEBUG_REPORT)
    #if wxUSE_STACKWALKER
        CrashReport::instance().GenerateReport();
    #else
        EXCEPTION_POINTERS* p = new EXCEPTION_POINTERS; //lets hope this'll never get called
        CrashReport::instance().GenerateReport(p);
    #endif
#else
    wxMessageBox( _("The application has generated a fatal error and will be terminated\nGenerating a bug report is not possible\n\nplease get a wxWidgets library that supports wxUSE_DEBUGREPORT"),_("Critical error"), wxICON_ERROR | wxOK );
#endif
}

void Springsettings::OnInitCmdLine(wxCmdLineParser& parser)
{
    #ifndef HAVE_WX29
        wxCmdLineEntryDesc cmdLineDesc[] =
        {
            { wxCMD_LINE_SWITCH, _T("h"), _T("help"), _("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
            { wxCMD_LINE_SWITCH, _T("nc"), _T("no-crash-handler"), _("don't use the crash handler (useful for debugging)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
        #if wxUSE_STD_IOSTREAM
            { wxCMD_LINE_SWITCH, _T("cl"), _T("console-logging"),  _("shows application log to the console(if available)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
        #endif
            { wxCMD_LINE_SWITCH, _T("gl"), _T("gui-logging"),  _("enables application log window"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
            //{ wxCMD_LINE_OPTION, _T("c"), _T("config-file"),  _("override default choice for config-file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_NEEDS_SEPARATOR },
            { wxCMD_LINE_OPTION, _T("l"), _T("log-verbosity"),  _("overrides default logging verbosity, can be:\n                                0: no log\n                                1: critical errors\n                                2: errors\n                                3: warnings (default)\n                                4: messages\n                                5: function trace"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
            { wxCMD_LINE_NONE }
        };
    #else
        wxCmdLineEntryDesc cmdLineDesc[] =
        {
            { wxCMD_LINE_SWITCH, "h", "help", _("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
            { wxCMD_LINE_SWITCH, "nc", "no-crash-handler", _("don't use the crash handler (useful for debugging)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
        #if wxUSE_STD_IOSTREAM
            { wxCMD_LINE_SWITCH, "cl", "console-logging",  _("shows application log to the console(if available)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
        #endif
            { wxCMD_LINE_SWITCH, "gl", "gui-logging",  _("enables application log window"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
            //{ wxCMD_LINE_OPTION, _T("c"), _T("config-file"),  _("override default choice for config-file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_NEEDS_SEPARATOR },
            { wxCMD_LINE_OPTION, "l", "log-verbosity",  _("overrides default logging verbosity, can be:\n                                0: no log\n                                1: critical errors\n                                2: errors\n                                3: warnings (default)\n                                4: messages\n                                5: function trace"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
            { wxCMD_LINE_NONE }
        };
    #endif
    parser.SetDesc( cmdLineDesc );
    parser.SetSwitchChars (wxT("-"));
}

//! @brief parses the command line and sets global app options like log verbosity or log target
bool Springsettings::OnCmdLineParsed(wxCmdLineParser& parser)
{
  #if wxUSE_CMDLINE_PARSER
    if ( !parser.Parse(true) )
    {
#if wxUSE_STD_IOSTREAM
        m_log_console = parser.Found(_T("console-logging"));
#endif
        m_log_window_show = parser.Found(_T("gui-logging"));
        m_crash_handle_disable = parser.Found(_T("no-crash-handler"));

        if ( !parser.Found(_T("log-verbosity"), &m_log_verbosity ) )
            m_log_verbosity = 3;

        if ( parser.Found(_T("help")) )
            return false; // not a syntax error, but program should stop if user asked for command line usage

        return true;
    }
    else
    {
        return false;
    }
  #else // wxUSE_CMDLINE_PARSER
  return true;
  #endif
}
