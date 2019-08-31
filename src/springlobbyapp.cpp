/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: SpringLobbyApp
//
#include "springlobbyapp.h"

#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <lslunitsync/unitsync.h>
#include <lslutils/config.h>
#include <lslutils/globalsmanager.h>
#include <wx/choicdlg.h>
#include <wx/cmdline.h>
#include <wx/debugrpt.h>
#include <wx/dirdlg.h>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/fs_zip.h> //filesystem zip handler
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/socket.h>
#include <wx/stdpaths.h>
#include <wx/tooltip.h>
#include <wx/utils.h>
#include <wx/wfstream.h>
#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif
#if wxUSE_UNIX
#include <X11/Xlib.h>
#endif

#include "channel.h"
#include "downloader/lib/src/FileSystem/FileSystem.h"
#include "downloader/prdownloader.h"
#include "gui/controls.h"
#include "gui/customdialogs.h"
#include "gui/iconscollection.h"
#include "gui/mainwindow.h"
#include "gui/notifications/notificationmanager.h"
#include "gui/playback/playbacktab.h"
#include "gui/ui.h"
#include "log.h"
#include "servermanager.h"
#include "settings.h"
#include "stacktrace.h"
#include "sysinfo.h"
#include "utils/conversion.h"
#include "utils/globalevents.h"
#include "utils/platform.h"
#include "utils/slconfig.h"
#include "utils/slpaths.h"
#include "utils/version.h"
#include "utils/wxTranslationHelper.h"

SLCONFIG("/ResetLayout", false, "reset layout on restart");

IMPLEMENT_APP(SpringLobbyApp)

BEGIN_EVENT_TABLE(SpringLobbyApp, wxApp)
END_EVENT_TABLE()

SpringLobbyApp::SpringLobbyApp()
    : quit_called(false)
    , m_translationhelper(NULL)
    , m_log_verbosity(6)
    , m_log_console(true)
    , m_log_window_show(false)
    , m_crash_handle_disable(false)
    , m_appname(GetSpringlobbyName())
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

SLCONFIG("/General/LocalePath", "", "Path to locales");

static wxString getLocalePath()
{
	wxString path = cfg().ReadString("/General/LocalePath");

	if (path.empty()) {
#ifdef __WXMSW__
		path = wxPathOnly(wxStandardPaths::Get().GetExecutablePath()) + wxFileName::GetPathSeparator() + _T("locale");
#else
#if defined(LOCALE_INSTALL_DIR)
		path = _T(LOCALE_INSTALL_DIR);
#else
	// use a dummy name here, we're only interested in the base path
		path = wxStandardPaths::Get().GetLocalizedResourcesDir(_T("noneWH"), wxStandardPaths::ResourceCat_Messages);
		path = path.Left(path.First(_T("noneWH")));
#endif
#endif
		cfg().Write("/General/LocalePath", path);
	}
	return path;
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
#else
	try {
#endif

	const std::string configdir = SlPaths::GetConfigfileDir();
	SlPaths::mkDir(configdir);


	const wxString m_log_file_path = SlPaths::GetLobbyWriteDir() + "springlobby.log";
	//initialize all loggers, we'll use the returned pointer to set correct parent window later
	wxLogWindow* loggerwin = Logger::InitializeLoggingTargets(0, m_log_console, m_log_file_path, m_log_window_show, m_log_verbosity);

	wxLogMessage(_T("%s started"), TowxString(GetSpringlobbyAgent()).c_str());
	wxLogMessage("Config dir: %s", configdir.c_str());

	//this needs to called _before_ mainwindow instance is created
	wxInitAllImageHandlers();
	wxFileSystem::AddHandler(new wxZipFSHandler);
	wxSocketBase::Initialize();


	m_translationhelper = new wxTranslationHelper(GetAppName().Lower(), getLocalePath());

	if (cfg().ReadBool(_T("/ResetLayout"))) {
		wxLogMessage("Resetting Layout...");
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
	wxLogMessage("Configuring Unitsync...");
	// configure unitsync paths before trying to load
	SlPaths::ReconfigureUnitsync();

	sett().Setup(m_translationhelper);

	notificationManager(); //needs to be initialized too

	wxLogMessage("Showing Main Window");
	ui().ShowMainWindow();
	SetTopWindow(&ui().mw());
	ui().mw().SetLogWin(loggerwin);

	//unitsync first load, FIXME move to a thread!
	wxLogMessage("Refreshing Spring Version List...");
	SlPaths::RefreshSpringVersionList();
	if (LSL::usync().ReloadUnitSyncLib()) {
		GlobalEventManager::Instance()->Send(GlobalEventManager::OnUnitsyncReloaded);
	} else {
		wxLogWarning("Couldn't load unitsync");
	}

	wxLogMessage("%s", GetSpringlobbyInfo());

	ui().OnInit();

#if !wxUSE_ON_FATAL_EXCEPTION
} catch (const std::exception& e) {
	wxLogError(_T("Error had happened: %s"), e.what());
}
#endif

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

	IconsCollection::Release();
	ServerManager::Release();
	SetEvtHandlerEnabled(false);
	UiEvents::GetNotificationEventSender().Enable(false);
	LSL::Util::DestroyGlobals();
	GlobalEventManager::Release();

	return 0;
}

//! @brief is called when the app crashes
void SpringLobbyApp::OnFatalException()
{
	wxLogError("Fatal exception!");

	StackTrace stackTracer;
	stackTracer.WalkFromException();
	auto trace = stackTracer.GetStackTrace();

	wxLogError("Stack trace: " + trace);
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
			wxLogMessage(TowxString(GetSpringlobbyVersion()).c_str());
			return false;
		}
		return true;
	} else {
		return false;
	}
}


void SpringLobbyApp::OnQuit(wxCommandEvent& /*data*/)
{
	wxLogInfo("MainWindow::OnClose");
	GlobalEventManager::Instance()->Send(GlobalEventManager::OnQuit);
}
