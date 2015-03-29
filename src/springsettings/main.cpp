/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "main.h"
#include "frame.h"

#include "crashreport.h"
#include "utils/conversion.h"
#include "log.h"
#include "settings.h"
#include "utils/slpaths.h"
#include "se_utils.h"
#include "gui/customdialogs.h"
#include "utils/wxTranslationHelper.h"

#include <iostream>
#include <wx/msgdlg.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/cmdline.h>
#include <wx/frame.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

#include <lslutils/globalsmanager.h>
#include <lslunitsync/unitsync.h>
#include <lslutils/config.h>

IMPLEMENT_APP(Springsettings)

Springsettings::Springsettings()
    : m_translationhelper(NULL)
    , m_log_verbosity(3)
    , m_log_console(true)
    , m_log_file(false)
    , m_log_window_show(false)
    , m_crash_handle_disable(false)
{
}

Springsettings::~Springsettings()
{
}

void Springsettings::OnFatalException()
{
	CrashReport::instance().GenerateReport();
}

bool Springsettings::OnInit()
{
	wxSetEnv(_T("UBUNTU_MENUPROXY"), _T("0"));
	//this triggers the Cli Parser amongst other stuff
	if (!wxApp::OnInit())
		return false;

	SetAppName(_T("SpringSettings"));
	const wxString configdir = TowxString(SlPaths::GetConfigfileDir());
	if (!wxDirExists(configdir))
		wxMkdir(configdir);
#if wxUSE_ON_FATAL_EXCEPTION
	wxHandleFatalExceptions(!m_crash_handle_disable);
#endif

	//initialize all loggers
	//TODO non-constant parameters
	wxLogWindow* loggerwin = Logger::InitializeLoggingTargets(0, m_log_console, m_log_file_path, m_log_window_show, m_log_verbosity);
//this needs to called _before_ mainwindow instance is created

#ifdef __WXMSW__
	wxString path = wxPathOnly(wxStandardPaths::Get().GetExecutablePath()) + wxFileName::GetPathSeparator() + _T("locale");
#else
#if defined(LOCALE_INSTALL_DIR)
	wxString path(_T(LOCALE_INSTALL_DIR));
#else
	// use a dummy name here, we're only interested in the base path
	wxString path = wxStandardPaths::Get().GetLocalizedResourcesDir(_T("noneWH"), wxStandardPaths::ResourceCat_Messages);
	path = path.Left(path.First(_T("noneWH")));
#endif
#endif
	m_translationhelper = new wxTranslationHelper(GetAppName().Lower(), path);

	SetSettingsStandAlone(true);

	// configure unitsync paths before trying to load
	SlPaths::ReconfigureUnitsync();

	//unitsync first load, NEEDS to be blocking
	LSL::usync().ReloadUnitSyncLib();

	settings_frame* frame = new settings_frame(NULL, GetAppName());
	SetTopWindow(frame);
	frame->Show();

	if (loggerwin) { // we got a logwindow, lets set proper parent win
		loggerwin->GetFrame()->SetParent(frame);
	}

	return true;
}

int Springsettings::OnExit()
{
	if (m_translationhelper) {
		wxDELETE(m_translationhelper);
	}

	sett().SaveSettings(); // to make sure that cache path gets saved before destroying unitsync

	SetEvtHandlerEnabled(false);
	LSL::Util::DestroyGlobals();
	return 0;
}

void Springsettings::OnInitCmdLine(wxCmdLineParser& parser)
{
	static const wxCmdLineEntryDesc cmdLineDesc[] =
	    {
	     {wxCMD_LINE_SWITCH, "h", "help", wxTRANSLATE("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},
	     {wxCMD_LINE_SWITCH, "nc", "no-crash-handler", wxTRANSLATE("don't use the crash handler (useful for debugging)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
	     {wxCMD_LINE_SWITCH, "cl", "console-logging", wxTRANSLATE("shows application log to the console(if available)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
	     {wxCMD_LINE_OPTION, "fl", "file-logging", wxTRANSLATE("dumps application log to a file ( enter path )"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
	     {wxCMD_LINE_SWITCH, "gl", "gui-logging", wxTRANSLATE("enables application log window"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
	     {wxCMD_LINE_OPTION, "f", "config-file", wxTRANSLATE("override default choice for config-file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_NEEDS_SEPARATOR},
	     {wxCMD_LINE_OPTION, "l", "log-verbosity", wxTRANSLATE("overrides default logging verbosity, can be:\n                                0: no log\n                                1: critical errors\n                                2: errors\n                                3: warnings (default)\n                                4: messages\n                                5: function trace"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
	     {wxCMD_LINE_NONE, NULL, NULL, NULL, wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL} //this is mandatory according to http://docs.wxwidgets.org/stable/wx_wxcmdlineparser.html
	    };

	parser.SetDesc(cmdLineDesc);
	parser.SetSwitchChars(_T("-"));
}

//! @brief parses the command line and sets global app options like log verbosity or log target
bool Springsettings::OnCmdLineParsed(wxCmdLineParser& parser)
{
	if (!parser.Parse(true)) {
#if wxUSE_STD_IOSTREAM
		m_log_console = parser.Found(_T("console-logging"));
#endif
		m_log_file = parser.Found(_T("file-logging"), &m_log_file_path);
		m_log_window_show = parser.Found(_T("gui-logging"));
		m_crash_handle_disable = parser.Found(_T("no-crash-handler"));

		wxString config;
		;
		const bool userconfig = parser.Found(_T("config-file"), &config);
		if (userconfig) {
			wxFileName fn(config);
			if (!fn.IsAbsolute()) {
				wxLogError(_T("path for parameter \"config-file\" must be absolute"));
				return false;
			}
			if (!fn.IsFileWritable()) {
				wxLogError(_T("path for parameter \"config-file\" must be writeable"));
				return false;
			}
			SlPaths::SetUserConfigPath(STD_STRING(config));
		}


		if (!parser.Found(_T("log-verbosity"), &m_log_verbosity))
			m_log_verbosity = m_log_window_show ? 3 : 5;

		return true;
	} else {
		return false;
	}
}
