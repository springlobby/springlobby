/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: SpringLobbyApp
//

#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h>
#include <wx/image.h>
#include <wx/cmdline.h>
#include <wx/choicdlg.h>
#include <wx/filename.h>
#include <wx/dirdlg.h>
#include <wx/tooltip.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h> //filesystem zip handler
#include <wx/socket.h>
#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif
#include <wx/utils.h>
#include <wx/wfstream.h>

#include "springlobbyapp.h"
#include "gui/mainwindow.h"
#include "settings.h"
#include "utils/slconfig.h"
#include "crashreport.h"
#include "gui/controls.h"
#include "utils/platform.h"
#include "utils/version.h"
#include "utils/globalevents.h"
#include "channel.h"
#include "gui/customdialogs.h"
#include "downloader/prdownloader.h"
#include <lslutils/globalsmanager.h>
#include <lslutils/config.h>
#include <lslunitsync/unitsync.h>
#include "gui/notifications/notificationmanager.h"
#include "utils/wxTranslationHelper.h"
#include "gui/playback/playbacktab.h"
#include "utils/slpaths.h"
#include "downloader/lib/src/FileSystem/FileSystem.h"
#include "log.h"
#include "utils/conversion.h"
#include "gui/ui.h"
#include "servermanager.h"

#include <wx/debugrpt.h>
#include <wx/intl.h>

#if wxUSE_UNIX
#include <X11/Xlib.h>
#endif


SLCONFIG("/ResetLayout", false, "reset layout on restart");

IMPLEMENT_APP(SpringLobbyApp)

BEGIN_EVENT_TABLE(SpringLobbyApp, wxApp)
END_EVENT_TABLE()

SpringLobbyApp::SpringLobbyApp()
    : quit_called(false)
    , m_translationhelper(NULL)
    , m_log_verbosity(3)
    , m_log_console(true)
    , m_log_window_show(false)
    , m_crash_handle_disable(false)
    , m_appname(_T("SpringLobby"))
{
#if wxUSE_UNIX
	/*
workarrounds this crash:
The program 'springlobby' received an X Window System error.
This probably reflects a bug in the program.
The error was 'RenderBadPicture (invalid Picture parameter)'.
  (Details: serial 71245 error_code 143 request_code 139 minor_code 8)
  (Note to programmers: normally, X errors are reported asynchronously;
   that is, you will receive the error a while after causing it.
   To debug your program, run it with the --sync command line
   option to change this behavior. You can then get a meaningful
   backtrace from your debugger if you break on the gdk_x_error() function.)
*/
	XInitThreads();
#endif
}

//! @brief Initializes the application.
//!
//! It will open the main window and connect default to server or open the connect window.
bool SpringLobbyApp::OnInit()
{

	wxSetEnv(_T("UBUNTU_MENUPROXY"), _T("0"));
	//this triggers the Cli Parser amongst other stuff
	if (!wxApp::OnInit())
		return false;
	SetAppName(m_appname);
#if wxUSE_ON_FATAL_EXCEPTION
	wxHandleFatalExceptions(!m_crash_handle_disable);
#endif

	//initialize all loggers, we'll use the returned pointer to set correct parent window later
	wxLogWindow* loggerwin = Logger::InitializeLoggingTargets(0, m_log_console, m_log_file_path, m_log_window_show, m_log_verbosity);

	wxLogMessage(_T("SpringLobby %s started"), TowxString(getSpringlobbyVersion()).c_str());

	//this needs to called _before_ mainwindow instance is created
	wxInitAllImageHandlers();
	wxFileSystem::AddHandler(new wxZipFSHandler);
	wxSocketBase::Initialize();


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

	const wxString configdir = TowxString(SlPaths::GetConfigfileDir());
	SafeMkdir(configdir);

	if (cfg().ReadBool(_T("/ResetLayout"))) {
		//we do this early on and reset the config var a little later so we can save a def. perps once mw is created
		sett().RemoveLayouts();
		cfg().Write(_T( "/ResetLayout" ), false);
		ui().mw().SavePerspectives(_T("SpringLobby-default"));
	}

	if (SlPaths::IsPortableMode()) {
		// change write path for downloader if in portable mode
		fileSystem->setWritePath(SlPaths::GetExecutableFolder());
	} else {
		// only search if not in portable mode
		SlPaths::SetSpringBinary(SlPaths::GetCurrentUsedSpringIndex(), SlPaths::GetSpringBinary());
		SlPaths::SetUnitSync(SlPaths::GetCurrentUsedSpringIndex(), SlPaths::GetUnitSync());
	}

	// configure unitsync paths before trying to load
	SlPaths::ReconfigureUnitsync();

	//unitsync first load, NEEDS to be blocking
	SlPaths::RefreshSpringVersionList();
	LSL::usync().ReloadUnitSyncLib();


	sett().Setup(m_translationhelper);

	notificationManager(); //needs to be initialized too
	ui().ShowMainWindow();
	SetTopWindow(&ui().mw());

	ui().OnInit();

	ui().mw().SetLogWin(loggerwin);
	return true;
}


//! @brief Finalizes the application
int SpringLobbyApp::OnExit()
{
	if (quit_called) {
		return 0;
	}
	quit_called = true;

	if (m_translationhelper) {
		wxDELETE(m_translationhelper);
	}

	sett().SaveSettings(); // to make sure that cache path gets saved before destroying unitsync

	ServerManager::Release();
	GlobalEventManager::Release();
	SetEvtHandlerEnabled(false);
	UiEvents::GetNotificationEventSender().Enable(false);
	LSL::Util::DestroyGlobals();

	return 0;
}

//! @brief is called when the app crashes
void SpringLobbyApp::OnFatalException()
{
	CrashReport::instance().GenerateReport();
}


bool SpringLobbyApp::SelectLanguage()
{
	return m_translationhelper->AskUserForLanguage();
}

void SpringLobbyApp::OnInitCmdLine(wxCmdLineParser& parser)
{
	static const wxCmdLineEntryDesc cmdLineDesc[] =
	    {
	     {wxCMD_LINE_SWITCH, "h", "help", wxTRANSLATE("show this help message"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},
	     {wxCMD_LINE_SWITCH, "nc", "no-crash-handler", wxTRANSLATE("don't use the crash handler (useful for debugging)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
	     {wxCMD_LINE_OPTION, "fl", "file-logging", wxTRANSLATE("dumps application log to a file ( enter path )"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_NEEDS_SEPARATOR},
	     {wxCMD_LINE_SWITCH, "cl", "console-logging", wxTRANSLATE("shows application log to the console(if available)"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
	     {wxCMD_LINE_SWITCH, "gl", "gui-logging", wxTRANSLATE("enables application log window"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
	     {wxCMD_LINE_OPTION, "f", "config-file", wxTRANSLATE("override default choice for config-file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_NEEDS_SEPARATOR},
	     {wxCMD_LINE_OPTION, "l", "log-verbosity", wxTRANSLATE("overrides default logging verbosity, can be:\n                                1: critical errors\n                                2: errors\n                                3: warnings (default)\n                                4: messages\n                                5: function trace"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
	     {wxCMD_LINE_SWITCH, "ve", "version", wxTRANSLATE("print version"), wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
	     {wxCMD_LINE_OPTION, "n", "name", wxTRANSLATE("overrides default application name"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
	     {wxCMD_LINE_NONE, NULL, NULL, NULL, wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL} //this is mandatory according to http://docs.wxwidgets.org/stable/wx_wxcmdlineparser.html

	    };

	parser.SetDesc(cmdLineDesc);
	parser.SetSwitchChars(_T("-"));
}

//! @brief parses the command line and sets global app options like log verbosity or log target
bool SpringLobbyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	if (!parser.Parse(true)) {
		m_log_console = parser.Found(_T("console-logging"));
		m_log_window_show = parser.Found(_T("gui-logging"));
		parser.Found(_T("file-logging"), &m_log_file_path);
		m_crash_handle_disable = parser.Found(_T("no-crash-handler"));
		parser.Found(_T("log-verbosity"), &m_log_verbosity);

		// TODO make sure this is called before settings are accessed
		wxString config;
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
		if (parser.Found(_T("help")))
			return false; // not a syntax error, but program should stop if user asked for command line usage

		if (parser.Found(_T("version"))) {
			wxLogMessage(TowxString(getSpringlobbyVersion()).c_str());
			return false;
		}
		return true;
	} else {
		return false;
	}
}


void SpringLobbyApp::OnQuit(wxCommandEvent& /*data*/)
{
	GlobalEventManager::Instance()->Send(GlobalEventManager::OnQuit);
}
